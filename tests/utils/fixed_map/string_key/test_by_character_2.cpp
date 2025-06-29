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
void test_by_character_common_2()
{
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    // Differs at I == 3
    return std::vector<KVPair>{
      {to<CharT>("op_0"), {.value = 0}},
      {to<CharT>("op_1"), {.value = 10}},
      {to<CharT>("op_2"), {.value = 20}},
      {to<CharT>("op_3"), {.value = 30}},
      {to<CharT>("op_4"), {.value = 40}},
      {to<CharT>("op_5"), {.value = 50}},
      {to<CharT>("op_6"), {.value = 60}},
      {to<CharT>("op_7"), {.value = 70}},
      {to<CharT>("op_8"), {.value = 80}},
      {to<CharT>("op_9"), {.value = 90}},
      {to<CharT>("op_+"), {.value = 100}},
      {to<CharT>("op_-"), {.value = 110}},
      {to<CharT>("op_*"), {.value = 120}},
      {to<CharT>("op_/"), {.value = 130}},
      {to<CharT>("op_A"), {.value = 140}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .case_insensitive = CI,
    .enables_lookup_by_differed_character = true,
    .min_load_factor = 1.0,
    .optimization_threshold = 4,
    .dense_part_threshold = 4,
  });
  static_assert(std::is_same_v<
    typename decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_character"));
  EXPECT_EQ_STATIC(3, map.differed_index);
  EXPECT_EQ_STATIC(15, map.size());

  for (auto i = 0; i <= 9; i++) {
    auto key = to<CharT>("op_") + static_cast<CharT>('0' + i);
    EXPECT_EQ(i * 10, map[key]) << "Fails with key 'op_" << i << "'.";
  }
  EXPECT_EQ_STATIC(100, map[to<CharT>("op_+")]);
  EXPECT_FOUND_STATIC(110, map, to<CharT>("op_-"));
  EXPECT_FOUND_STATIC(120, map, to<CharT>("op_*"));
  EXPECT_FOUND_STATIC(130, map, to<CharT>("op_/"));
  EXPECT_FOUND_STATIC(140, map, to<CharT>("op_A"));

  for (auto i = 0; i <= 9; i++) {
    auto key = to<CharT>("Op_") + static_cast<CharT>('0' + i);
    EXPECT_EQ(CI ? i * 10 : magic_value, map[key])
      << "Fails with key 'Op_" << i << "'.";
  }
  EXPECT_EQ_STATIC(CI ? 100 : magic_value, map[to<CharT>("OP_+")]);
  EXPECT_EQ_STATIC(CI ? 110 : magic_value, map[to<CharT>("oP_-")]);
  EXPECT_EQ_STATIC(CI ? 120 : magic_value, map[to<CharT>("Op_*")]);
  EXPECT_EQ_STATIC(CI ? 140 : magic_value, map[to<CharT>("op_a")]);
  EXPECT_EQ_STATIC(CI ? 140 : magic_value, map[to<CharT>("OP_a")]);

  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("oq_+"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("op_%"));

  if constexpr (std::is_same_v<CharT, char16_t>) {
    EXPECT_NOT_FOUND_STATIC(magic_value, map, u"op_加");
    EXPECT_NOT_FOUND_STATIC(magic_value, map, u"中文_A");
  }
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)                 \
  TEST(UtilsFixedMap, StringKeyByCharacter2##CharTypeName) {    \
    test_by_character_common_2<char_type, false>();             \
  }                                                             \
  TEST(UtilsFixedMap, StringKeyByCharacter2CI##CharTypeName) {  \
    test_by_character_common_2<char_type, true>();              \
  }

#define FOR_EACH_CHARACTER_TYPE(F)  \
  F(char, Char)                     \
  F(wchar_t, WChar)                 \
  F(char16_t, Char16)

FOR_EACH_CHARACTER_TYPE(MAKE_MAP_TESTS)
