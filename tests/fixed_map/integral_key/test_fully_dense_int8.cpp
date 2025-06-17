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

TEST(FixedMap, IntegralKeyInt8Full) {
  constexpr auto delta = 1000.0;
  using KVPair = std::pair<int8_t, double>;
  constexpr auto make_kv_pairs = [delta]() constexpr {
    auto res = std::vector<KVPair>(1 << CHAR_BIT);
    for (auto i = SCHAR_MIN;; i++) {
      res[i - SCHAR_MIN] = {i, i + delta};
      if (i == SCHAR_MAX) {
        break;
      }
    }
    return res;
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs() /* with default options */);
  static_assert(std::is_same_v<decltype(map)::result_type, const double&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("fully_dense_integral_key_map"));
  EXPECT_EQ_STATIC(1 << CHAR_BIT, map.size());
  EXPECT_EQ_STATIC(SCHAR_MIN, map.min_key());
  EXPECT_EQ_STATIC(SCHAR_MAX, map.max_key());

  for (auto i = SCHAR_MIN;; i++) {
    auto [v, found] = map.get(i);
    EXPECT_TRUE(found) << "Failed with i = " << i;
    EXPECT_EQ(i + delta, v) << "Failed with i = " << i;
    if (i == SCHAR_MAX) {
      break;
    }
  }
  EXPECT_EQ_STATIC(0.0, map[SCHAR_MIN - 1]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, SCHAR_MAX + 1);
}

TEST(FixedMap, IntegralKeyUInt8Full) {
  constexpr auto delta = 1000.0;
  using KVPair = std::pair<uint8_t, double>;
  constexpr auto make_kv_pairs = [delta]() constexpr {
    auto res = std::vector<KVPair>(1 << CHAR_BIT);
    for (auto i = 0; i <= UCHAR_MAX; i++) {
      res[i] = {i, i + delta};
    }
    return res;
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs() /* with default options */);
  static_assert(std::is_same_v<decltype(map)::result_type, const double&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("fully_dense_integral_key_map"));
  EXPECT_EQ_STATIC(1 << CHAR_BIT, map.size());
  EXPECT_EQ_STATIC(0, map.min_key());
  EXPECT_EQ_STATIC(UCHAR_MAX, map.max_key());

  for (auto i = 0; i <= UCHAR_MAX; i++) {
    auto [v, found] = map.get(i);
    EXPECT_TRUE(found) << "Failed with i = " << i;
    EXPECT_EQ(i + delta, v) << "Failed with i = " << i;
  }
  EXPECT_EQ_STATIC(0.0, map[-1]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, UCHAR_MAX + 1);
}
