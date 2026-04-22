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

TEST(UtilsUtility, InRangeBool) {
  // Valid bool values
  EXPECT_TRUE_STATIC(reflect_cpp26::in_range<bool>(0));
  EXPECT_TRUE_STATIC(reflect_cpp26::in_range<bool>(1));
  EXPECT_TRUE_STATIC(reflect_cpp26::in_range<bool>(false));
  EXPECT_TRUE_STATIC(reflect_cpp26::in_range<bool>(true));

  // Invalid bool values
  EXPECT_FALSE_STATIC(reflect_cpp26::in_range<bool>(2));
  EXPECT_FALSE_STATIC(reflect_cpp26::in_range<bool>(-1));
  EXPECT_FALSE_STATIC(reflect_cpp26::in_range<bool>(100));
  EXPECT_FALSE_STATIC(reflect_cpp26::in_range<bool>(-100));
}
