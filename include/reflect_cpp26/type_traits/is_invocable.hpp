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

#ifndef REFLECT_CPP26_TYPE_TRAITS_IS_INVOCABLE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_IS_INVOCABLE_HPP

// Root header: include config.h and C++ stdlib headers only
// to prevent circular dependency.
#include <reflect_cpp26/utils/config.h>
#include <ranges>
#include <type_traits>

namespace reflect_cpp26 {
/**
 * Stronger constraint than std::is_invocable_r_v with implicit conversion
 * of result type disallowed. Usable if ambiguity is a concern.
 */
template <class R, class Func, class... Args>
constexpr auto is_invocable_exactly_r_v = false;

template <class R, class Func, class... Args>
  requires (std::is_invocable_v<Func, Args...>)
constexpr auto is_invocable_exactly_r_v<R, Func, Args...> =
  std::is_same_v<std::invoke_result_t<Func, Args...>, R>;

/**
 * Stronger constraint than std::is_nothrow_invocable_r_v with
 * implicit conversion of result type banned. Usable if ambiguity is a concern.
 */
template <class R, class Func, class... Args>
constexpr auto is_nothrow_invocable_exactly_r_v = false;

template <class R, class Func, class... Args>
  requires (std::is_nothrow_invocable_v<Func, Args...>)
constexpr auto is_nothrow_invocable_exactly_r_v<R, Func, Args...> =
  std::is_same_v<std::invoke_result_t<Func, Args...>, R>;

namespace impl {
consteval bool is_invocable_n_generic(
  std::meta::info Predicate, std::meta::info F, std::meta::info Arg, size_t N)
{
  auto params = std::vector{F};
  params.append_range(std::views::repeat(Arg, N));
  return extract<bool>(substitute(Predicate, params));
}

consteval bool is_invocable_r_n_generic(
  std::meta::info Predicate, std::meta::info R, std::meta::info F,
  std::meta::info Arg, size_t N)
{
  auto params = std::vector{R, F};
  params.append_range(std::views::repeat(Arg, N));
  return extract<bool>(substitute(Predicate, params));
}
} // namespace impl

/**
 * Equivalent to std::is_invocable_v<Func, Arg...>
 * where Arg is repeated N times.
 */
template <class Func, class Arg, size_t N>
constexpr auto is_invocable_n_v =
  impl::is_invocable_n_generic(^^std::is_invocable_v, ^^Func, ^^Arg, N);

/**
 * Equivalent to std::is_nothrow_invocable_v<Func, Arg...>
 * where Arg is repeated N times.
 */
template <class Func, class Arg, size_t N>
constexpr auto is_nothrow_invocable_n_v =
  impl::is_invocable_n_generic(^^std::is_nothrow_invocable_v, ^^Func, ^^Arg, N);

/**
 * Equivalent to std::is_invocable_r_v<R, Func, Arg...>
 * where Arg is repeated N times.
 */
template <class R, class Func, class Arg, size_t N>
constexpr auto is_invocable_r_n_v =
  impl::is_invocable_r_n_generic(
    ^^std::is_invocable_r_v, ^^R, ^^Func, ^^Arg, N);

/**
 * Equivalent to is_invocable_exactly_r_v<R, Func, Args...>
 * where Arg is repeated N times.
 */
template <class R, class Func, class Arg, size_t N>
constexpr auto is_invocable_exactly_r_n_v =
  impl::is_invocable_r_n_generic(
    ^^is_invocable_exactly_r_v, ^^R, ^^Func, ^^Arg, N);

/**
 * Equivalent to std::is_nothrow_invocable_r_v<R, Func, Arg...>
 * where Arg is repeated N times.
 */
template <class R, class Func, class Arg, size_t N>
constexpr auto is_nothrow_invocable_r_n_v =
  impl::is_invocable_r_n_generic(
    ^^std::is_nothrow_invocable_r_v, ^^R, ^^Func, ^^Arg, N);

/**
 * Equivalent to is_nothrow_invocable_exactly_r_v<R, Func, Args...>
 * where Arg is repeated N times.
 */
template <class R, class Func, class Arg, size_t N>
constexpr auto is_nothrow_invocable_exactly_r_n_v =
  impl::is_invocable_r_n_generic(
    ^^is_nothrow_invocable_exactly_r_v, ^^R, ^^Func, ^^Arg, N);

// -------- Concepts --------

#define REFLECT_CPP26_INVOCABLE_CONCEPT(name, ns)       \
  template <class F, class... Args>                     \
  concept name##_with = ns::is_##name##_v<F, Args...>;

REFLECT_CPP26_INVOCABLE_CONCEPT(invocable, std)
REFLECT_CPP26_INVOCABLE_CONCEPT(nothrow_invocable, std)
#undef REFLECT_CPP26_INVOCABLE_CONCEPT

#define REFLECT_CPP26_INVOCABLE_CONCEPT_R(name, ns)           \
  template <class F, class R, class... Args>                  \
  concept name##_r_with = ns::is_##name##_r_v<R, F, Args...>;

REFLECT_CPP26_INVOCABLE_CONCEPT_R(invocable, std)
REFLECT_CPP26_INVOCABLE_CONCEPT_R(nothrow_invocable, std)
REFLECT_CPP26_INVOCABLE_CONCEPT_R(invocable_exactly, reflect_cpp26)
REFLECT_CPP26_INVOCABLE_CONCEPT_R(nothrow_invocable_exactly, reflect_cpp26)
#undef REFLECT_CPP26_INVOCABLE_CONCEPT_R

#define REFLECT_CPP26_INVOCABLE_CONCEPT_N(name)       \
  template <class F, class Arg, size_t N>             \
  concept name##_with_n = is_##name##_n_v<F, Arg, N>;

REFLECT_CPP26_INVOCABLE_CONCEPT_N(invocable)
REFLECT_CPP26_INVOCABLE_CONCEPT_N(nothrow_invocable)
#undef REFLECT_CPP26_INVOCABLE_CONCEPT_N

#define REFLECT_CPP26_INVOCABLE_CONCEPT_R_N(name)             \
  template <class F, class R, class Arg, size_t N>            \
  concept name##_r_with_n = is_##name##_r_n_v<R, F, Arg, N>;

REFLECT_CPP26_INVOCABLE_CONCEPT_R_N(invocable)
REFLECT_CPP26_INVOCABLE_CONCEPT_R_N(nothrow_invocable)
REFLECT_CPP26_INVOCABLE_CONCEPT_R_N(invocable_exactly)
REFLECT_CPP26_INVOCABLE_CONCEPT_R_N(nothrow_invocable_exactly)
#undef REFLECT_CPP26_INVOCABLE_CONCEPT_R_N
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_IS_INVOCABLE_HPP
