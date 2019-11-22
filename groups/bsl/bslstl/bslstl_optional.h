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
//@PURPOSE: Provide a template for nullable (in-place) objects.
//
//@CLASSES:
//  bdlb::optional: template for nullable (in-place) objects
//
//@SEE_ALSO: bdlb_nullableallocatedvalue
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

#include <bslma_stdallocator.h>

#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_if.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>


#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <bslscm_version.h>


#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif
/*
//todo  - check includes
#ifndef INCLUDED_BSLSTL_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif


#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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
struct nullopt_t{
    // nullopt_t is a tag type used to create optional objects in a disengaged
    // state. It should not be default constructible so the following
    // assignment isn't ambiguous :
    // optional<SomeType> o;
    // o = {};
    // where o is an optional object.
    nullopt_t(int) { };
};
extern nullopt_t nullopt;

                        // =========================
                        // class in_place_t
                        // =========================
struct in_place_t{
  // in_place_t is a tag type used to tags that can be passed to the
  // constructors of optional to indicate that the contained object should be
  // constructed in-place.
  explicit in_place_t(){};
};
extern in_place_t in_place;



#endif //__cpp_lib_optional

//note : banner indented by 25 spaces. Check if correct and remove note.

                        // =========================
                        // class optional<TYPE>
                        // =========================
//note : The body of a class definition is divided into public and
//private sections using the public: and private: labels. These labels must appear on
//lines by themselves, indented exactly two spaces, and preceded by a blank line
template <class TYPE, bool UsesBslmaAllocator = BloombergLP::bslma::UsesBslmaAllocator<TYPE>::value>
class optional
{
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
    // no intervening blank line. A tag must not be used if the category is otherwise empty
	// PRIVATE TYPES

	typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

	// DATA
	BloombergLP::bsls::ObjectBuffer<TYPE>  d_buffer;       // in-place 'TYPE' object
	allocator_type d_allocator;
	bool                      d_hasValue;       // 'true' if object has value, otherwise
	                                // 'false'


  public:
	// todo : check if any other nested traits can be added
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(optional, BloombergLP::bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(optional,
        BloombergLP::bslmf::IsBitwiseMoveable,
        BloombergLP::bslmf::IsBitwiseMoveable<TYPE>::value);

    // CREATORS
    optional();
        // Create a disengaged optional object having the null value.  If
        // 'TYPE' is allocator aware, use the currently installed default
        // allocator to supply memory for future value objects.

    optional(nullopt_t );
        // Create a disengaged optional object having the null value.  If
        // 'TYPE' is allocator aware, use the currently installed default
        // allocator to supply memory for future value objects.

    optional(const optional& original);
        // Create a optional object having the value of the specified
        // 'original' object.  If 'TYPE' takes an optional allocator at
        // construction, the allocator associated with 'original' is
        // propagated for use in the newly-created object.

    optional(BloombergLP::bslmf::MovableRef<optional> original);
        // Create a optional object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  If 'TYPE' takes an optional allocator at
        // construction, the allocator associated with 'original' is propagated
        // for use in the newly-created object.  'original' is left in a valid
        // but unspecified state.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator);
        // Create a disengaged optional object. Use the specified
        // 'basicAllocator' to supply memory for future objects.  Note that
        // this method will fail to compile if 'TYPE' is not an allocator
        // aware type.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
                 nullopt_t);
        // Create a disengaged optional object. Use the specified
        // 'basicAllocator' to supply memory for future objects.  Note that
        // this method will fail to compile if 'TYPE' is not an allocator
        // aware type.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
                 const optional& original);
        // Create an optional object having the value of the specified
        // 'original' object. Use the specified 'basicAllocator' to supply
        // memory. Note that this method will fail to compile if 'TYPE' is not
        // an allocator aware type.

    optional(bsl::allocator_arg_t, allocator_type basicAllocator,
        BloombergLP::bslmf::MovableRef<optional>);
        // Create a optional object having the same value as the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  Use the specified 'basicAllocator' to supply
        // memory.  'original' is left in a valid but unspecified state. Note
        // that this method will fail to compile if 'TYPE' is not an allocator
        // aware type.

// todo: add variadic arg constructors
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

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Reset this object to the default constructed state (i.e., to have
        // the null value).
    //OBSERVERS
    const TYPE& value() const;
            // Return a reference providing modifiable access to the underlying
            // 'TYPE' object.  The behavior is undefined unless this object is
            // non-null.
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
            // Return allocator used for construction of value type. If value
            // type does not use allocators, returns bsl::allocator<void>

    //todo add T&& and const T&& overloads
    bool has_value() const BSLS_KEYWORD_NOEXCEPT;
            // Return 'true' if this object is disengaged, and 'false' otherwise.
    BSLS_KEYWORD_EXPLICIT operator bool()  const  BSLS_KEYWORD_NOEXCEPT;
            // Return 'true' if this object is disengaged, and 'false' otherwise.

  private :
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
};

// =========================
// class optional<TYPE>
// =========================
//note : The body of a class definition is divided into public and
//private sections using the public: and private: labels. These labels must appear on
//lines by themselves, indented exactly two spaces, and preceded by a blank line
template <class TYPE>
class optional<TYPE, false>
{
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
  // no intervening blank line. A tag must not be used if the category is otherwise empty
  // PRIVATE TYPES


  typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

  // DATA
  BloombergLP::bsls::ObjectBuffer<TYPE>  d_buffer;       // in-place 'TYPE' object
  bool                      d_hasValue;       // 'true' if object has value, otherwise
                                 // 'false'


  public:
  // todo : check if any other nested traits can be added
  // TRAITS
  BSLMF_NESTED_TRAIT_DECLARATION_IF(optional,
      BloombergLP::bslmf::IsBitwiseMoveable,
      BloombergLP::bslmf::IsBitwiseMoveable<TYPE>::value);

  // CREATORS
  optional();
  // Create a disengaged optional object having the null value.  If
  // 'TYPE' is allocator aware, use the currently installed default
  // allocator to supply memory for future value objects.

  optional(nullopt_t );
  // Create a disengaged optional object having the null value.  If
  // 'TYPE' is allocator aware, use the currently installed default
  // allocator to supply memory for future value objects.

  optional(const optional& original);
  // Create a optional object having the value of the specified
  // 'original' object.  If 'TYPE' takes an optional allocator at
  // construction, the allocator associated with 'original' is
  // propagated for use in the newly-created object.

  optional(BloombergLP::bslmf::MovableRef<optional> original);
  // Create a optional object having the same value as the specified
  // 'original' object by moving the contents of 'original' to the
  // newly-created object.  If 'TYPE' takes an optional allocator at
  // construction, the allocator associated with 'original' is propagated
  // for use in the newly-created object.  'original' is left in a valid
  // but unspecified state.

  ~optional();
          // Destroy this object.

  //MANIPULATORS

  void reset() BSLS_KEYWORD_NOEXCEPT;
  // Reset this object to the default constructed state (i.e., to have
  // the null value).
  //OBSERVERS
  const TYPE& value() const;
  // Return a reference providing modifiable access to the underlying
  // 'TYPE' object.  The behavior is undefined unless this object is
  // non-null.

  //todo add T&& and const T&& overloads
  bool has_value() const BSLS_KEYWORD_NOEXCEPT;
  // Return 'true' if this object is disengaged, and 'false' otherwise.
  BSLS_KEYWORD_EXPLICIT operator bool()  const  BSLS_KEYWORD_NOEXCEPT;
  // Return 'true' if this object is disengaged, and 'false' otherwise.

  private:
  //MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=5
  template<class... Args>
  void emplace(BSLS_COMPILERFEATURES_FORWARD_REF(Args)...);
  #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
  template<class U, class... Args>
  void emplace(std::initializer_list<U>, BSLS_COMPILERFEATURES_FORWARD_REF(Args)...);
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
: d_hasValue(false)
{
}
template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(nullopt_t)
: d_hasValue(false)
{
}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                                             allocator_type basicAllocator)
: d_hasValue(false)
, d_allocator(basicAllocator)
{
}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                                             allocator_type basicAllocator,
                                             nullopt_t)
: d_hasValue(false)
, d_allocator(basicAllocator)
{
}
template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                            allocator_type basicAllocator,
                            const optional&  original)
: d_hasValue(false)
, d_allocator(basicAllocator)
{
    if (original.has_value()) {
      this->emplace(original.value());
    }
}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(
    BloombergLP::bslmf::MovableRef<optional> original)
: d_hasValue(false)
, d_allocator(MoveUtil::access(original).d_allocator)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
       emplace(MoveUtil::move(lvalue.value()));
    }
}

template <typename TYPE, bool UsesBslmaAllocator>
inline
optional<TYPE, UsesBslmaAllocator>::optional(bsl::allocator_arg_t,
                allocator_type basicAllocator,
                BloombergLP::bslmf::MovableRef<optional>  original)
: d_hasValue(false)
, d_allocator(basicAllocator)
{
    optional& lvalue = original;

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
//MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <typename TYPE, bool UsesBslmaAllocator>
template <class... ARGS>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    d_allocator.construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_optional.h
template <typename TYPE, bool UsesBslmaAllocator>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(
                               )
{
    reset();
    d_allocator.construct(d_buffer.address());
    d_hasValue = true;
}

template <typename TYPE, bool UsesBslmaAllocator>
template <class ARGS_1>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1)
{
    reset();
    d_allocator.construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1));
    d_hasValue = true;
}

template <typename TYPE, bool UsesBslmaAllocator>
template <class ARGS_1,
          class ARGS_2>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2)
{
    reset();
    d_allocator.construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2));
    d_hasValue = true;
}

template <typename TYPE, bool UsesBslmaAllocator>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3)
{
    reset();
    d_allocator.construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3));
    d_hasValue = true;
}

template <typename TYPE, bool UsesBslmaAllocator>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4)
{
    reset();
    d_allocator.construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4));
    d_hasValue = true;
}

template <typename TYPE, bool UsesBslmaAllocator>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4,
          class ARGS_5>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5)
{
    reset();
    d_allocator.construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_5, args_5));
    d_hasValue = true;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <typename TYPE, bool UsesBslmaAllocator>
template <class... ARGS>
inline
void optional<TYPE, UsesBslmaAllocator>::emplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    d_allocator.construct(
        d_buffer.address(),
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}
// }}} END GENERATED CODE
#endif


template <class TYPE, bool UsesBslmaAllocator>
inline
void optional<TYPE, UsesBslmaAllocator>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue) {
        d_buffer.object().~TYPE();
        d_hasValue = false;
    }
}
//OBSERVERS
template <class TYPE, bool UsesBslmaAllocator>
inline
const TYPE& optional<TYPE, UsesBslmaAllocator>::value() const
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
template <class TYPE, bool UsesBslmaAllocator>
inline
bool optional<TYPE, UsesBslmaAllocator>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_hasValue;
}


template <class TYPE, bool UsesBslmaAllocator>
optional<TYPE, UsesBslmaAllocator>::operator bool() const BSLS_KEYWORD_NOEXCEPT
{
    return this->has_value();
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
: d_hasValue(false)
{
}
template <class TYPE>
inline
optional<TYPE, false>::optional(nullopt_t)
: d_hasValue(false)
{
}
template <class TYPE>
inline
optional<TYPE, false>::
optional(const optional& original)
: d_hasValue(false)
{
    if (original.has_value()) {
      this->emplace(original.value());
    }
}

template <class TYPE>
inline
optional<TYPE, false>::optional(
    BloombergLP::bslmf::MovableRef<optional> original)
: d_hasValue(false)
{
    optional& lvalue = original;

    if (lvalue.has_value()) {
      this->emplace(MoveUtil::move(lvalue.value()));
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
    reset();
    bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_optional.h
template <typename TYPE>
inline
void optional<TYPE, false>::emplace(
                               )
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0);
    d_hasValue = true;
}

template <typename TYPE>
template <class ARGS_1>
inline
void optional<TYPE, false>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1));
    d_hasValue = true;
}

template <typename TYPE>
template <class ARGS_1,
          class ARGS_2>
inline
void optional<TYPE, false>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2));
    d_hasValue = true;
}

template <typename TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3>
inline
void optional<TYPE, false>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3));
    d_hasValue = true;
}

template <typename TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4>
inline
void optional<TYPE, false>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4));
    d_hasValue = true;
}

template <typename TYPE>
template <class ARGS_1,
          class ARGS_2,
          class ARGS_3,
          class ARGS_4,
          class ARGS_5>
inline
void optional<TYPE, false>::emplace(
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_1) args_1,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_2) args_2,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_3) args_3,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_4) args_4,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_5) args_5)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS_1, args_1),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_2, args_2),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_3, args_3),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_4, args_4),
        BSLS_COMPILERFEATURES_FORWARD(ARGS_5, args_5));
    d_hasValue = true;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <typename TYPE>
template <class... ARGS>
inline
void optional<TYPE, false>::emplace(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    reset();
    BloombergLP::bslma::ConstructionUtil::construct(
        d_buffer.address(),
        (void *)0,
        BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    d_hasValue = true;
}
// }}} END GENERATED CODE
#endif

template <class TYPE>
inline
void optional<TYPE, false>::reset() BSLS_KEYWORD_NOEXCEPT
{
    if (d_hasValue) {
        d_buffer.object().~TYPE();
        d_hasValue = false;
    }
}
//OBSERVERS
template <class TYPE>
inline
const TYPE& optional<TYPE, false>::value() const
{
    BSLS_ASSERT_SAFE(d_hasValue);

    return d_buffer.object();
}
template <class TYPE>
inline
bool optional<TYPE, false>::has_value() const BSLS_KEYWORD_NOEXCEPT
{
    return d_hasValue;
}
}  // close package namespace


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
