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
#include <system_error>

namespace reflect_cpp26 {
namespace impl {
constexpr bool is_utf8_continuation_byte(uint8_t c) {
  return (c & 0xC0) == 0x80;
}

constexpr bool is_utf8_trailing_byte(uint8_t c) {
  return (c & 0xC0) == 0x80;
}

constexpr uint8_t utf8_continuation_value(uint8_t c) {
  return c & 0x3F;
}

constexpr char32_t combine_utf8_bytes(char32_t first, uint8_t cont) {
  return (first << 6) | utf8_continuation_value(cont);
}

constexpr auto decode_utf8(const char8_t* input, const char8_t* input_end)
    -> std::tuple<char32_t, const char8_t*, std::errc> {
  // Precondition: input < input_end
  auto c0 = static_cast<uint8_t>(*input++);
  if (c0 < 0x80) {
    return {static_cast<char32_t>(c0), input, std::errc{}};
  }
  if (c0 < 0xC0) {
    return {0, input, std::errc::invalid_argument};
  }

  char32_t code_point = 0;
  size_t num_continuations = 0;

  if (c0 < 0xE0) {
    code_point = c0 & 0x1F;
    num_continuations = 1;
    if (c0 < 0xC2) {
      return {0, input, std::errc::invalid_argument};
    }
  } else if (c0 < 0xF0) {
    code_point = c0 & 0x0F;
    num_continuations = 2;
  } else if (c0 < 0xF8) {
    code_point = c0 & 0x07;
    num_continuations = 3;
    if (c0 > 0xF4) {
      return {0, input, std::errc::invalid_argument};
    }
  } else {
    return {0, input, std::errc::invalid_argument};
  }

  if (input + num_continuations > input_end) {
    return {0, input, std::errc::invalid_argument};
  }
  for (size_t i = 0; i < num_continuations; ++i) {
    auto c = static_cast<uint8_t>(*input);
    if (!is_utf8_trailing_byte(c)) {
      return {0, input, std::errc::invalid_argument};
    }
    code_point = combine_utf8_bytes(code_point, c);
    ++input;
  }

  if (num_continuations == 2) {
    if (code_point < 0x800 || (code_point >= 0xD800 && code_point <= 0xDFFF)) {
      return {0, input, std::errc::invalid_argument};
    }
  } else if (num_continuations == 3) {
    if (code_point < 0x10000 || code_point > 0x10FFFF) {
      return {0, input, std::errc::invalid_argument};
    }
  }

  return {code_point, input, std::errc{}};
}

constexpr auto decode_utf16(const char16_t* input, const char16_t* input_end)
    -> std::tuple<char32_t, const char16_t*, std::errc> {
  // Precondition: input < input_end
  auto c0 = static_cast<char16_t>(*input++);
  if (c0 < 0xD800 || c0 > 0xDFFF) {
    return {static_cast<char32_t>(c0), input, std::errc{}};
  }
  if (c0 >= 0xDC00) {
    return {0, input, std::errc::invalid_argument};
  }
  if (input >= input_end) {
    return {0, input, std::errc::invalid_argument};
  }

  auto c1 = static_cast<char16_t>(*input);
  if (c1 < 0xDC00 || c1 > 0xDFFF) {
    return {0, input, std::errc::invalid_argument};
  }
  ++input;

  auto code_point = static_cast<char32_t>(0x10000 + ((static_cast<char32_t>(c0) - 0xD800) << 10)
                                          + (static_cast<char32_t>(c1) - 0xDC00));
  return {code_point, input, std::errc{}};
}

constexpr char8_t* encode_utf8(char8_t* dest, char32_t code_point) {
  if (code_point < 0x80) {
    *dest = static_cast<char8_t>(code_point);
    return dest + 1;
  }
  if (code_point < 0x800) {
    dest[0] = static_cast<char8_t>(0xC0 | (code_point >> 6));
    dest[1] = static_cast<char8_t>(0x80 | (code_point & 0x3F));
    return dest + 2;
  }
  if (code_point < 0x10000) {
    dest[0] = static_cast<char8_t>(0xE0 | (code_point >> 12));
    dest[1] = static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F));
    dest[2] = static_cast<char8_t>(0x80 | (code_point & 0x3F));
    return dest + 3;
  }
  dest[0] = static_cast<char8_t>(0xF0 | (code_point >> 18));
  dest[1] = static_cast<char8_t>(0x80 | ((code_point >> 12) & 0x3F));
  dest[2] = static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F));
  dest[3] = static_cast<char8_t>(0x80 | (code_point & 0x3F));
  return dest + 4;
}

constexpr char16_t* encode_utf16(char16_t* dest, char32_t code_point) {
  if (code_point <= 0xFFFF) {
    *dest = static_cast<char16_t>(code_point);
    return dest + 1;
  }
  code_point -= 0x10000;
  auto high = static_cast<char16_t>(0xD800 + (code_point >> 10));
  auto low = static_cast<char16_t>(0xDC00 + (code_point & 0x3FF));
  dest[0] = high;
  dest[1] = low;
  return dest + 2;
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
}  // namespace impl

#define REFLECT_CPP26_ENCODING_OVERLOAD(char_dest_t, char_src_t, given_dest_t, given_src_t) \
  static auto operator()(given_dest_t* dest,                                                \
                         given_dest_t* dest_end,                                            \
                         const given_src_t* input,                                          \
                         const given_src_t* input_end)                                      \
      ->encode_result_t<given_dest_t, given_src_t> {                                        \
    auto res = operator()(reinterpret_cast<char_dest_t*>(dest),                             \
                          reinterpret_cast<char_dest_t*>(dest_end),                         \
                          reinterpret_cast<const char_src_t*>(input),                       \
                          reinterpret_cast<const char_src_t*>(input_end));                  \
    return {reinterpret_cast<given_dest_t*>(res.out_ptr),                                   \
            reinterpret_cast<const given_src_t*>(res.in_ptr),                               \
            res.ec};                                                                        \
  }

template <class OutT, class InT>
struct encode_result_t {
  OutT* out_ptr;
  const InT* in_ptr;
  std::errc ec;
};

struct utf8_to_utf16_t {
  static constexpr auto operator()(char16_t* dest,
                                   char16_t* dest_end,
                                   const char8_t* input,
                                   const char8_t* input_end) -> encode_result_t<char16_t, char8_t> {
    while (input < input_end) {
      auto [code_point, next_input, ec] = impl::decode_utf8(input, input_end);
      if (ec != std::errc{}) {
        return {dest, input, ec};
      }
      size_t needed = (code_point <= 0xFFFF) ? 1 : 2;
      if (dest + needed > dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      dest = impl::encode_utf16(dest, code_point);
      input = next_input;
    }
    return {dest, input, std::errc{}};
  }

  // char16_t[] output buffer; char[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char8_t, char16_t, char)
  // char16_t[] output buffer; uint8_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char8_t, char16_t, uint8_t)
  // uint16_t[] output buffer; char8_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char8_t, uint16_t, char8_t)
  // uint16_t[] output buffer; char[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char8_t, uint16_t, char)
  // uint16_t[] output buffer; uint8_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char8_t, uint16_t, uint8_t)
};

struct utf8_to_utf32_t {
  static constexpr auto operator()(char32_t* dest,
                                   char32_t* dest_end,
                                   const char8_t* input,
                                   const char8_t* input_end) -> encode_result_t<char32_t, char8_t> {
    while (input < input_end) {
      auto [code_point, next_input, ec] = impl::decode_utf8(input, input_end);
      if (ec != std::errc{}) {
        return {dest, input, ec};
      }
      if (dest >= dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      *dest++ = code_point;
      input = next_input;
    }
    return {dest, input, std::errc{}};
  }

  // char32_t[] output buffer; char[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char8_t, char32_t, char)
  // char32_t[] output buffer; uint8_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char8_t, char32_t, uint8_t)
  // uint32_t[] output buffer; char8_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char8_t, uint32_t, char8_t)
  // uint32_t[] output buffer; char[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char8_t, uint32_t, char)
  // uint32_t[] output buffer; uint8_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char8_t, uint32_t, uint8_t)
};

struct utf16_to_utf8_t {
  static constexpr auto operator()(char8_t* dest,
                                   char8_t* dest_end,
                                   const char16_t* input,
                                   const char16_t* input_end)
      -> encode_result_t<char8_t, char16_t> {
    while (input < input_end) {
      auto [code_point, next_input, ec] = impl::decode_utf16(input, input_end);
      if (ec != std::errc{}) {
        return {dest, input, ec};
      }
      auto needed = impl::utf8_encoded_length(code_point);
      if (dest + needed > dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      dest = impl::encode_utf8(dest, code_point);
      input = next_input;
    }
    return {dest, input, std::errc{}};
  }

  // char[] output buffer; char16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char16_t, char, char16_t)
  // uint8_t[] output buffer; char16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char16_t, uint8_t, char16_t)
  // char8_t[] output buffer; uint16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char16_t, char8_t, uint16_t)
  // char[] output buffer; uint16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char16_t, char, uint16_t)
  // uint8_t[] output buffer; uint16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char16_t, uint8_t, uint16_t)
};

struct utf16_to_utf32_t {
  static constexpr auto operator()(char32_t* dest,
                                   char32_t* dest_end,
                                   const char16_t* input,
                                   const char16_t* input_end)
      -> encode_result_t<char32_t, char16_t> {
    while (input < input_end) {
      auto [code_point, next_input, ec] = impl::decode_utf16(input, input_end);
      if (ec != std::errc{}) {
        return {dest, input, ec};
      }
      if (dest >= dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      *dest++ = code_point;
      input = next_input;
    }
    return {dest, input, std::errc{}};
  }

  // char32_t[] output buffer; uint16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char16_t, char32_t, uint16_t)
  // uint32_t[] output buffer; char16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char16_t, uint32_t, char16_t)
  // uint32_t[] output buffer; uint16_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char32_t, char16_t, uint32_t, uint16_t)
};

struct utf32_to_utf8_t {
  static constexpr auto operator()(char8_t* dest,
                                   char8_t* dest_end,
                                   const char32_t* input,
                                   const char32_t* input_end)
      -> encode_result_t<char8_t, char32_t> {
    while (input < input_end) {
      auto code_point = *input;
      if (code_point > 0x10FFFF || (code_point >= 0xD800 && code_point <= 0xDFFF)) {
        return {dest, input, std::errc::invalid_argument};
      }
      auto needed = impl::utf8_encoded_length(code_point);
      if (dest + needed > dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      dest = impl::encode_utf8(dest, code_point);
      ++input;
    }
    return {dest, input, std::errc{}};
  }

  // char[] output buffer; char32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char32_t, char, char32_t)
  // uint8_t[] output buffer; char32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char32_t, uint8_t, char32_t)
  // char8_t[] output buffer; uint32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char32_t, char8_t, uint32_t)
  // char[] output buffer; uint32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char32_t, char, uint32_t)
  // uint8_t[] output buffer; uint32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char8_t, char32_t, uint8_t, uint32_t)
};

struct utf32_to_utf16_t {
  static constexpr auto operator()(char16_t* dest,
                                   char16_t* dest_end,
                                   const char32_t* input,
                                   const char32_t* input_end)
      -> encode_result_t<char16_t, char32_t> {
    while (input < input_end) {
      auto code_point = *input;
      if (code_point > 0x10FFFF || (code_point >= 0xD800 && code_point <= 0xDFFF)) {
        return {dest, input, std::errc::invalid_argument};
      }
      auto needed = impl::utf16_encoded_length(code_point);
      if (dest + needed > dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      dest = impl::encode_utf16(dest, code_point);
      ++input;
    }
    return {dest, input, std::errc{}};
  }

  // char16_t[] output buffer; uint32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char32_t, char16_t, uint32_t)
  // uint16_t[] output buffer; char32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char32_t, uint16_t, char32_t)
  // uint16_t[] output buffer; uint32_t[] input array
  REFLECT_CPP26_ENCODING_OVERLOAD(char16_t, char32_t, uint16_t, uint32_t)
};

constexpr auto utf8_to_utf16 = utf8_to_utf16_t{};
constexpr auto utf8_to_utf32 = utf8_to_utf32_t{};
constexpr auto utf16_to_utf8 = utf16_to_utf8_t{};
constexpr auto utf16_to_utf32 = utf16_to_utf32_t{};
constexpr auto utf32_to_utf8 = utf32_to_utf8_t{};
constexpr auto utf32_to_utf16 = utf32_to_utf16_t{};
}  // namespace reflect_cpp26

#undef REFLECT_CPP26_ENCODING_OVERLOAD

#endif  // REFLECT_CPP26_UTILS_STRING_ENCODING_HPP
