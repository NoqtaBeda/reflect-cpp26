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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_COMPARISON_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_COMPARISON_HPP

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
    requires(const T& t, const U& u) { LeafComparator::operator()(t, u); };

template <class LeafComparator, class T, class U>
constexpr bool is_generic_comparable_with();

template <class LeafComparator, class T, class U>
constexpr auto is_generic_comparable_with_v = is_generic_comparable_with<LeafComparator, T, U>();

template <class LeafComparator, class T, class U>
constexpr bool is_generic_comparable_with() {
  if constexpr (std::is_array_v<T>) {
    // Case (1.1): For C-style arrays:
    // if one of T and U is C-style array, then the other must be a range
    // (including another C-style array).
    // Case 1 is placed before direct comparison to disable the error-prone case
    // where one operand is array and the other is or converts to pointer.
    if constexpr (std::ranges::input_range<U>) {
      return is_generic_comparable_with_v<LeafComparator,
                                          std::remove_extent_t<T>,
                                          std::ranges::range_value_t<U>>;
    } else {
      return false;
    }
  } else if constexpr (std::is_array_v<U>) {
    // Case (1.2): Similar to above.
    if constexpr (std::ranges::input_range<T>) {
      return is_generic_comparable_with_v<LeafComparator,
                                          std::ranges::range_value_t<T>,
                                          std::remove_extent_t<U>>;
    } else {
      return false;
    }
  } else if constexpr (is_directly_comparable_with_v<LeafComparator, T, U>) {
    // Case (2): Directly comparable.
    return true;
  } else if constexpr (c_style_string<T>) {
    // Case (3.1): For C-style strings (CharT* or const CharT*):
    // the other operand must be a character range.
    // For the case of const CharT* vs. const CharU*,
    // If CharT and CharU are the same,
    //   it's allowed by case (2) to keep consistency with C/C++ language behavior,
    //   though it may be confusing and error-prone in practice.
    // Otherwise, comparison is disallowed.
    if constexpr (std::ranges::input_range<U>) {
      return char_type<std::ranges::range_value_t<U>>;
    } else {
      return false;
    }
  } else if constexpr (c_style_string<U>) {
    // Case (3.2): Similar to above
    if constexpr (std::ranges::input_range<T>) {
      return char_type<std::ranges::range_value_t<T>>;
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
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      using EIT = std::tuple_element_t<I, T>;
      using EIU = std::tuple_element_t<I, U>;
      if (!is_generic_comparable_with_v<LeafComparator, EIT, EIU>) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

template <class Derived, class LeafComparator>
struct generic_comparison_interface {
  static constexpr auto leaf_comp = LeafComparator{};

  template <class T, class U>
    requires(is_generic_comparable_with_v<LeafComparator, T, U>)
  static constexpr auto do_compare(const T& t, const U& u) {
    if constexpr (any_of_v<std::is_array, T, U>) {
      // Case (1): C-style arrays. The other operand is ensured to be range.
      return Derived::compare_range(t, u);
    } else if constexpr (c_style_string<T> && !c_style_string<U>) {
      // Case (3.1): C-style string comparison, where U is ensured to be range.
      auto sv = std::basic_string_view<char_type_t<T>>{};
      if (t != nullptr) {
        sv = t;
      }
      return Derived::compare_range(sv, u);
    } else if constexpr (c_style_string<U> && !c_style_string<T>) {
      // Case (3.2): C-style string comparison, where T is ensured to be range.
      auto sv = std::basic_string_view<char_type_t<U>>{};
      if (u != nullptr) {
        sv = u;
      }
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
    requires(is_generic_comparable_with_v<LeafComparator, T, U>)
  static constexpr auto operator()(const T& t, const U& u) {
    return do_compare(t, u);
  }

  template <class T, class U>
    requires(is_generic_comparable_with_v<LeafComparator, std::initializer_list<T>, U>)
  static constexpr auto operator()(std::initializer_list<T> t, const U& u) {
    return do_compare(t, u);
  }

  template <class T, class U>
    requires(is_generic_comparable_with_v<LeafComparator, T, std::initializer_list<U>>)
  static constexpr auto operator()(const T& t, std::initializer_list<U> u) {
    return do_compare(t, u);
  }

  template <class T, class U>
    requires(is_generic_comparable_with_v<LeafComparator, T, U>)
  static constexpr auto operator()(std::initializer_list<T> t, std::initializer_list<U> u) {
    return do_compare(t, u);
  }
};
}  // namespace impl

struct generic_equal_t : impl::generic_comparison_interface<generic_equal_t, equal_t> {
  template <class T, class U>
  static constexpr auto compare_range(const T& t, const U& u) -> bool {
    if (are_forward_ranges_v<T, U> && std::ranges::distance(t) != std::ranges::distance(u)) {
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
  static constexpr auto compare_tuple_like(const T& t, const U& u) -> bool {
    constexpr auto N = std::tuple_size_v<T>;
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if (!operator()(get_ith_element<I>(t), get_ith_element<I>(u))) {
        return false;
      }
    }
    return true;
  }
};

struct generic_not_equal_t : impl::generic_comparison_interface<generic_not_equal_t, not_equal_t> {
  template <class T, class U>
  static constexpr auto compare_range(const T& t, const U& u) -> bool {
    if (are_forward_ranges_v<T, U> && std::ranges::distance(t) != std::ranges::distance(u)) {
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
  static constexpr auto compare_tuple_like(const T& t, const U& u) -> bool {
    constexpr auto N = std::tuple_size_v<T>;
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if (operator()(get_ith_element<I>(t), get_ith_element<I>(u))) {
        return true;  // I-th elements are not equal
      }
    }
    return false;
  }
};

struct generic_compare_three_way_t
    : impl::generic_comparison_interface<generic_compare_three_way_t, compare_three_way_t> {
private:
  template <class T, class U>
  struct compare_tuple_like_result {
    static consteval auto get_common_type() -> std::meta::info {
      constexpr auto N = std::tuple_size_v<T>;
      auto results = std::vector<std::meta::info>();
      results.reserve(N);
      template for (constexpr auto I : std::views::iota(0zU, N)) {
        using R = decltype(operator()(std::declval<std::tuple_element_t<I, T>>(),
                                      std::declval<std::tuple_element_t<I, U>>()));
        results.push_back(^^R);
      }
      return substitute(^^std::common_type_t, results);
    }

    using type = [:get_common_type():];
  };

public:
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
    constexpr auto N = std::tuple_size_v<T>;
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      auto res = operator()(get_ith_element<I>(t), get_ith_element<I>(u));
      if (res != std::partial_ordering::equivalent) {
        return static_cast<ResultT>(res);
      }
    }
    return static_cast<ResultT>(std::strong_ordering::equal);
  }
};

/**
 * Generic comparison. Let op be the underlying comparator,
 * then comparison result of t and u is:
 * (1) If at least one of t and u is reference to C-style array, go to (3);
 * (2) If op(t, u) is supported directly, then return op(t, u);
 * (3) If exactly one of t or u is C-style string (i.e. const CharT*), then
 *     string comparison is applied;
 * (4) If both t and u are input ranges, then return the lexicographical
 *     comparison result, where each pair of elements are compared via
 *     generic comparator (i.e. generic comparison is a recursive procedure);
 * (5) If both t and u are tuple-like types with the same size, then return
 *     the lexicographical comparison result of tuple elements via
 *     generic comparator. 3-way comparison takes the common type as result;
 * Otherwise, invoking generic comparator is ill-formed.
 *
 * Be careful with C-style arrays:
 * (1) If both t and u are references to C-style arrays, then lexicographical
 *     comparison is supported, same as case (3) above;
 * (2) If exactly one of t and u is reference to C-style array, then the other
 *     operand must be an input range whose elements are comparable with those
 *     in the C-style array, otherwise invocation is ill-formed.
 *
 * Also be careful with C-style strings:
 * (1) If both t and u are C-style strings (i.e. const CharT*), then pointer
 *     comparison is applied with builtin operator== for pointers, same as
 *     case (1) above.
 */
constexpr auto generic_equal = generic_equal_t{};
constexpr auto generic_not_equal = generic_not_equal_t{};
constexpr auto generic_compare_three_way = generic_compare_three_way_t{};

/**
 * Whether generic_equal(t, u) is well-defined (see above).
 */
template <class T, class U>
constexpr auto is_generic_equal_comparable_v =
    requires(const T& t, const U& u) { generic_equal(t, u); };

/**
 * Whether generic_not_equal(t, u) is well-defined (see above).
 */
template <class T, class U>
constexpr auto is_generic_not_equal_comparable_v =
    requires(const T& t, const U& u) { generic_not_equal(t, u); };

/**
 * Whether generic_compare_three_way(t, u) is well-defined (see above).
 */
template <class T, class U>
constexpr auto is_generic_three_way_comparable_v =
    requires(const T& t, const U& u) { generic_compare_three_way(t, u); };

/**
 * concept generic_equal_comparable_with<T, U>
 * concept generic_not_equal_comparable_with<T, U>
 * concept generic_three_way_comparable_with<T, U>
 */
REFLECT_CPP26_COMPARISON_CONCEPT(generic_equal)
REFLECT_CPP26_COMPARISON_CONCEPT(generic_not_equal)
REFLECT_CPP26_COMPARISON_CONCEPT(generic_three_way)
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_COMPARISON_HPP
