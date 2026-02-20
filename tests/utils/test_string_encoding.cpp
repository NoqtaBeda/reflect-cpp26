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
  // 3 input characters processed before buffer full: out_ptr at output + 3, in_ptr at input + 3
  EXPECT_EQ(output + 3, result.out_ptr);
  EXPECT_EQ(input + 3, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16InvalidContinuation) {
  char8_t input[] = {static_cast<char8_t>(0xC2), static_cast<char8_t>(0x41)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Error at first byte (0xC2 expects continuation, got 0x41):
  // no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16OverlongEncoding) {
  char8_t input[] = {static_cast<char8_t>(0xC0), static_cast<char8_t>(0x80)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Overlong encoding (0xC0 0x80 would encode NUL, but is invalid):
  // no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16TruncatedSequence) {
  char8_t input[] = {static_cast<char8_t>(0xE2)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // 0xE2 starts 3-byte sequence but no continuation bytes:
  // no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf8ToUtf16SurrogateInUtf8) {
  char8_t input[] = {
      static_cast<char8_t>(0xED), static_cast<char8_t>(0xA0), static_cast<char8_t>(0x80)};
  char16_t output[16] = {};
  auto result = rfl::utf8_to_utf16(output, output + 16, input, input + 3);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // 0xED 0xA0 0x80 encodes U+D800 (surrogate), which is invalid in UTF-8:
  // no output written, in_ptr at error position
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
  // 2 chars converted before buffer full: out_ptr at output + 2, in_ptr at input + 2
  EXPECT_EQ(output + 2, result.out_ptr);
  EXPECT_EQ(input + 2, result.in_ptr);
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
  // Low surrogate without high surrogate: no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf16ToUtf8HighSurrogateOnly) {
  char16_t input[] = {static_cast<char16_t>(0xD800)};
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // High surrogate without low surrogate (end of input): no output written, in_ptr at error
  // position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf16ToUtf8HighSurrogateWithoutLow) {
  char16_t input[] = {static_cast<char16_t>(0xD800), u'A'};
  char8_t output[16] = {};
  auto result = rfl::utf16_to_utf8(output, output + 16, input, input + 2);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // High surrogate followed by non-low-surrogate: no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf16ToUtf8BufferTooSmall) {
  char16_t input[] = u"\u4E2D";
  char8_t output[2] = {};
  auto result = rfl::utf16_to_utf8(output, output + 2, input, input + 1);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // \u4E2D needs 3 bytes in UTF-8, buffer has 2: no output written, in_ptr at error position
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
  // 1 char converted before buffer full: out_ptr at output + 1, in_ptr at input + 1
  EXPECT_EQ(output + 1, result.out_ptr);
  EXPECT_EQ(input + 1, result.in_ptr);
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
  // Surrogate code point is invalid: no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8TooLarge) {
  char32_t input[] = {static_cast<char32_t>(0x110000)};
  char8_t output[16] = {};
  auto result = rfl::utf32_to_utf8(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Code point > U+10FFFF is invalid: no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf8BufferTooSmall) {
  char32_t input[] = {U'\u4E2D'};
  char8_t output[2] = {};
  auto result = rfl::utf32_to_utf8(output, output + 2, input, input + 1);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // \u4E2D needs 3 bytes in UTF-8, buffer has 2: no output written, in_ptr at error position
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
  // Surrogate code point is invalid: no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf16TooLarge) {
  char32_t input[] = {static_cast<char32_t>(0x200000)};
  char16_t output[16] = {};
  auto result = rfl::utf32_to_utf16(output, output + 16, input, input + 1);
  EXPECT_EQ(std::errc::invalid_argument, result.ec);
  // Code point > U+10FFFF is invalid: no output written, in_ptr at error position
  EXPECT_EQ(output, result.out_ptr);
  EXPECT_EQ(input, result.in_ptr);
}

TEST(UtilsStringEncoding, Utf32ToUtf16BufferTooSmall) {
  char32_t input[] = {U'\U0001F600'};
  char16_t output[1] = {};
  auto result = rfl::utf32_to_utf16(output, output + 1, input, input + 1);
  EXPECT_EQ(std::errc::value_too_large, result.ec);
  // Emoji needs 2 UTF-16 code units (surrogate pair), buffer has 1: no output written, in_ptr at
  // error position
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
