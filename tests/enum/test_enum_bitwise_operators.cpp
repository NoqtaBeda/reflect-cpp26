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
#include <reflect_cpp26/enum/enum_bitwise_operators.hpp>
#endif

namespace rfl = reflect_cpp26;

enum class flags_1 {
  none = 0,
  one = 1,
  two = 2,
  three = 3,
  four = 4,
  five = 5,
  six = 6,
  seven = 7,
};

TEST(EnumBitwiseOperators, OutOfBox)
{
  using namespace rfl::bitwise_operators;
  EXPECT_EQ_STATIC(flags_1::seven, flags_1::one | flags_1::two | flags_1::four);
  EXPECT_EQ_STATIC(flags_1::four, flags_1::five & flags_1::six);
  EXPECT_EQ_STATIC(flags_1::three, flags_1::five ^ flags_1::six);
  EXPECT_EQ_STATIC(flags_1::two, ~flags_1::five & flags_1::seven);

  auto x = flags_1::six;
  x &= flags_1::three;
  EXPECT_EQ(flags_1::two, x);

  auto y = flags_1::four;
  y |= flags_1::two;
  EXPECT_EQ(flags_1::six, y);

  auto z = flags_1::four;
  z ^= flags_1::five;
  EXPECT_EQ(flags_1::one, z);
}

namespace foo_2 {
enum class flags_2 {
  NONE = 0,
  ONE = 1,
  TWO = 2,
  THREE = 3,
  FOUR = 4,
  FIVE = 5,
  SIX = 6,
  SEVEN = 7,
};
REFLECT_CPP26_DEFINE_ENUM_BITWISE_OPERATORS(flags_2)
} // namespace foo_2

TEST(EnumBitwiseOperators, DefinedViaMacro)
{
  using enum foo_2::flags_2;
  EXPECT_EQ_STATIC(SEVEN, ONE | TWO | FOUR);
  EXPECT_EQ_STATIC(FOUR, FIVE & SIX);
  EXPECT_EQ_STATIC(THREE, FIVE ^ SIX);
  EXPECT_EQ_STATIC(TWO, ~FIVE & SEVEN);

  auto x = SIX;
  x &= THREE;
  EXPECT_EQ(TWO, x);

  auto y = FOUR;
  y |= TWO;
  EXPECT_EQ(SIX, y);

  auto z = FOUR;
  z ^= FIVE;
  EXPECT_EQ(ONE, z);
}
