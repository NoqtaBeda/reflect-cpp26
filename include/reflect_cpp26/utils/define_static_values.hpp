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

#ifndef REFLECT_CPP26_UTILS_DEFINE_STATIC_VALUES_HPP
#define REFLECT_CPP26_UTILS_DEFINE_STATIC_VALUES_HPP

#include <iterator>
#include <ranges>
#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/type_traits/type_comparison.hpp>
#include <reflect_cpp26/utils/config.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>

namespace reflect_cpp26 {
namespace impl {
template <class T, auto... Vs>
inline constexpr T fixed_array_impl[sizeof...(Vs)] = {Vs...};
}  // namespace impl

/**
 * Alternative to C++26 std::meta::define_static_array.
 */
template <std::ranges::input_range Range>
consteval auto define_static_array(Range&& range) /* -> meta_span<T> */
{
  using T = std::ranges::range_value_t<Range>;
  auto span = std::define_static_array(std::forward<Range>(range));
  return meta_span<T>::from_std_span(span);
}

// Specialization to prevent repeated meta-definition.
template <class T>
consteval auto define_static_array(meta_span<T> range) {
  return range;
}

/**
 * Alternative to C++26 std::meta::reflect_constant_string with the following
 * differences:
 * (1) Extends to all character types, instead of char and char8_t only which
 *     are supported by C++ standard;
 * (2) Forced check to ensure that no content follows the terminator character
 *     '\0' (if exists) to prevent unexpected behavior.
 * Supported overloads:
 * (1) Input is std::ranges::input_range;
 * (2) Input is CharT[];
 * (3) Input is const CharT*.
 */
template <std::ranges::input_range Range>
  requires(char_type<std::ranges::range_value_t<Range>>)
consteval auto reflect_constant_string(Range&& range) -> std::meta::info {
  using CharT = std::ranges::range_value_t<Range>;
  auto args = std::vector{^^CharT};
  auto null_found = false;

  for (auto c : range) {
    if (null_found) {
      compile_error("Characters after '\\0' are disallowed.");
    }
    if (c == '\0') {
      null_found = true;
    }
    args.push_back(std::meta::reflect_constant(c));
  }
  if (!null_found) {
    args.push_back(std::meta::reflect_constant(static_cast<CharT>('\0')));
  }
  return substitute(^^impl::fixed_array_impl, args);
}

// Overload (2), which has higher priority than overload (1) above.
template <char_type CharT, size_t N>
consteval auto reflect_constant_string(const CharT (&str)[N]) -> std::meta::info {
  auto span = std::span{str};
  return reflect_cpp26::reflect_constant_string(span);
}

// Overload (3), disambiguation with overload (2).
template <class CStringT>
  requires(std::is_pointer_v<CStringT> && char_type<std::remove_pointer_t<CStringT>>)
consteval auto reflect_constant_string(CStringT str) -> std::meta::info {
  if (str == nullptr) {
    compile_error("nullptr is disallowed.");
  }
  auto sv = std::basic_string_view{str};
  return reflect_cpp26::reflect_constant_string(sv);
}

/**
 * Alternative to C++26 std::meta::define_static_string.
 * It's guaranteed that the resulted meta_string_view is null-terminated,
 * i.e. *end() == '\0'.
 * Supported overloads:
 * (1) Input is std::ranges::input_range;
 * (2) Input is CharT[];
 * (3) Input is const CharT*.
 */
template <std::ranges::input_range Range>
  requires(char_type<std::ranges::range_value_t<Range>>)
consteval auto define_static_string(Range&& range)
/* -> meta_basic_string_view<CharT> */
{
  using CharT = std::ranges::range_value_t<Range>;
  auto arr_refl = reflect_constant_string(std::forward<Range>(range));
  return meta_basic_string_view<CharT>::from_literal(extract<const CharT*>(arr_refl));
}

// Overload (2), which has higher priority than overload (1) above.
template <char_type CharT, size_t N>
consteval auto define_static_string(const CharT (&str)[N]) -> meta_basic_string_view<CharT> {
  auto span = std::span{str};
  return reflect_cpp26::define_static_string(span);
}

// Overload (3), disambiguation with overload (2).
template <class CStringT>
  requires(std::is_pointer_v<CStringT> && char_type<std::remove_pointer_t<CStringT>>)
consteval auto define_static_string(CStringT str)
/* -> meta_basic_string_view<CharT> */
{
  if (str == nullptr) {
    compile_error("nullptr is disallowed.");
  }
  auto sv = std::basic_string_view{str};
  return reflect_cpp26::define_static_string(sv);
}

// Specialization to prevent repeated meta-definition.
template <class T>
consteval auto define_static_string(meta_basic_string_view<T> range) -> meta_basic_string_view<T> {
  // Makes sure the resulted range is null-terminated
  if (*range.end() != '\0') {
    compile_error("Malformed meta_string_view: expects null-terminated.");
  }
  return range;
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_DEFINE_STATIC_VALUES_HPP
