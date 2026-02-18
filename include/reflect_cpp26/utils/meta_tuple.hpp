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

#ifndef REFLECT_CPP26_UTILS_META_TUPLE_HPP
#define REFLECT_CPP26_UTILS_META_TUPLE_HPP

#include <ranges>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
template <class... Args>
struct meta_tuple {
  static constexpr auto tuple_size = sizeof...(Args);

  struct underlying_type;
  consteval {
    // clang-format off
    define_aggregate(^^underlying_type, {data_member_spec(^^Args)...});
    // clang-format on
  }
  // values are exposed as public data member to make meta_tuple
  // structural aggregate.
  underlying_type values;

private:
  static constexpr auto get_nth_field(size_t n) -> std::meta::info {
    return all_direct_nonstatic_data_members_v<underlying_type>[n];
  }

public:
  constexpr meta_tuple() = default;

  // cvref dropped during CTAD
  constexpr meta_tuple(const Args&... args) : values{args...} {}

  /**
   * Free get function of meta_tuple.
   */
  template <size_t I>
    requires(I < tuple_size)
  friend constexpr auto& get(meta_tuple& tuple) {
    return tuple.values.[:get_nth_field(I):];
  }

  template <size_t I>
    requires(I < tuple_size)
  friend constexpr const auto& get(const meta_tuple& tuple) {
    return tuple.values.[:get_nth_field(I):];
  }

  template <size_t I>
    requires(I < tuple_size)
  friend constexpr auto&& get(meta_tuple&& tuple) {
    return std::move(tuple.values.[:get_nth_field(I):]);
  }

  template <size_t I>
    requires(I < tuple_size)
  friend constexpr const auto&& get(const meta_tuple&& tuple) {
    return std::move(tuple.values.[:get_nth_field(I):]);
  }
};

// TODO: We need better way to implement operator == and <=>
//       between meta_tuple and other tuple-like types.

namespace impl {
template <class Tuple1, class Tuple2>
constexpr bool is_memberwise_eq_comparable() {
  constexpr auto N = std::tuple_size_v<Tuple1>;
  if constexpr (std::tuple_size_v<Tuple2> == N) {
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      using E1 = std::tuple_element_t<I, Tuple1>;
      using E2 = std::tuple_element_t<I, Tuple2>;
      if (!is_equal_comparable_v<E1, E2>) return false;
    }
    return true;
  } else {
    return false;
  }
}

template <class Tuple1, class Tuple2>
constexpr auto is_memberwise_eq_comparable_v = is_memberwise_eq_comparable<Tuple1, Tuple2>();
}  // namespace impl

template <tuple_like TupleLike, class... Args>
  requires(impl::is_memberwise_eq_comparable_v<meta_tuple<Args...>, TupleLike>)
constexpr bool operator==(const meta_tuple<Args...>& lhs, const TupleLike& rhs) {
  constexpr auto N = sizeof...(Args);
  template for (constexpr auto I : std::views::iota(0zU, N)) {
    const auto& x = get_ith_element<I>(lhs);
    const auto& y = get_ith_element<I>(rhs);
    if (!(x == y)) {
      return false;
    }
  }
  return true;
}

// Deduction guide (cvref dropped, same behavior as std::tuple)
template <class... Args>
meta_tuple(Args...) -> meta_tuple<Args...>;
}  // namespace reflect_cpp26

template <class... Args>
struct std::tuple_size<reflect_cpp26::meta_tuple<Args...>>
    : std::integral_constant<size_t, sizeof...(Args)> {};

template <size_t I, class... Args>
struct std::tuple_element<I, reflect_cpp26::meta_tuple<Args...>> {
  using type = Args...[I];
};

#endif  // REFLECT_CPP26_UTILS_META_TUPLE_HPP
