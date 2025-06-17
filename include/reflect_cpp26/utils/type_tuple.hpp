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

#include <reflect_cpp26/utils/constant.hpp>
#include <ranges>
#include <tuple>
#include <type_traits>

namespace reflect_cpp26 {
template <class... Args>
struct type_tuple {
  static constexpr auto size = sizeof...(Args);
};

template <class... Tuples>
struct type_tuple_cat {
  static_assert(!"Both TupleA and TupleB should be instance of type_tuple.");
};

template <class... Args>
struct type_tuple_cat<type_tuple<Args...>> {
  using type = type_tuple<Args...>;
};

template <class... ArgsA, class... ArgsB, class... Args>
struct type_tuple_cat<type_tuple<ArgsA...>, type_tuple<ArgsB...>, Args...> {
  using type = typename type_tuple_cat<
    type_tuple<ArgsA..., ArgsB...>, Args...>::type;
};

/**
 * Concatenates Tuples to a single type_tuple.
 */
template <class... Tuples>
using type_tuple_cat_t = typename type_tuple_cat<Tuples...>::type;

template <class Tuple, class T>
struct type_tuple_push_front {
  static_assert(false, "The 1st parameter is not an instance of type_tuple.");
};

template <class... Args, class T>
struct type_tuple_push_front<type_tuple<Args...>, T> {
  using type = type_tuple<T, Args...>;
};

/**
 * Prepends T to the head of Tuple.
 */
template <class Tuple, class T>
using type_tuple_push_front_t = typename type_tuple_push_front<Tuple, T>::type;

template <class Tuple, class T>
struct type_tuple_push_back {
  static_assert(false, "The 1st parameter is not an instance of type_tuple.");
};

template <class... Args, class T>
struct type_tuple_push_back<type_tuple<Args...>, T> {
  using type = type_tuple<Args..., T>;
};

/**
 * Appends T to the tail of Tuple.
 */
template <class Tuple, class T>
using type_tuple_push_back_t = typename type_tuple_push_back<Tuple, T>::type;

template <class T, size_t N>
struct type_tuple_repeat {
  using prev_type = typename type_tuple_repeat<T, N - 1>::type;
  using type = type_tuple_push_back_t<prev_type, T>;
};

template <class T>
struct type_tuple_repeat<T, 0> {
  using type = type_tuple<>;
};

/**
 * Makes type_tuple<T, T, ...> with T repeated N times.
 */
template <class T, size_t N>
using type_tuple_repeat_t = typename type_tuple_repeat<T, N>::type;

template <template <class...> class Traits, class TypeTuple>
struct type_tuple_apply {
  static_assert(false, "The 2nd parameter is not an instance of type_tuple.");
};

template <template <class...> class Traits, class... Args>
struct type_tuple_apply<Traits, type_tuple<Args...>> : Traits<Args...> {};

/**
 * Extracts Traits<Args...>::type where TypeTuple = type_tuple<Args...>.
 */
template <template <class...> class Traits, class TypeTuple>
using type_tuple_apply_t =
  typename type_tuple_apply<Traits, TypeTuple>::type;

/**
 * Extracts Traits<Args...>::value where TypeTuple = type_tuple<Args...>.
 */
template <template <class...> class Traits, class TypeTuple>
constexpr auto type_tuple_apply_v =
  type_tuple_apply<Traits, TypeTuple>::value;

template <template <class...> class Template, class TypeTuple>
struct type_tuple_substitute {
  static_assert(false, "The 2nd parameter is not an instance of type_tuple.");
};

template <template <class...> class Template, class... Args>
struct type_tuple_substitute<Template, type_tuple<Args...>> {
  using type = Template<Args...>;
};

/**
 * Instantiates Template<Args...> where TypeTuple = type_tuple<Args...>.
 */
template <template <class...> class Template, class TypeTuple>
using type_tuple_substitute_t =
  typename type_tuple_substitute<Template, TypeTuple>::type;

namespace impl {
template <template <class...> class Predicate, class Func, class TypeTuple>
struct type_tuple_is_invocable_generic {
  static_assert(false, "The last parameter is not an instance of type_tuple.");
};

template <template <class...> class Predicate, class Func, class... Args>
struct type_tuple_is_invocable_generic<Predicate, Func, type_tuple<Args...>> {
  static constexpr auto value = Predicate<Func, Args...>::value;
};

template <template <class...> class Predicate,
          class R, class Func, class TypeTuple>
struct type_tuple_is_invocable_r_generic {
  static_assert(false, "The last parameter is not an instance of type_tuple.");
};

template <template <class...> class Predicate,
          class R, class Func, class... Args>
struct type_tuple_is_invocable_r_generic<
  Predicate, R, Func, type_tuple<Args...>> {
  static constexpr auto value = Predicate<R, Func, Args...>::value;
};

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
template <class Func, class TypeTuple>
constexpr auto type_tuple_is_invocable_v =
  impl::type_tuple_is_invocable_generic<
    std::is_invocable, Func, TypeTuple>::value;

/**
 * Equivalent to std::is_nothrow_invocable_v<Func, Args...> where
 * TypeTuple = type_tuple<Args...>.
 */
template <class Func, class TypeTuple>
constexpr auto type_tuple_is_nothrow_invocable_v =
  impl::type_tuple_is_invocable_generic<
    std::is_nothrow_invocable, Func, TypeTuple>::value;

/**
 * Equivalent to std::is_invocable_r_v<R, Func, Args...> where
 * TypeTuple = type_tuple<Args...>.
 */
template <class R, class Func, class TypeTuple>
constexpr auto type_tuple_is_invocable_r_v =
  impl::type_tuple_is_invocable_r_generic<
    std::is_invocable_r, R, Func, TypeTuple>::value;

/**
 * Equivalent to std::is_nothrow_invocable_r_v<R, Func, Args...> where
 * TypeTuple = type_tuple<Args...>.
 */
template <class R, class Func, class TypeTuple>
constexpr auto type_tuple_is_nothrow_invocable_r_v =
  impl::type_tuple_is_invocable_r_generic<
    std::is_nothrow_invocable_r, R, Func, TypeTuple>::value;

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
