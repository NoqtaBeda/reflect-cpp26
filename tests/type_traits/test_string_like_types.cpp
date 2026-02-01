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

#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <string_view>

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#endif

namespace rfl = reflect_cpp26;

// From cppreference. Used as example of custom character traits.
struct ci_char_traits : public std::char_traits<char> {
  static char to_upper(char ch) {
    return std::toupper((unsigned char)ch);
  }

  static bool eq(char c1, char c2) {
    return to_upper(c1) == to_upper(c2);
  }

  static bool lt(char c1, char c2) {
    return to_upper(c1) < to_upper(c2);
  }

  static int compare(const char* s1, const char* s2, std::size_t n) {
    for (; n-- != 0; ++s1, ++s2) {
      if (to_upper(*s1) < to_upper(*s2)) return -1;
      if (to_upper(*s1) > to_upper(*s2)) return 1;
    }
    return 0;
  }

  static const char* find(const char* s, std::size_t n, char a) {
    const auto ua{to_upper(a)};
    for (; n-- != 0; s++)
      if (to_upper(*s) == ua) return s;
    return nullptr;
  }
};

TEST(TypeTraits, StringLike) {
  // String and string view: ✔️
  static_assert(rfl::string_like<std::string>);
  static_assert(rfl::string_like<std::wstring_view>);
  static_assert(rfl::string_like<rfl::meta_u8string_view>);
  // String with custom char traits and allocator types: ✔️
  static_assert(rfl::string_like<std::pmr::basic_string<char, ci_char_traits>>);
  // String view with custom char traits: ✔️
  static_assert(rfl::string_like<std::basic_string_view<char, ci_char_traits>>);
  // Const-qualified: ✔️
  static_assert(rfl::string_like<const std::string>);
  // Volatile-qualified: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::string_like<volatile std::wstring_view>);
  static_assert(NOT rfl::string_like<volatile rfl::meta_u8string_view>);
  // Reference-qualified: ❌
  static_assert(NOT rfl::string_like<const std::wstring_view&>);
  static_assert(NOT rfl::string_like<const rfl::meta_u8string_view&&>);

  // Pointer to (maybe const-qualified) characters: ✔️
  static_assert(rfl::string_like<char16_t*>);
  static_assert(rfl::string_like<const char32_t*>);
  // Pointers with cv-qualifiers itself: ✔️
  static_assert(rfl::string_like<const char16_t* const>);
  static_assert(rfl::string_like<char16_t* volatile>);
  // Pointer to volatile-qualified characters: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::string_like<volatile char16_t*>);
  static_assert(NOT rfl::string_like<const volatile char16_t*>);
  // Pointers with ref-qualifiers itself: ❌
  static_assert(NOT rfl::string_like<const char16_t* const&>);
  static_assert(NOT rfl::string_like<char16_t* volatile&&>);

  // Array of (maybe const-qualified) characters: ✔️
  static_assert(rfl::string_like<char32_t[]>);
  static_assert(rfl::string_like<char32_t[32]>);
  static_assert(rfl::string_like<const char32_t[]>);
  static_assert(rfl::string_like<const char32_t[32]>);
  // Array of volatile-qualified characters: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::string_like<const volatile char[]>);
  static_assert(NOT rfl::string_like<const volatile char[32]>);
  // Array which is ref-qualified itseif: ❌
  static_assert(NOT rfl::string_like<char32_t (&)[]>);
  static_assert(NOT rfl::string_like<char32_t (&)[32]>);
  static_assert(NOT rfl::string_like<const volatile char (&)[]>);
  static_assert(NOT rfl::string_like<const volatile char (&)[32]>);

  // Contiguous ranges of character type: ✔️
  static_assert(rfl::string_like<std::vector<char>>);
  static_assert(rfl::string_like<std::array<wchar_t, 32>>);
  static_assert(rfl::string_like<std::ranges::subrange<const char16_t*, const char16_t*>>);

  // Other types: ❌
  static_assert(NOT rfl::string_like<int>);
  static_assert(NOT rfl::string_like<char>);
  static_assert(NOT rfl::string_like<int[]>);
  static_assert(NOT rfl::string_like<int[32]>);
  static_assert(NOT rfl::string_like<const int8_t*>);
  static_assert(NOT rfl::string_like<std::array<const char*, 2>>);
  static_assert(NOT rfl::string_like<std::pair<const char*, const char*>>);
  static_assert(NOT rfl::string_like<std::vector<std::byte>>);
  static_assert(NOT rfl::string_like<std::array<uint8_t, 4096>>);
}

TEST(TypeTraits, StringLikeOf) {
  // String and string view: ✔️
  static_assert(rfl::string_like_of<std::string, char>);
  static_assert(rfl::string_like_of<std::wstring_view, wchar_t>);
  static_assert(rfl::string_like_of<rfl::meta_u8string_view, char8_t>);
  // String with custom char traits and allocator types: ✔️
  static_assert(rfl::string_like_of<std::pmr::basic_string<char, ci_char_traits>, char>);
  // String view with custom char traits: ✔️
  static_assert(rfl::string_like_of<std::basic_string_view<char, ci_char_traits>, char>);
  // Const-qualified: ✔️
  static_assert(rfl::string_like_of<const std::string, char>);
  // Volatile-qualified: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::string_like_of<volatile std::wstring_view, wchar_t>);
  static_assert(NOT rfl::string_like_of<volatile rfl::meta_u8string_view, char8_t>);
  // Reference-qualified: ❌
  static_assert(NOT rfl::string_like_of<const std::wstring_view&, wchar_t>);
  static_assert(NOT rfl::string_like_of<const rfl::meta_u8string_view&&, char8_t>);

  // Pointer to (maybe const-qualified) characters: ✔️
  static_assert(rfl::string_like_of<char16_t*, char16_t>);
  static_assert(rfl::string_like_of<const char32_t*, char32_t>);
  // Pointers with cv-qualifiers itself: ✔️
  static_assert(rfl::string_like_of<const char16_t* const, char16_t>);
  static_assert(rfl::string_like_of<char16_t* volatile, char16_t>);
  // Pointer to volatile-qualified characters: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::string_like_of<volatile char16_t*, char16_t>);
  static_assert(NOT rfl::string_like_of<const volatile char16_t*, char16_t>);
  // Pointers with ref-qualifiers itself: ❌
  static_assert(NOT rfl::string_like_of<const char16_t* const&, char16_t>);
  static_assert(NOT rfl::string_like_of<char16_t* volatile&&, char16_t>);

  // Array of (maybe const-qualified) characters: ✔️
  static_assert(rfl::string_like_of<char32_t[], char32_t>);
  static_assert(rfl::string_like_of<char32_t[32], char32_t>);
  static_assert(rfl::string_like_of<const char32_t[], char32_t>);
  static_assert(rfl::string_like_of<const char32_t[32], char32_t>);
  // Array of volatile-qualified characters: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::string_like_of<const volatile char[], char>);
  static_assert(NOT rfl::string_like_of<const volatile char[32], char>);
  // Array which is ref-qualified itseif: ❌
  static_assert(NOT rfl::string_like_of<char32_t (&)[], char32_t>);
  static_assert(NOT rfl::string_like_of<char32_t (&)[32], char32_t>);
  static_assert(NOT rfl::string_like_of<const volatile char (&)[], char>);
  static_assert(NOT rfl::string_like_of<const volatile char (&)[32], char>);

  // Contiguous ranges of character type: ✔️
  static_assert(rfl::string_like_of<std::vector<char>, char>);
  static_assert(rfl::string_like_of<std::array<wchar_t, 32>, wchar_t>);
  static_assert(
      rfl::string_like_of<std::ranges::subrange<const char16_t*, const char16_t*>, char16_t>);

  // Other types: ❌
  static_assert(NOT rfl::string_like_of<int, int>);
  static_assert(NOT rfl::string_like_of<char, char>);
  static_assert(NOT rfl::string_like_of<int[], int>);
  static_assert(NOT rfl::string_like_of<int[32], int>);
  static_assert(NOT rfl::string_like_of<const int8_t*, int8_t>);
  static_assert(NOT rfl::string_like_of<std::array<const char*, 2>, char*>);
  static_assert(NOT rfl::string_like_of<std::pair<const char*, const char*>, const char*>);
  static_assert(NOT rfl::string_like_of<std::vector<std::byte>, std::byte>);
  static_assert(NOT rfl::string_like_of<std::array<uint8_t, 4096>, uint8_t>);

  // String-like types with mismatched character type: ❌
  // char vs. wchar_t
  static_assert(NOT rfl::string_like_of<std::string, wchar_t>);
  // wchar_t vs. char
  static_assert(NOT rfl::string_like_of<std::wstring_view, char>);
  // char8_t vs. char16_t
  static_assert(NOT rfl::string_like_of<rfl::meta_u8string_view, char16_t>);
  // char vs. char16_t
  static_assert(NOT rfl::string_like_of<std::pmr::basic_string<char, ci_char_traits>, char16_t>);
  // char vs. char8_t
  static_assert(NOT rfl::string_like_of<std::basic_string_view<char, ci_char_traits>, char8_t>);
  // char16_t vs. char32_t
  static_assert(NOT rfl::string_like_of<char16_t*, char32_t>);
  // char32_t vs. char16_t
  static_assert(NOT rfl::string_like_of<const char32_t*, char16_t>);
  // char32_t vs. wchar_t
  static_assert(NOT rfl::string_like_of<char32_t[32], wchar_t>);
  // char vs. char8_t
  static_assert(NOT rfl::string_like_of<std::vector<char>, char8_t>);
  // wchar_t vs. char
  static_assert(NOT rfl::string_like_of<std::array<wchar_t, 32>, char>);
  // char16_t vs. char32_t
  static_assert(
      NOT rfl::string_like_of<std::ranges::subrange<const char16_t*, const char16_t*>, char32_t>);
}

TEST(TypeTraits, CStyleString) {
  // Pointer to (maybe const-qualified) characters: ✔️
  static_assert(rfl::c_style_string<char16_t*>);
  static_assert(rfl::c_style_string<const char32_t*>);
  // Pointers with cv-qualifiers itself: ✔️
  static_assert(rfl::c_style_string<const char16_t* const>);
  static_assert(rfl::c_style_string<char16_t* volatile>);
  // Pointer to volatile-qualified characters: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::c_style_string<volatile char16_t*>);
  static_assert(NOT rfl::c_style_string<const volatile char16_t*>);
  // Pointers with ref-qualifiers itself: ❌
  static_assert(NOT rfl::c_style_string<const char16_t* const&>);
  static_assert(NOT rfl::c_style_string<char16_t* volatile&&>);

  // Array of (maybe const-qualified) characters: ✔️
  static_assert(rfl::c_style_string<char32_t[]>);
  static_assert(rfl::c_style_string<char32_t[32]>);
  static_assert(rfl::c_style_string<const char32_t[]>);
  static_assert(rfl::c_style_string<const char32_t[32]>);
  // Array of volatile-qualified characters: ❌ (as volatile CharT* -> const CharT* is disallowed)
  static_assert(NOT rfl::c_style_string<const volatile char[]>);
  static_assert(NOT rfl::c_style_string<const volatile char[32]>);
  // Array which is ref-qualified itseif: ❌
  static_assert(NOT rfl::c_style_string<char32_t (&)[]>);
  static_assert(NOT rfl::c_style_string<char32_t (&)[32]>);
  static_assert(NOT rfl::c_style_string<const volatile char (&)[]>);
  static_assert(NOT rfl::c_style_string<const volatile char (&)[32]>);

  // ---- The following cases are all evaluated to false as they are not C-style strings ----
  // String and string view: ❌ (not C-style strings)
  static_assert(NOT rfl::c_style_string<std::string>);
  static_assert(NOT rfl::c_style_string<std::wstring_view>);
  static_assert(NOT rfl::c_style_string<rfl::meta_u8string_view>);
  // String with custom char traits and allocator types: ❌
  static_assert(NOT rfl::c_style_string<std::pmr::basic_string<char, ci_char_traits>>);
  // String view with custom char traits: ❌
  static_assert(NOT rfl::c_style_string<std::basic_string_view<char, ci_char_traits>>);
  // Const-qualified: ❌
  static_assert(NOT rfl::c_style_string<const std::string>);
  // Volatile-qualified: ❌
  static_assert(NOT rfl::c_style_string<volatile std::wstring_view>);
  static_assert(NOT rfl::c_style_string<volatile rfl::meta_u8string_view>);
  // Reference-qualified: ❌
  static_assert(NOT rfl::c_style_string<const std::wstring_view&>);
  static_assert(NOT rfl::c_style_string<const rfl::meta_u8string_view&&>);

  // Contiguous ranges of character type: ❌ (not C-style strings)
  static_assert(NOT rfl::c_style_string<std::vector<char>>);
  static_assert(NOT rfl::c_style_string<std::array<wchar_t, 32>>);
  static_assert(NOT rfl::c_style_string<std::ranges::subrange<const char16_t*, const char16_t*>>);

  // Other types: ❌
  static_assert(NOT rfl::c_style_string<int>);
  static_assert(NOT rfl::c_style_string<char>);
  static_assert(NOT rfl::c_style_string<int[]>);
  static_assert(NOT rfl::c_style_string<int[32]>);
  static_assert(NOT rfl::c_style_string<const int8_t*>);
  static_assert(NOT rfl::c_style_string<std::array<const char*, 2>>);
  static_assert(NOT rfl::c_style_string<std::pair<const char*, const char*>>);
  static_assert(NOT rfl::c_style_string<std::vector<std::byte>>);
  static_assert(NOT rfl::c_style_string<std::array<uint8_t, 4096>>);
}
