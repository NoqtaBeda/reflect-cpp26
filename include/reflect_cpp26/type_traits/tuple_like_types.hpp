/**
 * Copyright (c) 2026 NoqtaBeda (noqtabeda@163.com)
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

#ifndef REFLECT_CPP26_TYPE_TRAITS_TUPLE_LIKE_TYPES_HPP
#define REFLECT_CPP26_TYPE_TRAITS_TUPLE_LIKE_TYPES_HPP

#include <algorithm>
#include <concepts>
#include <ranges>
#include <reflect_cpp26/type_traits/type_comparison.hpp>
#include <reflect_cpp26/utils/config.hpp>
#include <type_traits>
#include <utility>

namespace reflect_cpp26 {
namespace impl {
template <class T>
concept has_tuple_size = requires {
  { std::tuple_size<T>::value } -> std::convertible_to<size_t>;
};

template <class T, size_t I>
concept has_tuple_member_get_ith = requires(T t) {
  {
    t.template get<I>()
  } -> same_as_one_of<std::tuple_element_t<I, T>, std::tuple_element_t<I, T>&>;
  {
    std::as_const(t).template get<I>()
  } -> same_as_one_of<std::tuple_element_t<I, T>, const std::tuple_element_t<I, T>&>;
  {
    std::move(t).template get<I>()
  } -> same_as_one_of<std::tuple_element_t<I, T>, std::tuple_element_t<I, T> &&>;
  {
    std::move(std::as_const(t)).template get<I>()
  } -> same_as_one_of<std::tuple_element_t<I, T>, const std::tuple_element_t<I, T> &&>;
};

// get<I>(t) can be found via ADL.
template <class T, size_t I>
concept has_tuple_free_get_ith = requires(T t) {
  { get<I>(t) } -> same_as_one_of<std::tuple_element_t<I, T>, std::tuple_element_t<I, T>&>;
  {
    get<I>(std::as_const(t))
  } -> same_as_one_of<std::tuple_element_t<I, T>, const std::tuple_element_t<I, T>&>;
  {
    get<I>(std::move(t))
  } -> same_as_one_of<std::tuple_element_t<I, T>, std::tuple_element_t<I, T> &&>;
  {
    get<I>(std::move(std::as_const(t)))
  } -> same_as_one_of<std::tuple_element_t<I, T>, const std::tuple_element_t<I, T> &&>;
};

template <class T, size_t I>
concept has_tuple_get_ith = has_tuple_member_get_ith<T, I> || has_tuple_free_get_ith<T, I>;

template <class T>
concept has_tuple_get =
    std::ranges::all_of(std::views::iota(0zU, std::tuple_size_v<T>), [](size_t i) {
      auto I = std::meta::reflect_constant(i);
      auto params_il = {^^T, I};
      return extract<bool>(substitute(^^has_tuple_get_ith, params_il));
    });

consteval bool tuple_elements_match(std::meta::info T,
                                    size_t tuple_size,
                                    std::span<const std::meta::info> args,
                                    bool (*match_predicate)(std::meta::info, std::meta::info)) {
  if (args.size() != tuple_size) {
    return false;
  }
  for (auto i = 0zU; i < tuple_size; i++) {
    auto vi = std::meta::reflect_constant(i);
    auto params_il = {vi, T};
    auto ti = substitute(^^std::tuple_element_t, params_il);
    if (!match_predicate(ti, args[i])) {
      return false;
    }
  }
  return true;
}

// clang-format off
template <class T, class... Args>
concept is_tuple_like_of_exactly = impl::tuple_elements_match(
    ^^T, std::tuple_size_v<T>, std::array{^^Args...}, std::meta::is_same_type);

template <class T, class... Args>
concept is_tuple_like_of = impl::tuple_elements_match(
    ^^T, std::tuple_size_v<T>, std::array{^^Args...}, std::meta::is_convertible_type);
// clang-format on

consteval bool sizes_are_equal(std::initializer_list<size_t> sizes) {
  auto first = data(sizes)[0];  // Compile error on empty sizes
  return std::ranges::all_of(sizes, [first](size_t s) { return s == first; });
}
}  // namespace impl

/**
 * Whether T (possible cv-qualified) is a tuple-like type.
 * See: https://en.cppreference.com/w/cpp/utility/tuple/tuple-like
 */
template <class T>
concept tuple_like =
    impl::has_tuple_size<std::remove_cv_t<T>> && impl::has_tuple_get<std::remove_cv_t<T>>;

/**
 * Whether std::remove_cv_t<T> is a pair-like type, i.e. tuple-like type
 * with size == 2.
 */
template <class T>
concept pair_like = tuple_like<T> && std::tuple_size_v<std::remove_cv_t<T>> == 2;

/**
 * Whether std::remove_cv_t<T> is a tuple-like type whose element types
 * are exactly Args... respectively.
 */
template <class T, class... Args>
concept tuple_like_of_exactly =
    tuple_like<T> && impl::is_tuple_like_of_exactly<std::remove_cv_t<T>, Args...>;

/**
 * Whether std::remove_cv_t<T> is a tuple-like type whose elements types
 * can be converted to Args... respectively.
 */
template <class T, class... Args>
concept tuple_like_of = tuple_like<T> && impl::is_tuple_like_of<std::remove_cv_t<T>, Args...>;

/**
 * Whether std::remove_cv_t<Args>... are tuple-like types
 * with the same tuple size.
 */
template <class... Args>
constexpr auto are_tuple_like_of_same_size_v = false;

template <tuple_like T, tuple_like... Args>
constexpr auto are_tuple_like_of_same_size_v<T, Args...> = impl::sizes_are_equal(
    {std::tuple_size_v<std::remove_cv_t<T>>, std::tuple_size_v<std::remove_cv_t<Args>>...});

/**
 * Whether std::remove_cv_t<T> and std::remove_cv_t<Tuple> are tuple-like
 * types with the same tuple size.
 */
template <class T, class Tuple>
concept tuple_like_of_same_size_with = are_tuple_like_of_same_size_v<T, Tuple>;
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_TUPLE_LIKE_TYPES_HPP
