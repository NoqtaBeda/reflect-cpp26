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
consteval auto make_kv_pairs() {
  using Value = std::pair<size_t, size_t>;
  using KVPair = std::pair<std::basic_string<CharT>, Value>;
  return std::vector<KVPair>{
      {to<CharT>("Amiya"), {0, 1}},
      {to<CharT>("Kal'tsit"), {1, 10}},
      {to<CharT>("Blaze"), {2, 20}},
      {to<CharT>("Rosmontis"), {3, 30}},
      {to<CharT>("Logos"), {4, 40}},
      {to<CharT>("Ch'en"), {5, 50}},
      {to<CharT>("Crownslayer"), {6, 60}},
      {to<CharT>("Sharp"), {7, 70}},
  };
}

template <class CharT, rfl::string_key_fixed_map_options Options>
constexpr void test_by_hash_search_common() {
  using Value = std::pair<size_t, size_t>;
  constexpr auto CI = Options.ascii_case_insensitive;
  constexpr auto AA = Options.adjusts_alignment;
  constexpr auto map = FIXED_MAP(make_kv_pairs<CharT>(), Options);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_binary_search"));
  static_assert(rfl::same_as_one_of<typename decltype(map)::result_type,
                                    const rfl::meta_tuple<size_t, size_t>&,
                                    const std::pair<size_t, size_t>&>);
  EXPECT_EQ_STATIC(8, map.size());

  constexpr auto pointer_size = sizeof(void*);
  // Contents in each entry:
  //   hash: uint64
  //   key: {const CharT*, const CharT*}
  //   value: {size_t, size_t}
  constexpr auto expected_element_size =
      Options.adjusts_alignment ? (pointer_size == 8 ? 64 : 32) : (pointer_size == 8 ? 40 : 24);
  constexpr auto actual_element_size = sizeof(typename decltype(map._entries)::value_type);
  EXPECT_EQ(expected_element_size, actual_element_size)
      << "Unexpected element size with fixed map type " << display_string_of(^^decltype(map));

  EXPECT_EQ_STATIC(Value(0, 1), map[to<CharT>("Amiya")]);
  EXPECT_FOUND_STATIC(Value(1, 10), map, to<CharT>("Kal'tsit"));
  EXPECT_FOUND_STATIC(Value(2, 20), map, to<CharT>("Blaze"));
  EXPECT_FOUND_STATIC(Value(3, 30), map, to<CharT>("Rosmontis"));
  EXPECT_FOUND_STATIC(Value(4, 40), map, to<CharT>("Logos"));
  EXPECT_FOUND_STATIC(Value(5, 50), map, to<CharT>("Ch'en"));
  EXPECT_FOUND_STATIC(Value(6, 60), map, to<CharT>("Crownslayer"));
  EXPECT_FOUND_STATIC(Value(7, 70), map, to<CharT>("Sharp"));

  constexpr auto DEFAULT = Value(0, 0);
  EXPECT_EQ_STATIC(CI ? Value(0, 1) : DEFAULT, map[to<CharT>("amiya")]);
  EXPECT_EQ_STATIC(CI ? Value(1, 10) : DEFAULT, map[to<CharT>("KAL'TSIT")]);
  EXPECT_EQ_STATIC(CI ? Value(2, 20) : DEFAULT, map[to<CharT>("BLaZe")]);
  EXPECT_EQ_STATIC(CI ? Value(3, 30) : DEFAULT, map[to<CharT>("ROSmonTIS")]);
  EXPECT_EQ_STATIC(CI ? Value(4, 40) : DEFAULT, map[to<CharT>("LOGOS")]);
  EXPECT_EQ_STATIC(CI ? Value(5, 50) : DEFAULT, map[to<CharT>("CH'en")]);
  EXPECT_EQ_STATIC(CI ? Value(6, 60) : DEFAULT, map[to<CharT>("crownSLAYer")]);
  EXPECT_EQ_STATIC(CI ? Value(7, 70) : DEFAULT, map[to<CharT>("SHARP")]);

  EXPECT_EQ_STATIC(DEFAULT, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("Emiya"));
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("Chen"));
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("Lógos"));
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("λόγος"));
}

template <class CharT, bool AA>
constexpr void test_by_hash_binary_search_common() {
  constexpr auto options = rfl::string_key_fixed_map_options{
      // Cases where CI == false has been tested in test_by_hash_search_1.cpp
      .ascii_case_insensitive = true,
      .adjusts_alignment = AA,
      .min_load_factor = 1.0,
      .binary_search_threshold = 8,
  };
  test_by_hash_search_common<CharT, options>();
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)                  \
  TEST(FixedMap, StringKeyByHashBinarySearch2##CharTypeName) {   \
    test_by_hash_binary_search_common<char_type, false>();       \
  }                                                              \
  TEST(FixedMap, StringKeyByHashBinarySearch2AA##CharTypeName) { \
    test_by_hash_binary_search_common<char_type, true>();        \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(char8_t, Char8)
MAKE_MAP_TESTS(char16_t, Char16)
MAKE_MAP_TESTS(char32_t, Char32)
