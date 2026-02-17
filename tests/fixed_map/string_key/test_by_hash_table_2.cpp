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
void test_by_hash_table_common_2() {
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {to<CharT>("Apple"), {.value = 0}},       // 0x0000'0004'841a'c9fa
        {to<CharT>("Banana"), {.value = 1}},      // 0x0000'0257'8da4'8005
        {to<CharT>("Cat"), {.value = 2}},         // 0x0000'0000'0011'bd72
        {to<CharT>("Dog"), {.value = 3}},         // 0x0000'0000'0012'0798
        {to<CharT>("Horse"), {.value = 4}},       // 0x0000'0004'fed9'023d
        {to<CharT>("Rabbit"), {.value = 5}},      // 0x0000'02e7'444d'd47e
        {to<CharT>("Donkey"), {.value = 6}},      // 0x0000'026a'7a6f'a930
        {to<CharT>("Sheep"), {.value = 7}},       // 0x0000'0005'befc'49e7
        {to<CharT>("Pineapple"), {.value = 8}},   // 0x6143'2afc'63e3'd68a
        {to<CharT>("Watermelon"), {.value = 9}},  // 0x0b85'1508'69f4'1520
        {to<CharT>("Capybara"), {.value = 10}},   // 0x009f'595e'1a65'd529
        {to<CharT>("ABCDEF"), {.value = 11}},     // 0x0000'024c'6c2c'6a9f
        {to<CharT>("ABCDEV"), {.value = 12}},     // 0x0000'024c'6c2c'6aaf
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(),
                                 {
                                     .ascii_case_insensitive = false,
                                     .min_load_factor = 0.25,
                                 });
  static_assert(std::is_same_v<typename decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_EQ_STATIC(13, map.size());
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_table_fast"));
  if constexpr (sizeof(CharT) <= 2) {
    EXPECT_EQ_STATIC(15, map._bucket_size_mask);
  } else {
    EXPECT_EQ_STATIC(31, map._bucket_size_mask);
  }

  EXPECT_EQ_STATIC(0, map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(1, map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(2, map, to<CharT>("Cat"));
  EXPECT_FOUND_STATIC(3, map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(4, map, to<CharT>("Horse"));
  EXPECT_FOUND_STATIC(5, map, to<CharT>("Rabbit"));
  EXPECT_FOUND_STATIC(6, map, to<CharT>("Donkey"));
  EXPECT_FOUND_STATIC(7, map, to<CharT>("Sheep"));
  EXPECT_FOUND_STATIC(8, map, to<CharT>("Pineapple"));
  EXPECT_FOUND_STATIC(9, map, to<CharT>("Watermelon"));
  EXPECT_FOUND_STATIC(10, map, to<CharT>("Capybara"));
  EXPECT_FOUND_STATIC(11, map, to<CharT>("ABCDEF"));
  EXPECT_FOUND_STATIC(12, map, to<CharT>("ABCDEV"));
  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("banana"));
  // 0x5'569e'5aed
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Mouse"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("ABCDEG"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("ABCDEf"));
}

template <class CharT>
void test_by_hash_table_common_ci_2() {
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {to<CharT>("Apple"), {.value = 0}},       // 0x0000'0006'b5d1'941a
        {to<CharT>("BANANA"), {.value = 1}},      // 0x0000'0376'fe2d'ee65
        {to<CharT>("cat"), {.value = 2}},         // 0x0000'0000'001a'1e92
        {to<CharT>("dog"), {.value = 3}},         // 0x0000'0000'001a'68b8
        {to<CharT>("HoRsE"), {.value = 4}},       // 0x0000'0007'308f'cc5d
        {to<CharT>("RaBBiT"), {.value = 5}},      // 0x0000'0406'b4d7'42de
        {to<CharT>("Donkey"), {.value = 6}},      // 0x0000'0389'eaf9'1790
        {to<CharT>("shEEp"), {.value = 7}},       // 0x0000'0007'f0b3'1407
        {to<CharT>("PineApple"), {.value = 8}},   // 0x87c7'401f'860c'8aaa
        {to<CharT>("WaterMelon"), {.value = 9}},  // 0xc11b'e602'e4c8'4180
        {to<CharT>("cApYbArA"), {.value = 10}},   // 0x00ea'9ded'071d'd689
        {to<CharT>("ABCDEF"), {.value = 11}},     // 0x0000'036e'12be'c81f
        {to<CharT>("ABCDEV"), {.value = 12}},     // 0x0000'036e'12be'c82f
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(),
                                 {
                                     .ascii_case_insensitive = true,
                                     .min_load_factor = 0.25,
                                 });
  static_assert(std::is_same_v<typename decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_EQ_STATIC(13, map.size());
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_table_fast"));
  if constexpr (sizeof(CharT) <= 2) {
    EXPECT_EQ_STATIC(15, map._bucket_size_mask);
  } else {
    EXPECT_EQ_STATIC(31, map._bucket_size_mask);
  }

  EXPECT_EQ_STATIC(0, map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(1, map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(1, map, to<CharT>("banana"));
  EXPECT_FOUND_STATIC(2, map, to<CharT>("Cat"));
  EXPECT_FOUND_STATIC(3, map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(4, map, to<CharT>("Horse"));
  EXPECT_FOUND_STATIC(5, map, to<CharT>("Rabbit"));
  EXPECT_FOUND_STATIC(6, map, to<CharT>("Donkey"));
  EXPECT_FOUND_STATIC(7, map, to<CharT>("Sheep"));
  EXPECT_FOUND_STATIC(8, map, to<CharT>("Pineapple"));
  EXPECT_FOUND_STATIC(9, map, to<CharT>("Watermelon"));
  EXPECT_FOUND_STATIC(10, map, to<CharT>("Capybara"));
  EXPECT_FOUND_STATIC(11, map, to<CharT>("ABCDEF"));
  EXPECT_FOUND_STATIC(11, map, to<CharT>("ABCDEf"));
  EXPECT_FOUND_STATIC(12, map, to<CharT>("ABCDEV"));

  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("")]);
  // 0x5'569e'5aed
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Mouse"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("ABCDEG"));
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)           \
  TEST(FixedMap, StringKeyByHashTable2##CharTypeName) {   \
    test_by_hash_table_common_2<char_type>();             \
  }                                                       \
  TEST(FixedMap, StringKeyByHashTable2CI##CharTypeName) { \
    test_by_hash_table_common_ci_2<char_type>();          \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(wchar_t, WChar)
MAKE_MAP_TESTS(char8_t, Char8)
MAKE_MAP_TESTS(char16_t, Char16)
MAKE_MAP_TESTS(char32_t, Char32)
