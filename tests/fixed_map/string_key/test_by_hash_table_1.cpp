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

template <bool A, class CharT>
void test_by_hash_table_common_1() {
  using Value = std::pair<size_t, size_t>;
  using KVPair = std::pair<std::basic_string<CharT>, Value>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {to<CharT>("Apple"), {0, 1}},
        {to<CharT>("Banana"), {1, 10}},
        {to<CharT>("Cat"), {2, 20}},
        {to<CharT>("Dog"), {3, 30}},
        {to<CharT>("Horse"), {4, 40}},
        {to<CharT>("Rabbit"), {5, 50}},
        {to<CharT>("Squirrow"), {6, 60}},
        {to<CharT>("Sheep"), {7, 70}},
    };
  };
  constexpr auto options = rfl::string_key_fixed_map_options{
      .ascii_case_insensitive = false,
      .adjusts_alignment = A,
      .min_load_factor = 0.5,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  constexpr auto expected_element_size = (sizeof(void*) == 8) ? (A ? 64 : 40) : (A ? 32 : 24);
  constexpr auto actual_element_size = sizeof(map.entries[0]);
  EXPECT_EQ(expected_element_size, actual_element_size)
      << "Unexpected element size with fixed map type " << display_string_of(^^decltype(map));

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("hash_table_with_skey"));
  EXPECT_EQ_STATIC(8, map.size());

  EXPECT_EQ_STATIC(Value(0, 1), map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(Value(1, 10), map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(Value(2, 20), map, to<CharT>("Cat"));
  EXPECT_FOUND_STATIC(Value(3, 30), map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(Value(4, 40), map, to<CharT>("Horse"));
  EXPECT_FOUND_STATIC(Value(5, 50), map, to<CharT>("Rabbit"));
  EXPECT_FOUND_STATIC(Value(6, 60), map, to<CharT>("Squirrow"));
  EXPECT_FOUND_STATIC(Value(7, 70), map, to<CharT>("Sheep"));

  constexpr auto DEFAULT = Value(0, 0);
  EXPECT_EQ_STATIC(DEFAULT, map[to<CharT>("Donkey")]);  // 0x26a'7a6f'a930
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("apple"));
  // 0x6143'2afc'63e3'd68a
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("Pineapple"));
}

template <class CharT>
void test_by_hash_table_common_ci_1() {
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {to<CharT>("Apple"), {.value = 0}},
        {to<CharT>("BANANA"), {.value = 1}},
        {to<CharT>("CAT"), {.value = 2}},
        {to<CharT>("dog"), {.value = 3}},
        {to<CharT>("HORSE"), {.value = 4}},
        {to<CharT>("RaBbIt"), {.value = 5}},
        {to<CharT>("Squirrow"), {.value = 6}},
        {to<CharT>("shEEp"), {.value = 7}},
    };
  };
  constexpr auto options = rfl::string_key_fixed_map_options{
      .ascii_case_insensitive = true,
      .min_load_factor = 0.5,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("hash_table_with_skey"));
  EXPECT_EQ_STATIC(8, map.size());

  EXPECT_EQ_STATIC(0, map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(1, map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(2, map, to<CharT>("Cat"));
  EXPECT_FOUND_STATIC(3, map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(4, map, to<CharT>("Horse"));
  EXPECT_FOUND_STATIC(5, map, to<CharT>("Rabbit"));
  EXPECT_FOUND_STATIC(6, map, to<CharT>("SQuiRRoW"));
  EXPECT_FOUND_STATIC(7, map, to<CharT>("Sheep"));

  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("Donkey")]);  // 0x26a'7a6f'a930
  // 0x6143'2afc'63e3'd68a
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Pineapple"));
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)           \
  TEST(FixedMap, StringKeyByHashTable1##CharTypeName) {   \
    test_by_hash_table_common_1<false, char_type>();      \
  }                                                       \
  TEST(FixedMap, StringKeyByHashTable1A##CharTypeName) {  \
    test_by_hash_table_common_1<true, char_type>();       \
  }                                                       \
  TEST(FixedMap, StringKeyByHashTable1CI##CharTypeName) { \
    test_by_hash_table_common_ci_1<char_type>();          \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(wchar_t, WChar)
MAKE_MAP_TESTS(char8_t, Char8)
MAKE_MAP_TESTS(char16_t, Char16)
MAKE_MAP_TESTS(char32_t, Char32)
