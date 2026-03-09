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
constexpr auto invalid_code_point = static_cast<char32_t>(-1);

constexpr bool is_utf8_trailing_byte(uint8_t c) {
  return (c & 0xC0) == 0x80;
}

constexpr bool is_invalid_utf8_leading_byte(uint8_t c) {
  return (c >= 0xC0 && c <= 0xC1) || (c >= 0xF5);
}

constexpr bool is_high_surrogate(char16_t c) {
  return c >= 0xD800 && c <= 0xDBFF;
}

constexpr bool is_low_surrogate(char16_t c) {
  return c >= 0xDC00 && c <= 0xDFFF;
}

constexpr uint8_t utf8_continuation_value(uint8_t c) {
  return c & 0x3F;
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

#define REFLECT_CPP26_UNWIND_AND_RETURN_INVALID() \
  do {                                            \
    *input = original;                            \
    return invalid_code_point;                    \
  } while (false)

constexpr char32_t decode_utf8(const char8_t** input, const char8_t* input_end) {
  // Precondition: *input < input_end
  const char8_t* original = *input;
  auto c0 = static_cast<uint8_t>(*(*input)++);
  if (c0 < 0x80) {
    return static_cast<char32_t>(c0);
  }
  if (c0 < 0xC0) {
    REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
  }

  char32_t code_point = 0;
  size_t num_continuations = 0;

  if (c0 < 0xE0) {
    code_point = c0 & 0x1F;
    num_continuations = 1;
    if (c0 < 0xC2) {
      REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
    }
  } else if (c0 < 0xF0) {
    code_point = c0 & 0x0F;
    num_continuations = 2;
  } else if (c0 < 0xF8) {
    code_point = c0 & 0x07;
    num_continuations = 3;
    if (c0 > 0xF4) {
      REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
    }
  } else {
    REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
  }

  if (*input + num_continuations > input_end) {
    REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
  }
  for (size_t i = 0; i < num_continuations; ++i) {
    auto c = static_cast<uint8_t>(*(*input)++);
    if (!is_utf8_trailing_byte(c)) {
      REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
    }
    code_point = (code_point << 6) | (c & 0x3F);
  }

  if (num_continuations == 2) {
    if (code_point < 0x800 || (code_point >= 0xD800 && code_point <= 0xDFFF)) {
      REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
    }
  } else if (num_continuations == 3) {
    if (code_point < 0x10000 || code_point > 0x10FFFF) {
      REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
    }
  }

  return code_point;
}

constexpr char32_t decode_utf16(const char16_t** input, const char16_t* input_end) {
  const char16_t* original = *input;
  auto c0 = static_cast<char16_t>(*(*input)++);
  if (c0 < 0xD800 || c0 > 0xDFFF) {
    return static_cast<char32_t>(c0);
  }
  if (c0 >= 0xDC00) {
    REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
  }
  if (*input >= input_end) {
    REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
  }

  auto c1 = static_cast<char16_t>(*(*input)++);
  if (c1 < 0xDC00 || c1 > 0xDFFF) {
    REFLECT_CPP26_UNWIND_AND_RETURN_INVALID();
  }

  auto code_point =
      0x10000 + ((static_cast<char32_t>(c0) - 0xD800) << 10) + (static_cast<char32_t>(c1) - 0xDC00);
  return static_cast<char32_t>(code_point);
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
      const auto* next_input = input;
      auto code_point = impl::decode_utf8(&next_input, input_end);
      if (code_point == impl::invalid_code_point) {
        // Does not consume the invalid character
        return {dest, input, std::errc::invalid_argument};
      }
      size_t needed = impl::utf16_encoded_length(code_point);
      if (dest + needed > dest_end) {
        // Does not consume the character that fails to be written to dest.
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
      if (dest >= dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      auto code_point = impl::decode_utf8(&input, input_end);  // input advanced
      if (code_point == impl::invalid_code_point) {
        // Does not consume the invalid character
        return {dest, input, std::errc::invalid_argument};
      }
      *dest++ = code_point;
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
      const auto* next_input = input;
      auto code_point = impl::decode_utf16(&next_input, input_end);
      if (code_point == impl::invalid_code_point) {
        // Does not consume the invalid character
        return {dest, input, std::errc::invalid_argument};
      }
      auto needed = impl::utf8_encoded_length(code_point);
      if (dest + needed > dest_end) {
        // Does not consume the character that fails to be written to dest.
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
      if (dest >= dest_end) {
        return {dest, input, std::errc::value_too_large};
      }
      auto code_point = impl::decode_utf16(&input, input_end);  // input advanced
      if (code_point == impl::invalid_code_point) {
        // Does not consume the invalid character
        return {dest, input, std::errc::invalid_argument};
      }
      *dest++ = code_point;
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
    for (; input < input_end; ++input) {
      auto code_point = *input;
      if (code_point > 0x10FFFF || (code_point >= 0xD800 && code_point <= 0xDFFF)) {
        // Does not consume the invalid character
        return {dest, input, std::errc::invalid_argument};
      }
      auto needed = impl::utf8_encoded_length(code_point);
      if (dest + needed > dest_end) {
        // Does not consume the character that fails to be written to dest.
        return {dest, input, std::errc::value_too_large};
      }
      dest = impl::encode_utf8(dest, code_point);
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
    for (; input < input_end; ++input) {
      auto code_point = *input;
      if (code_point > 0x10FFFF || (code_point >= 0xD800 && code_point <= 0xDFFF)) {
        // Does not consume the invalid character
        return {dest, input, std::errc::invalid_argument};
      }
      auto needed = impl::utf16_encoded_length(code_point);
      if (dest + needed > dest_end) {
        // Does not consume the character that fails to be written to dest.
        return {dest, input, std::errc::value_too_large};
      }
      dest = impl::encode_utf16(dest, code_point);
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

// Consumes the maximal prefix of continuous invalid UTF-8 bytes.
// Let ret be the returned pointer, [input, ret) is expected to be replaced as a single placeholder
// '�' according to Unicode 11 or later standard.
struct consume_utf8_invalid_sequence_t {
  static constexpr auto operator()(const char8_t* input, const char8_t* input_end)  //
      -> const char8_t* {
    while (input < input_end) {
      auto c = static_cast<uint8_t>(*input);
      if (c < 0x80) {
        break;
      }
      if (c < 0xC0) {
        ++input;
        continue;
      }
      if (impl::is_invalid_utf8_leading_byte(c)) {
        ++input;
        continue;
      }
      break;
    }
    return input;
  }

  static auto operator()(const char* input, const char* input_end) -> const char* {
    const char8_t* res = operator()(reinterpret_cast<const char8_t*>(input),
                                    reinterpret_cast<const char8_t*>(input_end));
    return reinterpret_cast<const char*>(res);
  }

  static auto operator()(const uint8_t* input, const uint8_t* input_end) -> const uint8_t* {
    const char8_t* res = operator()(reinterpret_cast<const char8_t*>(input),
                                    reinterpret_cast<const char8_t*>(input_end));
    return reinterpret_cast<const uint8_t*>(res);
  }
};

// Consumes the maximal prefix of continuous invalid UTF-16 double-bytes.
// Let ret be the returned pointer, [input, ret) is expected to be replaced as a single placeholder
// '�' according to Unicode 11 or later standard.
struct consume_utf16_invalid_sequence_t {
  static constexpr auto operator()(const char16_t* input, const char16_t* input_end)  //
      -> const char16_t* {
    while (input < input_end) {
      auto c = static_cast<char16_t>(*input);
      if (!impl::is_high_surrogate(c) && !impl::is_low_surrogate(c)) {
        break;
      }
      if (impl::is_low_surrogate(c)) {
        ++input;
        continue;
      }
      if (impl::is_high_surrogate(c)) {
        if (input + 1 < input_end && impl::is_low_surrogate(*(input + 1))) {
          break;
        }
        ++input;
        continue;
      }
    }
    return input;
  }

  static auto operator()(const uint16_t* input, const uint16_t* input_end) -> const uint16_t* {
    const char16_t* res = operator()(reinterpret_cast<const char16_t*>(input),
                                     reinterpret_cast<const char16_t*>(input_end));
    return reinterpret_cast<const uint16_t*>(res);
  }
};

constexpr auto consume_utf8_invalid_sequence = consume_utf8_invalid_sequence_t{};
constexpr auto consume_utf16_invalid_sequence = consume_utf16_invalid_sequence_t{};
}  // namespace reflect_cpp26

#undef REFLECT_CPP26_ENCODING_OVERLOAD
#undef REFLECT_CPP26_UNWIND_AND_RETURN_INVALID

#endif  // REFLECT_CPP26_UTILS_STRING_ENCODING_HPP
