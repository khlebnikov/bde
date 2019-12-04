// bslalg_constructusingallocator.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_CONSTRUCTUSINGALLOCATOR
#define INCLUDED_BSLALG_CONSTRUCTUSINGALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")
// todo open questions :
// - should this be implemented in terms of bsl::allocator or bslma::allocator ?
// - which package to put this in ?
// - what to call it ?
// - add documentation to implementation
// - add documentation to tests


//@PURPOSE: Provide primitive implementation fo construction using an allocator.
//
//@CLASSES:

//
//@DESCRIPTION: This component provides a primitive implementation of
// a construction using an allocator.
// The traits under consideration by this component are:
//..
//  Trait                                         Description
//  --------------------------------------------  -----------------------------
//
//  bslma::UsesBslmaAllocator                     "the 'TYPE' constructor takes
//                                                an allocator argument", or
//                                                "'TYPE' supports 'bslma'
//                                                allocators"
//
//  bslmf::UsesAllocatorArgT                      "the 'TYPE' constructor takes
//                                                an allocator argument", and
//                                                optionally passes allocators
//                                                as the first two arguments to
//                                                each constructor, where the
//                                                tag type 'allocator_arg_t' is
//                                                first, and the allocator type
//                                                is second.
//..
//
///Usage
///-----
// todo

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_movableref.h>
#include <bslmf_usesallocatorargt.h>
#include <bslmf_util.h>



namespace BloombergLP {

namespace bslalg {

template <class TARGET_TYPE, bool = bslma::UsesBslmaAllocator<TARGET_TYPE>::value,
          bool= bslmf::UsesAllocatorArgT<TARGET_TYPE>::value >
struct AllocatorUtil{
};


template <class TARGET_TYPE>
struct AllocatorUtil<TARGET_TYPE, true, false>{

  static TARGET_TYPE construct(bslma::Allocator   *allocator)
  {
    return TARGET_TYPE(allocator);
  }

  static TARGET_TYPE construct (bslmf::MovableRef<TARGET_TYPE> original,
                  bslma::Allocator                                  *allocator)
  {
    return TARGET_TYPE(bslmf::MovableRefUtil::move(original), allocator);
  }

  template <class ANY_TYPE>
  static TARGET_TYPE construct (BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
                  bslma::Allocator                                  *allocator)
  {
    return TARGET_TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE,original), allocator);
  }

  template <class ANY_TYPE>
  static TARGET_TYPE construct (bslmf::MovableRef<ANY_TYPE> original,
                                bslma::Allocator  *allocator)
  {
    return TARGET_TYPE(bslmf::MovableRefUtil::move(original), allocator);
  }
};

template <class TARGET_TYPE, bool USES_BSLMA>
struct AllocatorUtil<TARGET_TYPE, USES_BSLMA, true>{

  static TARGET_TYPE construct(bslma::Allocator   *allocator)
  {
    return TARGET_TYPE(bsl::allocator_arg, allocator);
  }
  static TARGET_TYPE construct (const TARGET_TYPE&                                 original,
                  bslma::Allocator                                  *allocator)
  {
    return TARGET_TYPE(bsl::allocator_arg, allocator, original);
  }
  static TARGET_TYPE construct (bslmf::MovableRef<TARGET_TYPE> original,
                    bslma::Allocator                                  *allocator)
    {
      return TARGET_TYPE(bsl::allocator_arg, allocator, bslmf::MovableRefUtil::move(original));
    }

  template <class ANY_TYPE>
  static TARGET_TYPE construct (BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
                  bslma::Allocator                                  *allocator)
  {
    return TARGET_TYPE(bsl::allocator_arg, allocator,
        BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE,original));
  }

  template <class ANY_TYPE>
  static TARGET_TYPE construct (bslmf::MovableRef<ANY_TYPE> original,
                                bslma::Allocator  *allocator)
  {
    return TARGET_TYPE(bsl::allocator_arg, allocator, bslmf::MovableRefUtil::move(original));
  }
};

template <class TARGET_TYPE>
struct AllocatorUtil<TARGET_TYPE, false, false>{

  static TARGET_TYPE construct(bslma::Allocator*)
  {
    return TARGET_TYPE();
  }
  static TARGET_TYPE construct (const TARGET_TYPE&                                 original,
                  bslma::Allocator*)
  {
    return TARGET_TYPE(original);
  }

  static TARGET_TYPE construct (bslmf::MovableRef<TARGET_TYPE> original,
                    bslma::Allocator*)
  {
    return TARGET_TYPE(bslmf::MovableRefUtil::move(original));
  }


  template <class ANY_TYPE>
  static TARGET_TYPE construct (BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
                  bslma::Allocator*)
  {
    return TARGET_TYPE(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE,original));
  }

  template <class ANY_TYPE>
  static TARGET_TYPE construct (bslmf::MovableRef<ANY_TYPE> original,
                                bslma::Allocator*)
  {
    return TARGET_TYPE(bslmf::MovableRefUtil::move(original));
  }

};
}  // close package namespace
}  // close enterprise namespace

#endif

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
