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

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_count.hpp>
#endif

using namespace reflect_cpp26;

TEST(EnumCount, All) {
  EXPECT_EQ_STATIC(9, enum_count_v<foo_signed>);
  EXPECT_EQ_STATIC(9, enum_count_v<const foo_signed_reorder>);
  EXPECT_EQ_STATIC(11, enum_count_v<volatile foo_signed_rep>);
  EXPECT_EQ_STATIC(11, enum_count_v<const volatile bar_unsigned>);
  EXPECT_EQ_STATIC(5, enum_count_v<baz_signed>);
  EXPECT_EQ_STATIC(6, enum_count_v<qux_unsigned>);
  EXPECT_EQ_STATIC(0, enum_count_v<empty>);
  EXPECT_EQ_STATIC(1, enum_count_v<const single>);
  EXPECT_EQ_STATIC(6, enum_count_v<volatile single_rep>);
  EXPECT_EQ_STATIC(141, enum_count_v<const volatile color>);
  EXPECT_EQ_STATIC(16, enum_count_v<terminal_color>);
  EXPECT_EQ_STATIC(2, enum_count_v<hash_collision>);
}
