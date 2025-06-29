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

#include "tests/test_options.hpp"
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <string_view>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#endif

namespace rfl = reflect_cpp26;

// -------- string_like --------

// String and string view
static_assert(rfl::is_string_like_v<std::string>);
static_assert(rfl::is_string_like_v<std::wstring_view>);
static_assert(rfl::is_string_like_v<rfl::meta_u8string_view>);
// Character array
static_assert(rfl::is_string_like_v<char16_t*>);
static_assert(rfl::is_string_like_v<char32_t[]>);
static_assert(rfl::is_string_like_v<char32_t[32]>);

// With cvref qualifiers
static_assert(rfl::is_string_like_v<const std::string>);
static_assert(rfl::is_string_like_v<const std::wstring_view &>);
static_assert(rfl::is_string_like_v<const rfl::meta_u8string_view &&>);
// Volatile values are not allowed
// as volatile CharT* -> const CharT* is disallowed.
static_assert(NOT rfl::is_string_like_v<volatile std::wstring_view>);
static_assert(NOT rfl::is_string_like_v<volatile rfl::meta_u8string_view &>);
// Pointer
static_assert(rfl::is_string_like_v<const char16_t* const>);
static_assert(rfl::is_string_like_v<const char16_t* const &>);
static_assert(rfl::is_string_like_v<char16_t* volatile>);
static_assert(rfl::is_string_like_v<char16_t* volatile &&>);
// volatile CharT* -> const CharT* is disallowed.
static_assert(NOT rfl::is_string_like_v<volatile char16_t*>);
// Array
static_assert(rfl::is_string_like_v<const char32_t[]>);
static_assert(rfl::is_string_like_v<const char32_t[32]>);
static_assert(rfl::is_string_like_v<char32_t (&)[]>);
static_assert(rfl::is_string_like_v<char32_t (&)[32]>);
static_assert(NOT rfl::is_string_like_v<const volatile char[]>);
static_assert(NOT rfl::is_string_like_v<const volatile char[32]>);
static_assert(NOT rfl::is_string_like_v<const volatile char (&)[]>);
static_assert(NOT rfl::is_string_like_v<const volatile char (&)[32]>);

// Other types
static_assert(NOT rfl::is_string_like_v<int>);
static_assert(NOT rfl::is_string_like_v<char>);
static_assert(NOT rfl::is_string_like_v<std::vector<char>>);
static_assert(NOT rfl::is_string_like_v<std::array<char, 32>>);
static_assert(NOT rfl::is_string_like_v<int[]>);
static_assert(NOT rfl::is_string_like_v<int[32]>);
static_assert(NOT rfl::is_string_like_v<std::array<const char*, 2>>);
static_assert(NOT rfl::is_string_like_v<std::pair<const char*, const char*>>);

// -------- string_like_of --------

static_assert(rfl::is_string_like_of_v<std::string, char>);
static_assert(rfl::is_string_like_of_v<std::wstring_view, wchar_t>);
static_assert(rfl::is_string_like_of_v<rfl::meta_u8string_view, char8_t>);
static_assert(NOT rfl::is_string_like_of_v<std::string, char16_t>);
static_assert(NOT rfl::is_string_like_of_v<std::wstring_view, char32_t>);
static_assert(NOT rfl::is_string_like_of_v<rfl::meta_u8string_view, char16_t>);

static_assert(rfl::is_string_like_of_v<char16_t*, char16_t>);
static_assert(rfl::is_string_like_of_v<char32_t[], char32_t>);
static_assert(rfl::is_string_like_of_v<char32_t[32], char32_t>);
static_assert(NOT rfl::is_string_like_of_v<char16_t*, char>);
static_assert(NOT rfl::is_string_like_of_v<char32_t[], char>);
static_assert(NOT rfl::is_string_like_of_v<char32_t[32], wchar_t>);

static_assert(rfl::is_string_like_of_v<const std::string, char>);
static_assert(rfl::is_string_like_of_v<const std::wstring_view &, wchar_t>);
static_assert(NOT rfl::is_string_like_of_v<volatile std::string_view, char>);
static_assert(NOT rfl::is_string_like_of_v<const std::string, char8_t>);
static_assert(NOT rfl::is_string_like_of_v<const std::wstring_view &, char8_t>);
static_assert(NOT rfl::is_string_like_of_v<volatile std::string_view, char8_t>);

static_assert(rfl::is_string_like_of_v<const char16_t* const, char16_t>);
static_assert(rfl::is_string_like_of_v<const char16_t* const &, char16_t>);
static_assert(rfl::is_string_like_of_v<char16_t* volatile, char16_t>);
static_assert(rfl::is_string_like_of_v<char16_t* volatile &&, char16_t>);
static_assert(NOT rfl::is_string_like_of_v<const char16_t* const, wchar_t>);
static_assert(NOT rfl::is_string_like_of_v<const char16_t* const &, char8_t>);
static_assert(NOT rfl::is_string_like_of_v<char16_t* volatile, char32_t>);
static_assert(NOT rfl::is_string_like_of_v<char16_t* volatile &&, char>);

static_assert(NOT rfl::is_string_like_of_v<volatile char16_t*, char16_t>);
static_assert(NOT rfl::is_string_like_of_v<volatile char16_t*, char32_t>);

static_assert(rfl::is_string_like_of_v<const char32_t[], char32_t>);
static_assert(rfl::is_string_like_of_v<const char32_t[32], char32_t>);
static_assert(rfl::is_string_like_of_v<char32_t (&)[], char32_t>);
static_assert(rfl::is_string_like_of_v<char32_t (&)[32], char32_t>);
static_assert(NOT rfl::is_string_like_of_v<const volatile char[], char>);
static_assert(NOT rfl::is_string_like_of_v<const volatile char[32], char>);
static_assert(NOT rfl::is_string_like_of_v<const volatile char (&)[], char>);
static_assert(NOT rfl::is_string_like_of_v<const volatile char (&)[32], char>);
static_assert(NOT rfl::is_string_like_of_v<const char32_t[], char>);
static_assert(NOT rfl::is_string_like_of_v<const char32_t[32], char8_t>);
static_assert(NOT rfl::is_string_like_of_v<char32_t (&)[], char>);
static_assert(NOT rfl::is_string_like_of_v<char32_t (&)[32], char16_t>);
static_assert(NOT rfl::is_string_like_of_v<const volatile char[], char8_t>);
static_assert(NOT rfl::is_string_like_of_v<const volatile char (&)[], char8_t>);

static_assert(NOT rfl::is_string_like_of_v<int, wchar_t>);
static_assert(NOT rfl::is_string_like_of_v<char, char>);
static_assert(NOT rfl::is_string_like_of_v<std::vector<char>, char>);
static_assert(NOT rfl::is_string_like_of_v<std::array<char, 32>, char>);
static_assert(NOT rfl::is_string_like_of_v<int[], char>);
static_assert(NOT rfl::is_string_like_of_v<int[32], char8_t>);
static_assert(NOT rfl::is_string_like_of_v<std::array<const char*, 2>, char>);

// -------- are_string_like_of_same_type_v

static_assert(rfl::are_string_like_of_same_char_type_v<
  std::string, std::string_view>);
static_assert(rfl::are_string_like_of_same_char_type_v<
  const std::wstring&, const std::wstring_view&, rfl::meta_wstring_view>);
static_assert(rfl::are_string_like_of_same_char_type_v<
  std::u8string&&, const std::u8string_view&&, char8_t[32]>);
static_assert(rfl::are_string_like_of_same_char_type_v<
  std::u16string, char16_t (&)[32], const char16_t* volatile>);

static_assert(NOT rfl::are_string_like_of_same_char_type_v<
  std::string, std::wstring_view>);
static_assert(NOT rfl::are_string_like_of_same_char_type_v<
  const std::string&, const std::string_view&, rfl::meta_u8string_view>);
static_assert(NOT rfl::are_string_like_of_same_char_type_v<
  std::u8string&&, char[32]>);
static_assert(NOT rfl::are_string_like_of_same_char_type_v<
  std::u32string, char16_t (&)[32], const char16_t* volatile>);

static_assert(NOT rfl::are_string_like_of_same_char_type_v<
  char[], wchar_t[], char8_t[], char16_t[], char32_t[]>);

static_assert(NOT rfl::are_string_like_of_same_char_type_v<
  char[], const char*, const char>);
static_assert(NOT rfl::are_string_like_of_same_char_type_v<
  char[], const char*, unsigned char>);

TEST(TypeTraits, StringLikeTypes) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
