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

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_flags_cast.hpp>
#endif

using namespace reflect_cpp26;

enum foo {
  foo_zero = 0,
  foo_one = 1,
  foo_two = 2,
  foo_four = 4,
  foo_eight = 8,
};

constexpr auto str_concat(std::string_view lhs, std::string_view rhs) -> std::string {
  return std::string{lhs} + rhs;
}

TEST(EnumFlagsCastBindExpression, All) {
  using namespace std::placeholders;

  constexpr auto F = enum_flags_cast<foo>(_2);
  EXPECT_EQ_STATIC(3, F("one", "foo_two|foo_one"));
  EXPECT_EQ_STATIC(std::nullopt, F("foo_one", "two|four"));
  EXPECT_EQ_STATIC(7, F("foo_one", 7));
  EXPECT_EQ_STATIC(std::nullopt, F(2, 17));

  constexpr auto G = enum_flags_cast<foo>(std::bind(str_concat, _2, _1), _3);
  EXPECT_EQ_STATIC(10, G("foo_eight", "foo_two | ", '|'));
  EXPECT_EQ_STATIC(std::nullopt, G("foo_eight", "Foo_Two | ", '|'));
  EXPECT_EQ_STATIC(std::nullopt, G("foo_eight", "foo_two | ", ", "));
  EXPECT_EQ_STATIC(std::nullopt, G("foo_eight | ", "foo_two", '|'));

  constexpr auto H =
      enum_flags_cast<foo>(ascii_case_insensitive, std::bind(str_concat, _2, _1), _4);
  EXPECT_EQ_STATIC(10, H("Foo_Eight", "Foo_Two | ", "(I have nothing to do)", '|'));
  EXPECT_EQ_STATIC(std::nullopt,
                   H("Foo_Eight", "Foo_Two | ", "(I have nothing to do either)", ", "));
}
