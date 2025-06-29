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

#ifndef REFLECT_CPP26_TYPE_TRAITS_ARITHMETIC_TYPES_HPP
#define REFLECT_CPP26_TYPE_TRAITS_ARITHMETIC_TYPES_HPP

// Root header: Include only:
// (1) C++ stdlib; (2) utils/config.h; (3) Other root headers
#include <reflect_cpp26/utils/config.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace reflect_cpp26 {
namespace impl {
template <class T>
struct is_char_type_impl : std::false_type {};

template <>
struct is_char_type_impl<char> : std::true_type {};
template <>
struct is_char_type_impl<wchar_t> : std::true_type {};
template <>
struct is_char_type_impl<char8_t> : std::true_type {};
template <>
struct is_char_type_impl<char16_t> : std::true_type {};
template <>
struct is_char_type_impl<char32_t> : std::true_type {};
} // namespace impl

/**
 * Whether T is a (possibly cv-qualified) character type.
 * All candidates character types are listed below.
 *
 * Note: signed char and unsigned char match integer_type instead of char_type
 * since their semantics are designed to be a character's
 * integral representation rather than the character itself
 * according to C++ standard.
 */
template <class T>
constexpr bool is_char_type_v =
  impl::is_char_type_impl<std::remove_cv_t<T>>::value;

template <class T>
concept char_type = is_char_type_v<T>;

/**
 * Whether T is a (possibly cv-qualified) integral type which is not bool.
 */
template <class T>
constexpr bool is_non_bool_integral_v = std::is_integral_v<T>
  && !std::is_same_v<std::remove_cv_t<T>, bool>;

template <class T>
concept non_bool_integral = is_non_bool_integral_v<T>;

/**
 * Whether T is a (possibly cv-qualified) integer type.
 */
template <class T>
constexpr bool is_integer_type_v = is_non_bool_integral_v<T>
  && !is_char_type_v<T>;

template <class T>
concept integer_type = is_integer_type_v<T>;

namespace impl {
consteval auto integral_to_integer_impl(std::meta::info T) -> std::meta::info
{
  switch (size_of(T)) {
    case 1:
      return is_signed_type(T) ? ^^int8_t : ^^uint8_t;
    case 2:
      return is_signed_type(T) ? ^^int16_t : ^^uint16_t;
    case 4:
      return is_signed_type(T) ? ^^int32_t : ^^uint32_t;
    case 8:
      return is_signed_type(T) ? ^^int64_t : ^^uint64_t;
    default:
      compile_error("Unsupported type.");
      return std::meta::info{};
  }
}
} // namespace impl

/**
 * Transforms an arbitrary integral type (including bool and characters)
 * to integer type with the same size and signedness
 */
template <std::integral T>
using integral_to_integer_t = [: impl::integral_to_integer_impl(^^T) :];
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_ARITHMETIC_TYPES_HPP
