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


using namespace BloombergLP;
using namespace bsl;
// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
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

namespace std
{
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
void bslstl_optional_test1()
{
    // --------------------------------------------------------------------
    // TESTING DISENGAGED CONSTRUCTORS AND BASIC ACCESSORS
    //   This test will verify that the construction of a disengaged optional is working as
    //   expected.  Also, we test that the basic accessors are working as
    //   expected.
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
    //   resulting object's allocator is the default allocator. //TODO
    //
    //   Next, verify the constructor taking nullopt_t by testing that the
    //   resulting object is null. For bsl::string, verify that the
    //   resulting object's allocator is the default allocator. //TODO
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

        ValueType *addr;

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

        typedef bsl::string                    ValueType;
        typedef bsl::optional<ValueType> Obj;

        ValueType *addr;

        ASSERT(sizeof(ValueType) == sizeof(Obj::value_type));

        if (veryVerbose) printf( "\tTesting default constructor with default allocator.\n");

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bslma::TestAllocatorMonitor dam(&da);

            Obj mX;
            const Obj& X = mX;
            ASSERT(!X.has_value());
            ASSERT(X.get_allocator().mechanism() == &da);
            ASSERT(dam.isTotalSame());

        }
        if (veryVerbose) printf( "\tTesting nullopt_t constructor with default allocator.\n");
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
            ASSERT(false == X);
            ASSERT(false == X.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace(1);
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.emplace(3);
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX = Obj(2);
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
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
            ASSERT(false == X);
            ASSERT(false == X.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");

            mX.emplace("tralala");
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.emplace("");
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
            ASSERT(false == mX.has_value());
        }
        {
            Obj mX = Obj(ValueType("tralala"));
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
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
    //
    // Plan:
    //   Conduct the test using 'int' (does not use allocator) and
    //   'bsl::string' (uses allocator) for 'TYPE'.
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
    //   optional(bsl::allocator_arg_t, allocator_type basicAllocator, const optional& original;
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
            ASSERT(false == mX);
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
            ASSERT(false == mX.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace(1);
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
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
            ASSERT(false == mX);
            ASSERT(false == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
            ASSERT(false == mX.has_value());

            if (veryVerbose) printf( "\templacing a value.\n");
            mX.emplace("tralala");
            ASSERT(true == mX);
            ASSERT(true == mX.has_value());

            mX.reset();
            ASSERT(false == mX);
            ASSERT(false == mX.has_value());


            Obj nX = Obj(bsl::allocator_arg, bsl::allocator<char>(&oa), mX);
            ASSERT(mX.get_allocator().mechanism() == &da);
            ASSERT(nX.get_allocator().mechanism() == &oa);
            nX.reset();
            ASSERT(false == nX);
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
    //   * Calling value() on a disengaged optional throws bad_optional_access exception.
    //   * It is possible to call value() on a constant optional object
    //   * It is possible to call value() on a temporary optional object
    //   * It is possible to modify non constant optional through call of value()
    //   * It is not possible to modify non constant optional through call of value().
    //     Note that this requires tests which check for compilation errors.
    //   * It is possible to call value() on a temporary optional object
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
    //   bool value() const;
    //
    //   void reset();
    //   optional(const T &);
    //   optional(bsl::allocator_arg_t, allocator_type basicAllocator, const optional& original;
    //   emplace(const T &);
    //
    // --------------------------------------------------------------------

    if (verbose) printf(
                       "\nTESTING RESET MEMBER FUNCTION "
                       "\n================================================\n");

    if (verbose) printf( "\nUsing 'bsl::optional<int>'.\n");
    bool unexpected_exception_thrown = false;
    try
    {
        typedef int                            ValueType;
        typedef bsl::optional<ValueType> Obj;

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
            ASSERT(i = 4);
#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUE1)
            X.value() = 2; // this should not compile
#endif
        }
    } catch (...)
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

        Obj mX; const Obj& X = mX;

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
        ri = "test string 2";

        ASSERT(mX.value() == "test string 2");

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

        bsl::string i = Obj("test string 4").value();
        ASSERT(i == "test string 4");

#if defined(BSLSTL_OPTIONAL_TEST_BAD_VALUE2)
        X.value() = "test string 5"; // this should not compile
#endif

    }catch (...)
    {
      unexpected_exception_thrown = true;
    }
    ASSERT(unexpected_exception_thrown == false);
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
      case 1:
        bslstl_optional_test1();
        break;
      case 2:
        bslstl_optional_test2();
        break;
      case 3:
        bslstl_optional_test3();
        break;
      case 4:
        bslstl_optional_test4();
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
