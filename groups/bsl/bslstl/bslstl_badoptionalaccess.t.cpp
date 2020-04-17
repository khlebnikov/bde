// bslstl_badoptionalaccess.t.cpp                                     -*-C++-*-
#include <bslstl_badoptionalaccess.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The type under test is 'bsl::bad_optional_access', an exception type whose
// interface and contract is dictated by the C++ standard.  If
// 'std::bad_optional_access' exception is available, we need to check that
// 'bsl::bad_optional_access' is a typedef to the standard's exception type.
// If 'std::bad_optional_access' exception isn't available, we need to check
// that 'bsl::bad_optional_access' is a typedef to the
// 'bslstl::BadOptionalAccess' type, and that 'bslstl::BadOptionalAccess'
// satisfies the interface and contract of 'std::bad_optional_access'.
//
// ----------------------------------------------------------------------------
// creators:
// [ 3] BadOptionalAccess();
//
// accessors:
// [ 3] const char *what() const;
//
// typedef:
// [ 4] typedef bsl::bad_optional_access
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: Methods qualified 'noexcept' in standard are so.

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                         GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
//  bool veryVerbose         = argc > 3;
//  bool veryVeryVerbose     = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // 'bsl::bad_optional_access' TYPE
        //
        // Concerns:
        //: 1 The 'bsl::bad_optional_access' is a typedef for
        //:   'bslstl::BadOptionalAccess' if 'std::bad_optional_access' isn't
        //:   available.
        //: 2 The 'bsl::bad_optional_access' is a typedef for
        //:   'std::bad_optional_access' if 'std::bad_optional_access' is
        //:    available.
        //
        // Plan:
        //: 1 For concern 1, if we're using CPP17 library, check that
        //:   'bsl::bad_optional_access' is the same type as
        //:   'bslstl::BadOptionalAccess' using 'bsl::is_same'.
        //: 2 For concern 2, if we're using CPP17 library, check that
        //:   'bsl::bad_optional_access' is the same type as
        //:   'std::bad_optional_access' using 'bsl::is_same'.
        //
        // Testing:
        //   typedef bsl::bad_optional_access
        // --------------------------------------------------------------------
        if (verbose) printf("\n'bsl::bad_optional_access' TYPE"
                            "\n===============================\n");

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

        ASSERT((bsl::is_same<bsl::bad_optional_access,
                             bslstl::BadOptionalAccess>::value));

#else

        ASSERT((bsl::is_same<bsl::bad_optional_access,
                             std::bad_optional_access>::value));

#endif //BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTION AND 'what' METHOD
        //
        // Concerns:
        //: 1 A 'BadOptionalAccess' object can be default constructed.
        //: 2 Invoking a 'what' method on a 'BadOptionalAccess' object returns
        //:   a "bad_optional_access" string.
        //: 3 'what' method can be invoked on a const 'BadOptionalAccess'
        //:   object
        //
        // Plan:
        //: 1 For concern 1, default construct a 'BadOptionalAccess' object.
        //: 2 For concern 2, invoke the 'what' method on a 'BadOptionalAccess'
        //:   object and check that the returned string is
        //:   "bad_optional_access".
        //: 3 For concern 3, in step 2, use a const qualified
        //:   'BadOptionalAccess' object.
        //
        // Testing:
        //   BadOptionalAccess();
        //   const char *what() const;
        // --------------------------------------------------------------------
        if (verbose) printf("\nDEFAULT CONSTRUCTION AND 'what' METHOD"
                            "\n======================================\n");
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        const bslstl::BadOptionalAccess b;
        ASSERT(0 == strcmp("bad_optional_access", b.what()));
#endif // #ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        //
        // Concerns:
        //: 1 The 'noexcept' specification has been applied to
        //:   'BadOptionalAccess' constructor as required by the standard.
        //: 2 The 'noexcept' specification has been applied to
        //:   'BadOptionalAccess' 'what' method as required by the standard.
        // Plan:
        //: 1 Apply the unary 'noexcept' operator to a 'BadOptionalAccess'
        //:   constructor and, for concern 1, confirm that calculated boolean
        //:   value matches the expected value.
        //: 2 Apply the unary 'noexcept' operator to 'BadOptionalAccess' 'what'
        //:   method and, for concern 2,  confirm that calculated boolean value
        //:   matches the expected value.
        //
        // Testing:
        //   CONCERN: Methods qualified 'noexcept' in standard are so.
        // --------------------------------------------------------------------

        if (verbose) printf("\n'noexcept' SPECIFICATION"
                            "\n========================\n");

        // N4835: 20.6.5 Class 'bad_optional_access' [optional.bad.access]
        //..
        //     namespace std {
        //       class bad_optional_access: public std::exception {
        //       public:
        //         const char* what() const noexcept override;
        //       };
        //     } // namespace std
        //..
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(bslstl::BadOptionalAccess()));

        bslstl::BadOptionalAccess    b;
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(b.what()));
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        bslstl::BadOptionalAccess    b;
        const native_std::exception *ptr = &b;

        ASSERT(0 == strcmp("bad_optional_access", b.what()));
        ASSERT(0 == strcmp("bad_optional_access", ptr->what()));
#endif

        const bsl::bad_optional_access b2;
        const native_std::exception *ptr2 = &b2;

        ASSERT(0 != b2.what());
        ASSERT(0 != ptr2->what());

      }break;
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
