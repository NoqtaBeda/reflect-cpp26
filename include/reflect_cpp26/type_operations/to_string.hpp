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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_TO_STRING_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_TO_STRING_HPP

#include <reflect_cpp26/enum/enum_flags_name.hpp>
#include <reflect_cpp26/enum/enum_name.hpp>
#include <reflect_cpp26/enum/enum_type_name.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>
#include <reflect_cpp26/utils/to_string.hpp>  // Predefined to_string() functions

namespace reflect_cpp26 {
namespace impl {
template <class T>
consteval bool is_generic_to_string_invocable();
}  // namespace impl

/**
 * Whether to_string(t) is well-defined.
 * to_string is either predefined in namespace reflect_cpp26 (see
 * utils/to_string.hpp for details), or found via argument-depend lookup (ADL).
 */
template <class T>
concept has_to_string = requires(const T& t) {
  { to_string(t) } -> std::convertible_to<std::string>;
};

/**
 * Whether to_display_string(t) is well-defined.
 * to_display_string is either predefined in namespace reflect_cpp26 (see
 * utils/to_string.hpp for details), or found via argument-depend lookup (ADL).
 */
template <class T>
concept has_to_display_string = requires(const T& t) {
  { to_display_string(t) } -> std::convertible_to<std::string>;
};

/**
 * Whether generic_to_string() and generic_to_display_string() are invocable
 * for type T. Details see above.
 */
template <class T>
concept generic_to_string_invocable = impl::is_generic_to_string_invocable<T>();

namespace impl {
template <class T>
consteval bool is_generic_to_string_invocable() {
  if constexpr (has_to_string<T> || std::is_enum_v<T>) {
    return true;
  } else if constexpr (std::ranges::input_range<T>) {
    return generic_to_string_invocable<std::ranges::range_value_t<T>>;
  } else if constexpr (tuple_like<T>) {
    constexpr auto N = std::tuple_size_v<T>;
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if (!generic_to_string_invocable<std::tuple_element_t<I, T>>) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

template <class T>
constexpr auto generic_enum_to_string(T input) -> std::string {
  constexpr auto is_flags_type = std::is_scoped_enum_v<T> && requires(T x, T y) {
    { x | y } -> std::same_as<T>;
  };
  if constexpr (is_flags_type) {
    auto s = enum_flags_name(input);
    if (s.has_value()) {
      return std::move(*s);
    }
  } else {
    auto sv = enum_name(input);
    if (!sv.empty()) {
      return std::string{sv};
    }
  }
  // Fallback: (T)value
  auto res = std::string{"("} + enum_type_name_v<T> + ')';
  res += to_string(std::to_underlying(input));
  return res;
}

template <class ToStringFn, class T>
constexpr auto generic_range_to_string(const T& input) -> std::string {
  auto res = std::string{"["};
  auto index = 0zU;
  for (const auto& cur : input) {
    if (index++ != 0) {
      res += ", ";
    }
    res += ToStringFn::operator()(cur);
  }
  res += ']';
  return res;
}

template <class ToStringFn, class T>
constexpr auto generic_tuple_like_to_string(const T& input) -> std::string {
  constexpr auto N = std::tuple_size_v<T>;
  auto res = std::string{"{"};
  template for (constexpr auto I : std::views::iota(0zU, N)) {
    if constexpr (I != 0) {
      res += ", ";
    }
    res += ToStringFn::operator()(get_ith_element<I>(input));
  }
  res += '}';
  return res;
}

template <class ToStringFn, class T>
constexpr auto generic_to_string(const T& input) -> std::string {
  if constexpr (has_to_string<T>) {
    // (1): Calls to_string(input) directly if supported.
    return to_string(input);
  } else if constexpr (std::is_enum_v<T>) {
    // (2): Calls enum_name(input) or enum_flags_name(input)
    //      if T is an enum type.
    return generic_enum_to_string(input);
  } else if constexpr (std::ranges::input_range<T>) {
    // (3): Calls ToStringFn recursively for each range element in input
    //      if T is a range.
    return generic_range_to_string<ToStringFn>(input);
  } else if constexpr (tuple_like<T>) {
    // (4): Calls ToStringFn recursively for each tuple element of input
    //      if T is a tuple.
    return generic_tuple_like_to_string<ToStringFn>(input);
  } else {
    static_assert(false, "Invalid type.");
  }
}
}  // namespace impl

struct generic_to_display_string_t {
  using self_type = generic_to_display_string_t;

  /**
   * Dumps input value to display-style string.
   * (1) If has_to_display_string<T> is true, then to_display_string(input)
   *     is called;
   * (2) Otherwise, impl::generic_to_string() is used, details see above.
   */
  template <generic_to_string_invocable T>
  static constexpr auto operator()(const T& input) -> std::string {
    if constexpr (has_to_display_string<T>) {
      return to_display_string(input);
    } else {
      return impl::generic_to_string<self_type>(input);
    }
  }

  /**
   * Dumps input value to display-style string if the previous overload is
   * supported, or alt otherwise.
   */
  template <class T>
  static constexpr auto operator()(const T& input, std::string_view alt) -> std::string {
    if constexpr (generic_to_string_invocable<T>) {
      return operator()(input);
    } else {
      return std::string{alt};
    }
  }
};
// Predefined function object
constexpr auto generic_to_display_string = generic_to_display_string_t{};

struct generic_to_string_t {
  using self_type = generic_to_string_t;

  /**
   * Dumps input value to string.
   * See impl::generic_to_string() above for details.
   */
  template <generic_to_string_invocable T>
  static constexpr auto operator()(const T& input) -> std::string {
    return impl::generic_to_string<self_type>(input);
  }

  /**
   * Dumps input value to string.
   * See generic_to_display_string_t::operator() and impl::generic_to_string()
   * above for details.
   */
  template <generic_to_string_invocable T>
  static constexpr auto operator()(const T& input, bool displayed_style) -> std::string {
    return displayed_style ? generic_to_display_string_t::operator()(input)
                           : generic_to_string_t::operator()(input);
  }

  /**
   * Dumps input value to string if the previous overload is supported,
   * or alt otherwise.
   */
  template <class T>
  static constexpr auto operator()(const T& input, std::string_view alt) -> std::string {
    if constexpr (generic_to_string_invocable<T>) {
      return operator()(input);
    } else {
      return std::string{alt};
    }
  }

  /**
   * Dumps input value to string if the previous overload is supported,
   * or alt otherwise.
   */
  template <class T>
  static constexpr auto operator()(const T& input, bool displayed_style, std::string_view alt)
      -> std::string {
    if constexpr (generic_to_string_invocable<T>) {
      return operator()(input, displayed_style);
    } else {
      return std::string{alt};
    }
  }
};
// Predefined function object
constexpr auto generic_to_string = generic_to_string_t{};
};  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_TO_STRING_HPP
