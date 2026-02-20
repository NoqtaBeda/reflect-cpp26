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

#include <reflect_cpp26/utils/utility.hpp>

#include "tests/test_options.hpp"

// Examples from docs/utils.md - Integer Comparison section
TEST(UtilsUtility, IntegerComparisonChar) {
  // reflect_cpp26 accepts them:
  EXPECT_TRUE_STATIC(reflect_cpp26::cmp_less('a', 100));  // OK: compares as int
}

TEST(UtilsUtility, IntegerComparisonBool) {
  // reflect_cpp26 accepts them:
  EXPECT_FALSE_STATIC(reflect_cpp26::cmp_less(true, 1));  // OK: compares as int
}

// Examples from docs/utils.md - In-Range Check section
TEST(UtilsUtility, InRange) {
  EXPECT_TRUE_STATIC(reflect_cpp26::in_range<uint8_t>(255));   // true
  EXPECT_FALSE_STATIC(reflect_cpp26::in_range<uint8_t>(256));  // false
  EXPECT_FALSE_STATIC(reflect_cpp26::in_range<uint8_t>(-1));   // false
  EXPECT_TRUE_STATIC(reflect_cpp26::in_range<int8_t>(127));    // true
  EXPECT_FALSE_STATIC(reflect_cpp26::in_range<int8_t>(128));   // false
}

// Examples from docs/utils.md - Underlying Type Conversion section
enum class color : uint8_t {
  red = 1,
  green = 2,
  blue = 3
};

TEST(UtilsUtility, ToUnderlying) {
  CONSTEVAL_ON_STATIC_TEST auto r = reflect_cpp26::to_underlying(color::red);  // r is uint8_t{1}
  EXPECT_EQ_STATIC(r, uint8_t{1});
  EXPECT_TRUE_STATIC((std::is_same_v<decltype(r), uint8_t>));
}

// Examples from docs/utils.md - Zero/Sign Extension section
TEST(UtilsUtility, ZeroSignExtend) {
  int8_t x = -1;
  // x = int8_t(0xFF). Zero-extends to uint64_t(0x00'00'00'FF)
  EXPECT_EQ_STATIC(reflect_cpp26::zero_extend<uint64_t>(x), uint64_t{255});
  // x = int8_t(0xFF). Sign-extends to int64_t(0xFF'FF'FF'FF)
  EXPECT_EQ_STATIC(reflect_cpp26::sign_extend<int64_t>(x), int64_t{-1});

  uint8_t y = 255;
  // y = uint8_t(0xFF). Zero-extends to uint64_t(0x00'00'00'FF)
  EXPECT_EQ_STATIC(reflect_cpp26::zero_extend<uint64_t>(y), uint64_t{255});
  // y = uint8_t(0xFF). Sign-extends to int64_t(0xFF'FF'FF'FF)
  EXPECT_EQ_STATIC(reflect_cpp26::sign_extend<int64_t>(y), int64_t{-1});
}
