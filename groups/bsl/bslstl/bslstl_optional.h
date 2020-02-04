// bslstl_optional.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLSTL_OPTIONAL
#define INCLUDED_BSLSTL_OPTIONAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")
// questions:
//     - how to include traits header bsl_type_traits.h ?
//todo : add constructor constraints
//todo : add swap
//todo : add make optional
//todo : add alloc_optional
//todo : fix documentation
//todo : sprinkle constexpr
//todo : replace traits with bsl traits
// todo (decisions)
//  * what allocator to use in value_or(T&&) &
//  * what allocator to use in value_or(const T&) &&
// C++03 notes :
//   is_constructible is needed for correct overload resolution
//   in operator= . Using is_convertible in C++03 may lead to
//   different overload resolution when the type is_constructible
//   but !is_convertible. Not having any constraints on
//   operator=(moveable_ref<other_type>), and separating T from U
///  overloads may be an option as
//   perfect forwarding best match issue is not problematic
//   in C++03. Will need investigating which assignment overloads
//   should be removed in c++03.
//
//   !(bsl::is_same<ANY_TYPE, std::decay<TYPE>::type>
//   && bsl::is_scalar<TYPE>) constraints on operator=(U&&) are
//   needed so cases like bsl::optonal<int>={} create a disengaged
//   optional.
//
//known limitations :
//  - BloombergLP::bslalg::ScalarPrimitives::construct does not do
//    argument forwarding, so move semantics are limited for
//    cases where new value_type object is created
//  - MoveableRef can not be used where the argument is a deduced type.
//    This means that rvalue overloads taking optional<OTHER_TYPE>
//    must use optional<OTHER_TYPE> &&
//  - For assignment/construction constraints, we use is_constructible
//    but the exact creation will be done using allocation construction
//    which will invoke an allocator extended constructor for allocator
//    aware types. If the value type is constructible from the
//    assignment/constructor parameter, but doesn't have a corresponding
//    allocator extended constructor, the overload selection may not be
//    be correct.
//
//
//
//@PURPOSE: Provide a template for nullable (in-place) objects.
//
//@CLASSES:
//  bdlb::optional: template for nullable (in-place) objects
//
//@DESCRIPTION: This component provides a template class,
// 'bslstl::optional<TYPE>', that can be used to augment an arbitrary
// value-semantic 'TYPE', such as 'int' or 'bsl::string', so that it also
// supports the notion of a "null" value.  That is, the set of values
// representable by the template parameter 'TYPE' is extended to include null.
// If the underlying 'TYPE' is fully value-semantic, then so will the augmented
// type 'bslstl::optional<TYPE>'.  Two homogeneous nullable objects have the
// same value if their underlying (non-null) 'TYPE' values are the same, or
// both are null.
//
// Note that the object of template parameter 'TYPE' that is managed by a
// 'bslstl::optional<TYPE>' object is created *in*-*place*.  Consequently,
// the template parameter 'TYPE' must be a complete type when the class is
// instantiated.  In contrast, 'bslstl::NullableAllocatedValue<TYPE>' (see
// 'bslstl_nullableallocatedvalue') does not require that 'TYPE' be complete when
// that class is instantiated, with the trade-off that the managed 'TYPE'
// object is always allocated out-of-place in that case.
//
// In addition to the standard homogeneous, value-semantic, operations such as
// copy construction, copy assignment, equality comparison, and BDEX streaming,
// 'bslstl::optional' also supports conversion between augmented types for
// which the underlying types are convertible, i.e., for heterogeneous copy
// construction, copy assignment, and equality comparison (e.g., between 'int'
// and 'double'); attempts at conversion between incompatible types, such as
// 'int' and 'bsl::string', will fail to compile.  Note that these operational
// semantics are similar to those found in 'bsl::shared_ptr'.
//
// Furthermore, a move constructor (taking an optional allocator) and a
// move-assignment operator are also provided.  Note that move semantics are
// emulated with C++03 compilers.
//
///Usage
///-----
// The following snippets of code illustrate use of this component:
//
// First, create a nullable 'int' object:
//..
//  bslstl::optional<int> nullableInt;
//  assert( !nullableInt.has_value());
//..
// Next, give the 'int' object the value 123 (making it non-null):
//..
//  nullableInt.makeValue(123);
//  assert(nullableInt.has_value());
//  assert(123 == nullableInt.value());
//..
// Finally, reset the object to its default constructed state (i.e., null):
//..
//  nullableInt.reset();
//  assert( !nullableInt.has_value());
//..

#include <bslalg_constructusingallocator.h>
#include <bslalg_scalarprimitives.h>
#include <bslalg_swaputil.h>

#include <bslma_stdallocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_if.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_removeconst.h>


#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <bslscm_version.h>

#include <bslstl_badoptionalaccess.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
//#include <bsl_type_traits.h>
#include <type_traits>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif

//todo move these macros somewhere more appropriate
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
#define BSLS_KEYWORD_LVREF_QUAL &
#define BSLS_KEYWORD_RVREF_QUAL &&
#else
#define BSLS_KEYWORD_LVREF_QUAL
//#define BSLS_KEYWORD_RVREF_QUAL //deliberately not defined as no C++03 functions should have
// this qualifier in C++11
#endif



namespace bsl {

#ifdef __cpp_lib_optional
using nullopt_t = std::nullopt_t;
using std::nullopt;

// todo : find a better place for in_place_t
// in_place_t doesn't have it's own feature test macro, but it will be
// available with __cpp_lib_optional as std::optional uses it.
using inplace_t = std::in_place_t;
using std::in_place
#else
                        // =========================
                        // class nullopt_t
                        // =========================
struct nullopt_t {
    // nullopt_t is a tag type used to create optional objects in a disengaged
    // state. It should not be default constructible so the following
    // assignment isn't ambiguous :
    // optional<SomeType> o;
    // o = {};
    // where o is an optional object.
    explicit nullopt_t(int) { };
};
extern nullopt_t nullopt;

                        // =========================
                        // class in_place_t
                        // =========================
struct in_place_t {
  // in_place_t is a tag type used to tags that can be passed to the
  // constructors of optional to indicate that the contained object should be
  // constructed in-place.
  explicit in_place_t(){};
};
extern in_place_t in_place;


#endif //__cpp_lib_optional

template <class TYPE, bool UsesBslmaAllocator = BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class optional;

// Type traits to assist in choosing the correct assignment
// and construction overload. If the value_type converts
// or assigns from an optional<other_type>, then the overload
// passing the optional parameter to the value type is preferred.
// As in std implementation, if the value type converts or assigns
// from any value category, we consider it convertible/assignable
// from optional.

template <typename TYPE, typename ANY_TYPE>
struct bsl_converts_from_optional
: integral_constant< bool,
          std::is_constructible<TYPE, const optional<ANY_TYPE>&>::value
          ||
          std::is_constructible<TYPE, optional<ANY_TYPE>&>::value
          ||
          std::is_constructible<TYPE, const optional<ANY_TYPE>&&>::value
          ||
          std::is_constructible<TYPE, optional<ANY_TYPE>&&>::value
          ||
          std::is_convertible<const optional<ANY_TYPE>&, TYPE>::value
          ||
          std::is_convertible<optional<ANY_TYPE>&, TYPE>::value
          ||
          std::is_convertible<const optional<ANY_TYPE>&&, TYPE>::value
          ||
          std::is_convertible<optional<ANY_TYPE>&&, TYPE>::value>{
};


template <typename TYPE, typename ANY_TYPE>
struct bsl_assigns_from_optional
: integral_constant< bool,
            std::is_assignable<TYPE&, const optional<ANY_TYPE>&>::value
            ||
            std::is_assignable<TYPE&, optional<ANY_TYPE>&>::value
            ||
            std::is_assignable<TYPE&, const optional<ANY_TYPE>&&>::value
            ||
            std::is_assignable<TYPE&, optional<ANY_TYPE>&&>::value> {

};

                        // =========================
                        // class optional_data_imp
                        // =========================
template <class TYPE, bool UsesBslmaAllocator = BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
struct optional_data_imp {
    // optional_data_imp is a type used to manage const correctness of the
    // value_type within optional. It is not intended to be used outside of
    // optional implementation

  private:
    typedef typename bsl::remove_const<TYPE>::type stored_type;

    BloombergLP::bsls::ObjectBuffer<stored_type>  d_buffer;
                                     // in-place 'TYPE' object
    bool           d_hasValue;       // 'true' if object has value, Otherwise
                                     // 'false'
  public:
    typedef typename bsl::allocator<char> allocator_type;

    BSLMF_NESTED_TRAIT_DECLARATION_IF(optional_data_imp,
                                      BloombergLP::bslma::UsesBslmaAllocator,
                                      BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(optional_data_imp,
                                      BloombergLP::bslmf::UsesAllocatorArgT,
                                      BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value);
  //CREATORS
    optional_data_imp() BSLS_KEYWORD_NOEXCEPT;
    ~optional_data_imp();
  //MANIPULATORS
    void emplace(bsl::allocator_arg_t, allocator_type basicAllocator);
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... ARGS>
    void emplace(bsl::allocator_arg_t, allocator_type,
                BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... ARGS>
    void emplace(bsl::allocator_arg_t, allocator_type , std::initializer_list<U>,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    void reset() BSLS_KEYWORD_NOEXCEPT;

  //ACCESSORS
    TYPE& value() BSLS_KEYWORD_LVREF_QUAL;
    const TYPE& value() const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& value() BSLS_KEYWORD_RVREF_QUAL;
    const TYPE&& value() const BSLS_KEYWORD_RVREF_QUAL;
#endif // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;

};

template <class TYPE>
struct optional_data_imp<TYPE, false> {
    // optional_data_imp is a type used to manage const correctness of the
    // value_type within optional. It is not intended to be used outside of
    // optional implementation

  private:
    typedef typename bsl::remove_const<TYPE>::type stored_type;
    typedef typename bsl::allocator<char> allocator_type;

    BloombergLP::bsls::ObjectBuffer<stored_type>  d_buffer;
                                     // in-place 'TYPE' object
    bool           d_hasValue;       // 'true' if object has value, Otherwise
                                     // 'false'
  public:

  //CREATORS
    optional_data_imp() BSLS_KEYWORD_NOEXCEPT;
    ~optional_data_imp();

   //MANIPULATORS
    void emplace();
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... ARGS>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... ARGS>
    void emplace(std::initializer_list<U>,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    void reset() BSLS_KEYWORD_NOEXCEPT;

  //ACCESSORS
    TYPE& value() BSLS_KEYWORD_LVREF_QUAL;
    const TYPE& value() const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& value() BSLS_KEYWORD_RVREF_QUAL;
    const TYPE&& value() const BSLS_KEYWORD_RVREF_QUAL;
#endif // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;


};

template <class TYPE, bool UsesBslmaAllocator>
optional_data_imp<TYPE,UsesBslmaAllocator>::optional_data_imp() BSLS_KEYWORD_NOEXCEPT
: d_hasValue(false)
{}


template <class TYPE, bool UsesBslmaAllocator>
void optional_data_imp<TYPE,UsesBslmaAllocator>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue)
    {
      d_buffer.object().~stored_type();
    }
    d_hasValue = false;
}
template <typename TYPE, bool UsesBslmaAllocator>
inline
void optional_data_imp<TYPE, UsesBslmaAllocator>::emplace(
    bsl::allocator_arg_t, allocator_type allocator)
{
    reset();
    BloombergLP::bslalg::ScalarPrimitives::construct(
        d_buffer.address(),
        allocator.mechanism());
    d_hasValue = true;
}
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename TYPE, bool UsesBslmaAllocator>
template <class... ARGS>
inline
void optional_data_imp<TYPE, UsesBslmaAllocator>::emplace(
    bsl::allocator_arg_t, allocator_type allocator,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    BloombergLP::bslalg::ScalarPrimitives::construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...,
        allocator.mechanism());
    d_hasValue = true;
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <typename TYPE, bool UsesBslmaAllocator>
template<class U, class... ARGS>
void optional_data_imp<TYPE, UsesBslmaAllocator>::emplace(
    bsl::allocator_arg_t,
    allocator_type allocator,
    std::initializer_list<U> il,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    BloombergLP::bslalg::ScalarPrimitives::construct(
        d_buffer.address(),
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...,
        allocator.mechanism());
    d_hasValue = true;
}
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_optional.h
#endif
template <class TYPE, bool UsesBslmaAllocator>
optional_data_imp<TYPE,UsesBslmaAllocator>::~optional_data_imp()
{
    this->reset();
}
//ACCESSORS
template <class TYPE, bool UsesBslmaAllocator>
bool optional_data_imp<TYPE,UsesBslmaAllocator>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue)
    {
      return true;
    }
    return false;
}
template <class TYPE, bool UsesBslmaAllocator>
TYPE& optional_data_imp<TYPE,UsesBslmaAllocator>::value()
BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
template <class TYPE, bool UsesBslmaAllocator>
const TYPE& optional_data_imp<TYPE,UsesBslmaAllocator>::value() const
BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE, bool UsesBslmaAllocator>
TYPE&& optional_data_imp<TYPE,UsesBslmaAllocator>::value()
BSLS_KEYWORD_RVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return std::move(d_buffer.object());
}
template <class TYPE, bool UsesBslmaAllocator>
const TYPE&& optional_data_imp<TYPE,UsesBslmaAllocator>::value() const
BSLS_KEYWORD_RVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return std::move(d_buffer.object());
}
#endif
template <class TYPE>
optional_data_imp<TYPE,false>::optional_data_imp() BSLS_KEYWORD_NOEXCEPT
: d_hasValue(false)
{}
template <class TYPE>
void optional_data_imp<TYPE,false>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue)
    {
      d_buffer.object().~stored_type();
    }
    d_hasValue = false;
}
template <typename TYPE>
inline
void optional_data_imp<TYPE, false>::emplace()
{
    reset();
    BloombergLP::bslalg::ScalarPrimitives::construct(
        d_buffer.address(),
        (void *) 0);
    d_hasValue = true;
}
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename TYPE>
template <class... ARGS>
inline
void optional_data_imp<TYPE, false>::emplace(
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    BloombergLP::bslalg::ScalarPrimitives::construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...,
        (void *) 0);
    d_hasValue = true;
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <typename TYPE>
template<class U, class... ARGS>
void optional_data_imp<TYPE, false>::emplace(
    std::initializer_list<U> il,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    BloombergLP::bslalg::ScalarPrimitives::construct(
        d_buffer.address(),
        il,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...,
        (void *) 0);
    d_hasValue = true;
}
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_optional.h
#endif
template <class TYPE>
optional_data_imp<TYPE,false>::~optional_data_imp()
{
    this->reset();
}
//ACCESSORS
template <class TYPE>
bool optional_data_imp<TYPE,false>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue)
    {
      return true;
    }
    return false;
}
template <class TYPE>
TYPE& optional_data_imp<TYPE, false>::value() BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
template <class TYPE>
const TYPE& optional_data_imp<TYPE, false>::value() const BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE>
TYPE&& optional_data_imp<TYPE, false>::value() BSLS_KEYWORD_RVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return std::move(d_buffer.object());
}
template <class TYPE>
const TYPE&& optional_data_imp<TYPE, false>::value() const  BSLS_KEYWORD_RVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return std::move(d_buffer.object());
}
#endif


//note : banner indented by 25 spaces. Check if correct and remove note.

                        // =========================
                        // class optional<TYPE>
                        // =========================
//note : The body of a class definition is divided into public and
//private sections using the public: and private: labels. These labels must appear on
//lines by themselves, indented exactly two spaces, and preceded by a blank line
template <class TYPE, bool UsesBslmaAllocator>
class optional {
  public :
    // PUBLIC TYPES
    typedef TYPE value_type;
        // 'value_type' is an alias for the underlying 'TYPE' upon which this
        // template class is instantiated, and represents the type of the
        // managed object. The name is chosen so it is compatible with the
        // std::optional implementation.

    typedef typename bsl::allocator<char> allocator_type;

  private:
    // note : Each category must be preceded by a tag comment that identifies the category.
    // The tag must appear immediately before the first declaration in that category, with
    // no intervening blank line. A tag must not be used if the category is Otherwise empty
	// PRIVATE TYPES

	typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

	// DATA
	optional_data_imp<TYPE>  d_value;       // in-place 'TYPE' object
    allocator_type d_allocator;


  public:
	// todo : check if any rhs nested traits can be added
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(optional, BloombergLP::bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(optional, BloombergLP::bslmf::UsesAllocatorArgT);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(optional,
        BloombergLP::bslmf::IsBitwiseMoveable,
        BloombergLP::bslmf::IsBitwiseMoveable<TYPE>::value);

    // CREATORS
    optional();
        // Create a disengaged optional object having the null value. Use
        // the currently installed default allocator to supply memory for
        // future value objects.

    optional(nullopt_t );
        // Create a disengaged optional object having the null value. Use
        // the currently installed default allocator to supply memory for
        // future value objects.

    optional(const optional& original);
        // If original contains a value, initialize the contained value using
        // *original. Otherwise, create a disengaged optional. Use the currently
        // installed default allocator to supply memory for this and any rhs
        // future value objects.

    optional(BloombergLP::bslmf::MovableRef<optional> original);
        // If original contains a value, initialize the contained value by moving
        // from *original. Otherwise, create a disengaged optional. Use the allocator
        // from original to supply memory to supply memory for this and any rhs
        // future value objects.  'original' is left in a valid, but unspecified state.

    template<class ANY_TYPE = TYPE,
        typename bsl::enable_if<!bsl::is_same<ANY_TYPE, optional<TYPE>>::value
                                &&
                                !bsl::is_same<ANY_TYPE, nullopt_t>::value
                                &&
                                std::is_constructible<TYPE, ANY_TYPE>::value
                                &&
                                bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                bool>::type = true>
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, original));
    }
    template<class ANY_TYPE = TYPE,
            typename bsl::enable_if<!bsl::is_same<ANY_TYPE, optional<TYPE>>::value
                                    &&
                                    !bsl::is_same<ANY_TYPE, nullopt_t>::value
                                    &&
                                    std::is_constructible<TYPE, ANY_TYPE>::value
                                    &&
                                    !bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                    bool>::type = false>
    explicit
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, original));
    }
        // Create an optional object having the value of the specified
        // 'original' object.  Use the currently installed default
        // allocator to supply memory for future value objects.

    template<class ANY_TYPE,
              typename bsl::enable_if<
                      !bsl::is_same<ANY_TYPE, TYPE>::value
                      &&
                      std::is_constructible<TYPE,const ANY_TYPE &>::value
                      &&
                      bsl::is_convertible<const ANY_TYPE &, TYPE>::value
                      &&
                      !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
                      bool>::type = true>
    optional(const optional<ANY_TYPE>& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(original.value());
        }
    }
    template<class ANY_TYPE,
             typename bsl::enable_if<
                      !bsl::is_same<ANY_TYPE, TYPE>::value
                      &&
                      std::is_constructible<TYPE, const ANY_TYPE &>::value
                      &&
                      !bsl::is_convertible<const ANY_TYPE &, TYPE>::value
                      &&
                      !bsl_converts_from_optional<TYPE,ANY_TYPE>::value ,
                      bool>::type = false>
    explicit
    optional(const optional<ANY_TYPE>& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(original.value());
        }
    }
      // If original contains a value, initialize the contained value using
      // *original. Otherwise, create a disengaged optional.

    template<class ANY_TYPE,
              typename bsl::enable_if<
                      !bsl::is_same<ANY_TYPE, TYPE>::value
                      &&
                      std::is_constructible<TYPE, ANY_TYPE >::value
                      &&
                      bsl::is_convertible<ANY_TYPE , TYPE>::value
                      &&
                      !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
                      bool>::type = true>
    optional(optional<ANY_TYPE>&& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(MoveUtil::move(original.value()));
        }
    }
    template<class ANY_TYPE,
              typename bsl::enable_if<
                    !bsl::is_same<ANY_TYPE, TYPE>::value
                    &&
                    std::is_constructible<TYPE, ANY_TYPE >::value
                    &&
                    !bsl::is_convertible<ANY_TYPE , TYPE>::value
                    &&
                    !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
                    bool>::type = false>
    explicit
    optional(optional<ANY_TYPE>&& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(MoveUtil::move(original.value()));
        }
    }
      // If original contains a value, initialize the contained value by moving
      // from *original. Otherwise, create a disengaged optional.
      // If original contains a value, initialize the contained value by moving
      // from *original. Otherwise, create a disengaged optional. Use the currently
      // installed default allocator to supply memory for this and any rhs
      // future value objects.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... ARGS>
    explicit optional(in_place_t,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... ARGS>
    explicit optional(in_place_t, std::initializer_list<U>,
        BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

    optional(bsl::allocator_arg_t, allocator_type basicAllocator);
        // Create a disengaged optional object. Use the specified
        // 'basicAllocator' to supply memory for future objects.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
                 nullopt_t);
        // Create a disengaged optional object. Use the specified
        // 'basicAllocator' to supply memory for future objects.


    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
                 const optional&);
        // If original contains a value, initialize the contained value
        // with *original. Otherwise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        BloombergLP::bslmf::MovableRef<optional> original);
        // If original contains a value, initialize the contained value by
        // moving from *original. Otherwise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

    template<class ANY_TYPE = TYPE,
             typename bsl::enable_if<
                       !bsl::is_same<ANY_TYPE, optional<TYPE>>::value
                        &&
                        !bsl::is_same<ANY_TYPE, nullopt_t>::value
                        &&
                        std::is_constructible<TYPE, ANY_TYPE>::value,
                        bool>::type = false>
    explicit
    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) rhs)
    : d_allocator(basicAllocator)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, rhs));
    }
        // Create a optional object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  Use the specified 'basicAllocator' to supply
        // memory.  'original' is left in a valid but unspecified state.

    template<class ANY_TYPE,
    typename bsl::enable_if<
             !bsl::is_same<ANY_TYPE, TYPE>::value
             &&
             std::is_constructible<TYPE, const ANY_TYPE &>::value
             &&
             !bsl_converts_from_optional<TYPE,ANY_TYPE>::value ,
             bool>::type = false>
    explicit
    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        const optional<ANY_TYPE>& rhs)
    : d_allocator(basicAllocator)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (rhs.has_value()) {
          this->emplace(rhs.value());
        }
    }
        // If original contains a value, initialize the contained value
        // with *original. Otherwise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

    template<class ANY_TYPE,
    typename bsl::enable_if<
             !bsl::is_same<ANY_TYPE, TYPE>::value
             &&
             std::is_constructible<TYPE,ANY_TYPE >::value
             &&
             !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
             bool>::type = false>
    explicit
    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        optional<ANY_TYPE>&& rhs)
    : d_allocator(basicAllocator)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        optional<ANY_TYPE>& lvalue = rhs;

        if (lvalue.has_value()) {
           emplace(MoveUtil::move(lvalue.value()));
        }
    }
        // If original contains a value, initialize the contained value by
        // moving from *original. Otherwise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... ARGS>
    explicit optional(bsl::allocator_arg_t, allocator_type,
                      in_place_t,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... ARGS>
    explicit optional(bsl::allocator_arg_t, allocator_type,
                      in_place_t, std::initializer_list<U>,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

    ~optional();
    // Destroy this object.

    //MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... ARGS>
    void emplace(ARGS&&...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... ARGS>
    void emplace(std::initializer_list<U>, ARGS&&...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif //!BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to the default constructed state (i.e., to have
        // the null value).
    //OBSERVERS

    TYPE& value() BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& value() BSLS_KEYWORD_RVREF_QUAL;
#endif
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object. Throws a bsl::bad_optional_access if the optional
        // object is disengaged.
    const TYPE& value() const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    const TYPE&& value() const BSLS_KEYWORD_RVREF_QUAL;
#endif
            // Return a reference providing non-modifiable access to the underlying
            // 'TYPE' object. Throws a bsl::bad_optional_access if the optional
            // object is disengaged.

    optional& operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to be disengaged and return a reference
        // providing modifiable access to this object.

    optional& operator=(const optional& rhs);
    optional& operator=(BloombergLP::bslmf::MovableRef<optional> rhs);
        // If rhs is engaged, assign its value to this object. Otherwise,
        // reset this object to a disengaged state. Return a reference
        // providing modifiable access to this object.  Note that this
        // method may invoke assignment from rhs, or construction from rhs,
        // depending on whether the lhs optional object is engaged.

    template<class ANY_TYPE>
    typename bsl::enable_if<
              std::is_constructible<TYPE,const ANY_TYPE &>::value
              &&
              std::is_assignable<TYPE&, ANY_TYPE>::value
              &&
              !bsl_converts_from_optional<TYPE,ANY_TYPE>::value
              &&
              !bsl_assigns_from_optional<TYPE,ANY_TYPE>::value,
              optional>::type &
              operator=(const optional<ANY_TYPE> &rhs)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (rhs.has_value())
          {
            if (this->has_value())
              this->value() = rhs.value();
            else
              this->emplace(rhs.value());
          }
          else
          {
            this->reset();
          }
          return *this;
    }

    template<class ANY_TYPE>
    typename bsl::enable_if<
              std::is_constructible<TYPE, ANY_TYPE>::value
              &&
              std::is_assignable<TYPE&, ANY_TYPE>::value
              &&
              !bsl_converts_from_optional<TYPE,ANY_TYPE>::value
              &&
              !bsl_assigns_from_optional<TYPE,ANY_TYPE>::value,
              optional>::type &
              operator=(optional<ANY_TYPE>&& rhs)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (rhs.has_value())
        {
          if (this->has_value())
            this->value() = MoveUtil::move(rhs.value());
          else
            this->emplace(MoveUtil::move(rhs.value()));
        }
        else
        {
          this->reset();
        }
        return *this;
    }


#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES )
    template<class ANY_TYPE = TYPE>
    typename bsl::enable_if<
              !bsl::is_same<ANY_TYPE, optional>::value
              &&
              !(bsl::is_same<ANY_TYPE,
                    typename std::decay<TYPE>::type >::value
                && std::is_scalar<TYPE>::value)
              &&
              std::is_constructible<TYPE, ANY_TYPE>::value
              &&
              std::is_assignable<TYPE&, ANY_TYPE>::value,
              optional>::type &
    operator=(ANY_TYPE&& rhs)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (this->has_value())
        {
            this->value() = std::forward<ANY_TYPE>(rhs);
        }
        else
        {
            this->emplace(std::forward<ANY_TYPE>(rhs));
        }
        return *this;
    }
        // Assign to this object the value of the specified 'rhs' object (of
        // 'BDE_rhs_TYPE') converted to 'TYPE', and return a reference
        // providing modifiable access to this object.  Note that this
        // method may invoke assignment from rhs, or construction from rhs,
        // depending on whether the lhs optional object is engaged.
        // Equality trait is needed to remove this function from the
        // overload set when assignment is done from the optional<TYPE>
        // object, as this is a better match for lvalues than the
        // operator=(const optional& rhs). Without rvalue references
        // and perfect forwarding, this is not the case.
#else
    template<class ANY_TYPE = TYPE>
    optional& operator=(const ANY_TYPE &rhs);

    template<class ANY_TYPE = TYPE>
    optional& operator=(ANY_TYPE&& rhs);
#endif


    TYPE* operator->();
        // Return a pointer to the current modifiable underlying
        // 'TYPE' object. The behaviour is undefined if the optional
        // object is disengaged.
    const TYPE* operator->() const ;
            // Return a pointer to the current non-modifiable underlying 'TYPE' object.
            // The behaviour is undefined if the optional object is disengaged.


    TYPE& operator*() BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& operator*() BSLS_KEYWORD_RVREF_QUAL;
#endif
            // Return a reference to the current, modifiable element.  The behavior
            // is undefined if the optional object is disengaged.
    const TYPE& operator*() const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    const TYPE&& operator*() const BSLS_KEYWORD_RVREF_QUAL;
#endif
            // Return a reference providing non-modifiable access to the underlying
            // 'TYPE' object. The behavior is undefined if if the optional object
            // is disengaged.


    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
            // Return allocator used for construction of value type. If value
            // type does not use allocators, returns bsl::allocator<void>

    bool has_value() const BSLS_KEYWORD_NOEXCEPT;
            // Return 'true' if this object is disengaged, and 'false' Otherwise.

    BSLS_KEYWORD_EXPLICIT operator bool()  const  BSLS_KEYWORD_NOEXCEPT;
            // Return 'true' if this object is disengaged, and 'false' Otherwise.

    template<class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    template<class ANY_TYPE>
    TYPE value_or(ANY_TYPE&& rhs) BSLS_KEYWORD_RVREF_QUAL;
#endif


    template<class ANY_TYPE>
    TYPE value_or(bsl::allocator_arg_t, allocator_type,
        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)
        ) const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    template<class ANY_TYPE>
    TYPE value_or(bsl::allocator_arg_t,
        allocator_type,
        ANY_TYPE&&) BSLS_KEYWORD_RVREF_QUAL;
#endif

    void swap(optional& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  In effect, performs
        // 'using bsl::swap; swap(c, other.c);'.
  private :

};

                             // =========================
                             // class optional<TYPE>
                             // =========================
//note : The body of a class definition is divided into public and
//private sections using the public: and private: labels. These labels must appear on
//lines by themselves, indented exactly two spaces, and preceded by a blank line
template <class TYPE>
class optional<TYPE, false> {
    // specialization of bslstl_optional for type that are no allocator aware
  public :
    // PUBLIC TYPES
    typedef TYPE value_type;

    // 'value_type' is an alias for the underlying 'TYPE' upon which this
    // template class is instantiated, and represents the type of the
    // managed object. The name is chosen so it is compatible with the
    // std::optional implementation.

  private:
  // note : Each category must be preceded by a tag comment that identifies the category.
  // The tag must appear immediately before the first declaration in that category, with
  // no intervening blank line. A tag must not be used if the category is Otherwise empty
  // PRIVATE TYPES

  typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

  // DATA
  optional_data_imp<TYPE>  d_value;       // in-place 'TYPE' object


  public:
    // todo : check if any rhs nested traits can be added
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(optional,
        BloombergLP::bslmf::IsBitwiseMoveable,
        BloombergLP::bslmf::IsBitwiseMoveable<TYPE>::value);

    // CREATORS
    optional();
      // Create a disengaged optional object having the null value.

    optional(nullopt_t );
      // Create a disengaged optional object having the null value.

    optional(const optional& original);
      // If original contains a value, initialize the contained value using
      // *original. Otherwise, create a disengaged optional.

    optional(BloombergLP::bslmf::MovableRef<optional> original);
      // If original contains a value, initialize the contained value by moving
      // from *original. Otherwise, create a disengaged optional. 'original'
      // is left in a valid, but unspecified state.

    template<class ANY_TYPE = TYPE,
              typename bsl::enable_if<!bsl::is_same<ANY_TYPE, optional<TYPE>>::value
                                      &&
                                      !bsl::is_same<ANY_TYPE, nullopt_t>::value
                                      &&
                                      std::is_constructible<TYPE, ANY_TYPE>::value
                                      &&
                                      bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                      bool>::type = true>
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, original));
    }
    template<class ANY_TYPE = TYPE,
              typename bsl::enable_if<!bsl::is_same<ANY_TYPE, optional<TYPE>>::value
                                      &&
                                      !bsl::is_same<ANY_TYPE, nullopt_t>::value
                                      &&
                                      std::is_constructible<TYPE, ANY_TYPE>::value
                                      &&
                                      !bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                      bool>::type = false>
    explicit
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, original));
    }

      // Create an optional object having the value of the specified
      // 'original' object.

    template<class ANY_TYPE,
            typename bsl::enable_if<
                    !bsl::is_same<ANY_TYPE, TYPE>::value
                    &&
                    std::is_constructible<TYPE,const ANY_TYPE &>::value
                    &&
                    bsl::is_convertible<const ANY_TYPE &, TYPE>::value
                    &&
                    !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
                    bool>::type = true>
    optional(const optional<ANY_TYPE>& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(original.value());
        }
    }

    template<class ANY_TYPE,
              typename bsl::enable_if<
                      !bsl::is_same<ANY_TYPE, TYPE>::value
                      &&
                      std::is_constructible<TYPE, const ANY_TYPE &>::value
                      &&
                      !bsl::is_convertible<const ANY_TYPE &, TYPE>::value
                      &&
                      !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
                      bool>::type = false>
    explicit
    optional(const optional<ANY_TYPE>& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(original.value());
        }
    }

      // If original contains a value, initialize the contained value using
      // *original. Otherwise, create a disengaged optional.

    template<class ANY_TYPE,
            typename bsl::enable_if<
                    !bsl::is_same<ANY_TYPE, TYPE>::value
                    &&
                    std::is_constructible<TYPE, ANY_TYPE >::value
                    &&
                    bsl::is_convertible<ANY_TYPE , TYPE>::value
                    &&
                    !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
                    bool>::type = true>
    optional(optional<ANY_TYPE>&& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(MoveUtil::move(original.value()));
        }
    }

    template<class ANY_TYPE,
            typename bsl::enable_if<
                    !bsl::is_same<ANY_TYPE, TYPE>::value
                    &&
                    std::is_constructible<TYPE, ANY_TYPE >::value
                    &&
                    !bsl::is_convertible<ANY_TYPE , TYPE>::value
                    &&
                    !bsl_converts_from_optional<TYPE,ANY_TYPE>::value,
                    bool>::type = false>
    explicit
    optional(optional<ANY_TYPE>&& original)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (original.has_value()) {
           emplace(MoveUtil::move(original.value()));
        }
    }

      // If original contains a value, initialize the contained value by moving
      // from *original. Otherwise, create a disengaged optional.
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... ARGS>
    explicit optional(in_place_t, BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... ARGS>
    explicit optional(in_place_t, std::initializer_list<U>,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

  ~optional();
          // Destroy this object.

  //MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... ARGS>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... ARGS>
    void emplace(std::initializer_list<U>, BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#else
#endif

  void reset() BSLS_KEYWORD_NOEXCEPT;
  // Reset this object to the default constructed state (i.e., to have
  // the null value).
  //OBSERVERS

  TYPE& value() BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  TYPE&& value() BSLS_KEYWORD_RVREF_QUAL;
#endif
          // Return a reference providing modifiable access to the underlying
          // 'TYPE' object. Throws a bsl::bad_optional_access if the optional
          // object is disengaged.
  const TYPE& value() const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  const TYPE&& value() const BSLS_KEYWORD_RVREF_QUAL;
#endif
          // Return a reference providing non-modifiable access to the underlying
          // 'TYPE' object. Throws a bsl::bad_optional_access if the optional
          // object is disengaged.

  optional& operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;

  optional& operator=(const optional&);
  optional& operator=(BloombergLP::bslmf::MovableRef<optional>);

  template<class ANY_TYPE>
  typename bsl::enable_if<
            std::is_constructible<TYPE,const ANY_TYPE &>::value
            &&
            std::is_assignable<TYPE&, ANY_TYPE>::value
            &&
            !bsl_converts_from_optional<TYPE,ANY_TYPE>::value
            &&
            !bsl_assigns_from_optional<TYPE,ANY_TYPE>::value,
            optional>::type &
            operator=(const optional<ANY_TYPE> &rhs)
  {
      // Must be in-place inline because the use of 'enable_if' will
      // otherwise break the MSVC 2010 compiler.
      if (rhs.has_value())
      {
        if (this->has_value())
          this->value() = rhs.value();
        else
          this->emplace(rhs.value());
      }
      else
      {
        this->reset();
      }
      return *this;
  }

  template<class ANY_TYPE>
  typename bsl::enable_if<
            std::is_constructible<TYPE, ANY_TYPE>::value
            &&
            std::is_assignable<TYPE&, ANY_TYPE>::value
            &&
            !bsl_converts_from_optional<TYPE,ANY_TYPE>::value
            &&
            !bsl_assigns_from_optional<TYPE,ANY_TYPE>::value,
            optional>::type &
            operator=(optional<ANY_TYPE>&& rhs)
  {
      // Must be in-place inline because the use of 'enable_if' will
      // otherwise break the MSVC 2010 compiler.
      if (rhs.has_value())
      {
        if (this->has_value())
          this->value() = MoveUtil::move(rhs.value());
        else
          this->emplace(MoveUtil::move(rhs.value()));
      }
      else
      {
        this->reset();
      }
      return *this;
  }


#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES )
    template<class ANY_TYPE = TYPE>
    typename bsl::enable_if<
                  !bsl::is_same<ANY_TYPE, optional>::value
                  &&
                  !(bsl::is_same<ANY_TYPE,
                        typename std::decay<TYPE>::type >::value
                    && std::is_scalar<TYPE>::value)
                  &&
                  std::is_constructible<TYPE, ANY_TYPE>::value
                  &&
                  std::is_assignable<TYPE&, ANY_TYPE>::value,
                  optional>::type &
    operator=(ANY_TYPE&& rhs)
    {
        // Must be in-place inline because the use of 'enable_if' will
        // otherwise break the MSVC 2010 compiler.
        if (this->has_value())
        {
            this->value() = std::forward<ANY_TYPE>(rhs);
        }
        else
        {
            this->emplace(std::forward<ANY_TYPE>(rhs));
        }
        return *this;
    }
          // Assign to this object the value of the specified 'rhs' object (of
          // 'BDE_rhs_TYPE') converted to 'TYPE', and return a reference
          // providing modifiable access to this object.  Note that this
          // method may invoke assignment from rhs, or construction from rhs,
          // depending on whether the lhs optional object is engaged.
          // Equality trait is needed to remove this function from the
          // overload set when assignment is done from the optional<TYPE>
          // object, as this is a better match for lvalues than the
          // operator=(const optional& rhs). Without rvalue references
          // and perfect forwarding, this is not the case.
#else
    template<class ANY_TYPE = TYPE>
    optional& operator=(const ANY_TYPE &rhs);

    template<class ANY_TYPE = TYPE>
    optional& operator=(ANY_TYPE&& rhs);
#endif

  TYPE* operator->();
              // Return a pointer to the current modifiable underlying
              // 'TYPE' object. The behaviour is undefined if the optional
              // object is disengaged.
  const TYPE* operator->() const;
          // Return a pointer to the current non-modifiable underlying 'TYPE' object.
          // The behaviour is undefined if the optional object is disengaged.

  TYPE& operator*() BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  TYPE&& operator*() BSLS_KEYWORD_RVREF_QUAL;
#endif
          // Return a reference to the current, modifiable element.  The behavior
          // is undefined if the optional object is disengaged.

  const TYPE& operator*() const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  const TYPE&& operator*() const BSLS_KEYWORD_RVREF_QUAL;
#endif
          // Return a reference providing non-modifiable access to the underlying
          // 'TYPE' object. The behavior is undefined if if the optional object
          // is disengaged.

  //todo add Type&& and const Type&& overloads
  bool has_value() const BSLS_KEYWORD_NOEXCEPT;
  // Return 'true' if this object is disengaged, and 'false' Otherwise.

  BSLS_KEYWORD_EXPLICIT operator bool()  const  BSLS_KEYWORD_NOEXCEPT;
  // Return 'true' if this object is disengaged, and 'false' Otherwise.

  template<class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const BSLS_KEYWORD_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  template<class ANY_TYPE>
    TYPE value_or(ANY_TYPE&&) BSLS_KEYWORD_RVREF_QUAL;
#endif

    void swap(optional& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  In effect, performs
        // 'using bsl::swap; swap(c, other.c);'.
  private:


};

// FREE FUNCTIONS
template <class TYPE>
void swap(bsl::optional<TYPE>& lhs,
          bsl::optional<TYPE>& rhs);
    // Swap the value of the specified 'lhs' optional with the value of the
    // specified 'rhs' optional.

// HASH SPECIALIZATIONS
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const optional<TYPE>&  input);
    // Pass the specified 'input' to the specified 'hashAlg'

// FREE OPERATORS
// comparison with optional
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const optional<LHS_TYPE>& lhs,
                const optional<RHS_TYPE>& rhs);
    // If bool(x) != bool(y), false; otherwise if bool(x) == false, true;
    // otherwise *x == *y. Note that this function will fail to compile if
    // 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const optional<LHS_TYPE>& lhs,
                const optional<RHS_TYPE>& rhs);
    // If bool(x) != bool(y), true; otherwise, if bool(x) == false, false;
    // otherwise *x != *y.  Note that this function will fail to compile if
    // 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const optional<LHS_TYPE>& lhs,
               const optional<RHS_TYPE>& rhs);
    // If !y, false; otherwise, if !x, true; otherwise *x < *y.  Note that
    // this function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are
    // not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const optional<LHS_TYPE>& lhs,
               const optional<RHS_TYPE>& rhs);
    // If !x, false; otherwise, if !y, true; otherwise *x > *y. note that this
    // function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not
    // compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const optional<LHS_TYPE>& lhs,
                const optional<RHS_TYPE>& rhs);
    // If !x, true; otherwise, if !y, false; otherwise *x <= *y. Note that this
    // function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not
    // compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const optional<LHS_TYPE>& lhs,
                const optional<RHS_TYPE>& rhs);
    // If !y, true; otherwise, if !x, false; otherwise *x >= *y. Note that this
    // function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not
    // compatible.

// comparison with nullopt_t
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator==(const optional<TYPE>& value,
                const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT;
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator==(const nullopt_t&         ,
                const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if 'value' is disengaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator!=(const optional<TYPE>& value,
                const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT;
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator!=(const nullopt_t&         ,
                const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if 'value' is engaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<(const optional<TYPE>&,
               const nullopt_t&        ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'false'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<(const nullopt_t&         ,
               const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if 'value' is engaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>(const optional<TYPE>& value,
               const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if 'value' is engaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>(const nullopt_t&         ,
               const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'false'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<=(const optional<TYPE>& value,
                const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if 'value' is disengaged, and 'false' otherwise.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator<=(const nullopt_t&         ,
                const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>=(const optional<TYPE>& value,
                const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true'.

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
bool operator>=(const nullopt_t&         ,
                const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT;
    /// Return 'true' if 'value' is disengaged, and 'false' otherwise.

// comparison with T
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const optional<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator!=(const LHS_TYPE&                lhs,
                const optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  A optional object and a value of
    // some type do not have the same value if either the optional object is
    // null, or its underlying value does not compare equal to the other value.
    // Note that this function will fail to compile if 'LHS_TYPE' and
    // 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const optional<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
template <class LHS_TYPE, class RHS_TYPE>
bool operator==(const LHS_TYPE&                lhs,
                const optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  A optional object and a value of some
    // type have the same value if the optional object is non-null and its
    // underlying value compares equal to the other value.  Note that this
    // function will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not
    // compatible.


template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const optional<LHS_TYPE>& lhs,
               const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered before
    // the specified 'rhs', and 'false' otherwise.  'lhs' is ordered before
    // 'rhs' if 'lhs' is null or 'lhs.value()' is ordered before 'rhs'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<(const LHS_TYPE&                lhs,
               const optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' optional object, and 'false' otherwise.  'lhs' is ordered before
    // 'rhs' if 'rhs' is not null and 'lhs' is ordered before 'rhs.value()'.


template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const optional<LHS_TYPE>& lhs,
               const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs', and 'false' otherwise.  'lhs' is ordered after
    // 'rhs' if 'lhs' is not null and 'lhs.value()' is ordered after 'rhs'.
    // Note that this operator returns 'rhs < lhs'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>(const LHS_TYPE&                lhs,
               const optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' optional object, and 'false' otherwise.  'lhs' is ordered after
    // 'rhs' if 'rhs' is null or 'lhs' is ordered after 'rhs.value()'.  Note
    // that this operator returns 'rhs < lhs'.


template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const optional<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered before
    // the specified 'rhs' or 'lhs' and 'rhs' have the same value, and 'false'
    // otherwise.  (See 'operator<' and 'operator=='.)  Note that this operator
    // returns '!(rhs < lhs)'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator<=(const LHS_TYPE&                lhs,
                const optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' optional object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  (See 'operator<' and 'operator=='.)  Note that this
    // operator returns '!(rhs < lhs)'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const optional<LHS_TYPE>& lhs,
                const optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' optional object or 'lhs' and 'rhs' have the same
    // value, and 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note
    // that this operator returns '!(lhs < rhs)'.  Also note that this function
    // will fail to compile if 'LHS_TYPE' and 'RHS_TYPE' are not compatible.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const optional<LHS_TYPE>& lhs,
                const RHS_TYPE&                rhs);
    // Return 'true' if the specified 'lhs' optional object is ordered after
    // the specified 'rhs' or 'lhs' and 'rhs' have the same value, and 'false'
    // otherwise.  (See 'operator>' and 'operator=='.)  Note that this operator
    // returns '!(lhs < rhs)'.

template <class LHS_TYPE, class RHS_TYPE>
bool operator>=(const LHS_TYPE&                lhs,
                const optional<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' optional object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  (See 'operator>' and 'operator=='.)  Note that this
    // operator returns '!(lhs < rhs)'.


template<class TYPE>
BSLS_KEYWORD_CONSTEXPR
optional<typename std::decay<TYPE>::type>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs);
    // Return an 'optional' object containing a 'TYPE' object created from
    // the given parameter using
    // optional((BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs);
    // If TYPE uses an allocator, the default allocator will be used for the
    // optional object.
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template<class TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // Return an 'optional' object containing a 'TYPE' object created from
    // the given parameters using
    // optional(in_place, (BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // If TYPE uses an allocator, the default allocator will be used for the
    // optional object.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template<class TYPE, class U, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
make_optional(std::initializer_list<U> il,
              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // Return an 'optional' object containing a 'TYPE' object created from
    // the given parameters using
    // optional(in_place,
    //          il,
    //          (BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // If TYPE uses an allocator, the default allocator will be used for the
    // optional object.
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif //!BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template<class TYPE>
BSLS_KEYWORD_CONSTEXPR
optional<typename std::decay<TYPE>::type>
alloc_optional(typename optional<typename std::decay<TYPE>::type
                                 >::allocator_type const&,
               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs);
    // Return an 'optional' object containing a 'TYPE' object created from
    // the given TYPE parameter using
    // optional(allocator_arg,
    //          alloc,
    //          (BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs));
    // Note that this function will fail to compile if TYPE doesn't use
    // allocators.
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template<class TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
alloc_optional(typename optional<TYPE>::allocator_type const& alloc,
               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // Return an 'optional' object containing a 'TYPE' object created from
    // the given TYPE parameter using
    // optional(allocator_arg,
    //          alloc,
    //          in_place,
    //          (BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // Note that this function will fail to compile if TYPE doesn't use
    // allocators.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template<class TYPE, class U, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
alloc_optional(typename optional<TYPE>::allocator_type const& alloc,
               std::initializer_list<U> il,
               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // Return an 'optional' object containing a 'TYPE' object created from
    // the given TYPE parameter using
    // optional(allocator_arg,
    //          alloc,
    //          in_place,
    //          il,
    //          (BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
    // Note that this function will fail to compile if TYPE doesn't use
    // allocators.
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif // !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                      // -------------------------
                      // class optional<TYPE>
                      // -------------------------

// CREATORS
template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional()
{}
template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(nullopt_t)
{}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
    const optional& original)
{
    if (original.has_value()) {
       emplace(original.value());
    }
}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
    BloombergLP::bslmf::MovableRef<optional> original)
: d_allocator(MoveUtil::access(original).get_allocator())
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
       emplace(MoveUtil::move(lvalue.value()));
    }
}
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
template <typename TYPE, bool UsesBslmaAllocator>
template<class... ARGS>
inline
optional<TYPE, UsesBslmaAllocator>::optional(in_place_t,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(std::forward<ARGS>(args)...);
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <typename TYPE, bool UsesBslmaAllocator>
template<class U, class... ARGS>
inline
optional<TYPE, UsesBslmaAllocator>::optional(in_place_t,
                std::initializer_list<U> it,
                BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(it, std::forward<ARGS>(args)...);
}
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif //BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                                             allocator_type basicAllocator)

:d_allocator(basicAllocator)
{
}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                                             allocator_type basicAllocator,
                                             nullopt_t)

:d_allocator(basicAllocator)
{
}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                            allocator_type basicAllocator,
                            const optional&  original)
: d_allocator(basicAllocator)
{
    if (original.has_value()) {
      this->emplace(original.value());
    }
}


template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                allocator_type basicAllocator,
                BloombergLP::bslmf::MovableRef<optional>  original)
: d_allocator(basicAllocator)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
       emplace(MoveUtil::move(lvalue.value()));
    }
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
template <typename TYPE, bool UsesBslmaAllocator>
template<class... ARGS>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
    bsl::allocator_arg_t,
    allocator_type alloc,
    in_place_t,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
: d_allocator(alloc)
{
      emplace(std::forward<ARGS>(args)...);
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <typename TYPE, bool UsesBslmaAllocator>
template<class U, class... ARGS>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
                  bsl::allocator_arg_t,
                  allocator_type alloc,
                  in_place_t,
                  std::initializer_list<U> il,
                  BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
: d_allocator(alloc)
{
    emplace(il, std::forward<ARGS>(args)...);
}
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::~optional()
{
    this->reset();
}


//MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename TYPE, bool UsesBslmaAllocator>
template <class... ARGS>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(ARGS&&... args)
{
    d_value.emplace(bsl::allocator_arg, d_allocator,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <typename TYPE, bool UsesBslmaAllocator>
template<class U, class... ARGS>
void optional<TYPE, UsesBslmaAllocator>::emplace(std::initializer_list<U> il,
                                                 ARGS&&... args)
{
    d_value.emplace(bsl::allocator_arg, d_allocator, il,
            BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_optional.h

#else

#endif


template <class TYPE, bool UsesBslmaAllocator>
inline
void optional<TYPE, UsesBslmaAllocator>::reset() BSLS_KEYWORD_NOEXCEPT
{
    d_value.reset();
}
//OBSERVERS
template <class TYPE, bool UsesBslmaAllocator>
inline
TYPE& optional<TYPE, UsesBslmaAllocator>::value() BSLS_KEYWORD_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE, bool UsesBslmaAllocator>
inline
TYPE&& optional<TYPE, UsesBslmaAllocator>::value() BSLS_KEYWORD_RVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE&& optional<TYPE, UsesBslmaAllocator>::value() const BSLS_KEYWORD_RVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}
#endif
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE& optional<TYPE, UsesBslmaAllocator>::value() const BSLS_KEYWORD_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class TYPE, bool UsesBslmaAllocator>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, UsesBslmaAllocator>::value_or(
            BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) rhs) const BSLS_KEYWORD_LVREF_QUAL
{
    if (this->has_value())
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          BloombergLP::bslma::Default::defaultAllocator(),
          this->value());
    else
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          BloombergLP::bslma::Default::defaultAllocator(),
          BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, rhs));

}
template <class TYPE, bool UsesBslmaAllocator>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, UsesBslmaAllocator>::value_or(bsl::allocator_arg_t,
    allocator_type allocator,
    BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) rhs) const BSLS_KEYWORD_LVREF_QUAL
{
    if (has_value())
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          allocator.mechanism(),
          this->value());
    else
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          allocator.mechanism(),
          BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, rhs));

}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE, bool UsesBslmaAllocator>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, UsesBslmaAllocator>::value_or(ANY_TYPE&& rhs) BSLS_KEYWORD_RVREF_QUAL
{
    if (has_value())
        return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
            BloombergLP::bslma::Default::defaultAllocator(),
            std::move(this->value()));
    else
        return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
            BloombergLP::bslma::Default::defaultAllocator(),
            std::forward<ANY_TYPE>(rhs));
}
template <class TYPE, bool UsesBslmaAllocator>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, UsesBslmaAllocator>::value_or(bsl::allocator_arg_t,
    allocator_type allocator,
    ANY_TYPE&& rhs) BSLS_KEYWORD_RVREF_QUAL
{
    if (has_value())
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          allocator.mechanism(),
          std::move(this->value()));
    else
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          allocator.mechanism(),
          std::forward<ANY_TYPE>(rhs));

}
#endif
template <class TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>&
optional<TYPE, UsesBslmaAllocator>::operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    this->reset();
    return *this;
}

template <class TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>&
optional<TYPE, UsesBslmaAllocator>::operator=(const optional &rhs)
{
  if (rhs.has_value())
  {
    if (this->has_value())
      this->value() = rhs.value();
    else
      this->emplace(rhs.value());
  }
  else
  {
    this->reset();
  }
  return *this;
}

template <class TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>&
optional<TYPE, UsesBslmaAllocator>::operator=(BloombergLP::bslmf::MovableRef<optional> rhs)
{
  if (rhs.has_value())
  {
    if (this->has_value())
      this->value() = MoveUtil::move(rhs.value());
    else
      this->emplace(MoveUtil::move(rhs.value()));
  }
  else
  {
    this->reset();
  }
  return *this;
}


template <class TYPE, bool UsesBslmaAllocator>
inline
TYPE& optional<TYPE, UsesBslmaAllocator>::operator*() BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return this->value();
}
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE& optional<TYPE, UsesBslmaAllocator>::operator*() const BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);
    return this->value();
}
template <class TYPE, bool UsesBslmaAllocator>
inline
TYPE* optional<TYPE, UsesBslmaAllocator>::operator->()
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return BSLS_UTIL_ADDRESSOF(this->value());
}
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE* optional<TYPE, UsesBslmaAllocator>::operator->() const
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return BSLS_UTIL_ADDRESSOF(this->value());
}

template <class TYPE, bool UsesBslmaAllocator>
inline
bool optional<TYPE, UsesBslmaAllocator>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_value.has_value();
}

template <class TYPE, bool UsesBslmaAllocator>
optional<TYPE, UsesBslmaAllocator>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return this->has_value();
}
template <class TYPE, bool UsesBslmaAllocator>
inline
typename optional<TYPE, UsesBslmaAllocator>::allocator_type
optional<TYPE, UsesBslmaAllocator>::get_allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return d_allocator;
}

template <class TYPE, bool UsesBslmaAllocator>
void optional<TYPE, UsesBslmaAllocator>::swap(optional &other)
{
    if (this->has_value() && other.has_value())
        BloombergLP::bslalg::SwapUtil::swap(&(this->value()),
                                            &(other.value()));
    else if (this->has_value())
    {
        other.emplace(MoveUtil::move(this->value()));
        this->reset();
    }
    else if (other.has_value())
    {
        this->emplace(MoveUtil::move(other.value()));
        other.reset();
    }
}

                      // ------------------------------------------
                      // class optional_WithoutAllocator<TYPE>
                      // ------------------------------------------

// CREATORS
template <class TYPE>
inline
optional<TYPE, false>::optional()
{
}
template <class TYPE>
inline
optional<TYPE, false>::optional(nullopt_t)
{
}

template <class TYPE>
inline
optional<TYPE, false>::optional(
    const optional& original)
{
    if (original.has_value()) {
       emplace(original.value());
    }
}

template <class TYPE>
inline
optional<TYPE, false>::optional(
    BloombergLP::bslmf::MovableRef<optional> original)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
       emplace(MoveUtil::move(lvalue.value()));
    }
}



#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
template <class TYPE>
template<class... ARGS>
inline
optional<TYPE, false>::optional(in_place_t,
                 BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(std::forward<ARGS>(args)...);
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class TYPE>
template<class U, class... ARGS>
inline
optional<TYPE, false>::optional(in_place_t,
                std::initializer_list<U> it,
                BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(it, std::forward<ARGS>(args)...);
}
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif //BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

template <class TYPE>
inline
optional<TYPE, false>::~optional()
{
    reset();
}

//MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename TYPE>
template <class... ARGS>
inline
void optional<TYPE, false>::emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    d_value.emplace(BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <typename TYPE>
template<class U, class... ARGS>
void optional<TYPE, false>::emplace(std::initializer_list<U> il,
    BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    d_value.emplace(il, BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif

template <class TYPE>
inline
void optional<TYPE, false>::reset() BSLS_KEYWORD_NOEXCEPT
{
    d_value.reset();
}
//OBSERVERS
template <class TYPE>
inline
TYPE& optional<TYPE, false>::value() BSLS_KEYWORD_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
template <class TYPE>
inline
const TYPE& optional<TYPE, false>::value() const BSLS_KEYWORD_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE>
inline
TYPE&& optional<TYPE, false>::value() BSLS_KEYWORD_RVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}
template <class TYPE>
inline
const TYPE&& optional<TYPE, false>::value() const BSLS_KEYWORD_RVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}
#endif
template <class TYPE>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, false>::value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) rhs)
          const BSLS_KEYWORD_LVREF_QUAL
{
    if (has_value())
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          BloombergLP::bslma::Default::defaultAllocator(),
          this->value());
    else
      return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
          BloombergLP::bslma::Default::defaultAllocator(),
          BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, rhs));

}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, false>::value_or(ANY_TYPE&& rhs) BSLS_KEYWORD_RVREF_QUAL
{
    if (has_value())
        return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
            BloombergLP::bslma::Default::defaultAllocator(),
            std::move(this->value()));
    else
        return BloombergLP::bslalg::AllocatorUtil<TYPE>::construct(
            BloombergLP::bslma::Default::defaultAllocator(),
            std::forward<ANY_TYPE>(rhs));
}
#endif
template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT
{
    this->reset();
    return *this;
}


template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(const optional &rhs)
{
  if (rhs.has_value())
  {
    if (this->has_value())
      this->value() = rhs.value();
    else
      this->emplace(rhs.value());
  }
  else
  {
    this->reset();
  }
  return *this;
}

template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(BloombergLP::bslmf::MovableRef<optional> rhs)
{
  if (rhs.has_value())
  {
    if (this->has_value())
      this->value() = MoveUtil::move(rhs.value());
    else
      this->emplace(MoveUtil::move(rhs.value()));
  }
  else
  {
    this->reset();
  }
  return *this;
}

template <class TYPE>
inline
TYPE& optional<TYPE, false>::operator*() BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(has_value());

    return d_value.value();
}
template <class TYPE>
inline
const TYPE& optional<TYPE, false>::operator*() const BSLS_KEYWORD_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(has_value());

    return d_value.value();
}
template <class TYPE>
inline
TYPE* optional<TYPE, false>::operator->()
{
    BSLS_ASSERT_SAFE(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}
template <class TYPE>
inline
const TYPE* optional<TYPE, false>::operator->() const
{
    BSLS_ASSERT_SAFE(has_value());

    return BSLS_UTIL_ADDRESSOF(d_value.value());
}

template <class TYPE>
inline
bool optional<TYPE, false>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_value.has_value();
}
template <class TYPE>
optional<TYPE, false>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return this->has_value();
}
template <class TYPE>
void optional<TYPE, false>::swap(optional &other)
{
    if (this->has_value() && other.has_value())
        BloombergLP::bslalg::SwapUtil::swap(&(this->value()),
                                            &(other.value()));
    else if (this->has_value())
    {
        other.emplace(MoveUtil::move(this->value()));
        this->reset();
    }
    else if (other.has_value())
    {
        this->emplace(MoveUtil::move(other.value()));
        other.reset();
    }
}

// FREE FUNCTIONS
template<typename TYPE>
inline
void swap(bsl::optional<TYPE>& lhs, optional<TYPE>& rhs)
{
    lhs.swap(rhs);
}
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const optional<TYPE>& input)
{
    if (input.has_value()) {
        hashAppend(hashAlg, true);
        hashAppend(hashAlg, input.value());
    }
    else {
        hashAppend(hashAlg, false);
    }
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const optional<LHS_TYPE>& lhs,
                      const optional<RHS_TYPE>& rhs)
{
    if (lhs && rhs) {
        return lhs.value() == rhs.value();                            // RETURN
    }

    return lhs.has_value() == rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const optional<LHS_TYPE>& lhs,
                      const optional<RHS_TYPE>& rhs)
{
    if (lhs && rhs) {
        return lhs.value() != rhs.value();                            // RETURN
    }

    return lhs.has_value() != rhs.has_value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const optional<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return lhs && lhs.value() == rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator==(const LHS_TYPE&                lhs,
                      const optional<RHS_TYPE>& rhs)
{
    return rhs && rhs.value() == lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const optional<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return !lhs || lhs.value() != rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator!=(const LHS_TYPE&                lhs,
                      const optional<RHS_TYPE>& rhs)
{
    return !rhs || rhs.value() != lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const optional<LHS_TYPE>& lhs,
                     const optional<RHS_TYPE>& rhs)
{
    if (!rhs) {
        return false;                                                 // RETURN
    }

    return !lhs || lhs.value() < rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const optional<LHS_TYPE>& lhs,
                     const RHS_TYPE&                rhs)
{
    return !lhs || lhs.value() < rhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<(const LHS_TYPE&                lhs,
                     const optional<RHS_TYPE>& rhs)
{
    return rhs && lhs < rhs.value();
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const optional<LHS_TYPE>& lhs,
                     const optional<RHS_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const optional<LHS_TYPE>& lhs,
                     const RHS_TYPE&                rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>(const LHS_TYPE&                lhs,
                     const optional<RHS_TYPE>& rhs)
{
    return rhs < lhs;
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const optional<LHS_TYPE>& lhs,
                      const optional<RHS_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const optional<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator<=(const LHS_TYPE&                lhs,
                      const optional<RHS_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const optional<LHS_TYPE>& lhs,
                      const optional<RHS_TYPE>& rhs)
{
    return !(lhs < rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const optional<LHS_TYPE>& lhs,
                      const RHS_TYPE&                rhs)
{
    return !(lhs < rhs);
}

template <class LHS_TYPE, class RHS_TYPE>
inline
bool operator>=(const LHS_TYPE&                lhs,
                      const optional<RHS_TYPE>& rhs)
{
    return !(lhs < rhs);
}


template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator==(const optional<TYPE>& value,
                      const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator==(const nullopt_t&         ,
                      const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator!=(const optional<TYPE>& value,
                      const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator!=(const nullopt_t&         ,
                      const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator<(const optional<TYPE>&,
                     const nullopt_t&        ) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator<(const nullopt_t&         ,
                     const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator>(const optional<TYPE>& value,
                     const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT
{
    return value.has_value();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator>(const nullopt_t&         ,
                     const optional<TYPE>& ) BSLS_KEYWORD_NOEXCEPT
{
    return false;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator<=(const optional<TYPE>& value,
                      const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator<=(const nullopt_t&         ,
                      const optional<TYPE>& ) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator>=(const optional<TYPE>& ,
                      const nullopt_t&         ) BSLS_KEYWORD_NOEXCEPT
{
    return true;
}

template <class TYPE>
inline BSLS_KEYWORD_CONSTEXPR
bool operator>=(const nullopt_t&  ,
                      const optional<TYPE>& value) BSLS_KEYWORD_NOEXCEPT
{
    return !value.has_value();
}


template<class TYPE>
BSLS_KEYWORD_CONSTEXPR
optional<typename std::decay<TYPE>::type>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs)
{
    return optional<typename std::decay<TYPE>::type>(
          BSLS_COMPILERFEATURES_FORWARD(TYPE, rhs));
}
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template<class TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
make_optional(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return optional<TYPE>(in_place,
                          BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template<class TYPE, class U, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
make_optional(std::initializer_list<U> il,
              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return optional<TYPE>(in_place, il,
                          BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#endif // !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template<class TYPE>
BSLS_KEYWORD_CONSTEXPR
optional<typename std::decay<TYPE>::type>
alloc_optional(typename optional<typename std::decay<TYPE>::type
                                >::allocator_type const& alloc,
               BSLS_COMPILERFEATURES_FORWARD_REF(TYPE) rhs)
{
    return optional<typename std::decay<TYPE>::type >(allocator_arg,
                          alloc,
                          in_place,
                          BSLS_COMPILERFEATURES_FORWARD(TYPE, rhs));
}
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template<class TYPE, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
alloc_optional(typename optional<TYPE>::allocator_type const& alloc,
               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)...args)
{
    return optional<TYPE>(allocator_arg,
                          alloc,
                          in_place,
                          BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif //!BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template<class TYPE, class U, class... ARGS>
BSLS_KEYWORD_CONSTEXPR
optional<TYPE>
alloc_optional(typename optional<TYPE>::allocator_type const& alloc,
               std::initializer_list<U> il,
               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    return optional<TYPE>(allocator_arg,
                          alloc,
                          in_place,
                          il,
                          BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
}
#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
}  // close bsl namespace
#endif // INCLUDED_BSLSTL_OPTIONAL
//todo : fix license
// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
