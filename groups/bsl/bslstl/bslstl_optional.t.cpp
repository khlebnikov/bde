// bslstl_optional.t.cpp                                              -*-C++-*-

#include <bslstl_optional.h>
#include <bsls_bsltestutil.h>
#include <bslstl_string.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsltf_templatetestfacility.h>

#include <stdio.h>
#include <stdlib.h>

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

using namespace BloombergLP;
using namespace bsl;

typedef bslmf::MovableRefUtil MoveUtil;

namespace {

const int MOVED_FROM_VAL = 0x01d;

}  // close unnamed namespace

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

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

    // CREATORS

    explicit MyClass1a(int v)  // IMPLICIT
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
        ++s_moveConstructorInvocations;
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

    // ACCESSORS
    int value() const { return d_data.value(); }
};
// CLASS DATA
int MyClass1a::s_copyConstructorInvocations = 0;
int MyClass1a::s_moveConstructorInvocations = 0;
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
        s_moveConstructorInvocations++;
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

    MyClass2a(const MyClass2a& rhs)
    : d_data(rhs.d_data)
    {
        ++s_copyConstructorInvocations;
    }

    MyClass2a(bsl::allocator_arg_t, bslma::Allocator *a, const MyClass2a& rhs)
    : d_data(rhs.d_data, a)
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

#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    // MANIPULATORS
    MyClass2a& operator=(const MyClass2a& rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(rhs.d_data);
        return *this;
    }

    MyClass2a& operator=(bslmf::MovableRef<MyClass2a> rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(MoveUtil::move(MoveUtil::access(rhs).d_data));
        return *this;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(bslmf::MovableRef<const MyClass2a> rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(MoveUtil::access(rhs).d_data);
        return *this;
    }

#endif  //#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MyClass2a& operator=(int rhs)
        // assign the value of specified 'rhs' to this object
    {
        d_data. operator=(rhs);
        return *this;
    }
    // ACCESSORS

    // returns the value of this object
    int value() const { return d_data.value(); }

    // returns the allocator used for this object
    bsl::allocator<char> get_allocator() const
    {
        return d_data.get_allocator();
    }
};

bool operator==(const MyClass2a& lhs, const MyClass2a& rhs)
    // Check if specified 'lhs' matches the specified 'rhs'
{
    return (lhs.value() == rhs.value());
}
int MyClass2a::s_copyConstructorInvocations = 0;
int MyClass2a::s_moveConstructorInvocations = 0;
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

// helper functions to determine the constness of a pointer
template <class TYPE>
bool isConstPtr(TYPE *)
{
    return false;
}
template <class TYPE>
bool isConstPtr(const TYPE *)
{
    return true;
}

template <class TYPE,
          bool USES_BSLMA_ALLOC =
              BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class Test_Util {
    // This class provided test utilities that have different behaviour
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
    // This class provided test utilities that have different behaviour
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
    // check that the specified 'obj' uses the specified 'allocator'.
{
    return Test_Util<TYPE>::checkAllocator(obj, allocator);
}

template <class TYPE>
bool hasSameAllocator(const TYPE& obj1, const TYPE& obj2)
    // check that the specified 'obj1' and specified 'obj2' use the same
    // allocator.
{
    return Test_Util<TYPE>::hasSameAllocator(obj1, obj2);
}

// ============================================================================
//                          TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

template <class TYPE>
class TestDriver {
    // This class template provides a namespace for testing the 'optional'
    // type.
  public:
    static void testCase2();
    static void testCase2_Imp();
        // Optional_DataImp TEST.  The test requires TYPE to be default
        // constructible.

    static void testCase1();
        // Optional_Data TEST

};
template <class TYPE>
void TestDriver<TYPE>::testCase2()
{
    testCase2_Imp();
    TestDriver<const TYPE>::testCase2_Imp();

}
template <class TYPE>
void TestDriver<TYPE>::testCase2_Imp()
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

    typedef const TYPE CONST_TYPE;

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

    switch (test) {
      case 0:
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
// Copyright 2020 Bloomberg Finance L.P.
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
