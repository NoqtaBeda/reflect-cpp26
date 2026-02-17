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
using namespace std::string_literals;

template <bool AA, class CharT>
void test_by_hash_table_common_3() {
  using Value = std::pair<size_t, size_t>;
  using KVPair = std::pair<std::basic_string<CharT>, Value>;
  constexpr auto make_kv_pairs = []() consteval {
    auto res = std::vector<KVPair>{
        {to<CharT>("BA"), {1, 10}},  // 0x2207
        {to<CharT>("Ba"), {2, 20}},  // 0x2227
    };
    for (auto i = 'A'; i <= 'Z'; i++) {
      auto key = to<CharT>("A") + static_cast<CharT>(i);  // 0x2184 ~ 0x219d
      auto value = Value{i, i + 1};
      res.emplace_back(key, value);
    }
    return res;
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(),
                                 {
                                     .ascii_case_insensitive = false,
                                     .adjusts_alignment = AA,
                                     .min_load_factor = 0.25,
                                 });
  static_assert(rfl::same_as_one_of<typename decltype(map)::result_type,
                                    const rfl::meta_tuple<size_t, size_t>&,
                                    const std::pair<size_t, size_t>&>);

  constexpr auto expected_element_size = (sizeof(void*) == 8) ? (AA ? 64 : 40) : (AA ? 32 : 24);
  constexpr auto actual_element_size = sizeof(typename decltype(map._entries)::value_type);
  EXPECT_EQ(expected_element_size, actual_element_size)
      << "Unexpected element size with fixed map type " << display_string_of(^^decltype(map));

  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_table_slow"));
  EXPECT_EQ_STATIC(28, map.size());
  if constexpr (sizeof(CharT) == 1) {
    EXPECT_EQ_STATIC(33, map._bucket_size);  // Modulo: 0 ~ 25, 31, 32
  }

  EXPECT_FOUND_STATIC(Value('G', 'H'), map, to<CharT>("AG"));
  EXPECT_FOUND_STATIC(Value('T', 'U'), map, to<CharT>("AT"));
  for (auto i = 'B'; i <= 'Z'; i++) {
    auto key = to<CharT>("A") + static_cast<CharT>(i);
    auto char_key = "a"s + static_cast<char>(i);
    EXPECT_EQ(Value(i, i + 1), map[key]) << "Failed with key '" << char_key << "'";
  }
  constexpr auto DEFAULT = Value{0, 0};
  EXPECT_EQ_STATIC(DEFAULT, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("B@"));
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("A{"));
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("ba"));
}

template <class CharT>
void test_by_hash_table_common_ci_3() {
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() consteval {
    auto res = std::vector<KVPair>{
        {to<CharT>("BA"), {.value = 1}},  // 0x3287
        {to<CharT>("bb"), {.value = 2}},  // 0x3288
    };
    for (auto i = 'A'; i <= 'Z'; i++) {
      auto key = to<CharT>("A") + static_cast<CharT>(i);  // 0x3204 ~ 0x321d
      auto value = wrapper_t<int>{.value = i};
      res.emplace_back(key, value);
    }
    return res;
  };
  constexpr auto map =
      FIXED_MAP(make_kv_pairs(), {.ascii_case_insensitive = true, .min_load_factor = 0.5});
  static_assert(std::is_same_v<typename decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_table_slow"));
  EXPECT_EQ_STATIC(28, map.size());

  EXPECT_FOUND_STATIC('G', map, to<CharT>("ag"));
  EXPECT_FOUND_STATIC('T', map, to<CharT>("At"));
  for (auto i = 'B'; i <= 'Z'; i++) {
    auto key = to<CharT>("a") + static_cast<CharT>(i);
    auto char_key = "a"s + static_cast<char>(i);
    EXPECT_EQ(i, map[key]) << "Failed with key '" << char_key << "'";
  }
  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("B@"));
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("A{"));
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)           \
  TEST(FixedMap, StringKeyByHashTable3##CharTypeName) {   \
    test_by_hash_table_common_3<false, char_type>();      \
  }                                                       \
  TEST(FixedMap, StringKeyByHashTable3AA##CharTypeName) { \
    test_by_hash_table_common_3<true, char_type>();       \
  }                                                       \
  TEST(FixedMap, StringKeyByHashTable3CI##CharTypeName) { \
    test_by_hash_table_common_ci_3<char_type>();          \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(wchar_t, WChar)
MAKE_MAP_TESTS(char8_t, Char8)
MAKE_MAP_TESTS(char16_t, Char16)
MAKE_MAP_TESTS(char32_t, Char32)
