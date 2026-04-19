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

#ifndef REFLECT_CPP26_UTILS_FUNCTIONAL_TUPLE_HPP
#define REFLECT_CPP26_UTILS_FUNCTIONAL_TUPLE_HPP

#include <ranges>
#include <reflect_cpp26/utils/compare_tuple.hpp>
#include <reflect_cpp26/utils/functional.hpp>

namespace reflect_cpp26 {
// -------- Generic tuple-like access --------

template <size_t I>
struct get_ith_element_t {
  template <class T>
    requires(tuple_like<std::remove_cvref_t<T>> && I < std::tuple_size_v<std::remove_cvref_t<T>>)
  static constexpr decltype(auto) operator()(T&& tuple) {
    constexpr auto has_free_get = requires { get<I>(std::forward<T>(tuple)); };
    constexpr auto has_member_get = requires { std::forward<T>(tuple).template get<I>(); };

    if constexpr (has_free_get) {
      return get<I>(std::forward<T>(tuple));
    } else if constexpr (has_member_get) {
      return std::forward<T>(tuple).template get<I>();
    } else {
      static_assert(false, "Not tuple-like.");
    }
  }
};

using get_first_t = get_ith_element_t<0>;
using get_second_t = get_ith_element_t<1>;

/**
 * Gets the first tuple element.
 */
constexpr auto get_first = get_ith_element_t<0>{};
/**
 * Gets the second tuple element.
 */
constexpr auto get_second = get_ith_element_t<1>{};
/**
 * Gets the i-th tuple element.
 */
template <size_t I>
constexpr auto get_ith_element = get_ith_element_t<I>{};

// -------- Generic tuple-like comparison --------

template <class T, class U>
constexpr auto is_elementwise_less_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_less_comparable_v);

struct elementwise_less_t {
  // Partial ordering: returns true only if get<I>(t) < get<I>(u) for every index I
  template <class T, class U>
    requires(is_elementwise_less_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) -> bool {
    template for (constexpr auto I : std::views::iota(0zU, std::tuple_size_v<T>)) {
      const auto& a = get_ith_element<I>(t);
      const auto& b = get_ith_element<I>(u);
      if (!less(a, b)) return false;
    }
    return true;
  }
};

template <class T, class U>
constexpr auto is_elementwise_greater_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_greater_comparable_v);

struct elementwise_greater_t {
  // Partial ordering: returns true only if get<I>(t) > get<I>(u) for every index I
  template <class T, class U>
    requires(is_elementwise_greater_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) -> bool {
    template for (constexpr auto I : std::views::iota(0zU, std::tuple_size_v<T>)) {
      const auto& a = get_ith_element<I>(t);
      const auto& b = get_ith_element<I>(u);
      if (!greater(a, b)) return false;
    }
    return true;
  }
};

template <class T, class U>
constexpr auto is_elementwise_less_equal_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_less_equal_comparable_v);

struct elementwise_less_equal_t {
  // Partial ordering: returns true only if get<I>(t) <= get<I>(u) for every index I
  template <class T, class U>
    requires(is_elementwise_less_equal_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) -> bool {
    template for (constexpr auto I : std::views::iota(0zU, std::tuple_size_v<T>)) {
      const auto& a = get_ith_element<I>(t);
      const auto& b = get_ith_element<I>(u);
      if (!less_equal(a, b)) return false;
    }
    return true;
  }
};

template <class T, class U>
constexpr auto is_elementwise_greater_equal_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_greater_equal_comparable_v);

struct elementwise_greater_equal_t {
  // Partial ordering: returns true only if get<I>(t) >= get<I>(u) for every index I
  template <class T, class U>
    requires(is_elementwise_greater_equal_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) -> bool {
    template for (constexpr auto I : std::views::iota(0zU, std::tuple_size_v<T>)) {
      const auto& a = get_ith_element<I>(t);
      const auto& b = get_ith_element<I>(u);
      if (!greater_equal(a, b)) return false;
    }
    return true;
  }
};

template <class T, class U>
constexpr auto is_elementwise_equal_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_equal_comparable_v);

struct elementwise_equal_t {
  // returns true only if get<I>(t) == get<I>(u) for every index I
  template <class T, class U>
    requires(is_elementwise_equal_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) -> bool {
    template for (constexpr auto I : std::views::iota(0zU, std::tuple_size_v<T>)) {
      const auto& a = get_ith_element<I>(t);
      const auto& b = get_ith_element<I>(u);
      if (!equal(a, b)) return false;
    }
    return true;
  }
};

template <class T, class U>
constexpr auto is_elementwise_not_equal_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_not_equal_comparable_v);

struct elementwise_not_equal_t {
  // returns true if get<I>(t) != get<I>(u) for ANY index I
  template <class T, class U>
    requires(is_elementwise_not_equal_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) -> bool {
    template for (constexpr auto I : std::views::iota(0zU, std::tuple_size_v<T>)) {
      const auto& a = get_ith_element<I>(t);
      const auto& b = get_ith_element<I>(u);
      if (not_equal(a, b)) return true;
    }
    return false;
  }
};

template <class T, class U>
constexpr auto is_elementwise_compare_three_way_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_compare_three_way_comparable_v);

struct elementwise_compare_three_way_t {
  template <class T, class U>
    requires(is_elementwise_compare_three_way_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) -> std::partial_ordering {
    auto res = std::partial_ordering::equivalent;

    constexpr auto N = std::tuple_size_v<T>;
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      const auto& a = get_ith_element<I>(t);
      const auto& b = get_ith_element<I>(u);
      auto cur_res = compare_three_way(a, b);

      if (cur_res == std::partial_ordering::unordered) {
        return std::partial_ordering::unordered;
      }
      if (cur_res != std::partial_ordering::equivalent) {
        if (res == std::partial_ordering::equivalent) {
          res = cur_res;  // First difference
        } else if (res != cur_res) {
          return std::partial_ordering::unordered;  // One less, another greater
        }
      }
    }
    return res;
  }
};

constexpr auto elementwise_less = elementwise_less_t{};
constexpr auto elementwise_greater = elementwise_greater_t{};
constexpr auto elementwise_less_equal = elementwise_less_equal_t{};
constexpr auto elementwise_greater_equal = elementwise_greater_equal_t{};
constexpr auto elementwise_equal = elementwise_equal_t{};
constexpr auto elementwise_not_equal = elementwise_not_equal_t{};
constexpr auto elementwise_compare_three_way = elementwise_compare_three_way_t{};

}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_FUNCTIONAL_TUPLE_HPP
