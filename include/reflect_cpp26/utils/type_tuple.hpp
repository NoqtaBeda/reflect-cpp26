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

#ifndef REFLECT_CPP26_TYPE_TRAITS_TYPE_TUPLE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_TYPE_TUPLE_HPP

#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/config.h>
#include <reflect_cpp26/utils/constant.hpp>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace reflect_cpp26 {
template <class... Args>
struct type_tuple {
  static constexpr auto size = sizeof...(Args);
};

namespace impl {
consteval auto make_type_tuple_cat_type(
  std::initializer_list<std::meta::info> Tuples) -> std::meta::info
{
  auto params = std::vector<std::meta::info>{};
  for (auto T: Tuples) {
    params.append_range(template_arguments_of(remove_cvref(T)));
  }
  return substitute(^^type_tuple, params);
}

consteval auto make_type_tuple_push_front_type(
  std::meta::info Tuple, std::meta::info T) -> std::meta::info
{
  auto params = std::vector{T};
  params.append_range(template_arguments_of(remove_cvref(Tuple)));
  return substitute(^^type_tuple, params);
}

consteval auto make_type_tuple_push_back_type(
  std::meta::info Tuple, std::meta::info T) -> std::meta::info
{
  auto params = template_arguments_of(remove_cvref(Tuple));
  params.push_back(T);
  return substitute(^^type_tuple, params);
}
} // namespace impl

/**
 * Concatenates Tuples to a single type_tuple.
 */
template <template_instance_of<type_tuple>... Tuples>
using type_tuple_cat_t = [: impl::make_type_tuple_cat_type({^^Tuples...}) :];

/**
 * Prepends T to the head of Tuple.
 */
template <template_instance_of<type_tuple> Tuple, class T>
using type_tuple_push_front_t =
  [: impl::make_type_tuple_push_front_type(^^Tuple, ^^T) :];

/**
 * Appends T to the tail of Tuple.
 */
template <template_instance_of<type_tuple> Tuple, class T>
using type_tuple_push_back_t =
  [: impl::make_type_tuple_push_back_type(^^Tuple, ^^T) :];

/**
 * Makes type_tuple<T, T, ...> with T repeated N times.
 */
template <class T, size_t N>
using type_tuple_repeat_t =
  [: substitute(^^type_tuple, std::views::repeat(^^T, N)) :];

template <template <class...> class Traits, class TypeTuple>
struct type_tuple_apply {
  static_assert(false, "The 2nd parameter is not an instance of type_tuple.");
};

template <template <class...> class Traits, class... Args>
struct type_tuple_apply<Traits, type_tuple<Args...>> : Traits<Args...> {};

/**
 * Extracts Traits<Args...>::type where TypeTuple = type_tuple<Args...>.
 */
template <template <class...> class Traits,
          template_instance_of<type_tuple> TypeTuple>
using type_tuple_apply_t = typename type_tuple_apply<Traits, TypeTuple>::type;

/**
 * Extracts Traits<Args...>::value where TypeTuple = type_tuple<Args...>.
 */
template <template <class...> class Traits,
          template_instance_of<type_tuple> TypeTuple>
constexpr auto type_tuple_apply_v = type_tuple_apply<Traits, TypeTuple>::value;

/**
 * Instantiates Template<Args...> where TypeTuple = type_tuple<Args...>.
 */
template <template <class...> class Template,
          template_instance_of<type_tuple> TypeTuple>
using type_tuple_substitute_t =
  [: substitute(^^Template, template_arguments_of(^^TypeTuple)) :];

namespace impl {
consteval bool type_tuple_is_invocable_generic(
  std::meta::info pred_v, std::meta::info Func, std::meta::info Tuple)
{
  auto params = std::vector<std::meta::info>{Func};
  params.append_range(template_arguments_of(remove_cvref(Tuple)));
  return extract<bool>(substitute(pred_v, params));
}

consteval bool type_tuple_is_invocable_r_generic(
  std::meta::info pred_v, std::meta::info R, std::meta::info Func,
  std::meta::info Tuple)
{
  auto params = std::vector<std::meta::info>{R, Func};
  params.append_range(template_arguments_of(remove_cvref(Tuple)));
  return extract<bool>(substitute(pred_v, params));
}

template <template <class> class Traits, class TypeTuple>
struct type_tuple_transform_to_type {
  static_assert(false, "The 2nd parameter is not an instance of type_tuple.");
};

template <template <class> class Traits, class... Args>
struct type_tuple_transform_to_type<Traits, type_tuple<Args...>> {
  using type = type_tuple<typename Traits<Args>::type...>;
};

template <template <class> class Traits, class TypeTuple>
struct type_tuple_transform_to_value {
  static_assert(false, "The 2nd parameter is not an instance of type_tuple.");
};

template <template <class> class Traits, class... Args>
struct type_tuple_transform_to_value<Traits, type_tuple<Args...>> {
  static constexpr auto value = constant<Traits<Args>::value...>{};
};
} // namespace impl

/**
 * Equivalent to std::is_invocable_v<Func, Args...> where
 * TypeTuple = type_tuple<Args...>.
 */
template <class Func, template_instance_of<type_tuple> TypeTuple>
constexpr auto type_tuple_is_invocable_v =
  impl::type_tuple_is_invocable_generic(
    ^^std::is_invocable_v, ^^Func, ^^TypeTuple);

/**
 * Equivalent to std::is_nothrow_invocable_v<Func, Args...> where
 * TypeTuple = type_tuple<Args...>.
 */
template <class Func, template_instance_of<type_tuple> TypeTuple>
constexpr auto type_tuple_is_nothrow_invocable_v =
  impl::type_tuple_is_invocable_generic(
    ^^std::is_nothrow_invocable_v, ^^Func, ^^TypeTuple);

/**
 * Equivalent to std::is_invocable_r_v<R, Func, Args...> where
 * TypeTuple = type_tuple<Args...>.
 */
template <class R, class Func, template_instance_of<type_tuple> TypeTuple>
constexpr auto type_tuple_is_invocable_r_v =
  impl::type_tuple_is_invocable_r_generic(
    ^^std::is_invocable_r_v, ^^R, ^^Func, ^^TypeTuple);

/**
 * Equivalent to std::is_nothrow_invocable_r_v<R, Func, Args...> where
 * TypeTuple = type_tuple<Args...>.
 */
template <class R, class Func, template_instance_of<type_tuple> TypeTuple>
constexpr auto type_tuple_is_nothrow_invocable_r_v =
  impl::type_tuple_is_invocable_r_generic(
    ^^std::is_nothrow_invocable_r_v, ^^R, ^^Func, ^^TypeTuple);

/**
 * Makes type_tuple<Ts...> where
 *   Ts...[i] = typename Traits<Args...[i]>::type
 *   TypeTuple = type_tuple<Args...>
 */
template <template <class> class Traits, class TypeTuple>
using type_tuple_transform_t =
  typename impl::type_tuple_transform_to_type<Traits, TypeTuple>::type;

/**
 * Makes constant<Vs...> where
 *   Vs...[i] = Traits<Args...[i]>::value
 *   TypeTuple = type_typle<Args...>
 */
template <template <class> class Traits, class TypeTuple>
constexpr auto type_tuple_transform_v =
  impl::type_tuple_transform_to_value<Traits, TypeTuple>::value;
} // namespace reflect_cpp26

// Note: Despite std::tuple_size and std::tuple_element being specialized,
// type_tuple is not tuple-like since getters are not defined.

template <class... Args>
struct std::tuple_size<reflect_cpp26::type_tuple<Args...>>
  : std::integral_constant<size_t, sizeof...(Args)> {};

template <size_t I, class... Args>
struct std::tuple_element<I, reflect_cpp26::type_tuple<Args...>> {
  using type = Args...[I];
};

#endif // REFLECT_CPP26_TYPE_TRAITS_TYPE_TUPLE_HPP
