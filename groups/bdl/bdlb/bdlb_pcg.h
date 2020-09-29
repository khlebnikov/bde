// bdlb_pcg.h                                                         -*-C++-*-
#ifndef INCLUDED_BDLB_PCG
#define INCLUDED_BDLB_PCG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class to generate random numbers using the PCG algorithm.
//
//@CLASSES:
//  bdlb::PCG: random number generator
//
//@SEE_ALSO: bdlb_random
//
//@DESCRIPTION: This component provides a single class, 'bdlb::PCG', that is
// used to generate random numbers employing the PCG algorithm, a
// high-performance, high-quality RNG.  The PCG uses a linear congruential
// generator as the state-transition function.
//
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Generating a high-quality GUID
///- - - - - - - - - - - - - - - - - - - - -
// In order to generate Guids more quickly than, for instance, reading from
// /dev/urandom.  In this example, we attempt to use /dev/urandom to obtain a
// seed for our PCG. If that fails, we use a backup seed.
//
//         bdlb::PCG      theRNG;
//         uint64_t       seed;
//         int            rtn_val =
//             readFile((unsigned char *)&seed, sizeof(seed), "/dev/urandom");
//         if (rtn_val != sizeof(seed)) {
//             seed = time(NULL) ^ (intptr_t)&bsl::printf;  //backup seed
//         }
//         int rounds = 5;  // see PCG code base for details of 'rounds'
//         theRNG.seed(seed, (intptr_t)&rounds);
//
//
//        size_t numBytesToFill = (numGuids * 16);
//        for (size_t i = 0; i < numBytesToFill; i += sizeof(bsl::uint32_t)) {
//            bsl::uint32_t rnd_int = pcg.getRandom();
//              memcpy(bytes + i,
//                &rnd_int,
//                sizeof(rnd_int));  // possible to unlock for this line
//        }
//
//        bytes = result; while (bytes != end) {
//             typedef unsigned char uc;
//             bytes[6] = uc(0x40 | (bytes[6] & 0x0F));
//             bytes[8] = uc(0x80 | (bytes[8] & 0x3F));
//             bytes += 16;
//        }
//

#include <bsl_cstdint.h>

namespace BloombergLP
{
    namespace bdlb
    {

        // =========
        // class PCG
        // =========
        class PCG
        {
            // This mechanism class implements a random number generator (RNG) based on
            // the PCG algorithm.  For details of the algorithm, see
            // http://www.pcg-random.org.  This class does not throw exceptions.

        private:
            // DATA
            bsl::uint64_t d_state; // RNG state.  All values are possible.

            bsl::uint64_t d_streamSelector; // Controls which RNG sequence (stream) is
                                            // selected. Must *always* be odd.

            // FRIENDS
            friend bool operator==(const PCG &lhs, const PCG &rhs);

        public:
            // CREATORS
            PCG(bsl::uint64_t initState = 0, bsl::uint64_t streamSelector = 1);
            // Create a 'PCG' object and seed it with the specified 'initState' and
            // 'streamSelector.' For the description of the parameters, refer to
            // the 'seed' method. Note that 'seed' invoked in the body of this
            // constructor.

            PCG(const PCG &original);
            // Create a 'PCG' object having the same state as the specified
            // 'original' object. Note that this newly created object will generate
            // the same sequence of numbers as the 'original' object.

            // MANIPULATORS
            PCG &operator=(const PCG &rhs);
            // Assign to this object the value of the specified 'rhs' object, and
            // return a non-'const' reference to this object. Note that this newly
            // created object will generate the same sequence of numbers as the
            // 'original' object.

            void seed(bsl::uint64_t initState, bsl::uint64_t streamSelector);
            // Seed the RNG with the specified 'initState' and 'streamSelector'.
            // 'initState' is the starting state for the RNG.  Any 64-bit value may
            // be passed. 'streamSelector' selects the output sequence for the RNG.
            // Any 64-bit value may be passed, although only the low 63 bits are
            // significant.  There are 2^63 different RNGs available, and
            // 'streamSelector' selects from among them.  Invoking different
            // instances with the identical 'initState' and 'streamSelector' will
            // result in the same sequence of random numbers from subsequent
            // invocations of getRandom().

            bsl::uint32_t getRandom();
            // Return the next random number in the sequence.
        };

        // FREE OPERATORS
        bool operator==(const PCG &lhs,
                        const PCG &rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
        // value, and 'false' otherwise.  Two 'PCG' objects have
        // the same value if both of the corresponding values of their
        // 'd_state' and 'd_streamSelector' attributes
        // are the same.

        bool operator!=(const PCG &lhs,
                        const PCG &rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
        // same value, and 'false' otherwise.  Two 'PCG' objects do
        // not have the same value if either of the corresponding values of their
        // 'd_state' or 'd_streamSelector' attributes are
        // not the same.

        // ============================================================================
        //                            INLINE DEFINITIONS
        // ============================================================================

        // ---------
        // class PCG
        // ---------

        // CREATORS
        inline PCG::PCG(bsl::uint64_t initState, bsl::uint64_t streamSelector)
        {
            seed(initState, streamSelector);
        }

        inline PCG::PCG(const PCG &original)
            : d_state(original.d_state), d_streamSelector(original.d_streamSelector)
        {
        }

        // MANIPULATORS
        inline PCG &PCG::operator=(const PCG &rhs)
        {
            d_state = rhs.d_state;
            d_streamSelector = rhs.d_streamSelector;
            return *this;
        }

        inline void PCG::seed(bsl::uint64_t initState, bsl::uint64_t streamSelector)
        {
            d_state = 0U;
            d_streamSelector = (streamSelector << 1u) | 1u;
            getRandom();
            d_state += initState;
            getRandom();
        }

        inline bsl::uint32_t PCG::getRandom()
        {
            bsl::uint64_t oldstate = d_state;
            d_state = oldstate * 6364136223846793005ULL + d_streamSelector;
            bsl::uint32_t xorshifted =
                static_cast<bsl::uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
            bsl::uint32_t rot = static_cast<bsl::uint32_t>(oldstate >> 59u);
            return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        }

    } // namespace bdlb

    // FREE OPERATORS
    inline bool bdlb::operator==(const PCG &lhs,
                                 const PCG &rhs)
    {
        return lhs.d_state == rhs.d_state && lhs.d_streamSelector == rhs.d_streamSelector;
    }

    inline bool bdlb::operator!=(const PCG &lhs,
                                 const PCG &rhs)
    {
        return !(lhs == rhs);
    }

} // namespace BloombergLP

#endif

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