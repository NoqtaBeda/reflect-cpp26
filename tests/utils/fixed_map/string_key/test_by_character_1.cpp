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
void test_by_character_common_1()
{
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    // Differs at I == 1: 'p', 'i', 'e', 'a', 't', 'o', 'h', 'w'
    return std::vector<KVPair>{
      {to<CharT>("Apple"), {.value = 10}},
      {to<CharT>("Pineapple"), {.value = 20}},
      {to<CharT>("Pen"), {.value = 30}},
      {to<CharT>("Banana"), {.value = 40}},
      {to<CharT>("Strawberry"), {.value = 50}},
      {to<CharT>("Dog"), {.value = 60}},
      {to<CharT>("Sheep"), {.value = 70}},
      {to<CharT>("Owl"), {.value = 80}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .case_insensitive = CI,
    .enables_lookup_by_differed_character = true,
    .optimization_threshold = 4,
  });
  static_assert(std::is_same_v<
    typename decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_character"));
  EXPECT_EQ_STATIC(1, map.differed_index);
  EXPECT_EQ_STATIC(8, map.size());

  EXPECT_EQ_STATIC(10, map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(20, map, to<CharT>("Pineapple"));
  EXPECT_FOUND_STATIC(30, map, to<CharT>("Pen"));
  EXPECT_FOUND_STATIC(40, map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(50, map, to<CharT>("Strawberry"));
  EXPECT_FOUND_STATIC(60, map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(70, map, to<CharT>("Sheep"));
  EXPECT_FOUND_STATIC(80, map, to<CharT>("Owl"));

  EXPECT_EQ_STATIC(CI ? 10 : magic_value, map[to<CharT>("apple")]);
  EXPECT_EQ_STATIC(CI ? 20 : magic_value, map[to<CharT>("PINEAPPLE")]);
  EXPECT_EQ_STATIC(CI ? 30 : magic_value, map[to<CharT>("PeN")]);
  EXPECT_EQ_STATIC(CI ? 40 : magic_value, map[to<CharT>("bAnAnA")]);
  EXPECT_EQ_STATIC(CI ? 50 : magic_value, map[to<CharT>("StrawBerry")]);
  EXPECT_EQ_STATIC(CI ? 60 : magic_value, map[to<CharT>("DOG")]);
  EXPECT_EQ_STATIC(CI ? 70 : magic_value, map[to<CharT>("ShEEp")]);
  EXPECT_EQ_STATIC(CI ? 80 : magic_value, map[to<CharT>("OwL")]);

  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Dig"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("dug"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Watermelon"));
}

#define FOR_EACH_CHARACTER_TYPE(F)  \
  F(char, Char)                     \
  F(wchar_t, WChar)                 \
  F(char8_t, Char8)

#define MAKE_MAP_TESTS(char_type, CharTypeName)                 \
  TEST(UtilsFixedMap, StringKeyByCharacter1##CharTypeName) {    \
    test_by_character_common_1<char_type, false>();             \
  }                                                             \
  TEST(UtilsFixedMap, StringKeyByCharacter1CI##CharTypeName) {  \
    test_by_character_common_1<char_type, true>();              \
  }

FOR_EACH_CHARACTER_TYPE(MAKE_MAP_TESTS)
