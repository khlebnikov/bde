// bdlb_pcg.t.cpp                                                     -*-C++-*-

#include <bdlb_pcg.h>

#include <bslim_testutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdint.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;


// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a random number generator based on the
// PCG (Permuted Congruential Generator) algorithm.
//
// Primary Manipulators:
//: o 'seed'
//: o 'getRandom'
//
// This mechanism class  provides a 'seed' function takes as parameters 2
// values: an 'initState' and a 'streamSelector'.
//
//
//
// ----------------------------------------------------------------------------
// CLASS METHODS
//
// CREATORS

//
// MANIPULATORS
// [10] operator=(const baltzo::LocalTimeDescriptor& rhs);
// [11] operator=(bslmf::MovableRef<baltzo::LocalTimeDescriptor> rhs);

//
// ACCESSORS

//
// FREE OPERATORS

//
// FREE FUNCTIONS

// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q BSLIM_TESTUTIL_Q    // Quote identifier literally.
#define P BSLIM_TESTUTIL_P    // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

namespace {

bool verbose             = false;
bool veryVerbose         = false;
bool veryVeryVerbose     = false;
bool veryVeryVeryVerbose = false;


void checkDifferentSeeds()
{
    bdlb::PCG g1, g2;
    int ptr1   = 10;
    int rounds = 5;
    g1.seed((uint64_t)&ptr1, (uint64_t)&rounds);
    g2.seed(((uint64_t) & ptr1) + 1, (uint64_t)&rounds);

    const int CHECKS = 100;
    if (verbose)
        cout << "Checking uniqueness of " << CHECKS << " iteration"
                  << endl;

    for (int i = 0; i < CHECKS; ++i) {
        // if (veryVerbose) ... display the #s?
        ASSERT(g1.getRandom() != g2.getRandom());
    }

    if (verbose)
        cout << "Uniqueness test done." << endl;
}

void checkAgainstReferenceImplementation()
{
    // we have previously generated values from the implementation provided by
    // the PCG website
    const int           SIZE_OF_SAMPLES = 100;
    const uint64_t SAMPLE_SEED     = 42u;
    const uint64_t SAMPLE_SEQ      = 54u;

    uint32_t referenceValues[SIZE_OF_SAMPLES] = {
        2707161783, 2068313097, 3122475824, 2211639955, 3215226955, 3421331566,
        3217466285, 2167406445, 3860803674, 4181216144, 853247742,  499135993,
        3984091174, 941769757,  731976663,  475758987,  2721289578, 2228905443,
        3470160530, 2998992390, 2441179440, 1442744599, 1206460561, 1214968473,
        2984805051, 3261196357, 446402806,  2036656260, 1597429668, 518128941,
        2233071061, 691883599,  1838127612, 3275887881, 2691487686, 3828376787,
        3792673776, 1075531959, 2398224190, 3814187698, 2762927671, 718553706,
        2635185812, 1922090326, 1852782471, 84684515,   1339504387, 3338618763,
        1260167649, 374663825,  3439899378, 4160699816, 2024913114, 2701156396,
        2508740703, 3657599091, 1723134838, 241912730,  1209430164, 223923616,
        2406627518, 4154033139, 132502308,  2910379858, 355646068,  508074466,
        1819664228, 387832886,  4074297162, 619108615,  1199635762, 2598322316,
        2723497167, 4022773560, 446839380,  3315678907, 4147768777, 3282048506,
        1029575953, 2948913147, 1061239646, 1180748659, 2298246975, 3602830748,
        1672721738, 3598532062, 590958475,  322456388,  3716889276, 4284064286,
        1001483646, 1845611368, 3962861838, 2993439405, 3609446448, 3755020234,
        2117479073, 2188374819, 3415172021, 2603755493};

    bdlb::PCG gen;
    gen.seed(SAMPLE_SEED, SAMPLE_SEQ);
    if (verbose)
        cout << "Checking correctness of " << SIZE_OF_SAMPLES
                  << " samples" << endl;
    for (int i = 0; i < SIZE_OF_SAMPLES; ++i) {
        ASSERT(gen.getRandom() == referenceValues[i]);
    }
    if (verbose)
        cout << "Correctness test done." << endl;
}
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // HJB TBD - how is the numbering of the test cases done?
    // localtimedescriptor starts with case 14, random starts with case 3; why?

    switch (test) {
      case 0:  // Zero is always the leading case.
        checkDifferentSeeds();
        checkAgainstReferenceImplementation();
        break;
      case 1:
        checkDifferentSeeds();
        break;
      case 2:
        checkAgainstReferenceImplementation();
        break;
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\n"
                    "USAGE EXAMPLE"
                    "\n"
                    "============="
                    "\n";
        // doUsageExample();
        break;
      }
    }
}
