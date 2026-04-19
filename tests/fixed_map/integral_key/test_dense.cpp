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

template <bool A>
constexpr void test_signed_integral_key_common() {
  using InputKVPair = std::pair<signed char, const char*>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<InputKVPair>{
        {'a', "Apple"},
        {'b', "Banana"},
        {'c', "Cat"},
        {'d', "Dog"},
        {'h', "Hedgedog"},
        {'i', "Ice"},
        {'j', "John"},
    };
  };
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .already_sorted = true,
      .already_unique = true,
      .adjusts_alignment = A,
      .min_load_factor = 0.5,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("non_null_dense_with_ikey"));
  EXPECT_EQ_STATIC(7, map.size());
  EXPECT_EQ_STATIC(2 * sizeof(void*), sizeof(map.entries[0]));

  EXPECT_EQ_STATIC("Apple", map['a']);
  EXPECT_FOUND_STATIC("Banana", map, int8_t{'b'});
  EXPECT_FOUND_STATIC("Cat", map, uint8_t{'c'});
  EXPECT_FOUND_STATIC("Dog", map, int16_t{'d'});
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(nullptr, map['e']);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 'f');

  EXPECT_EQ_STATIC("Hedgedog", map[uint16_t{'h'}]);
  EXPECT_FOUND_STATIC("Ice", map, int32_t{'i'});
  EXPECT_FOUND_STATIC("John", map, uint32_t{'j'});
  // Safe integral comparison is used
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 0x100 + 'a');
}

TEST(FixedMap, SignedIntegralKeyDense1) {
  test_signed_integral_key_common<false>();
}

TEST(FixedMap, SignedIntegralKeyDense2) {
  test_signed_integral_key_common<true>();
}

// Test for dense fixed map from signed integer to signed integer
// where values can be 0. This case tests whether dense_integral_key_map
// can distinguish between valid values which happen to be equal to 0
// and holes value-initialized to 0.
template <bool A>
constexpr void test_signed_integral_key_with_zero_value_common() {
  using InputKVPair = std::pair<int8_t, int8_t>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<InputKVPair>{
        {-3, -10},
        {-2, 0},  // Valid value is 0
        {-1, 10},
        {0, -20},  // Key is 0, value is non-zero
        {1, 0},    // Valid value is 0
        {3, 30},
    };
  };
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .already_sorted = true,
      .already_unique = true,
      .adjusts_alignment = A,
      .min_load_factor = 0.5,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("dense_with_ikey"));
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::Not(testing::HasSubstr("non_null")));
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::Not(testing::HasSubstr("fully")));
  // {int8_t, bool} where bool indicates whether this entry represents a valid value
  EXPECT_EQ_STATIC(2, sizeof(map.entries[0]));
  EXPECT_EQ_STATIC(6, map.size());

  // Valid entries with non-zero values
  EXPECT_EQ_STATIC(-10, map[-3]);
  EXPECT_FOUND_STATIC(10, map, int8_t{-1});
  EXPECT_FOUND_STATIC(-20, map, int8_t{0});
  EXPECT_FOUND_STATIC(30, map, int8_t{3});

  // Valid entries with value 0 - must be distinguishable from holes
  EXPECT_FOUND_STATIC(0, map, -2);
  EXPECT_FOUND_STATIC(0, map, 1);

  // Hole at key 2. The value-initialized is returned as placeholder
  EXPECT_NOT_FOUND_STATIC(0, map, int8_t{2});

  // Out of range
  EXPECT_NOT_FOUND_STATIC(0, map, int8_t{-4});
  EXPECT_NOT_FOUND_STATIC(0, map, int8_t{4});
  // Signedness-safe and narrowing-safe comparison
  // Key range: [-3, 3], querying with larger/narrower types
  EXPECT_NOT_FOUND_STATIC(0, map, int16_t{-4});        // Signed, out of range
  EXPECT_NOT_FOUND_STATIC(0, map, int16_t{4});         // Signed, out of range
  EXPECT_NOT_FOUND_STATIC(0, map, uint8_t{253});       // Signedness-safe comparison
  EXPECT_NOT_FOUND_STATIC(0, map, uint8_t{252});       // Signedness-safe comparison
  EXPECT_NOT_FOUND_STATIC(0, map, uint16_t{256 + 3});  // Narrowing-safe comparison
}

TEST(FixedMap, SignedIntegralKeyDenseWithZeroValue1) {
  test_signed_integral_key_with_zero_value_common<false>();
}

TEST(FixedMap, SignedIntegralKeyDenseWithZeroValue2) {
  test_signed_integral_key_with_zero_value_common<true>();
}

template <bool A>
void test_unsigned_integral_key_common() {
  constexpr auto make_kv_pairs = []() consteval {
    using InputKVPair = std::pair<unsigned char, std::string>;
    return std::vector<InputKVPair>{
        {126, std::string("Apple")},
        {128, std::string("Banana")},
        {130, std::string("Cat")},
        {132, std::string("Dog")},
        {134, std::string("Hedgedog")},
        {136, std::string("Ice")},
    };
  };
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = A,
      .min_load_factor = 0.5,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  EXPECT_THAT(display_string_of(^^decltype(map)), testing::HasSubstr("non_null_dense_with_ikey"));
  // Size of meta_string_view = 2 * pointer size
  EXPECT_EQ_STATIC(2 * sizeof(void*), sizeof(map.entries[0]));
  EXPECT_EQ_STATIC(6, map.size());

  EXPECT_EQ_STATIC("Apple", map[int8_t{126}]);
  EXPECT_FOUND_STATIC("Banana", map, uint8_t{128});
  EXPECT_FOUND_STATIC("Cat", map, int16_t{130});
  EXPECT_FOUND_STATIC("Dog", map, uint16_t{132});
  EXPECT_FOUND_STATIC("Hedgedog", map, int32_t{134});
  EXPECT_FOUND_STATIC("Ice", map, uint32_t{136});
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(nullptr, map[int8_t{127}]);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, uint8_t{129});
  EXPECT_NOT_FOUND_STATIC(nullptr, map, int16_t{131});
  EXPECT_NOT_FOUND_STATIC(nullptr, map, uint16_t{133});
  EXPECT_NOT_FOUND_STATIC(nullptr, map, int32_t{135});
  EXPECT_NOT_FOUND_STATIC(nullptr, map, uint32_t{137});
  // Safe integral comparison is used
  EXPECT_EQ_STATIC(nullptr, map[int8_t{-128}]);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, int16_t{256 + 128});
}

TEST(FixedMap, UnsignedIntegralKeyDense1) {
  test_unsigned_integral_key_common<false>();
}

TEST(FixedMap, UnsignedIntegralKeyDense2) {
  test_unsigned_integral_key_common<true>();
}
