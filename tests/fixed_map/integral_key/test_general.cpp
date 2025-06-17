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

#include <climits>
#include <reflect_cpp26/fixed_map/integral_key.hpp>

#include "tests/fixed_map/integral_key/integral_key_test_options.hpp"

namespace rfl = reflect_cpp26;
using namespace std::literals;

template <size_t I>
size_t fn() {
  return I;
}

TEST(FixedMap, SignedIntegralKeyGeneral) {
  using KVPair = std::pair<int, size_t (*)()>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {INT_MIN, fn<0>},
        {-3, fn<1>},
        {-1, fn<2>},
        {0, fn<3>},
        {1, fn<4>},
        {2, fn<5>},
        {3, fn<6>},
        {4, fn<7>},
        {8, fn<8>},
        {16, fn<9>},
        {32, fn<10>},
        {INT_MAX, fn<11>},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(),
                                 {
                                     .min_load_factor = 1.0,
                                     .dense_lookup_threshold = 6,
                                     .binary_search_threshold = 4,
                                 });
  static_assert(std::is_same_v<decltype(map)::result_type, size_t (*const&)()>);
  auto expected_regex = "general_integral_key_map"s + ".*"
                      + "linear_search_integral_key_map"          // left_sparse_part
                      + ".*" + "binary_search_integral_key_map";  // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::ContainsRegex(expected_regex));
  EXPECT_EQ_STATIC(12, map.size());
  EXPECT_EQ_STATIC(6, map._dense_part.size());
  EXPECT_EQ_STATIC(2, map._left_sparse_part.size());
  EXPECT_EQ_STATIC(4, map._right_sparse_part.size());
  EXPECT_EQ_STATIC(INT_MIN, map.min_key());
  EXPECT_EQ_STATIC(INT_MAX, map.max_key());

  auto check_non_null = [&map](size_t expected_value, int key) {
    EXPECT_NE(nullptr, map[key]) << "failed with key = " << key;
    EXPECT_EQ(expected_value, map[key]()) << "failed with key = " << key;
  };
  check_non_null(0, INT_MIN);
  check_non_null(1, -3);
  check_non_null(2, -1);
  check_non_null(3, 0);
  check_non_null(4, 1);
  check_non_null(5, 2);
  check_non_null(6, 3);
  check_non_null(7, 4);
  check_non_null(8, 8);
  check_non_null(9, 16);
  check_non_null(10, 32);
  check_non_null(11, INT_MAX);
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(nullptr, map[-7]);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, -5);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 9);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 65);
  // Safe integral comparison is used
  EXPECT_EQ_STATIC(nullptr, map[static_cast<unsigned>(-1)]);
}

constexpr auto make_kv_pairs_for_unsigned_integral_key_general() {
  using KVPair = std::pair<unsigned, size_t (*)()>;
  return std::vector<KVPair>{
      {UINT_MAX, fn<0>},
      {0, fn<1>},
      {2, fn<2>},
      {4, fn<3>},
      {6, fn<4>},
      {9, fn<5>},
      {12, fn<6>},
      {15, fn<7>},
      {18, fn<8>},
  };
}

template <class FixedMap>
constexpr auto test_unsigned_integral_key_general_common(const FixedMap& map) {
  static_assert(std::is_same_v<typename FixedMap::result_type, size_t (*const&)()>);
  EXPECT_EQ(9, map.size());
  EXPECT_EQ(5, map._dense_part.size());
  EXPECT_EQ(0, map._left_sparse_part.size());
  EXPECT_EQ(4, map._right_sparse_part.size());
  EXPECT_EQ(0, map.min_key());
  EXPECT_EQ(UINT_MAX, map.max_key());

  auto check_non_null = [&map](size_t expected_value, unsigned key) {
    EXPECT_NE(nullptr, map[key]) << "failed with key = " << key;
    EXPECT_EQ(expected_value, map[key]()) << "failed with key = " << key;
  };
  check_non_null(1, 0);
  check_non_null(2, 2);
  check_non_null(3, 4);
  check_non_null(4, 6);
  check_non_null(5, 9);
  check_non_null(6, 12);
  check_non_null(7, 15);
  check_non_null(8, 18);
  check_non_null(0, UINT_MAX);
  // Holes: Value-initialized
  EXPECT_EQ(nullptr, map[1]);
  EXPECT_NOT_FOUND(nullptr, map, 10);
  EXPECT_NOT_FOUND(nullptr, map, 16);
  EXPECT_NOT_FOUND(nullptr, map, UINT_MAX - 1);
  // Safe integral comparison is used
  EXPECT_EQ(nullptr, map[-1]);
}

TEST(FixedMap, UnsignedIntegralKeyGeneral1) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .min_load_factor = 0.5,
      .dense_lookup_threshold = 5,
      .binary_search_threshold = 4,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs_for_unsigned_integral_key_general(), options);
  // true: Uses value wrapper with flag 'is_valid'
  auto expected_regex = "general_integral_key_map"s + ".*" + "dense_integral_key_map"  // dense_part
                      + ".*" + "empty_integral_key_map"           // left_sparse_part
                      + ".*" + "binary_search_integral_key_map";  // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::ContainsRegex(expected_regex));
  test_unsigned_integral_key_general_common(map);
}

TEST(FixedMap, UnsignedIntegralKeyGeneral2) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .default_value_is_always_invalid = true,
      .min_load_factor = 0.5,
      .dense_lookup_threshold = 5,
      .binary_search_threshold = 4,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs_for_unsigned_integral_key_general(), options);
  // true: Noes not value wrapper with flag 'is_valid' as it's not needed.
  auto expected_regex = "general_integral_key_map"s + ".*" + "dense_integral_key_map"  // dense_part
                      + ".*" + "empty_integral_key_map"           // left_sparse_part
                      + ".*" + "binary_search_integral_key_map";  // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::ContainsRegex(expected_regex));
  test_unsigned_integral_key_general_common(map);
}
