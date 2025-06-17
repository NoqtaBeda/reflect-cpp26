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

#include <reflect_cpp26/fixed_map/string_key.hpp>

#include "tests/fixed_map/string_key/string_key_test_options.hpp"

namespace rfl = reflect_cpp26;

template <class CharT>
void test_empty_common() {
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  constexpr auto map_empty = FIXED_MAP(std::vector<KVPair>{});
  constexpr auto ci_map_empty = FIXED_MAP(std::vector<KVPair>{},
                                          {
                                              .ascii_case_insensitive = true,
                                          });

  EXPECT_THAT(display_string_of(^^decltype(map_empty)), testing::HasSubstr("empty_string_key_map"));
  EXPECT_THAT(display_string_of(^^decltype(ci_map_empty)),
              testing::HasSubstr("empty_string_key_map"));

  EXPECT_EQ_STATIC(0, map_empty.size());
  EXPECT_EQ_STATIC(magic_value, map_empty[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map_empty, to<CharT>("hello"));

  EXPECT_EQ_STATIC(0, ci_map_empty.size());
  EXPECT_EQ_STATIC(magic_value, ci_map_empty[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, ci_map_empty, to<CharT>("hello"));
}

#define FOR_EACH_CHARACTER_TYPE(F) \
  F(char, Char)                    \
  F(wchar_t, WChar)                \
  F(char8_t, Char8)                \
  F(char16_t, Char16)              \
  F(char32_t, Char32)

#define MAKE_EMPTY_MAP_TESTS(char_type, CharTypeName) \
  TEST(FixedMap, StringKeyEmpty##CharTypeName) {      \
    test_empty_common<char_type>();                   \
  }

FOR_EACH_CHARACTER_TYPE(MAKE_EMPTY_MAP_TESTS)
