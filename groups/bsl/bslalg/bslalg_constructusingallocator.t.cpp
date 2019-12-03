// bslalg_constructusingallocator.t.cpp                                      -*-C++-*-
#include <bslalg_constructusingallocator.h>

#include <bslalg_autoscalardestructor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructionutil.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_ispair.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_usesallocatorargt.h>

#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <stddef.h>
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides primitive operations to construct and destroy
// objects, abstracting the fact that the class constructors may or may not
// take an optional allocator argument of type 'bslma::Allocator *'.  These
// primitives allow one to write parameterized code (e.g., containers) in a
// manner that is independent of whether or not the template parameters take
// optional allocators.  In addition, the primitives use the most efficient
// implementation (e.g., bit-wise copy) whenever possible.
//
// The general concerns of this component are the proper detection of traits
// (using 'bslma::Allocator', using bit-wise copy) and the correct selection of
// the implementation.  Some of these concerns are addressed by the compilation
// (detecting the wrong traits will lead to compilation failure) and some
// others are addressed by runtime detection of values after evaluation.  A
// general mechanism used is to construct an object into a buffer previously
// initialized to some garbage value (usually 92).  For bit-wise copy, we use
// a fussy type that will modify its (internal and/or class-static) state upon
// invocation of the copy constructor, but not when copying bit-wise.
//-----------------------------------------------------------------------------
// [ 3] defaultConstruct(T *dst, *a);
// [ 4] copyConstruct(T *dst, const T& src, *a);
// [ 5] moveConstruct(T *dst, T& src, *a);
// [ 6] construct(T *dst, A[1--N]..., *a);
// [ 7] destructiveMove(T *dst, T *src, *a);
// [ 8] swap(T& lhs, T& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
//      Deprecated methods:
// [  ] destruct(T *address);
#endif // BDE_OMIT_INTERNAL_DEPRECATED

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define PP(X) printf(#X " = %p\n", (void*)(X));
                                         // Print pointer identifier and value.

// Pragmas to silence format warnings, should be cleaned up before final commit
// BDE_VERIFY pragma: -AL01  // Strict aliasing concerns should be addressed
// BDE_VERIFY pragma: -CC01  // C style casts
// BDE_VERIFY pragma: -FD01  // Lots of functions need a clear contract
// BDE_VERIFY pragma: -IND01 // Indent issues
// BDE_VERIFY pragma: -IND03 // Text-alignment issues
// BDE_VERIFY pragma: -IND04 // Text-alignment issues

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslmf::MovableRefUtil MovUtl;

const int MOVED_FROM_VAL = 0x01d;

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

                             // =================
                             // class my_ClassDef
                             // =================

struct my_ClassDef {
    // Data members that give my_ClassX size and alignment.  This class is a
    // simple aggregate, use to provide a common data layout to subsequent test
    // types.  There are no semantics associated with any of the members, in
    // particular the allocator pointer is not used directly by this aggregate
    // to allocate storage owned by this class.

    // DATA (exceptionally public, only in test driver)
    int                         d_value;
    int                        *d_data_p;
    bslma::Allocator           *d_allocator_p;
};

// In optimized builds, some compilers will elide some of the operations in the
// destructors of the test classes defined below.  In order to force the
// compiler to retain all of the code in the destructors, we provide the
// following function that can be used to (conditionally) print out the state
// of a 'my_ClassDef' data member.  If the destructor calls this function as
// its last operation, then all values set in the destructor have visible
// side-effects, but non-verbose test runs do not have to be burdened with
// additional output.

static bool forceDestructorCall = false;

void dumpClassDefState(const my_ClassDef& def)
{
    if (forceDestructorCall) {
        printf("%p: %d %p %p\n",
               &def, def.d_value, def.d_data_p, def.d_allocator_p);
    }
}

                             // ===============
                             // class my_Class1
                             // ===============

class my_Class1 {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'my_ClassDef' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer, while
    // the 'd_data_p' pointer is never initialized

 public:

    // DATA
    my_ClassDef d_def;

    // CREATORS
    explicit
    my_Class1(int v = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }
    my_Class1(const my_Class1& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    my_Class1(bslmf::MovableRef<my_Class1> other) {                 // IMPLICIT
        my_Class1& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
    }

    ~my_Class1() {
        ASSERT(d_def.d_value != 91);
        d_def.d_value = 91;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    my_Class1& operator=(const my_Class1& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};
                  // ===============
                  // class my_Class1
                  // ===============a

class my_Class1a {
  // This 'class' is a simple type that does not take allocators.  Its
  // implementation owns a 'my_Class1' aggregate, but uses only the
  // 'd_value' data member, to support the 'value' attribute.  The
  // 'd_allocator_p' pointer is always initialized to a null pointer.
public:
    my_Class1 d_data;

    // CREATORS
    my_Class1a() : d_data() { }

    explicit
    my_Class1a(int v)  : d_data(v) {}

    my_Class1a(const my_Class1& v)
            : d_data(v) {}

    my_Class1a(bslmf::MovableRef<my_Class1> v)
            : d_data(MovUtl::move(v)) {}

    my_Class1a(const my_Class1a& rhs) : d_data(rhs.d_data) {}

    my_Class1a(bslmf::MovableRef<my_Class1a> rhs)                   // IMPLICIT
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    // MANIPULATORS
    my_Class1a& operator=(const my_Class1a& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class1a& operator=(bslmf::MovableRef<my_Class1a> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};

                             // ===============
                             // class my_Class2
                             // ===============

class my_Class2 {
    // This 'class' supports the 'bslma::UsesBslmaAllocator' trait, providing
    // an allocator-aware version of every constructor.  While it holds an
    // allocator and has the expected allocator propagation properties of a
    // 'bslma::Allocator'-aware type, it does not actually allocate any memory.
    // In many ways, this is similar to a 'std::string' object that never grows
    // beyond the small string optimization.  The 'd_data_p' member of the
    // wrapper 'my_ClassDef' implementation type is never initialized, nor
    // used.  A signal value, 'MOVED_FROM_VAL', is used to detect an object in
    // a moved-from state.

 public:

    // DATA
    my_ClassDef d_def;

    // CREATORS
    explicit
    my_Class2(bslma::Allocator *a = 0) {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }
    explicit
    my_Class2(int v, bslma::Allocator *a = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }
    my_Class2(const my_Class2& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    my_Class2(bslmf::MovableRef<my_Class2> other, bslma::Allocator *a = 0) {
                                                                    // IMPLICIT
        my_Class2& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
    }

    my_Class2(const my_Class1& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    my_Class2(bslmf::MovableRef<my_Class1> other, bslma::Allocator *a = 0) {
                                                                    // IMPLICIT
        my_Class1& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = a;
    }

    ~my_Class2() {
        ASSERT(d_def.d_value != 92);
        d_def.d_value = 92;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    my_Class2& operator=(const my_Class2& rhs) {
        d_def.d_value = rhs.d_def.d_value;

        // do not touch allocator!

        return *this;
    }

    my_Class2& operator=(bslmf::MovableRef<my_Class2> rhs) {
        my_Class2& otherRef = MovUtl::access(rhs);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;

        // do not touch allocator!

        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<my_Class2> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                                 // ==========
                                 // my_Class2a
                                 // ==========

class my_Class2a {
    // This 'class' behaves the same as 'my_Class2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.

  public:
    my_Class2 d_data;

    // CREATORS
    my_Class2a() : d_data() { }

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    explicit
    my_Class2a(int v)  : d_data(v) {}

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
        : d_data(v, a) {}

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a,const my_Class2& v)
            : d_data(v, a) {}

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a,
                bslmf::MovableRef<my_Class2> v)
            : d_data(MovUtl::move(v), a) {}


    my_Class2a(const my_Class2a& rhs) : d_data(rhs.d_data) {}

    my_Class2a(bsl::allocator_arg_t  ,
               bslma::Allocator     *a,
               const my_Class2a&     rhs)
        : d_data(rhs.d_data, a) {}

    my_Class2a(bslmf::MovableRef<my_Class2a> rhs)                   // IMPLICIT
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    my_Class2a(bsl::allocator_arg_t,
               bslma::Allocator              *a,
               bslmf::MovableRef<my_Class2a>  rhs)
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a) {}

    // MANIPULATORS
    my_Class2a& operator=(const my_Class2a& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class2a& operator=(bslmf::MovableRef<my_Class2a> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};

// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<my_Class2a> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<my_Class2a> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

                             // ===============
                             // class my_Class3
                             // ===============

class my_Class3 {
    // This 'class' takes allocators similarly to 'my_Class2', but does not
    // have an explicit move constructor (move calls the corresponding copy
    // operation).

     public:

    // DATA
    my_ClassDef d_def;

    // CREATORS
    explicit
    my_Class3(bslma::Allocator *a = 0) {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }
    explicit
    my_Class3(int v, bslma::Allocator *a = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }
    my_Class3(const my_Class3& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    my_Class3(const my_Class2& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    ~my_Class3() {
        ASSERT(d_def.d_value != 93);
        d_def.d_value = 93;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    my_Class3& operator=(const my_Class3& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        // do not touch allocator!
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<my_Class3> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

                              // ===============
                              // macros TEST_OP*
                              // ===============

#define TEST_OP(typeNum, op, expVal, expAlloc) {                              \
    typedef my_Class ## typeNum Type;                                         \
     static const int EXP_VAL = (expVal);                                      \
     bslma::Allocator *const EXP_ALLOC = (expAlloc);                           \
     pre(&rawBuf);                                                             \
     Type dest =  BloombergLP::bslalg::AllocatorUtil<Type>:: op                \
     post(&rawBuf);                                                            \
     ASSERT(EXP_VAL == rawBuf.d_value);                                        \
     ASSERT(EXP_ALLOC == rawBuf.d_allocator_p);                                \
   }

//bslmf::MovableRefUtil::move(src)

    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_ClassN' (where 'N' stands for the specified 'typeNum'), and verifies
    // that the 'd_value' and 'd_allocator_p' members store the specified
    // 'expVal' and 'expAlloc' values after 'op' has been evaluated.

#define TEST_PAIR(op, expVal0, expA0, expVal1, expA1) {                       \
    static const int EXP_VAL0 = (expVal0);                                    \
    bslma::Allocator *const EXP_ALLOC0 = (expA0);                             \
    static const int EXP_VAL1 = (expVal1);                                    \
    bslma::Allocator *const EXP_ALLOC1 = (expA1);                             \
    my_ClassDef rawBuf[2];                                                    \
    memset(rawBuf, 92, sizeof(rawBuf));                                       \
    Type *objPtr = (Type*) rawBuf;                                            \
    pre(rawBuf);                                                              \
    Obj:: op ;                                                                \
    post(rawBuf);                                                             \
    ASSERT(EXP_VAL0 == rawBuf[0].d_value);                                    \
    ASSERT(EXP_ALLOC0 == rawBuf[0].d_allocator_p);                            \
    ASSERT(EXP_VAL1 == rawBuf[1].d_value);                                    \
    ASSERT(EXP_ALLOC1 == rawBuf[1].d_allocator_p);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of the locally
    // defined 'Type', and verifies that the 'd_value' and 'd_allocator_p'
    // members store the specified 'expVal0' and 'expAlloc0' values for the
    // first member and 'expVal0' and 'expAlloc0' values for the second member
    // after 'op' has been evaluated.

#define TEST_PAIROP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) { \
    typedef my_Pair_ ## typeNum0 ## _ ## typeNum1 Type;                       \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_Pair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRAOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1)  \
  {                                                                           \
    typedef my_PairA_ ## typeNum0 ## _ ## typeNum1 Type;                      \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_Pair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRAAOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) \
  {                                                                           \
    typedef my_PairAA_ ## typeNum0 ## _ ## typeNum1 Type;                     \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_PairAA_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

#define TEST_PAIRBBOP(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) \
  {                                                                           \
    typedef my_PairBB_ ## typeNum0 ## _ ## typeNum1 Type;                     \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_PairBB_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1').  See the 'TEST_PAIR' macro above for details.

                              // ===============
                              // macros TEST_MV*
                              // ===============

#define TEST_MV(typeNum, op, expVal, expAlloc) {                              \
    bslma::TestAllocator fromA;                                               \
    my_Class ## typeNum fromObj(expVal);                                      \
    TEST_OP(typeNum, op, expVal, expAlloc);                                   \
    ASSERT(isMovedFrom(fromObj));                                             \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_ClassN' (where 'N' stands for the specified 'typeNum') and the
    // modifiable object 'fromObj' of type 'my_ClassN' initialized to
    // 'expVal', and verifies that the 'd_value' and 'd_allocator_p' members
    // store the specified 'expVal' and 'expAlloc' values and 'fromObj' is in
    // a moved-from state after 'op' has been evaluated.

#define TEST_PAIRMV(typeNum0, typeNum1, op, expVal0, expA0, expVal1, expA1) { \
    typedef my_PairA_ ## typeNum0 ## _ ## typeNum1 Type;                      \
    Type fromObj(expVal0, expVal1);                                           \
    TEST_PAIR(op, expVal0, expA0, expVal1, expA1);                            \
    ASSERT(isMovedFrom(fromObj.first));                                       \
  }
    // This macro evaluates the specified 'op' expression in the namespace
    // under test, involving the address 'objPtr' of an object of type
    // 'my_Pair_N0_N1' (where 'N0' and 'N1' stand for the specified 'typeNum0
    // and 'typeNum1') and a modifiable object 'fromObj' of type
    // 'my_Pair_N0_N1' initialized to '{ expVal0, expVal1 }, and verifies that
    // 'fromObj.first' and 'fromObj.second' are in a moved-from state after
    // 'op' has been evaluated.  See the 'TEST_PAIR' macro above for details.

                         // ==========================
                         // debug breakpoints pre/post
                         // ==========================

void pre(const my_ClassDef* p)
    // Do nothing.  This function can be taken advantage of to debug the above
    // macros by setting a breakpoint to examine state prior to executing the
    // main operation under test.
{
    (void) p;  // remove unused variable warning
}

void post(const my_ClassDef* p)
    // Do nothing.  This function can be taken advantage of to debug the above
    // macros by setting a breakpoint to examine state after executing the
    // main operation under test.
{
    (void) p;  // remove unused variable warning
}

                       // ======================
                       // class ConstructTestArg
                       // ======================

template <int ID>
class ConstructTestArg {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters to 'construct' due to the fact that
    // 'ConstructTestArg<ID1>' is a different type than 'ConstructTestArg<ID2>'
    // if 'ID1 != ID2'.  This class does not take an allocator.

  public:
    // PUBLIC DATA FOR TEST DRIVER ONLY
    const int d_value;

    // CREATORS
    ConstructTestArg(int value = -1);                               // IMPLICIT
        // Create an object having the specified 'value'.
};

// CREATORS
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(int value)
    : d_value(value)
{
}

                       // ==============================
                       // class ConstructTestTypeNoAlloc
                       // ==============================

class ConstructTestTypeNoAlloc {
    // This 'struct' provides a test class capable of holding up to 14
    // parameters of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeNoAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  A 'ConstructTestTypeNoAlloc' can be invoked with up
    // to 14 parameters, via member functions 'testFunc[1--14]'.  These
    // functions are also called by the overloaded member 'operator()' of the
    // same signatures, and similar global functions 'testFunc[1--14]'.  All
    // invocations support the above 'ConstructTestSlotsNoAlloc' mechanism.
    //
    // This 'struct' intentionally does *not* take an allocator.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    Arg1  d_a1;
    Arg2  d_a2;
    Arg3  d_a3;
    Arg4  d_a4;
    Arg5  d_a5;
    Arg6  d_a6;
    Arg7  d_a7;
    Arg8  d_a8;
    Arg9  d_a9;
    Arg10 d_a10;
    Arg11 d_a11;
    Arg12 d_a12;
    Arg13 d_a13;
    Arg14 d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    explicit
    ConstructTestTypeNoAlloc(
                Arg1  a1  = N1, Arg2  a2  = N1, Arg3  a3  = N1,
                Arg4  a4  = N1, Arg5  a5  = N1, Arg6  a6  = N1, Arg7  a7  = N1,
                Arg8  a8  = N1, Arg9  a9  = N1, Arg10 a10 = N1, Arg11 a11 = N1,
                Arg12 a12 = N1, Arg13 a13 = N1, Arg14 a14 = N1)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6), d_a7(a7)
        , d_a8(a8), d_a9(a9), d_a10(a10), d_a11(a11), d_a12(a12), d_a13(a13)
        , d_a14(a14) {}
};

// FREE OPERATORS
bool operator==(const ConstructTestTypeNoAlloc& lhs,
                const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_a1.d_value  == rhs.d_a1.d_value &&
           lhs.d_a2.d_value  == rhs.d_a2.d_value &&
           lhs.d_a3.d_value  == rhs.d_a3.d_value &&
           lhs.d_a4.d_value  == rhs.d_a4.d_value &&
           lhs.d_a5.d_value  == rhs.d_a5.d_value &&
           lhs.d_a6.d_value  == rhs.d_a6.d_value &&
           lhs.d_a7.d_value  == rhs.d_a7.d_value &&
           lhs.d_a8.d_value  == rhs.d_a8.d_value &&
           lhs.d_a9.d_value  == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}

                       // =============================
                       // class ConstructTestTypeAlloc
                       // =============================

class ConstructTestTypeAlloc {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  This class intentionally *does* take an allocator.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    bslma::Allocator *d_allocator;
    Arg1  d_a1;
    Arg2  d_a2;
    Arg3  d_a3;
    Arg4  d_a4;
    Arg5  d_a5;
    Arg6  d_a6;
    Arg7  d_a7;
    Arg8  d_a8;
    Arg9  d_a9;
    Arg10 d_a10;
    Arg11 d_a11;
    Arg12 d_a12;
    Arg13 d_a13;
    Arg14 d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    explicit
    ConstructTestTypeAlloc(bslma::Allocator *allocator = 0)
        : d_allocator(allocator) {}
    ConstructTestTypeAlloc(const ConstructTestTypeAlloc&  other,
                           bslma::Allocator              *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
        , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
        , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
        , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
        , d_a13(other.d_a13), d_a14(other.d_a14) {}
    explicit
    ConstructTestTypeAlloc(Arg1  a1, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6)
        , d_a7(a7) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6)
        , d_a7(a7), d_a8(a8) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5), d_a6(a6)
        , d_a7(a7), d_a8(a8), d_a9(a9) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4), d_a5 (a5), d_a6 (a6)
        , d_a7 (a7), d_a8 (a8), d_a9 (a9)
        , d_a10(a10) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4), d_a5 (a5), d_a6 (a6)
        , d_a7 (a7), d_a8 (a8), d_a9 (a9)
        , d_a10(a10), d_a11(a11) {}
    ConstructTestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7), d_a8 (a8), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        {}
    ConstructTestTypeAlloc(Arg1  a1,  Arg2  a2,  Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6,  Arg7  a7,  Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12, Arg13 a13,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7), d_a8 (a8), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13) {}
    ConstructTestTypeAlloc(Arg1  a1,  Arg2  a2,  Arg3  a3, Arg4  a4, Arg5  a5,
                           Arg6  a6,  Arg7  a7,  Arg8  a8, Arg9  a9, Arg10 a10,
                           Arg11 a11, Arg12 a12, Arg13 a13, Arg14 a14,
                           bslma::Allocator *allocator = 0)
        : d_allocator(allocator)
        , d_a1 (a1), d_a2 (a2), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7), d_a8 (a8), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13), d_a14(a14) {}
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAlloc> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace

// FREE OPERATORS
bool operator==(const ConstructTestTypeAlloc& lhs,
                const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_a1.d_value  == rhs.d_a1.d_value &&
           lhs.d_a2.d_value  == rhs.d_a2.d_value &&
           lhs.d_a3.d_value  == rhs.d_a3.d_value &&
           lhs.d_a4.d_value  == rhs.d_a4.d_value &&
           lhs.d_a5.d_value  == rhs.d_a5.d_value &&
           lhs.d_a6.d_value  == rhs.d_a6.d_value &&
           lhs.d_a7.d_value  == rhs.d_a7.d_value &&
           lhs.d_a8.d_value  == rhs.d_a8.d_value &&
           lhs.d_a9.d_value  == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}

                       // ================================
                       // class ConstructTestTypeAllocArgT
                       // ================================

class ConstructTestTypeAllocArgT {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAllocArgT' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  This class takes an allocator using the
    // 'allocator_arg_t' protocol.

    // PRIVATE TYPES
    typedef ConstructTestArg<1>  Arg1;
    typedef ConstructTestArg<2>  Arg2;
    typedef ConstructTestArg<3>  Arg3;
    typedef ConstructTestArg<4>  Arg4;
    typedef ConstructTestArg<5>  Arg5;
    typedef ConstructTestArg<6>  Arg6;
    typedef ConstructTestArg<7>  Arg7;
    typedef ConstructTestArg<8>  Arg8;
    typedef ConstructTestArg<9>  Arg9;
    typedef ConstructTestArg<10> Arg10;
    typedef ConstructTestArg<11> Arg11;
    typedef ConstructTestArg<12> Arg12;
    typedef ConstructTestArg<13> Arg13;
    typedef ConstructTestArg<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    bslma::Allocator *d_allocator;
    Arg1  d_a1;
    Arg2  d_a2;
    Arg3  d_a3;
    Arg4  d_a4;
    Arg5  d_a5;
    Arg6  d_a6;
    Arg7  d_a7;
    Arg8  d_a8;
    Arg9  d_a9;
    Arg10 d_a10;
    Arg11 d_a11;
    Arg12 d_a12;
    Arg13 d_a13;
    Arg14 d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    explicit
    ConstructTestTypeAllocArgT(Arg1  a1  = N1, Arg2  a2 = N1,  Arg3  a3  = N1,
                               Arg4  a4  = N1, Arg5  a5 = N1,  Arg6  a6  = N1,
                               Arg7  a7  = N1, Arg8  a8 = N1,  Arg9  a9  = N1,
                               Arg10 a10 = N1, Arg11 a11 = N1, Arg12 a12 = N1,
                               Arg13 a13 = N1, Arg14 a14 = N1)
        : d_allocator(0)
        , d_a1 (a1 ), d_a2 (a2 ), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7 ), d_a8 (a8 ), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13), d_a14(a14) {}

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               Arg1  a1  = N1, Arg2  a2 = N1,  Arg3  a3  = N1,
                               Arg4  a4  = N1, Arg5  a5 = N1,  Arg6  a6  = N1,
                               Arg7  a7  = N1, Arg8  a8 = N1,  Arg9  a9  = N1,
                               Arg10 a10 = N1, Arg11 a11 = N1, Arg12 a12 = N1,
                               Arg13 a13 = N1, Arg14 a14 = N1)
        : d_allocator(alloc)
        , d_a1 (a1 ), d_a2 (a2 ), d_a3 (a3), d_a4 (a4 ), d_a5 (a5 ), d_a6 (a6 )
        , d_a7 (a7 ), d_a8 (a8 ), d_a9 (a9), d_a10(a10), d_a11(a11), d_a12(a12)
        , d_a13(a13), d_a14(a14) {}
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAllocArgT> : bsl::true_type { };

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<ConstructTestTypeAllocArgT> : bsl::true_type { };

}  // close namespace bslmf
}  // close enterprise namespace

// FREE OPERATORS
bool operator==(const ConstructTestTypeAllocArgT& lhs,
                const ConstructTestTypeAllocArgT& rhs)
{
    return lhs.d_a1.d_value  == rhs.d_a1.d_value &&
           lhs.d_a2.d_value  == rhs.d_a2.d_value &&
           lhs.d_a3.d_value  == rhs.d_a3.d_value &&
           lhs.d_a4.d_value  == rhs.d_a4.d_value &&
           lhs.d_a5.d_value  == rhs.d_a5.d_value &&
           lhs.d_a6.d_value  == rhs.d_a6.d_value &&
           lhs.d_a7.d_value  == rhs.d_a7.d_value &&
           lhs.d_a8.d_value  == rhs.d_a8.d_value &&
           lhs.d_a9.d_value  == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}

                           // ======================
                           // macros TEST_CONSTRUCT*
                           // ======================

#define TEST_CONSTRUCT(op, expArgs)                                           \
  {                                                                           \
    ConstructTestTypeNoAlloc EXP expArgs ;                                    \
    ConstructTestTypeNoAlloc  dest =                                          \
            BloombergLP::bslalg::AllocatorUtil<ConstructTestTypeNoAlloc>:: op;\
    ConstructTestTypeNoAlloc&  mX     =  dest;                                \
    const ConstructTestTypeNoAlloc& X =  mX;                                  \
    ASSERT(EXP == X);                                                         \
  }

#define TEST_CONSTRUCTA(op, expArgs, alloc)                                   \
  {                                                                           \
    /* Expects allocator at end of argument list */                           \
    ConstructTestTypeAlloc EXP expArgs ;                                      \
    ConstructTestTypeAlloc  dest =                                            \
            BloombergLP::bslalg::AllocatorUtil<ConstructTestTypeAlloc>:: op;  \
    const ConstructTestTypeAlloc& X = dest;                                  \
    ASSERT(EXP == X);                                                         \
    ASSERT(alloc == X.d_allocator);                                           \
  }                                                                           \
  {                                                                           \
    /* Expects allocator after 'allocator_arg_t' tag */                       \
    ConstructTestTypeAllocArgT EXP expArgs ;                                  \
    ConstructTestTypeAllocArgT  dest =                                            \
        BloombergLP::bslalg::AllocatorUtil<ConstructTestTypeAllocArgT>:: op;   \
    const ConstructTestTypeAllocArgT& X = dest;                            \
    ASSERT(EXP == X);                                                         \
    ASSERT(alloc == X.d_allocator);                                           \
  }

//=============================================================================
//               GLOBAL TYPEDEFS AND CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------


const my_Class1     V1(1);
const my_Class2     V2(2);
const my_Class2a    V2A(0x2a);
const my_Class3     V3(3);

ConstructTestArg<1>    VA1(1);
ConstructTestArg<2>    VA2(2);
ConstructTestArg<3>    VA3(3);
ConstructTestArg<4>    VA4(4);
ConstructTestArg<5>    VA5(5);
ConstructTestArg<6>    VA6(6);
ConstructTestArg<7>    VA7(7);
ConstructTestArg<8>    VA8(8);
ConstructTestArg<9>    VA9(9);
ConstructTestArg<10>   VA10(10);
ConstructTestArg<11>   VA11(11);
ConstructTestArg<12>   VA12(12);
ConstructTestArg<13>   VA13(13);
ConstructTestArg<14>   VA14(14);

template <class TYPE>
inline bool isMovedFrom(const TYPE& x)
    // Return 'true' if the specified 'x' is in a moved-from state.  This
    // template is for classes with explicit move constructors whose 'value()'
    // member returns 'MOVED_FROM_VAL' if the class is in a moved-from state.
    // Classes without explicit move constructors should overload this function
    // to return true if 'x' holds the expected initial value.
{
    // For classes with explicit move constructors, the moved-from object holds
    // 'MOVED_FROM_VAL'.
    return x.value() == MOVED_FROM_VAL;
}

inline bool isMovedFrom(const my_Class3& x)
    // Return 'true' if the specified 'x' is in the moved-from state, which is
    // assumed to be the same as its initial value, which is, in turn, assumed
    // to be 'V3'.
{
    return x.value() == V3.value();
}

template <typename TYPE>
TYPE createTemporaryCopy(BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) original,
    BloombergLP::bslma::Allocator   *allocator)
{
  TYPE temp = BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
      BSLS_COMPILERFEATURES_FORWARD(TYPE,original), allocator);
  return temp;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    forceDestructorCall = veryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.

      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'returning constructed object by value'
        //
        // Concerns: In simple return by value functions, the RVO will not kick in
        //      and the created object will be copied/moved.
        //
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'returning constructed object by value '"
                            "\n=========================\n");

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        bslma::TestAllocator otherAllocator(veryVeryVeryVerbose);

        if (verbose) printf("Value and allocator testing.\n");

        // my_Class #  Operation                            Val Alloc
        // ==========  ==================================== === =====
        {
            my_Class1 dest = createTemporaryCopy<my_Class1>(V1, TA);
            ASSERTV(dest.d_def.d_value, 1 == dest.d_def.d_value);
            ASSERT(0  == dest.d_def.d_allocator_p);
        }
        {
            my_Class2 dest = createTemporaryCopy<my_Class2>(V2, TA);
            ASSERTV(dest.d_def.d_value, 2 == dest.d_def.d_value);
            ASSERT(TA  == dest.d_def.d_allocator_p);
        }
        {
            my_Class2a dest = createTemporaryCopy<my_Class2a>(V2A, TA);
            ASSERTV(dest.d_data.d_def.d_value, 42 == dest.d_data.d_def.d_value);
            ASSERT(TA  == dest.d_data.d_def.d_allocator_p);
        }
        {
            my_Class3 dest = createTemporaryCopy<my_Class3>(V3, TA);
            ASSERTV(dest.d_def.d_value, 3 == dest.d_def.d_value);
            ASSERT(TA  == dest.d_def.d_allocator_p);
        }
      } break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'construct'
        //
        // Concerns:
        //  o That arguments are forwarded in the proper order and
        //    number (typos could easily make a10 become the 11th argument to
        //    the constructor).
        //  o That allocators are forwarded appropriately according to the
        //    traits and to the type (bslma::Allocator* or void*).
        //
        // Plan: Construct an object using construct, and verify
        //   that the value and allocator is as expected.  In order to
        //   ascertain the proper forwarding of the arguments, use different
        //   types and values.
        //
        // Testing:
        //   construct( A[1--N]..., *a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'construct'"
                            "\n===================\n");

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;

        if (verbose) printf("TEST_CONSTRUCT (without allocators).\n");

        // OP  = construct(&ConstructTestArg, VA[1--N], TA)
        // EXP = ConstructTestArg(VA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCT(construct(TA),                            // OP
                       /* no ctor arg list */                            // EXP
                      );

        TEST_CONSTRUCT(construct(VA1, TA),                       // OP
                       (VA1)                                             // EXP
                      );

#if false// no VA arguments yet
        TEST_CONSTRUCT(construct( VA1, VA2, TA),                  // OP
                       (VA1, VA2)                                        // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, TA),             // OP
                       (VA1, VA2, VA3)                                   // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, TA),        // OP
                       (VA1, VA2, VA3, VA4)                              // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         TA),                            // OP
                       (VA1, VA2, VA3, VA4, VA5)                         // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, TA),                       // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6)                    // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, TA),                  // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7)               // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, TA),             // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8)          // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, TA),        // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9)                                             // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         TA),                            // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7,
                        VA8, VA9, VA10)                                  // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, TA),                      // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11)                                 // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, TA),                // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11, VA12)                           // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, TA),          // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11, VA12, VA13)                     // EXP
                      );

        TEST_CONSTRUCT(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, VA14, TA),    // OP
                       (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                        VA9, VA10, VA11, VA12, VA13, VA14)               // EXP
                      );
#endif
        if (verbose) printf("TEST_CONSTRUCTA (with bslma::Allocator*).\n");

        // OP  = construct(&ConstructTestArg, VA[1--N], TA)
        // EXP = ConstructTestArg(VA[1--N])
        // ---   -------------------------------------------------
        TEST_CONSTRUCTA(construct( TA),                         // OP
                        /* no ctor arg list */,                        // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, TA),                    // OP
                        (VA1),                                         // EXP
                        TA);                                           // ALLOC
#if false// no VA argument version yet
        TEST_CONSTRUCTA(construct( VA1, VA2, TA),               // OP
                        (VA1, VA2),                                    // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, TA),          // OP
                        (VA1, VA2, VA3),                               // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, TA),     // OP
                        (VA1, VA2, VA3, VA4),                          // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5, TA),// OP
                        (VA1, VA2, VA3, VA4, VA5),                     // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, TA),                     // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6),                // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                          VA6, VA7, TA),                // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7),            // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, TA),           // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8),      // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, TA),      // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9),                                         // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         TA),                          // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7,
                         VA8, VA9, VA10),                              // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, TA),                    // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11),                             // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, TA),              // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12),                       // EXP
                        TA);                                           // ALLOC

        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, TA),        // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13),                 // EXP
                        TA);                                           // ALLOC
        TEST_CONSTRUCTA(construct( VA1, VA2, VA3, VA4, VA5,
                                         VA6, VA7, VA8, VA9, VA10,
                                         VA11, VA12, VA13, VA14, TA),  // OP
                        (VA1, VA2, VA3, VA4, VA5, VA6, VA7, VA8,
                         VA9, VA10, VA11, VA12, VA13, VA14),           // EXP
                        TA);                                           // ALLOC
#endif


      } break;
      case 8: {
      // --------------------------------------------------------------------
      // TESTING 'move Construct using bslmf::MovableRefUtil::move' from
      //    a different type
      //
      // Concerns:
      //   o That the move construct properly forwards the allocator
      //
      //   o That the move construct invokes a move constructor when appropriate
      //
      // Plan: ...
      //
      // Testing:
      //   construct (bslmf::MovableRef<TARGET_TYPE> original,
      //            bslma::Allocator  *allocator)
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'move construct from a different type'"
                          "\n=============================================\n");

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const TA = &testAllocator;
        bslma::TestAllocator otherAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator *const OA = &otherAllocator;


        {
            my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                            V1, TA);
            my_Class1a dest =  BloombergLP::bslalg::AllocatorUtil<my_Class1a>::construct(
                  bslmf::MovableRefUtil::move(src), src.d_def.d_allocator_p);
            ASSERTV(src.d_def.d_value, MOVED_FROM_VAL == src.d_def.d_value);
            ASSERTV(dest.d_data.d_def.d_value, 1== dest.d_data.d_def.d_value);
            ASSERT(0 == dest.d_data.d_def.d_allocator_p);
        }
        {
            my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                  V1, TA);
            my_Class2 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                  bslmf::MovableRefUtil::move(src), OA);

            ASSERTV(src.d_def.d_value, MOVED_FROM_VAL == src.d_def.d_value);
            ASSERTV(dest.d_def.d_value, 1 == dest.d_def.d_value);
            ASSERT(OA == dest.d_def.d_allocator_p);
        }
        {
            my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                  V2, TA);
            my_Class2a dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                  bslmf::MovableRefUtil::move(src), OA);
            ASSERTV(src.d_def.d_value, MOVED_FROM_VAL == src.d_def.d_value);
            ASSERTV(dest.d_data.d_def.d_value, 2 == dest.d_data.d_def.d_value);
            ASSERT(OA == dest.d_data.d_def.d_allocator_p);
        }
        {
            my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                  V2, TA);
            my_Class3 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                  bslmf::MovableRefUtil::move(src), OA);
            ASSERTV(dest.d_def.d_value, 2== dest.d_def.d_value);
            ASSERT(OA == dest.d_def.d_allocator_p);
        }
     } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'move Construct using bslmf::MovableRefUtil::move'
        //
        // Concerns:
        //   o That the move construct properly forwards the allocator
        //
        //   o That the move construct invokes a move constructor when appropriate
        //
        // Plan: ...
        //
        // Testing:
        //   construct (bslmf::MovableRef<TARGET_TYPE> original,
        //            bslma::Allocator  *allocator)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'move construct'"
                            "\n=========================\n");

          bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
          bslma::TestAllocator *const TA = &testAllocator;
          bslma::TestAllocator otherAllocator(veryVeryVeryVerbose);
          bslma::TestAllocator *const OA = &otherAllocator;


          {
              my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                              V1, TA);
              my_Class1 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                    bslmf::MovableRefUtil::move(src), src.d_def.d_allocator_p);
              ASSERTV(src.d_def.d_value, MOVED_FROM_VAL == src.d_def.d_value);
              ASSERTV(dest.d_def.d_value, 1== dest.d_def.d_value);
              ASSERT(0 == dest.d_def.d_allocator_p);
          }
          {
              my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                    V2, TA);
              my_Class2 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                    bslmf::MovableRefUtil::move(src), src.d_def.d_allocator_p);

              ASSERTV(src.d_def.d_value, MOVED_FROM_VAL == src.d_def.d_value);
              ASSERTV(dest.d_def.d_value, 2 == dest.d_def.d_value);
              ASSERT(TA == dest.d_def.d_allocator_p);
          }
          {
              my_Class2a src = BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                    V2A, TA);
              my_Class2a dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                    bslmf::MovableRefUtil::move(src), src.d_data.d_def.d_allocator_p);
              ASSERTV(src.d_data.d_def.d_value, MOVED_FROM_VAL == src.d_data.d_def.d_value);
              ASSERTV(dest.d_data.d_def.d_value, 42 == dest.d_data.d_def.d_value);
              ASSERT(TA == dest.d_data.d_def.d_allocator_p);
          }
          {
              my_Class3 src = BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                    V3, TA);
              my_Class3 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                    bslmf::MovableRefUtil::move(src), src.d_def.d_allocator_p);
              ASSERTV(dest.d_def.d_value, 3== dest.d_def.d_value);
              ASSERT(TA == dest.d_def.d_allocator_p);
          }
          {
              my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                  V1, TA);
              my_Class1 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                  bslmf::MovableRefUtil::move(src), OA);
              ASSERTV(src.d_def.d_value, MOVED_FROM_VAL == src.d_def.d_value);
              ASSERTV(dest.d_def.d_value, 1 == dest.d_def.d_value);
          }
          {
              my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                  V2, TA);
              my_Class2 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                  bslmf::MovableRefUtil::move(src), OA);
              ASSERTV(src.d_def.d_value, MOVED_FROM_VAL == src.d_def.d_value);
              ASSERTV(dest.d_def.d_value, 2== dest.d_def.d_value);
              ASSERT(OA == dest.d_def.d_allocator_p);
          }
          {
              my_Class2a src = BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                    V2A, TA);
              my_Class2a dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                    bslmf::MovableRefUtil::move(src), OA);
              ASSERTV(src.d_data.d_def.d_value, MOVED_FROM_VAL == src.d_data.d_def.d_value);
              ASSERTV(dest.d_data.d_def.d_value, 42== dest.d_data.d_def.d_value);
              ASSERT(OA == dest.d_data.d_def.d_allocator_p);
          }
          {
              my_Class3 src = BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                    V3, TA);
              my_Class3 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                    bslmf::MovableRefUtil::move(src), OA);
              ASSERTV(src.d_def.d_value, 3 == src.d_def.d_value);
              ASSERTV(dest.d_def.d_value, 3== dest.d_def.d_value);
              ASSERT(OA == dest.d_def.d_allocator_p);
          }

      } break;
      case 6: {
         // --------------------------------------------------------------------
         // TESTING 'move Construct with a temporary of a different type
         //
         // Concerns:
         //   o That the move construct properly forwards the allocator
         //
         //   o That the move construct invokes a move constructor when appropriate //todo
         //
         // Plan: ...
         //
         // Testing:
         //   construct (BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original
         //                 bslma::Allocator  *allocator)
         // --------------------------------------------------------------------

         if (verbose) printf("\nTESTING 'move construct'"
                             "\n=========================\n");

           bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const TA = &testAllocator;
           bslma::TestAllocator otherAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const OA = &otherAllocator;


         {
             my_Class1a dest =  BloombergLP::bslalg::AllocatorUtil<my_Class1a>::construct(
                 createTemporaryCopy<my_Class1>(V1, TA), OA);

             ASSERTV(dest.d_data.d_def.d_value, 1== dest.d_data.d_def.d_value);
         }
         {
              my_Class2 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                 createTemporaryCopy<my_Class1>(V1, TA), OA);

             ASSERTV(dest.d_def.d_value, 1== dest.d_def.d_value);
             ASSERT(OA == dest.d_def.d_allocator_p);
         }
         {
             my_Class2a dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                 createTemporaryCopy<my_Class2>(V2, TA), OA);

             ASSERTV(dest.d_data.d_def.d_value, 2== dest.d_data.d_def.d_value);
             ASSERT(OA == dest.d_data.d_def.d_allocator_p);
         }
         {
             my_Class3 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                 createTemporaryCopy<my_Class2>(V2, TA), OA);

             ASSERTV(dest.d_def.d_value, 2== dest.d_def.d_value);
             ASSERT(OA == dest.d_def.d_allocator_p);
         }
       } break;
      case 5: {
         // --------------------------------------------------------------------
         // TESTING 'move Construct with a temporary
         //
         // Concerns:
         //   o That the move construct properly forwards the allocator
         //
         //   o That the move construct invokes a move constructor when appropriate
         //
         // Plan: ...
         //
         // Testing:
         //   construct (const TARGET_TYPE&                                 original,
         //              bslma::Allocator   *allocator) // in C++03
         //   construct (bslmf::MovableRef<TARGET_TYPE> original,
         //                 bslma::Allocator  *allocator) // In C++11 onwards
         // --------------------------------------------------------------------

         if (verbose) printf("\nTESTING 'move construct'"
                             "\n=========================\n");

           bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const TA = &testAllocator;
           bslma::TestAllocator otherAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const OA = &otherAllocator;


         {
             my_Class1 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                 createTemporaryCopy<my_Class1>(V1, TA), OA);

             ASSERTV(dest.d_def.d_value, 1== dest.d_def.d_value);
         }
         {
              my_Class2 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                 createTemporaryCopy<my_Class2>(V2, TA), OA);

             ASSERTV(dest.d_def.d_value, 2== dest.d_def.d_value);
             ASSERT(OA == dest.d_def.d_allocator_p);
         }
         {
             my_Class2a dest =  BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                 createTemporaryCopy<my_Class2a>(V2A, TA), OA);

             ASSERTV(dest.d_data.d_def.d_value, 42== dest.d_data.d_def.d_value);
             ASSERT(OA == dest.d_data.d_def.d_allocator_p);
         }
         {
             my_Class3 dest =  BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                 createTemporaryCopy<my_Class3>(V3, TA), OA);

             ASSERTV(dest.d_def.d_value, 3== dest.d_def.d_value);
             ASSERT(OA == dest.d_def.d_allocator_p);
         }
       } break;
      case 4: {
           // --------------------------------------------------------------------
           // TESTING 'copy construct from a different type'
           //
           // Concerns:
           //   o That the copy constructor properly forwards the allocator
           //     when appropriate.
           //
           // Plan: Construct an object of a type which can be used to construct a
           //   an object of the tested type. Create a new object of the tested type
           //   using construct facility by passing the original object and an
           //   allocator. Check the value and the allocator of the newly created
           //   object are correct. Check the original objects hasn't changed.
           //
           //
           // Testing:
           //   construct (BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
           //              bslma::Allocator*)
           // --------------------------------------------------------------------

           if (verbose) printf("\nTESTING 'copy construct from a different type'"
                               "\n=============================================\n");

           bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const TA = &testAllocator;
           bslma::TestAllocator otherAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const OA = &otherAllocator;
           {
               my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                               V1, TA);
               my_Class1a dest = BloombergLP::bslalg::AllocatorUtil<my_Class1a>::construct(
                               src, OA);

               ASSERTV(src.d_def.d_value, 1 == src.d_def.d_value);
               ASSERTV(dest.d_data.d_def.d_value, 1 == dest.d_data.d_def.d_value);
               ASSERT(0 == dest.d_data.d_def.d_allocator_p);
           }
           {
               my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                       V1, 0);
               my_Class2 dest = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                       src, TA);

               ASSERTV(src.d_def.d_value, 1 == src.d_def.d_value);
               ASSERT(0 == src.d_def.d_allocator_p);
               ASSERTV(dest.d_def.d_value, 1 == dest.d_def.d_value);
               ASSERT(TA == dest.d_def.d_allocator_p);
           }
           {
                my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                        V2, TA);
                my_Class2a dest = BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                        src, OA);

                ASSERTV(src.d_def.d_value, 2 == src.d_def.d_value);
                ASSERT(TA == src.d_def.d_allocator_p);
                ASSERTV(dest.d_data.d_def.d_value, 2 == dest.d_data.d_def.d_value);
                ASSERT(OA == dest.d_data.d_def.d_allocator_p);
            }
           {
               my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                       V2, TA);
               my_Class3 dest = BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(
                       src, OA);

               ASSERTV(src.d_def.d_value, 2 == src.d_def.d_value);
               ASSERT(TA == src.d_def.d_allocator_p);
               ASSERTV(dest.d_def.d_value, 2 == dest.d_def.d_value);
               ASSERT(OA == dest.d_def.d_allocator_p);
           }

       } break;
       case 3: {
           // --------------------------------------------------------------------
           // TESTING 'copyConstruct'
           //
           // Concerns:
           //   o That the copy constructor properly forwards the allocator
           //     when appropriate.
           //
           // Plan: Construct an object of the tested type. Create a new object
           //   using construct facility by passing the original object and the same
           //   allocator to the one used for the original object. Check the
           //   value and the allocator of the newly created object are correct.
           //
           //   Next, Create a new object
           //   using construct facility by passing the original object and a different
           //   allocator to the one used for the original object. Check the
           //   value and the allocator of the newly created object are correct.
           //
           // Testing:
           //   construct (const TARGET_TYPE&, bslma::Allocator*)
           // --------------------------------------------------------------------

           if (verbose) printf("\nTESTING 'copy construct'"
                                       "\n=========================\n");

           bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const TA = &testAllocator;
           bslma::TestAllocator otherAllocator(veryVeryVeryVerbose);
           bslma::TestAllocator *const OA = &otherAllocator;
           {
               my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                               V1, TA);
               my_Class1 dest = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                               src, src.d_def.d_allocator_p);

               ASSERTV(src.d_def.d_value, 1 == src.d_def.d_value);
               ASSERTV(dest.d_def.d_value, 1 == dest.d_def.d_value);
               ASSERT(0 == dest.d_def.d_allocator_p);
           }
           {
               my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                       V2, TA);
               my_Class2 dest = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                       src, src.d_def.d_allocator_p);

               ASSERTV(src.d_def.d_value, 2 == src.d_def.d_value);
               ASSERT(TA == src.d_def.d_allocator_p);
               ASSERTV(dest.d_def.d_value, 2 == dest.d_def.d_value);
               ASSERT(TA == dest.d_def.d_allocator_p);
           }
           {
               my_Class2a src = BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                       V2A, TA);
               my_Class2a dest = BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                       src, src.d_data.d_def.d_allocator_p);

               ASSERTV(src.d_data.d_def.d_value, 42 == src.d_data.d_def.d_value);
               ASSERT(TA == src.d_data.d_def.d_allocator_p);
               ASSERTV(dest.d_data.d_def.d_value, 42 == dest.d_data.d_def.d_value);
               ASSERT(TA == dest.d_data.d_def.d_allocator_p);
           }
           {
                my_Class1 src = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                                V1, TA);
                my_Class1 dest = BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(
                                src, OA);

                ASSERTV(src.d_def.d_value, 1 == src.d_def.d_value);
                ASSERTV(dest.d_def.d_value, 1 == dest.d_def.d_value);
                ASSERT(0 == dest.d_def.d_allocator_p);
            }
            {
                my_Class2 src = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                                V2, TA);
                my_Class2 dest = BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(
                                src, OA);

                ASSERTV(src.d_def.d_value, 2 == src.d_def.d_value);
                ASSERT(TA == src.d_def.d_allocator_p);
                ASSERTV(dest.d_def.d_value, 2 == dest.d_def.d_value);
                ASSERT(OA == dest.d_def.d_allocator_p);
            }
            {
                my_Class2a src = BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                              V2A, TA);
                my_Class2a dest = BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(
                              src, OA);

                ASSERTV(src.d_data.d_def.d_value, 42 == src.d_data.d_def.d_value);
                ASSERT(TA == src.d_data.d_def.d_allocator_p);
                ASSERTV(dest.d_data.d_def.d_value, 42 == dest.d_data.d_def.d_value);
                ASSERT(OA == dest.d_data.d_def.d_allocator_p);
            }

       } break;
      case 2: {
          // --------------------------------------------------------------------
          // TESTING 'defaultConstruct'
          //
          // Concerns:
          //   o That the default constructor properly forwards the allocator
          //     when appropriate.
          //
          // Plan: Construct an object using construct overload taking only an allocator,
          //   and verify that the value and allocator of the object are as expected.
          //
          // Testing:
          //   construct(bslma::Allocator*)
          // --------------------------------------------------------------------

          if (verbose) printf("\nTESTING 'default construct'"
                              "\n==========================\n");

          bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
          bslma::TestAllocator *const TA = &testAllocator;

          {
              my_Class1 dest =
                  BloombergLP::bslalg::AllocatorUtil<my_Class1>::construct(TA);
              ASSERTV(dest.d_def.d_value, 0 == dest.d_def.d_value);
          }
          {
              my_Class2 dest =
                   BloombergLP::bslalg::AllocatorUtil<my_Class2>::construct(TA);
              ASSERTV(dest.d_def.d_value, 0 == dest.d_def.d_value);
              ASSERT(TA == dest.d_def.d_allocator_p);
          }
          {
              my_Class2a dest =
                  BloombergLP::bslalg::AllocatorUtil<my_Class2a>::construct(TA);
              ASSERTV(dest.d_data.d_def.d_value, 0 == dest.d_data.d_def.d_value);
              ASSERT(TA == dest.d_data.d_def.d_allocator_p);
          }
          {
              my_Class3 dest =
                  BloombergLP::bslalg::AllocatorUtil<my_Class3>::construct(TA);
              ASSERTV(dest.d_def.d_value, 0 == dest.d_def.d_value);
              ASSERT(TA == dest.d_def.d_allocator_p);
          }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That the templates can be instantiated without errors.
        //
        // Plan:
        //: 1 Simply instantiate the templates in very simple examples,
        //:   constructing or destroying an object stored in some buffer.  No
        //:   thorough testing is performed, beyond simply asserting the call
        //:   was forwarded properly by examining the value of the buffer
        //:   storing the object.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        my_Class1 v1(1);        ASSERT(1 == v1.value());
        my_Class2 v2(2);        ASSERT(2 == v2.value());

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        bslma::Allocator *const theAlloc = &testAllocator;

        // 'defaultConstruct' invokes default constructor, with defaulted
        // arguments, even if type does not take an allocator.

        my_Class1 vdefault =  bslalg::AllocatorUtil<my_Class1>::construct(theAlloc);
        ASSERT(0 == vdefault.d_def.d_allocator_p);
        ASSERT(0 == vdefault.d_def.d_value);

        // 'copyConstruct' invokes copy constructor, even if type does not take
        // an allocator.

        my_Class1 v1copy = bslalg::AllocatorUtil<my_Class1>::construct(v1, theAlloc);
        ASSERT(0 == v1copy.d_def.d_allocator_p);
        ASSERT(1 == v1copy.d_def.d_value);

        // 'copyConstruct' invokes copy constructor, passing the allocator if
        // type takes an allocator.

        my_Class2 v2copy = bslalg::AllocatorUtil<my_Class2>::construct(v2, theAlloc);

        ASSERT(theAlloc == v2copy.d_def.d_allocator_p);
        ASSERT(2 == v2copy.d_def.d_value);

        // 'construct' invokes constructor, even if type does not take an
        // allocator.

        my_Class1 v13 = bslalg::AllocatorUtil<my_Class1>::construct(3, theAlloc);

        ASSERT(0 == v13.d_def.d_allocator_p);
        ASSERT(3 == v13.d_def.d_value);


        // 'construct' invokes constructor, passing the allocator if type takes
        // an allocator.

        my_Class2 v24 = bslalg::AllocatorUtil<my_Class2>::construct(4, theAlloc);
        ASSERT(theAlloc == v24.d_def.d_allocator_p);
        ASSERT(4 == v24.d_def.d_value);

        // 'destroy' invokes destructor ... with no particular constraints.

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
