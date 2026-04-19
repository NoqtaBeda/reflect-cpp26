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

#include <reflect_cpp26/fixed_map/integral_key.hpp>

#include "tests/fixed_map/integral_key/integral_key_test_options.hpp"

namespace rfl = reflect_cpp26;

TEST(FixedMap, SignedIntegralKeySparseLinearSearch) {
  using KVPair = std::pair<int64_t, double>;

  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {-100, 1.25},
        {200, 3.5},
        {-300, 5.75},
        {400, 7.0},
        {-500, 8.25},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {.binary_search_threshold = 100});

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("linear_search_with_ikey"));
  EXPECT_EQ_STATIC(5, map.size());

  EXPECT_EQ_STATIC(1.25, map[-100]);
  EXPECT_FOUND_STATIC(5.75, map, -300);
  EXPECT_FOUND_STATIC(8.25, map, -500);
  EXPECT_FOUND_STATIC(3.5, map, 200);
  EXPECT_FOUND_STATIC(7.0, map, 400);
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(0.0, map[0]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, 10);
  EXPECT_NOT_FOUND_STATIC(0.0, map, 1000);
  // Safe integral comparison is used
  EXPECT_NOT_FOUND_STATIC(0.0, map, static_cast<unsigned>(-100));
}

template <bool A>
void test_binary_search_common() {
  using InputValueType = std::pair<uint64_t, int64_t>;
  using KVPair = std::pair<uint64_t, InputValueType>;

  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {static_cast<uint64_t>(-200), {12, -12}},
        {static_cast<uint64_t>(-100), {24, -24}},
        {100, {36, -36}},
        {300, {48, -48}},
        {500, {60, -60}},
    };
  };
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = A,
      .binary_search_threshold = 1,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("binary_search_with_ikey"));
  EXPECT_EQ_STATIC(5, map.size());

  constexpr auto expected_element_size = options.adjusts_alignment ? 32 : 24;
  constexpr auto actual_element_size = sizeof(map.entries[0]);
  EXPECT_EQ(expected_element_size, actual_element_size)
      << "Unexpected element size with fixed map type " << display_string_of(^^decltype(map));

  EXPECT_EQ_STATIC(InputValueType(36, -36), map[100]);
  EXPECT_FOUND_STATIC(InputValueType(48, -48), map, 300);
  EXPECT_FOUND_STATIC(InputValueType(60, -60), map, 500);
  EXPECT_FOUND_STATIC(InputValueType(24, -24), map, static_cast<uint64_t>(-100));
  EXPECT_FOUND_STATIC(InputValueType(12, -12), map, static_cast<uint64_t>(-200));
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(InputValueType(0, 0), map[0]);
  // Safe integral comparison is used
  EXPECT_EQ_STATIC(InputValueType(0, 0), map[-100]);
  EXPECT_NOT_FOUND_STATIC(InputValueType(0, 0), map, -200);
}

TEST(FixedMap, UnsignedIntegralKeySparseBinarySearch1) {
  test_binary_search_common<false>();
}

TEST(FixedMap, UnsignedIntegralKeySparseBinarySearch2) {
  test_binary_search_common<true>();
}
