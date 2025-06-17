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
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  return std::vector<KVPair>{
      {to<CharT>("Apple"), {.value = 0}},
      {to<CharT>("Banana"), {.value = 1}},
      {to<CharT>("Cat"), {.value = 2}},
      {to<CharT>("Dog"), {.value = 3}},
      {to<CharT>("Horse"), {.value = 4}},
      {to<CharT>("Rabbit"), {.value = 5}},
      {to<CharT>("Pineapple"), {.value = 6}},
      {to<CharT>("Sheep"), {.value = 7}},
  };
}

template <class CharT, rfl::string_key_fixed_map_options Options>
constexpr void test_by_hash_search_common(const char* expected_type_name) {
  constexpr auto CI = Options.ascii_case_insensitive;
  constexpr auto map = FIXED_MAP(make_kv_pairs<CharT>(), Options);
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr(expected_type_name));
  static_assert(std::is_same_v<typename decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_EQ_STATIC(8, map.size());

  EXPECT_EQ_STATIC(0, map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(1, map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(2, map, to<CharT>("Cat"));
  EXPECT_FOUND_STATIC(3, map, to<CharT>("Dog"));
  EXPECT_FOUND_STATIC(4, map, to<CharT>("Horse"));
  EXPECT_FOUND_STATIC(5, map, to<CharT>("Rabbit"));
  EXPECT_FOUND_STATIC(6, map, to<CharT>("Pineapple"));
  EXPECT_FOUND_STATIC(7, map, to<CharT>("Sheep"));

  EXPECT_EQ_STATIC(CI ? 0 : magic_value, map[to<CharT>("apple")]);
  EXPECT_EQ_STATIC(CI ? 1 : magic_value, map[to<CharT>("BananA")]);
  EXPECT_EQ_STATIC(CI ? 2 : magic_value, map[to<CharT>("CAT")]);
  EXPECT_EQ_STATIC(CI ? 3 : magic_value, map[to<CharT>("dog")]);
  EXPECT_EQ_STATIC(CI ? 4 : magic_value, map[to<CharT>("HoRse")]);
  EXPECT_EQ_STATIC(CI ? 5 : magic_value, map[to<CharT>("RaBBit")]);
  EXPECT_EQ_STATIC(CI ? 6 : magic_value, map[to<CharT>("PineApple")]);
  EXPECT_EQ_STATIC(CI ? 7 : magic_value, map[to<CharT>("ShEEp")]);

  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("Donkey"));
}

template <class CharT, bool CI>
constexpr void test_by_hash_linear_search_common() {
  constexpr auto options = rfl::string_key_fixed_map_options{
      .ascii_case_insensitive = CI,
      .min_load_factor = 1.0,
      .binary_search_threshold = 9,
  };
  test_by_hash_search_common<CharT, options>("string_key_map_by_hash_linear_search");
}

template <class CharT, bool CI>
constexpr void test_by_hash_binary_search_common() {
  constexpr auto options = rfl::string_key_fixed_map_options{
      .ascii_case_insensitive = CI,
      .min_load_factor = 1.0,
      .binary_search_threshold = 8,
  };
  test_by_hash_search_common<CharT, options>("string_key_map_by_hash_binary_search");
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)                  \
  TEST(FixedMap, StringKeyByHashLinearSearch1##CharTypeName) {   \
    test_by_hash_linear_search_common<char_type, false>();       \
  }                                                              \
  TEST(FixedMap, StringKeyByHashLinearSearch1CI##CharTypeName) { \
    test_by_hash_linear_search_common<char_type, true>();        \
  }                                                              \
  TEST(FixedMap, StringKeyByHashBinarySearch1##CharTypeName) {   \
    test_by_hash_binary_search_common<char_type, false>();       \
  }                                                              \
  TEST(FixedMap, StringKeyByHashBinarySearch1CI##CharTypeName) { \
    test_by_hash_binary_search_common<char_type, true>();        \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(char8_t, Char8)
