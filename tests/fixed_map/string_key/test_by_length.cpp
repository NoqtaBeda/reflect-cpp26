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

template <class CharT, bool CI, bool AA>
void test_by_length_common() {
  using Value = std::pair<double, double>;
  using KVPair = std::pair<std::basic_string<CharT>, Value>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {to<CharT>("Apple"), {12.5, -12.5}},
        {to<CharT>("Pineapple"), {25.0, -25.0}},
        {to<CharT>("Pen"), {37.5, -37.5}},
        {to<CharT>("Banana"), {50.0, -50.0}},
        {to<CharT>("Strawberry"), {62.5, -62.5}},
        {to<CharT>(""), {75.0, -75.0}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(),
                                 {
                                     .ascii_case_insensitive = CI,
                                     .adjusts_alignment = AA,
                                     .min_load_factor = 1.0,
                                     .optimization_threshold = 4,
                                     .binary_search_threshold = 4,
                                 });
  static_assert(rfl::same_as_one_of<typename decltype(map)::result_type,
                                    const rfl::meta_tuple<double, double>&,
                                    const std::pair<double, double>&>);

  // Contents in each element:
  //   length: size_t
  //   key: {const CharT*, const CharT*},
  //   value: {double, double}
  if constexpr (sizeof(void*) == 8) {
    constexpr auto expected_element_size = AA ? 64 : 40;
    constexpr auto actual_element_size =
        sizeof(typename decltype(map._underlying._entries)::value_type);
    EXPECT_EQ(expected_element_size, actual_element_size)
        << "Unexpected element size with fixed map type " << display_string_of(^^decltype(map));
  }

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("string_key_map_by_length"));
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("binary_search_integral_key_map"));
  EXPECT_THAT(display_string_of(^^decltype(map)),
              Not(testing::HasSubstr("general_integral_key_map")));
  EXPECT_THAT(display_string_of(^^decltype(map)),
              Not(testing::HasSubstr("dense_integral_key_map")));
  EXPECT_THAT(display_string_of(^^decltype(map)),
              Not(testing::HasSubstr("linear_search_integral_key_map")));
  EXPECT_EQ_STATIC(6, map.size());

  EXPECT_EQ_STATIC(Value(12.5, -12.5), map[to<CharT>("Apple")]);
  EXPECT_FOUND_STATIC(Value(25.0, -25.0), map, to<CharT>("Pineapple"));
  EXPECT_FOUND_STATIC(Value(37.5, -37.5), map, to<CharT>("Pen"));
  EXPECT_FOUND_STATIC(Value(50.0, -50.0), map, to<CharT>("Banana"));
  EXPECT_FOUND_STATIC(Value(62.5, -62.5), map, to<CharT>("Strawberry"));
  EXPECT_FOUND_STATIC(Value(75.0, -75.0), map, to<CharT>(""));

  constexpr auto default_value = Value{0.0, 0.0};
  EXPECT_EQ_STATIC(CI ? Value(12.5, -12.5) : default_value, map[to<CharT>("apple")]);
  EXPECT_EQ_STATIC(CI ? Value(25.0, -25.0) : default_value, map[to<CharT>("PINEAPPLE")]);
  EXPECT_EQ_STATIC(CI ? Value(37.5, -37.5) : default_value, map[to<CharT>("pEn")]);
  EXPECT_EQ_STATIC(CI ? Value(50.0, -50.0) : default_value, map[to<CharT>("BaNaNa")]);
  EXPECT_EQ_STATIC(CI ? Value(62.5, -62.5) : default_value, map[to<CharT>("StrawBerry")]);

  EXPECT_NOT_FOUND_STATIC(default_value, map, to<CharT>("App"));
  EXPECT_NOT_FOUND_STATIC(default_value, map, to<CharT>("Cat"));
  EXPECT_NOT_FOUND_STATIC(default_value, map, to<CharT>("Berry"));
}

#define FOR_EACH_CHARACTER_TYPE(F) \
  F(char, Char)                    \
  F(wchar_t, WChar)

#define MAKE_BY_LENGTH_MAP_TESTS(char_type, CharTypeName) \
  TEST(FixedMap, StringKeyByLength1##CharTypeName) {      \
    test_by_length_common<char_type, false, false>();     \
  }                                                       \
  TEST(FixedMap, StringKeyByLength2##CharTypeName) {      \
    test_by_length_common<char_type, false, true>();      \
  }                                                       \
  TEST(FixedMap, StringKeyByLength1CI##CharTypeName) {    \
    test_by_length_common<char_type, true, false>();      \
  }                                                       \
  TEST(FixedMap, StringKeyByLength2CI##CharTypeName) {    \
    test_by_length_common<char_type, true, true>();       \
  }

FOR_EACH_CHARACTER_TYPE(MAKE_BY_LENGTH_MAP_TESTS)
