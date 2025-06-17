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

#ifndef REFLECT_CPP26_UTILS_META_TUPLE_HPP
#define REFLECT_CPP26_UTILS_META_TUPLE_HPP

// Note: Beware circular dependency when including type_traits/* headers.
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <ranges>

namespace reflect_cpp26 {
template <class... Args>
struct meta_tuple {
  static_assert(sizeof...(Args) >= 1,
    "At least 1 template parameter required.");
  static constexpr auto tuple_size = sizeof...(Args);

  struct underlying_type;
  consteval {
    define_aggregate(^^underlying_type, {data_member_spec(^^Args)...});
  }
  // values are exposed as public data member to make meta_tuple
  // structured aggregate.
  underlying_type values;

private:
  static constexpr auto get_nth_field(size_t n) {
    return all_direct_nsdm_of(^^underlying_type)[n];
  }

public:
  constexpr meta_tuple() = default;

  // cvref dropped during CTAD
  constexpr meta_tuple(const Args&... args) : values{args...} {}

  template <class TupleLike>
  constexpr auto& operator=(TupleLike&& tuple)
  {
    constexpr auto members = all_direct_nsdm_of(^^underlying_type);
    REFLECT_CPP26_EXPAND(members).for_each(
      [this, &tuple](auto index, auto member) {
        values.[:member:] =
          get_ith_element<index>(std::forward<TupleLike>(tuple));
      });
    return *this;
  }

  /**
   * Free get function of meta_tuple.
   */
  template <size_t I>
    requires (I < tuple_size)
  friend constexpr auto& get(meta_tuple& tuple) {
    return tuple.values.[: get_nth_field(I) :];
  }

  template <size_t I>
    requires (I < tuple_size)
  friend constexpr const auto& get(const meta_tuple& tuple) {
    return tuple.values.[: get_nth_field(I) :];
  }

  template <size_t I>
    requires (I < tuple_size)
  friend constexpr auto&& get(meta_tuple&& tuple) {
    return std::move(tuple.values.[: get_nth_field(I) :]);
  }

  template <size_t I>
    requires (I < tuple_size)
  friend constexpr const auto&& get(const meta_tuple&& tuple) {
    return std::move(tuple.values.[: get_nth_field(I) :]);
  }
};

// Deduction guide (cvref dropped, same behavior as std::tuple)
template <class... Args>
meta_tuple(Args...) -> meta_tuple<Args...>;
} // namespace reflect_cpp26

template <class... Args>
struct std::tuple_size<reflect_cpp26::meta_tuple<Args...>>
  : std::integral_constant<size_t, sizeof...(Args)> {};

template <size_t I, class... Args>
struct std::tuple_element<I, reflect_cpp26::meta_tuple<Args...>> {
  using type = Args...[I];
};

#endif // REFLECT_CPP26_UTILS_META_TUPLE_HPP
