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

#ifndef REFLECT_CPP26_UTILS_TO_STRING_HPP
#define REFLECT_CPP26_UTILS_TO_STRING_HPP

#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <reflect_cpp26/utils/utility.hpp>
#include <bit>
#include <charconv>

namespace reflect_cpp26 {
namespace impl {
constexpr auto byte_to_hex_table_v =
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
  "202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"
  "404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f"
  "606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f"
  "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"
  "a0a1a2a3a4a5a6a7a8a9aaabacadaeafb0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
  "c0c1c2c3c4c5c6c7c8c9cacbcccdcecfd0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
  "e0e1e2e3e4e5e6e7e8e9eaebecedeeeff0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";

constexpr auto to_string(char8_t value, bool quoted = false) -> std::string
{
  if (isprint(value)) {
    if (quoted) {
      auto res = std::string(3zU, '\'');
      res[1] = value;
      return res;
    }
    return std::string(1zU, static_cast<char>(value));
  }
  switch (value) {
    case '\0': return quoted ? "'\\0'" : "\\0";
    case '\t': return quoted ? "'\\t'" : "\\t";
    case '\n': return quoted ? "'\\n'" : "\\n";
    case '\v': return quoted ? "'\\v'" : "\\v";
    case '\f': return quoted ? "'\\f'" : "\\f";
    case '\r': return quoted ? "'\\r'" : "\\r";
    default: break;
  }
  auto res = std::string{quoted ? "'\\x**'" : "\\x**"};
  auto* out = res.data() + quoted + 2; // 2 : Length of backslash and 'x'
  constexpr auto n_xdigits_per_byte = 2;
  std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * value,
              n_xdigits_per_byte, out);
  return res;
}

template <std::endian Endian>
constexpr auto to_string(char16_t value, bool quoted = false) -> std::string
{
  static_assert(sizeof(char16_t) == 2, "Unsupported char16_t");
  if (in_range<char8_t>(value)) {
    return to_string(static_cast<char8_t>(value), quoted);
  }
  auto bytes = std::bit_cast<std::array<uint8_t, 2>>(value);
  auto res = std::string{quoted ? "'\\u****'" : "\\u****"};
  auto* out = res.data() + quoted + 2; // 2 : Length of backslash and 'u'

  constexpr auto n_xdigits_per_byte = 2;
  if constexpr (Endian == std::endian::little) {
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[1],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[0],
                      n_xdigits_per_byte, out);
  } else {
    static_assert(Endian == std::endian::big, "Unknown endian");
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[0],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[1],
                      n_xdigits_per_byte, out);
  }
  return res;
}

template <std::endian Endian>
constexpr auto to_string(char32_t value, bool quoted = false) -> std::string
{
  static_assert(sizeof(char32_t) == 4, "Unsupported char32_t");
  if (in_range<char16_t>(value)) {
    return to_string<Endian>(static_cast<char16_t>(value), quoted);
  }
  auto bytes = std::bit_cast<std::array<uint8_t, 4>>(value);
  auto res = std::string{quoted ? "'\\U********'" : "\\U********"};
  auto* out = res.data() + quoted + 2; // 2 : Length of backslash and 'U'

  constexpr auto n_xdigits_per_byte = 2;
  if constexpr (Endian == std::endian::little) {
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[3],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[2],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[1],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[0],
                      n_xdigits_per_byte, out);
  } else {
    static_assert(Endian == std::endian::big, "Unknown endian.");
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[0],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[1],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[2],
                      n_xdigits_per_byte, out);
    out = std::copy_n(byte_to_hex_table_v + n_xdigits_per_byte * bytes[3],
                      n_xdigits_per_byte, out);
  }
  return res;
}

template <class CharT>
constexpr auto char_to_string_dispatch(CharT value, bool quoted) -> std::string
{
  if constexpr (sizeof(CharT) == 1) {
    return to_string(static_cast<char8_t>(value), quoted);
  } else if constexpr (sizeof(CharT) == 2) {
    return to_string<std::endian::native>(static_cast<char16_t>(value), quoted);
  } else {
    static_assert(sizeof(CharT) == 4, "Unknown char type.");
    return to_string<std::endian::native>(static_cast<char32_t>(value), quoted);
  }
}

template <class... Args>
constexpr bool try_to_chars(std::string* dest, Args... args)
{
  auto [ptr, ec] = std::to_chars(
    dest->data(), dest->data() + dest->size(), args...);
  if (std::errc{} == ec) {
    dest->resize(ptr - dest->data());
    return true;
  }
  return false;
}
} // namespace impl

/**
 * to_string(bool)
 */
constexpr auto to_string(bool value) -> std::string {
  return value ? "true" : "false";
}

constexpr auto to_display_string(bool value) -> std::string {
  return to_string(value);
}

/**
 * to_string(CharT) where CharT is one of character type.
 */
template <char_type CharT>
constexpr auto to_string(CharT value, bool quoted = false) -> std::string {
  return impl::char_to_string_dispatch(value, quoted);
}

template <char_type CharT>
constexpr auto to_display_string(CharT value) -> std::string {
  return to_string(value, true);
}

/**
 * to_string(IntegerT) where IntegerT is one of integer type.
 */
template <integer_type IntegerT>
constexpr auto to_string(IntegerT value, int radix = 10) -> std::string
{
  if (radix < 2 || radix > 36) {
    REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid radix: out of range [2, 36].");
    return "<ERROR:invalid-radix>";
  }
  // 8 : One byte for minus sign '-' and other 7 bytes for alignment
  constexpr auto buffer_size = CHAR_BIT * sizeof(IntegerT) + 8;
  auto res = std::string{};
  res.resize_and_overwrite(buffer_size,
    [value, radix](char* buffer, size_t buffer_size) {
      auto [ptr, ec] = std::to_chars(
        buffer, buffer + buffer_size, value, radix);
      if (std::errc{} != ec) {
        REFLECT_CPP26_UNREACHABLE("Internal error");
      }
      return ptr - buffer;
    });
  return res;
}

template <integer_type IntegerT>
constexpr auto to_display_string(IntegerT value) -> std::string {
  return to_string(value);
}

/**
 * to_string(FloatT) where FloatT is one of floating-point type.
 * Note: Only general and hex modes are supported.
 */
template <std::floating_point FloatT>
constexpr auto to_string(
  FloatT value, std::chars_format fmt = std::chars_format::general)
  -> std::string
{
  if (fmt != std::chars_format::general && fmt != std::chars_format::hex) {
    REFLECT_CPP26_ERROR_IF_CONSTEVAL("Unsupported format mode.");
    return "<ERROR:invalid-format>";
  }
  // 64 bytes is enough for hex and scientific mode with 128-bit IEEE quadraple.
  constexpr auto buffer_size = 64zU;
  auto res = std::string{};
  res.resize_and_overwrite(buffer_size,
    [value, fmt](char* buffer, size_t buffer_size) {
      auto [ptr, ec] = std::to_chars(
        buffer, buffer + buffer_size, value, fmt);
      if (std::errc{} != ec) {
        REFLECT_CPP26_UNREACHABLE("Internal error");
      }
      return ptr - buffer;
    });
  return res;
}

/**
 * to_string(FloatT, fmt, precision) where FloatT is one of floating-point type.
 * Note: Only general and hex modes are supported.
 */
template <std::floating_point FloatT>
constexpr auto to_string(FloatT value, std::chars_format fmt, int precision)
  -> std::string
{
  if (precision < 0) {
    REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid precision: non-negative only.");
    return "<ERROR:invalid-precision>";
  }
  if (fmt != std::chars_format::general && fmt != std::chars_format::hex) {
    REFLECT_CPP26_ERROR_IF_CONSTEVAL("Unsupported format mode.");
    return "<ERROR:invalid-format>";
  }
  auto res = std::string{};
  // 12 more bytes is enough for hex and scientific mode.
  res.resize_and_overwrite(precision + 12,
    [value, fmt, precision](char* buffer, size_t buffer_size) {
      auto [ptr, ec] = std::to_chars(
        buffer, buffer + buffer_size, value, fmt, precision);
      if (std::errc{} != ec) {
        REFLECT_CPP26_UNREACHABLE("Internal error");
      }
      return ptr - buffer;
    });
  return res;
}

template <std::floating_point FloatT>
constexpr auto to_display_string(FloatT value) -> std::string {
  return to_string(value);
}

namespace impl {
constexpr auto write_display_string(
  const char* input_cur, const char* input_end,
  char* buffer_cur, const char* buffer_end) -> std::pair<const char*, char*>
{
  constexpr auto n_xdigits_per_byte = 2;
  // spc: special control
  constexpr char spc_chars[] =
    {'\0', '\t', '\n', '\v', '\f', '\r', '"', '\\'};
  constexpr char spc_chars_display[] =
    { '0',  't',  'n',  'v',  'f',  'r', '"', '\\'};

  for (; input_cur < input_end && buffer_cur < buffer_end; ++input_cur) {
    // (1) Special control characters (see above)
    auto spc_pos = std::ranges::find(spc_chars, *input_cur);
    if (std::end(spc_chars) != spc_pos) {
      if (buffer_end - buffer_cur < 2) {
        break; // 2 : length of "\0" or "\t" etc.
      }
      *buffer_cur++ = '\\';
      *buffer_cur++ = spc_chars_display[spc_pos - spc_chars];
      continue;
    }
    // (2) Printable characters (including whitespace ' ')
    if (isprint(*input_cur)) {
      *buffer_cur++ = *input_cur;
      continue;
    }
    // (3) Other non-printable characters
    if (buffer_end - buffer_cur < 4) {
      break; // 4 : length of "\xAB", where AB represents two hex digits
    }
    buffer_cur = std::copy_n("\\x", 2, buffer_cur);
    buffer_cur = std::copy_n(
      byte_to_hex_table_v + (uint8_t)(*input_cur) * n_xdigits_per_byte,
      n_xdigits_per_byte, buffer_cur);
  }
  return std::pair{input_cur, buffer_cur};
}

constexpr auto to_display_string(std::string_view string) -> std::string
{
  if (string.empty()) {
    return "\"\"";
  }
  auto res = std::string{"\""};
  auto temp = std::string{};
  const auto* input_cur = string.data();
  const auto* input_end = input_cur + string.size();

  constexpr auto extra_reserved_size = 16zU;
  for (; input_cur != input_end; ) {
    temp.resize_and_overwrite(string.size() + extra_reserved_size,
      [&input_cur, input_end](char* buffer_cur, size_t buffer_length) {
        const auto* buffer_begin = buffer_cur;
        std::tie(input_cur, buffer_cur) = write_display_string(
          input_cur, input_end, buffer_cur, buffer_cur + buffer_length);
        return buffer_cur - buffer_begin;
      });
    res += temp;
  }
  res.push_back('"');
  return res;
}
} // namespace impl

/**
 * to_string(const char*)
 */
constexpr auto to_string(const char* string, bool display_style = false)
  -> std::string
{
  if (string == nullptr) {
    return display_style ? "\"\"" : "";
  }
  return display_style ? impl::to_display_string(string) : std::string{string};
}

constexpr auto to_display_string(const char* string) -> std::string {
  return to_string(string, true);
}

/**
 * to_string(const std::string&)
 */
template <class Traits, class Alloc>
constexpr auto to_string(
  const std::basic_string<char, Traits, Alloc>& string,
  bool display_style = false) -> std::string
{
  auto sv = std::string_view{string.data(), string.size()};
  return display_style ? impl::to_display_string(sv) : std::string{sv};
}

template <class Traits, class Alloc>
constexpr auto to_display_string(
  const std::basic_string<char, Traits, Alloc>& string) -> std::string
{
  return to_string(string, true);
}

/**
 * to_string(std::string&&) with move-semantic optimization.
 */
template <class Traits, class Alloc>
constexpr auto to_string(
  std::basic_string<char, Traits, Alloc>&& string, bool display_style = false)
  -> std::string
{
  if (display_style) {
    return impl::to_display_string({string.data(), string.size()});
  }
  if constexpr (std::is_same_v<
      std::remove_cvref_t<decltype(string)>, std::string>) {
    return std::move(string);
  } else {
    return std::string{string.data(), string.size()};
  }
}

template <class Traits, class Alloc>
constexpr auto to_display_string(
  std::basic_string<char, Traits, Alloc>&& string) -> std::string
{
  return to_string(std::move(string), true);
}

/**
 * to_string(std::string_view)
 */
template <class Traits>
constexpr auto to_string(
  std::basic_string_view<char, Traits> string, bool display_style = false)
  -> std::string
{
  auto sv = std::string_view{string.data(), string.size()};
  return display_style ? impl::to_display_string(sv) : std::string{sv};
}

template <class Traits>
constexpr auto to_display_string(
  std::basic_string_view<char, Traits> string) -> std::string
{
  return to_string(string, true);
}

/**
 * to_string(meta_string_view)
 */
constexpr auto to_string(meta_string_view string, bool display_style = false)
  -> std::string
{
  auto sv = std::string_view{string.data(), string.size()};
  return display_style ? impl::to_display_string(sv) : std::string{sv};
}

constexpr auto to_display_string(meta_string_view string) -> std::string {
  return to_string(string, true);
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_TO_STRING_HPP
