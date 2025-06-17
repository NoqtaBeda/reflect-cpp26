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

#ifndef REFLECT_CPP26_UTILS_FUNCTIONAL_HPP
#define REFLECT_CPP26_UTILS_FUNCTIONAL_HPP

#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/compare.hpp>
#include <reflect_cpp26/utils/utility.hpp>
#include <functional>
#include <type_traits>

namespace reflect_cpp26 {
// -------- Extension of generic comparison --------
// (1) Less constraints than std::ranges::less, etc. and std::compare_three_way.
// (2) Uses integral comparison above if both operands are integral types.

template <class T, class U>
constexpr auto is_less_comparable_v = is_operator_lt_comparable_v<T, U>;

struct less_t {
  template <class T, class U>
    requires (is_less_comparable_v<T, U>)
  static constexpr bool operator()(const T& t, const U& u) noexcept
  {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
      return cmp_less(t, u);
    } else {
      return t < u;
    }
  }
};

template <class T, class U>
constexpr auto is_greater_comparable_v =
     is_operator_gt_comparable_v<T, U>
  || is_operator_lt_comparable_v<U, T>;

struct greater_t {
  template <class T, class U>
    requires (is_greater_comparable_v<T, U>)
  static constexpr bool operator()(const T& t, const U& u) noexcept
  {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
      return cmp_greater(t, u);
    } else if constexpr (is_operator_gt_comparable_v<T, U>) {
      return t > u;
    } else { // if constexpr (is_operator_lt_comparable_v<U, T>)
      return u < t;
    }
  }
};

template <class T, class U>
constexpr auto is_less_equal_comparable_v =
     is_operator_le_comparable_v<T, U>
  || is_operator_lt_comparable_v<T, U> && is_operator_eq_comparable_v<T, U>;

struct less_equal_t {
  // Note: !(u < t) may be incorrect behavior for partial ordering.
  template <class T, class U>
    requires (is_less_equal_comparable_v<T, U>)
  static constexpr bool operator()(const T& t, const U& u) noexcept
  {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
      return cmp_less_equal(t, u);
    } else if constexpr (is_operator_le_comparable_v<T, U>) {
      return t <= u;
    } else {
      return t < u || t == u;
    }
  }
};

template <class T, class U>
constexpr auto is_greater_equal_comparable_v =
     is_operator_ge_comparable_v<T, U>
  || is_operator_le_comparable_v<U, T>
  || is_operator_gt_comparable_v<T, U> && is_operator_eq_comparable_v<T, U>
  || is_operator_lt_comparable_v<U, T> && is_operator_eq_comparable_v<U, T>;

struct greater_equal_t {
  // Note: !(t < u) may be incorrect behavior for partial ordering
  template <class T, class U>
    requires (is_greater_equal_comparable_v<T, U>)
  static constexpr bool operator()(const T& t, const U& u) noexcept
  {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
      return cmp_greater_equal(t, u);
    } else if constexpr (is_operator_ge_comparable_v<T, U>) {
      return t >= u;
    } else if constexpr (is_operator_le_comparable_v<U, T>) {
      return u <= t;
    } else if constexpr (is_operator_gt_comparable_v<T, U>
                      && is_operator_eq_comparable_v<T, U>) {
      return t > u || t >= u;
    } else {
      return u < t || u == t;
    }
  }
};

template <class T, class U>
constexpr auto is_equal_comparable_v = is_operator_eq_comparable_v<T, U>;

struct equal_t {
  template <class T, class U>
    requires (is_equal_comparable_v<T, U>)
  static constexpr bool operator()(const T& t, const U& u) noexcept
  {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
      return cmp_equal(t, u);
    } else {
      return t == u;
    }
  }
};

template <class T, class U>
constexpr auto is_not_equal_comparable_v =
     is_operator_ne_comparable_v<T, U>
  || is_operator_eq_comparable_v<T, U>;

struct not_equal_t {
  template <class T, class U>
    requires (is_not_equal_comparable_v<T, U>)
  static constexpr bool operator()(const T& t, const U& u) noexcept
  {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
      return cmp_not_equal(t, u);
    } else if constexpr (is_operator_ne_comparable_v<T, U>) {
      return t != u;
    } else { // if constexpr (is_operator_eq_comparable_v<T, U>)
      return !(t == u);
    }
  }
};

// Special rule for integral types with different signedness
template <class T, class U>
constexpr auto is_compare_three_way_comparable_v =
     std::is_integral_v<T> && std::is_integral_v<U>
  || is_operator_3way_comparable_v<T, U>
  || is_less_comparable_v<T, U>
        && is_greater_comparable_v<T, U>
        && is_equal_comparable_v<T, U>;

struct compare_three_way_t {
  // Note: For types where operator<=> is not supported,
  // the most conservative comparison policy std::partial_ordering is used.
  template <class T, class U>
  static constexpr auto do_indirect_compare(const T& t, const U& u) noexcept
    -> std::partial_ordering
  {
    if (less_t::operator()(t, u)) {
      return std::partial_ordering::less;
    }
    if (greater_t::operator()(t, u)) {
      return std::partial_ordering::greater;
    }
    if (equal_t::operator()(t, u)) {
      return std::partial_ordering::equivalent;
    }
    return std::partial_ordering::unordered;
  }

  template <class T, class U>
    requires (is_compare_three_way_comparable_v<T, U>)
  static constexpr auto operator()(const T& t, const U& u) noexcept
  {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
      return cmp_three_way(t, u);
    } else if constexpr (is_operator_3way_comparable_v<T, U>) {
      return t <=> u;
    } else {
      return do_indirect_compare(t, u);
    }
  }
};

constexpr auto less = less_t{};
constexpr auto greater = greater_t{};
constexpr auto less_equal = less_equal_t{};
constexpr auto greater_equal = greater_equal_t{};
constexpr auto equal = equal_t{};
constexpr auto not_equal = not_equal_t{};
constexpr auto compare_three_way = compare_three_way_t{};

REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(less)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(greater)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(less_equal)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(greater_equal)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(equal)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(not_equal)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(compare_three_way)

template <size_t I>
struct get_ith_element_t {
  template <tuple_like T>
    requires (I < std::tuple_size_v<std::remove_cvref_t<T>>)
  static constexpr decltype(auto) operator()(T&& tuple)
  {
    constexpr auto has_free_get =
      requires { get<I>(std::forward<T>(tuple)); };
    constexpr auto has_member_get =
      requires { std::forward<T>(tuple).template get<I>(); };

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

constexpr auto get_first = get_ith_element_t<0>{};
constexpr auto get_second = get_ith_element_t<1>{};
template <size_t I>
constexpr auto get_ith_element = get_ith_element_t<I>{};
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_FUNCTIONAL_HPP
