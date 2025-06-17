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
using namespace std::literals;

enum opcode {
  opcode_nop = -1000,
  opcode_invalid = -900,
  opcode_placeholder = -800,
  opcode_mov = 0,
  opcode_add = 1,
  opcode_sub = 2,
  opcode_mul = 3,
  opcode_div = 4,
  opcode_rdiv = 5,
  opcode_mod = 6,
  opcode_bit_and = 10,
  opcode_bit_or = 20,
  opcode_bit_xor = 40,
  opcode_bit_shl = 80,
  opcode_bit_shr = 160,
};

constexpr auto make_kv_pairs() {
  using KVPair = std::pair<opcode, double>;
  return std::vector<KVPair>{
      {opcode_mov, 0.25},
      {opcode_add, 0.5},
      {opcode_sub, 0.75},
      {opcode_mul, 1.25},
      {opcode_div, 1.5},
      {opcode_rdiv, 1.75},
      {opcode_mod, 2.25},
      {opcode_bit_and, 2.5},
      {opcode_bit_or, 2.75},
      {opcode_bit_xor, 3.25},
      {opcode_bit_shl, 3.5},
      {opcode_bit_shr, 3.75},
      {opcode_nop, -10.25},
      {opcode_invalid, -10.5},
      {opcode_placeholder, -10.75},
  };
}

template <rfl::integral_key_fixed_map_options Options>
constexpr void test_unscoped_enum_wrapper_common() {
  constexpr auto map = FIXED_MAP(make_kv_pairs(), Options);
  static_assert(std::is_same_v<typename decltype(map)::result_type, const double&>);
  auto expected_regex = "enum_wrapper.*general_integral_key_map"s + ".*"
                      + "fully_dense_integral_key_map"            // dense_part
                      + ".*" + "linear_search_integral_key_map"   // left_sparse_part
                      + ".*" + "binary_search_integral_key_map";  // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::ContainsRegex(expected_regex));
  EXPECT_EQ_STATIC(15, map.size());

  EXPECT_EQ_STATIC(0.25, map[opcode_mov]);
  EXPECT_FOUND_STATIC(0.5, map, opcode_add);
  EXPECT_FOUND_STATIC(0.75, map, opcode_sub);
  EXPECT_FOUND_STATIC(1.25, map, opcode_mul);
  EXPECT_FOUND_STATIC(1.5, map, opcode_div);
  EXPECT_FOUND_STATIC(1.75, map, opcode_rdiv);
  EXPECT_FOUND_STATIC(2.25, map, opcode_mod);
  EXPECT_FOUND_STATIC(2.5, map, opcode_bit_and);
  EXPECT_FOUND_STATIC(2.75, map, opcode_bit_or);
  EXPECT_FOUND_STATIC(3.25, map, opcode_bit_xor);
  EXPECT_FOUND_STATIC(3.5, map, opcode_bit_shl);
  EXPECT_FOUND_STATIC(3.75, map, opcode_bit_shr);
  EXPECT_FOUND_STATIC(-10.25, map, opcode_nop);
  EXPECT_FOUND_STATIC(-10.5, map, opcode_invalid);
  EXPECT_FOUND_STATIC(-10.75, map, opcode_placeholder);

  EXPECT_EQ_STATIC(0.0, map[static_cast<opcode>(-1)]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, static_cast<opcode>(-1023));
  EXPECT_NOT_FOUND_STATIC(0.0, map, static_cast<opcode>(7));
  EXPECT_NOT_FOUND_STATIC(0.0, map, static_cast<opcode>(120));
  EXPECT_NOT_FOUND_STATIC(0.0, map, static_cast<opcode>(1023));
}

TEST(FixedMap, UnscopedEnumWrapper1) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = false,
      .default_value_is_always_invalid = false,
      .min_load_factor = 0.9,
      .binary_search_threshold = 4,
  };
  test_unscoped_enum_wrapper_common<options>();
}

TEST(FixedMap, UnscopedEnumWrapper2) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = true,
      .default_value_is_always_invalid = false,
      .min_load_factor = 0.9,
      .binary_search_threshold = 4,
  };
  test_unscoped_enum_wrapper_common<options>();
}

TEST(FixedMap, UnscopedEnumWrapper3) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = false,
      .default_value_is_always_invalid = true,
      .min_load_factor = 0.9,
      .binary_search_threshold = 4,
  };
  test_unscoped_enum_wrapper_common<options>();
}

TEST(FixedMap, UnscopedEnumWrapper4) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = true,
      .default_value_is_always_invalid = true,
      .min_load_factor = 0.9,
      .binary_search_threshold = 4,
  };
  test_unscoped_enum_wrapper_common<options>();
}
