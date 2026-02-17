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

#ifndef REFLECT_CPP26_TYPE_TRAITS_TYPE_TUPLE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_TYPE_TUPLE_HPP

#include <ranges>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/config.hpp>
#include <tuple>
#include <type_traits>

namespace reflect_cpp26 {
template <class... Args>
struct type_tuple {
  static constexpr auto size = sizeof...(Args);
};

namespace impl {
consteval auto make_type_tuple_cat_type(std::initializer_list<std::meta::info> Tuples)
    -> std::meta::info {
  auto params = std::vector<std::meta::info>{};
  for (auto T : Tuples) {
    params.append_range(template_arguments_of(remove_cvref(T)));
  }
  return substitute(^^type_tuple, params);
}

consteval auto make_type_tuple_push_front_type(std::meta::info Tuple, std::meta::info T)
    -> std::meta::info {
  auto params = std::vector{T};
  params.append_range(template_arguments_of(remove_cvref(Tuple)));
  return substitute(^^type_tuple, params);
}

consteval auto make_type_tuple_push_back_type(std::meta::info Tuple, std::meta::info T)
    -> std::meta::info {
  auto params = template_arguments_of(remove_cvref(Tuple));
  params.push_back(T);
  return substitute(^^type_tuple, params);
}

consteval auto tuple_elements_to_type_tuple(std::meta::info T) {
  auto params_il_n = {T};
  auto n = extract<size_t>(substitute(^^std::tuple_size_v, params_il_n));
  auto elements = std::vector<std::meta::info>{};
  for (auto i = 0zU; i < n; i++) {
    auto I = std::meta::reflect_constant(i);
    auto params_il_elems = {I, T};
    elements.push_back(substitute(^^std::tuple_element_t, params_il_elems));
  }
  return substitute(^^type_tuple, elements);
}

template <class T>
concept tuple_elements_defined = requires {
  { typename T::tuple_elements{} } -> template_instance_of<type_tuple>;
};
}  // namespace impl

/**
 * Extracts the I-th type in given type_tuple.
 */
template <size_t I, template_instance_of<type_tuple> TypeTuple>
using ith_element_t = [:template_arguments_of(^^TypeTuple)[I]:];

/**
 * Concatenates Tuples to a single type_tuple.
 */
template <template_instance_of<type_tuple>... Tuples>
using type_tuple_cat_t = [:impl::make_type_tuple_cat_type({^^Tuples...}):];

/**
 * Prepends T to the head of Tuple.
 */
template <template_instance_of<type_tuple> Tuple, class T>
using type_tuple_push_front_t = [:impl::make_type_tuple_push_front_type(^^Tuple, ^^T):];

/**
 * Appends T to the tail of Tuple.
 */
template <template_instance_of<type_tuple> Tuple, class T>
using type_tuple_push_back_t = [:impl::make_type_tuple_push_back_type(^^Tuple, ^^T):];

/**
 * Makes type_tuple<T, T, ...> with T repeated N times.
 */
template <class T, size_t N>
using type_tuple_repeat_t = [:substitute(^^type_tuple, std::views::repeat(^^T, N)):];

/**
 * Given a tuple-like type T, tuple_elements_t<T> is a list that contains
 * std::tuple_element_t<I, T> for I = 0 to N-1 where N is the tuple size of T.
 * Example:
 * using Tuple = std::tuple<int, unsigned, float>;
 * using Elems = tuple_elements_t<Tuple>;
 * static_assert(std::is_same_v<type_tuple<int, unsigned, float>, Elems>);
 */
template <tuple_like Tuple>
using tuple_elements_t = [:impl::tuple_elements_to_type_tuple(^^Tuple):];
}  // namespace reflect_cpp26

/**
 * Convenient way to mark a type as tuple-like (enabling std::tuple_size and
 * std::tuple_element) by specifying it's tuple elements in 'tuple_elements'
 * type member which is an instance of type_tuple.
 * Examples in reflect_cpp26 source code:
 * (1) tuple_wrapper in tests/utils/test_type_tuple.cpp;
 * (2) kv_pair_wrapper types in implementation of module fixed_map.
 */
template <reflect_cpp26::impl::tuple_elements_defined TupleLike>
struct std::tuple_size<TupleLike>
    : std::integral_constant<size_t, TupleLike::tuple_elements::size> {};

/**
 * Same as above.
 */
template <size_t I, reflect_cpp26::impl::tuple_elements_defined TupleLike>
struct std::tuple_element<I, TupleLike> {
  using tuple_elements = typename TupleLike::tuple_elements;
  using type = reflect_cpp26::ith_element_t<I, tuple_elements>;
};

#endif  // REFLECT_CPP26_TYPE_TRAITS_TYPE_TUPLE_HPP
