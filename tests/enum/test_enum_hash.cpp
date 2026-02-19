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

#include <reflect_cpp26/enum/enum_hash.hpp>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

using namespace reflect_cpp26;

TEST(EnumHash, All) {
  EXPECT_EQ_STATIC(enum_hash_v<foo_signed>, enum_hash_v<foo_signed_reorder>);
  EXPECT_EQ_STATIC(enum_hash_v<const foo_signed>, enum_hash_v<volatile foo_signed_reorder>);
  EXPECT_NE_STATIC(enum_hash_v<foo_signed>, enum_hash_v<foo_signed_rep>);
  EXPECT_NE_STATIC(enum_hash_v<foo_signed>, enum_hash_v<bar_unsigned>);
  EXPECT_NE_STATIC(enum_hash_v<single>, enum_hash_v<empty>);
  EXPECT_NE_STATIC(enum_hash_v<single>, enum_hash_v<single_rep>);
}
