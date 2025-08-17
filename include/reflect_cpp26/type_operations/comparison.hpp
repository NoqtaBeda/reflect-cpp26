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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_COMPARISON_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_COMPARISON_HPP

#include <reflect_cpp26/type_traits/function_types.hpp>
#include <reflect_cpp26/type_traits/reduction.hpp>
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>
#include <reflect_cpp26/utils/ranges.hpp>

namespace reflect_cpp26 {
namespace impl {
template <class LeafComparator, class T, class U>
constexpr auto is_directly_comparable_with_v =
  requires (const T& t, const U& u) { LeafComparator::operator()(t, u); };

template <class LeafComparator, class T, class U>
constexpr bool is_generic_comparable_with();

template <class LeafComparator, class T, class U>
constexpr auto is_generic_comparable_with_v =
  is_generic_comparable_with<LeafComparator, T, U>();

template <class LeafComparator, class T, class U>
constexpr bool is_generic_comparable_with()
{
  if constexpr (std::is_array_v<T>) {
    // Case (1.1): For C-style arrays:
    // if one of T and U is C-style array, then the other must be a range
    // (including another C-style array).
    // Case 1 is placed before direct comparison to disable the error-prone case
    // where one operand is array and the other is or converts to pointer.
    if constexpr (std::ranges::input_range<U>) {
      return is_generic_comparable_with_v<
        LeafComparator, std::remove_extent_t<T>, std::ranges::range_value_t<U>>;
    } else {
      return false;
    }
  } else if constexpr (std::is_array_v<U>) {
    // Case (1.2): Similar to above.
    if constexpr (std::ranges::input_range<T>) {
      return is_generic_comparable_with_v<
        LeafComparator, std::ranges::range_value_t<T>, std::remove_extent_t<U>>;
    } else {
      return false;
    }
  } else if constexpr (is_directly_comparable_with_v<LeafComparator, T, U>) {
    // Case (2): Directly comparable.
    return true;
  } else if constexpr (is_c_style_string_v<T>) {
    // Case (3.1): For C-style strings (CharT* or const CharT*):
    // the other operand must be a character range.
    // For the case of const CharT* vs. const CharU*,
    // it's allowed by case (2) yet discouraged due to semantic confusion
    // if CharT and CharU are the same; disallowed otherwise.
    if constexpr (std::ranges::input_range<U>) {
      return is_char_type_v<std::ranges::range_value_t<U>>;
    } else {
      return false;
    }
  } else if constexpr (is_c_style_string_v<U>) {
    // Case (3.2): Similar to above
    if constexpr (std::ranges::input_range<T>) {
      return is_char_type_v<std::ranges::range_value_t<T>>;
    } else {
      return false;
    }
  } else if constexpr (are_input_ranges_v<T, U>) {
    // Case (4): For ranges: elementwise comparison is performed.
    using VT = std::ranges::range_value_t<T>;
    using VU = std::ranges::range_value_t<U>;
    return is_generic_comparable_with_v<LeafComparator, VT, VU>;
  } else if constexpr (are_tuple_like_of_same_size_v<T, U>) {
    // Case (5): For tuple-like objects: elementwise comparison is performed.
    constexpr auto N = std::tuple_size_v<T>;
    return REFLECT_CPP26_EXPAND_I(N).all_of([](auto I) {
      using EIT = std::tuple_element_t<I, T>;
      using EIU = std::tuple_element_t<I, U>;
      return is_generic_comparable_with_v<LeafComparator, EIT, EIU>;
    });
  } else {
    return false;
  }
}

template <class Derived, class LeafComparator>
struct generic_comparison_interface {
  static constexpr auto leaf_comp = LeafComparator{};

  template <class T, class U>
    requires (is_generic_comparable_with_v<LeafComparator, T, U>)
  static constexpr auto do_compare(const T& t, const U& u)
  {
    if constexpr (any_of_v<std::is_array, T, U>) {
      // Case (1): C-style arrays. The other operand is ensured to be range.
      return Derived::compare_range(t, u);
    } else if constexpr (is_c_style_string_v<T> && !is_c_style_string_v<U>) {
      // Case (3.1): C-style string comparison, where U is ensured to be range.
      auto sv = std::basic_string_view<char_type_t<T>>{};
      if (t != nullptr) { sv = t; }
      return Derived::compare_range(sv, u);
    } else if constexpr (is_c_style_string_v<U> && !is_c_style_string_v<T>) {
      // Case (3.2): C-style string comparison, where T is ensured to be range.
      auto sv = std::basic_string_view<char_type_t<U>>{};
      if (u != nullptr) { sv = u; }
      return Derived::compare_range(t, sv);
    } else if constexpr (requires { leaf_comp(t, u); }) {
      // Case (2): direct comparison
      return leaf_comp(t, u);
    } else if constexpr (are_input_ranges_v<T, U>) {
      // Case (4): range elementwise comparison
      return Derived::compare_range(t, u);
    } else if constexpr (are_tuple_like_of_same_size_v<T, U>) {
      // Case (5): tuple-like elementwise comparsison
      return Derived::compare_tuple_like(t, u);
    } else {
      static_assert(!"Implementation error: T and U are not comparable.");
    }
  }

  template <class T, class U>
    requires (is_generic_comparable_with_v<LeafComparator, T, U>)
  static constexpr auto operator()(const T& t, const U& u)
  {
    return do_compare(t, u);
  }

  template <class T, class U>
    requires (is_generic_comparable_with_v<
      LeafComparator, std::initializer_list<T>, U>)
  static constexpr auto operator()(std::initializer_list<T> t, const U& u)
  {
    return do_compare(t, u);
  }

  template <class T, class U>
    requires (is_generic_comparable_with_v<
      LeafComparator, T, std::initializer_list<U>>)
  static constexpr auto operator()(const T& t, std::initializer_list<U> u)
  {
    return do_compare(t, u);
  }

  template <class T, class U>
    requires (is_generic_comparable_with_v<LeafComparator, T, U>)
  static constexpr auto operator()(
    std::initializer_list<T> t, std::initializer_list<U> u)
  {
    return do_compare(t, u);
  }
};
} // namespace impl

struct generic_equal_t
  : impl::generic_comparison_interface<generic_equal_t, equal_t>
{
  template <class T, class U>
  static constexpr auto compare_range(const T& t, const U& u) -> bool
  {
    if (are_forward_ranges_v<T, U>
        && std::ranges::distance(t) != std::ranges::distance(u)) {
      return false;
    }
    auto [ti, t_end] = std::ranges::subrange(t);
    auto [ui, u_end] = std::ranges::subrange(u);
    for (; ti != t_end && ui != u_end; ++ti, ++ui) {
      if (!operator()(*ti, *ui)) {
        return false;
      }
    }
    if constexpr (!are_forward_ranges_v<T, U>) {
      return ti == t_end && ui == u_end;
    } else {
      return true;
    }
  }

  template <class T, class U>
  static constexpr auto compare_tuple_like(const T& t, const U& u) -> bool
  {
    return REFLECT_CPP26_EXPAND_I(std::tuple_size_v<T>).all_of(
      [&t, &u](auto I) {
        return operator()(get_ith_element<I>(t), get_ith_element<I>(u));
      });
  }
};

struct generic_not_equal_t
  : impl::generic_comparison_interface<generic_not_equal_t, not_equal_t>
{
  template <class T, class U>
  static constexpr auto compare_range(const T& t, const U& u) -> bool
  {
    if (are_forward_ranges_v<T, U>
        && std::ranges::distance(t) != std::ranges::distance(u)) {
      return true;
    }
    auto [ti, t_end] = std::ranges::subrange(t);
    auto [ui, u_end] = std::ranges::subrange(u);
    for (; ti != t_end && ui != u_end; ++ti, ++ui) {
      if (operator()(*ti, *ui)) {
        return true;
      }
    }
    if constexpr (!are_forward_ranges_v<T, U>) {
      return (ti == t_end) ^ (ui == u_end);
    } else {
      return false;
    }
  }

  template <class T, class U>
  static constexpr auto compare_tuple_like(const T& t, const U& u) -> bool
  {
    return REFLECT_CPP26_EXPAND_I(std::tuple_size_v<T>).any_of(
      [&t, &u](auto I) {
        return operator()(get_ith_element<I>(t), get_ith_element<I>(u));
      });
  }
};

struct generic_compare_three_way_t
  : impl::generic_comparison_interface<
      generic_compare_three_way_t, compare_three_way_t>
{
  template <class T, class U>
  struct compare_tuple_like_result {
    static consteval auto get_common_type() -> std::meta::info
    {
      constexpr auto N = std::tuple_size_v<T>;
      auto results = REFLECT_CPP26_EXPAND_I(N).map([](auto I) {
        using R = decltype(operator()(
          std::declval<std::tuple_element_t<I, T>>(),
          std::declval<std::tuple_element_t<I, U>>()));
        return ^^R;
      });
      return substitute(^^std::common_type_t, results);
    }
    using type = [:get_common_type():];
  };

  template <class T, class U>
  static constexpr auto compare_range(const T& t, const U& u) /* -> ResultT */
  {
    auto [ti, t_end] = std::ranges::subrange(t);
    auto [ui, u_end] = std::ranges::subrange(u);
    using ResultT = decltype(operator()(*ti, *ui));

    for (; ti != t_end && ui != u_end; ++ti, ++ui) {
      auto res = operator()(*ti, *ui);
      if (res != std::strong_ordering::equal) {
        return res;
      }
    }
    auto res = (ti != t_end) <=> (ui != u_end);
    return static_cast<ResultT>(res);
  }

  template <class T, class U>
  static constexpr auto compare_tuple_like(const T& t, const U& u)
    /* -> ResultT */
  {
    using ResultT = typename compare_tuple_like_result<T, U>::type;
    auto res = static_cast<ResultT>(std::strong_ordering::equal);
    REFLECT_CPP26_EXPAND_I(std::tuple_size_v<T>).for_each(
      [&t, &u, &res](auto I) {
        res = operator()(get_ith_element<I>(t), get_ith_element<I>(u));
        return /* continues if */ res == std::strong_ordering::equal;
      });
    return res;
  }
};

/**
 * Generic comparison. Let op be the underlying comparator,
 * then comparison result of t and u is:
 * (1) If op(t, u) is supported directly, then return op(t, u);
 * (2) If both t and u are input ranges, then return the lexicographical
 *     comparison result, where each pair of elements are compared via
 *     generic comparator (i.e. generic comparison is a recursive procedure);
 * (3) If both t and u are tuple-like types with the same size, then return
 *     the lexicographical comparison result of tuple elements via
 *     generic comparator. 3-way comparison takes the common type as result;
 * (4) Otherwise, invoking generic comparator is ill-formed.
 *
 * Be careful with C-style arrays:
 * (1) If both t and u are references to C-style arrays, then lexicographical
 *     comparison is supported;
 * (2) If exactly one of t and u is reference to C-style array, then the other
 *     operand must not be convertible to pointer type, otherwise invocation
 *     is ill-formed.
 */
constexpr auto generic_equal = generic_equal_t{};
constexpr auto generic_not_equal = generic_not_equal_t{};
constexpr auto generic_compare_three_way = generic_compare_three_way_t{};

template <class T, class U>
constexpr auto is_generic_equal_comparable_v =
  requires (const T& t, const U& u) {
    generic_equal(t, u);
    generic_equal(u, t);
  };

template <class T, class U>
constexpr auto is_generic_not_equal_comparable_v =
  requires (const T& t, const U& u) {
    generic_not_equal(t, u);
    generic_not_equal(u, t);
  };

template <class T, class U>
constexpr auto is_generic_three_way_comparable_v =
  requires (const T& t, const U& u) {
    generic_compare_three_way(t, u);
    generic_compare_three_way(u, t);
  };

REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(generic_equal)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(generic_not_equal)
REFLECT_CPP26_UTILITY_MAKE_COMPARISON_TRAITS_TYPE(generic_three_way)
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_OPERATIONS_COMPARISON_HPP
