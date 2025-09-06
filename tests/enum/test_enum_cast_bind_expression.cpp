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
#include <reflect_cpp26/enum/enum_cast.hpp>
#endif

using namespace reflect_cpp26;

enum foo {
  foo_zero = 0,
  foo_one = 1,
  foo_two = 2,
  foo_four = 4,
  foo_eight = 8,
};

constexpr auto str_concat(std::string_view lhs, std::string_view rhs)
  -> std::string {
  return std::string{lhs} + rhs;
}

TEST(EnumCastBindExpression, All)
{
  using namespace std::placeholders;

  constexpr auto F = enum_cast<foo>(_2);
  EXPECT_EQ_STATIC(foo_two, F("one", "foo_two"));
  EXPECT_EQ_STATIC(std::nullopt, F("foo_one", "two"));
  EXPECT_EQ_STATIC(foo_four, F("foo_one", 4));
  EXPECT_EQ_STATIC(std::nullopt, F(2, -4));

  constexpr auto G = enum_cast<foo>(std::bind(str_concat, _2, _1));
  EXPECT_EQ_STATIC(foo_eight, G("eight", "foo_"));
  EXPECT_EQ_STATIC(std::nullopt, G("EIGHT", "foo_"));
  EXPECT_EQ_STATIC(std::nullopt, G("foo_", "eight"));

  constexpr auto H = enum_cast<foo>(
    ascii_case_insensitive, std::bind(str_concat, _2, _1));
  EXPECT_EQ_STATIC(foo_eight, H("eight", "foo_"));
  EXPECT_EQ_STATIC(foo_eight, H("EIGHT", "foo_"));
  EXPECT_EQ_STATIC(std::nullopt, H("foo_", "eight"));
}
