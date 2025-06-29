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

#include "tests/utils/fixed_map/string_key/string_key_test_options.hpp"
#include <reflect_cpp26/utils/fixed_map/string_key.hpp>

namespace rfl = reflect_cpp26;

template <class CharT, bool CI>
void test_by_length_common()
{
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<double>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {to<CharT>("Apple"), {.value = 12.5}},
      {to<CharT>("Pineapple"), {.value = 25.0}},
      {to<CharT>("Pen"), {.value = 37.5}},
      {to<CharT>("Banana"), {.value = 50.0}},
      {to<CharT>("Strawberry"), {.value = 62.5}},
      {to<CharT>(""), {.value = 75.0}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .case_insensitive = CI,
    .optimization_threshold = 4,
  });
  static_assert(std::is_same_v<
    typename decltype(map)::result_type, const wrapper_t<double>&>);

  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_length"));
  EXPECT_EQ_STATIC(6, map.size());

  EXPECT_EQ_STATIC(12.5, map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(25.0, map, to<CharT>("Pineapple"));
  EXPECT_FOUND_STATIC(37.5, map, to<CharT>("Pen"));
  EXPECT_FOUND_STATIC(50.0, map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(62.5, map, to<CharT>("Strawberry"));
  EXPECT_FOUND_STATIC(75.0, map, to<CharT>(""));

  EXPECT_EQ_STATIC(CI ? 12.5 : magic_value, map[to<CharT>("apple")]);
  EXPECT_EQ_STATIC(CI ? 25.0 : magic_value, map[to<CharT>("PINEAPPLE")]);
  EXPECT_EQ_STATIC(CI ? 37.5 : magic_value, map[to<CharT>("pEn")]);
  EXPECT_EQ_STATIC(CI ? 50.0 : magic_value, map[to<CharT>("BaNaNa")]);
  EXPECT_EQ_STATIC(CI ? 62.5 : magic_value, map[to<CharT>("StrawBerry")]);

  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("App"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Cat"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Berry"));
}

#define FOR_EACH_CHARACTER_TYPE(F)  \
  F(char, Char)                     \
  F(wchar_t, WChar)                 \
  F(char8_t, Char8)

#define MAKE_BY_LENGTH_MAP_TESTS(char_type, CharTypeName)   \
  TEST(UtilsFixedMap, StringKeyByLength##CharTypeName) {    \
    test_by_length_common<char_type, false>();              \
  }                                                         \
  TEST(UtilsFixedMap, StringKeyByLengthCI##CharTypeName) {  \
    test_by_length_common<char_type, true>();               \
  }

FOR_EACH_CHARACTER_TYPE(MAKE_BY_LENGTH_MAP_TESTS)
