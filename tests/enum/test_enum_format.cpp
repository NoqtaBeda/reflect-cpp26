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

#include <reflect_cpp26/enum/enum_format.hpp>
#include <tests/test_options.hpp>

enum class foo_t {
  one = 1,
  two = 2,
  three = 3,
  four = 4,
  five = 5,
  six = 6,
  seven = 7,
  eight = 8,
};

TEST(EnumFormat, NonFlags) {
  EXPECT_EQ("one", std::format("{}", foo_t::one));
  EXPECT_EQ("three", std::format("{}", foo_t::three));
  EXPECT_EQ("five", std::format("{}", foo_t::five));
  EXPECT_EQ("seven", std::format("{}", foo_t::seven));

  EXPECT_EQ("(foo_t)0", std::format("{}", static_cast<foo_t>(0)));
  EXPECT_EQ("(foo_t)10", std::format("{}", static_cast<foo_t>(10)));
  EXPECT_EQ("(foo_t)-10", std::format("{}", static_cast<foo_t>(-10)));
}

TEST(EnumFormat, Flags) {
  EXPECT_EQ("one", std::format("{:f}", foo_t::one));
  EXPECT_THAT(std::format("{:F}", foo_t::three), testing::AnyOf("three", "two|one"));
  EXPECT_THAT(std::format("{:f | }", foo_t::five), testing::AnyOf("five", "four | one"));
  EXPECT_THAT(std::format("{:F, }", foo_t::seven), testing::AnyOf("seven", "four, two, one"));
  EXPECT_THAT(std::format("{:F, }", static_cast<foo_t>(11)),
              testing::AnyOf("eight, three", "eight, two, one"));
  EXPECT_THAT(std::format("{:f\n}", static_cast<foo_t>(15)),
              testing::AnyOf("eight\nseven", "eight\nfour\ntwo\none"));

  EXPECT_EQ("", std::format("{:F}", static_cast<foo_t>(0)));
  EXPECT_EQ("(foo_t)-10", std::format("{:F}", static_cast<foo_t>(-10)));
}
