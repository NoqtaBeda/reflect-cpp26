/**
 * Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#ifndef REFLECT_CPP26_TYPE_TRAITS_FUNCTION_TYPES_HPP
#define REFLECT_CPP26_TYPE_TRAITS_FUNCTION_TYPES_HPP

#include <reflect_cpp26/utils/type_tuple.hpp>

namespace reflect_cpp26 {
struct function_trait_flags {
  bool has_ellipsis_parameter;
  bool is_const;
  bool is_volatile;
  bool is_lvalue_reference;
  bool is_rvalue_reference;
  bool is_noexcept;

  constexpr bool operator==(const function_trait_flags&) const = default;
};

template <class Fn>
struct function_traits {
  static constexpr auto is_function = false;
};

/**
 * Extracts type info of function type
 * Ret(Args...) qualifiers or Ret(Args..., ...) qualifiers.
 *
 * (1) is_function: true for function types, false otherwise.
 * (2) flags: Boolean properties of given function type.
 *            See function_trait_flags for details.
 * (3) result_type: Gets Ret.
 * (4) args_type: Gets type_tuple<Args...>.
 */
#define REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS(AfterRet, qualifiers, ...) \
  template <class Ret, class... Args>                                       \
  struct function_traits<Ret AfterRet qualifiers> {                         \
    static constexpr auto is_function = true;                               \
    static constexpr auto flags = function_trait_flags{__VA_ARGS__};        \
    static constexpr auto has_ellipsis_parameter =                          \
      flags.has_ellipsis_parameter;                                         \
    static constexpr auto is_const =                                        \
      flags.is_const;                                                       \
    static constexpr auto is_volatile =                                     \
      flags.is_volatile;                                                    \
    static constexpr auto is_lvalue_reference =                             \
      flags.is_lvalue_reference;                                            \
    static constexpr auto is_rvalue_reference =                             \
      flags.is_rvalue_reference;                                            \
    static constexpr auto is_noexcept =                                     \
      flags.is_noexcept;                                                    \
                                                                            \
    using result_type = Ret;                                                \
    using args_type = type_tuple<Args...>;                                  \
  };

// 2 variants: with ellipsis (C-style va-arg) or without
#define REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_VA_ARG(qualifiers, ...)  \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS(                               \
    (Args...), qualifiers, __VA_ARGS__)                                   \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS(                               \
    (Args..., ...), qualifiers, .has_ellipsis_parameter = true, __VA_ARGS__)

// 4 variants: all subsets of {const, volatile}
#define REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_CV(qualifiers, ...)          \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_VA_ARG(qualifiers,                 \
    __VA_ARGS__);                                                             \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_VA_ARG(const qualifiers,           \
    .is_const = true, __VA_ARGS__);                                           \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_VA_ARG(volatile qualifiers,        \
    .is_volatile = true, __VA_ARGS__);                                        \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_VA_ARG(const volatile qualifiers,  \
    .is_const = true, .is_volatile = true, __VA_ARGS__)

// 3 variants: no-ref, lvalue-ref and rvalue-ref
#define REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_REF(qualifiers, ...) \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_CV(qualifiers,             \
    __VA_ARGS__);                                                     \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_CV(& qualifiers,           \
    .is_lvalue_reference = true, __VA_ARGS__);                        \
  REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_CV(&& qualifiers,          \
    .is_rvalue_reference = true, __VA_ARGS__)

// 2 variants: with / without noexcept
REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_REF(/* n/a */, /* no flags */);
REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_REF(noexcept, .is_noexcept = true);
// Total: 2 x 4 x 3 x 2 = 48

#undef REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS
#undef REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_VA_ARG
#undef REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_CV
#undef REFLECT_CPP26_SPECIALIZE_FUNCTION_TRAITS_REF

namespace impl {
template <class T>
struct to_function_pointer {}; // type undefined

#define REFLECT_CPP26_SPECIALIZE_TO_FUNCTION_POINTER(SignatureSuffix) \
  template <class Ret, class... Args>                                 \
  struct to_function_pointer<Ret SignatureSuffix> {                   \
    using type = Ret (*) SignatureSuffix;                             \
  };                                                                  \
  template <class Ret, class... Args>                                 \
  struct to_function_pointer<Ret (*) SignatureSuffix> {               \
    using type = Ret (*) SignatureSuffix;                             \
  };

REFLECT_CPP26_SPECIALIZE_TO_FUNCTION_POINTER((Args...))
REFLECT_CPP26_SPECIALIZE_TO_FUNCTION_POINTER((Args..., ...))
REFLECT_CPP26_SPECIALIZE_TO_FUNCTION_POINTER((Args...) noexcept)
REFLECT_CPP26_SPECIALIZE_TO_FUNCTION_POINTER((Args..., ...) noexcept)

#undef REFLECT_CPP26_SPECIALIZE_TO_FUNCTION_POINTER

template <class T>
struct to_function {
  static constexpr auto enabled = false; // type undefined
};

template <class T>
  requires (std::is_function_v<T>)
struct to_function<T> {
  static constexpr auto enabled = true;
  using type = T;
};

template <class T>
  requires (std::is_function_v<T>)
struct to_function<T*> {
  static constexpr auto enabled = true;
  using type = T;
};

template <class T, class U>
  requires (std::is_function_v<T>)
struct to_function<T U::*> {
  static constexpr auto enabled = true;
  using type = T;
};
} // namespace impl

/**
 * Checks whether T can be converted to a function pointer type.
 */
template <class T>
constexpr auto is_convertible_to_function_pointer_v =
  requires { std::declval<
    typename impl::to_function_pointer<std::remove_cvref_t<T>>::type>(); };

/**
 * If T is a function type, converts function T to the corresponding
 * function pointer type (if possible).
 * If T is already a function pointer type, gets std::remove_cvref_t<T>.
 * In all other cases, the program is ill-formed.
 */
template <class T>
  requires (is_convertible_to_function_pointer_v<T>)
using to_function_pointer_t =
  typename impl::to_function_pointer<std::remove_cvref_t<T>>::type;

/**
 * Checks whether T can be converted to a function type.
 */
template <class T>
constexpr auto is_convertible_to_function_v =
  impl::to_function<std::remove_cvref_t<T>>::enabled;

/**
 * Converts T to a function type.
 * (1) If T is a function type, gets std::remove_cvref_t<T>.
 * (2) If T is a function pointer type U*, converts to U.
 * (3) If T is a member function pointer type U W::*, converts to U.
 * Otherwise, the program is ill-formed.
 */
template <class T>
  requires (is_convertible_to_function_v<T>)
using to_function_t = typename impl::to_function<std::remove_cvref_t<T>>::type;
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_FUNCTION_TYPES_HPP
