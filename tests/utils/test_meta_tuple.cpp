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

#include <reflect_cpp26/utils/meta_tuple.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

TEST(UtilsMetaTuple, Basic) {
  using tuple_type = rfl::meta_tuple<char, int, double>;
  static_assert(std::tuple_size_v<tuple_type> == 3);
  static_assert(std::is_same_v<std::tuple_element_t<0, tuple_type>, char>);
  static_assert(std::is_same_v<std::tuple_element_t<1, tuple_type>, int>);
  static_assert(std::is_same_v<std::tuple_element_t<2, tuple_type>, double>);

  auto t = tuple_type{'0', 123, 2.5};
  // Free get() via ADL
  EXPECT_EQ('0', get<0>(t));
  EXPECT_EQ(123, get<1>(t));
  EXPECT_EQ(2.5, get<2>(t));
}

TEST(UtilsMetaTuple, CTAD) {
  constexpr auto x = 1;
  constexpr auto y = 2.0;
  auto t = rfl::meta_tuple{x, y};
  static_assert(std::is_same_v<rfl::meta_tuple<int, double>, decltype(t)>);
  get<0>(t) += 100;
  get<1>(t) += 1.5;
  EXPECT_EQ(101, get<0>(t));
  EXPECT_EQ(3.5, get<1>(t));
}
