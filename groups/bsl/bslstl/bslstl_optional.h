// bslstl_optional.h                                                  -*-C++-*-

#ifndef INCLUDED_BSLSTL_OPTIONAL
#define INCLUDED_BSLSTL_OPTIONAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant allocator aware optional type.
//
//@CLASSES:
//  bsl::optional: template class for optional objects
//
//@DESCRIPTION: This component provides a template class,
// 'bsl::optional<TYPE>', that implements a notion of object that may or may
// not contain a 'TYPE' value.  This template class also provides an interface
// to check if the optional object contains a value or not, as well as a
// contextual conversion to 'bool'.  If an optional object is engaged, i.e.
// contains a value, it will evaluate to 'true' when converted to 'bool';
// otherwise, it will evaluate to 'false' when converted to 'bool'.
//
// An optional object is engaged if it has been initialized with, or assigned
// from an object of 'TYPE' or another engaged optional object, or if the value
// was created using the 'emplace' method.  Other types of assignment and
// initialization (including default initialization), as well as calling
// 'reset' method will result in the optional object being disengaged.
//
// If the underlying 'TYPE' has value-semantics, then so will the type
// 'bsl::optional<TYPE>'.  Two homogeneous optional objects have the same value
// if their underlying (non-null) 'TYPE' values are the same, or both are null.
//
// Note that the object of template parameter 'TYPE' that is managed by a
// 'bsl:optional<TYPE>' object is created *in*-*place*.  Consequently, the
// template parameter 'TYPE' must be a complete type when the class is
// instantiated.
//
// In addition to the standard homogeneous, value-semantic, operations such as
// copy/move construction, copy/move assignment, equality comparison, and
// relational operators, 'bsl::optional' also supports conversion between
// optional types for which the underlying types are convertible, i.e., for
// heterogeneous copy construction, copy assignment, and equality comparison
// (e.g., between 'int' and 'double'); attempts at conversion between
// incompatible types, such as 'int' and 'bsl::string', will fail to compile.
//
// For allocator-aware types, bsl::optional uses the same allocator for all
// 'value_type' objects it manages during its lifetime.
//
///Usage
///-----
// The following snippets of code illustrate use of this component:
//
// First, create a nullable 'int' object:
//..
//  bsl::optional<int> optionalInt;
//  assert(!optionalInt.has_value());
//..
// Next, give the 'int' object the value 123 (making it non-null):
//..
//  optionalInt.emplace(123);
//  assert( optionalInt.has_value());
//  assert(123 == optionalInt.value());
//..
// Finally, reset the object to its default constructed state (i.e., null):
//..
//  optionalInt.reset();
//  assert(!optionalInt.has_value());
//..
//
///Known limitations
///-----------------
//: o For assignment/construction constraints, we use 'is_constructible' but
//:   the exact creation will be done using allocation construction that will
//:   invoke an allocator-extended constructor for allocator-aware types.
//:   If the 'value_type' is constructible from the assignment/constructor
//:   argument, but doesn't have a corresponding allocator-extended
//:   constructor, the overload selection may not be be correct.
//:
//: o 'optional<const TYPE>' is fully supported in C++11 and onwards. However,
//:   due to limitations of 'MovableRef<const TYPE>', C++03 support for const
//:   'value_type's is limited and move semantics of such an 'optional' in
//:   C++03 will not work.

#include <bslscm_version.h>

#include <bslstl_badoptionalaccess.h>
#include <bslalg_swaputil.h>

#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_decay.h>
#include <bslmf_if.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_removeconst.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>

#include <stddef.h>
#include "bslstl_inplace.h"

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <type_traits>
#endif
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
//In C++17, bsl::optional for non-aa types inherits from std::optional
#include <optional>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif

namespace bsl {

#ifdef __cpp_lib_optional

using nullopt_t = std::nullopt_t;
using std::nullopt;

#else

                              // ===============
                              // class nullopt_t
                              // ===============
struct nullopt_t {
    // This trivial tag type is used to create 'optional' objects in a
    // disengaged state.  It is not default constructible so the following
    // assignment is unambiguous:
    //..
    //   optional<SomeType> o;
    //   o = {};
    //..
    // where 'o' is an 'optional' object.

    // CREATORS
    explicit BSLS_KEYWORD_CONSTEXPR nullopt_t(int) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'nullopt_t' object.  Note that the argument is not used.
};

// CREATORS

// bde_verify requires an out-of-class definition.  It also requires the tag
// variable below not to be extern.  The variable itself requires a constructor
// definition before its own definition.
inline
BSLS_KEYWORD_CONSTEXPR nullopt_t::nullopt_t(int) BSLS_KEYWORD_NOEXCEPT
{
}

static const BSLS_KEYWORD_CONSTEXPR nullopt_t nullopt = nullopt_t(0);
// Value of type 'nullopt_t' used as an argument to functions that take a
// 'nullopt_t' argument.

#endif  // __cpp_lib_optional

template <class TYPE,
          bool USES_BSLMA_ALLOC =
              BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class optional;

}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

                        // ============================
                        // class Optional_OptNoSuchType
                        // ============================

struct Optional_OptNoSuchType {
    // This trivial tag type is used to distinguish between arguments passed by
    // a user, and an 'enable_if' argument.  It is not default constructible so
    // the following construction never invokes a constrained single parameter
    // constructor:
    //..
    //   optional<SomeType> o(int, {});
    //..

    // CREATORS
    explicit BSLS_KEYWORD_CONSTEXPR Optional_OptNoSuchType(
                                                    int) BSLS_KEYWORD_NOEXCEPT;
        // Create an 'Optional_OptNoSuchType' object.  Note that the argument is
        // not used.
};

// CREATORS

// bde_verify requires an out-of-class definition.  It also requires the tag
// variable below not to be extern.  The variable itself requires a constructor
// definition before its own definition.
inline
BSLS_KEYWORD_CONSTEXPR Optional_OptNoSuchType::Optional_OptNoSuchType(
                                                     int) BSLS_KEYWORD_NOEXCEPT
{
}

static const BSLS_KEYWORD_CONSTEXPR Optional_OptNoSuchType optNoSuchType =
    Optional_OptNoSuchType(0);
// Value of type 'Optional_OptNoSuchType' used as the default argument in
// functions that are constrained using a function argument.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#define BSLSTL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE                             \
    std::is_trivially_destructible

#else
#define BSLSTL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE bsl::is_trivially_copyable
// C++03 does not provide a trivially destructible state.  Instead we use
// 'bsl::is_trivially_copyable' which implies the type is also trivially
// destructible.
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

                           // ======================
                           // class Optional_DataImp
                           // ======================

template <class TYPE>
struct Optional_DataImp {
    // This component-private 'struct' manages a 'value_type' object in
    // 'optional'.  This class provides an
    // abstraction for 'const' value type.  An 'optional' may contain a 'const'
    // type object.  An assignment to such an 'optional' should not succeed.
    // However, unless the 'optional' itself is 'const', it should be possible
    // to change the value of the 'optional' using 'emplace'.  In order to
    // allow for that, this class manages a non-const object of 'value_type',
    // but all the accessors return a 'const' adjusted reference to the managed
    // object.  This functionality is common for all value types and is
    // implemented in the 'Optional_DataImp' base class.  The derived class,
    // 'Optional_Data', is specialised on 'value_type'
    // 'is_trivially_destructible' trait.  The main template is for types that
    // are not trivially destructible, and it provides a destructor that
    // ensures the 'value_type' destructor is called if 'd_buffer' holds an
    // object.  The specialisation for 'is_trivially_destructible' types does
    // not have a user-provided destructor and 'is_trivially_destructible'
    // itself.

  private:
    // PRIVATE TYPES
    typedef typename bsl::remove_const<TYPE>::type StoredType;

    // DATA
    bsls::ObjectBuffer<StoredType> d_buffer;
        // in-place 'TYPE' object
        
    bool                           d_hasValue;  
        // 'true' if object has a value, and 'false' otherwise
  public:
    // CREATORS
    Optional_DataImp() BSLS_KEYWORD_NOEXCEPT;
        // Create an empty 'Optional_DataImp' object.

    // MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create an object of 'StoredType' in 'd_buffer' using the specified
        // 'allocator' and arguments.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
        // Create an object of 'StoredType' in 'd_buffer' using the specified
        // 'allocator', 'initializer_list', and arguments.

#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_optional.h
#ifndef BSLSTL_OPTIONAL_VARIADIC_LIMIT
#define BSLSTL_OPTIONAL_VARIADIC_LIMIT 10
#endif
#ifndef BSLSTL_OPTIONAL_VARIADIC_LIMIT_A
#define BSLSTL_OPTIONAL_VARIADIC_LIMIT_A BSLSTL_OPTIONAL_VARIADIC_LIMIT
#endif
#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 0
    void emplace(bslma::Allocator                           *allocator);
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 0

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 1
    template <class ARGS_01>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 1

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 2
    template <class ARGS_01,
              class ARGS_02>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 2

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 3
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 3

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 4
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 4

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 5
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 5

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 6
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 6

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 7
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 7

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 8
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 8

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 9
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 9

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 10
    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09,
              class ARGS_10>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 10


#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 0
    template <class INIT_LIST_TYPE>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list);
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 0

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 1
    template <class INIT_LIST_TYPE, class ARGS_01>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 1

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 2
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 2

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 3
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 3

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 4
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03,
                                    class ARGS_04>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 4

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 5
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03,
                                    class ARGS_04,
                                    class ARGS_05>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 5

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 6
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03,
                                    class ARGS_04,
                                    class ARGS_05,
                                    class ARGS_06>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 6

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 7
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03,
                                    class ARGS_04,
                                    class ARGS_05,
                                    class ARGS_06,
                                    class ARGS_07>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 7

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 8
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03,
                                    class ARGS_04,
                                    class ARGS_05,
                                    class ARGS_06,
                                    class ARGS_07,
                                    class ARGS_08>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 8

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 9
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03,
                                    class ARGS_04,
                                    class ARGS_05,
                                    class ARGS_06,
                                    class ARGS_07,
                                    class ARGS_08,
                                    class ARGS_09>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 9

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 10
    template <class INIT_LIST_TYPE, class ARGS_01,
                                    class ARGS_02,
                                    class ARGS_03,
                                    class ARGS_04,
                                    class ARGS_05,
                                    class ARGS_06,
                                    class ARGS_07,
                                    class ARGS_08,
                                    class ARGS_09,
                                    class ARGS_10>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09),
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10));
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_A >= 10


#endif
#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    void emplace(bslma::Allocator                           *allocator,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template <class INIT_LIST_TYPE, class... ARGS>
    void emplace(bslma::Allocator                           *allocator,
                 std::initializer_list<INIT_LIST_TYPE>       initializer_list,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);

#endif
// }}} END GENERATED CODE
#endif

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Destroy the 'value_type' object in 'd_buffer', if any.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE& value() &;
    TYPE&& value() &&;
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'
#else
    TYPE& value();
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'
#endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS

    //ACCESSORS
    bool hasValue() const BSLS_KEYWORD_NOEXCEPT;
        // return 'true' if there is a value in 'd_buffer', and 'false'
        //otherwise.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    const TYPE& value() const &;
    const TYPE&& value() const &&;
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'

#else
    const TYPE& value() const;
        // Return the 'value_type' object in 'd_buffer' with const
        // qualification adjusted to match that of 'TYPE'.  The behavior is
        // undefined unless 'this->hasValue() == true'
#endif  // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
};

                            // ===================
                            // class Optional_Data
                            // ===================

template <class TYPE,
          bool IS_TRIVIALLY_DESTRUCTIBLE =
              BSLSTL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE<TYPE>::value>
struct Optional_Data : public Optional_DataImp<TYPE> {
    // This component-private 'struct' manages a 'value_type' object in
    // 'optional' by inheriting from `Optional_DataImp`.  In addition, this primary template properly
    // destroys the owned instance of 'TYPE' in its destructor.

  public:
    // CREATORS
    ~Optional_Data();
        // Destroy the managed 'value_type' object, if it exists.
};

                            // ===================
                            // class Optional_Data
                            // ===================

template <class TYPE>
struct Optional_Data<TYPE, true> : public Optional_DataImp<TYPE> {
    // This partial specialization manages a trivially destructible 'value_type' in optional. 
    // It does not have a user-provided destructor, which makes it
    // 'is_trivially_destructible' itself.

  public:
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // Workaround for C++03 'bsl::is_trivially_copyable' trait.  Note that,
    // whether 'Optional_Data<TYPE>' satisfies 'bsl::is_trivally_copyable'
    // doesn't affect 'Optional<TYPE>' 'bsl::is_trivally_copyable' trait.  We
    // only add this nested trait for the tests to be able to check the C++03
    // implementation of 'Optional_Data'.  For correct C++03 functionality,
    // 'bsl::optional' has to add a nested trait as well.
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Optional_Data,
                                      bsl::is_trivially_copyable,
                                      bsl::is_trivially_copyable<TYPE>::value);
#endif  //BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                           // ======================
                           // class Optional_DataImp
                           // ======================

// CREATORS
template <class TYPE>
Optional_DataImp<TYPE>::Optional_DataImp() BSLS_KEYWORD_NOEXCEPT
: d_hasValue(false)
{
}

// MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class TYPE>
template <class... ARGS>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE>
template <class INIT_LIST_TYPE, class... ARGS>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}
#endif  //BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_optional.h
#ifndef BSLSTL_OPTIONAL_VARIADIC_LIMIT
#define BSLSTL_OPTIONAL_VARIADIC_LIMIT 10
#endif
#ifndef BSLSTL_OPTIONAL_VARIADIC_LIMIT_B
#define BSLSTL_OPTIONAL_VARIADIC_LIMIT_B BSLSTL_OPTIONAL_VARIADIC_LIMIT
#endif
#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 0
template <class TYPE>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator);
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 0

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 1
template <class TYPE>
template <class ARGS_01>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 1

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 2
template <class TYPE>
template <class ARGS_01,
          class ARGS_02>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 2

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 3
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 3

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 4
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 4

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 5
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 5

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 6
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 6

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 7
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 7

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 8
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_08, args_08));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 8

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 9
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_08, args_08),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_09, args_09));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 9

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 10
template <class TYPE>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09,
          class ARGS_10>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_08, args_08),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_09, args_09),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_10, args_10));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 10


#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 0
template <class TYPE>
template <class INIT_LIST_TYPE>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il);
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 0

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 1
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 1

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 2
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 2

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 3
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 3

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 4
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03,
                                class ARGS_04>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 4

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 5
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03,
                                class ARGS_04,
                                class ARGS_05>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 5

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 6
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03,
                                class ARGS_04,
                                class ARGS_05,
                                class ARGS_06>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 6

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 7
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03,
                                class ARGS_04,
                                class ARGS_05,
                                class ARGS_06,
                                class ARGS_07>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 7

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 8
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03,
                                class ARGS_04,
                                class ARGS_05,
                                class ARGS_06,
                                class ARGS_07,
                                class ARGS_08>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_08, args_08));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 8

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 9
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03,
                                class ARGS_04,
                                class ARGS_05,
                                class ARGS_06,
                                class ARGS_07,
                                class ARGS_08,
                                class ARGS_09>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_08, args_08),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_09, args_09));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 9

#if BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 10
template <class TYPE>
template <class INIT_LIST_TYPE, class ARGS_01,
                                class ARGS_02,
                                class ARGS_03,
                                class ARGS_04,
                                class ARGS_05,
                                class ARGS_06,
                                class ARGS_07,
                                class ARGS_08,
                                class ARGS_09,
                                class ARGS_10>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_01, args_01),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_02, args_02),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_03, args_03),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_04, args_04),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_05, args_05),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_06, args_06),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_07, args_07),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_08, args_08),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_09, args_09),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_10, args_10));
    d_hasValue = true;
}
#endif  // BSLSTL_OPTIONAL_VARIADIC_LIMIT_B >= 10

#endif
#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TYPE>
template <class... ARGS>
inline
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE>
template <class INIT_LIST_TYPE, class... ARGS>
void Optional_DataImp<TYPE>::emplace(
                         bslma::Allocator                           *allocator,
                         std::initializer_list<INIT_LIST_TYPE>       il,
                         BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...  args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        allocator,
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}
#endif
// }}} END GENERATED CODE
#endif

template <class TYPE>
void Optional_DataImp<TYPE>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue) {
        bslma::DestructionUtil::destroy(&(d_buffer.object()));
    }
    d_hasValue = false;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE>
inline
TYPE& Optional_DataImp<TYPE>::value() &
{
    BSLS_ASSERT(d_hasValue);

    return d_buffer.object();
}

template <class TYPE>
inline
TYPE&& Optional_DataImp<TYPE>::value() &&
{
    BSLS_ASSERT(d_hasValue);

    return std::move(d_buffer.object());
}
#else
template <class TYPE>
inline
TYPE& Optional_DataImp<TYPE>::value()
{
    BSLS_ASSERT(d_hasValue);

    return d_buffer.object();
}
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

//ACCESSORS
template <class TYPE>
inline
bool Optional_DataImp<TYPE>::hasValue() const BSLS_KEYWORD_NOEXCEPT
{
    return d_hasValue;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE>
inline
const TYPE& Optional_DataImp<TYPE>::value() const &
{
    BSLS_ASSERT(d_hasValue);

    return d_buffer.object();
}

template <class TYPE>
inline
const TYPE&& Optional_DataImp<TYPE>::value() const &&
{
    BSLS_ASSERT(d_hasValue);

    return std::move(d_buffer.object());
}
#else
template <class TYPE>
inline
const TYPE& Optional_DataImp<TYPE>::value() const
{
    BSLS_ASSERT(d_hasValue);

    return d_buffer.object();
}
#endif  //defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

                            // ===================
                            // class Optional_Data
                            // ===================

// CREATORS
template <class TYPE, bool IS_TRIVIALLY_DESTRUCTIBLE>
Optional_Data<TYPE, IS_TRIVIALLY_DESTRUCTIBLE>::~Optional_Data()
{
    this->reset();
}

}  // close package namespace
}  // close enterprise namespace

#undef BSLS_KEYWORD_LVREF_QUAL
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
#undef BSLS_KEYWORD_RVREF_QUAL
#endif

#undef BSLSTL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE

#endif  // INCLUDED_BSLSTL_OPTIONAL
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
