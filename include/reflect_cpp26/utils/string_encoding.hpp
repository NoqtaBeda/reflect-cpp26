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

#ifndef REFLECT_CPP26_UTILS_STRING_ENCODING_HPP
#define REFLECT_CPP26_UTILS_STRING_ENCODING_HPP

#include <cstdint>
#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/utils/ctype.hpp>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

namespace reflect_cpp26 {
enum class escaping_mode {
  none = 0,
  json = 1,            // Following RFC 8259 standard
  display_char = 2,    // Diaplaying character values dueing local debugging
  display_string = 3,  // Diaplaying string values dueing local debugging
};

enum class escaping_status {
  done = 0,
  no_escape = 1,
  error = 2,
};

enum class encoding_status {
  done = 0,
  invalid_character = 1,
  buffer_run_out = 2,
};

constexpr auto invalid_code_point = static_cast<char32_t>(-1);
constexpr auto replacement_code_point = static_cast<char32_t>(0xFFFD);  // '�'

namespace impl {
consteval auto canonical_char_type_of(std::meta::info T) -> std::meta::info {
  if (size_of(T) == 1) {
    return ^^char8_t;
  } else if (size_of(T) == 2) {
    return ^^char16_t;
  } else if (size_of(T) == 4) {
    return ^^char32_t;
  } else {
    compile_error("Invalid integral type.");
  }
}

template <class T>
using canonical_char_type_t = [:canonical_char_type_of(^^T):];

template <class T, size_t... SizeCandidates>
concept non_bool_integral_of_size = non_bool_integral<T> && ((sizeof(T) == SizeCandidates) || ...);

constexpr bool is_high_surrogate(char16_t c) {
  return c >= 0xD800 && c <= 0xDBFF;
}

constexpr bool is_low_surrogate(char16_t c) {
  return c >= 0xDC00 && c <= 0xDFFF;
}

constexpr size_t utf8_encoded_length(char32_t code_point) {
  if (code_point < 0x80) return 1;
  if (code_point < 0x800) return 2;
  if (code_point < 0x10000) return 3;
  return 4;
}

constexpr size_t utf16_encoded_length(char32_t code_point) {
  return (code_point <= 0xFFFF) ? 1 : 2;
}

// Note: Escaping '/' is common practice for JSON serialization
constexpr std::pair<char, char> ascii_json_escape_map[] = {
    {'"', '"'},
    {'\\', '\\'},
    {'\b', 'b'},
    {'\f', 'f'},
    {'\n', 'n'},
    {'\r', 'r'},
    {'\t', 't'},
    {'/', '/'},
};

constexpr std::pair<char, char> ascii_display_escape_map[] = {
    {'\\', '\\'},
    {'\0', '0'},
    {'\a', 'a'},
    {'\b', 'b'},
    {'\f', 'f'},
    {'\n', 'n'},
    {'\r', 'r'},
    {'\t', 't'},
    {'\v', 'v'},
};
}  // namespace impl

// -------- Code Point Validation --------

constexpr bool is_valid_code_point(char32_t code_point) {
  return code_point <= 0xD7FF || (code_point >= 0xE000 && code_point <= 0x10FFFF);
}

// -------- Writing Single Escaping Character --------

#define REFLECT_CPP26_CHECK_BUFFER_SIZE(n)     \
  if constexpr (Checks) {                      \
    if (dest + (n) >= dest_end) [[unlikely]] { \
      return {escaping_status::error, dest};   \
    }                                          \
  }

template <escaping_mode Mode, bool Checks>
struct write_escaped_character_base_t;

template <bool Checks>
struct write_escaped_character_base_t<escaping_mode::json, Checks> {
  // Assumes when Checks == false: Buffer size is enough (>= 6)
  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t c)
      -> std::pair<escaping_status, CharT*> {
    // Marks every branch as [[unlikely]] since escaping characters are rare in real-world text.
    template for (constexpr auto entry : impl::ascii_json_escape_map) {
      if (c == entry.first) [[unlikely]] {
        // Note: dest_end is unused if Checks == false
        REFLECT_CPP26_CHECK_BUFFER_SIZE(2);  // 2 : Length of R"(\n)", R"(\n)", etc.
        *dest++ = '\\';
        *dest++ = entry.second;
        return {escaping_status::done, dest};
      }
    }
    if (c < 0x20) [[unlikely]] {
      REFLECT_CPP26_CHECK_BUFFER_SIZE(6);  // 6 : Length of R"(\u00xx)"
      *dest++ = '\\';
      *dest++ = 'u';
      *dest++ = '0';
      *dest++ = '0';
      *dest++ = static_cast<CharT>(c >= 0x10 ? '1' : '0');
      auto low = c & 0xF;
      *dest++ = static_cast<CharT>(low < 10 ? '0' + low : 'A' + low - 10);
      return {escaping_status::done, dest};
    }
    // Does nothing if c is not an escaping character
    return {escaping_status::no_escape, dest};
  }
};

template <bool Checks>
struct write_escaped_character_base_t<escaping_mode::display_char, Checks> {
  // Assumes when Checks == false: Buffer size is enough (>= 4)
  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t c)
      -> std::pair<escaping_status, CharT*> {
    // Marks every branch as [[unlikely]] since escaping characters are rare in real-world text.
    if (c == '\\') [[unlikely]] {
      // Note: dest_end is unused if Checks == false
      REFLECT_CPP26_CHECK_BUFFER_SIZE(2);  // 2 : Length of R"(\')" or R"(\"")"
      *dest++ = '\\';
      *dest++ = c;
      return {escaping_status::done, dest};
    }
    template for (constexpr auto entry : impl::ascii_display_escape_map) {
      if (c == entry.first) [[unlikely]] {
        REFLECT_CPP26_CHECK_BUFFER_SIZE(2);  // 2 : Length of R"(\n)", R"(\n)", etc.
        *dest++ = '\\';
        *dest++ = entry.second;
        return {escaping_status::done, dest};
      }
    }
    if (c < 0x80 && !ascii_isprint(c)) [[unlikely]] {
      REFLECT_CPP26_CHECK_BUFFER_SIZE(4);  // 4 : Length of R"(\x??)" where ?? are hex digits
      *dest++ = '\\';
      *dest++ = 'x';
      auto high = c >> 8;
      *dest++ = static_cast<CharT>(high < 10 ? '0' + high : 'A' + high - 10);
      auto low = c & 0xF;
      *dest++ = static_cast<CharT>(low < 10 ? '0' + low : 'A' + low - 10);
      return {escaping_status::done, dest};
    }
    // Note: Non-printable Unicode code points beyond 0x80 can not be escaped.
    return {escaping_status::no_escape, dest};
  }
};

template <bool Checks>
struct write_escaped_character_base_t<escaping_mode::display_string, Checks> {
  // Assumes when Checks == false: Buffer size is enough (>= 4)
  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t c)
      -> std::pair<escaping_status, CharT*> {
    // Marks every branch as [[unlikely]] since escaping characters are rare in real-world text.
    if (c == '\"') [[unlikely]] {
      // Note: dest_end is unused if Checks == false
      REFLECT_CPP26_CHECK_BUFFER_SIZE(2);  // 2 : Length of R"(\')" or R"(\"")"
      *dest++ = '\\';
      *dest++ = c;
      return {escaping_status::done, dest};
    }
    template for (constexpr auto entry : impl::ascii_display_escape_map) {
      if (c == entry.first) [[unlikely]] {
        REFLECT_CPP26_CHECK_BUFFER_SIZE(2);  // 2 : Length of R"(\n)", R"(\n)", etc.
        *dest++ = '\\';
        *dest++ = entry.second;
        return {escaping_status::done, dest};
      }
    }
    if (c < 0x80 && !ascii_isprint(c)) [[unlikely]] {
      REFLECT_CPP26_CHECK_BUFFER_SIZE(4);  // 4 : Length of R"(\x??)" where ?? are hex digits
      *dest++ = '\\';
      *dest++ = 'x';
      auto high = c >> 8;
      *dest++ = static_cast<CharT>(high < 10 ? '0' + high : 'A' + high - 10);
      auto low = c & 0xF;
      *dest++ = static_cast<CharT>(low < 10 ? '0' + low : 'A' + low - 10);
      return {escaping_status::done, dest};
    }
    // Note: Non-printable Unicode code points beyond 0x80 can not be escaped.
    return {escaping_status::no_escape, dest};
  }
};

template <escaping_mode Mode>
using write_escaped_character_t = write_escaped_character_base_t<Mode, true>;

template <escaping_mode Mode>
struct write_escaped_character_unsafe_t : write_escaped_character_base_t<Mode, false> {
  using base_type = write_escaped_character_base_t<Mode, false>;
  using base_type::operator();

  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(CharT* dest, char32_t c) -> std::pair<escaping_status, CharT*> {
    // dest_end is unused when Checks == false
    return base_type::operator()(dest, static_cast<const CharT*>(nullptr), c);
  }
};

#define REFLECT_CPP26_WRITE_ESCAPED_CHARACTER_FN(fn, mode)                     \
  using fn##_t = write_escaped_character_t<escaping_mode::mode>;               \
  using fn##_unsafe_t = write_escaped_character_unsafe_t<escaping_mode::mode>; \
  constexpr auto fn = fn##_t{};                                                \
  constexpr auto fn##_unsafe = fn##_unsafe_t{};

REFLECT_CPP26_WRITE_ESCAPED_CHARACTER_FN(write_escaped_character_for_json, json)
REFLECT_CPP26_WRITE_ESCAPED_CHARACTER_FN(write_escaped_character_for_char, display_char)
REFLECT_CPP26_WRITE_ESCAPED_CHARACTER_FN(write_escaped_character_for_string, display_string)

template <escaping_mode Mode>
constexpr auto write_escaped_character = write_escaped_character_t<Mode>{};

template <escaping_mode Mode>
constexpr auto write_escaped_character_unsafe = write_escaped_character_unsafe_t<Mode>{};

#undef REFLECT_CPP26_CHECK_BUFFER_SIZE
#undef REFLECT_CPP26_WRITE_ESCAPED_CHARACTER_FN

// -------- Decoding Single Code Point --------

// Unicode 11 UTF-8 Encoding Rules (Chapter 3, Table 3-7):
// +------------------+----------+----------+----------+----------+
// | Code Point Range | 1st Byte | 2nd Byte | 3rd Byte | 4th Byte |
// +------------------+----------+----------+----------+----------+
// | U+0000-U+007F    | 00-7F    | -        | -        | -        |
// +------------------+----------+----------+----------+----------+
// | U+0080-U+07FF    | C2-DF    | 80-BF    | -        | -        |
// +------------------+----------+----------+----------+----------+
// | U+0800-U+0FFF    | E0       | A0-BF    | 80-BF    | -        |
// | U+1000-U+CFFF    | E1-EC    | 80-BF    | 80-BF    | -        |
// | U+D000-U+DFFF    | ED       | 80-9F    | 80-BF    | -        |
// | U+E000-U+FFFF    | EE-EF    | 80-BF    | 80-BF    | -        |
// +------------------+----------+----------+----------+----------+
// | U+10000-U+3FFFF  | F0       | 90-BF    | 80-BF    | 80-BF    |
// | U+40000-U+FFFFF  | F1-F3    | 80-BF    | 80-BF    | 80-BF    |
// | U+100000-U+10FFFF| F4       | 80-8F    | 80-BF    | 80-BF    |
// +------------------+----------+----------+----------+----------+

namespace impl {
template <class CharT>
struct decode_code_point_impl_t {
  using canonical_char_type = impl::canonical_char_type_t<CharT>;

  static constexpr auto operator()(const CharT* input, const CharT* input_end)
      -> std::pair<char32_t, const CharT*> {
    if consteval {
      if (input_end == nullptr) {
        input_end = std::ranges::find(input, std::unreachable_sentinel, '\0');
      }
      auto n = input_end - input;
      auto buffer = new canonical_char_type[n];
      std::ranges::copy(input, input_end, buffer);
      auto [c, p] = decode_code_point_impl_t<canonical_char_type>::operator()(buffer, buffer + n);
      auto d = p - buffer;
      delete[] buffer;
      return {c, input + d};
    } else {
      auto [c, p] = decode_code_point_impl_t<canonical_char_type>::operator()(
          reinterpret_cast<const canonical_char_type*>(input),
          reinterpret_cast<const canonical_char_type*>(input_end));
      return {c, reinterpret_cast<const CharT*>(p)};
    }
  }
};

template <>
struct decode_code_point_impl_t<char8_t> {
  static constexpr auto operator()(const char8_t* input, const char8_t* input_end)
      -> std::pair<char32_t, const char8_t*> {
    if (input >= input_end) [[unlikely]] {
      return {invalid_code_point, input};
    }

    const char8_t* original = input;
    auto c0 = static_cast<uint8_t>(*input++);
    if (c0 < 0x80) {
      return {static_cast<char32_t>(c0), input};
    }
    if (c0 < 0xC0) [[unlikely]] {
      return {invalid_code_point, original};
    }

    char32_t code_point = 0;
    size_t num_continuations = 0;

    if (c0 < 0xE0) {
      code_point = c0 & 0x1F;
      num_continuations = 1;
      if (c0 < 0xC2) [[unlikely]] {
        return {invalid_code_point, original};
      }
    } else if (c0 < 0xF0) {
      code_point = c0 & 0x0F;
      num_continuations = 2;
    } else if (c0 < 0xF8) {
      code_point = c0 & 0x07;
      num_continuations = 3;
      if (c0 > 0xF4) [[unlikely]] {
        return {invalid_code_point, original};
      }
    } else [[unlikely]] {
      return {invalid_code_point, original};
    }

    if (input + num_continuations > input_end) [[unlikely]] {
      return {invalid_code_point, original};
    }
    for (size_t i = 0; i < num_continuations; ++i) {
      auto c = static_cast<uint8_t>(*input++);
      if ((c & 0xC0) != 0x80) [[unlikely]] {
        return {invalid_code_point, original};
      }
      code_point = (code_point << 6) | (c & 0x3F);
    }

    if (num_continuations == 2) {
      if (code_point < 0x800 || (code_point >= 0xD800 && code_point <= 0xDFFF)) [[unlikely]] {
        return {invalid_code_point, original};
      }
    } else if (num_continuations == 3) {
      if (code_point < 0x10000 || code_point > 0x10FFFF) [[unlikely]] {
        return {invalid_code_point, original};
      }
    }
    return {code_point, input};
  }
};

template <>
struct decode_code_point_impl_t<char16_t> {
  static constexpr auto operator()(const char16_t* input, const char16_t* input_end)
      -> std::pair<char32_t, const char16_t*> {
    if (input >= input_end) [[unlikely]] {
      return {invalid_code_point, input};
    }

    const char16_t* original = input;
    auto c0 = static_cast<char16_t>(*input++);
    if (c0 < 0xD800 || c0 > 0xDFFF) {
      return {static_cast<char32_t>(c0), input};
    }
    if (c0 >= 0xDC00) [[unlikely]] {
      return {invalid_code_point, original};
    }
    if (input >= input_end) [[unlikely]] {
      return {invalid_code_point, original};
    }

    auto c1 = static_cast<char16_t>(*input++);
    if (c1 < 0xDC00 || c1 > 0xDFFF) [[unlikely]] {
      return {invalid_code_point, original};
    }

    auto code_point = 0x10000 + ((char32_t(c0) - 0xD800) << 10) + (char32_t(c1) - 0xDC00);
    return {static_cast<char32_t>(code_point), input};
  }
};

template <>
struct decode_code_point_impl_t<char32_t> {
  static constexpr auto operator()(const char32_t* input, const char32_t* input_end)
      -> std::pair<char32_t, const char32_t*> {
    if (input >= input_end || !is_valid_code_point(*input)) [[unlikely]] {
      return {invalid_code_point, input};
    }
    return {*input, input + 1};
  }
};
}  // namespace impl

struct decode_code_point_t {
  // 1, 2 or 4 byte-sequences. Behavior (UTF-8, UTF-16, UTF-32) depends on input type.
  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end)
      -> std::pair<char32_t, const CharT*> {
    return impl::decode_code_point_impl_t<CharT>::operator()(input, input_end);
  }
};

struct decode_code_point_from_utf8_t {
  // 1 byte-sequence as UTF-8 input.
  template <impl::non_bool_integral_of_size<1> CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end)
      -> std::pair<char32_t, const CharT*> {
    return impl::decode_code_point_impl_t<CharT>::operator()(input, input_end);
  }
};

struct decode_code_point_from_utf16_t {
  // 2 byte-sequence as UTF-16 input.
  template <impl::non_bool_integral_of_size<2> CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end)
      -> std::pair<char32_t, const CharT*> {
    return impl::decode_code_point_impl_t<CharT>::operator()(input, input_end);
  }
};

constexpr auto decode_code_point = decode_code_point_t{};
constexpr auto decode_code_point_from_utf8 = decode_code_point_from_utf8_t{};
constexpr auto decode_code_point_from_utf16 = decode_code_point_from_utf16_t{};

// -------- Encoding Single Code Point --------

#define REFLECT_CPP26_CHECK_BUFFER_SIZE(n)            \
  if constexpr (Checks) {                             \
    if (dest + (n) > dest_end) [[unlikely]] {         \
      return {dest, encoding_status::buffer_run_out}; \
    }                                                 \
  }
#define REFLECT_CPP26_CHECK_CODE_POINT(code_point)       \
  if constexpr (Checks) {                                \
    if (!is_valid_code_point(code_point)) [[unlikely]] { \
      return {dest, encoding_status::invalid_character}; \
    }                                                    \
  }

namespace impl {
template <class CharT, bool Checks>
struct encode_code_point_impl_t {
  using canonical_char_type = canonical_char_type_t<CharT>;

  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> std::pair<CharT*, encoding_status> {
    if consteval {
      if (dest_end == nullptr) {
        canonical_char_type buffer[4];
        auto [p, s] = encode_code_point_impl_t<canonical_char_type, Checks>::operator()(
            buffer, buffer + 4, code_point);
        auto* t = std::ranges::copy(buffer, p, dest).out;
        return {t, s};
      } else {
        auto n = dest_end - dest;
        auto buffer = new canonical_char_type[n];
        auto [p, s] = encode_code_point_impl_t<canonical_char_type, Checks>::operator()(
            buffer, buffer + n, code_point);
        auto* t = std::ranges::copy(buffer, p, dest).out;
        delete[] buffer;
        return {t, s};
      }
    } else {
      auto [p, s] = encode_code_point_impl_t<canonical_char_type, Checks>::operator()(
          reinterpret_cast<canonical_char_type*>(dest),
          reinterpret_cast<const canonical_char_type*>(dest_end),
          code_point);
      return {reinterpret_cast<CharT*>(p), s};
    }
  }
};

template <bool Checks>
struct encode_code_point_impl_t<char8_t, Checks> {
  static constexpr auto operator()(char8_t* dest, const char8_t* dest_end, char32_t code_point)
      -> std::pair<char8_t*, encoding_status> {
    // Assumes when Checks == false: (1) Buffer size is enough; (2) code_point is valid
    REFLECT_CPP26_CHECK_CODE_POINT(code_point);
    // (1) ASCII
    if (code_point < 0x80) {
      REFLECT_CPP26_CHECK_BUFFER_SIZE(1);
      *dest = static_cast<char8_t>(code_point);
      return {dest + 1, encoding_status::done};
    }
    // (2) 2-byte sequence
    if (code_point < 0x800) {
      REFLECT_CPP26_CHECK_BUFFER_SIZE(2);
      dest[0] = static_cast<char8_t>(0xC0 | (code_point >> 6));
      dest[1] = static_cast<char8_t>(0x80 | (code_point & 0x3F));
      return {dest + 2, encoding_status::done};
    }
    // (3) 3-byte sequence
    if (code_point < 0x10000) {
      REFLECT_CPP26_CHECK_BUFFER_SIZE(3);
      dest[0] = static_cast<char8_t>(0xE0 | (code_point >> 12));
      dest[1] = static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F));
      dest[2] = static_cast<char8_t>(0x80 | (code_point & 0x3F));
      return {dest + 3, encoding_status::done};
    }
    // (4) 4-byte sequence
    REFLECT_CPP26_CHECK_BUFFER_SIZE(4);
    dest[0] = static_cast<char8_t>(0xF0 | (code_point >> 18));
    dest[1] = static_cast<char8_t>(0x80 | ((code_point >> 12) & 0x3F));
    dest[2] = static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F));
    dest[3] = static_cast<char8_t>(0x80 | (code_point & 0x3F));
    return {dest + 4, encoding_status::done};
  }
};

template <bool Checks>
struct encode_code_point_impl_t<char16_t, Checks> {
  static constexpr auto operator()(char16_t* dest, const char16_t* dest_end, char32_t code_point)
      -> std::pair<char16_t*, encoding_status> {
    // Assumes when Checks == false: (1) Buffer size is enough; (2) code_point is valid
    REFLECT_CPP26_CHECK_CODE_POINT(code_point);
    // (1) BMP (Basic Multilingual Plane)
    if (code_point <= 0xFFFF) {
      REFLECT_CPP26_CHECK_BUFFER_SIZE(1);
      *dest = static_cast<char16_t>(code_point);
      return {dest + 1, encoding_status::done};
    }
    // (2) Surrogate pair
    REFLECT_CPP26_CHECK_BUFFER_SIZE(2);
    code_point -= 0x10000;
    auto high = static_cast<char16_t>(0xD800 + (code_point >> 10));
    auto low = static_cast<char16_t>(0xDC00 + (code_point & 0x3FF));
    dest[0] = high;
    dest[1] = low;
    return {dest + 2, encoding_status::done};
  }
};

template <bool Checks>
struct encode_code_point_impl_t<char32_t, Checks> {
  static constexpr auto operator()(char32_t* dest, const char32_t* dest_end, char32_t code_point)
      -> std::pair<char32_t*, encoding_status> {
    // Assumes when Checks == false: (1) Buffer size is enough; (2) code_point is valid
    REFLECT_CPP26_CHECK_CODE_POINT(code_point);
    REFLECT_CPP26_CHECK_BUFFER_SIZE(1);
    *dest = code_point;
    return {dest + 1, encoding_status::done};
  }
};
}  // namespace impl

struct encode_code_point_t {
  // 1, 2 or 4 byte-sequences. Behavior (UTF-8, UTF-16, UTF-32) depends on input type.
  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> std::pair<CharT*, encoding_status> {
    return impl::encode_code_point_impl_t<CharT, true>::operator()(dest, dest_end, code_point);
  }
};

struct encode_code_point_unsafe_t {
  // 1, 2 or 4 byte-sequences. Behavior (UTF-8, UTF-16, UTF-32) depends on input type.
  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> CharT* {
    auto [written_until, _] =
        impl::encode_code_point_impl_t<CharT, false>::operator()(dest, dest_end, code_point);
    return written_until;
  }

  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(CharT* dest, char32_t code_point) -> CharT* {
    const auto* dest_end = static_cast<const CharT*>(nullptr);
    auto [written_until, _] =
        impl::encode_code_point_impl_t<CharT, false>::operator()(dest, dest_end, code_point);
    return written_until;
  }
};

struct encode_code_point_to_utf8_t {
  // 1-byte UTF-8 sequence
  template <impl::non_bool_integral_of_size<1> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> std::pair<CharT*, encoding_status> {
    return impl::encode_code_point_impl_t<CharT, true>::operator()(dest, dest_end, code_point);
  }
};

struct encode_code_point_to_utf8_unsafe_t {
  // 1-byte UTF-8 sequence
  template <impl::non_bool_integral_of_size<1> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> CharT* {
    auto [written_until, _] =
        impl::encode_code_point_impl_t<CharT, false>::operator()(dest, dest_end, code_point);
    return written_until;
  }

  template <impl::non_bool_integral_of_size<1> CharT>
  static constexpr auto operator()(CharT* dest, char32_t code_point) -> CharT* {
    const auto* dest_end = static_cast<const CharT*>(nullptr);
    auto [written_until, _] =
        impl::encode_code_point_impl_t<CharT, false>::operator()(dest, dest_end, code_point);
    return written_until;
  }
};

struct encode_code_point_to_utf16_t {
  // 2-byte UTF-16 sequence
  template <impl::non_bool_integral_of_size<2> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> std::pair<CharT*, encoding_status> {
    return impl::encode_code_point_impl_t<CharT, true>::operator()(dest, dest_end, code_point);
  }
};

struct encode_code_point_to_utf16_unsafe_t {
  // 2-byte UTF-16 sequence
  template <impl::non_bool_integral_of_size<2> CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> CharT* {
    auto [written_until, _] =
        impl::encode_code_point_impl_t<CharT, false>::operator()(dest, dest_end, code_point);
    return written_until;
  }

  template <impl::non_bool_integral_of_size<2> CharT>
  static constexpr auto operator()(CharT* dest, char32_t code_point) -> CharT* {
    const auto* dest_end = static_cast<const CharT*>(nullptr);
    auto [written_until, _] =
        impl::encode_code_point_impl_t<CharT, false>::operator()(dest, dest_end, code_point);
    return written_until;
  }
};

constexpr auto encode_code_point = encode_code_point_t{};
constexpr auto encode_code_point_unsafe = encode_code_point_unsafe_t{};
constexpr auto encode_code_point_to_utf8 = encode_code_point_to_utf8_t{};
constexpr auto encode_code_point_to_utf8_unsafe = encode_code_point_to_utf8_unsafe_t{};
constexpr auto encode_code_point_to_utf16 = encode_code_point_to_utf16_t{};
constexpr auto encode_code_point_to_utf16_unsafe = encode_code_point_to_utf16_unsafe_t{};

#undef REFLECT_CPP26_CHECK_BUFFER_SIZE
#undef REFLECT_CPP26_CHECK_CODE_POINT

// -------- UTF Conversion --------

template <class OutT, class InT>
struct encode_result_t {
  OutT* out_ptr;
  const InT* in_ptr;
  encoding_status status;
};

namespace impl {
template <class OutT, class InT, escaping_mode Mode>
struct utf_convert_impl_t {
  using canonical_out_type = canonical_char_type_t<OutT>;
  using canonical_in_type = canonical_char_type_t<InT>;
  using canonical_utf_convert_impl_t =
      utf_convert_impl_t<canonical_out_type, canonical_in_type, Mode>;

  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    if consteval {
      auto dest_n = dest_end - dest;
      auto input_n = input_end - input;
      auto dest_buffer = new canonical_out_type[dest_n];
      auto input_buffer = new canonical_in_type[input_n];
      std::ranges::copy(input, input_end, input_buffer);

      auto [po, pi, s] = canonical_utf_convert_impl_t::operator()(
          dest_buffer, dest_buffer + dest_n, input_buffer, input_buffer + input_n);
      auto* ro = std::ranges::copy(dest_buffer, po, dest).out;
      auto* ri = input + (pi - input_buffer);
      delete[] dest_buffer;
      delete[] input_buffer;
      return {ro, ri, s};
    } else {
      auto res = canonical_utf_convert_impl_t::operator()(
          reinterpret_cast<canonical_out_type*>(dest),
          reinterpret_cast<const canonical_out_type*>(dest_end),
          reinterpret_cast<const canonical_in_type*>(input),
          reinterpret_cast<const canonical_in_type*>(input_end));
      return {reinterpret_cast<OutT*>(res.out_ptr),
              reinterpret_cast<const InT*>(res.in_ptr),
              res.status};
    }
  }
};

template <escaping_mode Mode>
struct utf_convert_impl_t<char16_t, char8_t, Mode> {
  static constexpr auto operator()(char16_t* dest,
                                   const char16_t* dest_end,
                                   const char8_t* input,
                                   const char8_t* input_end) -> encode_result_t<char16_t, char8_t> {
    while (input < input_end) {
      if constexpr (Mode != escaping_mode::none) {
        auto [esc_status, next_dest] =
            write_escaped_character<Mode>(dest, dest_end, static_cast<char32_t>(*input));
        if (esc_status == escaping_status::done) [[unlikely]] {
          dest = next_dest;
          input += 1;
          continue;
        } else if (esc_status == escaping_status::error) [[unlikely]] {
          return {dest, input, encoding_status::buffer_run_out};
        }
      }
      auto [code_point, next_input] = decode_code_point_from_utf8(input, input_end);
      if (code_point == invalid_code_point) [[unlikely]] {
        return {dest, input, encoding_status::invalid_character};
      }
      size_t needed = impl::utf16_encoded_length(code_point);
      if (dest + needed > dest_end) [[unlikely]] {
        return {dest, input, encoding_status::buffer_run_out};
      }
      dest = encode_code_point_to_utf16_unsafe(dest, code_point);
      input = next_input;
    }
    return {dest, input, encoding_status::done};
  }
};

template <escaping_mode Mode>
struct utf_convert_impl_t<char32_t, char8_t, Mode> {
  static constexpr auto operator()(char32_t* dest,
                                   const char32_t* dest_end,
                                   const char8_t* input,
                                   const char8_t* input_end) -> encode_result_t<char32_t, char8_t> {
    while (input < input_end) {
      if (dest >= dest_end) [[unlikely]] {
        return {dest, input, encoding_status::buffer_run_out};
      }
      if constexpr (Mode != escaping_mode::none) {
        auto [esc_status, next_dest] =
            write_escaped_character<Mode>(dest, dest_end, static_cast<char32_t>(*input));
        if (esc_status == escaping_status::done) [[unlikely]] {
          dest = next_dest;
          input += 1;
          continue;
        } else if (esc_status == escaping_status::error) [[unlikely]] {
          return {dest, input, encoding_status::buffer_run_out};
        }
      }
      auto [code_point, next_input] = decode_code_point_from_utf8(input, input_end);
      if (code_point == invalid_code_point) [[unlikely]] {
        return {dest, input, encoding_status::invalid_character};
      }
      *dest++ = code_point;
      input = next_input;
    }
    return {dest, input, encoding_status::done};
  }
};

template <escaping_mode Mode>
struct utf_convert_impl_t<char8_t, char16_t, Mode> {
  static constexpr auto operator()(char8_t* dest,
                                   const char8_t* dest_end,
                                   const char16_t* input,
                                   const char16_t* input_end)
      -> encode_result_t<char8_t, char16_t> {
    while (input < input_end) {
      if constexpr (Mode != escaping_mode::none) {
        auto [esc_status, esc_dest] =
            write_escaped_character<Mode>(dest, dest_end, static_cast<char32_t>(*input));
        if (esc_status == escaping_status::done) [[unlikely]] {
          dest = esc_dest;
          input += 1;
          continue;
        } else if (esc_status == escaping_status::error) [[unlikely]] {
          return {dest, input, encoding_status::buffer_run_out};
        }
      }
      auto [code_point, next_input] = decode_code_point_from_utf16(input, input_end);
      if (code_point == invalid_code_point) [[unlikely]] {
        return {dest, input, encoding_status::invalid_character};
      }
      auto needed = impl::utf8_encoded_length(code_point);
      if (dest + needed > dest_end) [[unlikely]] {
        return {dest, input, encoding_status::buffer_run_out};
      }
      dest = encode_code_point_to_utf8_unsafe(dest, code_point);
      input = next_input;
    }
    return {dest, input, encoding_status::done};
  }
};

template <escaping_mode Mode>
struct utf_convert_impl_t<char32_t, char16_t, Mode> {
  static constexpr auto operator()(char32_t* dest,
                                   const char32_t* dest_end,
                                   const char16_t* input,
                                   const char16_t* input_end)
      -> encode_result_t<char32_t, char16_t> {
    while (input < input_end) {
      if (dest >= dest_end) [[unlikely]] {
        return {dest, input, encoding_status::buffer_run_out};
      }
      if constexpr (Mode != escaping_mode::none) {
        auto [esc_status, esc_dest] =
            write_escaped_character<Mode>(dest, dest_end, static_cast<char32_t>(*input));
        if (esc_status == escaping_status::done) [[unlikely]] {
          dest = esc_dest;
          input += 1;
          continue;
        } else if (esc_status == escaping_status::error) [[unlikely]] {
          return {dest, input, encoding_status::buffer_run_out};
        }
      }
      auto [code_point, next_input] = decode_code_point_from_utf16(input, input_end);
      if (code_point == invalid_code_point) [[unlikely]] {
        return {dest, input, encoding_status::invalid_character};
      }
      *dest++ = code_point;
      input = next_input;
    }
    return {dest, input, encoding_status::done};
  }
};

template <escaping_mode Mode>
struct utf_convert_impl_t<char8_t, char32_t, Mode> {
  static constexpr auto operator()(char8_t* dest,
                                   const char8_t* dest_end,
                                   const char32_t* input,
                                   const char32_t* input_end)
      -> encode_result_t<char8_t, char32_t> {
    for (; input < input_end; ++input) {
      auto code_point = *input;
      if (!is_valid_code_point(code_point)) [[unlikely]] {
        return {dest, input, encoding_status::invalid_character};
      }
      if constexpr (Mode != escaping_mode::none) {
        auto [esc_status, esc_dest] = write_escaped_character<Mode>(dest, dest_end, code_point);
        if (esc_status == escaping_status::done) [[unlikely]] {
          dest = esc_dest;
          continue;
        } else if (esc_status == escaping_status::error) [[unlikely]] {
          return {dest, input, encoding_status::buffer_run_out};
        }
      }
      auto needed = impl::utf8_encoded_length(code_point);
      if (dest + needed > dest_end) [[unlikely]] {
        return {dest, input, encoding_status::buffer_run_out};
      }
      dest = encode_code_point_to_utf8_unsafe(dest, code_point);
    }
    return {dest, input, encoding_status::done};
  }
};

template <escaping_mode Mode>
struct utf_convert_impl_t<char16_t, char32_t, Mode> {
  static constexpr auto operator()(char16_t* dest,
                                   const char16_t* dest_end,
                                   const char32_t* input,
                                   const char32_t* input_end)
      -> encode_result_t<char16_t, char32_t> {
    for (; input < input_end; ++input) {
      auto code_point = *input;
      if (!is_valid_code_point(code_point)) [[unlikely]] {
        return {dest, input, encoding_status::invalid_character};
      }
      if constexpr (Mode != escaping_mode::none) {
        auto [esc_status, esc_dest] = write_escaped_character<Mode>(dest, dest_end, code_point);
        if (esc_status == escaping_status::done) [[unlikely]] {
          dest = esc_dest;
          continue;
        } else if (esc_status == escaping_status::error) [[unlikely]] {
          return {dest, input, encoding_status::buffer_run_out};
        }
      }
      auto needed = impl::utf16_encoded_length(code_point);
      if (dest + needed > dest_end) [[unlikely]] {
        return {dest, input, encoding_status::buffer_run_out};
      }
      dest = encode_code_point_to_utf16_unsafe(dest, code_point);
    }
    return {dest, input, encoding_status::done};
  }
};
}  // namespace impl

template <escaping_mode Mode>
struct utf_convert_base_t {
  template <impl::non_bool_integral_of_size<1, 2, 4> OutT,
            impl::non_bool_integral_of_size<1, 2, 4> InT>
  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    return impl::utf_convert_impl_t<OutT, InT, Mode>::operator()(dest, dest_end, input, input_end);
  }
};

template <escaping_mode Mode>
struct utf8_to_utf16_base_t {
  template <impl::non_bool_integral_of_size<2> OutT,  // 2-byte sequence as UTF-16 output
            impl::non_bool_integral_of_size<1> InT>   // 1-byte sequence as UTF-8 input
  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    return impl::utf_convert_impl_t<OutT, InT, Mode>::operator()(dest, dest_end, input, input_end);
  }
};

template <escaping_mode Mode>
struct utf8_to_utf32_base_t {
  template <impl::non_bool_integral_of_size<4> OutT,  // 4-byte sequence as UTF-32 output
            impl::non_bool_integral_of_size<1> InT>   // 1-byte sequence as UTF-8 input
  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    return impl::utf_convert_impl_t<OutT, InT, Mode>::operator()(dest, dest_end, input, input_end);
  }
};

template <escaping_mode Mode>
struct utf16_to_utf8_base_t {
  template <impl::non_bool_integral_of_size<1> OutT,  // 1-byte sequence as UTF-8 output
            impl::non_bool_integral_of_size<2> InT>   // 2-byte sequence as UTF-16 input
  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    return impl::utf_convert_impl_t<OutT, InT, Mode>::operator()(dest, dest_end, input, input_end);
  }
};

template <escaping_mode Mode>
struct utf16_to_utf32_base_t {
  template <impl::non_bool_integral_of_size<4> OutT,  // 4-byte sequence as UTF-32 output
            impl::non_bool_integral_of_size<2> InT>   // 2-byte sequence as UTF-16 input
  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    return impl::utf_convert_impl_t<OutT, InT, Mode>::operator()(dest, dest_end, input, input_end);
  }
};

template <escaping_mode Mode>
struct utf32_to_utf8_base_t {
  template <impl::non_bool_integral_of_size<1> OutT,  // 1-byte sequence as UTF-8 output
            impl::non_bool_integral_of_size<4> InT>   // 4-byte sequence as UTF-32 input
  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    return impl::utf_convert_impl_t<OutT, InT, Mode>::operator()(dest, dest_end, input, input_end);
  }
};

template <escaping_mode Mode>
struct utf32_to_utf16_base_t {
  template <impl::non_bool_integral_of_size<2> OutT,  // 2-byte sequence as UTF-16 output
            impl::non_bool_integral_of_size<4> InT>   // 4-byte sequence as UTF-32 input
  static constexpr auto operator()(OutT* dest,
                                   const OutT* dest_end,
                                   const InT* input,
                                   const InT* input_end) -> encode_result_t<OutT, InT> {
    return impl::utf_convert_impl_t<OutT, InT, Mode>::operator()(dest, dest_end, input, input_end);
  }
};

#define REFLECT_CPP26_ENCODING_FN(fn)                           \
  using fn##_t = fn##_base_t<escaping_mode::none>;              \
  using fn##_json_escaped_t = fn##_base_t<escaping_mode::json>; \
  constexpr auto fn = fn##_t{};                                 \
  constexpr auto fn##_json_escaped = fn##_json_escaped_t{};     \
  template <escaping_mode Mode>                                 \
  constexpr auto fn##_by = fn##_base_t<Mode>{};

REFLECT_CPP26_ENCODING_FN(utf8_to_utf16)
REFLECT_CPP26_ENCODING_FN(utf8_to_utf32)
REFLECT_CPP26_ENCODING_FN(utf16_to_utf8)
REFLECT_CPP26_ENCODING_FN(utf16_to_utf32)
REFLECT_CPP26_ENCODING_FN(utf32_to_utf8)
REFLECT_CPP26_ENCODING_FN(utf32_to_utf16)
REFLECT_CPP26_ENCODING_FN(utf_convert)  // Dispatcher

#undef REFLECT_CPP26_ENCODING_FN

// -------- Consuming Invalid UTF Sequence --------

// Consumes the maximal prefix of continuous invalid UTF-8, UTF-16 or UTF-32 units.
// Let ret be the returned pointer, [input, ret) is expected to be replaced as a single
// placeholder '�' according to Unicode 11 or later standard.
//
// Unicode 11 UTF-8 Encoding Rules (Chapter 3, Table 3-7):
// +------------------+----------+----------+----------+----------+
// | Code Point Range | 1st Byte | 2nd Byte | 3rd Byte | 4th Byte |
// +------------------+----------+----------+----------+----------+
// | U+0000-U+007F    | 00-7F    | -        | -        | -        |
// +------------------+----------+----------+----------+----------+
// | U+0080-U+07FF    | C2-DF    | 80-BF    | -        | -        |
// +------------------+----------+----------+----------+----------+
// | U+0800-U+0FFF    | E0       | A0-BF    | 80-BF    | -        |
// | U+1000-U+CFFF    | E1-EC    | 80-BF    | 80-BF    | -        |
// | U+D000-U+DFFF    | ED       | 80-9F    | 80-BF    | -        |
// | U+E000-U+FFFF    | EE-EF    | 80-BF    | 80-BF    | -        |
// +------------------+----------+----------+----------+----------+
// | U+10000-U+3FFFF  | F0       | 90-BF    | 80-BF    | 80-BF    |
// | U+40000-U+FFFFF  | F1-F3    | 80-BF    | 80-BF    | 80-BF    |
// | U+100000-U+10FFFF| F4       | 80-8F    | 80-BF    | 80-BF    |
// +------------------+----------+----------+----------+----------+

namespace impl {
template <class CharT>
struct consume_invalid_sequence_impl_t {
  using canonical_char_type = canonical_char_type_t<CharT>;

  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT* {
    if consteval {
      if (input_end == nullptr) {
        input_end = std::ranges::find(input, std::unreachable_sentinel, '\0');
      }
      auto n = input_end - input;
      auto buffer = new canonical_char_type[n];
      std::ranges::copy(input, input_end, buffer);
      const auto* res =
          consume_invalid_sequence_impl_t<canonical_char_type>::operator()(buffer, buffer + n);
      auto d = res - buffer;
      delete[] buffer;
      return input + d;
    } else {
      const auto* res = consume_invalid_sequence_impl_t<canonical_char_type>::operator()(
          reinterpret_cast<const canonical_char_type*>(input),
          reinterpret_cast<const canonical_char_type*>(input_end));
      return reinterpret_cast<const CharT*>(res);
    }
  }
};

#define REFLECT_CPP26_CONSUME_IF(cond) \
  do {                                 \
    if (cond) {                        \
      ++input;                         \
      continue;                        \
    } else {                           \
      return input;                    \
    }                                  \
  } while (false)

template <>
struct consume_invalid_sequence_impl_t<char8_t> {
  static constexpr auto operator()(const char8_t* input, const char8_t* input_end)  //
      -> const char8_t* {
    while (input < input_end) {
      auto c = static_cast<uint8_t>(*input);
      if (c < 0x80) {
        return input;  // Valid ASCII character, stop here
      }
      if (c <= 0xC1 || c >= 0xF5) {
        ++input;  // Invalid leading byte, continue consuming
        continue;
      }
      if (c <= 0xDF) {
        auto is_invalid = (input + 2 > input_end)                // Truncated
                       || (input[1] < 0x80 || input[1] > 0xBF);  // Invalid 2nd byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      }
      if (c == 0xE0) {
        auto is_invalid = (input + 3 > input_end)                // Truncated
                       || (input[1] < 0xA0 || input[1] > 0xBF)   // Overlong: < U+0800
                       || (input[2] < 0x80 || input[2] > 0xBF);  // Invalid 3rd byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      } else if (c >= 0xE1 && c <= 0xEC) {
        auto is_invalid = (input + 3 > input_end)                // Truncated
                       || (input[1] < 0x80 || input[1] > 0xBF)   // Invalid 2nd byte
                       || (input[2] < 0x80 || input[2] > 0xBF);  // Invalid 3rd byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      } else if (c == 0xED) {
        auto is_invalid = (input + 3 > input_end)                // Truncated
                       || (input[1] < 0x80 || input[1] > 0x9F)   // Surrogates: U+D800-U+DFFF
                       || (input[2] < 0x80 || input[2] > 0xBF);  // Invalid 3rd byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      } else if (c >= 0xEE && c <= 0xEF) {
        auto is_invalid = (input + 3 > input_end)                // Truncated
                       || (input[1] < 0x80 || input[1] > 0xBF)   // Invalid 2nd byte
                       || (input[2] < 0x80 || input[2] > 0xBF);  // Invalid 3rd byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      } else if (c == 0xF0) {
        auto is_invalid = (input + 4 > input_end)                // Truncated
                       || (input[1] < 0x90 || input[1] > 0xBF)   // Overlong: < U+10000
                       || (input[2] < 0x80 || input[2] > 0xBF)   // Invalid 3rd byte
                       || (input[3] < 0x80 || input[3] > 0xBF);  // Invalid 4th byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      } else if (c >= 0xF1 && c <= 0xF3) {
        auto is_invalid = (input + 4 > input_end)                // Truncated
                       || (input[1] < 0x80 || input[1] > 0xBF)   // Invalid 2nd byte
                       || (input[2] < 0x80 || input[2] > 0xBF)   // Invalid 3rd byte
                       || (input[3] < 0x80 || input[3] > 0xBF);  // Invalid 4th byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      } else if (c == 0xF4) {
        auto is_invalid = (input + 4 > input_end)                // Truncated
                       || (input[1] < 0x80 || input[1] > 0x8F)   // > U+10FFFF
                       || (input[2] < 0x80 || input[2] > 0xBF)   // Invalid 3rd byte
                       || (input[3] < 0x80 || input[3] > 0xBF);  // Invalid 4th byte
        REFLECT_CPP26_CONSUME_IF(is_invalid);
      } else {
        std::unreachable();  // All possible cases checked above
        return input;
      }
    }
    return input;
  }
};

#undef REFLECT_CPP26_CONSUME_IF

template <>
struct consume_invalid_sequence_impl_t<char16_t> {
  static constexpr auto operator()(const char16_t* input, const char16_t* input_end)  //
      -> const char16_t* {
    while (input < input_end) {
      auto c = static_cast<char16_t>(*input);
      auto is_high_surrogate = impl::is_high_surrogate(c);
      auto is_low_surrogate = impl::is_low_surrogate(c);

      if (!is_high_surrogate && !is_low_surrogate) {
        break;  // (1) Not a surrogate: valid BMP character, stop here
      }
      if (is_low_surrogate) {
        ++input;  // (2) Orphaned low surrogate (0xDC00-0xDFFF): continue consuming
        continue;
      }
      if (is_high_surrogate) {
        if (input + 1 < input_end && impl::is_low_surrogate(*(input + 1))) {
          break;  // (3) Valid surrogate pair (high followed by low): stop here
        }
        ++input;  // (4) Orphaned high surrogate (0xD800-0xDBFF) without low: continue consuming
        continue;
      }
    }
    return input;
  }
};

template <>
struct consume_invalid_sequence_impl_t<char32_t> {
  static constexpr auto operator()(const char32_t* input, const char32_t* input_end)  //
      -> const char32_t* {
    for (; input < input_end; ++input) {
      auto c = *input;
      if (is_valid_code_point(c)) {
        break;
      }
    }
    return input;
  }
};
}  // namespace impl

struct consume_utf_invalid_sequence_t {
  // 1, 2 or 4 byte-sequences. Behavior (UTF-8, UTF-16, UTF-32) depends on input type.
  template <impl::non_bool_integral_of_size<1, 2, 4> CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT* {
    return impl::consume_invalid_sequence_impl_t<CharT>::operator()(input, input_end);
  }
};

struct consume_utf8_invalid_sequence_t {
  // 1-byte sequence as UTF-8 input
  template <impl::non_bool_integral_of_size<1> CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT* {
    return impl::consume_invalid_sequence_impl_t<CharT>::operator()(input, input_end);
  }
};

struct consume_utf16_invalid_sequence_t {
  // 2-byte sequence as UTF-16 input
  template <impl::non_bool_integral_of_size<2> CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT* {
    return impl::consume_invalid_sequence_impl_t<CharT>::operator()(input, input_end);
  }
};

struct consume_utf32_invalid_sequence_t {
  // 4-byte sequence as UTF-32 input
  template <impl::non_bool_integral_of_size<4> CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT* {
    return impl::consume_invalid_sequence_impl_t<CharT>::operator()(input, input_end);
  }
};

constexpr auto consume_utf8_invalid_sequence = consume_utf8_invalid_sequence_t{};
constexpr auto consume_utf16_invalid_sequence = consume_utf16_invalid_sequence_t{};
constexpr auto consume_utf32_invalid_sequence = consume_utf32_invalid_sequence_t{};
constexpr auto consume_utf_invalid_sequence = consume_utf_invalid_sequence_t{};

#undef REFLECT_CPP26_CONSUMING_OVERLOAD
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_STRING_ENCODING_HPP
