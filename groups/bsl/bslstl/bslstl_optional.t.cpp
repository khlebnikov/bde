// bslstl_optional.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include "bslstl_optional.h"
#include <bsls_bsltestutil.h>
#include <bslstl_string.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#define BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING true
// A list of disabled tests :
// BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING
//      Tests in this group rely on perfect forwarding of arguments in
//      BloombergLP::bslalg::ScalarPrimitives::construct to correctly
//      support move semantics in bsl:optional.
//
//BSLSTL_OPTIONAL_TEST_BAD_VALUE
//      Tests in this group check that value categorie and cv qualification
//      of the return value from value() member function is correct.
//
//BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT
//      Tests in this group check that assignments is not possible if the
//      value type is not both assignable and constructible from the source
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
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
#error "BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES not supported"
#endif

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#error "Generalized initializers needed for the current version of bsl::optional"
#endif

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)


// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

const int   MAX_NUM_PARAMS = 5; // max in simulation of variadic templates

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

using namespace BloombergLP;
using namespace bsl;

typedef bslmf::MovableRefUtil MovUtl;

const int MOVED_FROM_VAL = 0x01d;
const int OPT_OL_OFFSET = 0x00a;

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

    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    explicit
    my_Class1(int v = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }
    my_Class1(const my_Class1& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
        ++copyConstructorInvocations;
    }

    my_Class1(bslmf::MovableRef<my_Class1> other) {
        my_Class1& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        d_def.d_allocator_p = 0;
        ++moveConstructorInvocations;
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

    my_Class1& operator=(bslmf::MovableRef<my_Class1> rhs) {
        my_Class1& otherRef = MovUtl::access(rhs);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        return *this;
    }

    my_Class1& operator=(int rhs) {
        d_def.d_value = rhs;
        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};
bool operator==(const my_Class1& lhs,
                const my_Class1& rhs)
{
 return (lhs.value()==rhs.value());
}
// CLASS DATA
int my_Class1::copyConstructorInvocations       = 0;
int my_Class1::moveConstructorInvocations       = 0;
                             // ===============
                             // class my_Class1a
                             // ===============

class my_Class1a {
// This 'class' is a simple type that does not take allocators.  Its
// implementation owns a 'my_Class1' aggregate, but uses only the
// 'd_value' data member, to support the 'value' attribute.  The
// 'd_allocator_p' pointer is always initialized to a null pointer.
// The class is both constructable and asignable from my_Class1.
  public:
    my_Class1 d_data;
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS

    explicit
    my_Class1a(int v)  : d_data(v) {}

    my_Class1a(const my_Class1& v)
    : d_data(v) {}

    my_Class1a(bslmf::MovableRef<my_Class1> v)
    : d_data(MovUtl::move(v)) {}

    my_Class1a(const my_Class1a& rhs) : d_data(rhs.d_data)
    {
        ++copyConstructorInvocations;
    }

    my_Class1a(bslmf::MovableRef<my_Class1a> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data))
    {
        ++moveConstructorInvocations;
    }

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
// CLASS DATA
int my_Class1a::copyConstructorInvocations       = 0;
int my_Class1a::moveConstructorInvocations       = 0;
bool operator==(const my_Class1a& lhs,
                const my_Class1a& rhs)
{
 return (lhs.value()==rhs.value());
}
                             // ===============
                             // class my_Class1b
                             // ===============

class my_Class1b {
    // This 'class' is a simple type that does not take allocators.  Its
    // implementation owns a 'my_Class1' aggregate, but uses only the
    // 'd_value' data member, to support the 'value' attribute.  The
    // 'd_allocator_p' pointer is always initialized to a null pointer.
    // The class is assignable from my_Class1, but not constructable from
    // my_Class1
  public:
    my_Class1 d_data;

    // CREATORS
    my_Class1b() : d_data() { }

    explicit
    my_Class1b(int v)  : d_data(v) {}

    my_Class1b(const my_Class1b& rhs) : d_data(rhs.d_data) {}

    my_Class1b(bslmf::MovableRef<my_Class1b> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    // MANIPULATORS
    my_Class1b& operator=(const my_Class1b& rhs) {
    d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class1b& operator=(bslmf::MovableRef<my_Class1b> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    my_Class1b& operator=(const my_Class1& rhs) {
        d_data.operator=(rhs);
        return *this;
    }

    my_Class1b& operator=(bslmf::MovableRef<my_Class1> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs)));
        return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const my_Class1b& lhs,
                const my_Class1b& rhs)
{
    return (lhs.value()==rhs.value());
}
                             // ================
                             // class my_Class1c
                             // ================

class my_Class1c {
  // This 'class' is a simple type that does not take allocators.  Its
  // implementation owns a 'my_Class1' aggregate, but uses only the
  // 'd_value' data member, to support the 'value' attribute.  The
  // 'd_allocator_p' pointer is always initialized to a null pointer.
  // The class is constructable from my_Class1, but not assignable from
  // my_Class1
  public:
    my_Class1 d_data;

    // CREATORS
    my_Class1c() : d_data() { }

    my_Class1c(int v)  : d_data(v) {}

    explicit
    my_Class1c(const my_Class1& v)
    : d_data(v) {}

    explicit
    my_Class1c(bslmf::MovableRef<my_Class1> v)
    : d_data(MovUtl::move(v)) {}

    my_Class1c(const my_Class1c& rhs) : d_data(rhs.d_data) {}

    my_Class1c(bslmf::MovableRef<my_Class1c> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    // MANIPULATORS
    my_Class1c& operator=(const my_Class1c& rhs) {
    d_data.operator=(rhs.d_data);
    return *this;
    }

    my_Class1c& operator=(bslmf::MovableRef<my_Class1c> rhs) {
    d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
    return *this;
    }

    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const my_Class1c& lhs,
                const my_Class1c& rhs)
{
    return (lhs.value()==rhs.value());
}

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

    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    explicit
    my_Class2(bslma::Allocator *a = 0) {
        d_def.d_value = 0;
        d_def.d_allocator_p = a;
    }

    my_Class2(int v, bslma::Allocator *a = 0) {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }
    my_Class2(const my_Class2& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
        copyConstructorInvocations++;
    }

    my_Class2(bslmf::MovableRef<my_Class2> other, bslma::Allocator *a = 0) {

        my_Class2& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
        if (a) {
            d_def.d_allocator_p = a;
        }
        else {
            d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
        }
        moveConstructorInvocations++;
    }

    my_Class2(const my_Class1& rhs, bslma::Allocator *a = 0) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = a;
    }

    my_Class2(bslmf::MovableRef<my_Class1> other, bslma::Allocator *a = 0) {

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

    my_Class2& operator=(int rhs) {
        d_def.d_value = rhs;

        // do not touch allocator!

        return *this;
    }

    // ACCESSORS
    int value() const { return d_def.d_value; }
};
// CLASS DATA
int my_Class2::copyConstructorInvocations       = 0;
int my_Class2::moveConstructorInvocations       = 0;
bool operator==(const my_Class2& lhs,
                const my_Class2& rhs)
{
    return (lhs.value()==rhs.value());
}

bool operator==(const int& lhs,
                const my_Class2& rhs)
{
    return (lhs==rhs.value());
}
bool operator==(const my_Class2& lhs,
                const int& rhs)
{
    return (lhs.value()==rhs);
}
bool operator!=(const my_Class2& lhs,
                const my_Class2& rhs)
{
    return !(lhs ==rhs);
}

bool operator!=(const int& lhs,
                const my_Class2& rhs)
{
    return !(lhs ==rhs);
}
bool operator!=(const my_Class2& lhs,
                const int& rhs)
{
    return !(lhs ==rhs);
}
bool operator<(const my_Class2& lhs,
                const my_Class2& rhs)
{
    return (lhs.value()<rhs.value());
}

bool operator<(const int& lhs,
                const my_Class2& rhs)
{
    return (lhs<rhs.value());
}
bool operator<(const my_Class2& lhs,
                const int& rhs)
{
    return (lhs.value()<rhs);
}
bool operator>(const my_Class2& lhs,
                const my_Class2& rhs)
{
    return (lhs.value()>rhs.value());
}

bool operator>(const int& lhs,
                const my_Class2& rhs)
{
    return (lhs>rhs.value());
}
bool operator>(const my_Class2& lhs,
                const int& rhs)
{
    return (lhs.value()>rhs);
}
bool operator<=(const my_Class2& lhs,
                const my_Class2& rhs)
{
    return (lhs.value()<=rhs.value());
}

bool operator<=(const int& lhs,
                const my_Class2& rhs)
{
    return (lhs<=rhs.value());
}
bool operator<=(const my_Class2& lhs,
                const int& rhs)
{
    return (lhs.value()<=rhs);
}
bool operator>=(const my_Class2& lhs,
                const my_Class2& rhs)
{
    return (lhs.value()>=rhs.value());
}

bool operator>=(const int& lhs,
                const my_Class2& rhs)
{
    return (lhs>=rhs.value());
}
bool operator>=(const my_Class2& lhs,
                const int& rhs)
{
    return (lhs.value()>=rhs);
}

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
    // This class is constructable and assignable from my_Class2


  public:
    my_Class2 d_data;

    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    // CREATORS
    my_Class2a() : d_data() { }

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    explicit
    my_Class2a(int v)  : d_data(v) {}

    my_Class2a(const my_Class2& rhs) : d_data(rhs) {}

    my_Class2a(bslmf::MovableRef<my_Class2> rhs)
      : d_data(MovUtl::move(rhs)) {}

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a, int v)
        : d_data(v, a) {}

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a,const my_Class2& v)
            : d_data(v, a) {}

    my_Class2a(bsl::allocator_arg_t, bslma::Allocator *a,
                bslmf::MovableRef<my_Class2> v)
            : d_data(MovUtl::move(v), a) {}


    my_Class2a(const my_Class2a& rhs) : d_data(rhs.d_data)
    {
        ++copyConstructorInvocations;
    }

    my_Class2a(bsl::allocator_arg_t  ,
               bslma::Allocator     *a,
               const my_Class2a&     rhs)
        : d_data(rhs.d_data, a)
    {
        ++copyConstructorInvocations;
    }

    my_Class2a(bslmf::MovableRef<my_Class2a> rhs)
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data))
    {
        ++moveConstructorInvocations;
    }

    my_Class2a(bsl::allocator_arg_t,
               bslma::Allocator              *a,
               bslmf::MovableRef<my_Class2a>  rhs)
        : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a)
    {
        ++moveConstructorInvocations;
    }

    // MANIPULATORS
    my_Class2a& operator=(const my_Class2a& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class2a& operator=(bslmf::MovableRef<my_Class2a> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    my_Class2a& operator=(int rhs) {
        d_data.operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }
};

bool operator==(const my_Class2a& lhs,
                const my_Class2a& rhs)
{
    return (lhs.value()==rhs.value());
}
int my_Class2a::copyConstructorInvocations       = 0;
int my_Class2a::moveConstructorInvocations       = 0;
// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<my_Class2a> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<my_Class2a> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

                                 // ==========
                                 // my_Class2b
                                 // ==========

class my_Class2b {
  // This 'class' behaves the same as 'my_Class2' (allocator-aware type that
  // never actually allocates memory) except that it uses the
  // 'allocator_arg_t' idiom for passing an allocator to constructors.
  // This class is assignable from my_Class2, but not constructible from
  // my_Class2


  public:
    my_Class2 d_data;

    // CREATORS
    my_Class2b() : d_data() { }

    my_Class2b(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    my_Class2b(const my_Class2b& rhs) : d_data(rhs.d_data) {}

    my_Class2b(bsl::allocator_arg_t  ,
                bslma::Allocator     *a,
                const my_Class2b&     rhs)
    : d_data(rhs.d_data, a) {}

    my_Class2b(bslmf::MovableRef<my_Class2b> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    my_Class2b(bsl::allocator_arg_t,
                bslma::Allocator              *a,
                bslmf::MovableRef<my_Class2b>  rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a) {}

    // MANIPULATORS
    my_Class2b& operator=(const my_Class2b& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class2b& operator=(bslmf::MovableRef<my_Class2b> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    my_Class2b& operator=(const my_Class2& rhs) {
       d_data.operator=(rhs);
       return *this;
    }

    my_Class2b& operator=(bslmf::MovableRef<my_Class2> rhs) {
       d_data.operator=(MovUtl::move(MovUtl::access(rhs)));
       return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const my_Class2b& lhs,
                const my_Class2b& rhs)
{
    return (lhs.value()==rhs.value());
}

// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<my_Class2b> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<my_Class2b> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace


                                 // ==========
                                 // my_Class2c
                                 // ==========

class my_Class2c {
  // This 'class' behaves the same as 'my_Class2' (allocator-aware type that
  // never actually allocates memory) except that it uses the
  // 'allocator_arg_t' idiom for passing an allocator to constructors.
  // This class is constructable from my_Class2, but not assignable from
  // my_Class2

  public:
    my_Class2 d_data;

    // CREATORS
    my_Class2c() : d_data() { }

    my_Class2c(bsl::allocator_arg_t, bslma::Allocator *a) : d_data(a) {}

    explicit
    my_Class2c(int v)  : d_data(v) {}

    my_Class2c(bsl::allocator_arg_t, bslma::Allocator *a, int v)
    : d_data(v, a) {}

    my_Class2c(bsl::allocator_arg_t, bslma::Allocator *a,const my_Class2& v)
    : d_data(v, a) {}

    my_Class2c(bsl::allocator_arg_t, bslma::Allocator *a,
    bslmf::MovableRef<my_Class2> v)
    : d_data(MovUtl::move(v), a) {}


    my_Class2c(const my_Class2c& rhs) : d_data(rhs.d_data) {}

    my_Class2c(bsl::allocator_arg_t  ,
    bslma::Allocator     *a,
    const my_Class2c&     rhs)
    : d_data(rhs.d_data, a) {}

    my_Class2c(bslmf::MovableRef<my_Class2c> rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data)) {}

    my_Class2c(bsl::allocator_arg_t,
              bslma::Allocator              *a,
              bslmf::MovableRef<my_Class2c>  rhs)
    : d_data(MovUtl::move(MovUtl::access(rhs).d_data), a) {}

    // MANIPULATORS
    my_Class2c& operator=(const my_Class2c& rhs) {
        d_data.operator=(rhs.d_data);
        return *this;
    }

    my_Class2c& operator=(bslmf::MovableRef<my_Class2c> rhs) {
        d_data.operator=(MovUtl::move(MovUtl::access(rhs).d_data));
        return *this;
    }

    my_Class2c& operator=(int rhs) {
        d_data.operator=(rhs);
        return *this;
    }
    // ACCESSORS
    int value() const { return d_data.value(); }
};
bool operator==(const my_Class2c& lhs,
                const my_Class2c& rhs)
{
    return (lhs.value()==rhs.value());
}
// TRAITS
namespace BloombergLP {

namespace bslma {
template <> struct UsesBslmaAllocator<my_Class2c> : bsl::true_type { };
}  // close namespace bslma

namespace bslmf {
template <> struct UsesAllocatorArgT<my_Class2c> : bsl::true_type { };
}  // close namespace bslmf

}  // close enterprise namespace

                              // ===============
                              // class my_Class3
                              // ===============

class my_Class3 {
// This 'class' takes allocators similarly to 'my_Class2'. In addition,
// it has overloads taking const rvalues.

public:

      // DATA
      my_ClassDef d_def;

      // CREATORS
      explicit
      my_Class3(bslma::Allocator *a = 0) {
          d_def.d_value = 0;
          d_def.d_allocator_p = a;
      }
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

      my_Class3(bslmf::MovableRef<my_Class3> other, bslma::Allocator *a = 0) {

          my_Class3& otherRef = MovUtl::access(other);
          d_def.d_value = otherRef.d_def.d_value + 10;
          otherRef.d_def.d_value = MOVED_FROM_VAL;
          if (a) {
              d_def.d_allocator_p = a;
          }
          else {
              d_def.d_allocator_p = otherRef.d_def.d_allocator_p;
          }
      }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
      my_Class3(const my_Class3&& other, bslma::Allocator *a = 0) {
          d_def.d_value = other.d_def.d_value + 20;
          if (a) {
              d_def.d_allocator_p = a;
          }
          else {
              d_def.d_allocator_p = other.d_def.d_allocator_p;
          }
      }
#endif
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
                      // class my_Class4
                      // ===============

class my_Class4 {

    // This 'class' is the same as my_Class3, except it doesn't use
    // allocators.

public:

    // DATA
    my_ClassDef d_def;

    // CREATORS
    explicit
    my_Class4() {
        d_def.d_value = 0;
        d_def.d_allocator_p = 0;
    }
    my_Class4(int v) {
        d_def.d_value = v;
        d_def.d_allocator_p = 0;
    }
    my_Class4(const my_Class4& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    my_Class4(const my_Class2& rhs) {
        d_def.d_value = rhs.d_def.d_value;
        d_def.d_allocator_p = 0;
    }

    my_Class4(bslmf::MovableRef<my_Class4> other) {
        my_Class4& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value + 10;
        otherRef.d_def.d_value = MOVED_FROM_VAL;
    }
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    my_Class4(const my_Class4&& other) {
        d_def.d_value = other.d_def.d_value + 20;
    }
#endif //BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    ~my_Class4() {
        ASSERT(d_def.d_value != 93);
        d_def.d_value = 93;
        d_def.d_allocator_p = 0;
        dumpClassDefState(d_def);
    }

    // MANIPULATORS
    my_Class4& operator=(const my_Class4& rhs) {
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
    int  d_copied_count;
        // if true, indicates this object was created from an rvalue

    // CREATORS
    ConstructTestArg(int value = -1);
        // Create an object having the specified 'value'.

    ConstructTestArg(const ConstructTestArg& other);
    ConstructTestArg(bslmf::MovableRef<ConstructTestArg>  other);
};

// CREATORS
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(int value)
    : d_value(value), d_copied_count(0)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(const ConstructTestArg& other)
    : d_value(other.d_value), d_copied_count(other.d_copied_count + 1)
{
}
template <int ID>
ConstructTestArg<ID>::ConstructTestArg(
                                     bslmf::MovableRef<ConstructTestArg> other)
    : d_value(MovUtl::access(other).d_value),
      d_copied_count(MovUtl::access(other).d_copied_count)
{}
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
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;


    int d_ilsum; // sum of initializer_list argument

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

    ConstructTestTypeNoAlloc() : d_ilsum(0){};

    explicit
    ConstructTestTypeNoAlloc(const Arg1 &  a1)
        : d_ilsum(0), d_a1(a1) {}
    explicit
    ConstructTestTypeNoAlloc(bslmf::MovableRef<Arg1>  a1)
        : d_ilsum(0), d_a1(MovUtl::move(a1)) {}

    template <class ARG1,  class ARG2>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {}

    template <class ARG1,  class ARG2,  class ARG3>
        explicit
        ConstructTestTypeNoAlloc(
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0),
      d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
      d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
      d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
      d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9>
    explicit
    ConstructTestTypeNoAlloc(
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10>
    explicit
    ConstructTestTypeNoAlloc(
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
     : d_ilsum(0),
       d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
       d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
       d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
       d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
       d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
       d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
       d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
       d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
       d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
       d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {}
    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10, class ARG11>
    explicit
    ConstructTestTypeNoAlloc(
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
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {}

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12>
    explicit
    ConstructTestTypeNoAlloc(
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
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
         d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {}

    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13>
    explicit
    ConstructTestTypeNoAlloc(
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
        : d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)) {}


    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13, class ARG14>
    explicit
    ConstructTestTypeNoAlloc(
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
        :d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14)) {}

    ConstructTestTypeNoAlloc(const ConstructTestTypeNoAlloc &other)
        : d_ilsum(0),
          d_a1(other.d_a1), d_a2(other.d_a2), d_a3(other.d_a3),
          d_a4(other.d_a4), d_a5(other.d_a5), d_a6(other.d_a6),
          d_a7(other.d_a7), d_a8(other.d_a8), d_a9(other.d_a9),
          d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12),
          d_a13(other.d_a13), d_a14(other.d_a14)
    {
        ++copyConstructorInvocations;
    }

    ConstructTestTypeNoAlloc(
                           bslmf::MovableRef<ConstructTestTypeNoAlloc> &other)
        : d_ilsum(0),
          d_a1(MovUtl::access(other).d_a1), d_a2(MovUtl::access(other).d_a2),
          d_a3(MovUtl::access(other).d_a3), d_a4(MovUtl::access(other).d_a4),
          d_a5(MovUtl::access(other).d_a5), d_a6(MovUtl::access(other).d_a6),
          d_a7(MovUtl::access(other).d_a7), d_a8(MovUtl::access(other).d_a8),
          d_a9(MovUtl::access(other).d_a9),d_a10(MovUtl::access(other).d_a10),
          d_a11(MovUtl::access(other).d_a11), d_a12(MovUtl::access(other).d_a12),
          d_a13(MovUtl::access(other).d_a13), d_a14(MovUtl::access(other).d_a14)
    {
        ++moveConstructorInvocations;
    }

    ConstructTestTypeNoAlloc(std::initializer_list<int> il)
        : d_ilsum(0)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
    {
      for (int i : il) d_ilsum+=i;
    }


    template <class ARG1,  class ARG2,  class ARG3>
        explicit
        ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
    : d_ilsum(0),
      d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
      d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
      d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
      d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                             BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
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
     : d_ilsum(0),
       d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
       d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
       d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
       d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
       d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
       d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
       d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
       d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
       d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
       d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                          class ARG4,  class ARG5,  class ARG6,
                          class ARG7,  class ARG8,  class ARG9,
                          class ARG10, class ARG11>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
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
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
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
       : d_ilsum(0),
         d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
         d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
         d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
         d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
         d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
         d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
         d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
         d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
         d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
         d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
         d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
         d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
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
        : d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
    {
      for (int i : il) d_ilsum+=i;
    }


    template <class ARG1,  class ARG2,  class ARG3,
                  class ARG4,  class ARG5,  class ARG6,
                  class ARG7,  class ARG8,  class ARG9,
                  class ARG10, class ARG11, class ARG12,
                  class ARG13, class ARG14>
    explicit
    ConstructTestTypeNoAlloc(std::initializer_list<int> il,
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
        :d_ilsum(0),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
    {
      for (int i : il) d_ilsum+=i;
    }


};
int ConstructTestTypeNoAlloc::copyConstructorInvocations       = 0;
int ConstructTestTypeNoAlloc::moveConstructorInvocations       = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeNoAlloc& lhs,
                const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_ilsum  == rhs.d_ilsum &&
           lhs.d_a1.d_value  == rhs.d_a1.d_value &&
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
           lhs.d_a14.d_value == rhs.d_a14.d_value ;
}
bool createdAlike(const ConstructTestTypeNoAlloc& lhs,
                  const ConstructTestTypeNoAlloc& rhs)
{
    return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
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
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    int d_ilsum; // sum of initializer_list argument

    bslma::Allocator *d_allocator_p;
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
        : d_ilsum(0), d_allocator_p(allocator) {}
    ConstructTestTypeAlloc(const ConstructTestTypeAlloc&  other,
                           bslma::Allocator              *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
        , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
        , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
        , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
        , d_a13(other.d_a13), d_a14(other.d_a14)
    {
        ++copyConstructorInvocations;
    }

    ConstructTestTypeAlloc(bslmf::MovableRef<ConstructTestTypeAlloc>  other,
                           bslma::Allocator              *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
          d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
          d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
          d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
          d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
          d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
          d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
          d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
          d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
          d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
          d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
          d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
          d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
          d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
        ++moveConstructorInvocations;
    }

    explicit
    ConstructTestTypeAlloc(const Arg1 &  a1, bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1(a1) {}
    explicit
    ConstructTestTypeAlloc(bslmf::MovableRef<Arg1>  a1,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1(MovUtl::move(a1)) {}

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           const Arg2 &                             a2,
                           bslma::Allocator             *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(a2) {}

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           bslmf::MovableRef<Arg2>                  a2,
                           bslma::Allocator *allocator              = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(MovUtl::move(a2)) {}

    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           const Arg3                              &a3,
                           bslma::Allocator *allocator              = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(a3){}

    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           bslmf::MovableRef<Arg3>                  a3,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(MovUtl::move(a3)){}

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           const Arg4                              &a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(a4){}
    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           bslmf::MovableRef<Arg4>                  a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(MovUtl::move(a4)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           const Arg5                              &a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(a5){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           bslmf::MovableRef<Arg5>                  a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(MovUtl::move(a5)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           const Arg6                              &a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(a6){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           bslmf::MovableRef<Arg6>                  a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(MovUtl::move(a6)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           const Arg7                              &a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(a7){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           bslmf::MovableRef<Arg7>                  a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(MovUtl::move(a7)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           const Arg8                              &a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(a8){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           bslmf::MovableRef<Arg8>                  a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(MovUtl::move(a8)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           const Arg9                              &a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(a9){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           bslmf::MovableRef<Arg9>                  a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(MovUtl::move(a9)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           const Arg10                             &a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(a10){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           bslmf::MovableRef<Arg10>                 a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(MovUtl::move(a10)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
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
                           const Arg11                             &a11,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(a11){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(MovUtl::move(a11)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
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
                           const Arg12                             &a12,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(a12){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(MovUtl::move(a12)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
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
                           const Arg13                             &a13,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(a13){}
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(MovUtl::move(a13)){}

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
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
                           const Arg14                             &a14,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(a14) {}
    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(MovUtl::move(a14)){}

    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
    {
      for (int i : il) d_ilsum+=i;
    }
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           const Arg1 &  a1,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator)
        , d_a1(a1)
    {
      for (int i : il) d_ilsum+=i;
    }
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           bslmf::MovableRef<Arg1>  a1,
                           bslma::Allocator *allocator = 0)
            : d_ilsum(0), d_allocator_p(allocator)
            , d_a1(MovUtl::move(a1))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           const Arg2 &                             a2,
                           bslma::Allocator             *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(a2)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1>
    explicit
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           bslmf::MovableRef<Arg2>                  a2,
                           bslma::Allocator *allocator              = 0)
            : d_ilsum(0), d_allocator_p(allocator),
              d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
              d_a2(MovUtl::move(a2))
    {
      for (int i : il) d_ilsum+=i;
    }


    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           const Arg3                              &a3,
                           bslma::Allocator *allocator              = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(a3)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           bslmf::MovableRef<Arg3>                  a3,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(MovUtl::move(a3))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           const Arg4                              &a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(a4)
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           bslmf::MovableRef<Arg4>                  a4,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(MovUtl::move(a4))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           const Arg5                              &a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(a5)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           bslmf::MovableRef<Arg5>                  a5,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(MovUtl::move(a5))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           const Arg6                              &a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(a6)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           bslmf::MovableRef<Arg6>                  a6,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(MovUtl::move(a6))
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           const Arg7                              &a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(a7)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           bslmf::MovableRef<Arg7>                  a7,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(MovUtl::move(a7))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           const Arg8                              &a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(a8)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           bslmf::MovableRef<Arg8>                  a8,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(MovUtl::move(a8))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           const Arg9                              &a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(a9)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           bslmf::MovableRef<Arg9>                  a9,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(MovUtl::move(a9))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           const Arg10                             &a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(a10)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9,
                           bslmf::MovableRef<Arg10>                 a10,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(MovUtl::move(a10))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
                           const Arg11                             &a11,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(a11)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(MovUtl::move(a11))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
                           const Arg12                             &a12,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(a12)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(MovUtl::move(a12))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
                           const Arg13                             &a13,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(a13)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(MovUtl::move(a13))
    {
      for (int i : il) d_ilsum+=i;
    }

    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
                      class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
                           const Arg14                             &a14,
                           bslma::Allocator *allocator = 0)
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(a14)
    {
      for (int i : il) d_ilsum+=i;
    }
    template <class ARG1,  class ARG2,  class ARG3,
                      class ARG4,  class ARG5,  class ARG6,
                      class ARG7,  class ARG8,  class ARG9,
                      class ARG10, class ARG11, class ARG12,
                      class ARG13>
    ConstructTestTypeAlloc(std::initializer_list<int> il,
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
        : d_ilsum(0), d_allocator_p(allocator),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(MovUtl::move(a14))
    {
      for (int i : il) d_ilsum+=i;
    }

};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAlloc> : bsl::true_type { };

}  // close namespace bslma
}  // close enterprise namespace
int ConstructTestTypeAlloc::copyConstructorInvocations       = 0;
int ConstructTestTypeAlloc::moveConstructorInvocations       = 0;
// FREE OPERATORS
bool operator==(const ConstructTestTypeAlloc& lhs,
                const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_ilsum  == rhs.d_ilsum &&
           lhs.d_a1.d_value  == rhs.d_a1.d_value &&
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
bool createdAlike(const ConstructTestTypeAlloc& lhs,
                  const ConstructTestTypeAlloc& rhs)
{
    return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
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
        N1 = -1   // default value for all private data
    };

  public:
    // DATA (exceptionally public, only within a test driver)
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    bslma::Allocator *d_allocator_p;
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
    ConstructTestTypeAllocArgT()
            : d_allocator_p(0){}

    ConstructTestTypeAllocArgT(ConstructTestTypeAllocArgT const& other)
    : d_allocator_p(0)
     , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
     , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
     , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
     , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
     , d_a13(other.d_a13), d_a14(other.d_a14)
    {
       ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(
              bslmf::MovableRef<ConstructTestTypeAllocArgT>  other)
       : d_allocator_p(other.d_allocator_p),
         d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
         d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
         d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
         d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
         d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
         d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
         d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
         d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
         d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
         d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
         d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
         d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
         d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
         d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
       ++moveConstructorInvocations;
    }
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               ConstructTestTypeAllocArgT const& other)
    : d_allocator_p(alloc)
     , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
     , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
     , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
     , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
     , d_a13(other.d_a13), d_a14(other.d_a14)
    {
       ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                          bslma::Allocator     *alloc,
                          bslmf::MovableRef<ConstructTestTypeAllocArgT>  other)
       : d_allocator_p(alloc),
         d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
         d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
         d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
         d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
         d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
         d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
         d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
         d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
         d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
         d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
         d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
         d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
         d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
         d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
       ++moveConstructorInvocations;
    }

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc)
        : d_allocator_p(alloc){}

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               const Arg1           &a1)
        : d_allocator_p(alloc),
          d_a1(a1){}

    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               bslmf::MovableRef<Arg1>  a1)
        : d_allocator_p(alloc),
          d_a1(MovUtl::move(a1)){}

    template <class ARG1,  class ARG2>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)) {}

    template <class ARG1,  class ARG2,  class ARG3>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
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
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
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
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
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
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)) {}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12,
              class ARG13>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
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
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)){}

    template <class ARG1,  class ARG2,  class ARG3,
              class ARG4,  class ARG5,  class ARG6,
              class ARG7,  class ARG8,  class ARG9,
              class ARG10, class ARG11, class ARG12,
              class ARG13, class ARG14>
    ConstructTestTypeAllocArgT(bsl::allocator_arg_t       ,
                               bslma::Allocator     *alloc,
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
        : d_allocator_p(alloc),
          d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
          d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
          d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
          d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
          d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
          d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
          d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
          d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
          d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
          d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
          d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
          d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
          d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
          d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14)) {}


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
int ConstructTestTypeAllocArgT::copyConstructorInvocations       = 0;
int ConstructTestTypeAllocArgT::moveConstructorInvocations       = 0;

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
bool createdAlike(const ConstructTestTypeAllocArgT& lhs,
                  const ConstructTestTypeAllocArgT& rhs)
{
    return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
           lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
           lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
           lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
           lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
           lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
           lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
           lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
           lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
           lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
           lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
           lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
           lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
           lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
}
                        // ================================
                        // class ConstructTestTypeAllocArgTIL
                        // ================================

class ConstructTestTypeAllocArgTIL {
    // This class provides a test class capable of holding up to 14 parameters
    // of types 'ConstructTestArg[1--14]'.  By default, a
    // 'ConstructTestTypeAllocArgTIL' is constructed with nil ('N1') values,
    // but instances can be constructed with actual values (e.g., for creating
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
    static int copyConstructorInvocations;
    static int moveConstructorInvocations;

    int d_ilsum;
    bslma::Allocator *d_allocator_p;
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
    ConstructTestTypeAllocArgTIL()
    : d_ilsum(0), d_allocator_p(0){}

    ConstructTestTypeAllocArgTIL(ConstructTestTypeAllocArgTIL const& other)
    : d_ilsum(0), d_allocator_p(0)
    , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
    , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
    , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
    , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
    , d_a13(other.d_a13), d_a14(other.d_a14)
    {
        ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgTIL(
    bslmf::MovableRef<ConstructTestTypeAllocArgTIL>  other)
    : d_ilsum(0), d_allocator_p(other.d_allocator_p),
    d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
    d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
    d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
    d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
    d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
    d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
    d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
    d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
    d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
    d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
    d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
    d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
    d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
    d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
      ++moveConstructorInvocations;
    }
    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
            bslma::Allocator     *alloc,
            ConstructTestTypeAllocArgTIL const& other)
    : d_ilsum(0), d_allocator_p(alloc)
    , d_a1 (other.d_a1),  d_a2 (other.d_a2),  d_a3 (other.d_a3)
    , d_a4 (other.d_a4),  d_a5 (other.d_a5),  d_a6 (other.d_a6)
    , d_a7 (other.d_a7),  d_a8 (other.d_a8),  d_a9 (other.d_a9)
    , d_a10(other.d_a10), d_a11(other.d_a11), d_a12(other.d_a12)
    , d_a13(other.d_a13), d_a14(other.d_a14)
    {
    ++copyConstructorInvocations;
    }

    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
       bslma::Allocator     *alloc,
       bslmf::MovableRef<ConstructTestTypeAllocArgTIL>  other)
    : d_ilsum(0), d_allocator_p(alloc),
    d_a1 (MovUtl::move(MovUtl::access(other).d_a1)),
    d_a2 (MovUtl::move(MovUtl::access(other).d_a2)),
    d_a3 (MovUtl::move(MovUtl::access(other).d_a3)),
    d_a4 (MovUtl::move(MovUtl::access(other).d_a4)),
    d_a5 (MovUtl::move(MovUtl::access(other).d_a5)),
    d_a6 (MovUtl::move(MovUtl::access(other).d_a6)),
    d_a7 (MovUtl::move(MovUtl::access(other).d_a7)),
    d_a8 (MovUtl::move(MovUtl::access(other).d_a8)),
    d_a9 (MovUtl::move(MovUtl::access(other).d_a9)),
    d_a10(MovUtl::move(MovUtl::access(other).d_a10)),
    d_a11(MovUtl::move(MovUtl::access(other).d_a11)),
    d_a12(MovUtl::move(MovUtl::access(other).d_a12)),
    d_a13(MovUtl::move(MovUtl::access(other).d_a13)),
    d_a14(MovUtl::move(MovUtl::access(other).d_a14))
    {
    ++moveConstructorInvocations;
    }

    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
            bslma::Allocator     *alloc)
    : d_ilsum(0), d_allocator_p(alloc){}

    ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il)
         : d_ilsum(0), d_allocator_p(alloc)
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG1)  a1,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG2)  a2,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG3)  a3,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG4)  a4,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG5)  a5,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG6)  a6,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG7)  a7,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG8)  a8,
                                BSLS_COMPILERFEATURES_FORWARD_REF(ARG9)  a9)
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
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
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
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
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11, class ARG12>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
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
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
           d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11, class ARG12,
               class ARG13>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
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
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
           d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
           d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13))
     {
       for (int i : il) d_ilsum+=i;
     }

     template <class ARG1,  class ARG2,  class ARG3,
               class ARG4,  class ARG5,  class ARG6,
               class ARG7,  class ARG8,  class ARG9,
               class ARG10, class ARG11, class ARG12,
               class ARG13, class ARG14>
     ConstructTestTypeAllocArgTIL(bsl::allocator_arg_t       ,
                                bslma::Allocator     *alloc,
                                std::initializer_list<int> il,
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
         : d_ilsum(0), d_allocator_p(alloc),
           d_a1(BSLS_COMPILERFEATURES_FORWARD(ARG1, a1)),
           d_a2(BSLS_COMPILERFEATURES_FORWARD(ARG2, a2)),
           d_a3(BSLS_COMPILERFEATURES_FORWARD(ARG3, a3)),
           d_a4(BSLS_COMPILERFEATURES_FORWARD(ARG4, a4)),
           d_a5(BSLS_COMPILERFEATURES_FORWARD(ARG5, a5)),
           d_a6(BSLS_COMPILERFEATURES_FORWARD(ARG6, a6)),
           d_a7(BSLS_COMPILERFEATURES_FORWARD(ARG7, a7)),
           d_a8(BSLS_COMPILERFEATURES_FORWARD(ARG8, a8)),
           d_a9(BSLS_COMPILERFEATURES_FORWARD(ARG9, a9)),
           d_a10(BSLS_COMPILERFEATURES_FORWARD(ARG10, a10)),
           d_a11(BSLS_COMPILERFEATURES_FORWARD(ARG11, a11)),
           d_a12(BSLS_COMPILERFEATURES_FORWARD(ARG12, a12)),
           d_a13(BSLS_COMPILERFEATURES_FORWARD(ARG13, a13)),
           d_a14(BSLS_COMPILERFEATURES_FORWARD(ARG14, a14))
     {
       for (int i : il) d_ilsum+=i;
     }

};

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConstructTestTypeAllocArgTIL> : bsl::true_type { };

}  // close namespace bslma

namespace bslmf {

template <>
struct UsesAllocatorArgT<ConstructTestTypeAllocArgTIL> : bsl::true_type { };

}  // close namespace bslmf
}  // close enterprise namespace
int ConstructTestTypeAllocArgTIL::copyConstructorInvocations       = 0;
int ConstructTestTypeAllocArgTIL::moveConstructorInvocations       = 0;

// FREE OPERATORS
bool operator==(const ConstructTestTypeAllocArgTIL& lhs,
             const ConstructTestTypeAllocArgTIL& rhs)
{
 return lhs.d_ilsum == rhs.d_ilsum &&
        lhs.d_a1.d_value  == rhs.d_a1.d_value &&
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
bool createdAlike(const ConstructTestTypeAllocArgTIL& lhs,
               const ConstructTestTypeAllocArgTIL& rhs)
{
 return lhs.d_a1.d_copied_count  == rhs.d_a1.d_copied_count &&
        lhs.d_a2.d_copied_count  == rhs.d_a2.d_copied_count &&
        lhs.d_a3.d_copied_count  == rhs.d_a3.d_copied_count &&
        lhs.d_a4.d_copied_count  == rhs.d_a4.d_copied_count &&
        lhs.d_a5.d_copied_count  == rhs.d_a5.d_copied_count &&
        lhs.d_a6.d_copied_count  == rhs.d_a6.d_copied_count &&
        lhs.d_a7.d_copied_count  == rhs.d_a7.d_copied_count &&
        lhs.d_a8.d_copied_count  == rhs.d_a8.d_copied_count &&
        lhs.d_a9.d_copied_count  == rhs.d_a9.d_copied_count &&
        lhs.d_a10.d_copied_count == rhs.d_a10.d_copied_count &&
        lhs.d_a11.d_copied_count == rhs.d_a11.d_copied_count &&
        lhs.d_a12.d_copied_count == rhs.d_a12.d_copied_count &&
        lhs.d_a13.d_copied_count == rhs.d_a13.d_copied_count &&
        lhs.d_a14.d_copied_count == rhs.d_a14.d_copied_count;
}

const my_Class1     V1(1);
const my_Class2     V2(2);
const my_Class3     V4(4);
const my_Class4     V5(5);
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

struct Swappable {

    // PUBLIC CLASS DATA
    static int s_swapCalled;

    // PUBLIC DATA
    int d_value;

    // CLASS METHODS
    static bool swapCalled()
    {
        return 0 != s_swapCalled;
    }

    static void swapReset()
    {
        s_swapCalled = 0;
    }

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
    my_ClassDef d_def;

    // CLASS METHODS
    static bool swapCalled()
    {
        return 0 != s_swapCalled;
    }

    static void swapReset()
    {
        s_swapCalled = 0;
    }

    // CREATORS
    explicit SwappableAA(int v, bslma::Allocator *a = 0)
    {
        d_def.d_value = v;
        d_def.d_allocator_p = a;
    }

    SwappableAA(const SwappableAA& rhs, bslma::Allocator *a = 0) {
       d_def.d_value = rhs.d_def.d_value;
       d_def.d_allocator_p = a;
    }

    SwappableAA(bslmf::MovableRef<SwappableAA> other, bslma::Allocator *a = 0)
    {

        SwappableAA& otherRef = MovUtl::access(other);
        d_def.d_value = otherRef.d_def.d_value;
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
struct UsesBslmaAllocator<SwappableAA> : bsl::true_type { };

}  // close namespace bslma
} // close namespace BloombergLP
// ======================
// macros TEST_EMPLACE*
// ======================

#define TEST_EMPLACE(expArgs)                                                 \
{                                                                             \
  ConstructTestTypeNoAlloc EXP expArgs ;                                      \
  bsl::optional<ConstructTestTypeNoAlloc> obj;                                \
  int CCI = ConstructTestTypeNoAlloc::copyConstructorInvocations;             \
  int MCI = ConstructTestTypeNoAlloc::moveConstructorInvocations;             \
  obj . emplace expArgs;                                                      \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(CCI == ConstructTestTypeNoAlloc::copyConstructorInvocations);        \
  ASSERT(MCI == ConstructTestTypeNoAlloc::moveConstructorInvocations);        \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}                                                                             \
{                                                                             \
  ConstructTestTypeAlloc EXP expArgs ;                                        \
  bslma::TestAllocator oa("other", veryVeryVeryVerbose);                      \
  bsl::optional<ConstructTestTypeAlloc> obj(bsl::allocator_arg, &oa);         \
  int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;               \
  int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;               \
  obj . emplace expArgs;                                                      \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(&oa == obj .value().d_allocator_p);                                  \
  ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);          \
  ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);          \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}

#define TEST_EMPLACE_ARGT(expArgs, emplaceArgs)                               \
{                                                                             \
  bslma::TestAllocator oa("other", veryVeryVeryVerbose);                      \
  ConstructTestTypeAllocArgT EXP expArgs ;                                    \
  bsl::optional<ConstructTestTypeAllocArgT> obj(bsl::allocator_arg, &oa);     \
  int CCI = ConstructTestTypeAllocArgT::copyConstructorInvocations;           \
  int MCI = ConstructTestTypeAllocArgT::moveConstructorInvocations;           \
  obj . emplace emplaceArgs;                                                  \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(&oa == obj .value().d_allocator_p);                                  \
  ASSERT(CCI == ConstructTestTypeAllocArgT::copyConstructorInvocations);      \
  ASSERT(MCI == ConstructTestTypeAllocArgT::moveConstructorInvocations);      \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}
#define TEST_EMPLACE_ARGTIL(expArgs, emplaceArgs)                             \
{                                                                             \
  bslma::TestAllocator oa("other", veryVeryVeryVerbose);                      \
  ConstructTestTypeAllocArgTIL EXP expArgs ;                                  \
  bsl::optional<ConstructTestTypeAllocArgTIL> obj(bsl::allocator_arg, &oa);   \
  int CCI = ConstructTestTypeAllocArgTIL::copyConstructorInvocations;         \
  int MCI = ConstructTestTypeAllocArgTIL::moveConstructorInvocations;         \
  obj . emplace emplaceArgs;                                                  \
  ASSERT(EXP == obj .value());                                                \
  ASSERT(&oa == obj .value().d_allocator_p);                                  \
  ASSERT(CCI == ConstructTestTypeAllocArgTIL::copyConstructorInvocations);    \
  ASSERT(MCI == ConstructTestTypeAllocArgTIL::moveConstructorInvocations);    \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}

#define TEST_EQUAL_EMPTY(obj, type)                                           \
{                                                                             \
  type sourceObj;                                                             \
  ASSERT(!sourceObj.has_value());                                             \
  obj = sourceObj;                                                            \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_EMPTY_MOVE(obj, type)                                      \
{                                                                             \
  type sourceObj;                                                             \
  ASSERT(!sourceObj.has_value());                                             \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_ENGAGED(obj, otype, type, val)                             \
{                                                                             \
  otype sourceObj(type(val));                                                 \
 ASSERT(sourceObj.has_value());                                               \
  obj = sourceObj;                                                            \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(val == sourceObj.value().value());                                   \
}
#define TEST_EQUAL_ENGAGED_MOVE(obj, otype, type, val, expVal)                \
{                                                                             \
  otype sourceObj(type(val));                                                 \
  ASSERT(sourceObj.has_value());                                              \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(expVal == sourceObj.value().value());                                \
}
#define TEST_EQUAL_EMPTY_A(obj, type)                                         \
{                                                                             \
  type sourceObj(bsl::allocator_arg, &ta);                                    \
  ASSERT(!sourceObj.has_value());                                             \
  obj = sourceObj;                                                            \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(&oa == obj.get_allocator().mechanism());                             \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_EMPTY_MOVE_A(obj, type)                                    \
{                                                                             \
  type sourceObj(bsl::allocator_arg, &ta);                                    \
  ASSERT(!sourceObj.has_value());                                             \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(!obj.has_value());                                                   \
  ASSERT(&oa == obj.get_allocator().mechanism());                             \
  ASSERT(!sourceObj.has_value());                                             \
}
#define TEST_EQUAL_ENGAGED_A(obj, otype, type, val)                           \
{                                                                             \
  otype sourceObj(bsl::allocator_arg, &ta, type(val));                        \
  ASSERT(sourceObj.has_value());                                              \
  obj = sourceObj;                                                            \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(&oa == obj.get_allocator().mechanism());                             \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(val == sourceObj.value().value());                                   \
}
#define TEST_EQUAL_ENGAGED_MOVE_A(obj, otype, type, val, expVal)              \
{                                                                             \
  otype sourceObj(bsl::allocator_arg, &ta, type(val));                        \
  ASSERT(sourceObj.has_value());                                              \
  obj = MovUtl::move(sourceObj);                                              \
  ASSERT(obj.has_value());                                                    \
  ASSERT(val == obj.value().value());                                         \
  ASSERT(&oa == obj.get_allocator().mechanism());                             \
  ASSERT(sourceObj.has_value());                                              \
  ASSERT(expVal == sourceObj.value().value());                                \
}

#define TEST_COPY(valtype, optype, init, expArgs)                             \
{                                                                             \
  valtype EXP expArgs ;                                                       \
  int CCI = valtype::copyConstructorInvocations;                              \
  int MCI = valtype::moveConstructorInvocations;                              \
  optype obj init ;                                                           \
  ASSERT(EXP == obj.value());                                                 \
  ASSERT(CCI == ConstructTestTypeAllocArgTIL::copyConstructorInvocations);    \
  ASSERT(MCI == ConstructTestTypeAllocArgTIL::moveConstructorInvocations);    \
  ASSERT(createdAlike(EXP,obj.value()) == true);                              \
}
#define TEST_COPYA(valtype, optype, init, expArgs, alloc)                     \
{                                                                             \
  /* Expects allocator at end of argument list */                             \
  valtype EXP expArgs ;                                                       \
  optype obj init ;                                                           \
  ASSERT(EXP == obj.value());                                                 \
  ASSERT(alloc == obj.value().d_allocator_p);                                 \
}
#define TEST_CONSTRUCT(op, expArgs)                                           \
  {                                                                           \
    ConstructTestTypeNoAlloc EXP expArgs ;                                    \
    bsls::ObjectBuffer<ConstructTestTypeNoAlloc> rawBuf;                      \
    ConstructTestTypeNoAlloc  *objPtr =  rawBuf.address();                    \
    ConstructTestTypeNoAlloc&  mX     = *objPtr;                              \
    const ConstructTestTypeNoAlloc& X =  mX;                                  \
    memset(static_cast<void *>(&mX), 92, sizeof mX);                          \
    Obj:: op ;                                                                \
    ASSERT(EXP == X);                                                         \
    ASSERT(createdAlike(EXP,X) == true);                                      \
  }

#define TEST_CONSTRUCTA(op, expArgs1, expArgs2, alloc)                        \
  {                                                                           \
    /* Expects allocator at end of argument list */                           \
    ConstructTestTypeAlloc EXP expArgs1;                                      \
    bsls::ObjectBuffer<ConstructTestTypeAlloc> rawBuf;                        \
    ConstructTestTypeAlloc  *objPtr =  rawBuf.address();                      \
    ConstructTestTypeAlloc&  mX     = *objPtr;                                \
    const ConstructTestTypeAlloc& X = *objPtr;                                \
    memset(static_cast<void *>(&mX), 92, sizeof mX);                          \
    Obj:: op ;                                                                \
    ASSERT(EXP == X);                                                         \
    ASSERT(alloc == X.d_allocator_p);                                         \
    ASSERT(createdAlike(EXP,X) == true);                                      \
  }

#define TEST_MAKEOP(expArgs, alloc)                                           \
  {                                                                           \
    ConstructTestTypeNoAlloc EXP expArgs ;                                    \
    int CCI = ConstructTestTypeNoAlloc::copyConstructorInvocations;           \
    int MCI = ConstructTestTypeNoAlloc::moveConstructorInvocations;           \
    bsl::optional<ConstructTestTypeNoAlloc>  X =                              \
                       bsl::make_optional<ConstructTestTypeNoAlloc> expArgs;  \
    ASSERT(EXP == X.value());                                                 \
    ASSERT(CCI == ConstructTestTypeNoAlloc::copyConstructorInvocations);      \
    ASSERT(MCI == ConstructTestTypeNoAlloc::moveConstructorInvocations);      \
    ASSERT(createdAlike(EXP,X.value()) == true);                              \
  }                                                                           \
  {                                                                           \
    ConstructTestTypeAlloc EXP expArgs ;                                      \
    int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;             \
    int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;             \
    bsl::optional<ConstructTestTypeAlloc> X =                                 \
                      bsl::make_optional<ConstructTestTypeAlloc> expArgs ;    \
    ASSERT(EXP == X.value());                                                 \
    ASSERT(alloc == X.value().d_allocator_p);                                 \
    ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);        \
    ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);        \
    ASSERT(createdAlike(EXP,X.value()) == true);                              \
  }

#define TEST_ALLOCOP( op, expArgs, alloc)                                     \
  {                                                                           \
    ConstructTestTypeAlloc EXP expArgs ;                                      \
    int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;             \
    int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;             \
    bsl::optional<ConstructTestTypeAlloc> X =                                 \
                      bsl::alloc_optional<ConstructTestTypeAlloc> op;         \
    ASSERT(EXP == X.value());                                                 \
    ASSERT(alloc == X.value().d_allocator_p);                                 \
    ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);        \
    ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);        \
    ASSERT(createdAlike(EXP,X.value()) == true);                              \
  }

#define TEST_HASH_EMPTY(ValueType)                                            \
  {                                                                           \
      bsl::optional<ValueType> X;                                             \
      ASSERTV(!X.has_value());                                                \
      const size_t hashValue_1 = bslh::Hash<>()(X);                           \
      const size_t hashValue_2 = bslh::Hash<>()(false);                       \
      ASSERTV(hashValue_1, hashValue_2, hashValue_1 == hashValue_2);          \
  }
#define TEST_HASH_ENGAGED(ValueType, init)                                    \
  {                                                                           \
      bsl::optional<ValueType> X init;                                        \
      ValueType Y init;                                                       \
      ASSERTV(X.has_value());                                                 \
      ASSERTV(X.value() == Y);                                                \
      const size_t hashValue_1 = bslh::Hash<>()(X);                           \
      bslh::DefaultHashAlgorithm hasher;                                      \
      hashAppend(hasher, true);                                               \
      hashAppend(hasher, Y);                                                  \
      const size_t hashValue_2 = static_cast<size_t>(hasher.computeHash());   \
      ASSERTV(hashValue_1, hashValue_2, hashValue_1 == hashValue_2);          \
  }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

void bslstl_optional_test1()
{
    // --------------------------------------------------------------------
    // TESTING DISENGAGED CONSTRUCTORS AND BASIC ACCESSORS
    //   This test will verify that the construction of a disengaged optional
    //   is working as expected.  Also, we test that the basic accessors are
    //   working as expected.
    //
    // Concerns:
    //   * The default constructor must create a disengaged object.
    //   * Constructor taking nullopt_T must create a disengaged object
    //   * Optional's allocator is the default allocator
    //
    // Plan:
    //   Conduct the test using 'int' (does not use allocator) and
    //   'bsl::string' (uses allocator) for 'TYPE'.  (Check that
    //   'ValueType' is the right size in each case.)
    //
    //   First, verify the default constructor by testing that the
    //   resulting object is null. For bsl::string, verify that the
    //   resulting object's allocator is the default allocator.
    //
    //   Next, verify the constructor taking nullopt_t by testing that the
    //   resulting object is null. For bsl::string, verify that the
    //   resulting object's allocator is the default allocator.
    //
    //   Note that the destructor is exercised on each configuration as the
    //   object being tested leaves scope.
    //
    //   Invoke all const methods on a const object.
    //
    // Testing:
    //   typedef TYPE value_type;
    //   Optional();
    //   Optional(nullopt_t);
    //   ~Optional();
    //   bool has_value() const;
    //   allocator_type get_allocator() const;
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING PRIMARY MANIPULATORS AND BASIC ACCESSORS"
                       "\n================================================\n");

    if (verbose) printf( "\nUsing 'bsl::optional<int>'.\n");
    {
        typedef int                            ValueType;
        typedef bsl::optional<ValueType> Obj;

        ASSERT(sizeof(ValueType) == sizeof(Obj::value_type));

        if (veryVerbose) printf("\tTesting default constructor.\n" );

        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(0 == da.numBlocksTotal());
        }

        if (veryVerbose) printf( "\tTesting nullopt_t constructor.\n");

        {
            bslma::TestAllocator da("nullopt_t", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX = Obj(nullopt);
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(0 == da.numBlocksTotal());
        }
    }

    if (verbose) printf( "\nUsing 'bsl::optional<bsl::string>'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::string                     ValueType;
        typedef bsl::optional<ValueType>        Obj;


        ASSERT(sizeof(ValueType) == sizeof(Obj::value_type));

        if (veryVerbose)
            printf( "\tTesting default constructor with default allocator.\n");

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bslma::TestAllocatorMonitor dam(&da);

            Obj mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(X.get_allocator().mechanism() == &da);
            ASSERT(dam.isTotalSame());

        }
        if (veryVerbose) printf( "\tTesting nullopt_t constructor with default "
                                 "allocator.\n");
        {
            bslma::TestAllocator scratch("scratchnull", veryVeryVeryVerbose);

            bslma::TestAllocatorMonitor dam(&da);

            Obj mX = Obj(nullopt);
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(X.get_allocator().mechanism() == &da);
            ASSERT(dam.isTotalSame());
        }
    }
}
void bslstl_optional_test2()
{
    // --------------------------------------------------------------------
    // TESTING CONVERSION TO BOOL
    //   This test will verify that the conversion to bool works as expected.
    //   The test relies on reset and emplace member functions, as well as
    //   construction from a value_type.
    //
    // Concerns:
    //   * A disengaged optional when converted to bool evaluates to false
    //   * An engaged optional when converted to bool evaluates to true
    //
    // Plan:
    //   Conduct the test using 'int' (does not use allocator) and
    //   'bsl::string' (uses allocator) for 'TYPE'.
    //
    //   Create disengaged optional of each type and verify they
    //   evaluate to false when converted to bool.
    //
    //   Emplace a value in each optional and verify they evaluate to true
    //   when converted to bool.
    //
    //   Call reset() on each optional and verify they evaluate to false
    //   when converted to bool.
    //
    //   Finally, create engaged optional of each type and verify they
    //   evaluate to true when converted to bool.
    //
    //   Note that the destructor is exercised on each configuration as the
    //   object being tested leaves scope.
    //
    //   Invoke const methods on a const object.
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

    if (verbose) printf(
                       "\nTESTING CONVERSION TO BOOL "
                       "\n================================================\n");

    if (verbose) printf( "\nUsing 'bsl::optional<int>'.\n");
    {
        typedef int                            ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(!X);
            ASSERT(false == X.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace(1);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.emplace(3);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX = Obj(2);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
    }
    if (verbose) printf( "\nUsing 'bsl::optional< const int>'.\n");
    {
        typedef const int               ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(!X);
            ASSERT(false == X.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace(1);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.emplace(3);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX = Obj(2);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
    }

    if (verbose) printf( "\nUsing 'bsl::optional<bsl::string>'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::string                    ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(!X);
            ASSERT(false == X.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");

            mX.emplace("tralala");
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.emplace("");
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX = Obj(ValueType("tralala"));
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
    }
    if (verbose) printf( "\nUsing 'bsl::optional<const bsl::string>'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        typedef const bsl::string      ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(!X);
            ASSERT(false == X.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");

            mX.emplace("tralala");
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.emplace("");
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX = Obj(ValueType("tralala"));
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
    }
}
void bslstl_optional_test3()
{
    // --------------------------------------------------------------------
    // TESTING RESET FUNCTIONALITY
    //   This test will verify that the reset function works as expected.
    //   The test relies on constructors, has_value and emplace member functions.
    //
    // Concerns:
    //   * A disengaged optional can be reset.
    //   * An engaged optional after reset is disengaged.
    //   * Calling reset does not modify the allocator
    //   * Reset works on optional containing a constant type
    //
    // Plan:
    //   Conduct the test using 'int' (does not use allocator) and
    //   'bsl::string' (uses allocator) for 'TYPE'. Run tests for const vesions
    //   of the two types.
    //
    //   Create disengaged optional of each type. Call reset on an disengaged
    //   optional. Check that the optional is disengaged by calling has_value.
    //
    //   Emplace a value in each optional. Call reset on an engaged
    //   optional. Check that the optional is disengaged by calling has_value.
    //
    //   Create a optional<bsl::string> with an allocator. Call reset. Check
    //   that the allocator has not changed.
    //
    // Testing:
    //   void reset();
    //
    //   bool has_value() const;
    //   optional(const T &);
    //   optional(bsl::allocator_arg_t, allocator_type basicAllocator,
    //            const optional& original;
    //   emplace(const T &);
    //   allocator_type get_allocator() const;
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING RESET MEMBER FUNCTION "
                       "\n================================================\n");

    if (verbose) printf( "\nUsing 'bsl::optional<int>'.\n");
    {
        typedef int                            ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace(1);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
    }
    if (verbose) printf( "\nUsing 'bsl::optional< const int>'.\n");
    {
        typedef const int ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace(1);
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());
        }
    }

    if (verbose) printf( "\nUsing 'bsl::optional<bsl::string>'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::string                    ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace("tralala");
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());


            Obj nX = Obj(bsl::allocator_arg, bsl::allocator<char>(&oa), mX);
            ASSERT(mX.get_allocator().mechanism() == &da);
            ASSERT(nX.get_allocator().mechanism() == &oa);
            nX.reset();
            ASSERT(!nX);
            ASSERT(false == nX.has_value());
            ASSERT(nX.get_allocator().mechanism() == &oa);
        }
    }
    if (verbose) printf( "\nUsing 'bsl::optional< const bsl::string>'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef const bsl::string  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace("tralala");
            ASSERT(mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(!mX);
            ASSERT(false == mX.has_value());


            Obj nX = Obj(bsl::allocator_arg, bsl::allocator<char>(&oa), mX);
            ASSERT(mX.get_allocator().mechanism() == &da);
            ASSERT(nX.get_allocator().mechanism() == &oa);
            nX.reset();
            ASSERT(!nX);
            ASSERT(false == nX.has_value());
            ASSERT(nX.get_allocator().mechanism() == &oa);
        }
    }
}
void bslstl_optional_test4()
{
    // --------------------------------------------------------------------
    // TESTING VALUE FUNCTIONALITY
    //   This test will verify that the value function works as expected.
    //   The test relies on constructors and emplace member functions.
    //
    // Concerns:
    //   * Calling value() on a engaged optional returns the value.
    //   * Calling value() on a disengaged optional throws bad_optional_access
    //     exception.
    //   * It is possible to call value() on a constant optional object
    //   * It is possible to call value() on a temporary optional object
    //   * It is possible to modify non constant optional through call of
    //     value()
    //   * It is not possible to modify non constant optional through call of
    //     value().
    //     Note that this requires tests which check for compilation errors.
    //   * It is possible to call value() on a temporary optional object
    //   * It is not possible to modify non constant optional of const type
    //     through call of value(). Note that this requires tests which check
    //     for compilation errors.
    //   * It is not possible to modify a constant optional of const type
    //     through call of value(). Note that this requires tests which check
    //     for compilation errors.
    //
    // Plan:
    //   Conduct the test using 'int' (does not use allocator) and
    //   'bsl::string' (uses allocator) for 'TYPE'.
    //
    //   Create disengaged optional of each type. Call value on a disengaged
    //   optional. Check that the bad_optional_access exception is thrown.
    //
    //   Emplace a value in each optional. Call value on an engaged
    //   optional. Check that the value is correct.
    //
    //   Bind const optional reference to optional object. Call value and check
    //   that the value is correct.
    //
    //   Modify the retrieved value through the returned reference. Call value
    //   on the optional object and check that the value is correct.
    //
    //   Call reset on the optional object. Call value() and check that the
    //   the correct exception is thrown.
    //
    //   Call value on a disengaged temporary optional object. Check that the
    //   correct exception is thrown.
    //
    //   Call value on an engaged temporary optional object. Check that the
    //   correct value is returned.
    //
    //   Make sure no unexpected exception is thrown.
    //
    // Testing:
    //   TYPE& value() &;
    //   const TYPE& value() & const;
    //   TYPE&& value() &&;
    //   const TYPE&& value() && const;
    //
    //   void reset();
    //   optional(const T &);
    //   optional(bsl::allocator_arg_t, allocator_type basicAllocator,
    //            const optional& original;
    //   emplace(const T &);
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING VALUE MEMBER FUNCTION "
                       "\n=============================\n");

    if (verbose) printf( "\nUsing 'bsl::optional<int>'.\n");
    bool unexpected_exception_thrown = false;
    try
    {
        typedef int                            ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        {
            Obj mX; const Obj& X = mX;

            bool bad_optional_exception_caught = false;
            try{ mX.value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            mX.emplace(3);
            ASSERT(mX.value() == 3);
            ASSERT(X.value() == 3);

            int& ri = mX.value();
            ri = 7;

            ASSERT(mX.value() == 7);

            mX.reset();
            try{ mX.value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            try{ Obj().value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            int i = Obj(4).value();
            ASSERT(i == 4);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUE)
            X.value() = 2; // this should not compile 1
#endif
            try{ CObj().value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            int j = CObj(4).value();
            ASSERT(j == 4);
        }
    } catch (...)
    {
      unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);

    if (verbose) printf( "\nUsing 'bsl::optional< const int>'.\n");
    unexpected_exception_thrown = false;
    try
    {
        typedef const int ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        {
            Obj mX; const Obj& X = mX;

            bool bad_optional_exception_caught = false;
            try{ mX.value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            mX.emplace(3);
            ASSERT(mX.value() == 3);
            ASSERT(X.value() == 3);

            ValueType& ri = mX.value();
            ASSERT(ri == 3);

            mX.reset();
            try{ mX.value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            try{ Obj().value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            int i = Obj(4).value();
            ASSERT(i == 4);

            try{ CObj().value();}
            catch (bsl::bad_optional_access &)
            {
              bad_optional_exception_caught = true;
            }
            ASSERT(bad_optional_exception_caught);
            bad_optional_exception_caught = false;

            int j = CObj(4).value();
            ASSERT(j == 4);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUE)
            int& bad1 = mX.value(); // this should not compile 2
            int& bad2 = X.value(); // this should not compile 3
            int& bad3 = Obj(4).value(); // this should not compile 4
            int& bad4 = CObj(4).value(); // this should not compile 5
#endif
        }
    } catch (...)
    {
      unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);

    unexpected_exception_thrown = false;
    if (verbose) printf( "\nUsing 'bsl::optional<const bsl::string>'.\n");
    try {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef const bsl::string                    ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;

        Obj mX(bsl::allocator_arg, &oa); const Obj& X = mX;

        bool bad_optional_exception_caught = false;
        try{ mX.value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        mX.emplace("test string 1");
        ASSERT(mX.value() == "test string 1");
        ASSERT(X.value() == "test string 1");

        mX.emplace("test string 2");
        ValueType& ri = mX.value();
        ASSERT(ri == "test string 2");
        ASSERT(ri.get_allocator().mechanism() == &oa);

        bsl::string ci = mX.value();
        ASSERT(ci == "test string 2");
        // a copy gets a default allocator
        ASSERT(ci.get_allocator().mechanism() == &da);

        bsl::string ci2 = X.value();
        ASSERT(ci2 == "test string 2");
        // a copy gets a default allocator
        ASSERT(ci2.get_allocator().mechanism() == &da);



        mX.reset();
        try{ mX.value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        try{ Obj().value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        ValueType i = Obj(bsl::allocator_arg, &oa, "test string 4").value();
        ASSERT(i == "test string 4");

        try{ CObj().value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        bsl::string j = CObj(bsl::allocator_arg, &oa, "test string 6").value();
        ASSERT(j == "test string 6");
        // todo : add a test that check const move returns a const && objects
        // and what ever allocator is deemed appropriate
        ASSERT(j.get_allocator().mechanism() == &da);

#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUE)
        bsl::string& bad1 = mX.value(); // this should not compile 6
        bsl::string& bad2 = X.value(); // this should not compile 7
        bsl::string& bad3 = Obj("test string 6").value();
            // this should not compile 8
        bsl::string& bad4 = CObj("test string 6").value();
          // this should not compile 9
#endif
    }catch (...)
    {
      unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);
    unexpected_exception_thrown = false;
    if (verbose) printf( "\nUsing 'bsl::optional<bsl::string>'.\n");
    try {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::string                    ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;

        Obj mX(bsl::allocator_arg, &oa); const Obj& X = mX;

        bool bad_optional_exception_caught = false;
        try{ mX.value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        mX.emplace("test string 1");
        ASSERT(mX.value() == "test string 1");
        ASSERT(X.value() == "test string 1");

        bsl::string& ri = mX.value();
        ASSERT(ri == "test string 1");
        ASSERT(ri.get_allocator().mechanism() == &oa);
        ri = "test string 2";
        // original value didn't change
        ASSERT(mX.value() == "test string 2");

        bsl::string ci = mX.value();
        ASSERT(ci == "test string 2");
        // a copy gets a default allocator
        ASSERT(ci.get_allocator().mechanism() == &da);

        bsl::string ci2 = X.value();
        ASSERT(ci2 == "test string 2");
        // a copy gets a default allocator
        ASSERT(ci2.get_allocator().mechanism() == &da);



        mX.reset();
        try{ mX.value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        try{ Obj().value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;

        bsl::string i = Obj(bsl::allocator_arg, &oa, "test string 4").value();
        ASSERT(i == "test string 4");
        // move uses the original default allocator
        ASSERT(i.get_allocator().mechanism() == &oa);

#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUE)
        X.value() = "test string 5"; // this should not compile 10
#endif
        try{ CObj().value();}
        catch (bsl::bad_optional_access &)
        {
          bad_optional_exception_caught = true;
        }
        ASSERT(bad_optional_exception_caught);
        bad_optional_exception_caught = false;
        CObj temp = CObj(bsl::allocator_arg, &oa, "test string 6");
        bsl::string j = MovUtl::move(temp).value();
        ASSERT(j == "test string 6");
        ASSERT(j.get_allocator().mechanism() == &da);
    }catch (...)
    {
      unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);


#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUE)
    {
         typedef const int                    ValueType;
         typedef bsl::optional<ValueType> Obj;
         typedef const Obj CObj;
         int&& bad1 = Obj("test string 6").value();
           // this should not compile 11
         int&& bad2 = CObj("test string 6").value();
           // this should not compile 12
    }
    {
        typedef const bsl::string                    ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        bsl::string&& bad3 = Obj("test string 6").value();
          // this should not compile 13
        bsl::string&& bad4 = CObj("test string 6").value();
          // this should not compile 14
    }
#endif

    unexpected_exception_thrown = false;
    if (verbose) printf( "\nUsing 'my_Class3'.\n");
    try {

        typedef my_Class3                   ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;

        CObj temp = CObj(V4);
        ValueType j = MovUtl::move(temp).value();
        //make sure const&& constructor was called
        ASSERT(24 == j.d_def.d_value);
    }catch (...)
    {
        unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);

    unexpected_exception_thrown = false;
    if (verbose) printf( "\nUsing 'my_Class4'.\n");
    try {

        typedef my_Class4   ValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;

        CObj temp = CObj(V5);
        ValueType j = MovUtl::move(temp).value();
        //make sure const&& constructor was called
        ASSERT(25 == j.d_def.d_value);
    }catch (...)
    {
        unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);
#endif


}
void bslstl_optional_test5()
{
    // --------------------------------------------------------------------
    // TESTING VALUE_OR FUNCTIONALITY
    //   This test will verify that the value_or function works as expected.
    //   The test relies on constructors and emplace member functions.
    //
    // Concerns:
    //   * Calling value_or() on a engaged optional returns the value in
    //     optional.
    //   * Calling value_or() on a disengaged optional returns the specified
    //     value.
    //   * It is possible to call value_or() on a constant optional object
    //   * It is possible to call value_or() on a temporary optional object
    //   * if no allocator is specified, the default allocator is used.
    //   * if an allocator is specified, that allocator is used
    //   * calling value_or does not modify the allocator of optional
    //   * It is not possible to modify non constant optional through call of
    //     value_or()
    //     Note that this requires tests which check for compilation errors.
    //
    // Plan:
    //   Conduct the test using 'int' (does not use allocator) and
    //   'bsl::string' (uses allocator) for 'TYPE'.
    //
    //   Create disengaged optional of each type. Call value_or on a disengaged
    //   optional. Check that the value returned and the allocator used is
    //   correct.
    //   Check that the optional object has not changed.
    //
    //   Emplace a value in each optional. Call value_or on an engaged
    //   optional. Check that the value is correct. Check that the value
    //   returned and the allocator used is correct. Check that the optional
    //   object has not changed.
    //
    //   Bind const optional reference to optional object. Call value_or and
    //   check that the value is correct.
    //
    //   Call value_or on a disengaged temporary optional object. Check that
    //   the value returned and the allocator used is correct.
    //
    //   Call value_or on an engaged temporary optional object. Check that the
    //   value returned and the allocator used is correct.
    //
    //   Call value_or on engaged optional object using move semantic. Check
    //   that the value returned and the allocator used is correct. Check the
    //   original optional's value has been moved from.
    //
    //
    // Testing:
    //   bool value_or() const;
    //
    //   void reset();
    //   optional(const T &);
    //   optional(bsl::allocator_arg_t, allocator_type basicAllocator, const
    //              optional& original;
    //   emplace(const T &);
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING VALUE_OR MEMBER FUNCTION "
                       "\n=================================\n");

    if (verbose) printf( "\nUsing 'bsl::optional<int>'.\n");
    {
        typedef int                            ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
            Obj mX; const Obj& X = mX;

#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUEOR)
            ValueType &i0 = mX.value_or(2); // this should not compile 1
#endif
            const ValueType &i1 = X.value_or(2);
            ASSERT(!X.has_value());
            ASSERT(i1 == 2);

            mX.emplace(3);
            ASSERT(mX.value() == 3);
            const ValueType &i2 = mX.value_or(i1);
            ASSERT(mX.value() == 3);
            ASSERT(i2 == 3);

            const ValueType &i3 = Obj().value_or(77);
            ASSERT(i3 == 77);

            ValueType i4 = Obj(44).value_or(77);
            ASSERT(i4 == 44);

        }
    }

    if (verbose) printf( "\nUsing 'bsl::optional<bsl::string>'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef bsl::string                    ValueType;
        typedef bsl::optional<ValueType> Obj;
        if (verbose) printf( "\n value tests'.\n");
        {
             Obj mX; const Obj& X = mX;

#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUEOR)
             ValueType &i0 = mX.value_or("some string");
               // this should not compile 2
#endif
             const ValueType &i1 = X.value_or("string1");
             ASSERT(!X.has_value());
             ASSERT(i1 == "string1");

             mX.emplace("string2");
             ASSERT(mX.value() == "string2");
             const ValueType &i2 = mX.value_or("another string");
             ASSERT(mX.value() == "string2");
             ASSERT(i2 == "string2");

             const ValueType &i3 = Obj().value_or("string3");
             ASSERT(i3 == "string3");

             ValueType i4 = Obj("string4").value_or("string5");
             ASSERT(i4 == "string4");
         }
        if (verbose) printf( "\n allocator tests of non allocator extended "
            "value_or'.\n");
        {
            Obj mX(bsl::allocator_arg, &oa); const Obj& X = mX;

            const ValueType &i1 = X.value_or("string1");
            ASSERT(!X.has_value());
            ASSERT(X.get_allocator().mechanism() == &oa);
            ASSERT(i1 == "string1");
            ASSERT(i1.get_allocator().mechanism() == &da);

            mX.emplace("string2");
            ASSERT(mX.value() == "string2");
            const ValueType &i2 = mX.value_or("another string");
            ASSERT(mX.value() == "string2");
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(i2 == "string2");
            ASSERT(i2.get_allocator().mechanism() == &da);

            const ValueType &i3 = Obj().value_or("string3");
            ASSERT(i3 == "string3");
            ASSERT(i3.get_allocator().mechanism() == &da);

            const ValueType &i4 = Obj().value_or(mX.value());
            ASSERT(i4 == "string2");
            ASSERT(i4.get_allocator().mechanism() == &da);

            ValueType i5 = Obj(bsl::allocator_arg, &oa, "string4")
                                .value_or("string5");
            ASSERT(i5 == "string4");
            ASSERT(i5.get_allocator().mechanism() == &da);
        }
        if (verbose) printf( "\n allocator tests of allocator extended "
            "value_or'.\n");
        {
           Obj mX(bsl::allocator_arg, &oa); const Obj& X = mX;

           const ValueType &i1 = X.value_or(bsl::allocator_arg, &ta,
                                            "string1");
           ASSERT(!X.has_value());
           ASSERT(X.get_allocator().mechanism() == &oa);
           ASSERT(i1 == "string1");
           ASSERT(i1.get_allocator().mechanism() == &ta);

           mX.emplace("string2");
           ASSERT(mX.value() == "string2");
           const ValueType &i2 = mX.value_or(bsl::allocator_arg, &ta,
                                             "another string");
           ASSERT(mX.value() == "string2");
           ASSERT(mX.get_allocator().mechanism() == &oa);
           ASSERT(i2 == "string2");
           ASSERT(i2.get_allocator().mechanism() == &ta);

           const ValueType &i3 = Obj().value_or(bsl::allocator_arg, &ta,
                                                "string3");
           ASSERT(i3 == "string3");
           ASSERT(i3.get_allocator().mechanism() == &ta);

           const ValueType &i4 = Obj().value_or(bsl::allocator_arg, &ta,
                                                mX.value());
           ASSERT(i4 == "string2");
           ASSERT(i4.get_allocator().mechanism() == &ta);

           ValueType i5 = Obj(bsl::allocator_arg, &oa,
                              "string4").value_or(bsl::allocator_arg, &ta,
                                                  "string5");
           ASSERT(i5 == "string4");
           ASSERT(i5.get_allocator().mechanism() == &ta);
        }
    }

}
void bslstl_optional_test6()
{
    // --------------------------------------------------------------------
    // TESTING operator-> FUNCTIONALITY
    //   This test will verify that the operator-> works as expected.
    //
    // Concerns:
    //   * Calling operator-> on an engaged optional returns a pointer to the
    //     contained value. It is possible to modify the value through to the
    //     acquired pointer.
    //   * Calling operator-> on an engaged const optional returns a pointer
    //     the contained value. It is not possible to modify the value through
    //     to the acquired pointer.
    //     Note that the last test requires a compilation failure and needs to
    //     be explicitly enabled.
    //
    // Plan:
    //   Conduct the test using 'my_class1', 'const my_class1' (does not use
    //   allocator) and 'my_class1' and 'const my_class1' (uses allocator) for
    //   'TYPE'.
    //
    //   Create engaged optional of each type. Call operator-> and check that
    //   the value and allocator, if any, of the object is correct.
    //
    //   Modify the optional through the return pointer. Check the value
    //   of the optional is correct.
    //
    //   Bind const optional reference to optional object. Call operator-> and
    //   check that the value of the object and the allocator is correct.
    //
    //   Check the value of the optional object can not be modified through
    //   operator-> const and through operator-> for const types. This requires
    //   compilation failures.
    //
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING operator-> MEMBER FUNCTION "
                       "\n==================================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                            ValueType;
        typedef const my_Class1                      ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const CObj CObjC;

        {
            Obj mX(ValueType(4));
            CObj cobjX(ValueType(8));
            ObjC objcX(ValueType(9));
            CObjC cobjcX(ValueType(10));

            ASSERT(mX->d_def.d_value == 4);
            ASSERT(cobjX->d_def.d_value == 8);
            ASSERT(objcX->d_def.d_value == 9);
            ASSERT(cobjcX->d_def.d_value == 10);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_OPERATOR_ARROW1)
            cobjX->d_def.d_value = 6; // this should not compile 1/6
            objcX->d_def.d_value = 6; // this should not compile 2/6
            cobjcX->d_def.d_value = 6; // this should not compile 3/6
            CObj(5)->d_def.d_value = 7; // this should not compile 4/6
            ObjC(5)->d_def.d_value = 7; // this should not compile 5/6
            CObjC(5)->d_def.d_value = 7; // this should not compile 6/6
#endif
            mX->d_def.d_value = 6;
            ASSERT(mX.value().d_def.d_value == 6);
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        typedef my_Class2                            ValueType;
        typedef const my_Class2                      ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const CObj CObjC;

        {
            Obj mX(bsl::allocator_arg, &oa, V2);
            CObj cobjX(bsl::allocator_arg, &ta, V2);
            ObjC objcX(ValueType(9));
            CObjC cobjcX(ValueType(10));

            ASSERT(mX->d_def.d_value == 2);
            ASSERT(mX->d_def.d_allocator_p == &oa);
            ASSERT(cobjX->d_def.d_value == 2);
            ASSERT(cobjX->d_def.d_allocator_p == &ta);
            ASSERT(objcX->d_def.d_value == 9);
            ASSERT(objcX->d_def.d_allocator_p == &da);
            ASSERT(cobjcX->d_def.d_value == 10);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_OPERATOR_ARROW2)
            cobjX->d_def.d_value = 6; // this should not compile 1/6
            objcX->d_def.d_value = 6; // this should not compile 2/6
            cobjcX->d_def.d_value = 6; // this should not compile 3/6
            CObj(5)->d_def.d_value = 7; // this should not compile 4/6
            ObjC(5)->d_def.d_value = 7; // this should not compile 5/6
            CObjC(5)->d_def.d_value = 7; // this should not compile 6/6
#endif
            mX->d_def.d_value = 6;
            ASSERT(mX.value().d_def.d_value == 6);
       }
    }
}
void bslstl_optional_test7()
{
    // --------------------------------------------------------------------
    // TESTING operator* FUNCTIONALITY
    //   This test will verify that the operator* works as expected.
    //
    // Concerns:
    //   * Calling operator* on an engaged optional returns a reference to the
    //      contained value.
    //   * It is possible to modify the value through to the
    //     acquired pointer for non-const optional objects containing non-const
    //     type object. Similarly, it is not possible to modify the contained
    //     value through the returned reference in any other case
    //
    //   * in C++11, Calling operator* on an engaged const optional returns an
    //     if the optional object was an lvalue, and an rvalue reference if the
    //     optional object was an rvalue.
    //
    // Plan:
    //   Conduct the test using 'my_class1', 'const my_class1' (does not use
    //   allocator) and 'my_class1' and 'const my_class1' (uses allocator) for
    //   'TYPE'.
    //
    //   Create engaged optional of each type. Call operator* and check that
    //   the value and allocator, if any, of the object are correct.
    //
    //   Modify the non const optional of a non const type through the return
    //   reference.
    //   Check the value of the optional is correct.
    //
    //   Bind const optional reference to optional object. Call operator-> and
    //   check that the value of the object and the allocator is correct.
    //
    //   Check the value of the optional object can not be modified through
    //   operator-> const and through operator-> for const types. This requires
    //   compilation failures.
    //
    //
    // Testing:
    //   const T* operator->() const;
    //   T* operator->();
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING operator* MEMBER FUNCTION "
                       "\n==================================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                            ValueType;
        typedef const my_Class1                      ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef const Obj CObj;
        typedef bsl::optional<ConstValueType> ObjC;
        typedef const CObj CObjC;

        {
            Obj mX(ValueType(4));
            CObj cobjX(ValueType(8));
            ObjC objcX(ValueType(9));
            CObjC cobjcX(ValueType(10));

            ASSERT((*mX).d_def.d_value == 4);
            ASSERT((*cobjX).d_def.d_value == 8);
            ASSERT((*objcX).d_def.d_value == 9);
            ASSERT((*cobjcX).d_def.d_value == 10);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_OPERATOR_STAR1)
            (*cobjX).d_def.d_value = 6; // this should not compile 1/6
            (*objcX).d_def.d_value = 6; // this should not compile 2/6
            (*cobjcX).d_def.d_value = 6; // this should not compile 3/6
            (*CObj(5)).d_def.d_value = 7; // this should not compile 4/6
            (*ObjC(5)).d_def.d_value = 7; // this should not compile 5/6
            (*CObjC(5)).d_def.d_value = 7; // this should not compile 6/6
#endif
            (*mX).d_def.d_value = 6;
            ASSERT(mX.value().d_def.d_value == 6);
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
       bslma::TestAllocator da("default", veryVeryVeryVerbose);
       bslma::TestAllocator oa("other", veryVeryVeryVerbose);
       bslma::TestAllocator ta("third", veryVeryVeryVerbose);

       bslma::DefaultAllocatorGuard dag(&da);
       typedef my_Class2                            ValueType;
       typedef const my_Class2                      ConstValueType;
       typedef bsl::optional<ValueType> Obj;
       typedef const Obj CObj;
       typedef bsl::optional<ConstValueType> ObjC;
       typedef const CObj CObjC;

       {
           Obj mX(bsl::allocator_arg, &oa, V2);
           CObj cobjX(bsl::allocator_arg, &ta, V2);
           ObjC objcX(ValueType(9));
           CObjC cobjcX(ValueType(10));

           ASSERT((*mX).d_def.d_value == 2);
           ASSERT((*mX).d_def.d_allocator_p == &oa);
           ASSERT((*cobjX).d_def.d_value == 2);
           ASSERT((*cobjX).d_def.d_allocator_p == &ta);
           ASSERT((*objcX).d_def.d_value == 9);
           ASSERT((*objcX).d_def.d_allocator_p == &da);
           ASSERT((*cobjcX).d_def.d_value == 10);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_OPERATOR_STAR2)
           (*cobjX).d_def.d_value = 6; // this should not compile 1/6
           (*objcX).d_def.d_value = 6; // this should not compile 2/6
           (*cobjcX).d_def.d_value = 6; // this should not compile 3/6
           (*CObj(5)).d_def.d_value = 7; // this should not compile 4/6
           (*ObjC(5)).d_def.d_value = 7; // this should not compile 5/6
           (*CObjC(5)).d_def.d_value = 7; // this should not compile 6/6
#endif
           (*mX).d_def.d_value = 6;
           ASSERT(mX.value().d_def.d_value == 6);
       }
   }
}

void bslstl_optional_test8()
{
  // --------------------------------------------------------------------
  // TESTING emplace FUNCTIONALITY
  //   This test will verify that the emplace function works
  //   as expected.
  //
  // Concerns:
  //   * Calling emplace on a non-engaged optional creates a value type object
  //     using the optional's allocator and emplace arguments
  //   * Calling emplace on an engaged optional replaces the value type object
  //     with a new one created using the optional's allocator and emplace
  //     arguments
  //   * Calling emplace with no arguments creates a default constructed value
  //     type object
  //   * Calling emplace does not modify the allocator, even when called with
  //     an rvalue value type argument
  //   * emplace can not be used on a const qualified optional.
  //   * there are no unnecessary value type copies created
  //
  //
  // Plan:
  //
  //
  // Testing:
  //
  //   void emplace();
  //   void emplace(Args&&...);
  //
  //   void value();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING emplace MEMBER FUNCTION "
                       "\n==================================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj mX;
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;

            mX.emplace();
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 0 );

            ValueType other(3);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(other);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 3 );
            ASSERT(CCI == ValueType::copyConstructorInvocations -1 );
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            ValueType third(4);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(MovUtl::move(third));
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 4 );
            #if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(CCI == ValueType::copyConstructorInvocations );
            ASSERT(MCI == ValueType::moveConstructorInvocations -1);
            #endif

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            mX.emplace(6);
            ASSERT(mX.has_value());
            ASSERT(mX->d_def.d_value == 6 );
            ASSERT(CCI == my_Class1::copyConstructorInvocations);
            ASSERT(MCI == my_Class1::moveConstructorInvocations);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

       typedef my_Class2                 ValueType;
       typedef bsl::optional<ValueType> Obj;

       {
           Obj mX(bsl::allocator_arg, &oa);
           int CCI = ValueType::copyConstructorInvocations;
           int MCI = ValueType::moveConstructorInvocations;

           mX.emplace();
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 0 );
           ASSERT(mX.get_allocator().mechanism() == &oa );

           ValueType other(3, &da);
           ASSERT(other.d_def.d_allocator_p == &da );
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(other);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 3 );
           ASSERT(mX.get_allocator().mechanism() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations -1 );
           ASSERT(MCI == ValueType::moveConstructorInvocations);

           ValueType third(4, &da);
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(MovUtl::move(third));
           ASSERT(third.d_def.d_allocator_p == &da );
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 4 );
  #if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations -1);
  #endif
           ASSERT(mX.get_allocator().mechanism() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );

           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(6);
           ASSERT(mX.has_value());
           ASSERT(mX->d_def.d_value == 6 );
           ASSERT(mX.get_allocator().mechanism() == &oa );
           ASSERT(mX->d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

       typedef my_Class2a                 ValueType;
       typedef bsl::optional<ValueType> Obj;

       {
           Obj mX(bsl::allocator_arg, &oa);
           int CCI = ValueType::copyConstructorInvocations;
           int MCI = ValueType::moveConstructorInvocations;

           mX.emplace();
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 0 );
           ASSERT(mX.get_allocator().mechanism() == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);

           ValueType other(bsl::allocator_arg, &da, 3);
           ASSERT(other.d_data.d_def.d_allocator_p == &da );
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(other);
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 3 );
           ASSERT(mX.get_allocator().mechanism() == &oa );
           ASSERT(mX->d_data.d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations -1);
           ASSERT(MCI == ValueType::moveConstructorInvocations);

           ValueType third(bsl::allocator_arg, &da, 4);
           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(MovUtl::move(third));
           ASSERT(third.d_data.d_def.d_allocator_p == &da );
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 4 );
  #if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations -1);
  #endif
           ASSERT(mX.get_allocator().mechanism() == &oa );
           ASSERT(mX->d_data.d_def.d_allocator_p == &oa );

           CCI = ValueType::copyConstructorInvocations;
           MCI = ValueType::moveConstructorInvocations;
           mX.emplace(6);
           ASSERT(mX.has_value());
           ASSERT(mX->d_data.d_def.d_value == 6 );
           ASSERT(mX.get_allocator().mechanism() == &oa );
           ASSERT(mX->d_data.d_def.d_allocator_p == &oa );
           ASSERT(CCI == ValueType::copyConstructorInvocations);
           ASSERT(MCI == ValueType::moveConstructorInvocations);
      }
    }
    if (verbose) printf( "\nTesting var args emplace .\n");
    {
        {
          ConstructTestTypeNoAlloc EXP;
          bsl::optional<ConstructTestTypeNoAlloc> obj;
          int CCI = ConstructTestTypeNoAlloc::copyConstructorInvocations;
          int MCI = ConstructTestTypeNoAlloc::moveConstructorInvocations;
          obj.emplace();
          ASSERT(EXP == obj.value());
          ASSERT(CCI == ConstructTestTypeNoAlloc::copyConstructorInvocations);
          ASSERT(MCI == ConstructTestTypeNoAlloc::moveConstructorInvocations);
          ASSERT(createdAlike(EXP,obj.value()) == true);
        }
        {
          ConstructTestTypeAlloc EXP;
          bslma::TestAllocator oa("other", veryVeryVeryVerbose);
          bsl::optional<ConstructTestTypeAlloc> obj(bsl::allocator_arg, &oa);
          int CCI = ConstructTestTypeAlloc::copyConstructorInvocations;
          int MCI = ConstructTestTypeAlloc::moveConstructorInvocations;
          obj.emplace();
          ASSERT(EXP == obj .value());
          ASSERT(&oa == obj .value().d_allocator_p);
          ASSERT(CCI == ConstructTestTypeAlloc::copyConstructorInvocations);
          ASSERT(MCI == ConstructTestTypeAlloc::moveConstructorInvocations);
          ASSERT(createdAlike(EXP,obj.value()) == true);
        }
        {
          ConstructTestTypeAllocArgT EXP;
          bslma::TestAllocator oa("other", veryVeryVeryVerbose);
          bsl::optional<ConstructTestTypeAllocArgT>
                               obj(bsl::allocator_arg, &oa);
          int CCI = ConstructTestTypeAllocArgT::copyConstructorInvocations;
          int MCI = ConstructTestTypeAllocArgT::moveConstructorInvocations;
          obj.emplace();
          ASSERT(EXP == obj .value());
          ASSERT(&oa == obj .value().d_allocator_p);
          ASSERT(CCI ==
                     ConstructTestTypeAllocArgT::copyConstructorInvocations);
          ASSERT(MCI ==
                     ConstructTestTypeAllocArgT::moveConstructorInvocations);
          ASSERT(createdAlike(EXP,obj.value()) == true);
        }

        TEST_EMPLACE((VA1));
        TEST_EMPLACE((MovUtl::move(VA1)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2));
        TEST_EMPLACE((VA1, MovUtl::move(VA2)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2, VA3));
        TEST_EMPLACE((VA1, MovUtl::move(VA2), VA3));
        TEST_EMPLACE((VA1, VA2, MovUtl::move(VA3)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12)));

        TEST_EMPLACE((MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12,
                    MovUtl::move(VA13)));
        TEST_EMPLACE((VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12),
                    VA13));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa),
                          ());

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,MovUtl::move(VA1)),
                          (MovUtl::move(VA1)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,VA1),
                          (VA1));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2),
                          (MovUtl::move(VA1), VA2));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2)),
                          (VA1, MovUtl::move(VA2)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3),
                          (VA1, MovUtl::move(VA2), VA3));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)),
                           (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)));


        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)));

        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                           MovUtl::move(VA13)),
                          (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                           MovUtl::move(VA13)));
        TEST_EMPLACE_ARGT((bsl::allocator_arg, &oa,
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                           VA13),
                          (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                           VA13));
    }

}


void bslstl_optional_test9()
{
    // --------------------------------------------------------------------
    // TESTING emplace FUNCTIONALITY
    //   This test will verify that the intializer list emplace function works
    //   as expected.
    //
    // Concerns:
    //   * Calling emplace on a non-engaged optional creates a value type object
    //     using the optional's allocator and emplace arguments
    //   * Calling emplace on an engaged optional replaces the value type object
    //     with a new one created using the optional's allocator and emplace
    //     arguments
    //   * Whe using initializer_list, the correct value type constructors is
    //     selected
    //   * Multiple arguments are correctly forwarded.
    //   * emplace can not be used on a const qualified optional.
    //
    //
    // Plan:
    //
    // Testing:
    //
    //   void emplace(std::initializer_list<U>, Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING emplace MEMBER FUNCTION "
                       "\n==================================\n");
    if (verbose) printf( "\nTesting initializer list emplace .\n");
    {
        TEST_EMPLACE(({1,2,3}));

        TEST_EMPLACE(({1,2,3}, VA1));
        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2, VA3));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2), VA3));
        TEST_EMPLACE(({1,2,3}, VA1, VA2, MovUtl::move(VA3)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12)));

        TEST_EMPLACE(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12,
                    MovUtl::move(VA13)));
        TEST_EMPLACE(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12),
                    VA13));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             VA1, MovUtl::move(VA2), VA3),
                            ({1,2,3},
                             VA1, MovUtl::move(VA2), VA3));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3}),
                             ({1,2,3}));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                              MovUtl::move(VA1)),
                             ({1,2,3},
                              MovUtl::move(VA1)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             VA1),
                            ({1,2,3},
                             VA1));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             VA1, MovUtl::move(VA2)),
                            ({1,2,3},
                             VA1, MovUtl::move(VA2)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                          ({1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10)));


        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                          ({1,2,3},
                           MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                           MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                           MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                          ({1,2,3},
                           VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                           VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                           VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)));

        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                             MovUtl::move(VA13)),
                            ({1,2,3},
                             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                             MovUtl::move(VA13)));
        TEST_EMPLACE_ARGTIL((bsl::allocator_arg, &oa, {1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                            VA13),
                           ({1,2,3},
                            VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                            VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                            VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                            VA13));
    }
#if defined(BSLSTL_OPTIONAL_TEST_BAD_IL_EMPLACE)
    if (verbose) printf( "\nUsing 'ConstructTestTypeNoAlloc'.\n");
    {
        typedef ConstructTestTypeNoAlloc                  ValueType;
        typedef const ConstructTestTypeNoAlloc            ConstValueType;
        typedef const bsl::optional<ValueType> CObj;
        typedef const bsl::optional<ConstValueType> CObjC;
       {
            CObj bad1;
            bad1.emplace({1,2,3});  // this should not compile 1
            bad1.emplace({1,2,3}, VA1); // this should not compile 2
            bad1.emplace({1,2,3}, VA1, VA2); // this should not compile 3
            bad1.emplace({1,2,3}, VA1, VA2, VA3); // this should not compile 4
            bad1.emplace({1,2,3}, VA1, VA2, VA3, VA4);
                  // this should not compile 5

            CObjC bad2;
            bad2.emplace({1,2,3});  // this should not compile 6
            bad2.emplace({1,2,3}, VA1); // this should not compile 7
            bad2.emplace({1,2,3}, VA1, VA2); // this should not compile 8
            bad2.emplace({1,2,3}, VA1, VA2, VA3); // this should not compile 9
            bad2.emplace({1,2,3}, VA1, VA2, VA3, VA4);
                  // this should not compile 10

        }
    }
    if (verbose) printf( "\nUsing 'ConstructTestTypeAlloc'.\n");
    {
        typedef ConstructTestTypeAlloc                  ValueType;
        typedef const ConstructTestTypeAlloc            ConstValueType;
        typedef const bsl::optional<ValueType>          CObj;
        typedef const bsl::optional<ConstValueType>     CObjC;
        {
            CObj bad1;
            bad1.emplace();  // this should not compile 11
            bad1.emplace(VA1); // this should not compile 12
            bad1.emplace(VA1, VA2); // this should not compile 13
            bad1.emplace(VA1, VA2, VA3); // this should not compile 14
            bad1.emplace(VA1, VA2, VA3, VA4); // this should not compile 15

            CObjC bad2;
            bad2.emplace();  // this should not compile 16
            bad2.emplace(VA1); // this should not compile 17
            bad2.emplace(VA1, VA2); // this should not compile 18
            bad2.emplace(VA1, VA2, VA3); // this should not compile 19
            bad2.emplace(VA1, VA2, VA3, VA4); // this should not compile 20

        }
    }
#endif
}
void bslstl_optional_test10()
{
  // --------------------------------------------------------------------
  // TESTING operator=(nullopt_t) FUNCTIONALITY
  //   This test will verify that the operator=(nullopt_t) function works
  //   as expected.
  //
  // Concerns:
  //   * Calling operator=(nullopt_t) on an engaged optional makes the optional
  //     disengaged. The allocator, if any, doesn't change.
  //   * Calling operator=(nullopt_t) on a disengaged optional leaves the
  //     optional disengaged. The allocator, if any, doesn't change.
  //   * operator=(nullopt_t) can not be called on a const qualified optional.
  //   * operator=(nullopt_t) can be called on a non const qualified optional
  //     of a const qualified value type.
  //
  //
  // Plan:
  //
  //   Conduct the test using 'int' (does not use allocator) and
  //   'bsl::string' (uses allocator) for 'TYPE'.
  //
  //   Create disengaged optional of each type. Call operator=(nullopt_t) on a
  //   disengaged optional. Check that optional is still disengaged and that
  //   the allocator, if any, hasn't changed.
  //
  //   Emplace a value in each optional. Call operator=(nullopt_t) on the
  //   engaged optional. Check that optional is disengaged and that
  //   the allocator, if any, hasn't changed.
  //
  //   Repeat the tests for an optional of const qualified value types.
  //
  //   Call operator=(nullopt_t) on a const qualified optional. This requires
  //   compilation failure tests which are not enabled by default.
  //
  // Testing:
  //
  //   operator=(nullopt_t)
  //
  //
  //   void emplace();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING operator(nullopt_t) MEMBER FUNCTION "
                       "\n==================================\n");

    if (verbose) printf( "\nUsing 'int'.\n");
    {
        typedef int                  ValueType;
        typedef const int            ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        {
          Obj mX;
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          mX.emplace(3);
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          ObjC mcX;
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());

          mcX.emplace(3);
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());
       }
    }
    if (verbose) printf( "\nUsing 'bsl::string'.\n");
    {
        typedef bsl::string                  ValueType;
        typedef const bsl::string            ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        {
          Obj mX;
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          mX.emplace("aaa");
          mX = bsl::nullopt;
          ASSERT(!mX.has_value());

          ObjC mcX;
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());

          mcX.emplace("tralala");
          mcX = bsl::nullopt;
          ASSERT(!mcX.has_value());
       }
    }
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST)
    {
      if (verbose) printf( "\nUsing 'int'.\n");
      {
          typedef int                  ValueType;
          typedef bsl::optional<ValueType> Obj;
          const Obj mX;
          mX = bsl::nullopt; // this should not compile 1/
       }
      if (verbose) printf( "\nUsing 'bsl::string'.\n");
      {
          typedef bsl::string                 ValueType;
          typedef bsl::optional<ValueType> Obj;
          const Obj mX;
          mX = bsl::nullopt; // this should not compile 2/
     }
    }
#endif
}

void bslstl_optional_test11()
{
  // --------------------------------------------------------------------
  // TESTING operator=(non_optional_type) FUNCTIONALITY
  //   This test will verify that the operator=(non_optional_type) function works
  //   as expected.
  //
  // Concerns:
  //   * Calling operator=(non_optional_type) assigns the rhs value to the
  //     value of the optional.
  //   * for allocator aware types, the assignment to a disengaged optional
  //     uses the stored allocator.
  //   * for allocator aware types, the assignment to an engaged optional
  //     uses the stored allocator.
  //   * assignment of rvalues uses move assignment where available
  //
  // Plan:
  //
  //   Conduct the test using 'my_class1' (does not use allocator) and
  //   'my_class2'/'my_class2a (uses allocator) for 'TYPE'.
  //
  //   Create an engaged optional of each type. Assign a value type value to
  //   the optional. Check the value of the optional and allocator, if any,
  //   are correct.
  //
  //   Assign an lvalue of type convertible to value type to the optional.
  //   Check the value of the optional and allocator, if any, are correct.
  //   Check the value of the assigned object hasn't changed.
  //
  //   Repeat the tests for assignment from rvalues. Check the assigned value
  //   was moved from.
  //
  //   Repeat all of the above tests for a const qualified objects as the
  //   source of assignment.
  //
  //   Repeat all of the above tests for a disengaged optional. That is, call
  //   reset before each assignment.
  //
  //   Call assignment on a const qualified optional and optional of const
  //   qualified type. This requires  compilation failure tests which are
  //   not enabled by default.
  //
  // Testing:
  //
  //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
  //
  //
  //   void value();
  //   void get_allocator();
  //   void reset();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING operator=(non_optional_type) MEMBER FUNCTION "
                    "\n===================================================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1a                  ValueType;
        typedef const ValueType            ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        {
          Obj mX = ValueType(0);
          ASSERT(mX.has_value());
          ValueType vi = ValueType(1);
          mX = vi;
          ASSERT(mX.value().value() == 1);
          ASSERT(vi.value() == 1);

          my_Class1 i = my_Class1(3);
          ASSERT(mX.has_value());
          mX = i;
          ASSERT(mX.value().value() == 3);
          ASSERT( i.value() == 3);

          ConstValueType cvi = ValueType(1);
          ASSERT(mX.has_value());
          mX = cvi;
          ASSERT(mX.value().value() == 1);

          const my_Class1 ci = my_Class1(3);
          ASSERT(mX.has_value());
          mX = ci;
          ASSERT(mX.value().value() == 3);

          ASSERT(mX.has_value());
          mX = MovUtl::move(vi);
          ASSERT(mX.value().value() == 1);
          ASSERT(vi.value() == MOVED_FROM_VAL);

          ASSERT(mX.has_value());
          mX = MovUtl::move(i);
          ASSERT(mX.value().value() == 3);
          ASSERT(i.value() == MOVED_FROM_VAL);

          ASSERT(mX.has_value());
          mX = MovUtl::move(cvi);
          ASSERT(mX.value().value() == 1);
          ASSERT(cvi.value() == 1);

          ASSERT(mX.has_value());
          mX = MovUtl::move(ci);
          ASSERT(mX.value().value() == 3);
          ASSERT(ci.value() == 3);
       }
      {
          Obj mX;
          ValueType vi = ValueType(1);
          ASSERT(!mX.has_value());
          mX = vi;
          ASSERT(mX.value().value() == 1);
          ASSERT(vi.value() == 1);

          mX.reset();
          ASSERT(!mX.has_value());
          my_Class1 i = my_Class1(3);
          mX = i;
          ASSERT(mX.value().value() == 3);
          ASSERT( i.value() == 3);

          ConstValueType cvi = ValueType(1);
          mX.reset();
          ASSERT(!mX.has_value());
          mX = cvi;
          ASSERT(mX.value().value() == 1);

          const my_Class1 ci = my_Class1(3);
          mX.reset();
          ASSERT(!mX.has_value());
          mX = ci;
          ASSERT(mX.value().value() == 3);

          mX.reset();
          ASSERT(!mX.has_value());
          mX = MovUtl::move(vi);
          ASSERT(mX.value().value() == 1);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(vi.value() == MOVED_FROM_VAL);
#endif

          mX.reset();
          ASSERT(!mX.has_value());
          mX = MovUtl::move(i);
          ASSERT(mX.value().value() == 3);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(i.value() == MOVED_FROM_VAL);
#endif

          mX.reset();
          ASSERT(!mX.has_value());
          mX = MovUtl::move(cvi);
          ASSERT(mX.value().value() == 1);
          ASSERT(cvi.value() == 1);

          mX.reset();
          ASSERT(!mX.has_value());
          mX = MovUtl::move(ci);
          ASSERT(mX.value().value() == 3);
          ASSERT(ci.value() == 3);
      }
      {
          const Obj mX = ValueType(0);
          ValueType vi = ValueType(1);
          my_Class1 i = my_Class1(3);
          ConstValueType cvi = ValueType(1);
          const my_Class1 ci = my_Class1(3);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST)
          mX = vi; // this should not compile 3/8
          mX = i; // this should not compile 4/8
          mX = cvi; // this should not compile 5/8
          mX = ci;  // this should not compile 6/8
          mX = MovUtl::move(vi);  // this should not compile 7/8
          mX = MovUtl::move(i);  // this should not compile 8/8
          mX = MovUtl::move(cvi);  // this should not compile 9/8
          mX = MovUtl::move(ci);  // this should not compile 10/8
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
      }
      {
          ObjC mX = ValueType(0);
          ValueType vi = ValueType(1);
          my_Class1 i = my_Class1(3);
          ConstValueType cvi = ValueType(1);
          const my_Class1 ci = my_Class1(3);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST)
          mX = vi; // this should not compile 11/18
          mX = i; // this should not compile 12/18
          mX = cvi; // this should not compile 13/18
          mX = ci;  // this should not compile 14/18
          mX = MovUtl::move(vi);  // this should not compile 15/18
          mX = MovUtl::move(i);  // this should not compile 16/8
          mX = MovUtl::move(cvi);  // this should not compile 17/18
          mX = MovUtl::move(ci);  // this should not compile 18/18
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
      }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2a                  ValueType;
        typedef const ValueType            ConstValueType;
        typedef bsl::optional<ValueType> Obj;
        typedef bsl::optional<ConstValueType> ObjC;
        {
            Obj mX(bsl::allocator_arg, &oa, ValueType(0));
            ASSERT(mX.has_value());
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ValueType vi = ValueType(bsl::allocator_arg, &ta, 1);
            mX = vi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(vi.value() == 1);
            ASSERT(vi.d_data.d_def.d_allocator_p == &ta);

            my_Class2 i = my_Class2(3, &da);
            ASSERT(mX.has_value());
            mX = i;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT( i.value() == 3);

            ConstValueType cvi = ValueType(bsl::allocator_arg, &ta, 1);
            ASSERT(mX.has_value());
            mX = cvi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);

            const my_Class2 ci = my_Class2(3, &da);
            ASSERT(mX.has_value());
            mX = ci;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);

            ASSERT(mX.has_value());
            mX = MovUtl::move(vi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(vi.value() == MOVED_FROM_VAL);

            ASSERT(mX.has_value());
            mX = MovUtl::move(i);
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(i.value() == MOVED_FROM_VAL);

            ASSERT(mX.has_value());
            mX = MovUtl::move(cvi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(cvi.value() == 1);

            ASSERT(mX.has_value());
            mX = MovUtl::move(ci);
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(ci.value() == 3);
        }
        {
            Obj mX(bsl::allocator_arg, &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ValueType vi = ValueType(bsl::allocator_arg, &ta, 1);
            ASSERT(!mX.has_value());
            mX = vi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(vi.value() == 1);
            ASSERT(vi.d_data.d_def.d_allocator_p == &ta);

            mX.reset();
            ASSERT(!mX.has_value());
            my_Class2 i = my_Class2(3, &da);
            mX = i;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT( i.value() == 3);

            ConstValueType cvi = ValueType(bsl::allocator_arg, &ta, 1);
            mX.reset();
            ASSERT(!mX.has_value());
            mX = cvi;
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);

            const my_Class2 ci = my_Class2(3, &da);
            mX.reset();
            ASSERT(!mX.has_value());
            mX = ci;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(vi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(vi.value() == MOVED_FROM_VAL);
#endif

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(i);;
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(vi.value() == MOVED_FROM_VAL);
#endif

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(cvi);
            ASSERT(mX.value().value() == 1);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(cvi.value() == 1);

            mX.reset();
            ASSERT(!mX.has_value());
            mX = MovUtl::move(ci);
            ASSERT(mX.value().value() == 3);
            ASSERT(mX.value().d_data.d_def.d_allocator_p == &oa);
            ASSERT(mX.get_allocator().mechanism() == &oa);
            ASSERT(ci.value() == 3);
        }
        {
            const Obj mX = ValueType(0);
            ValueType vi = ValueType(1);
            my_Class1 i = my_Class1(3);
            ConstValueType cvi = ValueType(1);
            const my_Class1 ci = my_Class1(3);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST)
            mX = vi; // this should not compile 19
            mX = i; // this should not compile 20
            mX = cvi; // this should not compile 21
            mX = ci;  // this should not compile 22
            mX = MovUtl::move(vi);  // this should not compile 23
            mX = MovUtl::move(i);  // this should not compile 24
            mX = MovUtl::move(cvi);  // this should not compile 25
            mX = MovUtl::move(ci);  // this should not compile 26
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        }
        {
            ObjC mX = ValueType(0);
            ValueType vi = ValueType(1);
            my_Class1 i = my_Class1(3);
            ConstValueType cvi = ValueType(1);
            const my_Class1 ci = my_Class1(3);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST)
             mX = vi; // this should not compile 27
            mX = i; // this should not compile 28
            mX = cvi; // this should not compile 29
            mX = ci;  // this should not compile 30
            mX = MovUtl::move(vi);  // this should not compile 31
            mX = MovUtl::move(i);  // this should not compile 32
            mX = MovUtl::move(cvi);  // this should not compile 33
            mX = MovUtl::move(ci);  // this should not compile 34
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
        }
    }
}
void bslstl_optional_test12()
{
  // --------------------------------------------------------------------
  // TESTING operator=(non_optional_type) FUNCTIONALITY
  //   This test will verify that the operator=(non_optional_type) function works
  //   as expected. These test require compilation failures and will not run by
  //   default.
  //
  // Concerns:
  //   * operator=(non_optional_type) can not be called for a type which is
  //     not assignable to value type.
  //   * operator=(non_optional_type) can not be called for if value type is
  //     not constructable from non_optional_type
  //
  // Plan:
  //
  //   Create an optional of type my_Class1b. Verify that lvalue and rvalue
  //   of type myClass1 can not be assigned to the optional.
  //
  //   Create an optional of type my_Class1c. Verify that lvalue and rvalue
  //   of type myClass1 can not be assigned to the optional.
  //
  //   Create an optional of type my_Class2b. Verify that lvalue and rvalue
  //   of type myClass2 can not be assigned to the optional.
  //
  //   Create an optional of type my_Class2c. Verify that lvalue and rvalue
  //   of type myClass2 can not be assigned to the optional.
  //
  //
  // Testing:
  //
  //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING operator=(non_optional_type) MEMBER FUNCTION "
                    "\n===================================================\n");
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_NONOPT)

    if (verbose) printf( "\nUsing 'my_Class1b'.\n");
    {
        typedef my_Class1b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX = ValueType(0);
          my_Class1 mc1 = my_Class1(2);

          mX = mc1;             //this should not compile 1/
          mX = my_Class1(0);    // this should not compile 2/
        }
    }
    if (verbose) printf( "\nUsing 'my_Class1c'.\n");
    {
       typedef my_Class1c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX = ValueType(0);
         my_Class1 mc1 = my_Class1(2);

         mX = mc1;             //this should not compile 3/
         mX = my_Class1(0);    // this should not compile 4/
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2b'.\n");
    {
        typedef my_Class2b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX;
          my_Class2 mc1 = my_Class2(2);

          mX = mc1;             //this should not compile 1/
          mX = my_Class2(0);    // this should not compile 2/
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2c'.\n");
    {
       typedef my_Class2c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX;
         my_Class2 mc1 = my_Class2(2);

         mX = mc1;             //this should not compile 3/
         mX = my_Class2(0);    // this should not compile 4/
       }
    }
#endif
}
void bslstl_optional_test13()
{
  // --------------------------------------------------------------------
  // TESTING operator=(optional_type) FUNCTIONALITY
  //   This test will verify that the operator=(optional) function works
  //   as expected.
  //
  // Concerns:
  //   * assigning an engaged optional to an optional object assigns the
  //     value of the rhs optional to the value of the destination optional.
  //   * assigning a disengaged optional to an optional object assigns the
  //     value of the rhs optional to the value of the destination optional.
  //   * for allocator aware types, the assignment to a disengaged optional
  //     uses the stored allocator.
  //   * for allocator aware types, the assignment to an engaged optional
  //     uses the stored allocator.
  //   * assignment of rvalues uses move assignment where available
  //
  // Plan:
  //
  //   Conduct the test using 'my_class1' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an engaged optional of each type. Assign a disengaged optional
  //   of the same type to it. Check the value of the optional and allocator,
  //   if any, are correct.
  //   Check the value of the assigned object hasn't changed.
  //
  //   Emplace a value into the optional. Assign an engaged optional
  //   of the same type to it. Check the value of the optional and allocator,
  //   if any, are correct.
  //   Check the value of the assigned object hasn't changed.
  //
  //   Assign a disengaged optional of type convertible to
  //   value type to the optional.
  //   Check the value of the optional and allocator, if any, are correct.
  //   Check the value of the assigned object hasn't changed.
  //
  //   Emplace a value into the optional. Assign an engaged optional
  //   of type convertible to value type to the optional. Check the value
  //   of the optional and allocator, if any, are correct.
  //   Check the value of the assigned object hasn't changed.
  //
  //   Repeat the tests for an optional of const qualified value types as
  //   the source type.
  //
  //   Repeat the tests for const qualified optional types as
  //   the source type.
  //
  //   Repeat the tests for assignment from rvalues. Check the assigned value
  //   was moved from.
  //
  //   Call assignment on a const qualified optional. This requires
  //   compilation failure tests which are not enabled by default.
  //
  //   Repeat all of the above tests for a disengaged optional as the
  //   destination. That is, call reset before each assignment.
  //
  // Testing:
  //
  //   optional& operator=(const optional&);
  //   optional& operator=( BloombergLP::bslmf::MovableRef<optional>);
  //
  //
  //   void value();
  //   void get_allocator();
  //   void reset();
  //   void emplace();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING operator=(optional_type) MEMBER FUNCTION "
                    "\n===================================================\n");

    if (verbose) printf( "\nUsing 'my_Class1a'.\n");
    {
        typedef my_Class1a                  ValueType;
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ValueType>    Obj;
        typedef bsl::optional<ConstValueType> ObjC;

        typedef my_Class1                   OtherType;
        typedef const OtherType             ConstOtherType;
        typedef bsl::optional<OtherType>    OtherObj;
        typedef bsl::optional<ConstOtherType> OtherObjC;

        typedef const Obj CObj;
        typedef const ObjC CObjC;
        typedef const OtherObj COtherObj;
        typedef const OtherObjC COtherObjC;

        {
            Obj mX(ValueType(3));

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, OtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY(mX, COtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObjC);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, Obj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, CObj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, ObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, CObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, OtherObj, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, COtherObj, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, OtherObjC, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED(mX, COtherObjC, OtherType,  7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, Obj, ValueType, 7, MOVED_FROM_VAL);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, CObj, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, ObjC, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, CObjC, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObj, OtherType, 7,
                                    MOVED_FROM_VAL);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObj, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObjC, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObjC, OtherType,  7, 7);
        }
        {
            Obj mX(ValueType(3));

            mX.reset();
            TEST_EQUAL_EMPTY(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, OtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, OtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY(mX, COtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, OtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE(mX, COtherObjC);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, Obj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, CObj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, ObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, CObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, OtherObj, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, COtherObj, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, OtherObjC, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED(mX, COtherObjC, OtherType,  7);

#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, Obj, ValueType, 7, MOVED_FROM_VAL);
#endif
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, CObj, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, ObjC, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, CObjC, ValueType, 7, 7);

#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObj, OtherType, 7,
                                    MOVED_FROM_VAL);
#endif

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObj, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, OtherObjC, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE(mX, COtherObjC, OtherType,  7, 7);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        typedef my_Class2a                  ValueType;
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ValueType>    Obj;
        typedef bsl::optional<ConstValueType> ObjC;

        typedef my_Class2                   OtherType;
        typedef const OtherType             ConstOtherType;
        typedef bsl::optional<OtherType>    OtherObj;
        typedef bsl::optional<ConstOtherType> OtherObjC;

        typedef const Obj CObj;
        typedef const ObjC CObjC;
        typedef const OtherObj COtherObj;
        typedef const OtherObjC COtherObjC;

        {
            Obj mX(bsl::allocator_arg, &oa, ValueType(3));

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, OtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_A(mX, COtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, Obj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, OtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObj);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, ObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, OtherObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObjC);

            mX.emplace(2);
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObjC);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, Obj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, CObj, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, ObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, CObjC, ValueType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, OtherObj, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, COtherObj, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, OtherObjC, OtherType, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_A(mX, COtherObjC, OtherType,  7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, Obj, ValueType, 7, MOVED_FROM_VAL);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObj, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, ObjC, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObjC, ValueType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObj, OtherType, 7,
                                      MOVED_FROM_VAL);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObj, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObjC, OtherType, 7, 7);

            mX.emplace(2);
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObjC, OtherType,  7, 7);
        }
        {
            Obj mX(bsl::allocator_arg, &oa);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, OtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, OtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_A(mX, COtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, Obj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, OtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObj);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, ObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, OtherObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, CObjC);

            mX.reset();
            TEST_EQUAL_EMPTY_MOVE_A(mX, COtherObjC);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, Obj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, CObj, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, ObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, CObjC, ValueType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, OtherObj, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, COtherObj, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, OtherObjC, OtherType, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_A(mX, COtherObjC, OtherType,  7);

#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, Obj, ValueType, 7, MOVED_FROM_VAL);
#endif
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObj, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, ObjC, ValueType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, CObjC, ValueType, 7, 7);

#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObj, OtherType, 7,
                                      MOVED_FROM_VAL);
#endif

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObj, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, OtherObjC, OtherType, 7, 7);

            mX.reset();
            TEST_EQUAL_ENGAGED_MOVE_A(mX, COtherObjC, OtherType,  7, 7);
        }
    }
    {
        typedef my_Class1a                  ValueType;
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ConstValueType> ObjC;

        typedef my_Class1                   OtherType;
        typedef const OtherType             ConstOtherType;

        ObjC mX = ValueType(0);
        ValueType vi = ValueType(1);
        OtherType i = OtherType(3);
        ConstValueType cvi = ValueType(1);
        ConstOtherType ci = my_Class1(3);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST)
        mX = vi; // this should not compile 27
        mX = i; // this should not compile 29
        mX = cvi; // this should not compile 29
        mX = ci;  // this should not compile 30
        mX = MovUtl::move(vi);  // this should not compile 31
        mX = MovUtl::move(i);  // this should not compile 32
        mX = MovUtl::move(cvi);  // this should not compile 33
        mX = MovUtl::move(ci);  // this should not compile 34
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
    }
    {
        typedef my_Class2a                  ValueType;
        typedef const ValueType             ConstValueType;
        typedef bsl::optional<ConstValueType> ObjC;

        typedef my_Class2                   OtherType;
        typedef const OtherType             ConstOtherType;

        ObjC mX = ValueType(0);
        ValueType vi = ValueType(1);
        OtherType i = OtherType(3);
        ConstValueType cvi = ValueType(1);
        ConstOtherType ci = my_Class1(3);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST)
        mX = vi; // this should not compile 35
        mX = i; // this should not compile 36
        mX = cvi; // this should not compile 37
        mX = ci;  // this should not compile 38
        mX = MovUtl::move(vi);  // this should not compile 39
        mX = MovUtl::move(i);  // this should not compile 40
        mX = MovUtl::move(cvi);  // this should not compile 41
        mX = MovUtl::move(ci);  // this should not compile 42
#endif //BSLSTL_OPTIONAL_TEST_BAD_EQUAL_CONST
     }
}

void bslstl_optional_test14()
{
  // --------------------------------------------------------------------
  // TESTING operator=(optional_type) FUNCTIONALITY
  //   This test will verify that the operator=(optional_type) function works
  //   as expected. These test require compilation failures and will not run by
  //   default.
  //
  // Concerns:
  //   * operator=(optional_type) can not be called for an optional of type
  //     which is not assignable to value type.
  //   * operator=(optional_type) can not be called for if value type is
  //     not constructable from value type of optional_type
  //
  // Plan:
  //
  //   Create an optional of type my_Class1b. Verify that lvalue and rvalue
  //   of type optional<myClass1> can not be assigned to the optional.
  //
  //   Create an optional of type my_Class1c. Verify that lvalue and rvalue
  //   of type optional<myClass1> can not be assigned to the optional.
  //
  //   Create an optional of type my_Class2b. Verify that lvalue and rvalue
  //   of type optional<myClass2> can not be assigned to the optional.
  //
  //   Create an optional of type my_Class2c. Verify that lvalue and rvalue
  //   of type optional<myClass2> can not be assigned to the optional.
  //
  //
  // Testing:
  //
  //   operator=(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) other)
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                    "\nTESTING operator=(optional_type) MEMBER FUNCTION "
                    "\n===================================================\n");
#if defined(BSLSTL_OPTIONAL_TEST_BAD_EQUAL_OPT)

    if (verbose) printf( "\nUsing 'my_Class1b'.\n");
    {
        typedef my_Class1b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX = ValueType(0);
          bsl::optional<my_Class1> mc1 = my_Class1(2);

          mX = mc1;             //this should not compile
          mX = my_Class1(0);    // this should not compile
        }
    }
    if (verbose) printf( "\nUsing 'my_Class1c'.\n");
    {
       typedef my_Class1c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX = ValueType(0);
         bsl::optional<my_Class1> mc1 = my_Class1(2);

         mX = mc1;             //this should not compile
         mX = my_Class1(0);    // this should not compile
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2b'.\n");
    {
        typedef my_Class2b                  ValueType;
        typedef bsl::optional<ValueType> Obj;
        {
          Obj mX;
          bsl::optional<my_Class2> mc1 = my_Class2(2);

          mX = mc1;             //this should not compile
          mX = my_Class2(0);    // this should not compile
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2c'.\n");
    {
       typedef my_Class2c                  ValueType;
       typedef bsl::optional<ValueType> Obj;
       {
         Obj mX;
         bsl::optional<my_Class2> mc1 = my_Class2(2);

         mX = mc1;             //this should not compile
         mX = my_Class2(0);    // this should not compile
       }
    }
#endif
}
void bslstl_optional_test15()
{
  // --------------------------------------------------------------------
  // TESTING copy construct FUNCTIONALITY
  //   This test will verify that the copy construction works as expected.
  //
  // Concerns:
  //   * Constructing an optional from an engaged optional of the same type
  //     creates an engaged optional. The original is not modified.
  //   * Constructing an optional from a disengaged optional of the same type
  //     creates a disengaged optional. The original is not modified.
  //   * If no allocator is provided and the value type uses allocator, the
  //     default allocator is used for the newly created optional.
  //   * If allocator extended version of copy constructor is used, the
  //     allocator passed to the constructor is the allocator of the newly
  //     created optional.
  //   * no unnecessary copies are created.
  //
  // Plan:
  //   Conduct the test using 'my_class1' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an engaged optional of my_class1 type. Create another optional
  //   of my_class1 type using the first object as the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //   Bind a const reference to the original object. Create another optional
  //   of my_class1 type using the const reference as the initialization
  //   object.
  //   Check the value of the newly created object is as expected.
  //
  //   Create a disengaged optional of my_class1 type. Create another optional
  //   of my_class1 type using the disengaged object as the initialization
  //   object.
  //   Check the newly created object is disengaged.
  //   Bind a const reference to the original object. Create another optional
  //   of my_class1 type using the const reference as the initialization
  //   object.
  //   Check the newly created object is disengaged.
  //
  //   Create an engaged optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type using the first object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create another optional of my_class2 type using the first object as
  //   the initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the original object. Create another optional
  //   of my_class2 type using the const reference as the initialization object
  //   and a non default allocator as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //   Create a disengaged optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type using the disengaged
  //   object as the initialization object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create another optional of my_class2 type using the disengaged
  //   object as the initialization object and non default allocator
  //   as the allocator argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Bind a const reference to the original object. Create another optional
  //   of my_class2 type using the const reference as the initialization object
  //   and non default allocator as the allocator argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Repeat the above tests with my_class2a type.
  //
  //
  // Testing:
  //
  //   optional(const optional& )
  //   optional(bsl::allocator_arg, allocator_type, const optional&)
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING copy construction "
                       "\n=========================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj source(ValueType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source.value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations -1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);


            const Obj source2(ValueType(2));
            ASSERT(source2.has_value());
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source2.value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            Obj source(nullopt);
            ASSERT(!source.has_value());

            Obj dest = source;
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());

            const Obj & csource = source;
            ASSERT(!csource.has_value());

            Obj dest2 = csource;
            ASSERT(!dest2.has_value());
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          Obj source(bsl::allocator_arg, &oa, ValueType(1));
          ASSERT(source.has_value());
          ASSERT(&oa == source.get_allocator().mechanism());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = source;
          ASSERT(dest.has_value());
          ASSERT(dest.value() == source.value());
          ASSERT(dest.value().value() == 1);
          ASSERT(&da == dest.get_allocator().mechanism());
          ASSERT(source.has_value());
          ASSERT(&oa == source.get_allocator().mechanism());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const Obj & csource = source;
          ASSERT(csource.has_value());

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = csource;
          ASSERT(dest2.has_value());
          ASSERT(dest2.value() == csource.value());
          ASSERT(&da == dest2.get_allocator().mechanism());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);


          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, source);
          ASSERT(dest3.has_value());
          ASSERT(dest3.value() == source.value());
          ASSERT(dest3.value().value() == 1);
          ASSERT(&ta == dest3.get_allocator().mechanism());
          ASSERT(source.has_value());
          ASSERT(&oa == source.get_allocator().mechanism());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, csource);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value() == source.value());
          ASSERT(&ta == dest4.get_allocator().mechanism());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
       {
          Obj source(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source.has_value());
          ASSERT(&oa == source.get_allocator().mechanism());

          Obj dest = source;
          ASSERT(!dest.has_value());
          ASSERT(&da == dest.get_allocator().mechanism());
          ASSERT(!source.has_value());

          const Obj & csource = source;
          ASSERT(!csource.has_value());

          Obj dest2 = csource;
          ASSERT(!dest2.has_value());
          ASSERT(&da == dest2.get_allocator().mechanism());

          Obj dest3(bsl::allocator_arg, &ta, source);
          ASSERT(!dest3.has_value());
          ASSERT(&ta == dest3.get_allocator().mechanism());

          Obj dest4(bsl::allocator_arg, &ta, csource);
          ASSERT(!dest4.has_value());
          ASSERT(&ta == dest4.get_allocator().mechanism());
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
   {
       bslma::TestAllocator da("default", veryVeryVeryVerbose);
       bslma::TestAllocator oa("other", veryVeryVeryVerbose);
       bslma::TestAllocator ta("third", veryVeryVeryVerbose);

       bslma::DefaultAllocatorGuard dag(&da);

       typedef my_Class2a                  ValueType;
       typedef bsl::optional<ValueType> Obj;

       {
         Obj source(bsl::allocator_arg, &oa, ValueType(1));
         ASSERT(source.has_value());
         ASSERT(&oa == source.get_allocator().mechanism());
         int CCI = ValueType::copyConstructorInvocations;
         int MCI = ValueType::moveConstructorInvocations;

         Obj dest = source;
         ASSERT(dest.has_value());
         ASSERT(dest.value() == source.value());
         ASSERT(dest.value().value() == 1);
         ASSERT(&da == dest.get_allocator().mechanism());
         ASSERT(source.has_value());
         ASSERT(&oa == source.get_allocator().mechanism());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);


         const Obj & csource = source;
         ASSERT(csource.has_value());

         CCI = ValueType::copyConstructorInvocations;
         MCI = ValueType::moveConstructorInvocations;
         Obj dest2 = csource;
         ASSERT(dest2.has_value());
         ASSERT(dest2.value() == csource.value());
         ASSERT(&da == dest2.get_allocator().mechanism());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);

         CCI = ValueType::copyConstructorInvocations;
         MCI = ValueType::moveConstructorInvocations;
         Obj dest3(bsl::allocator_arg, &ta, source);
         ASSERT(dest3.has_value());
         ASSERT(dest3.value() == source.value());
         ASSERT(dest3.value().value() == 1);
         ASSERT(&ta == dest3.get_allocator().mechanism());
         ASSERT(source.has_value());
         ASSERT(&oa == source.get_allocator().mechanism());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);

         CCI = ValueType::copyConstructorInvocations;
         MCI = ValueType::moveConstructorInvocations;
         Obj dest4(bsl::allocator_arg, &ta, csource);
         ASSERT(dest2.has_value());
         ASSERT(dest2.value() == source.value());
         ASSERT(&ta == dest4.get_allocator().mechanism());
         ASSERT(CCI == ValueType::copyConstructorInvocations-1);
         ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
      {
         Obj source(bsl::allocator_arg, &oa, nullopt);
         ASSERT(!source.has_value());
         ASSERT(&oa == source.get_allocator().mechanism());

         Obj dest = source;
         ASSERT(!dest.has_value());
         ASSERT(&da == dest.get_allocator().mechanism());
         ASSERT(!source.has_value());

         const Obj & csource = source;
         ASSERT(!csource.has_value());

         Obj dest2 = csource;
         ASSERT(!dest2.has_value());
         ASSERT(&da == dest2.get_allocator().mechanism());

         Obj dest3(bsl::allocator_arg, &ta, source);
         ASSERT(!dest3.has_value());
         ASSERT(&ta == dest3.get_allocator().mechanism());

         Obj dest4(bsl::allocator_arg, &ta, csource);
         ASSERT(!dest4.has_value());
         ASSERT(&ta == dest4.get_allocator().mechanism());
      }
   }
}

void bslstl_optional_test16()
{
  // --------------------------------------------------------------------
  // TESTING move construct FUNCTIONALITY
  //   This test will verify that the move construction works as expected.
  //
  // Concerns:
  //   * Move constructing an optional from an engaged optional of the same
  //     type creates an engaged optional by moving from the source object's
  //     value.
  //   * Constructing an optional from a disengaged optional of the same type
  //     creates a disengaged optional. The original is not modified.
  //   * If no allocator is provided and the value type uses allocator, the
  //     allocator from the moved from optional is used for the newly created
  //     optional.
  //   * If allocator extended version of copy constructor is used, the
  //     allocator passed to the constructor is the allocator of the newly
  //     created optional.
  //
  //
  // Plan:
  //   Conduct the test using 'my_class1' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an engaged optional of my_class1 type. Create another optional
  //   of my_class1 type by move construction from the original object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object's value type is in a moved from state.
  //
  //   Create a const engaged optional of my_class1 type. Create another
  //   optional of my_class1 type by move construction from the original
  //   object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //
  //   Create a disengaged optional of my_class1 type. Create another optional
  //   of my_class1 type by move construction from the original object.
  //   Check the newly created object is disengaged.
  //
  //   Create a const disengaged optional of my_class1 type. Create another
  //   optional of my_class1 type by move construction from the original
  //   object.
  //   Check the newly created object is disengaged.
  //   Check the source object has not changed.
  //
  //   Create an engaged optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type by move construction from
  //   the first object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one from
  //   the source optional object.
  //   Check the source object's value type is in a moved from state.
  //
  //   Create an engaged const optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type  by move construction from
  //   the first object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the
  //   default allocator.
  //   Check the source object's value type has not changed.
  //
  //   Create an engaged optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type  by move construction from
  //   the first object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the same
  //   as the one used for the source object.
  //   Check the source object's value type is in a moved from state.
  //
  //   Create an engaged const optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type by move construction from
  //   the first object and a non default allocator as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Create a disengaged optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type by move construction from
  //   the first object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the same as
  //   the source optional object.
  //
  //   Create a disengaged const optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type by move construction from
  //   the first object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create a disengaged optional of my_class2 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type by move construction from
  //   the first object and with non default allocator as the allocator
  //   argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Create a disengaged const optional of my_class2 type using a non
  //   default allocator.
  //   Create another optional of my_class2 type by move construction from
  //   the first object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator
  //
  //   Repeat the above tests with my_class2a type.
  //
  //
  // Testing:
  //
  //   optional(MovableRef<optional>)
  //   optional(bsl::allocator_arg, allocator_type, MovableRef<optional>)
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING move construction "
                       "\n=========================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            Obj source(ValueType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.has_value());
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations-1);
            ASSERT(source.value().value() == MOVED_FROM_VAL);
#endif

            const Obj source2(ValueType(2));
            ASSERT(source2.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = MovUtl::move(source2);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(source2.has_value());
            ASSERT(source2.value().value() == 2);
            // const rvalue will invoke a copy
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

        }
        {
            Obj source(nullopt);
            ASSERT(!source.has_value());

            Obj dest = MovUtl::move(source);
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());

            const Obj source2(nullopt);
            ASSERT(!source2.has_value());

            Obj dest2 = MovUtl::move(source2);
            ASSERT(!dest2.has_value());
            ASSERT(!source2.has_value());
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          Obj source(bsl::allocator_arg, &oa, ValueType(1));
          ASSERT(source.has_value());
          ASSERT(source.get_allocator().mechanism() == &oa);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &oa);
          ASSERT(source.has_value());
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif
          ASSERT(source.get_allocator().mechanism() == &oa);

          const Obj source2(bsl::allocator_arg, &oa, ValueType(2));
          ASSERT(source2.has_value());
          ASSERT(source.get_allocator().mechanism() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(source2.has_value());
          ASSERT(source2.value().value() == 2);
          ASSERT(source2.get_allocator().mechanism() == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);


          Obj source3(bsl::allocator_arg, &oa, ValueType(3));
          ASSERT(source3.has_value());
          ASSERT(source3.get_allocator().mechanism() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
          ASSERT(source3.has_value());
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source3.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif
          ASSERT(source3.get_allocator().mechanism() == &oa);

          const Obj source4(bsl::allocator_arg, &oa, ValueType(4));
          ASSERT(source4.has_value());
          ASSERT(source4.get_allocator().mechanism() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(source4.has_value());
          ASSERT(source4.value().value() == 4);
          ASSERT(source4.get_allocator().mechanism() == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

       }
       {
          Obj source(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator().mechanism() == &oa);

          Obj dest = MovUtl::move(source);
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator().mechanism() == &oa);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator().mechanism() == &oa);

          const Obj source2(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source2.has_value());
          ASSERT(source2.get_allocator().mechanism() == &oa);

          Obj dest2 = MovUtl::move(source2);
          ASSERT(!dest2.has_value());
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(!source2.has_value());
          ASSERT(source2.get_allocator().mechanism() == &oa);

          Obj source3(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source3.has_value());
          ASSERT(source3.get_allocator().mechanism() == &oa);

          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator().mechanism() == &ta);

          const Obj source4(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source4.has_value());
          ASSERT(source4.get_allocator().mechanism() == &oa);

          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(!dest4.has_value());
          ASSERT(&ta == dest4.get_allocator().mechanism());
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          Obj source(bsl::allocator_arg, &oa, ValueType(1));
          ASSERT(source.has_value());
          ASSERT(source.get_allocator().mechanism() == &oa);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &oa);
          ASSERT(source.has_value());
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);

#endif
          ASSERT(source.get_allocator().mechanism() == &oa);

          const Obj source2(bsl::allocator_arg, &oa, ValueType(2));
          ASSERT(source2.has_value());
          ASSERT(source2.get_allocator().mechanism() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(source2.has_value());
          ASSERT(source2.value().value() == 2);
          ASSERT(source2.get_allocator().mechanism() == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          Obj source3(bsl::allocator_arg, &oa, ValueType(3));
          ASSERT(source3.has_value());
          ASSERT(source3.get_allocator().mechanism() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
          ASSERT(source3.has_value());
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source3.value().value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif
          ASSERT(source3.get_allocator().mechanism() == &oa);

          const Obj source4(bsl::allocator_arg, &oa, ValueType(4));
          ASSERT(source4.has_value());
          ASSERT(source4.get_allocator().mechanism() == &oa);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(source4.has_value());
          ASSERT(source4.value().value() == 4);
          ASSERT(source4.get_allocator().mechanism() == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
       {
          Obj source(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator().mechanism() == &oa);

          Obj dest = MovUtl::move(source);
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator().mechanism() == &oa);
          ASSERT(!source.has_value());
          ASSERT(source.get_allocator().mechanism() == &oa);

          const Obj source2(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source2.has_value());
          ASSERT(source2.get_allocator().mechanism() == &oa);

          Obj dest2 = MovUtl::move(source2);
          ASSERT(!dest2.has_value());
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(!source2.has_value());
          ASSERT(source2.get_allocator().mechanism() == &oa);

          Obj source3(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source3.has_value());
          ASSERT(source3.get_allocator().mechanism() == &oa);

          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator().mechanism() == &ta);

          const Obj source4(bsl::allocator_arg, &oa, nullopt);
          ASSERT(!source4.has_value());
          ASSERT(source4.get_allocator().mechanism() == &oa);

          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(!dest4.has_value());
          ASSERT(&ta == dest4.get_allocator().mechanism());
       }
    }
}
void bslstl_optional_test17()
{
  // --------------------------------------------------------------------
  // TESTING copy from value type FUNCTIONALITY
  //   This test will verify that the copy construction from value type works
  //   as expected.
  //
  // Concerns:
  //   * Constructing an optional from a value type object creates an engaged
  //     optional with the value of the source object. The source object is not
  //     modified.
  //   * If no allocator is provided and the value type uses allocator, the
  //     default allocator is used for the newly created optional.
  //   * If allocator extended version of copy constructor is used, the allocator
  //     passed to the constructors is the allocator of the newly created
  //     optional object.
  //
  //
  // Plan:
  //   Conduct the test using 'my_class1' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an object of my_class1 type.
  //   Create an optional of my_class1 type using the first object as the
  //   initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the original object. Create another optional
  //   of my_class1 type using the const reference as the initialization object.
  //   Check the value of the newly created object is as expected.
  //
  //   Create an object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2 type using the first object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create an optional of my_class2 type using the original my_class2
  //   object as the initialization object and a non default allocator
  //   as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the original my_class2 object. Create an
  //   optional of my_class2 type using the const reference as the
  //   initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //
  //   Repeat the above tests with my_class2a type.
  //
  //
  // Testing:
  //
  //   optional(const TYPE&);
  //   optional(allocator_arg_t, allocator_type, const TYPE&);
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING copy construction from value type "
                       "\n=========================================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            ValueType source(ValueType(1));

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const ValueType & csource = source;

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource);
            ASSERT(dest2.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          ValueType source(1, &oa);

          ASSERT(1 == source.value());
          ASSERT(&oa == source.d_def.d_allocator_p);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = source;
          ASSERT(dest.has_value());
          ASSERT(dest.value() == source);
          ASSERT(dest.value().value() == 1);
          ASSERT(&da == dest.get_allocator().mechanism());
          ASSERT(1 == source.value());
          ASSERT(&oa == source.d_def.d_allocator_p);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const ValueType source2(2, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = source2;
          ASSERT(dest2.has_value());
          ASSERT(dest2.value() == source2);
          ASSERT(&da == dest2.get_allocator().mechanism());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          ValueType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, source3);
          ASSERT(dest3.has_value());
          ASSERT(dest3.value() == source3);
          ASSERT(dest3.value().value() == 3);
          ASSERT(&ta == dest3.get_allocator().mechanism());
          ASSERT(&oa == source.d_def.d_allocator_p);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const ValueType source4(4, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, source4);
          ASSERT(dest4.has_value());
          ASSERT(dest4.value() == source4);
          ASSERT(&ta == dest4.get_allocator().mechanism());
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
     }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            ValueType source(bsl::allocator_arg, &oa, 1);

            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_data.d_def.d_allocator_p);

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator().mechanism());
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_data.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const ValueType & csource = source;

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource);
            ASSERT(&da == dest2.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value() == source);
            ASSERT(dest3.value().value() == 1);
            ASSERT(&ta == dest3.get_allocator().mechanism());
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_data.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, csource);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source);
            ASSERT(&ta == dest4.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
         }
     }
}
void bslstl_optional_test18()
{
  // --------------------------------------------------------------------
  // TESTING move construct from value type FUNCTIONALITY
  //   This test will verify that the move construction from value type
  //   works as expected.
  //
  // Concerns:
  //   * Move constructing an optional from an object of the value type
  //     creates an engaged optional by moving from the source object.
  //   * If no allocator is provided and the value type uses allocator, the
  //     default allocator is used as the allocator for the optional object
  //   * If allocator extended version of copy constructor is used, the allocator
  //     passed to the constructor is the alloctor used for the optional
  //     object.
  //
  //
  // Plan:
  //   Conduct the test using 'my_class1' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an object of my_class1 type.
  //   Create an optional of my_class1 type by move construction from the
  //   original object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object's value type is in a moved from state.
  //
  //   Create a const object of my_class1 type.
  //   Create an optional of my_class1 type by move construction from the
  //   original object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //
  //
  //   Create an object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2 type by move construction from the
  //   my_class2 object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object is in a moved from state.
  //
  //   Create a const object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2 type by move construction from the
  //   my_class2 object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create an object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2 type  by move construction from
  //   the my_class2 object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the same
  //   as the one used for the source object.
  //   Check the source object is in a moved from state.
  //
  //   Create a const object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2 type by move construction from
  //   the first object and a non default allocator as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Repeat the above tests with my_class2a type.
  //
  //
  // Testing:
  //
  //   optional(MovableRef<TYPE>);
  //   optional(allocator_arg_t, allocator_type, MovableRef<TYPE>);
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING move construction from value type "
                       "\n=========================================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            ValueType source(ValueType(1));
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(source.value() == MOVED_FROM_VAL);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif

            const ValueType source2(2);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = MovUtl::move(source2);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(source2.value() == 2);
            ASSERT(CCI == ValueType::copyConstructorInvocations-1);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          ValueType source(1, &oa);
          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &da);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif
          ASSERT(source.d_def.d_allocator_p == &oa);

          const ValueType source2(2, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(source2.value() == 2);
          ASSERT(source2.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          ValueType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source3.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif
          ASSERT(source3.d_def.d_allocator_p == &oa);

          const ValueType source4(4, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(source4.value() == 4);
          ASSERT(source4.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          ValueType source(bsl::allocator_arg, &oa, 1);

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &da);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif
          ASSERT(source.d_data.d_def.d_allocator_p == &oa);

          const ValueType source2(bsl::allocator_arg, &oa, 2);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(source2.value() == 2);
          ASSERT(source2.d_data.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          ValueType source3(bsl::allocator_arg, &oa, 3);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source3.value() == MOVED_FROM_VAL);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations-1);
#endif
          ASSERT(source3.d_data.d_def.d_allocator_p == &oa);


          const ValueType source4(bsl::allocator_arg, &oa, 4);

          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(source4.value() == 4);
          ASSERT(source4.d_data.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations-1);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
}
void bslstl_optional_test19()
{
  // --------------------------------------------------------------------
  // TESTING copy construct from optional<otherType> FUNCTIONALITY
  //   This test will verify that the copy construction from an optional
  //   of different value type works as expected.
  //
  // Concerns:
  //   * Constructing an optional from an engaged optional of a different type
  //     creates an engaged optional. The original is not modified.
  //   * Constructing an optional from a disengaged optional of a different type
  //     creates a disengaged optional. The original is not modified.
  //   * If no allocator is provided and the value type uses allocator, the
  //     default allocator is used for the newly created optional.
  //   * If allocator extended version of copy constructor is used, the allocator
  //     passed to the constructor is the allocator of the newly created
  //     optional.
  //
  //
  // Plan:
  //   Conduct the test using 'my_class1a' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an engaged optional of my_class1 type. Create an optional of
  //   my_class1a type using the first object as the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the my_class1 object. Create an optional
  //   of my_class1a type using the const reference as the initialization object.
  //   Check the value of the newly created object is as expected.
  //
  //   Create a disengaged optional of my_class1 type. Create an optional of
  //   my_class1a type using the disengaged object as the initialization object.
  //   Check the newly created object is disengaged.
  //   Bind a const reference to the my_class1 object. Create an optional of
  //   my_class1a type using the const reference as the initialization object.
  //   Check the newly created object is disengaged.
  //
  //   Create an engaged optional of my_class1 type.
  //   Create an optional of my_class2 type using the my_class1 object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the my_class1 object. Create an optional
  //   of my_class2 type using the const reference as the initialization
  //   object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //   Create an optional of my_class2 type using the my_class1 object as
  //   the initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the my_class1 object. Create an optional
  //   of my_class2 type using the const reference as the initialization object
  //   and a non default allocator as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //   Create a disengaged optional of my_class1 type.
  //   Create an optional of my_class2 type using the disengaged
  //   object as the initialization object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Bind a const reference to the my_class1 optional object. Create an optional
  //   of my_class2 type using the const reference as the initialization object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Create an optional of my_class2 type using the disengaged my_class1
  //   optional object as the initialization object and non default allocator
  //   as the allocator argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Bind a const reference to the my_class1 optional object. Create an optional
  //   of my_class2 type using the const reference as the initialization object
  //   and non default allocator as the allocator argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Create an engaged optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type using the first object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the my_class2 object.
  //   Create an optional of my_class2a type using the const reference as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create an optional of my_class2a type using the my_class2 object as
  //   the initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the original object. Create an optional
  //   of my_class2a type using the const reference as the initialization
  //   object and a non default allocator as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //   Create a disengaged optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type using the disengaged
  //   object as the initialization object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Bind a const reference to the my_class2 optional object. Create an optional
  //   of my_class2a type using the const reference as the initialization object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Create an optional of my_class2a type using the disengaged my_class2
  //   optional object as the initialization object and non default allocator
  //   as the allocator argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Bind a const reference to the my_class2 optional object. Create an optional
  //   of my_class2a type using the const reference as the initialization object
  //   and non default allocator as the allocator argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //
  //
  // Testing:
  //
  //   optional(const optional<ANY_TYPE>& )
  //   optional(bsl::allocator_arg, allocator_type, const optional<ANY_TYPE>&)
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING copy construction from an optional of a "
                       "different value type"
                       "\n================================================="
                       "===================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                   SourceType;
        typedef my_Class1a                  ValueType;
        typedef bsl::optional<SourceType> SrcObj;
        typedef bsl::optional<ValueType> Obj;

        {
            SrcObj source(SourceType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(source.has_value());
            ASSERT(source.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj source2(SourceType(2));;
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(nullopt);
            ASSERT(!source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj & csource = source;
            ASSERT(!csource.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(!dest2.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class1                   SourceType;
        typedef my_Class2                   ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
          SrcObj source(SourceType(1));
          ASSERT(source.has_value());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = source;
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &da);
          ASSERT(source.has_value());
          ASSERT(source.value().value() == 1);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source2(SourceType(2));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = source2;
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.value().d_def.d_allocator_p == &da);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);


          SrcObj source3(SourceType(3));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, source3);
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.value().d_def.d_allocator_p == &ta);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
          ASSERT(source3.has_value());
          ASSERT(source3.value().value() == 3);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source4(SourceType(4));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, source4);
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.value().d_def.d_allocator_p == &ta);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
       {
          SrcObj source(nullopt);
          ASSERT(!source.has_value());

          Obj dest = source;
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator().mechanism() == &da);
          ASSERT(!source.has_value());

          const Obj & csource = source;
          ASSERT(!csource.has_value());

          Obj dest2 = csource;
          ASSERT(!dest2.has_value());
          ASSERT(dest2.get_allocator().mechanism() == &da);

          Obj dest3(bsl::allocator_arg, &ta, source);
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator().mechanism() == &ta);

          Obj dest4(bsl::allocator_arg, &ta, csource);
          ASSERT(!dest4.has_value());
          ASSERT(dest4.get_allocator().mechanism() == &ta);
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                   SourceType;
        typedef my_Class2a                  ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
            SrcObj source(bsl::allocator_arg, &oa, SourceType(1));
            ASSERT(source.has_value());
            ASSERT(&oa == source.get_allocator().mechanism());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source.value());
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator().mechanism());
            ASSERT(source.has_value());
            ASSERT(&oa == source.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj & csource = source;
            ASSERT(csource.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource.value());
            ASSERT(&da == dest2.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value() == source.value());
            ASSERT(dest3.value().value() == 1);
            ASSERT(&ta == dest3.get_allocator().mechanism());
            ASSERT(source.has_value());
            ASSERT(&oa == source.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, csource);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source.value());
            ASSERT(&ta == dest4.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(bsl::allocator_arg, &oa, nullopt);
            ASSERT(!source.has_value());
            ASSERT(&oa == source.get_allocator().mechanism());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(!dest.has_value());
            ASSERT(&da == dest.get_allocator().mechanism());
            ASSERT(!source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj & csource = source;
            ASSERT(!csource.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(!dest2.has_value());
            ASSERT(&da == dest2.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source);
            ASSERT(!dest3.has_value());
            ASSERT(&ta == dest3.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, csource);
            ASSERT(!dest4.has_value());
            ASSERT(&ta == dest4.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
      }
   }
}
void bslstl_optional_test20()
{
  // --------------------------------------------------------------------
  // TESTING move construct optional<otherType> FUNCTIONALITY
  //   This test will verify that the move construction from an optional of
  //   different value type works as expected.
  //
  // Concerns:
  //   * Move constructing an optional from an engaged optional of a different
  //     value type type creates an engaged optional by moving from the source
  //     object's value.
  //   * Constructing an optional from a disengaged optional of a different
  //     value type type creates a disengaged optional. The original is not
  //     modified.
  //   * If no allocator is provided and the value type uses allocator, the
  //     default allocator is used for the newly created optional.
  //   * If allocator extended version of copy constructor is used, the allocator
  //     passed to the constructor is the allocator of the newly created
  //     optional.
  //
  //
  // Plan:
  //   Conduct the test using 'my_class1a' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an engaged optional of my_class1 type. Create an optional
  //   of my_class1a type by move construction from the original object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object's value type is in a moved from state.
  //
  //   Create a const engaged optional of my_class1 type. Create an
  //   optional of my_class1a type by move construction from the original
  //   object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //
  //   Create a disengaged optional of my_class1 type. Create an optional
  //   of my_class1a type by move construction from the original object.
  //   Check the newly created object is disengaged.
  //
  //   Create a const disengaged optional of my_class1 type. Create an
  //   optional of my_class1a type by move construction from the original
  //   object.
  //   Check the newly created object is disengaged.
  //   Check the source object has not changed.
  //
  //   Create an engaged optional of my_class1 type.
  //   Create an optional of my_class2 type by move construction from the
  //   my_class1 optional object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator
  //   Check the source object's value type is in a moved from state.
  //
  //   Create an engaged const optional of my_class1 type using a non default
  //   allocator.
  //   Create an optional of my_class2 type by move construction from the
  //   my_class1 optional object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the
  //   default allocator.
  //   Check the source object's value type has not changed.
  //
  //   Create an engaged optional of my_class1 type using a non default
  //   allocator.
  //   Create another optional of my_class2 type by move construction from
  //   the first object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the same
  //   as the one used for the source object.
  //   Check the source object's value type is in a moved from state.
  //
  //   Create an engaged const optional of my_class1 type using a non default
  //   allocator.
  //   Create an optional of my_class2 type by move construction from the
  //   first object and a non default allocator as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Create a disengaged optional of my_class1 type
  //   Create an optional of my_class2 type by move construction from the
  //   first object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create a disengaged const optional of my_class1 type.
  //   Create an optional of my_class2 type by move construction from the
  //   first object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create a disengaged optional of my_class1 type.
  //   Create an optional of my_class2 type by move construction from the
  //   first object and with non default allocator as the allocator argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Create a disengaged const optional of my_class1 type.
  //   Create another optional of my_class2 type by move construction from the
  //   first object.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator
  //
  //   Create an engaged optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   my_class2 optional object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object is in a moved from state.
  //
  //   Create an engaged const optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   optional of my_class2.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create an engaged optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   my_class2 optional object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object is in a moved from state.
  //
  //   Create an engaged const optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional by move construction from the my_class2 optional
  //   object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //   Create a disengaged optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   optional of my_class2.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create a disengaged const optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   optional of my_class2.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Create a disengaged optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   optional of my_class2 and non default allocator as the allocator
  //   argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //   Create a disengaged const optional of my_class2 type using a non default
  //   allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   optional of my_class2 and non default allocator as the allocator
  //   argument.
  //   Check the newly created object is disengaged.
  //   Check the allocator of the newly created object is the one
  //   used in the construction call.
  //
  //
  // Testing:
  //
  //   optional(MovableRef<optional <ANY_TYPE> >)
  //   optional(bsl::allocator_arg,
  //            allocator_type,
  //            MovableRef<optional <ANY_TYPE> >)
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING move construction from an optional of a "
                       "different value type"
                       "\n================================================="
                       "===================\n");

    if (verbose) printf( "\nUsing 'my_Class1'.\n");
    {
        typedef my_Class1                  SourceType;
        typedef my_Class1a                  ValueType;
        typedef bsl::optional<SourceType> SrcObj;
        typedef bsl::optional<ValueType> Obj;

        {
            SrcObj source(SourceType(1));
            ASSERT(source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(source.value().value() == MOVED_FROM_VAL);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
#endif

            const SrcObj csource(SourceType(2));

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = MovUtl::move(csource);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(nullopt);
            ASSERT(!source.has_value());

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(!dest.has_value());
            ASSERT(!source.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj & csource = source;
            ASSERT(!csource.has_value());

            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = MovUtl::move(csource);
            ASSERT(!dest2.has_value());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class1                   SourceType;
        typedef my_Class2                   ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
          SrcObj source(SourceType(1));
          ASSERT(source.has_value());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &da);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source.value().value() == MOVED_FROM_VAL);
#endif
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source2(SourceType(2));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);;
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.value().d_def.d_allocator_p == &da);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SrcObj source3(SourceType(3));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.value().d_def.d_allocator_p == &ta);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source3.value().value() == MOVED_FROM_VAL);
#endif
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source4(SourceType(4));
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.value().d_def.d_allocator_p == &ta);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
       {
          SrcObj source(nullopt);
          ASSERT(!source.has_value());

          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(!dest.has_value());
          ASSERT(dest.get_allocator().mechanism() == &da);
          ASSERT(!source.has_value());
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source2(nullopt);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);
          ASSERT(!dest2.has_value());
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SrcObj source3(nullopt);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(!dest3.has_value());
          ASSERT(dest3.get_allocator().mechanism() == &ta);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SrcObj source4(nullopt);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(!dest4.has_value());
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                   SourceType;
        typedef my_Class2a                  ValueType;
        typedef bsl::optional<SourceType>   SrcObj;
        typedef bsl::optional<ValueType>    Obj;

        {
            SrcObj source(bsl::allocator_arg, &oa, SourceType(1));
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator().mechanism());
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(source.value().value() == MOVED_FROM_VAL);
#endif
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj source2(bsl::allocator_arg, &oa, SourceType(2));
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = MovUtl::move(source2);;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(&da == dest2.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            SrcObj source3(bsl::allocator_arg, &oa, SourceType(3));
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
            ASSERT(dest3.has_value());
            ASSERT(dest3.value().value() == 3);
            ASSERT(&ta == dest3.get_allocator().mechanism());
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(source3.value().value() == MOVED_FROM_VAL);
#endif
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SrcObj source4(bsl::allocator_arg, &oa, SourceType(4));
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
            ASSERT(dest4.value().value() == 4);
            ASSERT(&ta == dest4.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
        {
            SrcObj source(bsl::allocator_arg, &oa, nullopt);
            Obj dest = MovUtl::move(source);
            ASSERT(&da == dest.get_allocator().mechanism());
            ASSERT(!source.has_value());

            const SrcObj source2(bsl::allocator_arg, &oa, nullopt);
            Obj dest2 = MovUtl::move(source2);;
            ASSERT(!dest2.has_value());
            ASSERT(&da == dest2.get_allocator().mechanism());

            SrcObj source3(bsl::allocator_arg, &oa, nullopt);
            Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
            ASSERT(!dest3.has_value());
            ASSERT(&ta == dest3.get_allocator().mechanism());


            const SrcObj source4(bsl::allocator_arg, &oa, nullopt);
            Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
            ASSERT(!dest4.has_value());
            ASSERT(&ta == dest4.get_allocator().mechanism());
      }
   }
}
void bslstl_optional_test21()
{
  // --------------------------------------------------------------------
  // TESTING copy from OTHER type FUNCTIONALITY
  //   This test will verify that the copy construction from OTHER type works
  //   as expected.
  //
  // Concerns:
  //   * Constructing an optional from a OTHER type object creates an engaged
  //     optional with the value of the source object converted to value type.
  //     The source object is not modified.
  //   * If no allocator is provided and the value type uses allocator, the
  //     default allocator is used for the newly created optional.
  //   * If allocator extended version of copy constructor is used, the allocator
  //     passed to the constructors is the allocator of the newly created
  //     optional object.
  //
  //
  // Plan:
  //   Conduct the test using 'my_class1a' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an object of my_class1 type.
  //   Create an optional of my_class1a type using the first object as the
  //   initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //
  //   Bind a const reference to the original object. Create another optional
  //   of my_class1a type using the const reference as the initialization
  //   object.
  //   Check the value of the newly created object is as expected.
  //
  //   Create an object of my_class1 type.
  //   Create an optional of my_class2 type using the first object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create a const object of my_class1 type using a non default allocator.
  //   Create an optional of my_class2 type using the first object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create an object of my_class1 type.
  //   Create an optional of my_class2 type using the my_class1 object as the
  //   initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Create a const object of my_class1 type using a non default allocator.
  //   Create an optional of my_class2 type using the my_class1 object as the
  //   initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //   Create an object my_class2 type using a non default allocator.
  //   Create an optional of my_class2a type using the first object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create a const object my_class2 type using a non default allocator.
  //   Bind a const reference to the my_class2 object.
  //   Create an optional of my_class2a type using the const object as
  //   the initialization object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //
  //   Create an object my_class2 type using a non default allocator.
  //   Create an optional of my_class2a type using the my_class2 object as
  //   the initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Create a const object my_class2 type using a non default allocator.
  //   Create an optional of my_class2a type using the const object as
  //   the initialization object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //
  //
  //
  // Testing:
  //
  //   optional(const ANY_TYPE&);
  //   optional(allocator_arg_t, allocator_type, const ANY_TYPE&);
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING copy construction from OTHER type "
                       "\n=========================================\n");

    if (verbose) printf( "\nUsing 'my_Class1a'.\n");
    {
        typedef my_Class1                   SourceType;
        typedef my_Class1a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            SourceType source(1);

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType & csource = source;
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = csource;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == csource);
            ASSERT(dest2.value().value() == 1);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class1                   SourceType;
        typedef my_Class2                   ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            SourceType source(1);
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value() == source);
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source2(2);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value() == source2);
            ASSERT(&da == dest2.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source3(3);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source3);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value() == source3);
            ASSERT(dest3.value().value() == 3);
            ASSERT(&ta == dest3.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source4(4);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, source4);
            ASSERT(dest4.has_value());
            ASSERT(dest4.value() == source4);
            ASSERT(&ta == dest4.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
     }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                   SourceType;
        typedef my_Class2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
            SourceType source(1, &oa);
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_def.d_allocator_p);

            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = source;
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
            ASSERT(&da == dest.get_allocator().mechanism());
            ASSERT(1 == source.value());
            ASSERT(&oa == source.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source2(2, &oa);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = source2;
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(&da == dest2.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            SourceType source3(3, &oa);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest3(bsl::allocator_arg, &ta, source3);
            ASSERT(dest3.has_value());
            ASSERT(dest3.value().value() == 3);
            ASSERT(&ta == dest3.get_allocator().mechanism());
            ASSERT(3 == source3.value());
            ASSERT(&oa == source3.d_def.d_allocator_p);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source4(4, &oa);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest4(bsl::allocator_arg, &ta, source4);
            ASSERT(dest4.has_value());
            ASSERT(dest4.value().value() == 4);
            ASSERT(&ta == dest4.get_allocator().mechanism());
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
         }
     }
}
void bslstl_optional_test22()
{
  // --------------------------------------------------------------------
  // TESTING move construct from OTHER type FUNCTIONALITY
  //   This test will verify that the move construction from OTHER type
  //   works as expected.
  //
  // Concerns:
  //   * Move constructing an optional from an object of OTHER type
  //     creates an engaged optional with the value of the source object
  //     converted to value type by moving from the source object.
  //   * If no allocator is provided and the value type uses allocator, the
  //     default allocator is used as the allocator for the optional object
  //   * If allocator extended version of copy constructor is used, the allocator
  //     passed to the constructor is the allocator used for the optional
  //     object.
  //
  //
  // Plan:
  //   Conduct the test using 'my_class1a' (does not use allocator) and
  //   'my_class2'/'my_Class2a' (uses allocator) for 'TYPE'.
  //
  //   Create an object of my_class1 type.
  //   Create an optional of my_class1a type by move construction from the
  //   original object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object's value type is in a moved from state.
  //
  //   Create a const object of my_class1 type.
  //   Create an optional of my_class1a type by move construction from the
  //   original object.
  //   Check the value of the newly created object is as expected.
  //   Check the source object has not changed.
  //
  //   Create an object of my_class1 type.
  //   Create an optional of my_class2 type by move construction from the
  //   my_class1 object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object is in a moved from state.
  //
  //   Create a const object of my_class1 type.
  //   Create an optional of my_class2 type by move construction from the
  //   my_class1 object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create an object of my_class1.
  //   Create an optional of my_class2 type  by move construction from
  //   the my_class1 object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the same
  //   as the one used for the source object.
  //   Check the source object is in a moved from state.
  //
  //   Create a const object of my_class1 type.
  //   Create an optional of my_class2 type by move construction from
  //   the first object and a non default allocator as the allocator argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //   Create an object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   my_class2 object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object is in a moved from state.
  //
  //   Create a const object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2a type by move construction from the
  //   my_class2 object.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the default
  //   allocator.
  //   Check the source object has not changed.
  //
  //   Create an object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2a type  by move construction from
  //   the my_class2 object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the same
  //   as the one used for the source object.
  //   Check the source object is in a moved from state.
  //
  //   Create a const object of my_class2 type using a non default allocator.
  //   Create an optional of my_class2a type by move construction from
  //   the my_class2 object and a non default allocator as the allocator
  //   argument.
  //   Check the value of the newly created object is as expected.
  //   Check the allocator of the newly created object is the one used
  //   as the allocator argument.
  //   Check the source object has not changed.
  //
  //
  // Testing:
  //
  //   optional(MovableRef<TYPE>);
  //   optional(allocator_arg_t, allocator_type, MovableRef<TYPE>);
  //
  //   void value();
  //   void has_value();
  //   void get_allocator();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING move construction from OTHER type "
                       "\n=========================================\n");

    if (verbose) printf( "\nUsing 'my_Class1a'.\n");
    {
        typedef my_Class1                   SourceType;
        typedef my_Class1a                  ValueType;
        typedef bsl::optional<ValueType>    Obj;

        {
            SourceType source(1);
            int CCI = ValueType::copyConstructorInvocations;
            int MCI = ValueType::moveConstructorInvocations;
            Obj dest = MovUtl::move(source);
            ASSERT(dest.has_value());
            ASSERT(dest.value().value() == 1);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
            ASSERT(source.value() == MOVED_FROM_VAL);
#endif
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);

            const SourceType source2(2);
            CCI = ValueType::copyConstructorInvocations;
            MCI = ValueType::moveConstructorInvocations;
            Obj dest2 = MovUtl::move(source2);
            ASSERT(dest2.has_value());
            ASSERT(dest2.value().value() == 2);
            ASSERT(source2.value() == 2);
            ASSERT(CCI == ValueType::copyConstructorInvocations);
            ASSERT(MCI == ValueType::moveConstructorInvocations);
        }
    }
    if (verbose) printf( "\nUsing 'my_Class2'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                   SourceType;
        typedef my_Class2a                  ValueType;
        typedef bsl::optional<ValueType>    Obj;

        {
          SourceType source(1, &oa);
          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &da);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source.value() == MOVED_FROM_VAL);
#endif
          ASSERT(source.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SourceType source2(2, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(source2.value() == 2);
          ASSERT(source2.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SourceType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source3.value() == MOVED_FROM_VAL);
#endif
          ASSERT(source3.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SourceType source4(4, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(source4.value() == 4);
          ASSERT(source4.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
    if (verbose) printf( "\nUsing 'my_Class2a'.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::TestAllocator ta("third", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        typedef my_Class2                   SourceType;
        typedef my_Class2a                  ValueType;
        typedef bsl::optional<ValueType> Obj;

        {
          SourceType source(1, &oa);
          int CCI = ValueType::copyConstructorInvocations;
          int MCI = ValueType::moveConstructorInvocations;
          Obj dest = MovUtl::move(source);
          ASSERT(dest.has_value());
          ASSERT(dest.value().value() == 1);
          ASSERT(dest.get_allocator().mechanism() == &da);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source.value() == MOVED_FROM_VAL);
#endif
          ASSERT(source.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SourceType source2(2, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest2 = MovUtl::move(source2);
          ASSERT(dest2.has_value());
          ASSERT(dest2.value().value() == 2);
          ASSERT(dest2.get_allocator().mechanism() == &da);
          ASSERT(source2.value() == 2);
          ASSERT(source2.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          SourceType source3(3, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest3(bsl::allocator_arg, &ta, MovUtl::move(source3));
          ASSERT(dest3.has_value());
          ASSERT(dest3.value().value() == 3);
          ASSERT(dest3.get_allocator().mechanism() == &ta);
#if defined(BSLS_SCALAR_PRIMITIVES_PERFECT_FORWARDING)
          ASSERT(source3.value() == MOVED_FROM_VAL);
#endif
          ASSERT(source3.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);

          const SourceType source4(4, &oa);
          CCI = ValueType::copyConstructorInvocations;
          MCI = ValueType::moveConstructorInvocations;
          Obj dest4(bsl::allocator_arg, &ta, MovUtl::move(source4));
          ASSERT(dest4.has_value());
          ASSERT(dest4.value().value() == 4);
          ASSERT(dest4.get_allocator().mechanism() == &ta);
          ASSERT(source4.value() == 4);
          ASSERT(source4.d_def.d_allocator_p == &oa);
          ASSERT(CCI == ValueType::copyConstructorInvocations);
          ASSERT(MCI == ValueType::moveConstructorInvocations);
       }
    }
}

template <typename VALTYPE, typename OPTYPE>
void test_copy_helper()
{
    TEST_COPY(VALTYPE, OPTYPE, (bsl::in_place),
                 /* no ctor arg list */
                );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place, MovUtl::move(VA1)),
              (MovUtl::move(VA1))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place, VA1),
              (VA1)
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2),
              (MovUtl::move(VA1), VA2)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2)),
              (VA1, MovUtl::move(VA2))
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3),
              (VA1, MovUtl::move(VA2), VA3)
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4))
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5)
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6))
             );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8))
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10))
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12))
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13)),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13))
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13)
              );

    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13), VA14),
              (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
               MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
               MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
               MovUtl::move(VA13), VA14)
             );
    TEST_COPY(VALTYPE, OPTYPE,
              (bsl::in_place,
               VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13, MovUtl::move(VA14)),
              (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
               VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
               VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
               VA13, MovUtl::move(VA14))
              );
}
template <typename VALTYPE, typename OPTYPE>
void test_copyad_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place),
               /* no ctor arg list */,
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, MovUtl::move(VA1)),
               (MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, VA1),
               (VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2),
               (MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &da);

}
template <typename VALTYPE, typename OPTYPE>
void test_copyad_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place),
               (bsl::allocator_arg, &oa),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, VA1),
               (bsl::allocator_arg, &oa, VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &da);

 }
template <typename VALTYPE, typename OPTYPE>
void test_copya_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place),
               ,
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, MovUtl::move(VA1)),
               (MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, VA1),
               (VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2),
               (MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &oa);
}
template <typename VALTYPE, typename OPTYPE>
void test_copya_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&oa);
    // ---   -------------------------------------------------

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place),
               (bsl::allocator_arg, &oa),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, VA1),
               (bsl::allocator_arg, &oa, VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               (bsl::allocator_arg, &oa,
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13), VA14),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               (bsl::allocator_arg, &oa,
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13, MovUtl::move(VA14)),
               &oa);
}
template <typename VALTYPE, typename OPTYPE>
void test_copyil_helper()
{
  TEST_COPY(VALTYPE, OPTYPE, (bsl::in_place, {1,2,3}),
            ({1,2,3})
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3}, MovUtl::move(VA1)),
            ({1,2,3}, MovUtl::move(VA1))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},  VA1),
            ({1,2,3}, VA1)
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2),
            ({1,2,3}, MovUtl::move(VA1), VA2)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2)),
            ({1,2,3}, VA1, MovUtl::move(VA2))
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3)
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4))
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5)
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6))
           );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7)
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8))
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9)
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10))
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11)
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12)
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12))
            );

  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
             MovUtl::move(VA13)),
            ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
             MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
             MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
             MovUtl::move(VA13))
           );
  TEST_COPY(VALTYPE, OPTYPE,
            (bsl::in_place, {1,2,3},
             VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
             VA13),
            ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
             VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
             VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
             VA13)
            );
}
template <typename VALTYPE, typename OPTYPE>
void test_copyila_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}),
               ({1,2,3}),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1)),
               ({1,2,3}, MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}, VA1),
               ({1,2,3},VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               ({1,2,3}, MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               ({1,2,3}, VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);
}
template <typename VALTYPE, typename OPTYPE>
void test_copyila_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);
    bslma::TestAllocator ta("third", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}),
               (bsl::allocator_arg, &oa, {1,2,3}),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3}, VA1),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1), VA2),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1, MovUtl::move(VA2)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &oa);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &oa);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::allocator_arg, &oa, bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &oa);
}
template <typename VALTYPE, typename OPTYPE>
void test_copyilad_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------

    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place, {1,2,3}),
               ({1,2,3}),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, MovUtl::move(VA1)),
               ({1,2,3}, MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, VA1),
               ({1,2,3},VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               ({1,2,3}, MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               ({1,2,3}, VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               ({1,2,3}, MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               ({1,2,3}, VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);

}
template <typename VALTYPE, typename OPTYPE>
void test_copyilad_argt_helper()
{
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("other", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);
    // ---   -------------------------------------------------
    TEST_COPYA(VALTYPE, OPTYPE, (bsl::in_place, {1,2,3}),
               (bsl::allocator_arg, &oa, {1,2,3}),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, MovUtl::move(VA1)),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3}, VA1),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2),
               (bsl::allocator_arg, &oa, {1,2,3}, MovUtl::move(VA1), VA2),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2)),
               (bsl::allocator_arg, &oa, {1,2,3}, VA1, MovUtl::move(VA2)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
              (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
               &da);

    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               (bsl::allocator_arg, &oa, {1,2,3},
                MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                MovUtl::move(VA13)),
               &da);
    TEST_COPYA(VALTYPE, OPTYPE,
               (bsl::in_place, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               (bsl::allocator_arg, &oa, {1,2,3},
                VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                VA13),
               &da);
}
void bslstl_optional_test23()
{
    // ------------------------------------------------------------------------
    // TESTING hash_append facility
    //
    //  This test will verify that hashing functionality works with
    //  bsl::optional objects
    //
    // Concerns:
    //: 1 Hashing a disengaged optional is equivalent to appending 'false'
    //:   to the hash.
    //:
    //: 2 Hashing an engaged optional is equivalent to appending 'true' to
    //    the hash followed by the value.
    //
    // Plan:
    //
    //: 1 Execute test for int (doesn't use allocator) and bsl::string (uses
    //    allocator) value type.
    //
    //: 2 Create a disengaged optional and verify that hashing it yields the
    //    same value as hashing 'false'.
    //
    //: 3 Emplace a series of test values and verify that hashing the optional
    //    produces the same result as hashing 'true' and then the test values
    //    themselves.
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, optional<TYPE>& input);
    // ------------------------------------------------------------------------
  if (verbose) printf(
                      "\nTESTING hash_append FACILITY"
                      "\n============================\n");

   if (verbose) printf( "\nUsing 'bsl::optional<int>'.\n");
   {
       typedef size_t                          ValueType;

       TEST_HASH_EMPTY(ValueType);
       TEST_HASH_ENGAGED(ValueType, (0));
       TEST_HASH_ENGAGED(ValueType, (3));
       TEST_HASH_ENGAGED(ValueType, (777));
       TEST_HASH_ENGAGED(ValueType, (1056));

       TEST_HASH_EMPTY(const ValueType);
       TEST_HASH_ENGAGED(const ValueType, (0));
       TEST_HASH_ENGAGED(const ValueType, (3));
       TEST_HASH_ENGAGED(const ValueType, (777));
       TEST_HASH_ENGAGED(const ValueType, (1056));

   }

   if (verbose) printf( "\nUsing 'bsl::optional<bsl::string>'.\n");
   {
       typedef bsl::string                          ValueType;


       bslma::TestAllocator da("default", veryVeryVeryVerbose);

       bslma::DefaultAllocatorGuard dag(&da);

       TEST_HASH_EMPTY(ValueType);
       TEST_HASH_ENGAGED(ValueType, (""));
       TEST_HASH_ENGAGED(ValueType, ("short string"));
       TEST_HASH_ENGAGED(ValueType, ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                                     "$%^&*()_+-=`{}|[]\\\t\n "));
       TEST_HASH_ENGAGED(ValueType, ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                                     "1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                                     "1234567890abcdefghijklmnopqrstuvwxyz~!@"));

       TEST_HASH_EMPTY(const ValueType);
       TEST_HASH_ENGAGED(const ValueType, (""));
       TEST_HASH_ENGAGED(const ValueType, ("short string"));
       TEST_HASH_ENGAGED(const ValueType,
                         ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                          "$%^&*()_+-=`{}|[]\\\t\n "));
       TEST_HASH_ENGAGED(const ValueType,
                         ("1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                          "1234567890abcdefghijklmnopqrstuvwxyz~!@#"
                          "1234567890abcdefghijklmnopqrstuvwxyz~!@"));


   }
}
void bslstl_optional_test24()
{
    // --------------------------------------------------------------------
    // TESTING in_place_t constructor FUNCTIONALITY
    //   This test will verify that the in_place_t constructor works
    //   as expected.
    //
    // Concerns:
    //   * Calling in_place_t constructor creates an engaged optional whose
    //     value type object is created using theconstructors arguments
    //   * Multiple arguments are correctly forwarded.
    //   * No unnecessary copies of the value type are created
    //   * If no allocators is provided for a type which uses an allocator,
    //     default allocator is used.
    //
    //
    // Plan:
    //
    //
    // Testing:
    //
    //   void optional(in_place_t, Args&&...);
    //   void optional(in_place_t, allocator_arg, allocator, Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING in_place_t constructor "
                       "\n==============================\n");

    if (verbose) printf( "\nUsing 'ConstructTestTypeNoAlloc'.\n");
    {
        typedef ConstructTestTypeNoAlloc                  ValueType;
        test_copy_helper<ValueType, bsl::optional<ValueType> >();
        test_copy_helper<ValueType, const bsl::optional<ValueType> >();
        test_copy_helper<ValueType, bsl::optional< const ValueType> >();
        test_copy_helper<const ValueType, bsl::optional<ValueType> >();
        test_copy_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copy_helper<const ValueType, bsl::optional< const ValueType> >();
    }

    if (verbose) printf( "\nUsing 'ConstructTestTypeAlloc'.\n");
    {
        typedef ConstructTestTypeAlloc                  ValueType;
        test_copyad_helper<ValueType, bsl::optional<ValueType> >();
        test_copyad_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyad_helper<ValueType, bsl::optional< const ValueType> >();
        test_copyad_helper<const ValueType, bsl::optional<ValueType> >();
        test_copyad_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copyad_helper<const ValueType, bsl::optional< const ValueType> >();

        test_copya_helper<ValueType, bsl::optional<ValueType> >();
        test_copya_helper<ValueType, const bsl::optional<ValueType> >();
        test_copya_helper<ValueType, bsl::optional< const ValueType> >();
        test_copya_helper<const ValueType, bsl::optional<ValueType> >();
        test_copya_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copya_helper<const ValueType, bsl::optional< const ValueType> >();
    }
    if (verbose) printf( "\nUsing 'ConstructTestTypeAllocArgT'.\n");
    {
        typedef ConstructTestTypeAllocArgT                ValueType;
        test_copyad_argt_helper<ValueType, bsl::optional<ValueType> >();
        test_copyad_argt_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyad_argt_helper<ValueType, bsl::optional< const ValueType> >();
        test_copyad_argt_helper<const ValueType, bsl::optional<ValueType> >();
        test_copyad_argt_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copyad_argt_helper<const ValueType, bsl::optional< const ValueType> >();

        test_copya_argt_helper<ValueType, bsl::optional<ValueType> >();
        test_copya_argt_helper<ValueType, const bsl::optional<ValueType> >();
        test_copya_argt_helper<ValueType, bsl::optional< const ValueType> >();
        test_copya_argt_helper<const ValueType, bsl::optional<ValueType> >();
        test_copya_argt_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copya_argt_helper<const ValueType, bsl::optional< const ValueType> >();
    }
}

void bslstl_optional_test25()
{
    // --------------------------------------------------------------------
    // TESTING init list in_place constructor FUNCTIONALITY
    //   This test will verify that the init list in_place constructor works
    //   as expected.
    //
    // Concerns:
    //   * When using initializer_list, the correct value type constructors is selected
    //   * Multiple arguments are correctly forwarded.
    //   * emplace can not be used on a const qualified optional.
    //   * If no allocators is provided for a type which uses an allocator,
    //     default allocator is used.
    //
    //
    // Plan:
    //
    //
    // Testing:
    //
    //   void optional(in_place_t, std::initializer_list<U>, Args&&...);
    //   void optional(in_place_t, allocator_arg, allocator, std::initializer_list<U>, Args&&...);
    //
    //   void value();
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING emplace MEMBER FUNCTION "
                       "\n==================================\n");

    if (verbose) printf( "\nUsing 'ConstructTestTypeNoAlloc'.\n");
    {
        typedef ConstructTestTypeNoAlloc                  ValueType;
        test_copyil_helper<ValueType, bsl::optional<ValueType> >();
        test_copyil_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyil_helper<ValueType, bsl::optional< const ValueType> >();
        test_copyil_helper<const ValueType, bsl::optional<ValueType> >();
        test_copyil_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copyil_helper<const ValueType, bsl::optional< const ValueType> >();
    }

    if (verbose) printf( "\nUsing 'ConstructTestTypeAlloc'.\n");
    {
        typedef ConstructTestTypeAlloc                  ValueType;
        test_copyilad_helper<ValueType, bsl::optional<ValueType> >();
        test_copyilad_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyilad_helper<ValueType, bsl::optional< const ValueType> >();
        test_copyilad_helper<const ValueType, bsl::optional<ValueType> >();
        test_copyilad_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copyilad_helper<const ValueType, bsl::optional< const ValueType> >();

        test_copyila_helper<ValueType, bsl::optional<ValueType> >();
        test_copyila_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyila_helper<ValueType, bsl::optional< const ValueType> >();
        test_copyila_helper<const ValueType, bsl::optional<ValueType> >();
        test_copyila_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copyila_helper<const ValueType, bsl::optional< const ValueType> >();
    }
    if (verbose) printf( "\nUsing 'ConstructTestTypeAllocArgT'.\n");
    {
        typedef ConstructTestTypeAllocArgTIL                ValueType;
        test_copyilad_argt_helper<ValueType, bsl::optional<ValueType> >();
        test_copyilad_argt_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyilad_argt_helper<ValueType, bsl::optional< const ValueType> >();
        test_copyilad_argt_helper<const ValueType, bsl::optional<ValueType> >();
        test_copyilad_argt_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copyilad_argt_helper<const ValueType, bsl::optional< const ValueType> >();

        test_copyila_argt_helper<ValueType, bsl::optional<ValueType> >();
        test_copyila_argt_helper<ValueType, const bsl::optional<ValueType> >();
        test_copyila_argt_helper<ValueType, bsl::optional< const ValueType> >();
        test_copyila_argt_helper<const ValueType, bsl::optional<ValueType> >();
        test_copyila_argt_helper<const ValueType, const bsl::optional<ValueType> >();
        test_copyila_argt_helper<const ValueType, bsl::optional< const ValueType> >();
    }
}
void bslstl_optional_test26()
{
  // --------------------------------------------------------------------
  // TESTING operator=(optional_type) OVERLOAD RESOLUTION
  //   This test will verify that the operator=(optional) function works
  //   as expected in the presence of constructors and assignment
  //   operators in value type taking an optional.
  //   In these tests we do not care about the different allocation argument
  //   policies because the allocator propagation is tested in the
  //   functionality tests for operator=. Here, we only care that the correct
  //   overload is selected.
  //   Similarly, we do not care about the allocator state after assignment,
  //   nor do we care about the difference in behaviour when the source and
  //   destination optionals are engaged/disengaged. We deliberately always
  //   use engaged optionals as that directly forwards to the underlying
  //   value type assignment.
  //
  // Concerns:
  //   * if value type is assignable and constructible from an optional of
  //     another value type, the rhs optional is passed in to the value type.
  //     The resulting optional is always engaged.
  //   * assigning {} to an optional creates a disengaged optional
  //
  // Plan:
  //
  //   Conduct the test using 'int' (scalar type, doesn't use allocator),
  //   'my_class1' (non scalar type, doesn't use allocator), and
  //   'my_class2' (non scalar type, uses allocator), for 'TYPE'.
  //
  //   Create a disengaged source optional of each type.
  //   Create a destination optional of optional of each type.
  //   Assign the source object to the destination object. Check the
  //   resulting optional is engaged.
  //
  //   Assign source optional's value type object to destination type. Check
  //   the value has been propagated to the destination's value type object
  //
  //   Move assign source optional's value type object to destination type.
  //   Check the value has been propagated to the destination's value type
  //   object.
  //
  //   Assign {} to each of the destination objects. Check the resulting
  //   optional is disengaged
  //
  //   Emplace a value in the destination object.
  //
  //   Assign nullopt to each destination object. Check the resulting
  //   optional is disengaged
  //
  //
  //
  //
  // Testing:
  //
  //   optional& operator= overload set
  //
  //
  //   void has_value();
  //
  // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING operator=(optional_type) OVERLOAD set"
                       "\n===================================================\n");

    if (verbose) printf( "\nUsing 'int'.\n");
    {
        typedef int                             ValueType;
        typedef bsl::optional<ValueType>        OptV;
        typedef const OptV                      COptV;
        typedef bsl::optional<OptV>             Obj;
        typedef bsl::optional<COptV>             ObjC;

        OptV source = 4;
        Obj destination;
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == 4);

        source = {};
        ASSERT(!source.has_value());
        destination = {};
        ASSERT(!destination.has_value());

        source = 5;
        destination = MovUtl::move(source);
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == 5);

        source = 8;
        ASSERT(source.has_value());
        ASSERT(source.value() == 8);
        destination = 9;
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == 9);

        COptV source2;
        destination = source2;
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());

        destination = 4;
        destination = MovUtl::move(source2);
        ASSERT(destination.has_value());
        ASSERT(!destination.value().has_value());

        source = bsl::nullopt;
        ASSERT(!source.has_value());
        destination = bsl::nullopt;
        ASSERT(!destination.has_value());

        Obj source3;
        destination = source3;
        ASSERT(!destination.has_value());

        ObjC source4;
        destination = source4;
        ASSERT(!destination.has_value());

    }


    if (verbose) printf( "\nUsing 'bsl::string'.\n");
    {
        typedef bsl::string                     ValueType;
        typedef bsl::optional<ValueType>        OptV;
        typedef const OptV                      COptV;
        typedef bsl::optional<OptV>             Obj;

        OptV source("test26");
        Obj destination;
        destination = source;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == "test26");

        source = {};
        ASSERT(!source.has_value());
        destination = {};
        ASSERT(!destination.has_value());


        source = "another test string";
        destination = std::move(source);
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == "another test string");


        COptV source2("test26");
        destination = source2;
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == "test26");

        source = "booboo";
        ASSERT(source.has_value());
        ASSERT(source.value() == "booboo");
        destination = "haha";
        ASSERT(destination.has_value());
        ASSERT(destination.value().value() == "haha");

        destination = MovUtl::move(source2);
        ASSERT(destination.has_value());
        ASSERT(destination.value().has_value());
        ASSERT(destination.value().value() == "test26");
    }

}

void bslstl_optional_test27()
{
    // --------------------------------------------------------------------
    // TESTING swap FACILITY
    // Concerns:
    //   1. Swap of two disengaged objects is a no-op,
    //   2. Swap of an engaged and a disengage doptional moves the value
    //      from the engaged object to another without calling swap for the
    //      value type.
    //   3. Swap of two engaged objects calls swap for the value type.
    //
    // Plan:
    //   Conduct the test using 'Swappable' (doesn't use allocator),
    //   and 'SwappableAA' for 'TYPE'.
    //
    //   For each type, swap two disengaged optional objects and
    //   verify swap has not been called.
    //
    //   For each type, swap two engaged optional objects and
    //   verify swap has been called.
    //
    //   For each type, swap an engaged and disengaged optional
    //   object. Check swap has not been called. Check the correct
    //   values of swapped optional objects.
    //
    //   Execute the tests for both swap member function and free
    //   function
    //
    //
    // Testing:
    //   void swap(optional<TYPE>& other);
    //   void swap(optional<TYPE>& lhs,optional<TYPE>& rhs);
    // --------------------------------------------------------------------

    if (verbose) printf("\nTESTING SWAP METHOD"
                        "\n===================\n");

    using bsl::swap;
    {

        bsl::optional<Swappable> a;
        bsl::optional<Swappable> b;

        Swappable::swapReset();
        swap(a, b);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        Swappable::swapReset();
        a.swap(b);

        ASSERT(!Swappable::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());
    }
    {
        Swappable obj1(1);
        Swappable obj2(2);

        const Swappable Zobj1(obj1);
        const Swappable Zobj2(obj2);

        bsl::optional<Swappable> a = obj1;
        bsl::optional<Swappable> b = obj2;
        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        swap(a, b);
        ASSERT( Swappable::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        Swappable::swapReset();
        ASSERT(!Swappable::swapCalled());
        a.swap(b);
        ASSERT( Swappable::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);
    }
    {
        bsl::optional<Swappable> nonNullObj(Swappable(10));
        bsl::optional<Swappable> nonNullObjCopy(nonNullObj);
        bsl::optional<Swappable> nullObj;

        Swappable::swapReset();
        swap(nonNullObj, nullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        Swappable::swapReset();
        nonNullObj.swap(nullObj);

        ASSERT(!Swappable::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());
    }
    {

        bsl::optional<SwappableAA> a;
        bsl::optional<SwappableAA> b;

        SwappableAA::swapReset();
        swap(a, b);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());

        SwappableAA::swapReset();
        a.swap(b);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(!a.has_value());
        ASSERT(!b.has_value());
    }
    {
        SwappableAA obj1(1);
        SwappableAA obj2(2);

        const SwappableAA Zobj1(obj1);
        const SwappableAA Zobj2(obj2);

        bsl::optional<SwappableAA> a = obj1;
        bsl::optional<SwappableAA> b = obj2;
        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);

        SwappableAA::swapReset();
        ASSERT(!SwappableAA::swapCalled());
        swap(a, b);
        ASSERT( SwappableAA::swapCalled());

        ASSERT(b.value() == Zobj1);
        ASSERT(a.value() == Zobj2);

        SwappableAA::swapReset();
        ASSERT(!SwappableAA::swapCalled());
        a.swap(b);
        ASSERT( SwappableAA::swapCalled());

        ASSERT(a.value() == Zobj1);
        ASSERT(b.value() == Zobj2);
    }
    {
        bsl::optional<SwappableAA> nonNullObj(SwappableAA(10));
        bsl::optional<SwappableAA> nonNullObjCopy(nonNullObj);
        bsl::optional<SwappableAA> nullObj;

        SwappableAA::swapReset();
        swap(nonNullObj, nullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nullObj);
        ASSERT(!nonNullObj.has_value());

        SwappableAA::swapReset();
        nonNullObj.swap(nullObj);

        ASSERT(!SwappableAA::swapCalled());
        ASSERT(nonNullObjCopy == nonNullObj);
        ASSERT(!nullObj.has_value());
    }
}
void bslstl_optional_test28()
{
    // --------------------------------------------------------------------
    // TESTING RELATIONAL OPERATORS
    //
    // Concerns:
    //: 1 We can compare two optional objects if their value types are
    //    comparable. The result depends on whether the objects are engaged
    //    or not
    //:
    //: 2 We can compare an optional object of value type V and a non optional
    //    object of type U if U and V are comparable types. The result depends
    //    on whether the optional object is engaged or not.
    //:
    //: 3 We can compare any optional object with nulllopt_t. The result depends
    //    on whether the optional object is engaged or not.
    //:
    //
    // Plan:
    //: 1 Execute tests for int( doesn't use allocator) and my_class2 (uses
    //    allocator).
    //  2 For each relation operator, execute a combination of comparing:
    //      - two optional objects of different types
    //      - optional object with an object of a different type
    //      - optional object with nullopt_t
    //
    //   For each comparison, execute tests for both engaged and disengaged
    //   optional objects
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


    if (verbose) printf("\nTESTING RELATIONAL OPERATORS"
                        "\n============================\n");
    if (verbose) printf( "\nComparison with an optional.\n");
    {
        typedef bsl::optional<int>                    OptT;
        typedef bsl::optional<my_Class2>              OptV;

        OptT X;
        OptV Y;

        //comparing two disengaged optionals
        ASSERT(  X == Y  ); // If bool(x) != bool(y), false;
        ASSERT(!(X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT(!(X <  Y) ); // If !y, false;
        ASSERT(!(X >  Y) ); // If !x, false;
        ASSERT(  X <= Y  ); // If !x, true;
        ASSERT(  X >= Y  ); // If !y, true;

        //rhs disengaged, lhs engaged
        Y.emplace(3);
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT(!(X >  Y) ); // If !x, false;
        ASSERT(  X <= Y  ); // If !x, true;
        ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;

        //rhs engaged, lhs disengaged
        X.emplace(5);
        Y.reset();
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT( (X >  Y) ); // If !x, false; otherwise, if !y, true;
        ASSERT(!(X <= Y) ); // If !x, true; otherwise, if !y, false;
        ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

        //both engaged, compare the values
        X.emplace(1);
        Y.emplace(3);
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT( (X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT(!(X >  Y) ); // If !x, false; otherwise, if !y, true;
        ASSERT( (X <= Y) ); // If !x, true; otherwise, if !y, false;
        ASSERT(!(X >= Y) ); // If !y, true; otherwise, if !x, false;
    }
    if (verbose) printf( "\nComparison with a non optional .\n");
    {
        typedef bsl::optional<my_Class2>              OptV;

        OptV X;
        int Y = 3;

        //comparison with a disengaged optional on rhs
        ASSERT(!(X == Y) ); // return bool(x) ? *x == v : false;
        ASSERT( (X != Y) ); // return bool(x) ? *x != v : true;
        ASSERT( (X <  Y) ); // return bool(x) ? *x < v : true;
        ASSERT(!(X >  Y) ); // return bool(x) ? *x > v : false;
        ASSERT( (X <= Y) ); // return bool(x) ? *x <= v : true;
        ASSERT(!(X >= Y) ); // return bool(x) ? *x >= v : false;

        //comparison with a disengaged optional on lhs
        ASSERT(!(Y == X) ); // return bool(x) ? v == *x : false;
        ASSERT( (Y != X) ); // return bool(x) ? v != *x : true;
        ASSERT(!(Y <  X) ); // return bool(x) ? v < *x : false;
        ASSERT( (Y >  X) ); // return bool(x) ? v > *x : true;
        ASSERT(!(Y <= X) ); // return bool(x) ? v <= *x : false;
        ASSERT( (Y >= X) ); // return bool(x) ? v >= *x : true;

        //comparison with an engaged optional on rhs
        X.emplace(7);
        ASSERT(!(X == Y) ); // If bool(x) != bool(y), false;
        ASSERT( (X != Y) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT(!(X <  Y) ); // If !y, false; otherwise, if !x, true;
        ASSERT( (X >  Y) ); // If !x, false;
        ASSERT(!(X <= Y) ); // If !x, true;
        ASSERT( (X >= Y) ); // If !y, true; otherwise, if !x, false;

        //comparison with an engaged optional on lhs
        ASSERT(!(Y == X) ); // If bool(x) != bool(y), false;
        ASSERT( (Y != X) ); // If bool(x) != bool(y), true;
                            // otherwise, if bool(x) == false, false;
        ASSERT( (Y <  X) ); // If !y, false; otherwise, if !x, true;
        ASSERT(!(Y >  X) ); // If !x, false;
        ASSERT( (Y <= X) ); // If !x, true;
        ASSERT(!(Y >= X) ); // If !y, true; otherwise, if !x, false;
    }
    if (verbose) printf( "\nComparison with a nullopt_t .\n");
    {
        bsl::optional<my_Class2> X;

        //comparison with a disengaged optional on rhs
        ASSERT( (X == bsl::nullopt) ); // !x
        ASSERT(!(X != bsl::nullopt) ); // bool(x)
        ASSERT(!(X <  bsl::nullopt) ); // false
        ASSERT(!(X >  bsl::nullopt) ); // bool(x)
        ASSERT( (X <= bsl::nullopt) ); // !x
        ASSERT( (X >= bsl::nullopt) ); // true

        //comparison with a disengaged optional on lhs
        ASSERT( (bsl::nullopt == X) ); // !x
        ASSERT(!(bsl::nullopt != X) ); // bool(x)
        ASSERT(!(bsl::nullopt <  X) ); // bool(x)
        ASSERT(!(bsl::nullopt >  X) ); // false
        ASSERT( (bsl::nullopt <= X) ); // true
        ASSERT( (bsl::nullopt >= X) ); // !x

        //comparison with an engaged optional on rhs
        X.emplace(7);
        ASSERT(!(X == bsl::nullopt) ); // !x
        ASSERT( (X != bsl::nullopt) ); // bool(x)
        ASSERT(!(X <  bsl::nullopt) ); // false
        ASSERT( (X >  bsl::nullopt) ); // bool(x)
        ASSERT(!(X <= bsl::nullopt) ); // !x
        ASSERT( (X >= bsl::nullopt) ); // true

        //comparison with an engaged optional on lhs
        ASSERT(!(bsl::nullopt == X) ); // !x
        ASSERT( (bsl::nullopt != X) ); // bool(x)
        ASSERT( (bsl::nullopt <  X) ); // bool(x)
        ASSERT(!(bsl::nullopt >  X) ); // false
        ASSERT( (bsl::nullopt <= X) ); // true
        ASSERT(!(bsl::nullopt >= X) ); // !x
    }
}
void bslstl_optional_test29()
{
    // --------------------------------------------------------------------
    // TESTING make_optional FACILITY
    //
    //  In this test, we are ensuring that the optional created using
    //  make_optional facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an optional is tested in constructor tests.
    //
    //
    // Concerns:
    //: 1 Invoking make_optional creates an optional with the value of the
    //    arguments without any intermediate optional instances
    //
    //: 2 Multiple arguments are correctly forwarded.
    //
    //: 3 Arguments are perfectly forwarded.
    //
    //: 4 If the value type is allocator aware, default allocator is used
    //
    //
    // Plan:
    //: 1 Execute tests for my_class1( doesn't use allocator) and my_class2
    //    (uses allocator).
    //: 2 Call make_optional with an lvalue of each type. Check the value and
    //    allocator (if any) is correct. Check there were no additional copies
    //    created by checking the number of copy and move constructors
    //    invoked.
    //: 3 Call make_optional with an rvalue of each type. Check the value and
    //    allocator (if any) is correct. Check the move constructor was
    //    called. Check there were no additional copies created by checking
    //    the number of copy and move constructors invoked.
    //
    //: 4 Call make_optional to create an optional of ConstructTestTypeNoAlloc
    //    and ConstructTestTypeAlloc type. For each muber of var args check
    //    that:
    //      - the arguments are correctly forwarded
    //      - the arguments are perfectly forwarded
    //      - there are no additional copies of ConstructTestTypeNoAlloc/
    //        ConstructTestTypeAlloc created by checking the numbed of copy and
    //        move constructors invoked.
    //      - for ConstructTestTypeAlloc, check the default allocator is used.
    //
    //: 5 Repeat the above with initializer list as the first argument
    //
    // Testing:
    //      make_optional(T&&);
    //      make_optional(Args&&... args);
    //      make_optional(initializer_list<U> il, Args&&... args);

    if (verbose) printf("\nTESTING make_optional FACILITY"
                        "\n==============================\n");

    if (verbose) printf( "\nDeduced type make optional.\n");
    {
        my_Class1 source(2);

        int CCI = my_Class1::copyConstructorInvocations;
        int MCI = my_Class1::moveConstructorInvocations;

        optional<my_Class1> obj = bsl::make_optional(source);
        ASSERT( 2 == obj.value().value());
        ASSERT( CCI == (my_Class1::copyConstructorInvocations -1));
        ASSERT( MCI == my_Class1::moveConstructorInvocations);

        CCI = my_Class1::copyConstructorInvocations;
        MCI = my_Class1::moveConstructorInvocations;

        optional<my_Class1> obj2 = bsl::make_optional(std::move(source));
        ASSERT( 2 == obj2.value().value());
        ASSERT( CCI == (my_Class1::copyConstructorInvocations));
        ASSERT( MCI == (my_Class1::moveConstructorInvocations  - 1));
    }
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        my_Class2 source(2);

        int CCI = my_Class2::copyConstructorInvocations;
        int MCI = my_Class2::moveConstructorInvocations;

        optional<my_Class2> obj = bsl::make_optional(source);
        ASSERT( 2 == obj.value().value());
        ASSERT( &da == obj.value().d_def.d_allocator_p);
        ASSERT( CCI == (my_Class2::copyConstructorInvocations -1));
        ASSERT( MCI == my_Class2::moveConstructorInvocations);

        CCI = my_Class2::copyConstructorInvocations;
        MCI = my_Class2::moveConstructorInvocations;

        optional<my_Class2> obj2 = bsl::make_optional(std::move(source));
        ASSERT( 2 == obj2.value().value());
        ASSERT( &da == obj2.value().d_def.d_allocator_p);
        ASSERT( CCI == (my_Class2::copyConstructorInvocations));
        ASSERT( MCI == (my_Class2::moveConstructorInvocations  - 1));
    }
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        my_Class2a source(2);

        int CCI = my_Class2a::copyConstructorInvocations;
        int MCI = my_Class2a::moveConstructorInvocations;

        optional<my_Class2a> obj = bsl::make_optional(source);
        ASSERT( 2 == obj.value().value());
        ASSERT( &da == obj.value().d_data.d_def.d_allocator_p);
        ASSERT( CCI == (my_Class2a::copyConstructorInvocations -1));
        ASSERT( MCI == my_Class2a::moveConstructorInvocations);

        CCI = my_Class2a::copyConstructorInvocations;
        MCI = my_Class2a::moveConstructorInvocations;

        optional<my_Class2a> obj2 = bsl::make_optional(std::move(source));
        ASSERT( 2 == obj2.value().value());
        ASSERT( &da == obj2.value().d_data.d_def.d_allocator_p);
        ASSERT( CCI == (my_Class2a::copyConstructorInvocations));
        ASSERT( MCI == (my_Class2a::moveConstructorInvocations  - 1));
    }
    if (verbose) printf( "\nvar arg make optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        TEST_MAKEOP((VA1),&da);
        TEST_MAKEOP((MovUtl::move(VA1)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2, VA3),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2), VA3),&da);
        TEST_MAKEOP((VA1, VA2, MovUtl::move(VA3)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11), VA12),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11, MovUtl::move(VA12)),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11), VA12,
                     MovUtl::move(VA13)),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11, MovUtl::move(VA12),
                     VA13),&da);

        TEST_MAKEOP((MovUtl::move(VA1), VA2,
                     MovUtl::move(VA3), VA4,
                     MovUtl::move(VA5), VA6,
                     MovUtl::move(VA7), VA8,
                     MovUtl::move(VA9), VA10,
                     MovUtl::move(VA11), VA12,
                     MovUtl::move(VA13), VA14),&da);
        TEST_MAKEOP((VA1, MovUtl::move(VA2),
                     VA3, MovUtl::move(VA4),
                     VA5, MovUtl::move(VA6),
                     VA7, MovUtl::move(VA8),
                     VA9, MovUtl::move(VA10),
                     VA11, MovUtl::move(VA12),
                     VA13, MovUtl::move(VA14)),&da);
    }
    if (verbose) printf( "\ninitializer_list make optional.\n");
    {
       bslma::TestAllocator da("default", veryVeryVeryVerbose);
       bslma::DefaultAllocatorGuard dag(&da);
       TEST_MAKEOP(({1,2,3}),&da);

       TEST_MAKEOP(({1,2,3}, VA1),&da);
       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2, VA3),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2), VA3),&da);
       TEST_MAKEOP(({1,2,3}, VA1, VA2, MovUtl::move(VA3)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12)),&da);

       TEST_MAKEOP(({1,2,3}, MovUtl::move(VA1), VA2,
                    MovUtl::move(VA3), VA4,
                    MovUtl::move(VA5), VA6,
                    MovUtl::move(VA7), VA8,
                    MovUtl::move(VA9), VA10,
                    MovUtl::move(VA11), VA12,
                    MovUtl::move(VA13)),&da);
       TEST_MAKEOP(({1,2,3}, VA1, MovUtl::move(VA2),
                    VA3, MovUtl::move(VA4),
                    VA5, MovUtl::move(VA6),
                    VA7, MovUtl::move(VA8),
                    VA9, MovUtl::move(VA10),
                    VA11, MovUtl::move(VA12),
                    VA13),&da);

    }
}
void bslstl_optional_test30()
{
    // --------------------------------------------------------------------
    // TESTING alloc_optional FACILITY
    //
    //  In this test, we are ensuring that the optional created using
    //  alloc_optional facility is created using the given arguments and
    //  without unnecessary copies. We are not worried about the allocator
    //  policy of the allocator aware type, as choosing the allocator policy
    //  when creating an optional is tested in constructor tests.
    //
    //
    // Concerns:
    //: 1 Invoking alloc_optional creates an optional which uses the specified
    //    allocator and with the value of the given arguments without any
    //    intermediate optional instances
    //
    //: 2 Multiple arguments are correctly forwarded.
    //
    //: 3 Arguments are perfectly forwarded.
    //
    //
    // Plan:
    //: 1 Execute tests for my_class2.
    //: 2 Call alloc_optional with an lvalue of my_class2. Check the value and
    //    allocator of the created optional is correct. Check there were no
    //    additional copies created by checking the number of copy and move
    //    constructors invoked.
    //: 3 Call alloc_optional with an rvalue of of my_class2. Check the value
    //    and allocator of the created optional is correct. Check the move
    //    constructor was used. Check there were no additional copies created
    //    by checking the number of copy and move constructors invoked.
    //
    //: 4 Repeat the test with initializer list overload.
    //
    //: 5 Call alloc_optional to create an optional of ConstructTestTypeAlloc
    //    type. For each number of arguments check that:
    //      - correct allocator is used
    //      - the arguments are correctly forwarded
    //      - the arguments are perfectly forwarded
    //      - there are no additional copies of ConstructTestTypAlloc created
    //        by checking the numbed of copy and move constructors invoked.
    //
    //
    // Testing:
    //      alloc_optional(const bsl::allocator<char>& a, T&&);
    //      alloc_optional(const bsl::allocator<char>>& a, Args&&... args);
    //      alloc_optional(const bsl::allocator<char><>& a,
    //                     initializer_list<U> il,
    //                     Args&&... args);
    //

    if (verbose) printf("\nTESTING alloc_optional FACILITY"
                        "\n==============================\n");

    if (verbose) printf( "\nDeduced type alloc_optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);
        my_Class2 source(2);

        int CCI = my_Class2::copyConstructorInvocations;
        int MCI = my_Class2::moveConstructorInvocations;

        optional<my_Class2> obj = bsl::alloc_optional(&oa, source);
        ASSERT( 2 == obj.value().value());
        ASSERT( &oa == obj.value().d_def.d_allocator_p);
        ASSERT( CCI == (my_Class2::copyConstructorInvocations -1));
        ASSERT( MCI == my_Class2::moveConstructorInvocations);

        CCI = my_Class2::copyConstructorInvocations;
        MCI = my_Class2::moveConstructorInvocations;

        optional<my_Class2> obj2 = bsl::alloc_optional(&oa, std::move(source));
        ASSERT( 2 == obj2.value().value());
        ASSERT( &oa == obj2.value().d_def.d_allocator_p);
        ASSERT( CCI == (my_Class2::copyConstructorInvocations));
        ASSERT( MCI == (my_Class2::moveConstructorInvocations  - 1));
    }
    if (verbose) printf( "\nvar arg alloc_optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_ALLOCOP((&da, VA1),
                     (VA1, &da), &da);
        TEST_ALLOCOP((&da, MovUtl::move(VA1)),
                      (MovUtl::move(VA1), &da), &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2),
                     (MovUtl::move(VA1), VA2, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2)),
                     (VA1, MovUtl::move(VA2), &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3),
                     (VA1, MovUtl::move(VA2), VA3, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6),&da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9),&da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11),&da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      &da),
                     &da);

        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13)),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, &da),
                     &da);


        TEST_ALLOCOP((&da,MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), VA14),
                     (MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), VA14, &da),
                     &da);
        TEST_ALLOCOP((&da,VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, MovUtl::move(VA14)),
                     (VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, MovUtl::move(VA14)),
                     &da);

    }
    if (verbose) printf( "\ninitializer_list alloc_optional.\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("other", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TEST_ALLOCOP((&da,{1,2,3}),
                     ({1,2,3}, &da), &da);

        TEST_ALLOCOP((&da,{1,2,3}, VA1),
                     ({1,2,3}, VA1, &da), &da);
        TEST_ALLOCOP((&da,{1,2,3}, MovUtl::move(VA1)),
                      ({1,2,3}, MovUtl::move(VA1), &da), &da);

        TEST_ALLOCOP((&da, {1,2,3}, MovUtl::move(VA1), VA2),
                     ({1,2,3}, MovUtl::move(VA1), VA2, &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3}, VA1, MovUtl::move(VA2)),
                     ({1,2,3}, VA1, MovUtl::move(VA2), &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6),&da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9),&da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                       VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11),&da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12)),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      &da),
                     &da);

        TEST_ALLOCOP((&da, {1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13)),
                     ({1,2,3},
                      MovUtl::move(VA1), VA2, MovUtl::move(VA3), VA4,
                      MovUtl::move(VA5), VA6, MovUtl::move(VA7), VA8,
                      MovUtl::move(VA9), VA10, MovUtl::move(VA11), VA12,
                      MovUtl::move(VA13), &da),
                     &da);
        TEST_ALLOCOP((&da, {1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13),
                     ({1,2,3},
                      VA1, MovUtl::move(VA2), VA3, MovUtl::move(VA4),
                      VA5, MovUtl::move(VA6), VA7, MovUtl::move(VA8),
                      VA9, MovUtl::move(VA10), VA11, MovUtl::move(VA12),
                      VA13, &da),
                     &da);


    }

}
int main(int argc, char **argv)
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;


    printf( "TEST  %s CASE %d \n", __FILE__, test );

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 30:
        bslstl_optional_test30();
        break;
      case 29:
        bslstl_optional_test29();
        break;
      case 28:
        bslstl_optional_test28();
        break;
      case 27:
        bslstl_optional_test27();
        break;
      case 26:
        bslstl_optional_test26();
        break;
      case 25:
        bslstl_optional_test25();
        break;
      case 24:
        bslstl_optional_test24();
        break;
      case 23:
        bslstl_optional_test23();
        break;
      case 22:
        bslstl_optional_test22();
        break;
      case 21:
        bslstl_optional_test21();
        break;
      case 20:
        bslstl_optional_test20();
        break;
      case 19:
        bslstl_optional_test19();
        break;
      case 18:
        bslstl_optional_test18();
        break;
      case 17:
        bslstl_optional_test17();
        break;
      case 16:
        bslstl_optional_test16();
        break;
      case 15:
        bslstl_optional_test15();
        break;
      case 14:
        bslstl_optional_test14();
        break;
      case 13:
        bslstl_optional_test13();
        break;
      case 12:
        bslstl_optional_test12();
        break;
      case 11:
        bslstl_optional_test11();
        break;
      case 10:
        bslstl_optional_test10();
        break;
      case 9:
        bslstl_optional_test9();
        break;
     case 8:
        bslstl_optional_test8();
        break;
      case 7:
        bslstl_optional_test7();
        break;
      case 6:
        bslstl_optional_test6();
        break;
      case 5:
        bslstl_optional_test5();
        break;
      case 4:
        bslstl_optional_test4();
        break;
      case 3:
         bslstl_optional_test3();
         break;
      case 2:
        bslstl_optional_test2();
        break;
      case 1:
        bslstl_optional_test1();
        break;
      default: {
        printf( "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
      printf( "Error, non-zero test status = %d .\n", testStatus);
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
