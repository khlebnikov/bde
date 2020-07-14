// bslstl_optional.t.cpp                                              -*-C++-*-

#include <bsls_bsltestutil.h>
#include <bslstl_optional.h>
#include <bslstl_string.h>

#include <bslalg_constructorproxy.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

// A list of disabled tests :
//
//BSLSTL_OPTIONAL_TEST_BAD_VALUE
//      Tests in this group check that value category and cv qualification
//      of the return value from value() member function is correct.
//
//BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT
//      Tests in this group check that assignments is not possible if the
//      'value_type' is not both assignable and constructible from the source
//      type.
//
//BSLSTL_OPTIONAL_TEST_BAD_IL_EMPLACE
//      Tests in this group check that emplace is not possible for const
//      optional types.
//
//BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
//      Tests in this group check that assignments is not possible to
//      an optional of const type or to a const qualified optional

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The object under test is a type whose interface and contract is dictated by
// the C++ standard.  The general concern is compliance with the standard.  In
// C++03 mode, the concern is to test all the features of the standard type
// that can be supported using C++03 features.  This type is implemented in the
// form of a class template, and thus its proper instantiation for several
// types is a concern.  The purpose of this type is to represent a value object
// that may or may not exist.  If the value object is allocator aware, this
// type has an additional interface to allow for specifying the allocator and
// for retrieving the allocator in use.  This is implemented by having a
// different class template specialisation depending on whether the value type
// is allocator aware or not, and thus the behaviour needs to be tested for
// both allocator aware and non allocator aware value types.  One of the
// guarantees this type provides is that no unnecessary copies of the value
// type object are created when using this type as opposed to using a raw value
// type object.  In order to test this guarantee, we use test types designed to
// count the number of instances created.
//
//  Internal implementation types:
// [ 1] 'Optional_Data' class
// [ 2] 'Optional_DataImp' class

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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

#ifdef BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
#define ASSERT_IF_BOOL_CONVERSION(X) ASSERT(X)
#else
#define ASSERT_IF_BOOL_CONVERSION(X) ASSERT(X)
#endif  //BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT
// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

using namespace BloombergLP;
using namespace bsl;

typedef bslmf::MovableRefUtil MoveUtil;

namespace { const int MOVED_FROM_VAL = 0x01d; }  // close unnamed namespace

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

#define BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING                          \
    MyClass1, MyClass1a, MyClass2, MyClass2a

#define BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS                              \
    ConstructTestTypeNoAlloc, ConstructTestTypeAlloc,                         \
        ConstructTestTypeAllocArgT

                              // ================
                              // class MyClassDef
                              // ================

struct MyClassDef {
    // Data members that give MyClassX size and alignment.  This class is a
    // simple aggregate, use to provide a common data layout to subsequent test
    // types.  There are no semantics associated with any of the members, in
    // particular the allocator pointer is not used directly by this aggregate
    // to allocate storage owned by this class.

    // DATA (exceptionally public, only in test driver)
    int               d_value;
    int              *d_data_p;
    bslma::Allocator *d_allocator_p;
};

// In optimized builds, some compilers will elide some of the operations in the
// destructors of the test classes defined below.  In order to force the
// compiler to retain all of the code in the destructors, we provide the
// following function that can be used to (conditionally) print out the state
// of a 'MyClassDef' data member.  If the destructor calls this function as its
// last operation, then all values set in the destructor have visible
// side-effects, but non-verbose test runs do not have to be burdened with
// additional output.

static bool forceDestructorCall = false;

void dumpClassDefState(const MyClassDef& def)
{
    if (forceDestructorCall) {
        printf("%p: %d %p %p\n",
               &def,
               def.d_value,
               def.d_data_p,
               def.d_allocator_p);
    }
}

                               // ==============
                               // class MyClass1
                               // ==============

struct MyClass1 {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'MyClassDef' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer, while
    // the 'd_data_p' pointer is never initialized.  This class supports move,
    // copy, and destructor counters and can be used in tests that check for
    // unnecessary copies.  A signal value, 'MOVED_FROM_VAL', is used to detect
    // an object in a moved-from state.

    // DATA
    MyClassDef d_def;

    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_destructorInvocations;

    // CREATORS
    MyClass1(int v = 0)  // IMPLICIT
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
    }
    MyClass1(const MyClass1& rhs)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
        ++s_copyConstructorInvocations;
    }

    MyClass1(bslmf::MovableRef<MyClass1> other)
    {
        MyClass1& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p    = 0;
        ++s_moveConstructorInvocations;
    }

    ~MyClass1()
    {
        ASSERT(d_def.d_value != 91);
        d_def.d_value       = 91;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
        ++s_destructorInvocations;
    }

    MyClass1& operator=(const MyClass1& rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    MyClass1& operator=(bslmf::MovableRef<MyClass1> rhs)
        // assign the value of specified 'rhs' to this object
    {
        MyClass1& otherRef     = MoveUtil::access(rhs);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        return *this;
    }

    MyClass1& operator=(int rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_def.d_value = rhs;
        return *this;
    }
    // ACCESSORS
    int value() const { return d_def.d_value; }
};
bool operator==(const MyClass1& lhs, const MyClass1& rhs)
{
    return (lhs.value() == rhs.value());
}
// CLASS DATA
int MyClass1::s_copyConstructorInvocations = 0;
int MyClass1::s_moveConstructorInvocations = 0;
int MyClass1::s_destructorInvocations      = 0;

                              // ===============
                              // class MyClass1a
                              // ===============

struct MyClass1a {
    // This 'class' is the same as My_Class1, except it also supports
    // conversion from My_Class1. This allows for testing of converting
    // constructors and assignment from a type convertible to value type.

    MyClass1   d_data;
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_destructorInvocations;

    // CREATORS

    MyClass1a(int v = 0)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass1a(const MyClass1& v)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass1a(bslmf::MovableRef<MyClass1> v)  // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(v)))
    {
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a(bslmf::MovableRef<const MyClass1> v)
    : d_data(MoveUtil::access(v))
    {
    }   // IMPLICIT
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass1a(const MyClass1a& rhs)
    : d_data(rhs.d_data)
    {
        ++s_copyConstructorInvocations;
    }

    MyClass1a(bslmf::MovableRef<MyClass1a> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
        ++s_moveConstructorInvocations;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a(bslmf::MovableRef<const MyClass1a> rhs)
    : d_data(MoveUtil::access(rhs).d_data)
    {
        // a move from a const object is a copy
        ++s_copyConstructorInvocations;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    // MANIPULATORS
    MyClass1a& operator=(const MyClass1a& rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(rhs.d_data);
        return *this;
    }

    MyClass1a& operator=(bslmf::MovableRef<MyClass1a> rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass1a& operator=(bslmf::MovableRef<const MyClass1a> rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(MoveUtil::access(rhs).d_data);
        return *this;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass1a& operator=(int rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(rhs);
        return *this;
    }

    ~MyClass1a()
    {
        ASSERT(d_data.d_def.d_value != 92);
        d_data.d_def.d_value       = 92;
        d_data.d_def.d_allocator_p = 0;
        dumpClassDefState(d_data.d_def);
        ++s_destructorInvocations;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};
// CLASS DATA
int MyClass1a::s_copyConstructorInvocations = 0;
int MyClass1a::s_moveConstructorInvocations = 0;
int MyClass1a::s_destructorInvocations      = 0;
bool operator==(const MyClass1a& lhs, const MyClass1a& rhs)
{
    return (lhs.value() == rhs.value());
}

                               // ==============
                               // class MyClass2
                               // ==============
struct MyClass2 {
    // This 'class' supports the 'bslma::UsesBslmaAllocator' trait, providing
    // an allocator-aware version of every constructor.  While it holds an
    // allocator and has the expected allocator propagation properties of a
    // 'bslma::Allocator'-aware type, it does not actually allocate any memory.
    // This class supports move, copy, and destructor counters and can be used
    // in tests that check for unnecessary copies and correct destructor
    // invocation.  A signal value, 'MOVED_FROM_VAL', is used to detect an
    // object in a moved-from state.  This class is convertable and assignable
    // from an object of type 'MyClass1', which allows testing of converting
    // constructors and assignment from a type convertible to value type.

    // DATA
    MyClassDef d_def;

    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_destructorInvocations;

    // CREATORS
    explicit MyClass2(bslma::Allocator *a = 0)
    {
        d_def.d_value       = 0;
        d_def.d_allocator_p = a;
    }

    MyClass2(int v, bslma::Allocator *a = 0)  // IMPLICIT
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = a;
    }
    MyClass2(const MyClass2& rhs, bslma::Allocator *a = 0)  // IMPLICIT
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
        s_copyConstructorInvocations++;
    }

    MyClass2(bslmf::MovableRef<MyClass2> other, bslma::Allocator *a = 0)
    {
        // IMPLICIT
        MyClass2& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
        s_moveConstructorInvocations++;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass2> other, bslma::Allocator *a = 0)
    {  // IMPLICIT

        const MyClass2& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
        // a move from a const object is a copy
        s_copyConstructorInvocations++;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2(const MyClass1& rhs, bslma::Allocator *a = 0)  // IMPLICIT
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    MyClass2(bslmf::MovableRef<MyClass1> other, bslma::Allocator *a = 0)
        // IMPLICIT
    {
        MyClass1& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p    = a;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2(bslmf::MovableRef<const MyClass1> other, bslma::Allocator *a = 0)
    {  // IMPLICIT

        const MyClass1& otherRef = MoveUtil::access(other);
        d_def.d_value            = otherRef.d_def.d_value;
        d_def.d_allocator_p      = a;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~MyClass2()
    {
        ASSERT(d_def.d_value != 92);
        d_def.d_value       = 92;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
        ++s_destructorInvocations;
    }

    // MANIPULATORS
    MyClass2& operator=(const MyClass2& rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_def.d_value = rhs.d_def.d_value;
        // do not touch allocator!
        return *this;
    }

    MyClass2& operator=(bslmf::MovableRef<MyClass2> rhs)
        // assign the value of specified 'rhs' to this object
    {
        MyClass2& otherRef     = MoveUtil::access(rhs);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        // do not touch allocator!
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2& operator=(bslmf::MovableRef<const MyClass2> rhs)
        // assign the value of specified 'rhs' to this object
    {
        const MyClass2& otherRef = MoveUtil::access(rhs);
        d_def.d_value            = otherRef.d_def.d_value;
        // do not touch allocator!
        return *this;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2& operator=(int rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_def.d_value = rhs;
        // do not touch allocator!
        return *this;
    }

    // ACCESSORS

    // returns the value of this object
    int value() const { return d_def.d_value; }

    // returns the allocator used by this object
    bsl::allocator<char> get_allocator() const { return d_def.d_allocator_p; }
};
// CLASS DATA
int MyClass2::s_copyConstructorInvocations = 0;
int MyClass2::s_moveConstructorInvocations = 0;
int MyClass2::s_destructorInvocations      = 0;

bool operator==(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() == rhs.value());
}

bool operator==(const int& lhs, const MyClass2& rhs)
{
    return (lhs == rhs.value());
}
bool operator==(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() == rhs);
}
bool operator!=(const MyClass2& lhs, const MyClass2& rhs)
{
    return !(lhs == rhs);
}

bool operator!=(const int& lhs, const MyClass2& rhs)
{
    return !(lhs == rhs);
}
bool operator!=(const MyClass2& lhs, const int& rhs)
{
    return !(lhs == rhs);
}
bool operator<(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() < rhs.value());
}

bool operator<(const int& lhs, const MyClass2& rhs)
{
    return (lhs < rhs.value());
}
bool operator<(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() < rhs);
}
bool operator>(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() > rhs.value());
}

bool operator>(const int& lhs, const MyClass2& rhs)
{
    return (lhs > rhs.value());
}
bool operator>(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() > rhs);
}
bool operator<=(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() <= rhs.value());
}

bool operator<=(const int& lhs, const MyClass2& rhs)
{
    return (lhs <= rhs.value());
}
bool operator<=(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() <= rhs);
}
bool operator>=(const MyClass2& lhs, const MyClass2& rhs)
{
    return (lhs.value() >= rhs.value());
}

bool operator>=(const int& lhs, const MyClass2& rhs)
{
    return (lhs >= rhs.value());
}
bool operator>=(const MyClass2& lhs, const int& rhs)
{
    return (lhs.value() >= rhs);
}

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<MyClass2> : bsl::true_type {
};

}  // close namespace bslma
}  // close enterprise namespace

                                 // =========
                                 // MyClass2a
                                 // =========

struct MyClass2a {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.  This
    // class is constructible and assignable from MyClass2

    MyClass2   d_data;

    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;
    static int s_destructorInvocations;

    // CREATORS
    MyClass2a()
    : d_data()
    {
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    explicit MyClass2a(int v)  // IMPLICIT
    : d_data(v)
    {
    }

    MyClass2a(const MyClass2& rhs)  // IMPLICIT
    : d_data(rhs)
    {
    }

    MyClass2a(bslmf::MovableRef<MyClass2> rhs)  // IMPLICIT
    : d_data(MoveUtil::move(MoveUtil::access(rhs)))
    {
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bslmf::MovableRef<const MyClass2> rhs)  // IMPLICIT
    : d_data(MoveUtil::access(rhs))
    {
        ++s_copyConstructorInvocations;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(const MyClass2a& rhs)
    : d_data(rhs.d_data)
    {
        ++s_copyConstructorInvocations;
    }

    MyClass2a(bslmf::MovableRef<MyClass2a> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
        ++s_moveConstructorInvocations;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bslmf::MovableRef<const MyClass2a> rhs)
    : d_data(MoveUtil::access(rhs).d_data)
    {
        ++s_copyConstructorInvocations;
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a)
    {
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2& v)
    : d_data(v, a)
    {
    }

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator            *a,
              bslmf::MovableRef<MyClass2>  v)
    : d_data(MoveUtil::move(MoveUtil::access(v)), a)
    {
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator                  *a,
              bslmf::MovableRef<const MyClass2>  v)
    : d_data(MoveUtil::access(v), a)
    {
    }
#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2a& rhs)
    : d_data(rhs.d_data, a)
    {
        ++s_copyConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator             *a,
              bslmf::MovableRef<MyClass2a>  rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
        ++s_moveConstructorInvocations;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a(bsl::allocator_arg_t,
              bslma::Allocator                   *a,
              bslmf::MovableRef<const MyClass2a>  rhs)
    : d_data(MoveUtil::access(rhs).d_data, a)
    {
        ++s_copyConstructorInvocations;
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~MyClass2a()
    {
        ASSERT(d_data.d_def.d_value != 93);
        d_data.d_def.d_value       = 93;
        d_data.d_def.d_allocator_p = 0;
        dumpClassDefState(d_data.d_def);
        ++s_destructorInvocations;
    }

    // MANIPULATORS
    MyClass2a& operator=(const MyClass2a& rhs)
    {
        d_data. operator=(rhs.d_data);
        return *this;
    }

    MyClass2a& operator=(bslmf::MovableRef<MyClass2a> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(bslmf::MovableRef<const MyClass2a> rhs)
    {
        d_data. operator=(MoveUtil::access(rhs).d_data);
        return *this;
    }

#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(int rhs)
    {
        d_data. operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }

    bsl::allocator<char> get_allocator() const
    {
        return d_data.get_allocator();
    }
};

bool operator==(const MyClass2a& lhs, const MyClass2a& rhs)
{
    return (lhs.value() == rhs.value());
}
int MyClass2a::s_copyConstructorInvocations = 0;
int MyClass2a::s_moveConstructorInvocations = 0;
int MyClass2a::s_destructorInvocations      = 0;

// TRAITS
namespace BloombergLP {

namespace bslma {
template <>
struct UsesBslmaAllocator<MyClass2a> : bsl::true_type {
};
}  // close namespace bslma

namespace bslmf {
template <>
struct UsesAllocatorArgT<MyClass2a> : bsl::true_type {
};
}  // close namespace bslmf

}  // close enterprise namespace

                                 // =========
                                 // MyClass2b
                                 // =========

class MyClass2b {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.  This
    // class is assignable from MyClass2, but not constructible from MyClass2
  public:
    MyClass2 d_data;

    // CREATORS
    MyClass2b()
    : d_data()
    {
    }

    MyClass2b(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    MyClass2b(const MyClass2b& rhs)
    : d_data(rhs.d_data)
    {
    }

    MyClass2b(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2b& rhs)
    : d_data(rhs.d_data, a)
    {
    }

    MyClass2b(bslmf::MovableRef<MyClass2b> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
    }

    MyClass2b(bsl::allocator_arg_t,
              bslma::Allocator             *a,
              bslmf::MovableRef<MyClass2b>  rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
    }

    // MANIPULATORS
    MyClass2b& operator=(const MyClass2b& rhs)
    {
        d_data. operator=(rhs.d_data);
        return *this;
    }

    MyClass2b& operator=(bslmf::MovableRef<MyClass2b> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

    MyClass2b& operator=(const MyClass2& rhs)
    {
        d_data. operator=(rhs);
        return *this;
    }

    MyClass2b& operator=(bslmf::MovableRef<MyClass2> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs)));
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }

    bsl::allocator<char> get_allocator() const
    {
        return d_data.get_allocator();
    }
};
bool operator==(const MyClass2b& lhs, const MyClass2b& rhs)
{
    return (lhs.value() == rhs.value());
}

// TRAITS
namespace BloombergLP {

namespace bslma {
template <>
struct UsesBslmaAllocator<MyClass2b> : bsl::true_type {
};
}  // close namespace bslma

namespace bslmf {
template <>
struct UsesAllocatorArgT<MyClass2b> : bsl::true_type {
};
}  // close namespace bslmf

}  // close enterprise namespace

                                 // =========
                                 // MyClass2c
                                 // =========

class MyClass2c {
    // This 'class' behaves the same as 'MyClass2' (allocator-aware type that
    // never actually allocates memory) except that it uses the
    // 'allocator_arg_t' idiom for passing an allocator to constructors.  This
    // class is constructable from MyClass2, but not assignable from MyClass2

  public:
    MyClass2 d_data;

    // CREATORS
    MyClass2c()
    : d_data()
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a)
    : d_data(a)
    {
    }

    explicit MyClass2c(int v)
    : d_data(v)
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a)
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2& v)
    : d_data(v, a)
    {
    }

    MyClass2c(bsl::allocator_arg_t,
              bslma::Allocator            *a,
              bslmf::MovableRef<MyClass2>  v)
    : d_data(MoveUtil::move(v), a)
    {
    }

    MyClass2c(const MyClass2c& rhs)
    : d_data(rhs.d_data)
    {
    }

    MyClass2c(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2c& rhs)
    : d_data(rhs.d_data, a)
    {
    }

    MyClass2c(bslmf::MovableRef<MyClass2c> rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data))
    {
    }

    MyClass2c(bsl::allocator_arg_t,
              bslma::Allocator             *a,
              bslmf::MovableRef<MyClass2c>  rhs)
    : d_data(MoveUtil::move(MoveUtil::access(rhs).d_data), a)
    {
    }

    // MANIPULATORS
    MyClass2c& operator=(const MyClass2c& rhs)
    {
        d_data. operator=(rhs.d_data);
        return *this;
    }

    MyClass2c& operator=(bslmf::MovableRef<MyClass2c> rhs)
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

    MyClass2c& operator=(int rhs)
    {
        d_data. operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }

    bsl::allocator<char> get_allocator() const
    {
        return d_data.get_allocator();
    }
};
bool operator==(const MyClass2c& lhs, const MyClass2c& rhs)
{
    return (lhs.value() == rhs.value());
}
// TRAITS
namespace BloombergLP {

namespace bslma {
template <>
struct UsesBslmaAllocator<MyClass2c> : bsl::true_type {
};
}  // close namespace bslma

namespace bslmf {
template <>
struct UsesAllocatorArgT<MyClass2c> : bsl::true_type {
};
}  // close namespace bslmf

}  // close enterprise namespace

                               // ==============
                               // class MyClass3
                               // ==============

class MyClass3 {
    // This 'class' takes allocators similarly to 'MyClass2'. In addition, it
    // has overloads taking const rvalues.

  public:
    // DATA
    MyClassDef d_def;

    // CREATORS
    explicit MyClass3(bslma::Allocator *a = 0)
    {
        d_def.d_value       = 0;
        d_def.d_allocator_p = a;
    }
    MyClass3(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = a;
    }
    MyClass3(const MyClass3& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    MyClass3(const MyClass2& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    MyClass3(bslmf::MovableRef<MyClass3> other, bslma::Allocator *a = 0)
    {
        MyClass3& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value + 10;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    MyClass3(const MyClass3&& other, bslma::Allocator *a = 0)
    {
        d_def.d_value = other.d_def.d_value + 20;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = other.d_def.d_allocator_p;
        }
    }
#endif
    ~MyClass3()
    {
        ASSERT(d_def.d_value != 93);
        d_def.d_value       = 93;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    MyClass3& operator=(const MyClass3& rhs)
    {
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
struct UsesBslmaAllocator<MyClass3> : bsl::true_type {
};

}  // close namespace bslma
}  // close enterprise namespace

                               // ==============
                               // class MyClass4
                               // ==============

class MyClass4 {
    // This 'class' is the same as MyClass3, except it doesn't use allocators.

  public:
    // DATA
    MyClassDef d_def;

    // CREATORS
    explicit MyClass4()
    {
        d_def.d_value       = 0;
        d_def.d_allocator_p = 0;
    }
    MyClass4(int v)
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = 0;
    }
    MyClass4(const MyClass4& rhs)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    MyClass4(const MyClass2& rhs)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    MyClass4(bslmf::MovableRef<MyClass4> other)
    {
        MyClass4& otherRef     = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value + 10;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    MyClass4(const MyClass4&& other)
    {
        d_def.d_value = other.d_def.d_value + 20;
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    ~MyClass4()
    {
        ASSERT(d_def.d_value != 93);
        d_def.d_value       = 93;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    MyClass4& operator=(const MyClass4& rhs)
    {
        d_def.d_value = rhs.d_def.d_value;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};

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
    int       d_copied_count;
        // if true, indicates this object was created from an rvalue

    // CREATORS
    ConstructTestArg(int value = -1);
        // Create an object having the specified 'value'.

    ConstructTestArg(const ConstructTestArg& other);
    ConstructTestArg(bslmf::MovableRef<ConstructTestArg> other);
};

// CREATORS
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(int value)
: d_value(value)
, d_copied_count(0)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(const ConstructTestArg& other)
: d_value(other.d_value)
, d_copied_count(other.d_copied_count + 1)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(
                                     bslmf::MovableRef<ConstructTestArg> other)
: d_value(MoveUtil::access(other).d_value)
, d_copied_count(MoveUtil::access(other).d_copied_count)
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
        N1 = -1  // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    static int s_copyConstructorInvocations;
    static int s_moveConstructorInvocations;

    int        d_ilsum;  // sum of initializer_list argument

    Arg1       d_a1;
    Arg2       d_a2;
    Arg3       d_a3;
    Arg4       d_a4;
    Arg5       d_a5;
    Arg6       d_a6;
    Arg7       d_a7;
    Arg8       d_a8;
    Arg9       d_a9;
    Arg10      d_a10;
    Arg11      d_a11;
    Arg12      d_a12;
    Arg13      d_a13;
    Arg14      d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)

    ConstructTestTypeNoAlloc()
    : d_ilsum(0){};

    explicit ConstructTestTypeNoAlloc(const Arg1& a1)
    : d_ilsum(0)
    , d_a1(a1)
    {
    }
    explicit ConstructTestTypeNoAlloc(bslmf::MovableRef<Arg1> a1)
    : d_ilsum(0)
    , d_a1(MoveUtil::move(a1))
    {
    }

    template <class ARG1, class ARG2>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
    }

    template <class ARG1, class ARG2, class ARG3>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    explicit ConstructTestTypeNoAlloc(
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    explicit ConstructTestTypeNoAlloc(
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
    }

    ConstructTestTypeNoAlloc(const ConstructTestTypeNoAlloc& other)
    : d_ilsum(0)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeNoAlloc(
                            bslmf::MovableRef<ConstructTestTypeNoAlloc>& other)
    : d_ilsum(0)
    , d_a1(MoveUtil::access(other).d_a1)
    , d_a2(MoveUtil::access(other).d_a2)
    , d_a3(MoveUtil::access(other).d_a3)
    , d_a4(MoveUtil::access(other).d_a4)
    , d_a5(MoveUtil::access(other).d_a5)
    , d_a6(MoveUtil::access(other).d_a6)
    , d_a7(MoveUtil::access(other).d_a7)
    , d_a8(MoveUtil::access(other).d_a8)
    , d_a9(MoveUtil::access(other).d_a9)
    , d_a10(MoveUtil::access(other).d_a10)
    , d_a11(MoveUtil::access(other).d_a11)
    , d_a12(MoveUtil::access(other).d_a12)
    , d_a13(MoveUtil::access(other).d_a13)
    , d_a14(MoveUtil::access(other).d_a14)
    {
        ++s_moveConstructorInvocations;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    ConstructTestTypeNoAlloc(std::initializer_list<int> il)
    : d_ilsum(0)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    explicit ConstructTestTypeNoAlloc(
                                    std::initializer_list<int>              il,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                                    BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>              il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    explicit ConstructTestTypeNoAlloc(
        std::initializer_list<int>               il,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARG14) a14)
    : d_ilsum(0)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
        for (int i : il)
            d_ilsum += i;
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
};
int ConstructTestTypeNoAlloc::s_copyConstructorInvocations = 0;
int ConstructTestTypeNoAlloc::s_moveConstructorInvocations = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeNoAlloc& lhs,
                const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_ilsum == rhs.d_ilsum &&
           lhs.d_a1.d_value == rhs.d_a1.d_value &&
           lhs.d_a2.d_value == rhs.d_a2.d_value &&
           lhs.d_a3.d_value == rhs.d_a3.d_value &&
           lhs.d_a4.d_value == rhs.d_a4.d_value &&
           lhs.d_a5.d_value == rhs.d_a5.d_value &&
           lhs.d_a6.d_value == rhs.d_a6.d_value &&
           lhs.d_a7.d_value == rhs.d_a7.d_value &&
           lhs.d_a8.d_value == rhs.d_a8.d_value &&
           lhs.d_a9.d_value == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}
bool createdAlike(const ConstructTestTypeNoAlloc& lhs,
                  const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_a1.d_copied_count == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
}
                        // ============================
                        // class ConstructTestTypeAlloc
                        // ============================

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
        N1 = -1  // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    static int        s_copyConstructorInvocations;
    static int        s_moveConstructorInvocations;

    int               d_ilsum;  // sum of initializer_list argument

    bslma::Allocator *d_allocator_p;
    Arg1              d_a1;
    Arg2              d_a2;
    Arg3              d_a3;
    Arg4              d_a4;
    Arg5              d_a5;
    Arg6              d_a6;
    Arg7              d_a7;
    Arg8              d_a8;
    Arg9              d_a9;
    Arg10             d_a10;
    Arg11             d_a11;
    Arg12             d_a12;
    Arg13             d_a13;
    Arg14             d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    explicit ConstructTestTypeAlloc(bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    {
    }
    ConstructTestTypeAlloc(const ConstructTestTypeAlloc&  other,
                           bslma::Allocator              *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeAlloc(
                      bslmf::MovableRef<ConstructTestTypeAlloc>  other,
                      bslma::Allocator                          *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(MoveUtil::move(MoveUtil::access(other).d_a1))
    , d_a2(MoveUtil::move(MoveUtil::access(other).d_a2))
    , d_a3(MoveUtil::move(MoveUtil::access(other).d_a3))
    , d_a4(MoveUtil::move(MoveUtil::access(other).d_a4))
    , d_a5(MoveUtil::move(MoveUtil::access(other).d_a5))
    , d_a6(MoveUtil::move(MoveUtil::access(other).d_a6))
    , d_a7(MoveUtil::move(MoveUtil::access(other).d_a7))
    , d_a8(MoveUtil::move(MoveUtil::access(other).d_a8))
    , d_a9(MoveUtil::move(MoveUtil::access(other).d_a9))
    , d_a10(MoveUtil::move(MoveUtil::access(other).d_a10))
    , d_a11(MoveUtil::move(MoveUtil::access(other).d_a11))
    , d_a12(MoveUtil::move(MoveUtil::access(other).d_a12))
    , d_a13(MoveUtil::move(MoveUtil::access(other).d_a13))
    , d_a14(MoveUtil::move(MoveUtil::access(other).d_a14))
    {
        ++s_moveConstructorInvocations;
    }

    explicit ConstructTestTypeAlloc(const Arg1&       a1,
                                    bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(a1)
    {
    }
    explicit ConstructTestTypeAlloc(bslmf::MovableRef<Arg1>  a1,
                                    bslma::Allocator        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(MoveUtil::move(a1))
    {
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        const Arg2&                              a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(a2)
    {
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        bslmf::MovableRef<Arg2>                  a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(MoveUtil::move(a2))
    {
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        const Arg3&                              a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(a3)
    {
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        bslmf::MovableRef<Arg3>                  a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(MoveUtil::move(a3))
    {
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        const Arg4&                              a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(a4)
    {
    }
    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        bslmf::MovableRef<Arg4>                  a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(MoveUtil::move(a4))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        const Arg5&                              a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(a5)
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        bslmf::MovableRef<Arg5>                  a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(MoveUtil::move(a5))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        const Arg6&                              a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(a6)
    {
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        bslmf::MovableRef<Arg6>                  a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(MoveUtil::move(a6))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           const Arg7&                             a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(a7)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           bslmf::MovableRef<Arg7>                 a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(MoveUtil::move(a7))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           const Arg8&                             a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(a8)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           bslmf::MovableRef<Arg8>                 a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(MoveUtil::move(a8))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           const Arg9&                             a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(a9)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           bslmf::MovableRef<Arg9>                 a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(MoveUtil::move(a9))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           const Arg10&                            a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(a10)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           bslmf::MovableRef<Arg10>                a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(MoveUtil::move(a10))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           const Arg11&                             a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(a11)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           bslmf::MovableRef<Arg11>                 a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(MoveUtil::move(a11))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           const Arg12&                             a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(a12)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           bslmf::MovableRef<Arg12>                 a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(MoveUtil::move(a12))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           const Arg13&                             a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(a13)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           bslmf::MovableRef<Arg13>                 a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(MoveUtil::move(a13))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           const Arg14&                             a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(a14)
    {
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           bslmf::MovableRef<Arg14>                 a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(MoveUtil::move(a14))
    {
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    explicit ConstructTestTypeAlloc(std::initializer_list<int>  il,
                                    bslma::Allocator           *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    {
        for (int i : il)
            d_ilsum += i;
    }
    explicit ConstructTestTypeAlloc(std::initializer_list<int>  il,
                                    const Arg1&                 a1,
                                    bslma::Allocator           *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(a1)
    {
        for (int i : il)
            d_ilsum += i;
    }
    explicit ConstructTestTypeAlloc(std::initializer_list<int>  il,
                                    bslmf::MovableRef<Arg1>     a1,
                                    bslma::Allocator           *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(MoveUtil::move(a1))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        const Arg2&                              a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(a2)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    explicit ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        bslmf::MovableRef<Arg2>                  a2,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(MoveUtil::move(a2))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        const Arg3&                              a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(a3)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        bslmf::MovableRef<Arg3>                  a3,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(MoveUtil::move(a3))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        const Arg4&                              a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(a4)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        bslmf::MovableRef<Arg4>                  a4,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(MoveUtil::move(a4))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        const Arg5&                              a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(a5)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        bslmf::MovableRef<Arg5>                  a5,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(MoveUtil::move(a5))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        const Arg6&                              a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(a6)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAlloc(
                        std::initializer_list<int>               il,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                        BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                        bslmf::MovableRef<Arg6>                  a6,
                        bslma::Allocator                        *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(MoveUtil::move(a6))
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           const Arg7&                             a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(a7)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           bslmf::MovableRef<Arg7>                 a7,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(MoveUtil::move(a7))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           const Arg8&                             a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(a8)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           bslmf::MovableRef<Arg8>                 a8,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(MoveUtil::move(a8))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           const Arg9&                             a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(a9)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           bslmf::MovableRef<Arg9>                 a9,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(MoveUtil::move(a9))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           const Arg10&                            a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(a10)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int>              il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1) a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2) a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3) a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4) a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5) a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6) a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7) a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8) a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9) a9,
                           bslmf::MovableRef<Arg10>                a10,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(MoveUtil::move(a10))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           const Arg11&                             a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(a11)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           bslmf::MovableRef<Arg11>                 a11,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(MoveUtil::move(a11))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           const Arg12&                             a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(a12)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           bslmf::MovableRef<Arg12>                 a12,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(MoveUtil::move(a12))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           const Arg13&                             a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(a13)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           bslmf::MovableRef<Arg13>                 a13,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(MoveUtil::move(a13))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           const Arg14&                             a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(a14)
    {
        for (int i : il)
            d_ilsum += i;
    }
    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int>               il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG10) a10,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG11) a11,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG12) a12,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG13) a13,
                           bslmf::MovableRef<Arg14>                 a14,
                           bslma::Allocator *allocator = 0)
    : d_ilsum(0)
    , d_allocator_p(allocator)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(MoveUtil::move(a14))
    {
        for (int i : il)
            d_ilsum += i;
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    bsl::allocator<char> get_allocator() const { return d_allocator_p; }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAlloc> : bsl::true_type {
};

}  // close namespace bslma
}  // close enterprise namespace
int ConstructTestTypeAlloc::s_copyConstructorInvocations = 0;
int ConstructTestTypeAlloc::s_moveConstructorInvocations = 0;
// FREE OPERATORS
bool operator==(const ConstructTestTypeAlloc& lhs,
                const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_ilsum == rhs.d_ilsum &&
           lhs.d_a1.d_value == rhs.d_a1.d_value &&
           lhs.d_a2.d_value == rhs.d_a2.d_value &&
           lhs.d_a3.d_value == rhs.d_a3.d_value &&
           lhs.d_a4.d_value == rhs.d_a4.d_value &&
           lhs.d_a5.d_value == rhs.d_a5.d_value &&
           lhs.d_a6.d_value == rhs.d_a6.d_value &&
           lhs.d_a7.d_value == rhs.d_a7.d_value &&
           lhs.d_a8.d_value == rhs.d_a8.d_value &&
           lhs.d_a9.d_value == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}
bool createdAlike(const ConstructTestTypeAlloc& lhs,
                  const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_a1.d_copied_count == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
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
        N1 = -1  // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    static int        s_copyConstructorInvocations;
    static int        s_moveConstructorInvocations;

    int               d_ilsum;
    bslma::Allocator *d_allocator_p;
    Arg1              d_a1;
    Arg2              d_a2;
    Arg3              d_a3;
    Arg4              d_a4;
    Arg5              d_a5;
    Arg6              d_a6;
    Arg7              d_a7;
    Arg8              d_a8;
    Arg9              d_a9;
    Arg10             d_a10;
    Arg11             d_a11;
    Arg12             d_a12;
    Arg13             d_a13;
    Arg14             d_a14;

    // CREATORS (exceptionally in-line, only within a test driver)
    ConstructTestTypeAllocArgT()
    : d_ilsum(0)
    , d_allocator_p(0)
    {
    }

    ConstructTestTypeAllocArgT(ConstructTestTypeAllocArgT const& other)
    : d_ilsum(0)
    , d_allocator_p(0)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(
                           bslmf::MovableRef<ConstructTestTypeAllocArgT> other)
    : d_ilsum(0)
    , d_allocator_p(MoveUtil::access(other).d_allocator_p)
    , d_a1(MoveUtil::move(MoveUtil::access(other).d_a1))
    , d_a2(MoveUtil::move(MoveUtil::access(other).d_a2))
    , d_a3(MoveUtil::move(MoveUtil::access(other).d_a3))
    , d_a4(MoveUtil::move(MoveUtil::access(other).d_a4))
    , d_a5(MoveUtil::move(MoveUtil::access(other).d_a5))
    , d_a6(MoveUtil::move(MoveUtil::access(other).d_a6))
    , d_a7(MoveUtil::move(MoveUtil::access(other).d_a7))
    , d_a8(MoveUtil::move(MoveUtil::access(other).d_a8))
    , d_a9(MoveUtil::move(MoveUtil::access(other).d_a9))
    , d_a10(MoveUtil::move(MoveUtil::access(other).d_a10))
    , d_a11(MoveUtil::move(MoveUtil::access(other).d_a11))
    , d_a12(MoveUtil::move(MoveUtil::access(other).d_a12))
    , d_a13(MoveUtil::move(MoveUtil::access(other).d_a13))
    , d_a14(MoveUtil::move(MoveUtil::access(other).d_a14))
    {
        ++s_moveConstructorInvocations;
    }
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                  *alloc,
                               ConstructTestTypeAllocArgT const&  other)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(other.d_a1)
    , d_a2(other.d_a2)
    , d_a3(other.d_a3)
    , d_a4(other.d_a4)
    , d_a5(other.d_a5)
    , d_a6(other.d_a6)
    , d_a7(other.d_a7)
    , d_a8(other.d_a8)
    , d_a9(other.d_a9)
    , d_a10(other.d_a10)
    , d_a11(other.d_a11)
    , d_a12(other.d_a12)
    , d_a13(other.d_a13)
    , d_a14(other.d_a14)
    {
        ++s_copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(
                          bsl::allocator_arg_t,
                          bslma::Allocator                              *alloc,
                          bslmf::MovableRef<ConstructTestTypeAllocArgT>  other)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(MoveUtil::move(MoveUtil::access(other).d_a1))
    , d_a2(MoveUtil::move(MoveUtil::access(other).d_a2))
    , d_a3(MoveUtil::move(MoveUtil::access(other).d_a3))
    , d_a4(MoveUtil::move(MoveUtil::access(other).d_a4))
    , d_a5(MoveUtil::move(MoveUtil::access(other).d_a5))
    , d_a6(MoveUtil::move(MoveUtil::access(other).d_a6))
    , d_a7(MoveUtil::move(MoveUtil::access(other).d_a7))
    , d_a8(MoveUtil::move(MoveUtil::access(other).d_a8))
    , d_a9(MoveUtil::move(MoveUtil::access(other).d_a9))
    , d_a10(MoveUtil::move(MoveUtil::access(other).d_a10))
    , d_a11(MoveUtil::move(MoveUtil::access(other).d_a11))
    , d_a12(MoveUtil::move(MoveUtil::access(other).d_a12))
    , d_a13(MoveUtil::move(MoveUtil::access(other).d_a13))
    , d_a14(MoveUtil::move(MoveUtil::access(other).d_a14))
    {
        ++s_moveConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t, bslma::Allocator *alloc)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    {
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator     *alloc,
                               const Arg1&           a1)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(a1)
    {
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator        *alloc,
                               bslmf::MovableRef<Arg1>  a1)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(MoveUtil::move(a1))
    {
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14)  a14)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator           *alloc,
                               std::initializer_list<int>  il)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                        *alloc,
                               std::initializer_list<int>               il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
        for (int i : il)
            d_ilsum += i;
    }

    template <class ARG1,
              class ARG2,
              class ARG3,
              class ARG4,
              class ARG5,
              class ARG6,
              class ARG7,
              class ARG8,
              class ARG9,
              class ARG10,
              class ARG11,
              class ARG12,
              class ARG13,
              class ARG14>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t,
                               bslma::Allocator                         *alloc,
                               std::initializer_list<int>                il,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)   a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)   a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)   a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)   a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)   a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)   a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)   a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)   a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)   a9,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG10)  a10,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG11)  a11,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG12)  a12,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG13)  a13,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG14)  a14)
    : d_ilsum(0)
    , d_allocator_p(alloc)
    , d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    , d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    , d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    , d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    , d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    , d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    , d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    , d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    , d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    , d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    , d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    , d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    , d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    , d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
        for (int i : il)
            d_ilsum += i;
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

    bsl::allocator<char> get_allocator() const { return d_allocator_p; }
};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAllocArgT> : bsl::true_type {
};

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<ConstructTestTypeAllocArgT> : bsl::true_type {
};

}  // close namespace bslmf
}  // close enterprise namespace
int ConstructTestTypeAllocArgT::s_copyConstructorInvocations = 0;
int ConstructTestTypeAllocArgT::s_moveConstructorInvocations = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeAllocArgT& lhs,
                const ConstructTestTypeAllocArgT& rhs)
{
    return lhs.d_a1.d_value == rhs.d_a1.d_value &&
           lhs.d_a2.d_value == rhs.d_a2.d_value &&
           lhs.d_a3.d_value == rhs.d_a3.d_value &&
           lhs.d_a4.d_value == rhs.d_a4.d_value &&
           lhs.d_a5.d_value == rhs.d_a5.d_value &&
           lhs.d_a6.d_value == rhs.d_a6.d_value &&
           lhs.d_a7.d_value == rhs.d_a7.d_value &&
           lhs.d_a8.d_value == rhs.d_a8.d_value &&
           lhs.d_a9.d_value == rhs.d_a9.d_value &&
           lhs.d_a10.d_value == rhs.d_a10.d_value &&
           lhs.d_a11.d_value == rhs.d_a11.d_value &&
           lhs.d_a12.d_value == rhs.d_a12.d_value &&
           lhs.d_a13.d_value == rhs.d_a13.d_value &&
           lhs.d_a14.d_value == rhs.d_a14.d_value;
}
bool createdAlike(const ConstructTestTypeAllocArgT& lhs,
                  const ConstructTestTypeAllocArgT& rhs)
{
    return lhs.d_a1.d_copied_count == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
}

const MyClass1       V1(1);
const MyClass2       V2(2);
const MyClass3       V4(4);
const MyClass4       V5(5);
ConstructTestArg<1>  VA1(1);
ConstructTestArg<2>  VA2(2);
ConstructTestArg<3>  VA3(3);
ConstructTestArg<4>  VA4(4);
ConstructTestArg<5>  VA5(5);
ConstructTestArg<6>  VA6(6);
ConstructTestArg<7>  VA7(7);
ConstructTestArg<8>  VA8(8);
ConstructTestArg<9>  VA9(9);
ConstructTestArg<10> VA10(10);
ConstructTestArg<11> VA11(11);
ConstructTestArg<12> VA12(12);
ConstructTestArg<13> VA13(13);
ConstructTestArg<14> VA14(14);

struct Swappable {
    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    int d_value;

    // CLASS METHODS
    static bool swapCalled() { return 0 != s_swapCalled; }

    static void swapReset() { s_swapCalled = 0; }

    // CREATORS
    explicit Swappable(int v)
    : d_value(v)
    {
    }
};

// FREE OPERATORS
bool operator==(const Swappable& lhs, const Swappable& rhs)
{
    return lhs.d_value == rhs.d_value;
}

// PUBLIC CLASS DATA
int Swappable::s_swapCalled = 0;

void swap(Swappable& a, Swappable& b)
{
    ++Swappable::s_swapCalled;

    BloombergLP::bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

struct SwappableAA {
    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    MyClassDef d_def;

    // CLASS METHODS
    static bool swapCalled() { return 0 != s_swapCalled; }

    static void swapReset() { s_swapCalled = 0; }

    // CREATORS
    explicit SwappableAA(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value       = v;
        d_def.d_allocator_p = a;
    }

    SwappableAA(const SwappableAA& rhs, bslma::Allocator *a = 0)
    {
        d_def.d_value       = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    SwappableAA(bslmf::MovableRef<SwappableAA> other, bslma::Allocator *a = 0)
    {
        SwappableAA& otherRef  = MoveUtil::access(other);
        d_def.d_value          = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
    }
};

// FREE OPERATORS
bool operator==(const SwappableAA& lhs, const SwappableAA& rhs)
{
    return lhs.d_def.d_value == rhs.d_def.d_value;
}

// PUBLIC CLASS DATA
int SwappableAA::s_swapCalled = 0;

void swap(SwappableAA& a, SwappableAA& b)
{
    ++SwappableAA::s_swapCalled;

    BloombergLP::bslalg::SwapUtil::swap(&a.d_def.d_value, &b.d_def.d_value);
}
// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<SwappableAA> : bsl::true_type {
};

}  // close namespace bslma
}  // close namespace BloombergLP

namespace BloombergLP {
namespace bslh {

template <class HASHALG, class RETURN, class CLASS>
void hashAppend(HASHALG& hashAlg, RETURN (CLASS:: *member)())
{
    hashAlg(&member, sizeof(member));
}

template <class HASHALG, class RETURN, class CLASS>
void hashAppend(HASHALG& hashAlg, RETURN (CLASS:: *member)() const)
{
    hashAlg(&member, sizeof(member));
}

}  // close namespace bslh
}  // close enterprise namespace

// helper functions to determine the type of a reference
bool isConstRef(const MyClass1&)
{
    return true;
}
bool isConstRef(MyClass1&)
{
    return false;
}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
bool isConstRef(MyClass1&&)
{
    return false;
}
bool isConstRef(const MyClass1&&)
{
    return true;
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
bool isRvalueRef(const MyClass1&)
{
    return false;
}
bool isRvalueRef(MyClass1&)
{
    return false;
}
bool isRvalueRef(MyClass1&&)
{
    return true;
}
bool isRvalueRef(const MyClass1&&)
{
    return true;
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

bool isConstRef(const MyClass2&)
{
    return true;
}
bool isConstRef(MyClass2&)
{
    return false;
}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
bool isConstRef(MyClass2&&)
{
    return false;
}
bool isConstRef(const MyClass2&&)
{
    return true;
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
bool isRvalueRef(const MyClass2&)
{
    return false;
}
bool isRvalueRef(MyClass2&)
{
    return false;
}
bool isRvalueRef(MyClass2&&)
{
    return true;
}
bool isRvalueRef(const MyClass2&&)
{
    return true;
}
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

// helper functions to determine the constness of a pointer
template <typename T>
bool isConstPtr(T *)
{
    return false;
}
template <typename T>
bool isConstPtr(const T *)
{
    return true;
}

#define CHECK_INSTANCES_EMPLACE_EMPTY                                         \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        ValueType expVal;                                                     \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        mX.emplace();                                                         \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
    }

#define CHECK_INSTANCES_EMPLACE(args)                                         \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        ValueType expVal(args);                                               \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        mX.emplace(args);                                                     \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
    }

#define TEST_EMPLACE(expArgs, emplaceArgs)                                    \
    {                                                                         \
        int              expCopy = ValueType::s_copyConstructorInvocations;   \
        int              expMove = ValueType::s_moveConstructorInvocations;   \
        ValWithAllocator valBuffer expArgs;                                   \
        ValueType&                 EXP = valBuffer.object();                  \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj     = objBuffer.object();                        \
        int              numCopy = ValueType::s_copyConstructorInvocations;   \
        int              numMove = ValueType::s_moveConstructorInvocations;   \
        obj.emplace      emplaceArgs;                                         \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERTV(expMove, expMove == numMove);                                 \
        ASSERTV(numMove, expMove == numMove);                                 \
        ASSERT(EXP == obj.value());                                           \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
        ASSERT(createdAlike(EXP, obj.value()) == true);                       \
    }

#define TEST_ASSIGN_VAL_EMPTY(source)                                         \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        ValueType expVal(source);                                             \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        obj         = source;                                                 \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;      \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;      \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_MOVE_ASSIGN_VAL_EMPTY(source)                                    \
    {                                                                         \
        ValueType srcCopy(source);                                            \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        ValueType expVal(MoveUtil::move(srcCopy));                            \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        obj         = MoveUtil::move(source);                                 \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;      \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;      \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_ASSIGN_VAL_ENGAGED(source)                                        \
    {                                                                          \
        ValueType expVal;                                                      \
        int       expCopy = ValueType::s_copyConstructorInvocations;           \
        int       expMove = ValueType::s_moveConstructorInvocations;           \
        expVal            = source;                                            \
        expCopy           = ValueType::s_copyConstructorInvocations - expCopy; \
        expMove           = ValueType::s_moveConstructorInvocations - expMove; \
        ObjWithAllocator objBuffer(&oa);                                       \
        Obj&             obj = objBuffer.object();                             \
        obj.emplace();                                                         \
        ASSERT(obj.has_value());                                               \
        int numCopy = ValueType::s_copyConstructorInvocations;                 \
        int numMove = ValueType::s_moveConstructorInvocations;                 \
        obj         = source;                                                  \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;       \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;       \
        ASSERT(expCopy == numCopy);                                            \
        ASSERT(expMove == numMove);                                            \
        ASSERT(obj.has_value());                                               \
        ASSERT(obj.value() == expVal);                                         \
        ASSERT(checkAllocator(obj.value(), &oa));                              \
    }

#define TEST_MOVE_ASSIGN_VAL_ENGAGED(source)                                   \
    {                                                                          \
        ValueType srcCopy(source);                                             \
        ValueType expVal;                                                      \
        int       expCopy = ValueType::s_copyConstructorInvocations;           \
        int       expMove = ValueType::s_moveConstructorInvocations;           \
        expVal            = MoveUtil::move(srcCopy);                           \
        expCopy           = ValueType::s_copyConstructorInvocations - expCopy; \
        expMove           = ValueType::s_moveConstructorInvocations - expMove; \
        ObjWithAllocator objBuffer(&oa);                                       \
        Obj&             obj = objBuffer.object();                             \
        obj.emplace();                                                         \
        ASSERT(obj.has_value());                                               \
        int numCopy = ValueType::s_copyConstructorInvocations;                 \
        int numMove = ValueType::s_moveConstructorInvocations;                 \
        obj         = MoveUtil::move(source);                                  \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;       \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;       \
        ASSERT(expCopy == numCopy);                                            \
        ASSERT(expMove == numMove);                                            \
        ASSERT(obj.has_value());                                               \
        ASSERT(obj.value() == expVal);                                         \
        ASSERT(checkAllocator(obj.value(), &oa));                              \
    }
#define TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(source)                              \
    {                                                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        obj = source;                                                         \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &oa));                                     \
    }
#define TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(source)                            \
    {                                                                         \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        obj.emplace(ValueType());                                             \
        ASSERT(obj.has_value());                                              \
        int numDest = ValueType::s_destructorInvocations;                     \
        obj         = source;                                                 \
        numDest     = ValueType::s_destructorInvocations - numDest;           \
        ASSERT(1 == numDest);                                                 \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &oa));                                     \
    }

#define TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source)                            \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        ValueType expVal(source.value());                                     \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        obj         = source;                                                 \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;      \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;      \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }

#define TEST_MOVE_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source)                       \
    {                                                                         \
        OPT_TYPE  srcCopy(source);                                            \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        ValueType expVal(MoveUtil::move(srcCopy.value()));                    \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        ObjWithAllocator objBuffer(&oa);                                      \
        Obj&             obj = objBuffer.object();                            \
        ASSERT(!obj.has_value());                                             \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        obj         = MoveUtil::move(source);                                 \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;      \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;      \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(source.value() == srcCopy.value());                            \
        ASSERT(checkAllocator(obj.value(), &oa));                             \
    }
#define TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source)                           \
    {                                                                          \
        ValueType expVal;                                                      \
        int       expCopy = ValueType::s_copyConstructorInvocations;           \
        int       expMove = ValueType::s_moveConstructorInvocations;           \
        expVal            = source.value();                                    \
        expCopy           = ValueType::s_copyConstructorInvocations - expCopy; \
        expMove           = ValueType::s_moveConstructorInvocations - expMove; \
        ObjWithAllocator objBuffer(&oa);                                       \
        Obj&             obj = objBuffer.object();                             \
        obj.emplace();                                                         \
        ASSERT(obj.has_value());                                               \
        int numCopy = ValueType::s_copyConstructorInvocations;                 \
        int numMove = ValueType::s_moveConstructorInvocations;                 \
        obj         = source;                                                  \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;       \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;       \
        ASSERT(expCopy == numCopy);                                            \
        ASSERT(expMove == numMove);                                            \
        ASSERT(obj.has_value());                                               \
        ASSERT(obj.value() == expVal);                                         \
        ASSERT(checkAllocator(obj.value(), &oa));                              \
    }

#define TEST_MOVE_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source)                      \
    {                                                                          \
        OPT_TYPE  srcCopy(source);                                             \
        ValueType expVal;                                                      \
        int       expCopy = ValueType::s_copyConstructorInvocations;           \
        int       expMove = ValueType::s_moveConstructorInvocations;           \
        expVal            = MoveUtil::move(srcCopy.value());                   \
        expCopy           = ValueType::s_copyConstructorInvocations - expCopy; \
        expMove           = ValueType::s_moveConstructorInvocations - expMove; \
        ObjWithAllocator objBuffer(&oa);                                       \
        Obj&             obj = objBuffer.object();                             \
        obj.emplace();                                                         \
        ASSERT(obj.has_value());                                               \
        int numCopy = ValueType::s_copyConstructorInvocations;                 \
        int numMove = ValueType::s_moveConstructorInvocations;                 \
        obj         = MoveUtil::move(source);                                  \
        numCopy     = ValueType::s_copyConstructorInvocations - numCopy;       \
        numMove     = ValueType::s_moveConstructorInvocations - numMove;       \
        ASSERT(expCopy == numCopy);                                            \
        ASSERT(expMove == numMove);                                            \
        ASSERT(obj.has_value());                                               \
        ASSERT(obj.value() == expVal);                                         \
        ASSERT(checkAllocator(obj.value(), &oa));                              \
        ASSERT(source.value() == srcCopy.value());                             \
    }

#define TEST_COPY_FROM_EMPTY_OPT(source)                                      \
    {                                                                         \
        bslma::TestAllocatorMonitor dam(&da);                                 \
        ASSERT(!source.has_value());                                          \
        bsl::optional<DEST_TYPE> obj(source);                                 \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_MOVE_FROM_EMPTY_OPT(source, propagate)                           \
    {                                                                         \
        bslma::TestAllocator&       expAlloc = (propagate ? oa : da);         \
        bslma::TestAllocatorMonitor dam(&expAlloc);                           \
        ASSERT(!source.has_value());                                          \
        bsl::optional<DEST_TYPE> obj(MoveUtil::move(source));                 \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &expAlloc));                               \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_COPY_FROM_ENGAGED_OPT(source)                                    \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        DEST_TYPE expVal(source.value());                                     \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        bsl::optional<DEST_TYPE> obj(source);                                 \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }
#define TEST_MOVE_FROM_ENGAGED_OPT(source, propagate)                         \
    {                                                                         \
        SRC_OPT_TYPE          srcCopy(source);                                \
        bslma::TestAllocator& expAlloc = (propagate ? oa : da);               \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        DEST_TYPE expVal(MoveUtil::move(srcCopy.value()));                    \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        bsl::optional<DEST_TYPE> obj(MoveUtil::move(source));                 \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERTV(expCopy, numCopy, expCopy == numCopy);                        \
        ASSERTV(expMove, numMove, expMove == numMove);                        \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &expAlloc));                               \
        ASSERT(checkAllocator(obj.value(), &expAlloc));                       \
    }

#define TEST_EXT_COPY_FROM_EMPTY_OPT(source)                                  \
    {                                                                         \
        bslma::TestAllocatorMonitor dam(&da);                                 \
        ASSERT(!source.has_value());                                          \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, source);                   \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_EXT_MOVE_FROM_EMPTY_OPT(source)                                  \
    {                                                                         \
        bslma::TestAllocatorMonitor dam(&da);                                 \
        ASSERT(!source.has_value());                                          \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, MoveUtil::move(source));   \
        ASSERT(!obj.has_value());                                             \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(dam.isTotalSame());                                            \
    }

#define TEST_EXT_COPY_FROM_ENGAGED_OPT(source)                                \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        DEST_TYPE expVal(source.value());                                     \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        int           numCopy = ValueType::s_copyConstructorInvocations;      \
        int           numMove = ValueType::s_moveConstructorInvocations;      \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, source);                   \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }
#define TEST_EXT_MOVE_FROM_ENGAGED_OPT(source)                                \
    {                                                                         \
        SRC_OPT_TYPE srcCopy(source);                                         \
        int          expCopy = ValueType::s_copyConstructorInvocations;       \
        int          expMove = ValueType::s_moveConstructorInvocations;       \
        DEST_TYPE    expVal(MoveUtil::move(srcCopy.value()));                 \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        int           numCopy = ValueType::s_copyConstructorInvocations;      \
        int           numMove = ValueType::s_moveConstructorInvocations;      \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, MoveUtil::move(source));   \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }

#define TEST_COPY_FROM_VALUE(source)                                          \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        DEST_TYPE expVal(source);                                             \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        bsl::optional<DEST_TYPE> obj(source);                                 \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }
#define TEST_MOVE_FROM_VALUE(source)                                          \
    {                                                                         \
        SRC_TYPE  srcCopy(source);                                            \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        DEST_TYPE expVal(MoveUtil::move(srcCopy));                            \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        bsl::optional<DEST_TYPE> obj(MoveUtil::move(source));                 \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }

#define TEST_EXT_COPY_FROM_VALUE(source)                                      \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        DEST_TYPE expVal(source);                                             \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        int           numCopy = ValueType::s_copyConstructorInvocations;      \
        int           numMove = ValueType::s_moveConstructorInvocations;      \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, source);                   \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }
#define TEST_EXT_MOVE_FROM_VALUE(source)                                      \
    {                                                                         \
        SRC_TYPE  srcCopy(source);                                            \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        DEST_TYPE expVal(MoveUtil::move(srcCopy));                            \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        int           numCopy = ValueType::s_copyConstructorInvocations;      \
        int           numMove = ValueType::s_moveConstructorInvocations;      \
        DEST_OPT_TYPE obj(bsl::allocator_arg, &ta, MoveUtil::move(source));   \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == expVal);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }

#define TEST_IN_PLACE_CONSTRUCT(init, expArgs, expAlloc)                      \
    {                                                                         \
        int              expCopy = ValueType::s_copyConstructorInvocations;   \
        int              expMove = ValueType::s_moveConstructorInvocations;   \
        ValWithAllocator valBuffer expArgs;                                   \
        ValueType&                 EXP = valBuffer.object();                  \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        Obj obj init;                                                         \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERT(expMove == numMove);                                           \
        ASSERT(EXP == obj.value());                                           \
        ASSERT(obj.has_value());                                              \
        ASSERT(checkAllocator(obj, expAlloc));                                \
        ASSERT(checkAllocator(obj.value(), expAlloc));                        \
    }

#define TEST_MAKE_OPTIONAL_DEDUCED(source)                                    \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        bsl::optional<ValueType> expObj(bsl::in_place, source);               \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        bsl::optional<ValueType> obj = bsl::make_optional(source);            \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERTV(expCopy, numCopy, expCopy == numCopy);                        \
        ASSERTV(expMove, numMove, expMove == numMove);                        \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == source);                                        \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
    }

#define TEST_MAKE_OPTIONAL(expArgs, args)                                     \
    {                                                                         \
        int              expCopy = ValueType::s_copyConstructorInvocations;   \
        int              expMove = ValueType::s_moveConstructorInvocations;   \
        bsl::optional<ValueType> expObj expArgs;                              \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        int              numCopy = ValueType::s_copyConstructorInvocations;   \
        int              numMove = ValueType::s_moveConstructorInvocations;   \
        bsl::optional<ValueType> obj = bsl::make_optional<ValueType> args;    \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERTV(expMove, expMove == numMove);                                 \
        ASSERTV(numMove, expMove == numMove);                                 \
        ASSERT(checkAllocator(obj, &da));                                     \
        ASSERT(checkAllocator(obj.value(), &da));                             \
        ASSERT(createdAlike(expObj.value(), obj.value()) == true);            \
    }

#define TEST_ALLOC_OPTIONAL_DEDUCED(source)                                   \
    {                                                                         \
        int       expCopy = ValueType::s_copyConstructorInvocations;          \
        int       expMove = ValueType::s_moveConstructorInvocations;          \
        bsl::optional<ValueType> expObj(bsl::in_place, source);               \
        expCopy     = ValueType::s_copyConstructorInvocations - expCopy;      \
        expMove     = ValueType::s_moveConstructorInvocations - expMove;      \
        int numCopy = ValueType::s_copyConstructorInvocations;                \
        int numMove = ValueType::s_moveConstructorInvocations;                \
        bsl::optional<ValueType> obj = bsl::alloc_optional(&ta, source);       \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERTV(expCopy, numCopy, expCopy == numCopy);                        \
        ASSERTV(expMove, numMove, expMove == numMove);                        \
        ASSERT(obj.has_value());                                              \
        ASSERT(obj.value() == source);                                        \
        ASSERT(checkAllocator(obj, &ta));                                     \
        ASSERT(checkAllocator(obj.value(), &ta));                             \
    }

#define TEST_ALLOC_OPTIONAL(expArgs, args, alloc)                             \
    {                                                                         \
        int              expCopy = ValueType::s_copyConstructorInvocations;   \
        int              expMove = ValueType::s_moveConstructorInvocations;   \
        bsl::optional<ValueType> expObj expArgs;                              \
        expCopy = ValueType::s_copyConstructorInvocations - expCopy;          \
        expMove = ValueType::s_moveConstructorInvocations - expMove;          \
        int              numCopy = ValueType::s_copyConstructorInvocations;   \
        int              numMove = ValueType::s_moveConstructorInvocations;   \
        bsl::optional<ValueType> obj = bsl::alloc_optional<ValueType> args;   \
        numCopy = ValueType::s_copyConstructorInvocations - numCopy;          \
        numMove = ValueType::s_moveConstructorInvocations - numMove;          \
        ASSERT(expCopy == numCopy);                                           \
        ASSERTV(expMove, expMove == numMove);                                 \
        ASSERTV(numMove, expMove == numMove);                                 \
        ASSERT(checkAllocator(obj, alloc));                                   \
        ASSERT(checkAllocator(obj.value(), alloc));                           \
        ASSERT(createdAlike(expObj.value(), obj.value()) == true);            \
    }

template <class TYPE,
          bool USES_BSLMA_ALLOC =
              BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class Test_Util {
    // This class provided test utilities which have different behaviour
    // depending on whether 'TYPE is allocator aware or not.  The main template
    // is for allocator aware types.

  public:
    static bool checkAllocator(const TYPE&                 obj,
                               const bsl::allocator<char>& expected);
        // Check if, for the specified 'obj', 'obj.get_allocator()' returns the
        // specified 'expected' allocator.

    static bool hasSameAllocator(const TYPE& obj, const TYPE& other);
        // Check if, for the specified 'obj' and specified 'other',
        // 'obj.get_allocator() == other.get_allocator()';
};

template <class TYPE>
class Test_Util<TYPE, false> {
    // This class provided test utilities which have different behaviour
    // depending on whether 'TYPE is allocator aware or not.  This
    // specialization is for non allocator aware types.

  public:
    static bool checkAllocator(const TYPE&, const bsl::allocator<char>&);
        // return 'true'.

    static bool hasSameAllocator(const TYPE&, const TYPE&);
        // return 'true'.
};

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
bool Test_Util<TYPE, USES_BSLMA_ALLOC>::checkAllocator(
                                          const TYPE&                 obj,
                                          const bsl::allocator<char>& expected)
{
    return (expected == obj.get_allocator());
}

template <class TYPE, bool USES_BSLMA_ALLOC>
inline
bool Test_Util<TYPE, USES_BSLMA_ALLOC>::hasSameAllocator(const TYPE& obj,
                                                         const TYPE& other)
{
    return (obj.get_allocator() == other.get_allocator());
}

template <class TYPE>
inline bool
Test_Util<TYPE, false>::checkAllocator(const TYPE&,
                                       const bsl::allocator<char>&)
{
    return true;
}

template <class TYPE>
inline bool
Test_Util<TYPE, false>::hasSameAllocator(const TYPE&, const TYPE&)
{
    return true;
}

template <class TYPE>
bool checkAllocator(const TYPE& obj, const bsl::allocator<char>& allocator)
{
    return Test_Util<TYPE>::checkAllocator(obj, allocator);
}

template <class TYPE>
bool hasSameAllocator(const TYPE& obj1, const TYPE& obj2)
{
    return Test_Util<TYPE>::hasSameAllocator(obj1, obj2);
}

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class TEST_TYPE>
class TestDriver {
    // This class template provides a namespace for testing the 'optional'
    // type.

  private:
    // PRIVATE TYPES
    typedef TEST_TYPE ValueType;
        // ValueType under test.

    typedef bsl::optional<ValueType>             Obj;
    typedef bsl::optional<const ValueType>       ObjC;
    typedef const bsl::optional<ValueType>       CObj;
    typedef const bsl::optional<const ValueType> CObjC;
        // Type under test.

    typedef bslalg::ConstructorProxy<Obj> ObjWithAllocator;
        // Wrapper for 'Obj' whose constructor takes an allocator.

    typedef bslalg::ConstructorProxy<ValueType> ValWithAllocator;
        // Wrapper for 'ValueType' whose constructor takes an allocator.

    typedef bsltf::TestValuesArray<TEST_TYPE> TestValues;
        // Array of test values of 'TEST_TYPE'.

  public:

    static void testCase32();
        // TESTING type deduction

    static void testCase30();
    static void testCase30b();
        // TESTING 'alloc_optional' FACILITY

    static void testCase29();
    static void testCase29b();
        // TESTING 'make_optional' FACILITY

    static void testCase28();
        // TESTING TRAITS AND TYPEDEFS

    static void testCase27();
        // TESTING 'operator=' OVERLOAD RESOLUTION

    static void testCase26();
        // TESTING 'initializer_list' 'in_place_t' CONSTRUCTOR

    static void testCase25();
        // TESTING 'in_place_t' CONSTRUCTOR

    static void testCase24();
        // TESTING 'swap' METHOD

    static void testCase23();
        // TESTING 'hashAppend'

    template <class DEST_TYPE, class SRC_TYPE>
    static void testCase22_imp();
    static void testCase22();
        // TESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION FROM VALUE

    template <class DEST_TYPE, class SRC_TYPE>
    static void testCase21_imp();
    static void testCase21();
        // TESTING COPY/MOVE CONSTRUCTION FROM VALUE

    template <class DEST_TYPE, class SRC_TYPE>
    static void testCase20_imp();
    static void testCase20();
        // TESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION FROM OPTIONAL


    template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
    static void testCase19_imp();
    template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
    static void testCase19_imp_libgccbug();
    static void testCase19();
        // TESTING COPY/MOVE CONSTRUCTION FROM OPTIONAL

    template <class OPT_TYPE>
    static void testCase17_imp();
    static void testCase17();
        // TESTING 'operator=(optional_type)' MEMBER FUNCTION

    static void testCase15();
        // TESTING 'operator=(non_optional_type)' FUNCTIONALITY

    static void testCase14_imp();
    static void testCase14();
        // TESTING operator=(nullopt_t) MEMBER FUNCTION

    static void testCase13();
    static void testCase13b();
    static void testCase13c();
        // TESTING 'emplace' METHOD.  Note that this test can only be executed
        // with types that are constructible from an integer, have a 'value()'
        // method, and which provide a 's_copyConstructorInvocations' and
        // 's_moveConstructorInvocations' static variable that counts the
        // number of times copy/move constructor has been invoked.

    static void testCase12();
        // TESTING 'operator*' MEMBER FUNCTION.  Note that this test can only
        // be executed with types that are constructible from an integer, have
        // a 'value()' method, and for which there exists an overload of
        // 'isConstRef'.

    static void testCase11();
        // TESTING 'operator->' MEMBER FUNCTION.  Note that this test can only
        // be executed with types that are constructible from an integer and
        // have a 'value()' method.

    static void testCase10();
        // TESTING ALLOCATOR EXTENDED 'value_or' METHOD.  Note that this test
        // can only be executed with types that are constructible from an
        // integer, have a 'value()' method, are allocator aware and have a
        // 'get_allocator' method.

    static void testCase9();
        // TESTING 'value_or' METHOD.  Note that this test can only be executed
        // with types that are constructible from an integer, have a 'value()'
        // method, and, if TYPE is allocator aware, has a 'get_allocator'
        // method

    static void testCase8();
        // TESTING 'value' METHOD.  Note that this test can only be executed
        // with types that are constructible from an integer, have a 'value()'
        // method, and for which there exists an overload of 'isRvalueRef' and
        // 'isConstRef'.

    static void testCase7_imp();
    static void testCase7();
        // TESTING 'reset' MEMBER FUNCTION.  Note that this test requires
        // <TYPE> to provide a 's_destructorInvocations' static variable which
        // counts the number of times destructor has been invoked.

    static void testCase6();
        // TESTING CONVERSION TO BOOL

    static void testCase5();
        // ALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS.  Note that this test
        // requires 'TYPE' to be allocator aware.

    static void testCase4();
        // PRIMARY MANIPULATORS AND ACCESSORS.  Note that this test requires
        // <TYPE> to provide a 's_destructorInvocations' static variable which
        // counts the number of times destructor has been invoked.

    static void testCase3();
        // BREATHING TEST

    static void testCase2();
        // Optional_DataImp TEST.  The test requires TYPE to be default
        // constructible.

    static void testCase1();
        // Optional_Data TEST
};



template <class TYPE>
void bslstl_optional_value_type_deduce(const bsl::optional<TYPE>&)
{
}

template <class TYPE>
void bslstl_optional_optional_type_deduce(const TYPE&)
{
}

template <class TYPE>
void TestDriver<TYPE>::testCase32()
{
    // --------------------------------------------------------------------
    // TESTING type deduction
    //
    //  In this test, we check that type deduction with 'bsl::optional'
    //  works.
    //
    // Concerns:
    //: 1 Invoking a template function which takes 'bsl::optional<TYPE>'
    //    deduces the TYPE
    //: 2 Invoking a template function which takes 'TYPE' with 'bsl::optional'
    //    will be able to deduce 'TYPE' as 'bsl::optional'
    //
    // Plan:
    //: 2 Call a template function which takes bsl::optinal<TYPE> with an
    //    object of type bsl::optional<TestType>
    //: 3 Call a template function which takes TYPE with an object of type
    //    bsl::optional<TestType>
    //
    //
    // Testing:
    //    Type deduction
    //

    Obj x;

    bslstl_optional_value_type_deduce(x);
    bslstl_optional_optional_type_deduce(x);
}


template <class OPT_TYPE1, class OPT_TYPE2>
void testCase31_imp_a()
{
    OPT_TYPE1 X;
    OPT_TYPE2 Y;

    //comparing two disengaged optionals
    ASSERT(X == Y);     // If bool(x) != bool(y), false;
    ASSERT(!(X != Y));  // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X < Y));   // If !y, false;
    ASSERT(!(X > Y));   // If !x, false;
    ASSERT(X <= Y);     // If !x, true;
    ASSERT(X >= Y);     // If !y, true;

    //rhs disengaged, lhs engaged
    Y.emplace(3);
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT((X < Y));    // If !y, false; otherwise, if !x, true;
    ASSERT(!(X > Y));   // If !x, false;
    ASSERT(X <= Y);     // If !x, true;
    ASSERT(!(X >= Y));  // If !y, true; otherwise, if !x, false;

    //rhs engaged, lhs disengaged
    X.emplace(5);
    Y.reset();
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X < Y));   // If !y, false; otherwise, if !x, true;
    ASSERT((X > Y));    // If !x, false; otherwise, if !y, true;
    ASSERT(!(X <= Y));  // If !x, true; otherwise, if !y, false;
    ASSERT((X >= Y));   // If !y, true; otherwise, if !x, false;

    //both engaged, compare the values
    X.emplace(1);
    Y.emplace(3);
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT((X < Y));    // If !y, false; otherwise, if !x, true;
    ASSERT(!(X > Y));   // If !x, false; otherwise, if !y, true;
    ASSERT((X <= Y));   // If !x, true; otherwise, if !y, false;
    ASSERT(!(X >= Y));  // If !y, true; otherwise, if !x, false;
}

template <class OPT_TYPE, class VAL_TYPE>
void testCase31_imp_b()
{
    OPT_TYPE                             X;
    VAL_TYPE                             Y = 3;

    //comparison with a disengaged optional on rhs
    ASSERT(!(X == Y));  // return bool(x) ? *x == v : false;
    ASSERT((X != Y));   // return bool(x) ? *x != v : true;
    ASSERT((X < Y));    // return bool(x) ? *x < v : true;
    ASSERT(!(X > Y));   // return bool(x) ? *x > v : false;
    ASSERT((X <= Y));   // return bool(x) ? *x <= v : true;
    ASSERT(!(X >= Y));  // return bool(x) ? *x >= v : false;

    //comparison with a disengaged optional on lhs
    ASSERT(!(Y == X));  // return bool(x) ? v == *x : false;
    ASSERT((Y != X));   // return bool(x) ? v != *x : true;
    ASSERT(!(Y < X));   // return bool(x) ? v < *x : false;
    ASSERT((Y > X));    // return bool(x) ? v > *x : true;
    ASSERT(!(Y <= X));  // return bool(x) ? v <= *x : false;
    ASSERT((Y >= X));   // return bool(x) ? v >= *x : true;

    //comparison with an engaged optional on rhs
    X.emplace(7);
    ASSERT(!(X == Y));  // If bool(x) != bool(y), false;
    ASSERT((X != Y));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT(!(X < Y));   // If !y, false; otherwise, if !x, true;
    ASSERT((X > Y));    // If !x, false;
    ASSERT(!(X <= Y));  // If !x, true;
    ASSERT((X >= Y));   // If !y, true; otherwise, if !x, false;

    //comparison with an engaged optional on lhs
    ASSERT(!(Y == X));  // If bool(x) != bool(y), false;
    ASSERT((Y != X));   // If bool(x) != bool(y), true;
                        // otherwise, if bool(x) == false, false;
    ASSERT((Y < X));    // If !y, false; otherwise, if !x, true;
    ASSERT(!(Y > X));   // If !x, false;
    ASSERT((Y <= X));   // If !x, true;
    ASSERT(!(Y >= X));  // If !y, true; otherwise, if !x, false;
}

template <class VAL_TYPE>
void testCase31_imp_c()
{
    bsl::optional<VAL_TYPE> X;

    //comparison with a disengaged optional on rhs
    ASSERT((X == bsl::nullopt));   // !x
    ASSERT(!(X != bsl::nullopt));  // bool(x)
    ASSERT(!(X < bsl::nullopt));   // false
    ASSERT(!(X > bsl::nullopt));   // bool(x)
    ASSERT((X <= bsl::nullopt));   // !x
    ASSERT((X >= bsl::nullopt));   // true

    //comparison with a disengaged optional on lhs
    ASSERT((bsl::nullopt == X));   // !x
    ASSERT(!(bsl::nullopt != X));  // bool(x)
    ASSERT(!(bsl::nullopt < X));   // bool(x)
    ASSERT(!(bsl::nullopt > X));   // false
    ASSERT((bsl::nullopt <= X));   // true
    ASSERT((bsl::nullopt >= X));   // !x

    //comparison with an engaged optional on rhs
    X.emplace(7);
    ASSERT(!(X == bsl::nullopt));  // !x
    ASSERT((X != bsl::nullopt));   // bool(x)
    ASSERT(!(X < bsl::nullopt));   // false
    ASSERT((X > bsl::nullopt));    // bool(x)
    ASSERT(!(X <= bsl::nullopt));  // !x
    ASSERT((X >= bsl::nullopt));   // true

    //comparison with an engaged optional on lhs
    ASSERT(!(bsl::nullopt == X));  // !x
    ASSERT((bsl::nullopt != X));   // bool(x)
    ASSERT((bsl::nullopt < X));    // bool(x)
    ASSERT(!(bsl::nullopt > X));   // false
    ASSERT((bsl::nullopt <= X));   // true
    ASSERT(!(bsl::nullopt >= X));  // !x
}
void testCase31()
{
    // --------------------------------------------------------------------
    // TESTING RELATIONAL OPERATORS
    //
    // Concerns:
    //: 1 Two 'optional' objects can be compared if their 'value_type's are
    //:   comparable. The result depends on whether the objects are engaged
    //:   or not.
    //: 2 We can compare an 'optional' object of 'value_type' V and a non
    //:   'optional' object of type U if U and V are comparable types. The
    //:   result depends on whether the 'optional' object is engaged or not.
    //: 3 We can compare any 'optional' object with nulllopt_t. The result
    //:   depends on whether the 'optional' object is engaged or not.
    //
    // Plan:
    //: 1 For each relation operator, for concern 1, compare two 'optional'
    //:   objects of comparable valye types. Execute tests for a combination of
    //:   engaged and disengaged 'optional' objects.
    //: 2 For each relation operator, for concern 2, compare an 'optional'
    //:   object of type 'TYPE', and an object of type comparable to TYPE.
    //:   Execute tests for an engaged and disengaged 'optional' object.
    //: 3 For each relation operator, for concern 3, compare an 'optional'
    //:   object and a 'nullopt_t' object. Execute tests for an engaged and
    //:   disengaged 'optional' object.
    //
    // Testing:
    //   bool operator==(const optional<LHS>&, const optional<RHS>&)
    //   bool operator==(const optional<LHS>&, const RHS&)
    //   bool operator==(const LHS&, const optional<RHS>&)
    //   bool operator==(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator==(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator!=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator!=(const optional<LHS>&, const RHS&)
    //   bool operator!=(const LHS&, const optional<RHS>&)
    //   bool operator!=(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator!=(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator<=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator<=(const optional<LHS>&, const RHS&)
    //   bool operator<=(const LHS&, const optional<RHS>&)
    //   bool operator<=(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator<=(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator>=(const optional<LHS>&, const optional<RHS>&)
    //   bool operator>=(const optional<LHS>&, const RHS&)
    //   bool operator>=(const LHS&, const optional<RHS>&)
    //   bool operator>=(const optional<LHS>&, bsl::nullopt_t)
    //   bool operator>=(bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator< (const optional<LHS>&, const optional<RHS>&)
    //   bool operator< (const optional<LHS>&, const RHS&)
    //   bool operator< (const LHS&, const optional<RHS>&)
    //   bool operator< (const optional<LHS>&, bsl::nullopt_t)
    //   bool operator< (bsl::nullopt_t,         const optional<RHS>&)
    //   bool operator> (const optional<LHS>&, const optional<RHS>&)
    //   bool operator> (const optional<LHS>&, const RHS&)
    //   bool operator> (const LHS&, const optional<RHS>&)
    //   bool operator> (const optional<LHS>&, bsl::nullopt_t)
    //   bool operator> (bsl::nullopt_t,         const optional<RHS>&)

    if (verbose)
        printf("\nTESTING RELATIONAL OPERATORS"
               "\n============================\n");
    if (veryVerbose)
        printf("\tComparison with an 'optional'.\n");
    {
        testCase31_imp_a<bsl::optional<int>,
                                     bsl::optional<MyClass2> >();

        testCase31_imp_a<bsl::optional<MyClass2>,
                                     bsl::optional<int> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        testCase31_imp_a<std::optional<int>,
                                     bsl::optional<int> >();

        testCase31_imp_a<bsl::optional<int>,
                                     std::optional<int> >();

        testCase31_imp_a<std::optional<MyClass2>,
                                     bsl::optional<MyClass2> >();

        testCase31_imp_a<bsl::optional<MyClass2>,
                                     std::optional<MyClass2> >();

        testCase31_imp_a<std::optional<int>,
                                     bsl::optional<MyClass2> >();

        testCase31_imp_a<bsl::optional<MyClass2>,
                                     std::optional<int> >();

        testCase31_imp_a<std::optional<MyClass2>,
                                     bsl::optional<int> >();

        testCase31_imp_a<bsl::optional<int>,
                                     std::optional<MyClass2> >();
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

    }
    if (veryVerbose)
        printf("\tComparison with a non 'optional' .\n");
    {
        testCase31_imp_b<bsl::optional<int>,
                                     MyClass2 >();

        testCase31_imp_b<bsl::optional<MyClass2>,
                                     int >();
    }
    if (veryVerbose)
        printf("\tComparison with a nullopt_t .\n");
    {
        testCase31_imp_c<int>();
        testCase31_imp_c<MyClass2>();
    }
}


template <class TYPE>
void TestDriver<TYPE>::testCase30()
{
    // --------------------------------------------------------------------
    // TESTING 'alloc_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'alloc_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of 'alloc_optional' where the type
    //  is deduced.
    //
    //
    // Concerns:
    //: 1 Invoking 'alloc_optional' creates an 'optional' with the value of the
    //:   argument.
    //: 2 In a single non-allocator argument of 'alloc_optional' invocation,
    //:   the value type of the optional is deduced from the non allocator
    //:   argument.
    //: 3 Argument is perfectly forwarded.
    //: 4 The specified allocator is used as the allocator for the returned
    //:   'optional' object.
    //: 5 No unnecessary objects of 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Call 'alloc_optional' to create an 'optional'.  As an argument to
    //:   'alloc_optional' use an lvalue of the desired 'value_type'.  Check the
    //:   resulting 'optional' object has the expected value. [C-1]
    //: 2 In step 1, when invoking make_value, do not specify the template
    //:   argument. [C-2]
    //: 3 Repeat steps 1-2 with an rvalue argument, with a const lvalue
    //:   reference and with a const rvalue reference. Verify the number of
    //:   invocations of copy and move value type constructors is the same as
    //:   when the optional's in_place constructor has been called. [C-3][C-5]
    //: 4 In steps 1-3, check the resulting optional is using the allocator
    //:   specified in the 'alloc_optional' call. [C-4]
    //
    // Testing:
    //      alloc_optional(allocator_type const&, T&&);

    if (verbose)
        printf("\nTESTING 'alloc_optional' FACILITY"
               "\n==============+==================\n");

    if (veryVerbose)
        printf("\tDeduced type make_optional.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator         ta("third", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        ValWithAllocator sourceBuf(2, &oa);
        ValueType&       source      = sourceBuf.object();
        const ValueType& constSource = sourceBuf.object();

        TEST_ALLOC_OPTIONAL_DEDUCED(source);
        TEST_ALLOC_OPTIONAL_DEDUCED(constSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't deduction friendly when it comes to deducing
        // the allocator type
        TEST_ALLOC_OPTIONAL_DEDUCED(MoveUtil::move(source));
        TEST_ALLOC_OPTIONAL_DEDUCED(MoveUtil::move(constSource));
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase30b()
{
    // --------------------------------------------------------------------
    // TESTING 'alloc_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'alloc_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of 'alloc_optional' with var args.
    //
    //
    // Concerns:
    //: 1 Invoking 'alloc_optional' creates an 'optional' with the value of the
    //:   arguments converted to the specified value type.
    //: 2 Arguments are perfectly forwarded.
    //: 3 No unnecessary objects of 'value_type' are created.
    //: 4 The specified allocator is used as the allocator for the returned
    //:   'optional' object.
    //: 5 If the first argument is a 'braced-init-list', make_optional will
    //:   deduce an 'std::intializer list'
    //
    //
    // Plan:
    //: 1 Call 'alloc_optional' to create an 'optional'.  As an argument to
    //:   'alloc_optional' use an lvalue of the desired 'value_type'.  Check the
    //:   resulting 'optional' object has the expected value. [C-1]
    //: 2 Repeat step 1 with different number of arguments and with a
    //:   combination of lvalue and rvalue for each argument. Check that
    //:   the resulting object was created with the same combination of
    //:   lvalue and rvalue arguments. [C-2]
    //: 3 in steps 1-2, check there were no more copies of value type created
    //:   than when calling an in_place constructor. [C-3]
    //: 4 In steps 1-3, check the resulting optional is using the allocator
    //:   specified in the 'alloc_optional' call. [C-4]
    //: 5 Repeat steps 1-4 with an additional 'braced-init-list' as the first
    //:   argument to 'alloc_optional'. [C-5]
    //
    // Testing:
    //      alloc_optional(allocator_type const&, Args&&...);
    //      alloc_optional(allocator_type const&,
    //                     initializer_list<U> ,
    //                     Args&&... );

    if (verbose)
        printf("\nTESTING 'alloc_optional' FACILITY"
               "\n=================================\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_ALLOC_OPTIONAL((bsl::in_place), (&oa), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, MoveUtil::move(VA1)),
                           (&oa, MoveUtil::move(VA1)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, VA1), (&oa, VA1), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, MoveUtil::move(VA1), VA2),
                           (&oa, MoveUtil::move(VA1), VA2), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place, VA1, MoveUtil::move(VA2)),
                           (&oa, VA1, MoveUtil::move(VA2)), &oa);

        TEST_ALLOC_OPTIONAL(
               (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
               (&oa, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, VA1, MoveUtil::move(VA2), VA3),
                           (&oa, VA1, MoveUtil::move(VA2), VA3), &oa);

        TEST_ALLOC_OPTIONAL(
          (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4),
          (&oa, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4), &oa);
        TEST_ALLOC_OPTIONAL(
          (bsl::in_place, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)),
          (&oa, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)),
                           (&oa, MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)), &oa);

        TEST_ALLOC_OPTIONAL(
            (bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (&oa, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6),
                           (&oa, MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)),
                           (&oa, VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)),
                           (&oa, MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   (&oa, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8),
                           (&oa, MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)),
                           (&oa, VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)),
                           (&oa, MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9),
                           (&oa, VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10),
                           (&oa, MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)),
                           (&oa, VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)), &oa);
        /*
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   (&oa, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   (&oa, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   (&oa, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   (&oa, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   (&oa, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   (&oa, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14),
                   (&oa, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)),
                   (&oa, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)), &oa);
    */
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose)
        printf("\tUsing 'initializer_list' argument.\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_ALLOC_OPTIONAL((bsl::in_place, {1, 2, 3}), (&oa, {1, 2, 3}), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1)),
                           (&oa, {1, 2, 3}, MoveUtil::move(VA1)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, {1, 2, 3}, VA1), (&oa, {1, 2, 3}, VA1), &oa);

        TEST_ALLOC_OPTIONAL(
                         (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1), VA2),
                         (&oa, {1, 2, 3}, MoveUtil::move(VA1), VA2), &oa);
        TEST_ALLOC_OPTIONAL(
                         (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2)),
                         (&oa, {1, 2, 3}, VA1, MoveUtil::move(VA2)), &oa);

        TEST_ALLOC_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3)),
            (&oa, {1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)), &oa);

        TEST_ALLOC_OPTIONAL(
                    (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3),
                    (&oa, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3), &oa);

        TEST_ALLOC_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4),
            (&oa, {1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4), &oa);
        TEST_ALLOC_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4)),
            (&oa, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)),
                           (&oa, {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5),
                           (&oa, {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6),
                           (&oa, {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)),
                           (&oa, {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)),
                           (&oa, {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   (&oa, {1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8),
                           (&oa, {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)),
                           (&oa, {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)),
                           (&oa, {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9),
                           (&oa, {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10),
                           (&oa, {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)),
                           (&oa, {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)), &oa);
        /*
        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13), &oa);

        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14), &oa);
        TEST_ALLOC_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)), &oa);
    */
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

template <class TYPE>
void TestDriver<TYPE>::testCase29()
{
    // --------------------------------------------------------------------
    // TESTING 'make_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'make_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of 'make_optional' where the type
    //  is deduced.
    //
    //
    // Concerns:
    //: 1 Invoking 'make_optional' creates an 'optional' with the value of the
    //:   argument.
    //: 2 In a single argument 'make_optional' invocation, the value type of
    //:   the optional is deduced from the argument.
    //: 3 Argument is perfectly forwarded.
    //: 4 If the 'value_type' is allocator aware, default allocator is used to
    //:   construct the 'optional' object.
    //: 5 No unnecessary objects of 'value_type' are created.
    //
    //
    // Plan:
    //: 1 Call 'make_optional' to create an 'optional'.  As an argument to
    //:   'make_optional' use an lvalue of the desired 'value_type'.  Check the
    //:   resulting 'optional' object has the expected value. [C-1]
    //: 2 In step 1, when invoking make_value, do not specify the template
    //:   argument. [C-2]
    //: 3 Repeat steps 1-2 with an rvalue argument, with a const lvalue
    //:   reference and with a const rvalue reference. Verify the number of
    //:   invocations of copy and move value type constructors is the same as
    //:   when the optional's in_place constructor has been called. [C-3][C-5]
    //: 4 In steps 1-3, if type is allocator aware, check the resulting
    //:    optional is using the default allocator. [C-4]
    //
    // Testing:
    //      make_optional(T&&);

    if (verbose)
        printf("\nTESTING 'make_optional' FACILITY"
               "\n================================\n");

    if (veryVerbose)
        printf("\tDeduced type make_optional.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator         ta("third", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        ValWithAllocator sourceBuf(2, &oa);
        ValueType&       source      = sourceBuf.object();
        const ValueType& constSource = sourceBuf.object();

        TEST_MAKE_OPTIONAL_DEDUCED(source);
        TEST_MAKE_OPTIONAL_DEDUCED(constSource);
        TEST_MAKE_OPTIONAL_DEDUCED(MoveUtil::move(source));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make this test fail
        TEST_MAKE_OPTIONAL_DEDUCED(MoveUtil::move(constSource));
#endif  //BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase29b()
{
    // --------------------------------------------------------------------
    // TESTING 'make_optional' FACILITY
    //
    //  In this test, we are ensuring that the 'optional' created using
    //  'make_optional' facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an 'optional' is tested in constructor tests.
    //
    //  This test verifies the invocation of 'make_optional' with var args.
    //
    //
    // Concerns:
    //: 1 Invoking 'make_optional' creates an 'optional' with the value of the
    //:   arguments converted to the specified value type.
    //: 2 Arguments are perfectly forwarded.
    //: 3 No unnecessary objects of 'value_type' are created.
    //: 4 If the 'value_type' is allocator aware, default allocator is used to
    //:   construct the 'optional' object.
    //: 5 If the first argument is a 'braced-init-list', make_optional will
    //:   deduce an 'std::intializer list'
    //
    //
    // Plan:
    //: 1 Call 'make_optional' to create an 'optional'.  As an argument to
    //:   'make_optional' use an lvalue of the desired 'value_type'.  Check the
    //:   resulting 'optional' object has the expected value. [C-1]
    //: 2 Repeat step 1 with different number of arguments and with a
    //:   combination of lvalue and rvalue for each argument. Check that
    //:   the resulting object was created with the same combination of
    //:   lvalue and rvalue arguments. [C-2]
    //: 3 in steps 1-2, check there were no more copies of value type created
    //:   than when calling an in_place constructor. [C-3]
    //: 4 In steps 1-3, if type is allocator aware, check the resulting
    //:    optional is using the default allocator. [C-4]
    //: 5 Repeat steps 1-4 with an additional 'braced-init-list' as the first
    //:   argument to 'make_optional'. [C-5]
    //
    // Testing:
    //      make_optional(Args&&... args);
    //      make_optional(initializer_list<U> il, Args&&... args);

    if (verbose)
        printf("\nTESTING 'make_optional' FACILITY"
               "\n================================\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_MAKE_OPTIONAL((bsl::in_place), ());

        TEST_MAKE_OPTIONAL((bsl::in_place, MoveUtil::move(VA1)),
                           (MoveUtil::move(VA1)));

        TEST_MAKE_OPTIONAL((bsl::in_place, VA1), (VA1));

        TEST_MAKE_OPTIONAL((bsl::in_place, MoveUtil::move(VA1), VA2),
                           (MoveUtil::move(VA1), VA2));
        TEST_MAKE_OPTIONAL((bsl::in_place, VA1, MoveUtil::move(VA2)),
                           (VA1, MoveUtil::move(VA2)));

        TEST_MAKE_OPTIONAL(
               (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
               (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));

        TEST_MAKE_OPTIONAL((bsl::in_place, VA1, MoveUtil::move(VA2), VA3),
                           (VA1, MoveUtil::move(VA2), VA3));

        TEST_MAKE_OPTIONAL(
          (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4),
          (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_MAKE_OPTIONAL(
          (bsl::in_place, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)),
          (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)));

        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10),
                           (MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)),
                           (VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)));
        /*
        TEST_MAKE_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)));
    */
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose)
        printf("\tUsing 'initializer_list' argument.\n");

    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_MAKE_OPTIONAL((bsl::in_place, {1, 2, 3}), ({1, 2, 3}));

        TEST_MAKE_OPTIONAL((bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1)),
                           ({1, 2, 3}, MoveUtil::move(VA1)));

        TEST_MAKE_OPTIONAL((bsl::in_place, {1, 2, 3}, VA1), ({1, 2, 3}, VA1));

        TEST_MAKE_OPTIONAL(
                         (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1), VA2),
                         ({1, 2, 3}, MoveUtil::move(VA1), VA2));
        TEST_MAKE_OPTIONAL(
                         (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2)),
                         ({1, 2, 3}, VA1, MoveUtil::move(VA2)));

        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3)),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));

        TEST_MAKE_OPTIONAL(
                    (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3),
                    ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3));

        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_MAKE_OPTIONAL(
            (bsl::in_place,
             {1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4)),
            ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7)));
        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8)));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9)));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9));

        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10),
                           ({1, 2, 3},
                            MoveUtil::move(VA1),
                            VA2,
                            MoveUtil::move(VA3),
                            VA4,
                            MoveUtil::move(VA5),
                            VA6,
                            MoveUtil::move(VA7),
                            VA8,
                            MoveUtil::move(VA9),
                            VA10));
        TEST_MAKE_OPTIONAL((bsl::in_place,
                            {1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)),
                           ({1, 2, 3},
                            VA1,
                            MoveUtil::move(VA2),
                            VA3,
                            MoveUtil::move(VA4),
                            VA5,
                            MoveUtil::move(VA6),
                            VA7,
                            MoveUtil::move(VA8),
                            VA9,
                            MoveUtil::move(VA10)));
        /*
        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)));
        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11));

        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12));
        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)));

        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)));
        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13));

        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14),
                   ({1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14));
        TEST_MAKE_OPTIONAL((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)),
                   ({1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)));
    */
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

template <class TYPE>
void TestDriver<TYPE>::testCase28()
{
    // --------------------------------------------------------------------
    // TESTING TRAITS AND TYPEDEFS
    //
    //
    // Concerns:
    //: 1 That 'optional<TYPE>::value_type' is 'TYPE'
    //: 2 That 'optional<const TYPE>::value_type' is 'const TYPE'
    //: 3 That 'BloombergLP::bslma::UsesBslmaAllocator<optional<TYPE>>' and
    //:   'BloombergLP::bslmf::UsesAllocatorArgT<optional<TYPE>>' are 'false'
    //:   if 'TYPE' is a non allocator aware type.
    //: 4 That 'BloombergLP::bslma::UsesBslmaAllocator<optional<TYPE>>' and
    //:   'BloombergLP::bslmf::UsesAllocatorArgT<optional<TYPE>>' are 'true'
    //:   if 'TYPE' is an allocator aware type.
    //: 5 That 'optional<TYPE>' is trivially destructible if 'TYPE' is trivially
    //:   destructible
    //
    // Plan:
    //: 1 Verify that 'optional<TYPE>::value_type' matches 'TYPE'. [C-1]
    //: 2 Repeat step 1 using a 'const' 'value_type'. [C-2]
    //: 3 verify that both 'UsesBslmaAllocator' and
    //:   'UsesAllocatorArgT' traits for 'optional<TYPE'> are true if 'TYPE'
    //:   is allocator aware. [C-3]
    //: 4 verify that both 'UsesBslmaAllocator' and
    //:   'UsesAllocatorArgT' traits for 'optional<TYPE'> are false if 'TYPE'
    //:   is not allocator aware. [C-4]
    //: 5 verify that 'optional<TYPE'> is trivially destructible if 'TYPE' is
    //:   trivially destructible [C-5]
    //
    // Testing:
    //      optional<TYPE>::value_type;
    //      BloombergLP::bslma::UsesBslmaAllocator<optional<TYPE>>
    //      BloombergLP::bslma::UsesAllocatorArgT<optional<TYPE>>
    //      std::is_trivially_destructible<optional<TYPE>>
    if (verbose)
        printf("\nTESTNG TRAITS AND TYPEDEFS"
               "\n==========================\n");

    {
        ASSERT((bsl::is_same<typename Obj::value_type, ValueType>::value));
        ASSERT((bsl::is_same<typename ObjC::value_type, const ValueType>::value));

        ASSERT(BloombergLP::bslma::UsesBslmaAllocator<Obj>::value ==
            BloombergLP::bslma::UsesBslmaAllocator<ValueType>::value  );
        ASSERT(BloombergLP::bslmf::UsesAllocatorArgT<Obj>::value ==
            BloombergLP::bslma::UsesBslmaAllocator<ValueType>::value);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        ASSERT(std::is_trivially_destructible<Obj>::value ==
            std::is_trivially_destructible<ValueType>::value);
#else
        ASSERT(bsl::is_trivially_copyable<Obj>::value ==
            bsl::is_trivially_copyable<ValueType>::value);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase27()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=' OVERLOAD RESOLUTION
    //   This test will verify that the 'operator=' overload
    //   resolution works as expected in the presence of constructors and
    //   assignment operators of 'value_type' taking an 'optional'.
    //   In these tests we do not care about the different allocation argument
    //   policies because the allocator propagation is tested in the
    //   functionality tests for 'operator=. Here, we only care that the
    //   correct overload is selected.
    //   Similarly, we do not care about the allocator state after assignment,
    //   nor do we care about the difference in behaviour when the source and
    //   destination 'optional's are engaged/disengaged. We deliberately always
    //   use engaged 'optional's as that directly forwards to the underlying
    //   'value_type' assignment.
    //
    // Concerns:
    //: 1 If 'value_type' 'TYPE' is assignable and constructible from an
    //:   'optional' type 'OPT_TYPE', the assignment of 'OPT_TYPE' to an
    //:   'optional<TYPE>' assigns the 'OPT_TYPE' object to the 'value_type'
    //:   object of 'optional<TYPE>'. The resulting 'optional' is always
    //:   engaged.
    //: 2 If 'value_type' 'TYPE' is not assignable and constructible from an
    //:   'optional' type 'OPT_TYPE', the assignment of an engaged 'OPT_TYPE'
    //:   to an 'optional<TYPE>' assigns the 'value_type' object of the
    //:   'OPT_TYPE' object to the 'value_type' object of 'optional<TYPE>'.
    //: 3 Move assign and copy assign have the same overload resolution.
    //: 4 assigning '{}' to an 'optional' is interpreted as assigning a default
    //:   constructed 'optional' of same type. The resulting 'optional' is
    //:   disengaged.
    //: 5 Assigning a 'nullopt_t' object to an 'optional' object is always
    //:   interpreted as assigning a disengaged 'optional' of same type.
    //: 6 'optional' types of non allocator aware 'value_type' and allocator
    //:   aware type have the same overload resolution.
    //:
    //: Plan:
    //: 1 create a source object of
    //:   'OPT_TYPE', where 'OPT_TYPE' is 'optional<TYPE>'. Assign the source
    //:   object to an object of 'optional<OPT_TYPE>'. Check the
    //:   resulting object is engaged and that its 'value_type' object matches
    //:   the source object. [C-1]
    //: 2 Assign an engaged object of 'optional<OPT_TYPE>' to another object
    //:   of 'optional<OPT_TYPE>'. Check the 'value_type' of the destination
    //:    object matches the 'value_type' of the source object. [C-2]
    //: 3 In steps 1 and 2 use rvalues. [C-3]
    //: 4 assign '{}' to an object of 'OPT_TYPE' and verify
    //:   the resulting 'OPT_TYPE' object is disengaged. [C-4]
    //: 5 assign '{}' to an object of 'optional<OPT_TYPE>' and
    //:   verify the resulting 'optional<OPT_TYPE>' object is disengaged. [C-4]
    //: 6 assign 'nullopt_t' object to an object of 'OPT_TYPE'
    //:   and verify the resulting 'OPT_TYPE' object is disengaged. [C-5]
    //: 7 assign 'nullopt_t' object to an object of
    //:   'optional<OPT_TYPE>' and  verify the resulting 'optional<OPT_TYPE>'
    //:   object is disengaged. [C-5]
    //: 8 execute the test with an allocator aware TYPE. [C-6]
    //
    // Testing:
    //
    //   optional& operator= overload set
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=' OVERLOAD RESOLUTION"
               "\n=======================================\n");

    if (veryVerbose)
        printf("\tUsing non allocator aware 'value_type'.\n");
    {
        typedef bsl::optional<Obj>      OPT_TYPE;

        ValueType val;
        Obj source;
        OPT_TYPE  destination;
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());

        destination.reset();
        source.emplace(val);
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == source.value());

        CObj constSource;
        destination.reset();
        destination = constSource;
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        source = {};
        ASSERT(!source.has_value());
        destination = {};
        ASSERT(!destination.has_value());
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

        source.emplace(val);
        destination = MoveUtil::move(source);
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == val);

        source = val;
        ASSERT(source.has_value());
        ASSERT(source.value() == val);

        destination.reset();
        destination = val;
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == source.value());

        source = bsl::nullopt;
        ASSERT(!source.has_value());
        destination = bsl::nullopt;
        ASSERT(!destination.has_value());

    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase26()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED 'in_place_t' CONSTRUCTOR
    //   This test will verify that the allocator extended 'initializer_list'
    //   'in_place_t' constructor works as expected.
    //
    //: 1 Calling 'in_place_t' constructor creates an engaged 'optional' whose
    //:   'value_type' object is created using the constructors arguments.
    //: 2 Arguments to the 'in_place_t' constructor are forwarded in correct
    //:   order to the 'value_type' object's constructor.
    //: 3 Arguments to the 'in_place_t' constructor are perfect forwarded to
    //:   the 'value_type' object's constructor, that is, rvalue argument is
    //:   forwarded as an rvalue, and lvalue argument is forwarded as an lvalue
    //:   to the 'value_type' object's constructor
    //: 4 The newly created 'optional' object will use the allocator
    //:   specified in the constructor call.
    //: 5 If a braced init list is used for the first argument after the
    //:   'in_place' tag, 'optional' will deduce an initializer list and
    //:   forward it to the 'value_type'.
    //: 6 No unnecessary copies of the 'value_type' are created
    //
    //
    // Plan:
    //: 1 Using a non allocator aware 'value_type' TYPE, create an object of
    //:   'TYPE' using a set of arguments 'ARGS'. Create an 'optional' object
    //:   of 'value_type' 'TYPE' by invoking the 'in_place_t' constructor with
    //:   the same arguments. Check that the value of the
    //:   'value_type' object in the 'optional' object matches the value of the
    //:   'TYPE' object. [C-1]
    //: 2 in step 1 use different number of constructor arguments [C-2].
    //: 3 in step 1 and 2, use a mixture of rvalue and lvalue
    //:   arguments such that each argument position is tested with an
    //:   rvalue and an lvalue. Check that the rvalue arguments have been
    //:   moved from. [C-3]
    //: 4 Check that the allocator specified in the constructor call was used
    //:   to construct the 'value_type' object. [C-4]
    //: 5 Repeat steps 1-4 with an additional initializer_list argument. [C-5]
    //: 6 In steps 1-5, check no unnecessary copies of the TYPE object have
    //:   been created. [C-6]
    //
    // Plan:
    //
    //
    // Testing:
    //
    //   void optional(allocator_arg, allocator, in_place_t, Args&&...);
    //   void optional(allocator_arg, allocator, in_place_t,
    //                 std::initializer_list<U>, Args&&...);
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING ALLOCATOR EXTENDED 'in_place_t' CONSTRUCTOR "
               "\n===================================================\n");

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    if (veryVerbose)
            printf("\tUsing 'ValueType' argument.\n");
    {
        ValWithAllocator srcBuffer(&da);
        ValueType&  source = srcBuffer.object();
        const ValueType&  csource = srcBuffer.object();

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, source),
            (source, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, csource),
            (csource, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(source)),
            (MoveUtil::move(source), &ta),
            &oa);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make this
            // test fail
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(csource)),
            (MoveUtil::move(csource), &ta),
            &oa);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }

    if (veryVerbose)
            printf("\tUsing variadic arguments.\n");

    {
        TEST_IN_PLACE_CONSTRUCT(
          (bsl::allocator_arg, &oa, bsl::in_place), (&ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(VA1)),
            (MoveUtil::move(VA1), &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, VA1), (VA1, &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, MoveUtil::move(VA1), VA2),
            (MoveUtil::move(VA1), VA2, &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, VA1, MoveUtil::move(VA2)),
            (VA1, MoveUtil::move(VA2), &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3)),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3),
                                (VA1, MoveUtil::move(VA2), VA3, &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4)),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &ta),
            &oa);
        /*
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6),
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                  (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10),,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13,
                   &ta), &oa);
    */
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    }
    if (veryVerbose)
            printf("\tUsing 'initializer_list' argument.\n");

    {
          TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, {1, 2, 3}),
            (std::initializer_list<int>{1, 2, 3}, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1)),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg, &oa, bsl::in_place, {1, 2, 3}, VA1),
            (std::initializer_list<int>{1, 2, 3}, VA1, &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             {1, 2, 3},
             MoveUtil::move(VA1),
             VA2),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), VA2, &ta),
            &oa);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::allocator_arg,
             &oa,
             bsl::in_place,
             {1, 2, 3},
             VA1,
             MoveUtil::move(VA2)),
            (std::initializer_list<int>{1, 2, 3}, VA1, MoveUtil::move(VA2), &ta),
            &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 &ta),
                                &oa);

        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &ta),
                                &oa);
        TEST_IN_PLACE_CONSTRUCT((bsl::allocator_arg,
                                 &oa,
                                 bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 &ta),
                                &oa);
        /*
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6),
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                  (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10),,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11,
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                   &ta), &oa);

        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13),
                   &ta), &oa);
        TEST_IN_PLACE_CONSTRUCT(
                   (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13,
                   &ta), &oa);
    */
    #endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase25()
{
    // --------------------------------------------------------------------
    // TESTING 'in_place_t' CONSTRUCTOR
    //   This test will verify that the 'in_place_t' constructor works
    //   as expected.
    //
    // Concerns:
    //: 1 Calling 'in_place_t' constructor creates an engaged 'optional' whose
    //:   'value_type' object is created using the constructors arguments.
    //: 2 Arguments to the 'in_place_t' constructor are forwarded in correct
    //:   order to the 'value_type' object's constructor.
    //: 3 Arguments to the 'in_place_t' constructor are perfect forwarded to
    //:   the 'value_type' object's constructor, that is, rvalue argument is
    //:   forwarded as an rvalue, and lvalue argument is forwarded as an lvalue
    //:   to the 'value_type' object's constructor
    //: 4 If the type uses an allocator and no allocator is provided, the
    //:   'optional' object will use the default allocator.
    //: 5 If a braced init list is used for the first argument after the
    //:   'in_place' tag, 'optional' will deduce an initializer list and
    //:   forward it to the 'value_type'.
    //: 6 No unnecessary copies of the 'value_type' are created
    //
    //
    // Plan:
    //: 1 Using a non allocator aware 'value_type' TYPE, create an object of
    //:   'TYPE' using a set of arguments 'ARGS'. Create an 'optional' object
    //:   of 'value_type' 'TYPE' by invoking the 'in_place_t' constructor with
    //:   the same arguments. Check that the value of the
    //:   'value_type' object in the 'optional' object matches the value of the
    //:   'TYPE' object. [C-1]
    //: 2 in step 1 use different number of constructor arguments [C-2].
    //: 3 in step 1 and 2, use a mixture of rvalue and lvalue
    //:   arguments such that each argument position is tested with an
    //:   rvalue and an lvalue. Check that the rvalue arguments have been
    //:   moved from. [C-3]
    //: 4 If 'value_type' is allocator aware, check that the default allocator
    //:   was used to construct the 'value_type' object. [C-4]
    //: 5 Repeat steps 1-4 with an additional initializer_list argument. [C-5]
    //: 6 In steps 1-5, check no unnecessary copies of the TYPE object have
    //:   been created. [C-6]
    //
    // Testing:
    //
    //   void optional(in_place_t, Args&&...);
    //   void optional(in_place_t, std::initializer_list<U>, Args&&...);
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'in_place_t' CONSTRUCTOR "
               "\n================================\n");

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose)
            printf("\tUsing 'ValueType' argument.\n");
    {
        ValWithAllocator srcBuffer(&oa);
        ValueType&  source = srcBuffer.object();
        const ValueType&  csource = srcBuffer.object();

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, source),
            (source, &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, csource),
            (csource, &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, MoveUtil::move(source)),
            (MoveUtil::move(source), &oa),
            &da);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make this
            // test fail
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, MoveUtil::move(csource)),
            (MoveUtil::move(csource), &oa),
            &da);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }

    if (veryVerbose)
            printf("\tUsing variadic arguments.\n");

    {

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place), (&oa), &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, MoveUtil::move(VA1)), (MoveUtil::move(VA1), &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, VA1), (VA1, &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, MoveUtil::move(VA1), VA2),
                                (MoveUtil::move(VA1), VA2, &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, VA1, MoveUtil::move(VA2)),
                                (VA1, MoveUtil::move(VA2), &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, VA1, MoveUtil::move(VA2), VA3),
                                (VA1, MoveUtil::move(VA2), VA3, &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4),
            (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &oa),
            &da);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place,
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             VA5),
            (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &oa),
            &da);
        /*
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9,
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11,
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13,
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14),
                   (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)),
                   (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14),
                    &oa), &da);
    */
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    if (veryVerbose)
            printf("\tUsing 'initializer_list' argument.\n");

    {
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1, 2, 3}),
                                (std::initializer_list<int>{1, 2, 3}, &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1)),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1, 2, 3}, VA1),
                                (std::initializer_list<int>{1, 2, 3}, VA1, &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, MoveUtil::move(VA1), VA2),
            (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), VA2, &oa),
            &da);
        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2)),
            (std::initializer_list<int>{1, 2, 3}, VA1, MoveUtil::move(VA2), &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT(
            (bsl::in_place, {1, 2, 3}, VA1, MoveUtil::move(VA2), VA3),
            (std::initializer_list<int>{1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             &oa),
            &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 &oa),
                                &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4)),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5)),
                                (std::initializer_list<int>{1, 2, 3},
                                 MoveUtil::move(VA1),
                                 VA2,
                                 MoveUtil::move(VA3),
                                 VA4,
                                 MoveUtil::move(VA5),
                                 &oa),
                                &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place,
                                 {1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5),
                                (std::initializer_list<int>{1, 2, 3},
                                 VA1,
                                 MoveUtil::move(VA2),
                                 VA3,
                                 MoveUtil::move(VA4),
                                 VA5,
                                 &oa),
                                &da);
        /*
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9,
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11,
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13)),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13),
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13,
                   &oa), &da);

        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14),
                   (std::initializer_list<int>{1,2,3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4,
                    MoveUtil::move(VA5), VA6, MoveUtil::move(VA7), VA8,
                    MoveUtil::move(VA9), VA10, MoveUtil::move(VA11), VA12,
                    MoveUtil::move(VA13), VA14,
                   &oa), &da);
        TEST_IN_PLACE_CONSTRUCT((bsl::in_place, {1,2,3},
                    VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14)),
                   (std::initializer_list<int>{1,2,3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4),
                    VA5, MoveUtil::move(VA6), VA7, MoveUtil::move(VA8),
                    VA9, MoveUtil::move(VA10), VA11, MoveUtil::move(VA12),
                    VA13, MoveUtil::move(VA14),
                    &oa), &da);
    */
    }
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}


template <class TEST_TYPE, class OPT_TYPE1, class OPT_TYPE2>
void testCase24_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'swap' METHOD
    //
    // Concerns:
    //: 1 Swap of two disengaged objects is a no-op,
    //: 2 Swap of an engaged and a disengaged 'optional' moves the value
    //:   from the engaged object to another without calling swap for the
    //:   'value_type'.
    //: 3 Swap of two engaged objects calls swap for the 'value_type'.
    //: 4 Behaviour is the same for 'swap' member funciton, and 'swap' free
    //:   function
    //: 5 Behaviour is the same for allocator aware an non allocator aware
    //:   'value_type's.
    //
    // Plan:
    //: 1 Using a non allocator aware 'value_type', call 'swap' free
    //:   function with two disengaged 'optional' objects and, for concern 1,
    //:   verify 'value_type' 'swap' has not been called.
    //: 2 Using a non allocator aware 'value_type', call 'swap 'free
    //:   function with an engaged 'optional' object and a disengaged
    //:   'optional' object. For concern 2, verify the disengaged 'optional'
    //:   is now engaged with a value from the other object, and that the other
    //:   object is now disengaged. Verify that the 'value_type' 'swap' has not
    //:   been called.
    //: 3 Using a non allocator aware 'value_type', call swap free function
    //:   with two engaged 'optional' objects and, for concern 2, verify
    //:   'value_type' 'swap' has been called.
    //: 4 Repeat steps 1-3, and for concern 4, use the 'swap' member function.
    //: 5 Repeat steps 1-4, and for concern 5, use an allocator aware 'TYPE'.
    //
    //
    // Testing:
    //   void swap(optional<TYPE>& other);
    //   void swap(optional<TYPE>& lhs,optional<TYPE>& rhs);
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'swap' METHOD"
               "\n=====================\n");

    using bsl::swap;
    {
        OPT_TYPE1 a;
        OPT_TYPE2 b;

        TEST_TYPE::swapReset();
        swap(a, b);

        ASSERT(!TEST_TYPE::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        TEST_TYPE::swapReset();
        a.swap(b);

        ASSERT(!TEST_TYPE::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());
    }
    {
        TEST_TYPE obj1(1);
        TEST_TYPE obj2(2);

        const TEST_TYPE Zobj1(obj1);
        const TEST_TYPE Zobj2(obj2);

        OPT_TYPE1 a = obj1;
        OPT_TYPE2 b = obj2;
        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        TEST_TYPE::swapReset();
        ASSERT(!TEST_TYPE::swapCalled());
        swap(a, b);
        ASSERT(TEST_TYPE::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        TEST_TYPE::swapReset();
        ASSERT(!TEST_TYPE::swapCalled());
        a.swap(b);
        ASSERT(TEST_TYPE::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);
    }
    {
        OPT_TYPE1 nonNullObj(TEST_TYPE(10));
        OPT_TYPE1 nonNullObjCopy(nonNullObj);
        OPT_TYPE2 nullObj;

        TEST_TYPE::swapReset();
        swap(nonNullObj, nullObj);

        ASSERT(!TEST_TYPE::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        TEST_TYPE::swapReset();
        nonNullObj.swap(nullObj);

        ASSERT(!TEST_TYPE::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase24()
{
    testCase24_imp<TEST_TYPE,
                   bsl::optional<TEST_TYPE>,
                   bsl::optional<TEST_TYPE> > ();
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING 'hashAppend'
    //
    // Concerns:
    //: 1 Hashing a value with a null value is equivalent to appending 'false'
    //:   to the hash.
    //:
    //: 2 Hashing a value with a nullable value is equivalent to appending
    //:   'true' to the hash followed by the value.
    //
    // Plan:
    //: 1 Create a null nullable value and verify that hashing it yields the
    //:   same value as hashing 'false'. [C-1]
    //:
    //: 2 Create a non-null nullable value for a series of test values and
    //:   verify that hashing it produces the same result as hashing 'true' and
    //:   then the test values themselves. [C-2]
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, NullableValue<TYPE>& input);
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'hashAppend'"
               "\n====================\n");

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    {
        ObjWithAllocator object(&oa);

        Obj&       x = object.object();
        const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        const size_t hashValue_1 = bslh::Hash<>()(X);
        const size_t hashValue_2 = bslh::Hash<>()(false);
        ASSERTV(hashValue_1, hashValue_2, hashValue_1 == hashValue_2);
    }

    {
        ObjWithAllocator object(&oa);

        Obj&       x = object.object();
        const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            x = VALUES[i];

            bslma::TestAllocatorMonitor oam(&oa);

            bslh::DefaultHashAlgorithm hasher;

            const size_t hashValue_1 = bslh::Hash<>()(X);
            hashAppend(hasher, true);
            hashAppend(hasher, VALUES[i]);
            const size_t hashValue_2 =
                static_cast<size_t>(hasher.computeHash());

            ASSERTV(hashValue_1, hashValue_2, hashValue_2 == hashValue_1);

            ASSERT(oam.isInUseSame());
            ASSERT(0 == da.numBlocksInUse());

            x.reset();
        }
        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }
}

template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE>
void TestDriver<TYPE>::testCase22_imp()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION FROM A VALUE
    //   This test will verify that the allocator extended construction from
    //   a value works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from a 'value_type' object creates an
    //:   engaged 'optional' with the value of the source object.
    //: 2 The allocator provided in the constructor is the allocator used for
    //:   the newly created 'optional'.
    //: 3 Move construction of the 'value_type' is used when possible.
    //: 4 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //
    //: 1 Create a value object and use it as an initialization object for an
    //:   'optional' of that 'value_type'. Check the constructed 'optional'
    //:   object is engaged and contains the value of the object used to
    //:   initialise it. [C-1]
    //: 2 In step 1, check the allocator of the new 'optional' object is the
    //:   allocator used in the call to the constructor [C-2]
    //: 3 In steps 1-2, check that no unnecessary copies of the 'value_type'
    //:   are created by comparing the number of copy/move constructors invoked
    //:   to creating an instance of 'value_type' object from the source object
    //:   [C-4]
    //: 4 Repeat steps 1-3 using a const source object, an rvalue source object
    //:   , and a const rvalue source object. [C-4]
    //
    // Testing:
    //
    //   optional(bsl::allocator_arg, allocator_type, const ANY_TYPE&)
    //   optional(bsl::allocator_arg,
    //            allocator_type,
    //            BloombergLP::bslmf::MovableRef<ANY_TYPE>)
    // --------------------------------------------------------------------

    if (verbose)
        printf(
            "\nTESTING ALLOCATOR EXTENDED COPY/MOVE CONSTRUCTION FROM A VALUE"
            "\n=============================================================="
            "\n");

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::optional<DEST_TYPE> DEST_OPT_TYPE;
        {
            typedef bslalg::ConstructorProxy<SRC_TYPE> SourceWithAllocator;
            SourceWithAllocator                        sourceBuffer(22, &oa);
            SRC_TYPE&       source  = sourceBuffer.object();
            const SRC_TYPE& csource = sourceBuffer.object();

            TEST_EXT_COPY_FROM_VALUE(source);
            TEST_EXT_COPY_FROM_VALUE(csource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_EXT_COPY_FROM_VALUE(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            TEST_EXT_MOVE_FROM_VALUE(source);
        }
    }
}
template <class TYPE>
void TestDriver<TYPE>::testCase22()
{
    testCase22_imp<TYPE, TYPE>();
    testCase22_imp<TYPE, int>();
    testCase22_imp<const TYPE, TYPE>();
    testCase22_imp<const TYPE, int>();
}

template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE>
void TestDriver<TYPE>::testCase21_imp()
{
    // --------------------------------------------------------------------
    // TESTING COPY/MOVE CONSTRUCTION FROM A VALUE
    //   This test will verify that the copy construction from a value
    //   works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from a 'value_type' object creates an
    //:   engaged 'optional' with the value of the source object.
    //: 2 If no allocator is provided and the 'value_type' uses allocator, the
    //:   default allocator is used for the newly created 'optional'.
    //: 3 Move construction of the 'value_type' is used when possible.
    //: 4 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //
    //: 1 Create a value object and use it as an initialization object for an
    //:   'optional' of that 'value_type'. Check the constructed 'optional'
    //:   object is engaged and contains the value of the object used to
    //:   initialise it. [C-1]
    //: 2 If 'value_type' is allocator aware, check the allocator of the new
    //:   'optional' object is the default allocator [C-2]
    //: 3 In steps 1-2, check that no unnecessary copies of the 'value_type'
    //:   are created by comparing the number of copy/move constructors invoked
    //:   to creating an instance of 'value_type' object from the source object
    //:   [C-4]
    //: 4 Repeat steps 1-3 using a const source object, an rvalue source object
    //:   , and a const rvalue source object. [C-4]
    //
    // Testing:
    //
    //   optional(const ANY_TYPE&);
    //   optional(BloombergLP::bslmf::MovableRef<ANY_TYPE>)
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING COPY/MOVE CONSTRUCTION FROM A VALUE "
               "\n===========================================\n");

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bslalg::ConstructorProxy<SRC_TYPE> SourceWithAllocator;
            SourceWithAllocator                        sourceBuffer(21, &oa);
            SRC_TYPE&       source  = sourceBuffer.object();
            const SRC_TYPE& csource = sourceBuffer.object();

            TEST_COPY_FROM_VALUE(source);
            TEST_COPY_FROM_VALUE(csource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_COPY_FROM_VALUE(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            TEST_MOVE_FROM_VALUE(source);
        }
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase21()
{
    testCase21_imp<TYPE, TYPE>();
    testCase21_imp<TYPE, int>();
    testCase21_imp<const TYPE, TYPE>();
    testCase21_imp<const TYPE, int>();
}

template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE>
void TestDriver<TYPE>::testCase20_imp()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED COPY CONSTRUCTION
    //   This test will verify that the copy construction works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from an engaged 'optional' of the same
    //    type creates an engaged 'optional' where the 'value_type' object is
    //:   copy constructed from the 'value_type' object of the original
    //:   'optional' object. The original is not modified.
    //: 2 Constructing an 'optional' from a disengaged 'optional' of the same
    //    type creates a disengaged 'optional'. The original is not modified.
    //: 3 If allocator extended version of copy constructor is used, the
    //:   allocator passed to the constructor is the allocator of the newly
    //:   created 'optional'.
    //: 4 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of a non allocator aware 'value_type'.
    //:   Use the created object to copy initialize another 'optional'
    //:   object of the same type. For concern 1, check the value of the new
    //:   object and the value of the original object match.
    //: 2 Bind a 'const' reference to the 'original' object. Using the const
    //:   reference, copy initialize another 'optional' object of the same
    //:   type. For concern 1, check the value of the new object and the value
    //:   of the original object match.
    //: 3 Repeat steps 1 and 2 using a disengaged 'optional' object as the
    //:   source object. For concern 2, check that both the source and
    //:   destination object are disengaged.
    //: 4 For concern 3, execture steps 1-3 using an allocator aware type and
    //:   an allocator extended copy constructo, and verify that the allocator
    //:   of the newly created 'optional' object is the allocator provided to
    //:   the copy constructor.
    //: 5 In steps 1-4, for concern 4, check that no unnecessary copies of the
    //:   'value_type' have been created.
    //
    // Testing:
    //
    //   optional(bsl::allocator_arg, allocator_type, const optional&)
    //   optional(bsl::allocator_arg, allocator_type,
    //            BloombergLP::bslmf::MovableRef<optional>)
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING ALLOCATOR EXTENDED COPY CONSTRUCTION"
               "\n============================================\n");

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::optional<DEST_TYPE> DEST_OPT_TYPE;
        {
            typedef bsl::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_EXT_COPY_FROM_EMPTY_OPT(source);
            TEST_EXT_COPY_FROM_EMPTY_OPT(csource);
            TEST_EXT_MOVE_FROM_EMPTY_OPT(source);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_EXT_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            source.emplace(3);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(source);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(csource);
            TEST_EXT_MOVE_FROM_ENGAGED_OPT(source);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            source.emplace(3);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            typedef std::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_EXT_COPY_FROM_EMPTY_OPT(source);
            TEST_EXT_COPY_FROM_EMPTY_OPT(csource);
            TEST_EXT_COPY_FROM_EMPTY_OPT(MoveUtil::move(source));
            TEST_EXT_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));

            source.emplace(3);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(source);
            TEST_EXT_COPY_FROM_ENGAGED_OPT(csource);
            TEST_EXT_MOVE_FROM_ENGAGED_OPT(source);
            source.emplace(3);
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            TEST_EXT_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase20()
{
    testCase20_imp<TYPE, TYPE>();
    testCase20_imp<TYPE, int>();
    testCase20_imp<TYPE, const TYPE>();
    testCase20_imp<TYPE, const int>();
    testCase20_imp<const TYPE, TYPE>();
    testCase20_imp<const TYPE, int>();
    testCase20_imp<const TYPE, const TYPE>();
    testCase20_imp<const TYPE, const int>();
}

template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
void TestDriver<TYPE>::testCase19_imp()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONSTRUCTION
    //   This test will verify that the copy construction works as expected.
    //
    // Concerns:
    //: 1 Constructing an 'optional' from an engaged 'optional' creates an
    //:   engaged 'optional' where the 'value_type' object is constructed
    //:   from the 'value_type' object of the original 'optional' object.
    //: 2 Constructing an 'optional' from a disengaged 'optional' of the same
    //:    type creates a disengaged 'optional'.
    //: 3 If 'value_type' is allocator aware, and the source is an rvalue
    //:   optional of same value_type, the allocator is propagated. Otherwise,
    //:   the default allocator is used.
    //: 4 The 'value_type' object in optional is move constructed when
    //:   possible.
    //: 5 No unnecessary copies of the 'value_type' are created.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of 'SRC_TYPE' and use it to create an
    //:   'optional' of 'DEST_TYPE'. Check the value of the new
    //:   object and the value of the original object match. [C-1]
    //: 2 Bind a 'const' reference to the 'original' object. Using the const
    //:   reference, copy initialize another 'optional' object of the same
    //:   type. [C-4]
    //: 3 Repeat steps 1 and 2 using a disengaged 'optional' object as the
    //:   source object. Check that the created 'optional' object is
    //:   disengaged. [C-2]
    //: 4 If the 'value_type' is allocator aware, verify that the allocator of
    //:   the newly created 'optional' object is the default allocator. [C-3]
    //: 5 Repeat steps 1-3 using an rvalue for source. Check that the move
    //:   constructors is invoked where necessary. [C-4]
    //: 6 In step 5, if the 'value_type' is allocator aware, verify that the
    //:   allocator is propagated as needed. [C-3]
    //: 7 In steps 1-6, check that no unnecessary copies of the 'value_type'
    //:   have been created. [C-5]
    //
    // Testing:
    //
    //   optional(const optional& )
    //   optional(BloombergLP::bslmf::MovableRef<optional> )
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING COPY CONSTRUCTION "
               "\n=========================\n");

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bsl::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_COPY_FROM_EMPTY_OPT(source);
            TEST_COPY_FROM_EMPTY_OPT(csource);
            TEST_MOVE_FROM_EMPTY_OPT(source, PROPAGATE_ON_MOVE);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make these
            // tests fail
            TEST_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

            source.emplace(3);
            TEST_COPY_FROM_ENGAGED_OPT(source);
            TEST_COPY_FROM_ENGAGED_OPT(csource);
            // extracted to testCase19_imp_libgccbug
            // TEST_MOVE_FROM_ENGAGED_OPT(source, PROPAGATE_ON_MOVE);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            source.emplace(3);
            TEST_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            typedef std::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();
            const SRC_OPT_TYPE& csource = sourceBuffer.object();

            TEST_COPY_FROM_EMPTY_OPT(source);
            TEST_COPY_FROM_EMPTY_OPT(csource);
            TEST_MOVE_FROM_EMPTY_OPT(source, false);
            TEST_COPY_FROM_EMPTY_OPT(MoveUtil::move(csource));

            source.emplace(3);
            TEST_COPY_FROM_ENGAGED_OPT(source);
            TEST_COPY_FROM_ENGAGED_OPT(csource);
            // extracted to testCase19_imp_libgccbug
            //TEST_MOVE_FROM_ENGAGED_OPT(source, false);
            source.emplace(3);
            // we call copy macro here because a move from a 'const optional'
            // should trigger a copy constructor
            TEST_COPY_FROM_ENGAGED_OPT(MoveUtil::move(csource));
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    }
}
template <class TYPE>
template <class DEST_TYPE, class SRC_TYPE, bool PROPAGATE_ON_MOVE>
void TestDriver<TYPE>::testCase19_imp_libgccbug()
{
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            typedef bsl::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();

            source.emplace(3);
            TEST_MOVE_FROM_ENGAGED_OPT(source, PROPAGATE_ON_MOVE);
        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        {
            typedef std::optional<SRC_TYPE>                SRC_OPT_TYPE;
            typedef bslalg::ConstructorProxy<SRC_OPT_TYPE> SourceWithAllocator;
            SourceWithAllocator                            sourceBuffer(&oa);
            SRC_OPT_TYPE&       source  = sourceBuffer.object();


            source.emplace(3);
            TEST_MOVE_FROM_ENGAGED_OPT(source, false);
        }
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    }
}

#if (BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY &&  \
     BSLS_LIBRARYFEATURES_STDCPP_GNU && \
     (!defined(_GLIBCXX_RELEASE) || (defined(_GLIBCXX_RELEASE) &&\
                                     _GLIBCXX_RELEASE<8)))
#define STD_OPTIONAL_LIBCPP_BUG 1
#endif

template <class TYPE>
void TestDriver<TYPE>::testCase19()
{
    testCase19_imp<TYPE, TYPE, true>();
    testCase19_imp<TYPE, int, false>();
    testCase19_imp<TYPE, const TYPE, false>();
    testCase19_imp<TYPE, const int, false>();
    testCase19_imp<const TYPE, TYPE, true>();
    testCase19_imp<const TYPE, int, false>();
    testCase19_imp<const TYPE, const TYPE, true>();
    testCase19_imp<const TYPE, const int, false>();

    testCase19_imp_libgccbug<TYPE, TYPE, true>();
    testCase19_imp_libgccbug<TYPE, int, false>();
    testCase19_imp_libgccbug<TYPE, const TYPE, false>();
    testCase19_imp_libgccbug<TYPE, const int, false>();
    testCase19_imp_libgccbug<const TYPE, TYPE, true>();
    testCase19_imp_libgccbug<const TYPE, int, false>();
#ifndef STD_OPTIONAL_LIBCPP_BUG
    testCase19_imp_libgccbug<const TYPE, const TYPE, true>();
#endif
    testCase19_imp_libgccbug<const TYPE, const int, false>();
}

void testCase18()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(optional_type)' MEMBER FUNCTION
    //   This test will verify that the 'operator=(rhs)', where 'rhs' is an
    //   optional type, member function works as expected. These test require
    //   compilation failures and will not run by default.
    //
    // Concerns:
    //: 1 'operator=(rhs)', where 'rhs' is an 'optional' type whose
    //:   'value_type' is not assignable to the 'value_type' of the target
    //:    'optional', can not be called.
    //: 2 'operator=(rhs)', where 'rhs' is an 'optional' type whose
    //:   'value_type' is not convertible to target 'optional''s 'value_type',
    //:    can not be called.
    //
    // Plan:
    //: 1 Create an 'optional' object of non allocator aware 'value_type' as
    //:   the target object. For concern 1, verify that an 'optional' object of
    //:   'value_type' which is convertible, but not assignable to target
    //:   object 'value_type' can not be assigned to the target object. Note
    //:   that this test requires compilation errors and needs to be enabled
    //:   and checked manually.
    //: 2 For concern 2, verify that an 'optional' object of 'value_type' which
    //:   is assignable, but not convertible to target object's 'value_type'
    //:   can not be assigned to the target object. Note that this test
    //:   requires compilation errors and needs to be enabled and checked
    //:   manually.
    //: 3 Repeat steps 1 and 2 with an 'optional' of allocator aware
    //:   'value_type' as the target object.
    //
    //
    // Testing:
    //
    //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=(optional_type)' MEMBER FUNCTION"
               "\n==================================================\n");
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_OPT

    if (veryVerbose)
        printf("\tUsing 'MyClass1b'.\n");
    {
        typedef MyClass1b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX  = ValueType(0);
            bsl::optional<MyClass1> mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass1c'.\n");
    {
        typedef MyClass1c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX  = ValueType(0);
            bsl::optional<MyClass1> mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2b'.\n");
    {
        typedef MyClass2b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX;
            bsl::optional<MyClass2> mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2c'.\n");
    {
        typedef MyClass2c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj                     mX;
            bsl::optional<MyClass2> mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
#endif
}

template <class TYPE>
template <class OPT_TYPE>
void TestDriver<TYPE>::testCase17_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(optional_type)' MEMBER FUNCTION
    //   This test will verify that the 'operator=(rhs)', where 'rhs' is an
    //   'optional' type, works as expected.
    //
    // Concerns:
    //: 1 That 'operator=(rhs)', where 'rhs' is a disengaged 'optional' object,
    //:   makes the target 'optional' object disengaged.
    //: 2 That 'operator=(rhs)', invoked on an engaged 'optional' object and
    //:   where 'rhs' is an engaged 'optional' object, assigns the value of the
    //:   'rhs''optional' to the value of the target 'optional' object.
    //: 3 That 'operator=(rhs)', invoked on a disengaged 'optional' object and
    //:   where 'rhs' is an engaged 'optional' object, constructs a
    // 'value_type'
    //:   object from the value of 'rhs''optional' object.
    //: 4 An assignment operator takign an 'optional' type will be used even if
    //:   'rhs' is a const qualified 'optional', or on 'optional' of a const
    //:   qualified 'value_type'.
    //: 5 For allocator aware types, the assignment from an 'optional' object
    //:   does not modify the allocator
    //: 6 Assignment from rvalues uses move assignment where available
    //: 7 Assignment to an 'optional' of const qualified 'value_type' is not
    //:   possible.
    //
    // Plan:
    //: 1 Create an engaged 'optional' of a non allocator aware 'value_type'.
    //:   Assign a disengaged 'optional' of the same type to it. For concern 1
    //:   check that the destination object is disengaged.
    //: 2 Emplace a value into the test object. Assign an engaged 'optional'
    //:   of the same type to it. For concern 2, check the value of the test
    //:   object is the same as that of the object assigned to it.
    //: 3 Assign a disengaged 'optional' of a 'value_type' convertible to test
    //:   object's 'value_type'. For concern 1, check that the destination
    //:   object is disengaged.
    //: 4 Emplace a value into the test object. Assign an engaged 'optional'
    //:   of a 'value_type' convertible to test object's 'value_type'. For
    //:   concern 2, check the value of the test object is the same as that of
    //:   the object assigned to it.
    //: 5 For concern 4, repeat steps 1-4 using a const qualified 'optional'
    //:   object as 'rhs', and using an 'optional' of const qualified value
    //:   type.
    //: 6 For concern 6, repeat steps 1-5 using an rvalue as 'rhs'. Verify the
    //:   object was moved from if rhs is a non const qualified of a non const
    //:   'value_type', and copied otherwise.
    //: 7 For concern 3, repeat steps 1-6 using a disengaged 'optional' as the
    //:   test object in each step by calling 'reset' before each assignment.
    //: 8 Repeat steps 1-7 using an 'optional' object of allocator aware type
    //:   as the test object. For concern 5, check that the test object's
    //:   allocator has not been modified.
    //: 9 For concern 7, verify that a const qualified 'optional' can not be
    //:   assigned to. Note taht this test requires compilation failures and
    //:   needs to be enabled and checked manually.
    //
    //   Repeat all of the above tests for a disengaged 'optional' as the
    //   destination. That is, call reset before each assignment.
    //
    // Testing:
    //
    //   optional& operator=(const optional&);
    //   optional& operator=( BloombergLP::bslmf::MovableRef<optional>);
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=(optional_type)' MEMBER FUNCTION"
               "\n==================================================\n");

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bslalg::ConstructorProxy<OPT_TYPE> SourceWithAllocator;
        SourceWithAllocator                        X(&da);
        OPT_TYPE&                                  source  = X.object();
        const OPT_TYPE&                            cSource = X.object();

        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(source);
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(source);
        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(MoveUtil::move(source));
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(MoveUtil::move(source));

        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(cSource);
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make these tests
        // fail
        TEST_ASSIGN_OPT_EMPTY_FROM_EMPTY(MoveUtil::move(cSource));
        TEST_ASSIGN_OPT_ENGAGED_FROM_EMPTY(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        source.emplace(3);
        TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source);
        TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source);
        TEST_MOVE_ASSIGN_OPT_EMPTY_FROM_ENGAGED(source);
        source.emplace(3);
        TEST_MOVE_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(source);
        source.emplace(3);

        TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(cSource);
        TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(cSource);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // C++03 MovableRef isn't const friendly which will make these tests
        // fail
        TEST_ASSIGN_OPT_EMPTY_FROM_ENGAGED(MoveUtil::move(cSource));
        TEST_ASSIGN_OPT_ENGAGED_FROM_ENGAGED(MoveUtil::move(cSource));
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    }
    {
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        typedef int OtherType;

        ObjC      mX = ValueType(0);
        OtherType i  = OtherType(3);
        mX           = i;
#endif  //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase17()
{
    testCase17_imp<bsl::optional<TYPE> >();
    testCase17_imp<bsl::optional<int> >();
    testCase17_imp<bsl::optional<const TYPE> >();
    testCase17_imp<bsl::optional<const int> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    testCase17_imp<std::optional<TYPE> >();
    testCase17_imp<std::optional<int> >();
    testCase17_imp<std::optional<TYPE> >();
    testCase17_imp<std::optional<int> >();
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
}
void testCase16()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(non_optional_type)' MEMBER FUNCTION
    //   This test will verify that the 'operator=(non_optional_type)' function
    //   works as expected.
    //
    // Concerns:
    //: 1 'operator=(rhs)', where rhs is not an 'optional' object, can not be
    //:    called if 'rhs' is of a type which is not assignable to
    //:    'value_type'.
    //: 2  'operator=(rhs)', where rhs is not an 'optional' object, can not be
    //:    called if 'value_type' is not constructable from 'rhs'.
    //
    // Plan:
    //
    //: 1 Create an 'optional' object of non allocator aware 'value_type'.
    //:   For concern 1, verify that lvalue and rvalue of type convertible,
    //:   but not assignable to 'value_type' can not be assigned to the
    //:   'optional' object. Note that this test requires compilation errors
    //:   and needs to be enabled and checked manually.
    //: 2 Create an 'optional' object of non allocator aware 'value_type'.
    //:   For concern 2, verify that lvalue and rvalue of type assignable,
    //:   but not convertible to 'value_type' can not be assigned to the
    //:   'optional' object. Note that this test requires compilation errors
    //:   and needs to be enabled and checked manually.
    //: 3 Repeat steps 1 and 2 with an 'optional' of allocator aware
    //:   'value_type'.
    //
    //
    // Testing:
    //
    //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=(non_optional_type)' MEMBER FUNCTION"
               "\n======================================================"
               "\n");
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT

    if (veryVerbose)
        printf("\tUsing 'MyClass1b'.\n");
    {
        typedef MyClass1b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX  = ValueType(0);
            MyClass1 mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass1c'.\n");
    {
        typedef MyClass1c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX  = ValueType(0);
            MyClass1 mc1 = MyClass1(2);

            mX = mc1;          //this should not compile
            mX = MyClass1(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2b'.\n");
    {
        typedef MyClass2b                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX;
            MyClass2 mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
    if (veryVerbose)
        printf("\tUsing 'MyClass2c'.\n");
    {
        typedef MyClass2c                ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj      mX;
            MyClass2 mc1 = MyClass2(2);

            mX = mc1;          //this should not compile
            mX = MyClass2(0);  // this should not compile
        }
    }
#endif
}

template <class TYPE>
void TestDriver<TYPE>::testCase15()
{
    // --------------------------------------------------------------------
    // TESTING 'operator=(non_optional_type)' FUNCTIONALITY
    //   This test will verify that the 'operator=(rhs)' member function, where
    //   'rhs' is not an 'optional' type, works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator=(rhs)', where 'rhs' is not of an 'optional' type,
    //:   or an engaged 'optional' assigns 'rhs' to the 'value_type' object.
    //: 2 Calling 'operator=(rhs)', where 'rhs' is not of an 'optional' type,
    //:   or a disengaged 'optional' creates a 'value_type' object initialized
    //:   with 'rhs'.
    //: 3 For allocator aware types, the assignment to a disengaged 'optional'
    //:   uses the stored allocator.
    //: 4 Assignment of rvalues uses move assignment/construction where
    //:   available.
    //: 5 'optional' object of const qualified 'value_type' can not be assigned
    //:   to.
    //
    // Plan:
    //: 1 Create an engaged 'optional' object of non allocator aware type.
    //:   For concern 1, check that assignment from 'value_type', from const
    //:   qualified 'value_type', and from type assignable to 'value_type'
    //:   results in an 'optional' object having the value (possible converted
    //:   from) rhs.
    //: 2 Repeat step 1 using rvalue 'rhs' and, for concern 4, check 'rhs' was
    //:   moved from.
    //: 3 Create a disengaged 'optional' object of non allocator aware type.
    //:   For concern 2, check that assignment from 'value_type', from 'const'
    //:   qualified 'value_type', and from type assignable to 'value_type'
    //:   results in an 'optional' object having the value (possible converted
    //:   from) rhs.
    //: 4 Repeat step 4 using rvalue 'rhs' and, for concern 4, check 'rhs' was
    //:   moved from.
    //: 5 Repeat steps 1-4 using an 'optional' object of allocator aware type.
    //:   For concern 3, check the allocator of the resulting 'optional'
    //:   object.
    //: 6 For concern 7, verify that an 'optional' object of const qualified
    //:   'value_type' can not be assigned to. Note that this test requires
    //:   compilation errors and needs to be enabled and checked manually.
    //
    // Testing:
    //
    //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator=(non_optional_type)' FUNCTIONALITY"
               "\n===================================================="
               "\n");

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (veryVeryVerbose)
            printf("\t\tChecking assignment to an engaged "
                   "'optional'.\n");
        {
            ValWithAllocator valBuffer(1, &da);
            ValueType&       vi = valBuffer.object();
            TEST_ASSIGN_VAL_ENGAGED(vi);

            ValWithAllocator valBuffer2(2, &da);
            ValueType&       vi2 = valBuffer2.object();
            TEST_MOVE_ASSIGN_VAL_ENGAGED(vi2);
            ASSERT(vi2.value() == MOVED_FROM_VAL);

            const ValueType& cvi = valBuffer.object();
            TEST_ASSIGN_VAL_ENGAGED(cvi);
            TEST_ASSIGN_VAL_ENGAGED(MoveUtil::move(cvi));

            int i = 3;
            TEST_ASSIGN_VAL_ENGAGED(i);
            TEST_MOVE_ASSIGN_VAL_ENGAGED(i);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            // C++03 MovableRef isn't const friendly which will make this test
            // fail

            const int ci = 6;
            TEST_ASSIGN_VAL_ENGAGED(ci);
            TEST_ASSIGN_VAL_ENGAGED(MoveUtil::move(ci));
#endif
        }
        if (veryVeryVerbose)
            printf("\t\tChecking assignment to a disengaged"
                   " 'optional'.\n");
        {
            ValWithAllocator valBuffer(1, &da);
            ValueType&       vi = valBuffer.object();
            TEST_ASSIGN_VAL_EMPTY(vi);

            ValWithAllocator valBuffer2(2, &da);
            ValueType&       vi2 = valBuffer2.object();
            TEST_MOVE_ASSIGN_VAL_EMPTY(vi2);
            ASSERT(vi2.value() == MOVED_FROM_VAL);

            const ValueType& cvi = valBuffer.object();
            TEST_ASSIGN_VAL_EMPTY(cvi);
            TEST_ASSIGN_VAL_EMPTY(MoveUtil::move(cvi));

            int i = 3;
            TEST_ASSIGN_VAL_EMPTY(i);

            const int ci = 6;
            TEST_ASSIGN_VAL_EMPTY(ci);
            TEST_ASSIGN_VAL_EMPTY(MoveUtil::move(ci));
        }
        {
            ObjC      mX = ValueType(0);
            ValueType vi = ValueType(1);
#ifdef BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
            mX = vi;  // this should not compile
#endif                //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        }
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase14_imp()
{
    // --------------------------------------------------------------------
    // TESTING operator=(nullopt_t) MEMBER FUNCTION
    //   This test will verify that the operator=(nullopt_t) member function
    //   works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator=(nullopt_t)' on a disengaged 'optional' leaves the
    //:   'optional' disengaged.
    //: 2 Calling 'operator=(nullopt_t)' on an engaged 'optional' makes the
    //:   'optional' disengaged.
    //: 3 For an allocator aware 'value_type', calling 'operator=(nullopt_t)'
    //:   does not modify the allocator.
    //: 4 'operator=(nullopt_t)' can be called on a non const qualified
    //:   'optional' of a const qualified 'value_type'.
    //
    //
    // Plan:
    //: 1 Create a disengaged 'optional' of a non allocator aware 'value_type'.
    //:   Call operator=(nullopt_t) and, for concern 1, verify that the
    // 'ptional'
    //:   object is still disengaged
    //: 2 Emplace a value in the 'optional'. Call operator=(nullopt_t) and, for
    //:   concern 2, check that 'optional' is disengaged'
    //: 3 Repeat steps 1 and 2 with an allocator aware 'value_type', and for
    //:   concern 3, verify the allocator has not changed.
    //: 4 For concern 4, repeat steps 1 - 3, using a const qualified value
    //:   type.
    //
    // Testing:
    //
    //   operator=(nullopt_t)
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING operator=(nullopt_t) MEMBER FUNCTION "
               "\n============================================\n");

    {
        Obj mX;
        mX = bsl::nullopt;
        ASSERT(!mX.has_value());

        mX.emplace(ValueType());
        mX = bsl::nullopt;
        ASSERT(!mX.has_value());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase14()
{
    TestDriver<TYPE>::testCase14_imp();
    TestDriver<const TYPE>::testCase14_imp();
}

template <class TYPE>
void testCase13_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'emplace' METHOD
    //   This test will verify that the 'emplace' method works as expected.
    //
    // Concerns:
    //: 1 Calling 'emplace' on a non-engaged 'optional' creates a 'value_type'
    //:   object by invoking the constructor with the 'emplace' arguments.
    //: 2 Calling 'emplace' on an engaged 'optional' replaces the 'value_type'
    //:   object with a new one created by invoking the constructor with the
    //:   'emplace' arguments.
    //: 3 Calling 'emplace' with no arguments creates a default constructed
    //:   'value_type' object.
    //: 4 If 'value_type' is allocator aware, 'emplace' invokes the allocator
    //:   extended constructor using the 'optional''s allocator.
    //: 5 If 'value_type' is allocator aware, calling 'emplace' does not modify
    //:    the allocator, even when called with an rvalue 'value_type' argument
    //: 6 'Emplace' can be used with a const qualified 'value_type'.
    //: 7 'Emplace' can not be called on a const qualified 'optional'.
    //: 8 There are no unnecessary 'value_type' copies created
    //
    // Plan:
    //: 1 Create a non engaged 'optional' object.  Call 'emplace' and, for
    //:   concern 1, verify that the 'optional' object contains the expected
    //:   value.
    //: 2 For concern 2, call 'emplace' method on the engaged 'optional' object
    //:   and verify the value of the 'optional' object has changed.
    //: 3 In step 1, for concern 3, use 'emplace' method that takes no
    //:   arguments and verify the object is default constructed.
    //: 4 For concern 4, in steps 1-3, if 'value_type' is allocator aware,
    //:   verify the allocator used for the constructed value is as
    //:   expected.
    //: 5 For concern 5, if 'value_type' is allocator aware, call 'emplace'
    //:   with an rvalue of 'value_type' with a different allocator to the
    //:   'optional' object.  Verify the allocator has not been propagated.
    //: 6 For concern 8, in steps 1-5, verify there are no additional copies of
    //:   the 'value_type' created.
    //: 7 For concern 6, repeat steps 1-3 using a const qualified 'value_type'.
    //: 8 For concern 7, verify that 'emplace' method can not be invoked on a
    //:   const qualified 'optional'. Note that this test requires compilation
    //:   errors and needs to be enabled and checked manually.
    //
    // Testing:
    //
    //   void emplace();
    //   void emplace(Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    typedef TYPE                                ValueType;
    typedef bslalg::ConstructorProxy<ValueType> ValWithAllocator;
    typedef bsl::optional<ValueType>            Obj;

    if (verbose)
        printf("\nTESTING 'emplace' METHOD"
               "\n========================\n");

    if (veryVerbose)
        printf("\tUsing non const 'value_type'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX;

        CHECK_INSTANCES_EMPLACE_EMPTY;
        ASSERT(mX.has_value());
        ASSERT(mX->value() == 0);
        ASSERT(checkAllocator(mX, &da));
        ASSERT(checkAllocator(*mX, &da));

        ValWithAllocator valBuffer(3, &oa);
        const ValueType& val = valBuffer.object();

        CHECK_INSTANCES_EMPLACE(val);
        ASSERT(mX.has_value());
        ASSERT(mX->value() == 3);
        ASSERT(checkAllocator(mX, &da));
        ASSERT(checkAllocator(*mX, &da));

        ValWithAllocator valBuffer2(4, &oa);
        ValueType&       val2 = valBuffer2.object();

        CHECK_INSTANCES_EMPLACE(val2);
        ASSERT(mX.has_value());
        ASSERT(mX->value() == 4);
        ASSERT(checkAllocator(mX, &da));
        ASSERT(checkAllocator(*mX, &da));

        CHECK_INSTANCES_EMPLACE(6);
        ASSERT(mX.has_value());
        ASSERT(mX->value() == 6);
        ASSERT(checkAllocator(mX, &da));
        ASSERT(checkAllocator(*mX, &da));
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase13()
{
    testCase13_imp<TYPE>();
    testCase13_imp<const TYPE>();
}
template <class TYPE>
void TestDriver<TYPE>::testCase13b()
{
    // --------------------------------------------------------------------
    // TESTING 'emplace' METHOD
    //   This test will verify that the 'emplace' method works as expected.
    //
    // Concerns:
    //: 1 Calling 'emplace' with no arguments creates a default constructed
    //:   'value_type' object.
    //: 2 If 'value_type' is allocator aware, 'emplace' invokes the allocator
    //:   extended constructor using the 'optional''s allocator.
    //: 3 There are no unnecessary 'value_type' copies created
    //: 4 Variadic arguments to 'emplace' method are correctly forwarded to the
    //:   constructor arguments.
    //
    // Plan:
    //: 1 For concern 1,  call 'emplace' method that takes no arguments and
    //:   verify the value type object is default constructed.
    //: 2 For concern 2, in step 1, if the 'value_type' is allocator aware,
    //:   verify the allocator used for the constructed value is as
    //:   expected.
    //: 3 For concern 2, in steps 1-2, verify there are no additional copies of
    //:   the 'value_type' created.
    //: 4 Invoke 'emplace' method with varying number of arguments, some of
    //:   which are to be moved from. For concern 4, verify the arguments are
    //:   perfect forwarded to the constructor in the correct order.
    //: 5 In step 4, for concern 3, verify that the additional copies of the
    //:   arguments or 'value_type' have been created.
    //
    // Testing:
    //
    //   void emplace();
    //   void emplace(Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------
    if (veryVerbose)
        printf("\tTesting var args emplace .\n");
    {
        bslma::TestAllocator da("other", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        TEST_EMPLACE((&da), ());
        TEST_EMPLACE((MoveUtil::move(VA1), &da), (MoveUtil::move(VA1)));
        TEST_EMPLACE((VA1, &da), (VA1));
        TEST_EMPLACE((MoveUtil::move(VA1), VA2, &da),
                     (MoveUtil::move(VA1), VA2));
        TEST_EMPLACE((VA1, MoveUtil::move(VA2), &da),
                     (VA1, MoveUtil::move(VA2)));

        TEST_EMPLACE((MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), &da),
                     (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
        TEST_EMPLACE((VA1, MoveUtil::move(VA2), VA3, &da),
                     (VA1, MoveUtil::move(VA2), VA3));

        TEST_EMPLACE(
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4, &da),
                    (MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_EMPLACE(
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), &da),
                    (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5)));
        TEST_EMPLACE(
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5, &da),
               (VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4), VA5));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6)));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7)));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8)));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9)));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9));

        TEST_EMPLACE((MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10,
                      &da),
                     (MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10));
        TEST_EMPLACE((VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10),
                      &da),
                     (VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10)));
    }
#ifdef BSLSTL_OPTIONAL_TEST_BAD__EMPLACE
    {
        CObj bad1;
        bad1.emplace();               // this should not compile 1
        bad1.emplace(VA1);            // this should not compile 2
        bad1.emplace(VA1, VA2);       // this should not compile 3
        bad1.emplace(VA1, VA2, VA3);  // this should not compile 4
        bad1.emplace(VA1, VA2, VA3, VA4);
        // this should not compile 5

        CObjC bad2;
        bad2.emplace();               // this should not compile 6
        bad2.emplace(VA1);            // this should not compile 7
        bad2.emplace(VA1, VA2);       // this should not compile 8
        bad2.emplace(VA1, VA2, VA3);  // this should not compile 9
        bad2.emplace(VA1, VA2, VA3, VA4);
        // this should not compile 10
    }
#endif
}
template <class TYPE>
void TestDriver<TYPE>::testCase13c()
{
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    // --------------------------------------------------------------------
    // TESTING TESTING INITIALIZER LIST 'emplace' METHOD
    //   This test will verify that the initializer list 'emplace' method works
    //   as expected.
    //
    // Concerns:
    //: 1 Calling 'emplace' with only an initializer list detects the
    //:   initializer list.
    //: 2 Variadic arguments to 'emplace' method are correctly fowarded to the
    //:   constructor arguments.
    //: 3 If 'value_type' is allocator aware, 'emplace' invokes the allocator
    //:   extended constructor using the 'optional''s allocator.
    //: 4 There are no unnecessary argument type and 'value_type' copies
    //:   created
    //
    // Plan:
    //: 1 Create an 'optional' object of non allocator aware 'value_type'.
    //:   For concern 1, call 'emplace' method that takes just an initializer
    //:   list, and verify the object was constructed using an initializer list
    //:   constructor.
    //: 2 For concern 2, repeat steps 1 using varying number of arguments.
    //: 3 For concern 3, repeat steps 1-2 with an allocator aware 'value_type'
    //:   and verify the correct allocator is used
    //: 4 For concern 4, in stesp 1-3, verify no unnecessary copies of the
    //:   arguments and the 'value_type' have been created.
    //
    // Testing:
    //
    //   void emplace(std::initializer_list<U>, Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------
    if (verbose)
        printf("\nTESTING INITIALIZER LIST 'emplace' METHOD"
               "\n=========================================\n");
    if (veryVerbose)
        printf("\tTesting var args emplace .\n");
    {
        bslma::TestAllocator da("other", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3}, &da), ({1, 2, 3}));
        TEST_EMPLACE(
              (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), &da),
              ({1, 2, 3}, MoveUtil::move(VA1)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3}, VA1, &da),
                     ({1, 2, 3}, VA1));
        TEST_EMPLACE(
         (std::initializer_list<int>{1, 2, 3}, MoveUtil::move(VA1), VA2, &da),
         ({1, 2, 3}, MoveUtil::move(VA1), VA2));
        TEST_EMPLACE(
         (std::initializer_list<int>{1, 2, 3}, VA1, MoveUtil::move(VA2), &da),
         ({1, 2, 3}, VA1, MoveUtil::move(VA2)));

        TEST_EMPLACE(
            (std::initializer_list<int>{1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             &da),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      &da),
                     ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3));

        TEST_EMPLACE(
            (std::initializer_list<int>{1, 2, 3},
             MoveUtil::move(VA1),
             VA2,
             MoveUtil::move(VA3),
             VA4,
             &da),
            ({1, 2, 3}, MoveUtil::move(VA1), VA2, MoveUtil::move(VA3), VA4));
        TEST_EMPLACE(
            (std::initializer_list<int>{1, 2, 3},
             VA1,
             MoveUtil::move(VA2),
             VA3,
             MoveUtil::move(VA4),
             &da),
            ({1, 2, 3}, VA1, MoveUtil::move(VA2), VA3, MoveUtil::move(VA4)));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6)));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8)));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9)));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9));

        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10,
                      &da),
                     ({1, 2, 3},
                      MoveUtil::move(VA1),
                      VA2,
                      MoveUtil::move(VA3),
                      VA4,
                      MoveUtil::move(VA5),
                      VA6,
                      MoveUtil::move(VA7),
                      VA8,
                      MoveUtil::move(VA9),
                      VA10));
        TEST_EMPLACE((std::initializer_list<int>{1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10),
                      &da),
                     ({1, 2, 3},
                      VA1,
                      MoveUtil::move(VA2),
                      VA3,
                      MoveUtil::move(VA4),
                      VA5,
                      MoveUtil::move(VA6),
                      VA7,
                      MoveUtil::move(VA8),
                      VA9,
                      MoveUtil::move(VA10)));
    }
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
}

template <class TYPE>
void TestDriver<TYPE>::testCase12()
{
    // --------------------------------------------------------------------
    // TESTING 'operator*' FUNCTIONALITY
    //   This test will verify that the 'operator*' works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator* 'on an engaged 'optional' returns a reference to
    //:   the contained value.
    //: 2 Returned reference is 'const' qualified if the 'optional' object
    //:   is 'const' qualified, or if the 'value_type' is 'const' qualified.
    //: 3 All of the above concerns apply whether or not the 'value_type' is
    //:   allocator aware.
    //
    // Plan:
    //: 1 Create an engaged 'optional' object.  Using 'operator*', for concern
    //:   1, check the returned value matches the expected value.
    //: 2 Modify the value of the object obtained using 'operator*'. For
    //:   concern 1, check the value of the 'optional' has been modified.
    //: 3 For concern 2, check that the pointer returned from 'operator*'
    //:   is not 'const' qualified if neither the 'optional' object, nor the
    //:   'value_type' are 'const' qualified.
    //: 4 For concern 2, check that the pointer returned from 'operator*'
    //:   is 'const' qualified if the 'optional' object is 'const' qualified.
    //: 5 For concern 2, check that the pointer returned from 'operator*'
    //:   is 'const' qualified if the 'value_type' is 'const' qualified.
    //: 6 For concern3, execute the test with allocator aware and non allocator
    //:   aware 'TYPE'.
    //
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING operator* FUNCTIONALITY "
               "\n===============================\n");

    {
        Obj   mX(ValueType(4));
        CObj  cObjX(ValueType(8));
        ObjC  objcX(ValueType(9));
        CObjC cObjcX(ValueType(10));

        ASSERT((*mX).value() == 4);
        ASSERT((*cObjX).value() == 8);
        ASSERT((*objcX).value() == 9);
        ASSERT((*cObjcX).value() == 10);

        *mX = 6;
        ASSERT((*mX).value() == 6);

        ASSERT(!isConstRef(mX. operator*()));
        ASSERT(isConstRef(cObjX. operator*()));
        ASSERT(isConstRef(objcX. operator*()));
    }

    {
        Obj   mX(ValueType(4));
        CObj  cObjX(ValueType(8));
        ObjC  objcX(ValueType(9));
        CObjC cObjcX(ValueType(10));

        ASSERT((*(Obj(ValueType(4)))).value() == 4);
        ASSERT((*(CObj(ValueType(8)))).value() == 8);
        ASSERT((*(ObjC(ValueType(9)))).value() == 9);
        ASSERT((*(CObjC(ValueType(10)))).value() == 10);

        ASSERT(!isConstRef(Obj(ValueType(4)). operator*()));
        ASSERT(isConstRef(CObj(ValueType(8)). operator*()));
        ASSERT(isConstRef(ObjC(ValueType(9)). operator*()));
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase11()
{
    // --------------------------------------------------------------------
    // TESTING 'operator->' MEMBER FUNCTION
    //   This test will verify that the 'operator->' works as expected.
    //
    // Concerns:
    //: 1 Calling 'operator->' on an engaged 'optional' object returns a
    //:   pointer to the contained value object.
    //: 2 Returned pointer is 'const' qualified if the 'optional' object
    //:   is 'const' qualified, or if the 'value_type' is 'const' qualified.
    //: 3 All of the above concerns apply whether or not the 'value_type' is
    //:   allocator aware.
    //
    // Plan:
    //: 1 Create an engaged 'optional' object.  Using 'operator->', for
    //:   concern 1 check the returned value matches the expected value.
    //: 2 Assign a value to the 'optional' object through a call to
    //:   'operator->'.  For concern 1, check the value of the 'optional'
    //:   object is as expected.
    //: 3 For concern 2, check that the pointer returned from 'operator->'
    //:   is not 'const' qualified if neither the 'optional' object, nor the
    //:   'value_type' are 'const' qualified.
    //: 4 For concern 2, check that the pointer returned from 'operator->'
    //:   is 'const' qualified if the 'optional' object is 'const' qualified.
    //: 5 For concern 2, check that the pointer returned from 'operator->'
    //:   is 'const' qualified if the 'value_type' is 'const' qualified.
    //: 6 For concern 3, run the test using both allocator aware and non
    //:   allocator aware 'TYPE'
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'operator->' MEMBER FUNCTION"
               "\n====================================\n");

    {
        Obj   mX(2);
        CObj  cObjX(5);
        ObjC  objcX(ValueType(9));
        CObjC cObjcX(ValueType(10));

        ASSERT(mX->value() == 2);
        ASSERT(cObjX->value() == 5);
        ASSERT(objcX->value() == 9);
        ASSERT(cObjcX->value() == 10);

        *(mX. operator->()) = 6;
        ASSERT(mX->value() == 6);

        ASSERT(!isConstPtr(mX. operator->()));
        ASSERT(isConstPtr(cObjX. operator->()));
        ASSERT(isConstPtr(objcX. operator->()));
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase10()
{
    // --------------------------------------------------------------------
    // TESTING ALLOCATOR EXTENDED 'value_or' METHOD
    //   This test will verify that the allocator extended 'value_or' method
    //   works as expected. We will also check single argument 'value_or'
    //   method with an optional created using an allocator extended
    //   constructor.
    //   The test relies on constructors and 'emplace' method.
    //
    // Concerns:
    //: 1 Calling 'value_or' on a disengaged 'optional' object returns the
    //:   argument value converted to the 'value_type' of the 'optional'
    //:   object.
    //: 2 Calling 'value_or' on an engaged 'optional' object returns the value
    //:   in the 'optional' object.
    //: 3 In C++11, when calling value_or on an engaged 'optional' rvalue, the
    //:   returned object is created by moving from the 'value_type' object in
    //:   'optional'.
    //: 4 It is possible to call 'value_or' on a constant 'optional' object.
    //: 6 If 'value_type' is allocator aware, the allocator of the object
    //:   returned from a single argument 'value_or' method is determined by
    //:   the copy/move/conversion constructor of the 'value_type'.
    //: 7 The allocator of the object returned from the allocator extended
    //:   'value_or' method is the allocator specified in the call to
    //:   'value_or'.
    //
    // Plan:
    //
    //: 1 Create a disengaged 'optional' object.  For concern 1, call
    //:   'value_or' and check that the return object matches the argument
    //:   given to 'value_or'.
    //: 2 Emplace a value in the 'optional' object. Call 'value_or' and, for
    //:   concern 2, check that the returned value matches the value in the
    //:   'optional' object.
    //: 3 If ref qualifieres are supported, call 'value_or' on an engaged
    //:   'optional' rvalue. For concern 3, verify the resulting objects has
    //:   been created by move construction.
    //: 4 In step 1, for concern 4, call ' value_or' through a 'const'
    //:   reference to the  'optional' object.
    //: 5 For concern 6, if the 'value_type' is allocator aware, check that
    //:   the allocator of the returned object is correct.
    //: 7 Repeat steps 1-5 using an allocator extended 'value_or' method.  For
    //:   concern 7, check that the allocator of the returned object is the one
    //:   specified in the call to 'value_or'.
    //
    // Testing:
    //  TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const;
    //  TYPE value_or(bsl::allocator_arg_t,
    //                allocator_type,
    //                BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)
    //                ) const;
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING ALLOCATOR EXTENDED 'value_or' METHOD"
               "\n============================================\n");

    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj        mX(bsl::allocator_arg, &oa);
            const Obj& X = mX;

            ValWithAllocator valBuffer(5, &oa);
            const ValueType& source = valBuffer.object();

            const ValueType dest = X.value_or(source);
            ASSERT(dest.value() == 5);
            ValueType expected = source;
            // allocator is determined by the copy constructor of TYPE
            ASSERT(hasSameAllocator(dest, expected));
        }
        {
            Obj        mX(bsl::allocator_arg, &oa, 88);
            const Obj& X = mX;

            ValWithAllocator valBuffer(5, &oa);
            const ValueType& source = valBuffer.object();

            const ValueType dest = X.value_or(source);
            ASSERT(dest.value() == 88);
            ValueType expected = ValueType(88);
            // allocator is determined by the copy constructor of TYPE
            ASSERT(hasSameAllocator(dest, expected));
        }
        {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
            Obj              source(4);
            const ValueType& dest = MoveUtil::move(source).value_or(77);
            ASSERT(dest.value() == 4);
            ASSERT(source.value().value() == MOVED_FROM_VAL);
            //Todo Clarify what the expected allocator is
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        }
#ifdef BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
        if (veryVerbose)
            printf("\tallocator tests of allocator extended "
                   "value_or'.\n");
        {
            Obj        mX(bsl::allocator_arg, &oa);
            const Obj& X = mX;

            BloombergLP::bsls::ObjectBuffer<TYPE> valBuffer;
            BloombergLP::bslma::ConstructionUtil::construct(
                valBuffer.address(), &da, 5);
            const TYPE& val = valBuffer.object();

            const ValueType& i1 = X.value_or(bsl::allocator_arg, &ta, val);
            ASSERT(i1.value() == 5);
            ASSERT(i1.get_allocator() == &ta);

            mX.emplace(8);
            const ValueType& i2 = mX.value_or(bsl::allocator_arg, &ta, val);
            ASSERT(i2.value() == 8);
            ASSERT(i2.get_allocator() == &ta);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
            Obj              source(4);
            const ValueType& i3 =
                MoveUtil::move(source).value_or(bsl::allocator_arg, &ta, val);
            ASSERT(i3 == 4);
            ASSERT(source.value() == MOVED_FROM_VAL);
            ASSERT(i3.get_allocator() == &ta);
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        }
#endif  //BSL_COMPILERFEATURES_GUARANTEED_COPY_ELISION
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase9()
{
    // --------------------------------------------------------------------
    // TESTING 'value_or' METHOD
    //   This test will verify that the 'value_or' method works as expected.
    //   The test relies on constructors and 'emplace' method.
    //
    // Concerns:
    //: 1 Calling 'value_or' on a disengaged 'optional' object returns the
    //:   argument value converted to the 'value_type' of the 'optional'
    //:   object.
    //: 2 Calling 'value_or' on an engaged 'optional' object returns the value
    //:   in the 'optional' object.
    //: 3 In C++11, when calling value_or on an engaged 'optional' rvalue, the
    //:   returned object is created by moving from the 'value_type' object in
    //:   'optional'.
    //: 4 It is possible to call 'value_or' on a constant 'optional' object.
    //: 6 If 'value_type' is allocator aware, the allocator of the object
    //:   returned from a single argument 'value_or' method is determined by
    //:   the copy/move/conversion constructor of the 'value_type'.
    //
    // Plan:
    //
    //: 1 Create a disengaged 'optional' object.  For concern 1, call
    //:   'value_or' and check that the return object matches the argument
    //:   given to 'value_or'.
    //: 2 Emplace a value in the 'optional' object. Call 'value_or' and, for
    //:   concern 2, check that the returned value matches the value in the
    //:   'optional' object.
    //: 3 If ref qualifieres are supported, call 'value_or' on an engaged
    //:   'optional' rvalue. For concern 3, verify the resulting objects has
    //:   been created by move construction.
    //: 4 In step 1, for concern 4, call ' value_or' through a 'const'
    //:   reference to the  'optional' object.
    //: 5 For concern 6, if the 'value_type' is allocator aware, check that
    //:   the allocator of the returned object is correct.
    //
    // Testing:
    //  TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const;
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'value_or' METHOD"
               "\n=========================\n");

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    {
        Obj        mX;
        const Obj& X = mX;

        ValWithAllocator valBuffer(5, &oa);
        const ValueType& source = valBuffer.object();
        const ValueType& dest   = X.value_or(source);
        ASSERT(dest.value() == 5);
        ValueType expected = source;
        // allocator is determined by the copy constructor of TYPE
        ASSERT(hasSameAllocator(dest, expected));
    }
    {
        Obj        mX(88);
        const Obj& X = mX;

        ValWithAllocator valBuffer(5, &oa);
        const ValueType& source   = valBuffer.object();
        const ValueType& dest     = X.value_or(source);
        ValueType        expected = ValueType(88);
        ASSERT(dest.value() == 88);
        ASSERT(hasSameAllocator(dest, expected));
    }
    {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        Obj              source(4);
        const ValueType& dest = MoveUtil::move(source).value_or(77);
        ASSERT(dest.value() == 4);
        ASSERT(source.value().value() == MOVED_FROM_VAL);
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase8()
{
    // --------------------------------------------------------------------
    // TESTING 'value' METHOD
    //   This test will verify that the 'value' method works as expected.
    //   The test relies on constructors and 'emplace' method.
    //
    //   MSVC2015 has a bug which removes constness from temporaries in certain
    //   situations. For example :
    //
    //       bsl::string = CObj("s").value();
    //
    //   invokes non const qualified overload of 'value', despite the fact the
    //   temporary is of a const qualified 'bsl::optional'. In the following
    //   example, the constness is preserved:
    //
    //       Cobj temp = Cobj("s");
    //       bsl::string = MoveUtil::move(temp).value();
    //
    //    The tests have been written to take this issue into account.
    //
    // Concerns:
    //: 1 Calling 'value()' on a disengaged 'optional' throws
    //:   'bad_optional_access' exception.
    //: 2 Calling 'value()' on a engaged 'optional' returns the reference
    //:   to the 'value_type' object.
    //: 3 The reference returned from 'value()' is 'const' qualified if the
    //:   'optional' object is 'const' qualified, or if the 'value_type' is
    //:   'const' qualified.
    //: 4 In C++11 and onwards, the returned reference is an rvalue reference
    //    if the 'optional' object is an rvalue.
    //: 5 It is possible to modify non constant 'optional' of non constant
    //:   'value_type' through the reference returned by 'value' method.
    //: 6 That 'value' method behaves the same for allocator aware types and
    //:   non allocator aware types.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' object. Call the 'value' method and,
    //:   for concern 1, check that the 'bad_optional_access' exception is
    //:   thrown.
    //: 2 Emplace a value in the 'optional' object. Call the 'value' method
    //:   and, for concern 2, check that the returned object has the expected
    //:   value.
    //: 3 For concern 3, using 'isConstRef' verify the 'const' qualification
    //:   of a reference returned from 'value' on a non 'const' 'optional' of
    //:   non 'const' 'value_type', 'const' 'optional' of non 'const'
    //:   'value_type', and 'const' 'optional' of 'const' 'value_type'
    //: 4 If ref qualifiers are supported, for concern 3, using 'isRvalueRef'
    //:   verify that the reference retruned from 'value' is an rvalue
    //:   reference if the 'optional' object is an rvalue.
    //: 5 Modify the value of the 'optional' object through the reference
    //:   returned from the 'value' method. For concern 5, call 'value' method
    //:   check that the value of the 'optional' object has been modified.
    //: 6 Call 'reset' on the 'optional' object. For concern 1, call 'value'
    //:   method and check that the 'bad_optional_access' exception is thrown.
    //: 7 For concern 1, in steps 1-6, make sure no unexpected exception is
    //:   thrown.
    //: 8 For concern 6, run the test with an an allocator aware TYPE and a
    //:   non allocator aware type.
    //
    // Testing:
    //   TYPE& value() &;
    //   const TYPE& value() & const;
    //   TYPE&& value() &&;
    //   const TYPE&& value() && const;
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'value' METHOD"
               "\n======================\n");

    bool unexpected_exception_thrown = false;
    try {
        Obj        mX;
        const Obj& X = mX;

        bool bad_optional_exception_caught = false;
        try {
            mX.value();
        }
        catch (const bsl::bad_optional_access&) {
            bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        mX.emplace(3);
        ASSERT(mX.value().value() == 3);

        TYPE& ri         = mX.value();
        ri.d_def.d_value = 7;
        ASSERT(mX.value().value() == 7);

        ObjC cmX(3);
        ASSERT(!isConstRef(mX.value()));
        ASSERT(isConstRef(X.value()));
        ASSERT(isConstRef(cmX.value()));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        ASSERT(!isRvalueRef(mX.value()));
        ASSERT(isRvalueRef(Obj(3).value()));
        ASSERT(isRvalueRef(ObjC(4).value()));
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

        mX.reset();
        try {
            mX.value();
        }
        catch (bsl::bad_optional_access&) {
            bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;
    }
    catch (...) {
        unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);
    unexpected_exception_thrown = false;
}

template <class TYPE>
void TestDriver<TYPE>::testCase7_imp()
{
    // --------------------------------------------------------------------
    // TESTING 'reset' MEMBER FUNCTION
    //   This test will verify that the 'reset 'function works as expected.
    //   The test relies on constructors, 'has_value' and 'emplace' methods.
    //
    // Concerns:
    //: 1 Calling 'reset' on an disengaged 'optional' leaves it disengaged.
    //: 2 Calling 'reset' on an engaged 'optional' makes it disengaged and the
    //:   destructor of the 'value_type' is invoked.
    //: 3 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is 'const'.
    //: 4 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is allocator aware.
    //: 5 For an allocator aware 'value_type', calling 'reset' does not modify
    //:   the allocator.
    //
    // Plan:
    //: 1 Create a disengaged 'optional'object.  For concern 1, call 'reset' on
    //:   the created object and verify that it is still disengaged.
    //: 2 Emplace a value in the 'optional' object. Call 'reset' on the test
    //:   object and verify that it has been disengaged and that the destructor
    //:   of the 'value_type' has been invoked.
    //: 3 In steps 1-2, for concern 5, if the 'value_type' is allocator aware,
    //:   verify that 'get_allocator' method returns the allocator used to
    //:   construct the 'optional' type after each call to 'reset.
    //: 3 For concern 3, run the test using a const qualified TYPE.
    //: 4 For concern 4, run the test using an allocator aware TYPE.
    //
    // Testing:
    //   void reset();
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING 'reset' MEMBER FUNCTION "
               "\n===============================\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    {
        Obj mX;

        mX.reset();
        ASSERT(false == mX.has_value());
        ASSERT(checkAllocator(mX, &da));

        mX.reset();
        ASSERT(false == mX.has_value());
        ASSERT(checkAllocator(mX, &da));

        mX.emplace(TYPE());
        ASSERT(true == mX.has_value());

        int dI = ValueType::s_destructorInvocations;
        mX.reset();
        ASSERT(false == mX.has_value());
        ASSERT(checkAllocator(mX, &da));
        ASSERT(dI == ValueType::s_destructorInvocations - 1);
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase7()
{
    TestDriver<TYPE>::testCase7_imp();
    TestDriver<const TYPE>::testCase7_imp();
}

template <class TYPE>
void TestDriver<TYPE>::testCase6()
{
    // --------------------------------------------------------------------
    // TESTING CONVERSION TO BOOL
    //   This test will verify that the conversion to 'bool' works as expected.
    //   The test relies on reset and emplace member functions, as well as
    //   construction from a 'value_type'.
    //
    // Concerns:
    //: 1 A disengaged 'optional' when converted to 'bool' evaluates to 'false'
    //:   and returns 'false' from its 'has_value' method.
    //: 2 An engaged 'optional' when converted to 'bool' evaluates to 'true'
    //:   and returns 'false' from its 'has_value' method.
    //: 3 Both conversion to 'bool' and 'has_value' method can be performed/
    //:   invoked on a 'const' qualified 'optional' object.
    //: 4 All of the above concerns apply whether or not the 'optional'
    //:   object's 'value_type' is allocator aware.
    //
    // Plan:
    //: 1 Create a disengaged 'optional' and, for concern 1, verify that it
    //:   evaluates to 'false' when converted to 'bool' and that 'has_value'
    //:   method returns 'false'.
    //: 2 Emplace a value in the 'optional' object and, for concern 2, verify
    //:   that it evaluates to 'true' when converted to 'bool' and that the
    //:   'has_value' method returns 'true'.
    //: 3 Call 'reset' method and, for concern 1, verify that the 'optional'
    //:   object evaluates to 'false' when converted to 'bool' and that
    //:   'has_value' method returns 'false'.
    //: 4 Create an engaged 'optional' and, for concern 2, verify that it
    //:   evaluates to 'true' when converted to 'bool' and that 'has_value'
    //:   method returns 'true'.
    //: 5 In step 1, for concern 3, use a 'const' qualified reference to
    //:   test conversion to 'bool' and to invoke the 'has_value' method.
    //
    // Testing:
    //   operator bool() const;
    //   bool has_value() const;
    //
    //   void reset();
    //   optional(const T &);
    //   emplace(const T &);
    //
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nTESTING CONVERSION TO BOOL"
               "\n==========================\n");

    {
        Obj        mX;
        const Obj& X = mX;
        ASSERT_IF_BOOL_CONVERSION(!X);
        ASSERT(false == X.has_value());

        mX.emplace(ValueType());
        ASSERT_IF_BOOL_CONVERSION(mX);
        ASSERT(true == mX.has_value());

        mX.reset();
        ASSERT_IF_BOOL_CONVERSION(!mX);
        ASSERT(false == mX.has_value());
    }
    {
        Obj mX(bsl::in_place);
        ASSERT_IF_BOOL_CONVERSION(mX);
        ASSERT(true == mX.has_value());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase5()
{
    // --------------------------------------------------------------------
    //  ALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS
    //   This test will verify that the allocator extended construction of a
    //   disengaged 'optional' is working as expected.
    //
    // Concerns:
    //: 1 That the allocator extended default constructor creates a disengaged
    //:   object, as determined by 'has_value' returning 'false'.
    //: 2 That the allocator extended constructor taking 'nullopt_t' object
    //:   creates a disengaged object determined by 'has_value' returning
    //:   'false'.
    //: 3 That the 'get_allocator' method returns the allocator passed in to
    //:   the constructors.
    //: 4 These constructors do not allocate any memory.
    //
    // Plan:
    //: 1 Construct an 'optional' object using allocator extended default
    //:   construction and, for concern 1, verify that the 'optional' object is
    //:   disengaged.
    //: 2 Construct an 'optional' object using allocator extended constructor
    //:   that takes 'nullopt_t' argument and, for concern 2, verify that the
    //:   'optional' object is disengaged.
    //: 3 In step 1 and 2, for concern 3 verify that the 'get_allocator' method
    //:   returns the allocator used in 'optional' construction.
    //
    // Testing:
    //   typedef TYPE value_type;
    //   Optional(bsl::allocator_arg_t, allocator_type);
    //   Optional(bsl::allocator_arg_t, allocator_type, nullopt_t);
    //   allocator_type get_allocator() const;
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nALLOCATOR EXTENDED DISENGAGED CONSTRUCTORS"
               "\n==========================================\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    bslma::DefaultAllocatorGuard oag(&oa);

    {
        bslma::TestAllocatorMonitor dam(&da);
        bslma::TestAllocatorMonitor oam(&oa);

        Obj X(bsl::allocator_arg, &oa);
        ASSERT(!X.has_value());
        ASSERT(X.get_allocator() == &oa);
        ASSERT(dam.isTotalSame());
        ASSERT(oam.isTotalSame());
    }
    {
        bslma::TestAllocatorMonitor dam(&da);
        bslma::TestAllocatorMonitor oam(&oa);

        Obj X = Obj(bsl::allocator_arg, &oa, nullopt);
        ASSERT(!X.has_value());
        ASSERT(X.get_allocator() == &oa);
        ASSERT(dam.isTotalSame());
        ASSERT(oam.isTotalSame());
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase4()
{
    // --------------------------------------------------------------------
    // PRIMARY MANIPULATORS AND ACCESSORS
    //   This test will verify that the construction of a disengaged 'optional'
    //   is working as expected.  Also, we test that the basic accessors are
    //   working as expected.
    //
    // Concerns:
    //: 1 That the default constructor creates a disengaged object, as
    //:   determined by 'has_value' returning 'false'.
    //: 2 That the constructor taking 'nullopt_t' object creates a disengaged
    //:   object determined by 'has_value' returning 'false'.
    //: 3 If the 'value_type' of the 'optional' is allocator aware (AA), then
    //:   the 'get_allocator' method returns the default allocator when using
    //:   these constructors.
    //: 4 These constructors do not allocate any memory, whether or not the
    //:   'value_type' is AA.
    //: 5 That 'emplace' method taking a single argument of 'value_type' type
    //:   makes the 'optional' object engaged, as determined by 'has_value'
    //:   returning 'true', with a value of the argument, as determined by the
    //:   reference returned by 'value' method.
    //: 6 That for AA type, 'emplace' will create an object of 'value_type'
    //:   using the 'optional' object's allocator (which is always the default
    //:   allocator for now).
    //: 7 That an engaged 'optional' object converted to 'bool' evaluates to
    //:   'true', and a disengaged 'optional' object evaluates to 'false'.
    //: 8 That 'reset' method disengages an 'optional' object, as determined
    //:   by 'has_value' returning 'false'.
    //: 9 If an engaged 'optional' object is destroyed, the destructor of the
    //:   'value_type' is invoked.
    //
    // Plan:
    //: 1 Construct an 'optional' object using default construction and, for
    //:   concern 1, verify that the 'optional' object is disengaged.
    //: 2 Construct an 'optional' object using the constructor that takes
    //:   'nullopt_t' argument and, for concern 2, verify that the 'optional'
    //:   object is disengaged.
    //: 3 In step 1 and 2, for concern 3 verify that, if the 'value_type' is
    //:   allocator aware, the 'get_allocator' method returns the default
    //:   allocator.
    //: 4 Call 'emplace' on a disengaged 'optional' using an object of
    //:   'value_type' type.  For concern 5, verify the 'get_value' returns
    //:   'true' and that the 'value' method returns the value of the source
    //:   object.
    //: 5 In step 4, for concern 6, if the 'value_type is allocator aware,
    //:   check that the allocator of the object referenced by the result of
    //:   'value' method is the default allocator.
    //: 6 For concern 7, in step 1, verify the disengaged 'optional' object
    //:   converted to 'bool' evaluates to false.  Additionaly, in step 4,
    //:   verify that the engaged 'optional' objects converted to 'bool'
    //:   evaluates to true.
    //: 7 Call reset on an engaged 'optional' object and, for concern 8, verify
    //:   that 'get_value' returns 'false'.
    //: 8 In a separate scope, create an engaged 'optional' object.  For
    //:   concern 9, when the scope ends, verify the destructor of the
    //:   'value_type' has been called.
    //
    // Testing:
    //   typedef TYPE value_type;
    //   Optional();
    //   Optional(nullopt_t);
    //   template<class... ARGS>
    //   void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
    //   void reset()
    //   bool has_value() const;
    //   TYPE& value()
    //   allocator_type get_allocator() const;
    //   operator bool()  const
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nPRIMARY MANIPULATORS AND ACCESSORS"
               "\n=================================="
               "\n");

    if (veryVerbose)
        printf("\tUsing allocator aware type.\n");
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj        mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
            ASSERT(checkAllocator(X, &da));
            ASSERT(dam.isTotalSame());
        }
        {
            bslma::TestAllocatorMonitor dam(&da);

            Obj        mX = Obj(nullopt);
            const Obj& X  = mX;
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
            ASSERT(checkAllocator(X, &da));
            ASSERT(dam.isTotalSame());

            mX.emplace(ValueType(5));
            ASSERT(mX.has_value());
            ASSERT_IF_BOOL_CONVERSION(mX);
            ASSERT(mX.value() == ValueType(5));
            ASSERT(checkAllocator(X.value(), &da));

            mX.reset();
            ASSERT(!X.has_value());
            ASSERT_IF_BOOL_CONVERSION(!X);
        }
        int dI;
        {
            Obj mX = ValueType(5);
            ASSERT(mX.has_value());
            ASSERT_IF_BOOL_CONVERSION(mX);
            dI = ValueType::s_destructorInvocations;
        }
        ASSERT(dI == ValueType::s_destructorInvocations - 1);
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase3()
{
    // --------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Create a disengaged 'optional'
    //: 2 For allocator aware 'value_type, check that 'get_allocator' method
    //:   returns the allocator used to construct the 'optional' object.
    //: 3 Test that 'has_value' on a disengaged 'optional' returns 'false'
    //: 4 Test that dissengaged 'optional' evaluates to 'false'
    //: 5 Emplace a value into the 'optional' object and check that
    //:   'value()' method returns the emplaced value.
    //: 6 Verify reset() method disengages an engaged 'optional.
    //: 7 Create an engaged 'optional' and verify that 'value()' returns the
    //:   expected value
    //: 8 Test that an engaged 'optional' evaluates to 'true'
    //
    // Testing:
    //   BREATHING TEST
    // --------------------------------------------------------------------

    if (verbose)
        printf("\nBREATHING TEST"
               "\n==============\n");
    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    Obj X;
    ASSERT(checkAllocator(X, &da));
    ASSERT(!X.has_value());
    ASSERT_IF_BOOL_CONVERSION(!X);

    TYPE val;
    X.emplace(val);
    ASSERT(checkAllocator(X, &da));
    ASSERT(X.has_value());
    ASSERT(X.value() == val);

    X.reset();
    ASSERT(!X.has_value());

    Obj Y = val;
    ASSERT(Y.has_value());
    ASSERT(checkAllocator(X, &da));
    ASSERT(Y.value() == val);
    ASSERT_IF_BOOL_CONVERSION(Y);
}

template <class TYPE>
void testCase2_Imp()
{
    // --------------------------------------------------------------------
    // 'Optional_DataImp' TEST
    //   This case exercises (but does not fully test) the functionality of
    //   the 'Optional_DataImp' internal class.
    //
    // Concerns:
    //: 1 'Optional_DataImp<TYPE>' class is sufficiently functional to enable
    //:   comprehensive testing of 'bsl::optional'.  This class manages the
    //:   buffer that stores the optional's value type object to ensure access
    //:   to the value type object is 'const' if the value type is 'const'.
    //:   The class also provides an interface for creating the value type
    //:   object inside the buffer, which will be comprehensively tested
    //:   through the interface of 'bsl::optional'.
    //
    // Plan:
    //: 1 Create an empty 'Optional_DataImp' object
    //: 2 Test that 'has_value' returns 'false'
    //: 3 Emplace a value into the 'Optional_DataImp' object and check that
    //:   'value()' method returns the emplaced value
    //: 4 Check that the 'value' method returns a const object if 'TYPE' is
    //:   const qualified
    //: 5 Call 'reset' and verify that 'has_value' returns 'false'
    //
    // Testing:
    //   'Optional_DataImp' class
    // --------------------------------------------------------------------

    if (verbose)
        printf("\n'Optional_DataImp' TEST"
               "\n=======================\n");

    bslma::TestAllocator da("default", veryVeryVeryVerbose);

    BloombergLP::bslstl::Optional_DataImp<TYPE> X;
    ASSERT(!X.hasValue());

    TYPE val = TYPE();
    X.emplace(&da, val);
    ASSERT(X.hasValue());
    ASSERT(X.value() == val);
    ASSERT(checkAllocator(X, &da));
    ASSERT(isConstPtr(&val) == isConstPtr(&X.value()));

    X.reset();
    ASSERT(!X.hasValue());
}
template <class TYPE>
void TestDriver<TYPE>::testCase2()
{
    testCase2_Imp<TYPE>();
    testCase2_Imp<const TYPE>();
}

template <class TYPE>
void TestDriver<TYPE>::testCase1()
{
    // --------------------------------------------------------------------
    // 'Optional_Data' TEST
    //  This case exercises the functionality of 'Optional_Data' internal
    //  class.  'Optional_Data' allows 'bsl::optional' objects to be trivially
    //  destructible if 'TYPE' is trivially destructible.
    //
    // Concerns:
    //: 1 'Optional_Data<TYPE>' object is trivially destructible if 'TYPE' is
    //:    trivially destructible.
    //
    // Plan:
    //: 1 Compare 'std::is_trivially_destructible' trait for 'TYPE' and for
    //:   'Optional_Data<TYPE>'.  In C++03 mode, use
    //:   'bsl::is_trivially_copyable' trait instead.
    //
    // Testing:
    //   'Optional_Data' class
    // --------------------------------------------------------------------
    if (verbose)
        printf("\n'Optional_Data' TEST"
               "\n=================== \n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    ASSERT(std::is_trivially_destructible<TYPE>::value ==
           std::is_trivially_destructible<
               BloombergLP::bslstl::Optional_Data<TYPE> >::value);
#else
    ASSERT(bsl::is_trivially_copyable<TYPE>::value ==
           bsl::is_trivially_copyable<
               BloombergLP::bslstl::Optional_Data<TYPE> >::value);
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int test      = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST  %s CASE %d \n", __FILE__, test);

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) {
      case 0:
      case 32:
        RUN_EACH_TYPE(TestDriver,
                      testCase32,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        break;
      case 31:
        testCase31();
        break;
      case 30:
        RUN_EACH_TYPE(TestDriver,
                      testCase30,
                      MyClass2, MyClass2a);
        RUN_EACH_TYPE(TestDriver,
                      testCase30b,
                      ConstructTestTypeAlloc, ConstructTestTypeAllocArgT);
        break;
      case 29:
        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        RUN_EACH_TYPE(TestDriver,
                      testCase29b,
                      BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS);
        break;
      case 28:
        RUN_EACH_TYPE(TestDriver,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        break;
      case 27:
        RUN_EACH_TYPE(TestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        break;
      case 26:
        RUN_EACH_TYPE(TestDriver,
                      testCase26,
                      ConstructTestTypeAlloc,
                      ConstructTestTypeAllocArgT);
        break;
      case 25:
        RUN_EACH_TYPE(TestDriver,
                      testCase25,
                      BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS);
        break;
      case 24:
        RUN_EACH_TYPE(TestDriver, testCase24, Swappable, SwappableAA);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        // swap between bsl::optional and std::optional works only for non-AA
        // types
        testCase24_imp<Swappable,
                   bsl::optional<Swappable>,
                   std::optional<Swappable> > ();
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

        break;
      case 23:
        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
        break;
      case 22:
        RUN_EACH_TYPE(TestDriver, testCase22, MyClass2, MyClass2a);
        break;
      case 21:
        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;
      case 20:
        RUN_EACH_TYPE(TestDriver, testCase20, MyClass2, MyClass2a);
        break;
      case 19:
        RUN_EACH_TYPE(TestDriver,
                      testCase19,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;
      case 18:
        testCase18();
        break;
      case 17:
        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;
      case 16:
        testCase16();
        break;
      case 15:
        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;
      case 14:
        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        break;
      case 13:
        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        RUN_EACH_TYPE(TestDriver,
                      testCase13b,
                      BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS);
        RUN_EACH_TYPE(TestDriver,
                      testCase13c,
                      BSLSTL_OPTIONAL_TEST_TYPES_VARIADIC_ARGS);
        break;
      case 12:
        RUN_EACH_TYPE(TestDriver, testCase12, MyClass1, MyClass2);
        break;
      case 11:
        RUN_EACH_TYPE(TestDriver, testCase11, MyClass1, MyClass2, MyClass2a);
        break;
      case 10:
        RUN_EACH_TYPE(TestDriver, testCase10, MyClass2, MyClass2a);
        break;
      case 9:
        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;
      case 8:
        RUN_EACH_TYPE(TestDriver, testCase8, MyClass1, MyClass2);
        break;
      case 7:
        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);

        break;
      case 6:
        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;

      case 5:
        RUN_EACH_TYPE(TestDriver, testCase5, MyClass2, MyClass2a);
        break;
      case 4:
        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;
      case 3:

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLSTL_OPTIONAL_TEST_TYPES_INSTANCE_COUNTING);
        break;
      case 2: {
        // --------------------------------------------------------------------
        // 'Optional_DataImp' TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n"
                   "'Optional_DataImp' TEST"
                   "\n"
                   "======================="
                   "\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'Optional_Data' TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n"
                   "'Optional_Data' TEST"
                   "\n"
                   "===================="
                   "\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase1,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
