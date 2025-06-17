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

// Expected hash table layout:
// [0] ""       (0)
// [1] "Dog"    (8)
// [2] "Apple"  (a)
// [3] "Cat"    (2)
// [4]
// [5] Banana   (5)
// [6] Horse    (d)
// [7] Rabbit   (e)
// [8] Sheep    (7)
template <bool AA, class CharT>
void test_by_hash_table_common_1() {
  using Value = std::pair<size_t, size_t>;
  using KVPair = std::pair<std::basic_string<CharT>, Value>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {to<CharT>("Apple"), {0, 1}},    //   0x4'841a'c9fa
        {to<CharT>("Banana"), {1, 10}},  // 0x257'8da4'8005
        {to<CharT>("Cat"), {2, 20}},     //       0x11'bd72
        {to<CharT>("Dog"), {3, 30}},     //       0x12'0798
        {to<CharT>("Horse"), {4, 40}},   //   0x4'fed9'023d
        {to<CharT>("Rabbit"), {5, 50}},  // 0x2e7'444d'd47e
        {to<CharT>(""), {6, 60}},        //             0x0
        {to<CharT>("Sheep"), {7, 70}},   //   0x5'befc'49e7
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(),
                                 {
                                     .ascii_case_insensitive = false,
                                     .adjusts_alignment = AA,
                                     .min_load_factor = 0.5,
                                 });
  static_assert(rfl::same_as_one_of<typename decltype(map)::result_type,
                                    const rfl::meta_tuple<size_t, size_t>&,
                                    const std::pair<size_t, size_t>&>);

  constexpr auto expected_element_size = (sizeof(void*) == 8) ? (AA ? 64 : 40) : (AA ? 32 : 24);
  constexpr auto actual_element_size = sizeof(typename decltype(map._entries)::value_type);
  EXPECT_EQ(expected_element_size, actual_element_size)
      << "Unexpected element size with fixed map type " << display_string_of(^^decltype(map));

  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_table_fast"));
  EXPECT_EQ_STATIC(8, map.size());
  EXPECT_EQ_STATIC(7, map._bucket_size_mask);

  EXPECT_EQ_STATIC(Value(0, 1), map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(Value(1, 10), map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(Value(2, 20), map, to<CharT>("Cat"));
  EXPECT_FOUND_STATIC(Value(3, 30), map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(Value(4, 40), map, to<CharT>("Horse"));
  EXPECT_FOUND_STATIC(Value(5, 50), map, to<CharT>("Rabbit"));
  EXPECT_FOUND_STATIC(Value(6, 60), map, to<CharT>(""));
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
        {to<CharT>("Apple"), {.value = 0}},   //   0x6'b5d1'941a
        {to<CharT>("BANANA"), {.value = 1}},  // 0x376'fe2d'ee65
        {to<CharT>("CAT"), {.value = 2}},     //       0x1a'1e92
        {to<CharT>("dog"), {.value = 3}},     //       0x1a'68b8
        {to<CharT>("HORSE"), {.value = 4}},   //   0x7'308f'cc5d
        {to<CharT>("RaBbIt"), {.value = 5}},  // 0x406'b4d7'42de
        {to<CharT>(""), {.value = 6}},        //             0x0
        {to<CharT>("shEEp"), {.value = 7}},   //   0x7'f0b3'1407
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(),
                                 {
                                     .ascii_case_insensitive = true,
                                     .min_load_factor = 0.5,
                                 });
  static_assert(std::is_same_v<typename decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_table_fast"));
  EXPECT_EQ_STATIC(8, map.size());
  EXPECT_EQ_STATIC(7, map._bucket_size_mask);

  EXPECT_EQ_STATIC(0, map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(1, map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(2, map, to<CharT>("Cat"));
  EXPECT_FOUND_STATIC(3, map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(4, map, to<CharT>("Horse"));
  EXPECT_FOUND_STATIC(5, map, to<CharT>("Rabbit"));
  EXPECT_FOUND_STATIC(6, map, to<CharT>(""));
  EXPECT_FOUND_STATIC(7, map, to<CharT>("Sheep"));

  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("Donkey")]);  // 0x26a'7a6f'a930
  // 0x6143'2afc'63e3'd68a
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Pineapple"));
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)           \
  TEST(FixedMap, StringKeyByHashTable1##CharTypeName) {   \
    test_by_hash_table_common_1<false, char_type>();      \
  }                                                       \
  TEST(FixedMap, StringKeyByHashTable1AA##CharTypeName) { \
    test_by_hash_table_common_1<true, char_type>();       \
  }                                                       \
  TEST(FixedMap, StringKeyByHashTable1CI##CharTypeName) { \
    test_by_hash_table_common_ci_1<char_type>();          \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(char8_t, Char8)
