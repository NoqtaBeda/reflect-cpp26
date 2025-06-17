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

TEST(FixedMap, SignedIntegralKeyFullyContinuous) {
  using KVPair = std::pair<int32_t, int32_t>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{{-1, 123}, {1, 456}, {3, 789}, {-2, 12}, {0, 34}, {2, 56}};
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs() /* with default options */);
  static_assert(std::is_same_v<typename decltype(map)::result_type, const int32_t&>);
  // Elements in _entries are expected to be int32_t (value type of KVPair),
  // no is_valid flag.
  EXPECT_EQ_STATIC(4, sizeof(typename decltype(map._entries)::value_type));
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("fully_dense_integral_key_map"));
  EXPECT_EQ_STATIC(6, map.size());
  EXPECT_EQ_STATIC(-2, map.min_key());
  EXPECT_EQ_STATIC(3, map.max_key());

  EXPECT_EQ_STATIC(12, map[-2]);
  EXPECT_FOUND_STATIC(123, map, -1);
  EXPECT_FOUND_STATIC(34, map, 0);
  EXPECT_FOUND_STATIC(456, map, 1u);
  EXPECT_FOUND_STATIC(56, map, 2);
  EXPECT_FOUND_STATIC(789, map, 3u);
  // Value-initialized
  EXPECT_EQ_STATIC(0, map[-3]);
  EXPECT_NOT_FOUND_STATIC(0, map, std::numeric_limits<int32_t>::max());
  // Safe integral comparison is used
  EXPECT_EQ_STATIC(0, map[static_cast<unsigned>(-1)]);
  EXPECT_NOT_FOUND_STATIC(0, map, 0x1'0000'0000LL);
}

struct point_t {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  constexpr point_t() = default;
  constexpr point_t(double x, double y, double z) : x(x), y(y), z(z) {}

  constexpr auto operator<=>(const point_t&) const = default;
  constexpr bool operator==(const point_t&) const = default;
};

template <rfl::integral_key_fixed_map_options Options>
constexpr void test_unsigned_integral_key_common() {
  using Value = point_t;
  using KVPair = std::pair<uint64_t, Value>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
        {10, {1.25, -1.25, 2.0}},
        {12, {2.5, -2.5, 3.0}},
        {11, {3.75, -3.75, 4.0}},
        {14, {5.0, -5.0, 5.0}},
        {13, {6.25, -6.25, 6.0}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), Options);
  static_assert(std::is_same_v<typename decltype(map)::result_type, const Value&>);

  using UnderlyingElement = typename decltype(map._entries)::value_type;
  if constexpr (Options.adjusts_alignment) {
    EXPECT_EQ(32, sizeof(UnderlyingElement))
        << "Expects sizeof(" << display_string_of(dealias(^^UnderlyingElement))
        << ") to be 32 bytes after adjusting alignment, with map type = "
        << display_string_of(^^decltype(map));
  } else {
    EXPECT_EQ(24, sizeof(UnderlyingElement))
        << "Expects sizeof(" << display_string_of(dealias(^^UnderlyingElement))
        << ") to be 24 bytes without adjusting alignment, with map type = "
        << display_string_of(^^decltype(map));
  }
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("fully_dense_integral_key_map"));
  EXPECT_EQ_STATIC(5, map.size());
  EXPECT_EQ_STATIC(10, map.min_key());
  EXPECT_EQ_STATIC(14, map.max_key());

  EXPECT_EQ_STATIC(Value(1.25, -1.25, 2.0), map[10]);
  EXPECT_FOUND_STATIC(Value(3.75, -3.75, 4.0), map, 11);
  EXPECT_FOUND_STATIC(Value(2.5, -2.5, 3.0), map, 12);
  EXPECT_FOUND_STATIC(Value(6.25, -6.25, 6.0), map, 13);
  EXPECT_FOUND_STATIC(Value(5.0, -5.0, 5.0), map, 14);
  // Value-initialized
  EXPECT_EQ_STATIC(Value(0.0, 0.0, 0.0), map[0]);
  EXPECT_NOT_FOUND_STATIC(Value(0.0, 0.0, 0.0), map, -1);
  // Safe integral comparison is used
  EXPECT_NOT_FOUND_STATIC(Value(0.0, 0.0, 0.0), map, 0x1'0000'000aULL);
}

TEST(FixedMap, UnsignedIntegralKeyFullyContinuous1) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = false,
  };
  test_unsigned_integral_key_common<options>();
}

TEST(FixedMap, UnsignedIntegralKeyFullyContinuous2) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .adjusts_alignment = true,
  };
  test_unsigned_integral_key_common<options>();
}
