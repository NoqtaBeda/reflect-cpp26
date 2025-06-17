/**
 * Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
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
#include <reflect_cpp26/enum/enum_hash.hpp>
#endif

using namespace reflect_cpp26;

TEST(EnumHash, All)
{
  EXPECT_EQ_STATIC(enum_hash<foo_signed>(), enum_hash<foo_signed_reorder>());
  EXPECT_EQ_STATIC(enum_hash<const foo_signed>(),
                   enum_hash<volatile foo_signed_reorder>());
  EXPECT_NE_STATIC(enum_hash<foo_signed>(), enum_hash<foo_signed_rep>());
  EXPECT_NE_STATIC(enum_hash<foo_signed>(), enum_hash<bar_unsigned>());
  EXPECT_NE_STATIC(enum_hash<single>(), enum_hash<empty>());
  EXPECT_NE_STATIC(enum_hash<single>(), enum_hash<single_rep>());
}
