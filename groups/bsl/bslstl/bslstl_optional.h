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
//todo : fix doucmentation
//todo : sprinkle constexpr
// todo (decisions)
//  * what allocator to use in value_or(T&&) &
//  * what allocator to use in value_or(const T&) &&


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
//  assert( !nullableInt.hasValue());
//..
// Next, give the 'int' object the value 123 (making it non-null):
//..
//  nullableInt.makeValue(123);
//  assert(nullableInt.hasValue());
//  assert(123 == nullableInt.value());
//..
// Finally, reset the object to its default constructed state (i.e., null):
//..
//  nullableInt.reset();
//  assert( !nullableInt.hasValue());
//..

#include <bslalg_constructusingallocator.h>
#include <bslalg_scalarprimitives.h>

#include <bslma_stdallocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_if.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_removeconst.h>


#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <bslscm_version.h>

#include <bslstl_badoptionalaccess.h>



#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif
/*
//todo  - check includes

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif


#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif


#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif


#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif



#ifndef INCLUDED_BSLS_DEPRECATE
#include <bsls_deprecate.h>
#endif



#ifndef INCLUDED_BSLX_INSTREAMFUNCTIONS
#include <bslx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_OUTSTREAMFUNCTIONS
#include <bslx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLX_VERSIONFUNCTIONS
#include <bslx_versionfunctions.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_NEW
#include <bsl_new.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
*/

//todo move these macros somewhere more appropriate
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
#define BSLS_COMPILER_FEATURES_LVREF_QUAL &
#define BSLS_COMPILER_FEATURES_RVREF_QUAL &&
#else
#define BSLS_COMPILER_FEATURES_LVREF_QUAL
//#define BSLS_COMPILER_FEATURES_RVREF_QUAL //deliberately not defined as no C++03 functions should have
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
    typedef typename bsl::allocator<char> allocator_type;

    BloombergLP::bsls::ObjectBuffer<stored_type>  d_buffer;
                                     // in-place 'TYPE' object
    bool           d_hasValue;       // 'true' if object has value, rhswise
                                     // 'false'
  public:
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
    template<class... Args>
    void emplace(bsl::allocator_arg_t, allocator_type , Args&&...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... Args>
    void emplace(bsl::allocator_arg_t, allocator_type , std::initializer_list<U>, Args&&...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    void reset() BSLS_KEYWORD_NOEXCEPT;

  //ACCESSORS
    TYPE& value() BSLS_COMPILER_FEATURES_LVREF_QUAL;
    const TYPE& value() const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& value() BSLS_COMPILER_FEATURES_RVREF_QUAL;
    const TYPE&& value() const BSLS_COMPILER_FEATURES_RVREF_QUAL;
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
    bool           d_hasValue;       // 'true' if object has value, rhswise
                                     // 'false'
  public:

  //CREATORS
    optional_data_imp() BSLS_KEYWORD_NOEXCEPT;
    ~optional_data_imp();

   //MANIPULATORS
    void emplace();
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... Args>
    void emplace(Args&&...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... Args>
    void emplace(std::initializer_list<U>, Args&&...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    void reset() BSLS_KEYWORD_NOEXCEPT;

  //ACCESSORS
    TYPE& value() BSLS_COMPILER_FEATURES_LVREF_QUAL;
    const TYPE& value() const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& value() BSLS_COMPILER_FEATURES_RVREF_QUAL;
    const TYPE&& value() const BSLS_COMPILER_FEATURES_RVREF_QUAL;
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
    bsl::allocator_arg_t, allocator_type allocator, ARGS&&... args)
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
    ARGS&&... args)
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
BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
template <class TYPE, bool UsesBslmaAllocator>
const TYPE& optional_data_imp<TYPE,UsesBslmaAllocator>::value() const
BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE, bool UsesBslmaAllocator>
TYPE&& optional_data_imp<TYPE,UsesBslmaAllocator>::value()
BSLS_COMPILER_FEATURES_RVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return std::move(d_buffer.object());
}
template <class TYPE, bool UsesBslmaAllocator>
const TYPE&& optional_data_imp<TYPE,UsesBslmaAllocator>::value() const
BSLS_COMPILER_FEATURES_RVREF_QUAL
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
void optional_data_imp<TYPE, false>::emplace(ARGS&&... args)
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
    ARGS&&... args)
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
TYPE& optional_data_imp<TYPE, false>::value() BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
template <class TYPE>
const TYPE& optional_data_imp<TYPE, false>::value() const BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE>
TYPE&& optional_data_imp<TYPE, false>::value() BSLS_COMPILER_FEATURES_RVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return std::move(d_buffer.object());
}
template <class TYPE>
const TYPE&& optional_data_imp<TYPE, false>::value() const  BSLS_COMPILER_FEATURES_RVREF_QUAL
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
template <class TYPE, bool UsesBslmaAllocator = BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
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
    // no intervening blank line. A tag must not be used if the category is rhswise empty
	// PRIVATE TYPES

	typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

	// DATA
	optional_data_imp<TYPE>  d_value;       // in-place 'TYPE' object
    allocator_type d_allocator;


  public:
	// todo : check if any rhs nested traits can be added
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(optional, BloombergLP::bslma::UsesBslmaAllocator);
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

    optional(const TYPE& original);
        // Create an optional object having the value of the specified
        // 'original' object.  Use the currently installed default
        // allocator to supply memory for future value objects.

    optional(BloombergLP::bslmf::MovableRef<TYPE> original);
        // Create an optional object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' is
        // used as the allocator for the optional object. 'original' is left
        // in a valid but unspecified state.

    optional(const optional& original);
        // If original contains a value, initialize the contained value using
        // *original. rhswise, create a disengaged optional. Use the currently
        // installed default allocator to supply memory for this and any rhs
        // future value objects.

    optional(BloombergLP::bslmf::MovableRef<optional> original);
        // If original contains a value, initialize the contained value by moving
        // from *original. rhswise, create a disengaged optional. Use the allocator
        // from original to supply memory to supply memory for this and any rhs
        // future value objects.  'original' is left in a valid, but unspecified state.

    template<class ANY_TYPE>
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
        typename bsl::enable_if<!bsl::is_same<ANY_TYPE, TYPE>::value
                                &&
                                bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                void>::type * = 0);
        // Create an optional object having the value of the specified
        // 'original' object.  Use the currently installed default
        // allocator to supply memory for future value objects.

    template<class ANY_TYPE>
    optional(const optional<ANY_TYPE>& original,
                 typename bsl::enable_if<
                         bsl::is_convertible<ANY_TYPE, TYPE>::value,
                         void>::type * = 0);
        // If original contains a value, initialize the contained value using
        // *original. rhswise, create a disengaged optional. Use the currently
        // installed default allocator to supply memory for this and any rhs
        // future value objects.

    template<class ANY_TYPE>
    optional(BloombergLP::bslmf::MovableRef<optional<ANY_TYPE> > original,
             typename bsl::enable_if<
                     bsl::is_convertible<ANY_TYPE, TYPE>::value,
                     void>::type * = 0);
      // If original contains a value, initialize the contained value by moving
      // from *original. rhswise, create a disengaged optional. Use the currently
      // installed default allocator to supply memory for this and any rhs
      // future value objects.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... Args>
    explicit optional(in_place_t, Args&&...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... Args>
    explicit optional(in_place_t, std::initializer_list<U>, Args&&...);
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
                 const TYPE&);
        // Create an optional object having the value of the specified
        // 'original' object. Use the specified 'basicAllocator' to supply
        // memory.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        BloombergLP::bslmf::MovableRef<TYPE>);
        // Create a optional object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  Use the specified 'basicAllocator' to supply
        // memory.  'original' is left in a valid but unspecified state.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
                 const optional&);
        // If original contains a value, initialize the contained value
        // with *original. rhswise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        BloombergLP::bslmf::MovableRef<optional>);
        // If original contains a value, initialize the contained value by
        // moving from *original. rhswise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

    template<class ANY_TYPE>
    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) rhs,
        typename bsl::enable_if<!bsl::is_same<ANY_TYPE, TYPE>::value
                                &&
                                bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                void>::type * = 0);
        // Create a optional object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  Use the specified 'basicAllocator' to supply
        // memory.  'original' is left in a valid but unspecified state.

    template<class ANY_TYPE>
        optional(bsl::allocator_arg_t, allocator_type basicAllocator,
            const optional<ANY_TYPE>& rhs,
            typename bsl::enable_if<
                     bsl::is_convertible<ANY_TYPE, TYPE>::value,
                     void>::type * = 0);
        // If original contains a value, initialize the contained value
        // with *original. rhswise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

    template<class ANY_TYPE>
        optional(bsl::allocator_arg_t, allocator_type basicAllocator,
            BloombergLP::bslmf::MovableRef<optional<ANY_TYPE> > rhs,
            typename bsl::enable_if<
                     bsl::is_convertible<ANY_TYPE, TYPE>::value,
                     void>::type * = 0);
        // If original contains a value, initialize the contained value by
        // moving from *original. rhswise, create a disengaged optional.
        // Use the specified 'basicAllocator' to supply memory.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... Args>
    explicit optional(in_place_t, bsl::allocator_arg_t, allocator_type , Args&&...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... Args>
    explicit optional(in_place_t, bsl::allocator_arg_t, allocator_type , std::initializer_list<U>, Args&&...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

    ~optional();
    // Destroy this object.

    //MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... Args>
    void emplace(Args&&...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... Args>
    void emplace(std::initializer_list<U>, Args&&...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    // {{{ BEGIN GENERATED CODE
    // The following section is automatically generated.  **DO NOT EDIT**
    // Generator command line: sim_cpp11_features.pl bslstl_optional.h
    void emplace();

    template <class ARGS_1>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1);

    template <class ARGS_1,
              class ARGS_2>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2);

    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3);

    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4);

    template <class ARGS_1,
              class ARGS_2,
              class ARGS_3,
              class ARGS_4,
              class ARGS_5>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
// }}} END GENERATED CODE
#endif
    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to the default constructed state (i.e., to have
        // the null value).
    //OBSERVERS

    TYPE& value() BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& value() BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif
        // Return a reference providing modifiable access to the underlying
        // 'TYPE' object. Throws a bsl::bad_optional_access if the optional
        // object is disengaged.
    const TYPE& value() const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    const TYPE&& value() const BSLS_COMPILER_FEATURES_RVREF_QUAL;
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
    optional& operator=(const optional<ANY_TYPE> &rhs);
    template<class ANY_TYPE>
    optional& operator=(optional<ANY_TYPE>&& rhs);
        // If rhs is engaged, convert its value to TYPE and assign it
        // to this object. Otherwise, reset this object to a disengaged
        // state. Return a reference providing modifiable access to this
        // object.  Note that this method may invoke assignment from rhs, or
        // construction from rhs, depending on whether the lhs optional object
        // is engaged.

    optional& operator=(const TYPE& rhs);
    optional& operator=(BloombergLP::bslmf::MovableRef<TYPE> rhs);
        // Assign to this object the value of the specified 'rhs' object (of
        // 'BDE_rhs_TYPE'), and return a reference
        // providing modifiable access to this object.  Note that this
        // method may invoke assignment from rhs, or construction from rhs,
        // depending on whether the lhs optional object is engaged.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES )
    template<class ANY_TYPE>
    typename bsl::enable_if<
              !bsl::is_same<ANY_TYPE, TYPE>::value
              &&
              bsl::is_convertible<ANY_TYPE, TYPE>::value,
              optional>::type &
    operator=(ANY_TYPE&& rhs);
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
    template<class ANY_TYPE>
    optional& operator=(const ANY_TYPE &rhs);

    template<class ANY_TYPE>
    optional& operator=(BloombergLP::bslmf::MovableRef(ANY_TYPE) rhs);
#endif


    TYPE* operator->();
        // Return a pointer to the current modifiable underlying
        // 'TYPE' object. The behaviour is undefined if the optional
        // object is disengaged.
    const TYPE* operator->() const ;
            // Return a pointer to the current non-modifiable underlying 'TYPE' object.
            // The behaviour is undefined if the optional object is disengaged.


    TYPE& operator*() BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    TYPE&& operator*() BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif
            // Return a reference to the current, modifiable element.  The behavior
            // is undefined if the optional object is disengaged.
    const TYPE& operator*() const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    const TYPE&& operator*() const BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif
            // Return a reference providing non-modifiable access to the underlying
            // 'TYPE' object. The behavior is undefined if if the optional object
            // is disengaged.


    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
            // Return allocator used for construction of value type. If value
            // type does not use allocators, returns bsl::allocator<void>

    bool has_value() const BSLS_KEYWORD_NOEXCEPT;
            // Return 'true' if this object is disengaged, and 'false' rhswise.

    BSLS_KEYWORD_EXPLICIT operator bool()  const  BSLS_KEYWORD_NOEXCEPT;
            // Return 'true' if this object is disengaged, and 'false' rhswise.

    template<class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    template<class ANY_TYPE>
    TYPE value_or(ANY_TYPE&& rhs) BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif


    template<class ANY_TYPE>
    TYPE value_or(bsl::allocator_arg_t, allocator_type,
        BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)
        ) const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
    template<class ANY_TYPE>
    TYPE value_or(bsl::allocator_arg_t,
        allocator_type,
        ANY_TYPE&&) BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif

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
  // no intervening blank line. A tag must not be used if the category is rhswise empty
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

    optional(const TYPE& original);
      // Create an optional object having the value of the specified
      // 'original' object.

    optional(BloombergLP::bslmf::MovableRef<TYPE> original);
      // Create an optional object having the same value as the specified
      // 'original' object by moving the contents of 'original' to the
      // newly-created object. 'original' is left in a valid but
      // unspecified state.

    optional(const optional& original);
      // If original contains a value, initialize the contained value using
      // *original. rhswise, create a disengaged optional.

    optional(BloombergLP::bslmf::MovableRef<optional> original);
      // If original contains a value, initialize the contained value by moving
      // from *original. rhswise, create a disengaged optional. 'original'
      // is left in a valid, but unspecified state.

    template<class ANY_TYPE>
    optional(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
            typename bsl::enable_if<!bsl::is_same<ANY_TYPE, TYPE>::value
                                    &&
                                    bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                    void>::type * = 0);
      // Create an optional object having the value of the specified
      // 'original' object.

    template<class ANY_TYPE>
    optional(const optional<ANY_TYPE>& original,
                typename bsl::enable_if<
                        bsl::is_convertible<ANY_TYPE, TYPE>::value,
                        void>::type * = 0);
      // If original contains a value, initialize the contained value using
      // *original. rhswise, create a disengaged optional.

    template<class ANY_TYPE>
    optional(BloombergLP::bslmf::MovableRef<optional<ANY_TYPE> > original,
            typename bsl::enable_if<
                    bsl::is_convertible<ANY_TYPE, TYPE>::value,
                    void>::type * = 0);
      // If original contains a value, initialize the contained value by moving
      // from *original. rhswise, create a disengaged optional.
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... Args>
    explicit optional(in_place_t, Args&&...);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... Args>
    explicit optional(in_place_t, std::initializer_list<U>, Args&&...);
#endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

#endif // BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES

  ~optional();
          // Destroy this object.

  //MANIPULATORS
  //MANIPULATORS
  #if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
    template<class... Args>
    void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args)...);
    #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    template<class U, class... Args>
    void emplace(std::initializer_list<U>, BSLS_COMPILERFEATURES_FORWARD_REF(Args)...);
    #endif//BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
#else
#endif

  void reset() BSLS_KEYWORD_NOEXCEPT;
  // Reset this object to the default constructed state (i.e., to have
  // the null value).
  //OBSERVERS

  TYPE& value() BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  TYPE&& value() BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif
          // Return a reference providing modifiable access to the underlying
          // 'TYPE' object. Throws a bsl::bad_optional_access if the optional
          // object is disengaged.
  const TYPE& value() const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  const TYPE&& value() const BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif
          // Return a reference providing non-modifiable access to the underlying
          // 'TYPE' object. Throws a bsl::bad_optional_access if the optional
          // object is disengaged.

  optional& operator=(bsl::nullopt_t) BSLS_KEYWORD_NOEXCEPT;

  optional& operator=(const optional&);
  optional& operator=(BloombergLP::bslmf::MovableRef<optional>);

  template<class ANY_TYPE>
  optional& operator=(const optional<ANY_TYPE> &);
  template<class ANY_TYPE>
  optional& operator=(optional<ANY_TYPE>&&);

  optional& operator=(const TYPE&);
  optional& operator=(BloombergLP::bslmf::MovableRef<TYPE>);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES )
    template<class ANY_TYPE>
    typename bsl::enable_if<
              !bsl::is_same<ANY_TYPE, TYPE>::value
              &&
              bsl::is_convertible<ANY_TYPE, TYPE>::value,
              optional>::type &
    operator=(ANY_TYPE&& rhs);
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
    template<class ANY_TYPE>
    optional& operator=(const ANY_TYPE &rhs);

    template<class ANY_TYPE>
    optional& operator=(BloombergLP::bslmf::MovableRef(ANY_TYPE) rhs);
#endif

  TYPE* operator->();
              // Return a pointer to the current modifiable underlying
              // 'TYPE' object. The behaviour is undefined if the optional
              // object is disengaged.
  const TYPE* operator->() const;
          // Return a pointer to the current non-modifiable underlying 'TYPE' object.
          // The behaviour is undefined if the optional object is disengaged.

  TYPE& operator*() BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  TYPE&& operator*() BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif
          // Return a reference to the current, modifiable element.  The behavior
          // is undefined if the optional object is disengaged.

  const TYPE& operator*() const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  const TYPE&& operator*() const BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif
          // Return a reference providing non-modifiable access to the underlying
          // 'TYPE' object. The behavior is undefined if if the optional object
          // is disengaged.

  //todo add Type&& and const Type&& overloads
  bool has_value() const BSLS_KEYWORD_NOEXCEPT;
  // Return 'true' if this object is disengaged, and 'false' rhswise.

  BSLS_KEYWORD_EXPLICIT operator bool()  const  BSLS_KEYWORD_NOEXCEPT;
  // Return 'true' if this object is disengaged, and 'false' rhswise.

  template<class ANY_TYPE>
    TYPE value_or(BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)) const BSLS_COMPILER_FEATURES_LVREF_QUAL;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
  template<class ANY_TYPE>
    TYPE value_or(ANY_TYPE&&) BSLS_COMPILER_FEATURES_RVREF_QUAL;
#endif

  private:


};


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
inline optional<TYPE, UsesBslmaAllocator>::
optional(const TYPE& original)
{
    this->emplace(original);
}

template <typename TYPE, bool UsesBslmaAllocator>
inline optional<TYPE, UsesBslmaAllocator>::
optional(BloombergLP::bslmf::MovableRef<TYPE> original)
{
  TYPE& lvalue = original;
  this->emplace(MoveUtil::move(lvalue));
}

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


template <typename TYPE, bool UsesBslmaAllocator>
template<class ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
           BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
           typename bsl::enable_if<!bsl::is_same<ANY_TYPE, TYPE>::value
                                   &&
                                   bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                   void>::type *)
{
    this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, original));
}

template <typename TYPE, bool UsesBslmaAllocator>
template<class ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
    const optional<ANY_TYPE>& original,
    typename bsl::enable_if<
          bsl::is_convertible<ANY_TYPE, TYPE>::value,
          void>::type *)
{
    if (original.has_value()) {
       emplace(original.value());
    }
}

template <typename TYPE, bool UsesBslmaAllocator>
template<class ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
    BloombergLP::bslmf::MovableRef<optional<ANY_TYPE> > original,
    typename bsl::enable_if<
          bsl::is_convertible<ANY_TYPE, TYPE>::value,
          void>::type *)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
       emplace(MoveUtil::move(lvalue.value()));
    }
}

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
                            const TYPE&  original)
: d_allocator(basicAllocator)
{
    this->emplace(original);
}


template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                allocator_type basicAllocator,
                BloombergLP::bslmf::MovableRef<TYPE>  original)
: d_allocator(basicAllocator)
{
    TYPE& lvalue = original;
    this->emplace(MoveUtil::move(lvalue));
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

template <typename TYPE, bool UsesBslmaAllocator>
template <typename ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                            allocator_type basicAllocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE)  original,
                            typename bsl::enable_if<
                                      !bsl::is_same<ANY_TYPE, TYPE>::value
                                      &&
                                      bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                      void>::type *)
: d_allocator(basicAllocator)
{
    this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, original));
}

template <typename TYPE, bool UsesBslmaAllocator>
template <typename ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                            allocator_type basicAllocator,
                            const optional<ANY_TYPE>&  original,
                            typename bsl::enable_if<
                                             bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                             void>::type *)
: d_allocator(basicAllocator)
{
    if (original.has_value()) {
      this->emplace(original.value());
    }
}


template <typename TYPE, bool UsesBslmaAllocator>
template <typename ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                allocator_type basicAllocator,
                BloombergLP::bslmf::MovableRef<optional<ANY_TYPE>>  original,
                typename bsl::enable_if<
                                 bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                 void>::type *)
: d_allocator(basicAllocator)
{
    optional<ANY_TYPE>& lvalue = original;

    if (lvalue.has_value()) {
       emplace(MoveUtil::move(lvalue.value()));
    }
}

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
TYPE& optional<TYPE, UsesBslmaAllocator>::value() BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE, bool UsesBslmaAllocator>
inline
TYPE&& optional<TYPE, UsesBslmaAllocator>::value() BSLS_COMPILER_FEATURES_RVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE&& optional<TYPE, UsesBslmaAllocator>::value() const BSLS_COMPILER_FEATURES_RVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}
#endif
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE& optional<TYPE, UsesBslmaAllocator>::value() const BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}

template <class TYPE, bool UsesBslmaAllocator>
template <class ANY_TYPE>
inline
TYPE
optional<TYPE, UsesBslmaAllocator>::value_or(
            BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) rhs) const BSLS_COMPILER_FEATURES_LVREF_QUAL
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
    BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) rhs) const BSLS_COMPILER_FEATURES_LVREF_QUAL
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
optional<TYPE, UsesBslmaAllocator>::value_or(ANY_TYPE&& rhs) BSLS_COMPILER_FEATURES_RVREF_QUAL
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
    ANY_TYPE&& rhs) BSLS_COMPILER_FEATURES_RVREF_QUAL
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
optional<TYPE, UsesBslmaAllocator>::operator=(const TYPE &rhs)
{
  if (this->has_value())
      this->value() = rhs;
  else
      this->emplace(rhs);
  return *this;
}

template <class TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>&
optional<TYPE, UsesBslmaAllocator>::operator=(BloombergLP::bslmf::MovableRef<TYPE> rhs)
{
  if (this->has_value())
      this->value() = MoveUtil::move(rhs);
  else
      this->emplace(MoveUtil::move(rhs));
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES )
template <class TYPE, bool UsesBslmaAllocator>
template<class ANY_TYPE>
inline
typename bsl::enable_if<
          !bsl::is_same<ANY_TYPE, TYPE>::value
          &&
          bsl::is_convertible<ANY_TYPE, TYPE>::value,
          optional<TYPE, UsesBslmaAllocator>>::type &
optional<TYPE, UsesBslmaAllocator>::operator=(ANY_TYPE&& rhs)
{
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
#else
#endif

template <class TYPE, bool UsesBslmaAllocator>
template<class ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>&
optional<TYPE, UsesBslmaAllocator>::operator=(const optional<ANY_TYPE> &rhs)
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
template<class ANY_TYPE>
inline
optional<TYPE, UsesBslmaAllocator>&
optional<TYPE, UsesBslmaAllocator>::operator=(
    optional<ANY_TYPE>&& rhs)
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
TYPE& optional<TYPE, UsesBslmaAllocator>::operator*() BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return this->value();
}
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE& optional<TYPE, UsesBslmaAllocator>::operator*() const BSLS_COMPILER_FEATURES_LVREF_QUAL
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
                      // ---------------------------------------
                      // class optional<TYPE>
                      // ---------------------------------------




// ACCESSORS

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
optional<TYPE, false>::optional(const TYPE& original)
{
    this->emplace(original);
}

template <class TYPE>
inline
optional<TYPE, false>::
optional(BloombergLP::bslmf::MovableRef<TYPE> original)
{
    TYPE& lvalue = original;
    this->emplace(MoveUtil::move(lvalue));
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


template <class TYPE>
template<class ANY_TYPE>
inline
optional<TYPE, false>::optional(
           BSLS_COMPILERFEATURES_FORWARD_REF(ANY_TYPE) original,
           typename bsl::enable_if<!bsl::is_same<ANY_TYPE, TYPE>::value
                                   &&
                                   bsl::is_convertible<ANY_TYPE, TYPE>::value,
                                   void>::type *)
{
    this->emplace(BSLS_COMPILERFEATURES_FORWARD(ANY_TYPE, original));
}

template <class TYPE>
template<class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    const optional<ANY_TYPE>& original,
    typename bsl::enable_if<
          bsl::is_convertible<ANY_TYPE, TYPE>::value,
          void>::type *)
{
    if (original.has_value()) {
       emplace(original.value());
    }
}

template <class TYPE>
template<class ANY_TYPE>
inline
optional<TYPE, false>::optional(
    BloombergLP::bslmf::MovableRef<optional<ANY_TYPE> > original,
    typename bsl::enable_if<
          bsl::is_convertible<ANY_TYPE, TYPE>::value,
          void>::type *)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
       emplace(MoveUtil::move(lvalue.value()));
    }
}


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
    ARGS&&... args)
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
TYPE& optional<TYPE, false>::value() BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
template <class TYPE>
inline
const TYPE& optional<TYPE, false>::value() const BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return d_value.value();
}
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
template <class TYPE>
inline
TYPE&& optional<TYPE, false>::value() BSLS_COMPILER_FEATURES_RVREF_QUAL
{
    if (!has_value()) BSLS_THROW(bsl::bad_optional_access());

    return std::move(d_value.value());
}
template <class TYPE>
inline
const TYPE&& optional<TYPE, false>::value() const BSLS_COMPILER_FEATURES_RVREF_QUAL
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
          const BSLS_COMPILER_FEATURES_LVREF_QUAL
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
optional<TYPE, false>::value_or(ANY_TYPE&& rhs) BSLS_COMPILER_FEATURES_RVREF_QUAL
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
optional<TYPE, false>::operator=(const TYPE &rhs)
{
  if (this->has_value())
      this->value() = rhs;
  else
      this->emplace(rhs);
  return *this;
}

template <class TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(BloombergLP::bslmf::MovableRef<TYPE> rhs)
{
  if (this->has_value())
      this->value() = MoveUtil::move(rhs);
  else
      this->emplace(MoveUtil::move(rhs));
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES )
template <class TYPE>
template<class ANY_TYPE>
inline
typename bsl::enable_if<
          !bsl::is_same<ANY_TYPE, TYPE>::value
          &&
          bsl::is_convertible<ANY_TYPE, TYPE>::value,
          optional<TYPE, false>>::type &
optional<TYPE, false>::operator=(ANY_TYPE&& rhs)
{
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
#else
#endif


template <class TYPE>
template<class ANY_TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(const optional<ANY_TYPE> &rhs)
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
template<class ANY_TYPE>
inline
optional<TYPE, false>&
optional<TYPE, false>::operator=(
    optional<ANY_TYPE>&& rhs)
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
TYPE& optional<TYPE, false>::operator*() BSLS_COMPILER_FEATURES_LVREF_QUAL
{
    BSLS_ASSERT_SAFE(has_value());

    return d_value.value();
}
template <class TYPE>
inline
const TYPE& optional<TYPE, false>::operator*() const BSLS_COMPILER_FEATURES_LVREF_QUAL
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
