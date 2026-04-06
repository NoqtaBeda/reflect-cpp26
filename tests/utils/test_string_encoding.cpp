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

#include <reflect_cpp26/utils/string_encoding.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

namespace examples {

template <typename Char16T>
constexpr Char16T* encode_utf16(Char16T* dest, char32_t code_point) {
  if (code_point <= 0xFFFF) {
    *dest = static_cast<Char16T>(code_point);
    return dest + 1;
  }
  code_point -= 0x10000;
  auto high = static_cast<Char16T>(0xD800 + (code_point >> 10));
  auto low = static_cast<Char16T>(0xDC00 + (code_point & 0x3FF));
  dest[0] = high;
  dest[1] = low;
  return dest + 2;
}

// Example: UTF-8 to UTF-16 conversion
// Documentation: docs/utils.md
TEST(UtilsStringEncodingExamples, Utf8ToUtf16Basic) {
  char8_t utf8_input[] = u8"Hello, 世界! 🌍";
  constexpr auto len = sizeof(u8"Hello, 世界! 🌍") - 1;
  char16_t utf16_output[32] = {};
  auto result = rfl::utf8_to_utf16(utf16_output, utf16_output + 32, utf8_input, utf8_input + len);
  EXPECT_EQ(std::errc{}, result.ec);
  // Success: result.out_ptr points past the last character written
  EXPECT_EQ(utf8_input + len, result.in_ptr);
}

// Example: Error handling - invalid continuation byte
// Documentation: docs/utils.md
TEST(UtilsStringEncodingExamples, Utf8ToUtf16InvalidContinuation) {
  // 0x41 is not a valid continuation byte after 0xC2
  char8_t invalid_utf8[] = {char8_t(0xC2), char8_t(0x41)};
  char16_t output[16] = {};
  auto err_result = rfl::utf8_to_utf16(output, output + 16, invalid_utf8, invalid_utf8 + 2);
  EXPECT_EQ(std::errc::invalid_argument, err_result.ec);
  // err_result.in_ptr == invalid_utf8 (points to the invalid sequence, can retry)
  EXPECT_EQ(invalid_utf8, err_result.in_ptr);
  // err_result.out_ptr == output (nothing written)
  EXPECT_EQ(output, err_result.out_ptr);
}

// Example: Using alternative character types
// Documentation: docs/utils.md
TEST(UtilsStringEncodingExamples, AlternativeCharTypes) {
  char input[] = "ASCII";    // char instead of char8_t
  uint16_t buffer[16] = {};  // uint16_t instead of char16_t
  auto r = rfl::utf8_to_utf16(buffer, buffer + 16, input, input + 5);
  EXPECT_EQ(std::errc{}, r.ec);
  EXPECT_EQ(buffer + 5, r.out_ptr);
  EXPECT_EQ(input + 5, r.in_ptr);
}

// Example: Error handling with replacement character
// This test demonstrates the error handling pattern.
// Documentation: docs/utils.md
TEST(UtilsStringEncodingExamples, ReplaceInvalidWithReplacementChar) {
  char8_t input[] = {'H',
                     'e',
                     'l',
                     'l',
                     'o',
                     static_cast<char8_t>(0xFF),
                     static_cast<char8_t>(0xFE),
                     'W',
                     'o',
                     'r',
                     'l',
                     'd'};
  constexpr auto len = sizeof(input);
  char16_t output[32] = {};
  char16_t* out_ptr = output;

  // First conversion: convert "Hello", stop at invalid 0xFF
  auto result = rfl::utf8_to_utf16(out_ptr, output + 32, input, input + len);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  EXPECT_EQ(5, result.out_ptr - output);  // "Hello" = 5 chars

  // Verify in_ptr points to the invalid byte
  EXPECT_EQ(input + 5, result.in_ptr);

  // Consume invalid bytes (0xFF, 0xFE)
  auto invalid_end = rfl::consume_utf8_invalid_sequence(result.in_ptr, input + len);
  EXPECT_EQ(input + 7, invalid_end);

  // Update out_ptr to continue from where first conversion stopped
  out_ptr = result.out_ptr;

  // Write replacement character
  out_ptr = encode_utf16(out_ptr, 0xFFFD);

  // Second conversion: convert remaining "World"
  result = rfl::utf8_to_utf16(out_ptr, output + 32, invalid_end, input + len);

  // Final result should be 11 UTF-16 code units (5 + 1 + 5)
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(11, result.out_ptr - output);
}

}  // namespace examples

// ==================== UTF-8 to UTF-16 ====================

TEST(UtilsStringEncoding, Utf8ToUtf16Empty) {
  char8_t input[] = u8"";
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input);
  // No input processed, no output written
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf8ToUtf16Ascii) {
  char8_t input[] = u8"Hello";
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 5);
  // 5 ASCII chars converted: out_ptr advanced by 5, in_ptr advanced by 5
  EXPECT_EQ(output + 5, result.out_ptr);
  EXPECT_EQ(input + 5, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(u'H', output[0]);
  EXPECT_EQ(u'e', output[1]);
  EXPECT_EQ(u'l', output[2]);
  EXPECT_EQ(u'l', output[3]);
  EXPECT_EQ(u'o', output[4]);
}

TEST(UtilsStringEncoding, Utf8ToUtf16TwoByte) {
  char8_t input[] = u8"\u00A9\u00AE";
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 4);
  // 2 code points (2-byte UTF-8 each): out_ptr advanced by 2, in_ptr advanced by 4
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 4, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(u'\u00A9', output[0]);
  EXPECT_EQ(u'\u00AE', output[1]);
}

TEST(UtilsStringEncoding, Utf8ToUtf16ThreeByte) {
  char8_t input[] = u8"\u4E2D\u6587";
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 6);
  // 2 code points (3-byte UTF-8 each): out_ptr advanced by 2, in_ptr advanced by 6
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 6, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(u'\u4E2D', output[0]);
  EXPECT_EQ(u'\u6587', output[1]);
}

TEST(UtilsStringEncoding, Utf8ToUtf16FourByte) {
  char8_t input[] = u8"\U0001F600\U0001F389";
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 8);
  // 2 code points (4-byte UTF-8 each, each becomes surrogate pair):
  // out_ptr advanced by 4, in_ptr advanced by 8
  EXPECT_EQ(output + 4, result.out_ptr);
  EXPECT_EQ(input + 8, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(static_cast<char16_t>(0xD83D), output[0]);
  EXPECT_EQ(static_cast<char16_t>(0xDE00), output[1]);
  EXPECT_EQ(static_cast<char16_t>(0xD83C), output[2]);
  EXPECT_EQ(static_cast<char16_t>(0xDF89), output[3]);
}

TEST(UtilsStringEncoding, Utf8ToUtf16BufferTooSmall) {
  char8_t input[] = u8"Hello";
  char16_t output[3] = {};
  auto result = rfl::utf8_to_utf16(output, output + 3, input, input + 5);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // 3 chars written, 4th char doesn't fit: out_ptr at output + 3, in_ptr at input + 3 (NOT
  // consumed, can retry)
  EXPECT_EQ(output + 3, result.out_ptr);
  EXPECT_EQ(input + 3, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16InvalidContinuation) {
  char8_t input[] = {static_cast<char8_t>(0xC2), static_cast<char8_t>(0x41)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Error: 0xC2 expects continuation, got 0x41 - does not consume invalid bytes
  // no output written, in_ptr points to the invalid sequence (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16OverlongEncoding) {
  char8_t input[] = {static_cast<char8_t>(0xC0), static_cast<char8_t>(0x80)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Overlong encoding (0xC0 0x80 would encode NUL, but is invalid): does not consume
  // leading byte no output written, in_ptr points to the invalid sequence
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16TruncatedSequence) {
  char8_t input[] = {static_cast<char8_t>(0xE2)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // 0xE2 starts 3-byte sequence but no continuation bytes: does not consume
  // no output written, in_ptr points to the truncated sequence
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16SurrogateInUtf8) {
  char8_t input[] = {
      static_cast<char8_t>(0xED), static_cast<char8_t>(0xA0), static_cast<char8_t>(0x80)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 3);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // 0xED 0xA0 0x80 encodes U+D800 (surrogate), which is invalid in UTF-8: does not consume
  // no output written, in_ptr points to the invalid sequence
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

// ==================== UTF-8 to UTF-32 ====================

TEST(UtilsStringEncoding, Utf8ToUtf32Empty) {
  char8_t input[] = u8"";
  char32_t output[16] = {};
  auto result = rfl::utf8_to_utf32(output, output + 16, input, input);
  // No input processed, no output written
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf8ToUtf32Ascii) {
  char8_t input[] = u8"ABC";
  char32_t output[16] = {};
  auto result = rfl::utf8_to_utf32(output, output + 16, input, input + 3);
  // 3 ASCII chars converted: out_ptr advanced by 3, in_ptr advanced by 3
  EXPECT_EQ(output + 3, result.out_ptr);
  EXPECT_EQ(input + 3, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(U'A', output[0]);
  EXPECT_EQ(U'B', output[1]);
  EXPECT_EQ(U'C', output[2]);
}

TEST(UtilsStringEncoding, Utf8ToUtf32Mixed) {
  char8_t input[] = u8"A\u00A9\u4E2D\U0001F600";
  char32_t output[16] = {};
  auto result = rfl::utf8_to_utf32(output, output + 16, input, input + 10);
  // 4 code points (1+2+3+4 bytes in UTF-8): out_ptr advanced by 4, in_ptr advanced by 10
  EXPECT_EQ(output + 4, result.out_ptr);
  EXPECT_EQ(input + 10, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(U'A', output[0]);
  EXPECT_EQ(U'\u00A9', output[1]);
  EXPECT_EQ(U'\u4E2D', output[2]);
  EXPECT_EQ(U'\U0001F600', output[3]);
}

TEST(UtilsStringEncoding, Utf8ToUtf32BufferTooSmall) {
  char8_t input[] = u8"ABC";
  char32_t output[2] = {};
  auto result = rfl::utf8_to_utf32(output, output + 2, input, input + 3);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // 2 chars converted before buffer full: out_ptr at output + 2, in_ptr at input + 2 (NOT consumed,
  // can retry)
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf32InvalidContinuation) {
  char8_t input[] = {static_cast<char8_t>(0xC2), static_cast<char8_t>(0x41)};
  char32_t output[16] = {};
  auto result = rfl::utf8_to_utf32(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Error: 0xC2 expects continuation, got 0x41 - does not consume
  // no output written, in_ptr points to the invalid sequence (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

// ==================== UTF-16 to UTF-8 ====================

TEST(UtilsStringEncoding, Utf16ToUtf8Empty) {
  char16_t input[] = u"";
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input);
  // No input processed, no output written
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf16ToUtf8Ascii) {
  char16_t input[] = u"Hi";
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 2);
  // 2 BMP chars converted (each 1 byte in UTF-8): out_ptr advanced by 2, in_ptr advanced by 2
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(u8'H', output[0]);
  EXPECT_EQ(u8'i', output[1]);
}

TEST(UtilsStringEncoding, Utf16ToUtf8Bmp) {
  char16_t input[] = u"\u00A9\u4E2D";
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 2);
  // 2 BMP chars (2+3 bytes in UTF-8): out_ptr advanced by 5, in_ptr advanced by 2
  EXPECT_EQ(output + 5, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf16ToUtf8SurrogatePair) {
  char16_t input[] = {static_cast<char16_t>(0xD83D), static_cast<char16_t>(0xDE00)};
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 2);
  // 1 surrogate pair -> 1 code point (4 bytes in UTF-8): out_ptr advanced by 4, in_ptr advanced by
  // 2
  EXPECT_EQ(output + 4, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(static_cast<char8_t>(0xF0), output[0]);
  EXPECT_EQ(static_cast<char8_t>(0x9F), output[1]);
  EXPECT_EQ(static_cast<char8_t>(0x98), output[2]);
  EXPECT_EQ(static_cast<char8_t>(0x80), output[3]);
}

TEST(UtilsStringEncoding, Utf16ToUtf8LowSurrogateOnly) {
  char16_t input[] = {static_cast<char16_t>(0xDC00)};
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Low surrogate without high surrogate: does not consume
  // no output written, in_ptr points to the invalid surrogate (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf16ToUtf8HighSurrogateOnly) {
  char16_t input[] = {static_cast<char16_t>(0xD800)};
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // High surrogate without low surrogate (end of input): does not consume
  // no output written, in_ptr points to the truncated surrogate (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf16ToUtf8HighSurrogateWithoutLow) {
  char16_t input[] = {static_cast<char16_t>(0xD800), u'A'};
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // High surrogate followed by non-low-surrogate: does not consume
  // no output written, in_ptr points to the invalid sequence (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf16ToUtf8BufferTooSmall) {
  char16_t input[] = u"\u4E2D";
  char8_t output[2] = {};
  auto result = rfl::utf16_to_utf8(output, output + 2, input, input + 1);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // \u4E2D needs 3 bytes in UTF-8, buffer has 2: no output written, in_ptr NOT consumed
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

// ==================== UTF-16 to UTF-32 ====================

TEST(UtilsStringEncoding, Utf16ToUtf32Empty) {
  char16_t input[] = u"";
  char32_t output[16] = {};
  auto result = rfl::utf16_to_utf32(output, output + 16, input, input);
  // No input processed, no output written
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf16ToUtf32Bmp) {
  char16_t input[] = u"\u00A9\u4E2D";
  char32_t output[16] = {};
  auto result = rfl::utf16_to_utf32(output, output + 16, input, input + 2);
  // 2 BMP chars: out_ptr advanced by 2, in_ptr advanced by 2
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(U'\u00A9', output[0]);
  EXPECT_EQ(U'\u4E2D', output[1]);
}

TEST(UtilsStringEncoding, Utf16ToUtf32SurrogatePair) {
  char16_t input[] = {static_cast<char16_t>(0xD83D), static_cast<char16_t>(0xDE00)};
  char32_t output[16] = {};
  auto result = rfl::utf16_to_utf32(output, output + 16, input, input + 2);
  // 1 surrogate pair -> 1 code point: out_ptr advanced by 1, in_ptr advanced by 2
  EXPECT_EQ(output + 1, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(U'\U0001F600', output[0]);
}

TEST(UtilsStringEncoding, Utf16ToUtf32BufferTooSmall) {
  char16_t input[] = u"AB";
  char32_t output[1] = {};
  auto result = rfl::utf16_to_utf32(output, output + 1, input, input + 2);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // 1 char converted before buffer full: out_ptr at output + 1, in_ptr at input + 1 (NOT consumed,
  // can retry)
  EXPECT_EQ(output + 1, result.out_ptr);
  EXPECT_EQ(input + 1, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf16ToUtf32InvalidSurrogate) {
  char16_t input[] = {static_cast<char16_t>(0xD800)};
  char32_t output[16] = {};
  auto result = rfl::utf16_to_utf32(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // High surrogate without low surrogate: does not consume
  // no output written, in_ptr points to the truncated surrogate (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

// ==================== UTF-32 to UTF-8 ====================

TEST(UtilsStringEncoding, Utf32ToUtf8Empty) {
  char32_t input[] = U"";
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input);
  // No input processed, no output written
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf32ToUtf8Ascii) {
  char32_t input[] = U"AB";
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input + 2);
  // 2 ASCII chars: out_ptr advanced by 2, in_ptr advanced by 2
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(u8'A', output[0]);
  EXPECT_EQ(u8'B', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8TwoByte) {
  char32_t input[] = {U'\u00A9'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input + 1);
  // 1 code point (2 bytes in UTF-8): out_ptr advanced by 2, in_ptr advanced by 1
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(static_cast<char8_t>(0xC2), output[0]);
  EXPECT_EQ(static_cast<char8_t>(0xA9), output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8ThreeByte) {
  char32_t input[] = {U'\u4E2D'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input + 1);
  // 1 code point (3 bytes in UTF-8): out_ptr advanced by 3, in_ptr advanced by 1
  EXPECT_EQ(output + 3, result.out_ptr);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf32ToUtf8FourByte) {
  char32_t input[] = {U'\U0001F600'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input + 1);
  // 1 code point (4 bytes in UTF-8): out_ptr advanced by 4, in_ptr advanced by 1
  EXPECT_EQ(output + 4, result.out_ptr);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf32ToUtf8InvalidSurrogate) {
  char32_t input[] = {static_cast<char32_t>(0xD800)};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Surrogate code point is invalid: does not consume
  // no output written, in_ptr points to the invalid code point (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8TooLarge) {
  char32_t input[] = {static_cast<char32_t>(0x110000)};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Code point > U+10FFFF is invalid: does not consume
  // no output written, in_ptr points to the invalid code point (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8BufferTooSmall) {
  char32_t input[] = {U'\u4E2D'};
  char8_t output[2] = {};
  auto result = rfl::utf32_to_utf8(output, output + 2, input, input + 1);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // \u4E2D needs 3 bytes in UTF-8, buffer has 2: no output written, in_ptr NOT consumed
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

// ==================== UTF-32 to UTF-16 ====================

TEST(UtilsStringEncoding, Utf32ToUtf16Empty) {
  char32_t input[] = U"";
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16(output, output + 16, input, input);
  // No input processed, no output written
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
}

TEST(UtilsStringEncoding, Utf32ToUtf16Bmp) {
  char32_t input[] = {U'\u00A9', U'\u4E2D'};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16(output, output + 16, input, input + 2);
  // 2 BMP chars: out_ptr advanced by 2, in_ptr advanced by 2
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(u'\u00A9', output[0]);
  EXPECT_EQ(u'\u4E2D', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16Supplementary) {
  char32_t input[] = {U'\U0001F600'};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16(output, output + 16, input, input + 1);
  // 1 supplementary char -> surrogate pair: out_ptr advanced by 2, in_ptr advanced by 1
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(static_cast<char16_t>(0xD83D), output[0]);
  EXPECT_EQ(static_cast<char16_t>(0xDE00), output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16InvalidSurrogate) {
  char32_t input[] = {static_cast<char32_t>(0xDC00)};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Surrogate code point is invalid: does not consume
  // no output written, in_ptr points to the invalid code point (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf16TooLarge) {
  char32_t input[] = {static_cast<char32_t>(0x200000)};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Code point > U+10FFFF is invalid: does not consume
  // no output written, in_ptr points to the invalid code point (can retry)
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf16BufferTooSmall) {
  char32_t input[] = {U'\U0001F600'};
  char16_t output[1] = {};
  auto result = rfl::utf32_to_utf16(output, output + 1, input, input + 1);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // Emoji needs 2 UTF-16 code units (surrogate pair), buffer has 1: no output written, in_ptr NOT
  // consumed
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

// ==================== Round-trip tests ====================

TEST(UtilsStringEncoding, RoundTripUtf8ToUtf16ToUtf8) {
  char8_t original[] = u8"Hello\u00A9\u4E2D\U0001F600";
  constexpr auto len = sizeof(u8"Hello\u00A9\u4E2D\U0001F600") - 1;
  char16_t utf16_buf[16] = {};
  char8_t utf8_buf[16] = {};

  auto r1 = rfl::utf8_to_utf16(utf16_buf, utf16_buf + 16, original, original + len);
  EXPECT_EQ(std::errc{}, r1.ec);

  auto r2 = rfl::utf16_to_utf8(utf8_buf, utf8_buf + 16, utf16_buf, r1.out_ptr);
  EXPECT_EQ(std::errc{}, r2.ec);
  EXPECT_EQ(static_cast<ptrdiff_t>(len), r2.out_ptr - utf8_buf);

  for (size_t i = 0; i < len; ++i) {
    EXPECT_EQ(original[i], utf8_buf[i]);
  }
}

TEST(UtilsStringEncoding, RoundTripUtf8ToUtf32ToUtf8) {
  char8_t original[] = u8"Test\u00A9\u4E2D\U0001F600";
  constexpr auto len = sizeof(u8"Test\u00A9\u4E2D\U0001F600") - 1;
  char32_t utf32_buf[16] = {};
  char8_t utf8_buf[16] = {};

  auto r1 = rfl::utf8_to_utf32(utf32_buf, utf32_buf + 16, original, original + len);
  EXPECT_EQ(std::errc{}, r1.ec);

  auto r2 = rfl::utf32_to_utf8(utf8_buf, utf8_buf + 16, utf32_buf, r1.out_ptr);
  EXPECT_EQ(std::errc{}, r2.ec);
  EXPECT_EQ(static_cast<ptrdiff_t>(len), r2.out_ptr - utf8_buf);

  for (size_t i = 0; i < len; ++i) {
    EXPECT_EQ(original[i], utf8_buf[i]);
  }
}

TEST(UtilsStringEncoding, RoundTripUtf16ToUtf32ToUtf16) {
  char16_t original[] = u"Test\u00A9\u4E2D";
  char32_t utf32_buf[16] = {};
  char16_t utf16_buf[16] = {};

  auto r1 = rfl::utf16_to_utf32(utf32_buf, utf32_buf + 16, original, original + 6);
  EXPECT_EQ(std::errc{}, r1.ec);

  auto r2 = rfl::utf32_to_utf16(utf16_buf, utf16_buf + 16, utf32_buf, r1.out_ptr);
  EXPECT_EQ(std::errc{}, r2.ec);
  EXPECT_EQ(6, r2.out_ptr - utf16_buf);

  for (ptrdiff_t i = 0; i < 6; ++i) {
    EXPECT_EQ(original[i], utf16_buf[i]);
  }
}

// ==================== Consume invalid UTF-8 sequence ====================
// consume_utf8_invalid_sequence consumes the maximal prefix of continuous invalid UTF-8 bytes.
// Invalid bytes include: continuation bytes (0x80-0xBF), overlong encodings (0xC0-0xC1),
// and invalid leading bytes (0xF5-0xFF). Valid UTF-8 stops the consumption.

TEST(UtilsStringEncoding, ConsumeUtf8InvalidSequenceEmpty) {
  // Empty input: returns input unchanged
  char8_t input[] = u8"";
  auto end = rfl::consume_utf8_invalid_sequence(input, input);
  EXPECT_EQ(input, end);
}

TEST(UtilsStringEncoding, ConsumeUtf8InvalidSequenceAscii) {
  // ASCII bytes (0x00-0x7F) are valid UTF-8: returns input unchanged
  char8_t input[] = u8"ABC";
  auto end = rfl::consume_utf8_invalid_sequence(input, input + 3);
  EXPECT_EQ(input, end);
}

TEST(UtilsStringEncoding, ConsumeUtf8InvalidSequenceContinuationOnly) {
  // Standalone continuation bytes (0x80-0xBF) are invalid: consume all 3
  char8_t input[] = {
      static_cast<char8_t>(0x80), static_cast<char8_t>(0x81), static_cast<char8_t>(0xBF)};
  auto end = rfl::consume_utf8_invalid_sequence(input, input + 3);
  EXPECT_EQ(input + 3, end);
}

TEST(UtilsStringEncoding, ConsumeUtf8InvalidSequenceOverlong) {
  // Overlong encodings (0xC0-0xC1) are invalid: consume both bytes
  char8_t input[] = {static_cast<char8_t>(0xC0), static_cast<char8_t>(0xC1)};
  auto end = rfl::consume_utf8_invalid_sequence(input, input + 2);
  EXPECT_EQ(input + 2, end);
}

TEST(UtilsStringEncoding, ConsumeUtf8InvalidSequenceInvalidLeading) {
  // Invalid leading bytes (0xF5-0xFF) are invalid: consume both bytes
  char8_t input[] = {static_cast<char8_t>(0xF5), static_cast<char8_t>(0xFF)};
  auto end = rfl::consume_utf8_invalid_sequence(input, input + 2);
  EXPECT_EQ(input + 2, end);
}

TEST(UtilsStringEncoding, ConsumeUtf8InvalidSequenceMixed) {
  // Mixed: continuation byte + overlong leading byte consumed, stops at valid ASCII
  char8_t input[] = {
      static_cast<char8_t>(0x80), static_cast<char8_t>(0xC0), static_cast<char8_t>('A')};
  auto end = rfl::consume_utf8_invalid_sequence(input, input + 3);
  EXPECT_EQ(input + 2, end);
}

TEST(UtilsStringEncoding, ConsumeUtf8InvalidSequenceValidUtf8) {
  // Valid multi-byte UTF-8 sequences: returns input unchanged (no invalid prefix)
  char8_t input[] = u8"\u00A9\u00AE";
  auto end = rfl::consume_utf8_invalid_sequence(input, input + 4);
  EXPECT_EQ(input, end);
}

// ==================== Consume invalid UTF-16 sequence ====================
// consume_utf16_invalid_sequence consumes the maximal prefix of continuous invalid UTF-16 code
// units. Invalid code units include: unpaired surrogates (standalone low surrogates, high
// surrogates without following low surrogate). Valid surrogate pairs and non-surrogate BMP chars
// stop consumption.

TEST(UtilsStringEncoding, ConsumeUtf16InvalidSequenceEmpty) {
  // Empty input: returns input unchanged
  char16_t input[] = u"";
  auto end = rfl::consume_utf16_invalid_sequence(input, input);
  EXPECT_EQ(input, end);
}

TEST(UtilsStringEncoding, ConsumeUtf16InvalidSequenceBmp) {
  // Regular BMP characters (non-surrogate) are valid: returns input unchanged
  char16_t input[] = u"AB";
  auto end = rfl::consume_utf16_invalid_sequence(input, input + 2);
  EXPECT_EQ(input, end);
}

TEST(UtilsStringEncoding, ConsumeUtf16InvalidSequenceLowSurrogateOnly) {
  // Standalone low surrogates (0xDC00-0xDFFF) are invalid: consume both
  char16_t input[] = {static_cast<char16_t>(0xDC00), static_cast<char16_t>(0xDC01)};
  auto end = rfl::consume_utf16_invalid_sequence(input, input + 2);
  EXPECT_EQ(input + 2, end);
}

TEST(UtilsStringEncoding, ConsumeUtf16InvalidSequenceHighSurrogateWithoutLow) {
  // High surrogate (0xD800-0xDBFF) followed by non-surrogate: consume only the high surrogate
  char16_t input[] = {static_cast<char16_t>(0xD800), u'A'};
  auto end = rfl::consume_utf16_invalid_sequence(input, input + 2);
  EXPECT_EQ(input + 1, end);
}

TEST(UtilsStringEncoding, ConsumeUtf16InvalidSequenceHighSurrogateWithLow) {
  // Valid surrogate pair (high + low): returns input unchanged (not invalid)
  char16_t input[] = {static_cast<char16_t>(0xD83D), static_cast<char16_t>(0xDE00)};
  auto end = rfl::consume_utf16_invalid_sequence(input, input + 2);
  EXPECT_EQ(input, end);
}

TEST(UtilsStringEncoding, ConsumeUtf16InvalidSequenceTruncatedSurrogate) {
  // Truncated surrogate at end of input: consume the high surrogate
  char16_t input[] = {static_cast<char16_t>(0xD800)};
  auto end = rfl::consume_utf16_invalid_sequence(input, input + 1);
  EXPECT_EQ(input + 1, end);
}

TEST(UtilsStringEncoding, Utf8ToUtf16JsonEscapedBasic) {
  char8_t input[] = u8"Hello";
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16_json_escaped(output, output + 16, input, input + 5);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(output + 5, result.out_ptr);
  EXPECT_EQ(input + 5, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16JsonEscapedControlChar) {
  char8_t input[] = {char8_t(0x01), char8_t(0x0A), char8_t(0x7F)};
  char16_t output[32] = {};
  auto result = rfl::utf8_to_utf16_json_escaped(output, output + 32, input, input + 3);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 3, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedBasic) {
  char32_t input[] = {U'A', U'B', U'C'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 16, input, input + 3);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 3, result.in_ptr);
  EXPECT_EQ(output + 3, result.out_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedControlChar) {
  char32_t input[] = {U'\n', U'\t'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 2, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedDoubleQuote) {
  char32_t input[] = {U'"'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'"', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedBackslash) {
  char32_t input[] = {U'\\'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'\\', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedSlash) {
  char32_t input[] = {U'/'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'/', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedBackspace) {
  char32_t input[] = {U'\b'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'b', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedFormFeed) {
  char32_t input[] = {U'\f'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'f', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedNewline) {
  char32_t input[] = {U'\n'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'n', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedCarriageReturn) {
  char32_t input[] = {U'\r'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'r', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedTab) {
  char32_t input[] = {U'\t'};
  char8_t output[8] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8't', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedControlCharU0000) {
  char32_t input[] = {U'\x00'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'u', output[1]);
  EXPECT_EQ(u8'0', output[2]);
  EXPECT_EQ(u8'0', output[3]);
  EXPECT_EQ(u8'0', output[4]);
  EXPECT_EQ(u8'0', output[5]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedControlCharU001F) {
  char32_t input[] = {U'\x1F'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\\', output[0]);
  EXPECT_EQ(u8'u', output[1]);
  EXPECT_EQ(u8'0', output[2]);
  EXPECT_EQ(u8'0', output[3]);
  EXPECT_EQ(u8'1', output[4]);
  EXPECT_EQ(u8'F', output[5]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedNoEscapeForNormalChars) {
  char32_t input[] = {U'A', U'B', U'C'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 16, input, input + 3);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 3, result.in_ptr);
  EXPECT_EQ(output + 3, result.out_ptr);
  EXPECT_EQ(u8'A', output[0]);
  EXPECT_EQ(u8'B', output[1]);
  EXPECT_EQ(u8'C', output[2]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedNoEscapeForU007F) {
  char32_t input[] = {U'\x7F'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u8'\x7F', output[0]);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedEmpty) {
  char32_t input[] = {};
  char8_t output[4] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 4, input, input);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(output, result.out_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8JsonEscapedMixed) {
  char32_t input[] = {U'A', U'"', U'B'};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8_json_escaped(output, output + 16, input, input + 3);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 3, result.in_ptr);
  EXPECT_EQ(u8'A', output[0]);
  EXPECT_EQ(u8'\\', output[1]);
  EXPECT_EQ(u8'"', output[2]);
  EXPECT_EQ(u8'B', output[3]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedDoubleQuote) {
  char32_t input[] = {U'"'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'"', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedBackslash) {
  char32_t input[] = {U'\\'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'\\', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedSlash) {
  char32_t input[] = {U'/'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'/', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedBackspace) {
  char32_t input[] = {U'\b'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'b', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedFormFeed) {
  char32_t input[] = {U'\f'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'f', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedNewline) {
  char32_t input[] = {U'\n'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'n', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedCarriageReturn) {
  char32_t input[] = {U'\r'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'r', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedTab) {
  char32_t input[] = {U'\t'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u't', output[1]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedControlCharU0000) {
  char32_t input[] = {U'\x00'};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'u', output[1]);
  EXPECT_EQ(u'0', output[2]);
  EXPECT_EQ(u'0', output[3]);
  EXPECT_EQ(u'0', output[4]);
  EXPECT_EQ(u'0', output[5]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedControlCharU001F) {
  char32_t input[] = {U'\x1F'};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\\', output[0]);
  EXPECT_EQ(u'u', output[1]);
  EXPECT_EQ(u'0', output[2]);
  EXPECT_EQ(u'0', output[3]);
  EXPECT_EQ(u'1', output[4]);
  EXPECT_EQ(u'F', output[5]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedNoEscapeForNormalChars) {
  char32_t input[] = {U'A', U'B', U'C'};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 16, input, input + 3);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 3, result.in_ptr);
  EXPECT_EQ(output + 3, result.out_ptr);
  EXPECT_EQ(u'A', output[0]);
  EXPECT_EQ(u'B', output[1]);
  EXPECT_EQ(u'C', output[2]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedNoEscapeForU007F) {
  char32_t input[] = {U'\x7F'};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(u'\x7F', output[0]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedEmpty) {
  char32_t input[] = {};
  char16_t output[4] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 4, input, input);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input, result.in_ptr);
  EXPECT_EQ(output, result.out_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedMixed) {
  char32_t input[] = {U'A', U'"', U'B'};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 16, input, input + 3);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 3, result.in_ptr);
  EXPECT_EQ(u'A', output[0]);
  EXPECT_EQ(u'\\', output[1]);
  EXPECT_EQ(u'"', output[2]);
  EXPECT_EQ(u'B', output[3]);
}

TEST(UtilsStringEncoding, Utf32ToUtf16JsonEscapedSurrogatePair) {
  char32_t input[] = {U'\U0001F600'};
  char16_t output[8] = {};
  auto result = rfl::utf32_to_utf16_json_escaped(output, output + 8, input, input + 1);
  EXPECT_EQ(std::errc{}, result.ec);
  EXPECT_EQ(input + 1, result.in_ptr);
  EXPECT_EQ(output + 2, result.out_ptr);
}
