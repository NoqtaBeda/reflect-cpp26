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
#include <reflect_cpp26/enum/enum_values.hpp>
#endif

using namespace reflect_cpp26;

TEST(EnumValues, All)
{
  EXPECT_EQ_STATIC(foo_signed_rep::yi, enum_value<foo_signed_rep>(3));
  EXPECT_EQ_STATIC(foo_signed_rep::four,
    enum_value<const foo_signed_rep, enum_entry_order::by_value>(7));
  EXPECT_EQ_STATIC(foo_signed_rep::yi,
    enum_value<volatile foo_signed_rep, enum_entry_order::by_name>(9));
}

TEST(EnumValueConstants, All)
{
  EXPECT_TRUE_STATIC(std::is_same_v<
    decltype(enum_value_constants<baz_signed>()),
    constant<baz_signed::ling, baz_signed::yi, baz_signed::er,
      baz_signed::san, baz_signed::fuyi>>);
  EXPECT_TRUE_STATIC(std::is_same_v<
    decltype(enum_value_constants<baz_signed, enum_entry_order::by_value>()),
    constant<baz_signed::fuyi, baz_signed::ling, baz_signed::yi,
      baz_signed::er, baz_signed::san>>);
  EXPECT_TRUE_STATIC(std::is_same_v<
    decltype(enum_value_constants<baz_signed, enum_entry_order::by_name>()),
    constant<baz_signed::er, baz_signed::fuyi, baz_signed::ling,
      baz_signed::san, baz_signed::yi>>);

  EXPECT_TRUE_STATIC(std::is_same_v<
    decltype(enum_value_constant<2, baz_signed>()), constant<baz_signed::er>>);
  EXPECT_TRUE_STATIC(std::is_same_v<
    decltype(enum_value_constant<2, baz_signed, enum_entry_order::by_value>()),
    constant<baz_signed::yi>>);
  EXPECT_TRUE_STATIC(std::is_same_v<
    decltype(enum_value_constant<2, baz_signed, enum_entry_order::by_name>()),
    constant<baz_signed::ling>>);
}
