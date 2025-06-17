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

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_comparison_operators.hpp>
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

TEST(EnumComparisonOperators, OutOfBox) {
  using namespace rfl::enum_comparison_operators;

  EXPECT_TRUE_STATIC(flags_1::one < flags_1::two);
  EXPECT_FALSE_STATIC(flags_1::one > flags_1::two);
  EXPECT_TRUE_STATIC(flags_1::one <= flags_1::two);
  EXPECT_FALSE_STATIC(flags_1::one >= flags_1::two);
  EXPECT_FALSE_STATIC(flags_1::one == flags_1::two);
  EXPECT_TRUE_STATIC(flags_1::one != flags_1::two);
  EXPECT_EQ_STATIC(std::strong_ordering::less, flags_1::one <=> flags_1::two);

  constexpr auto x = flags_1::three;
  EXPECT_FALSE_STATIC(x < flags_1::three);
  EXPECT_FALSE_STATIC(x > flags_1::three);
  EXPECT_TRUE_STATIC(x <= flags_1::three);
  EXPECT_TRUE_STATIC(x >= flags_1::three);
  EXPECT_TRUE_STATIC(x == flags_1::three);
  EXPECT_FALSE_STATIC(x != flags_1::three);
  EXPECT_EQ_STATIC(std::strong_ordering::equal, x <=> flags_1::three);

  EXPECT_FALSE_STATIC(flags_1::five < flags_1::four);
  EXPECT_TRUE_STATIC(flags_1::five > flags_1::four);
  EXPECT_FALSE_STATIC(flags_1::five <= flags_1::four);
  EXPECT_TRUE_STATIC(flags_1::five >= flags_1::four);
  EXPECT_FALSE_STATIC(flags_1::five == flags_1::four);
  EXPECT_TRUE_STATIC(flags_1::five != flags_1::four);
  EXPECT_EQ_STATIC(std::strong_ordering::greater, flags_1::five <=> flags_1::four);
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
REFLECT_CPP26_DEFINE_ENUM_COMPARISON_OPERATORS(flags_2)
}  // namespace foo_2

TEST(EnumComparisonOperators, DefinedViaMacro) {
  using enum foo_2::flags_2;

  EXPECT_TRUE_STATIC(ONE < TWO);
  EXPECT_FALSE_STATIC(ONE > TWO);
  EXPECT_TRUE_STATIC(ONE <= TWO);
  EXPECT_FALSE_STATIC(ONE >= TWO);
  EXPECT_FALSE_STATIC(ONE == TWO);
  EXPECT_TRUE_STATIC(ONE != TWO);
  EXPECT_EQ_STATIC(std::strong_ordering::less, ONE <=> TWO);

  constexpr auto x = THREE;
  EXPECT_FALSE_STATIC(x < THREE);
  EXPECT_FALSE_STATIC(x > THREE);
  EXPECT_TRUE_STATIC(x <= THREE);
  EXPECT_TRUE_STATIC(x >= THREE);
  EXPECT_TRUE_STATIC(x == THREE);
  EXPECT_FALSE_STATIC(x != THREE);
  EXPECT_EQ_STATIC(std::strong_ordering::equal, x <=> THREE);

  EXPECT_FALSE_STATIC(FIVE < FOUR);
  EXPECT_TRUE_STATIC(FIVE > FOUR);
  EXPECT_FALSE_STATIC(FIVE <= FOUR);
  EXPECT_TRUE_STATIC(FIVE >= FOUR);
  EXPECT_FALSE_STATIC(FIVE == FOUR);
  EXPECT_TRUE_STATIC(FIVE != FOUR);
  EXPECT_EQ_STATIC(std::strong_ordering::greater, FIVE <=> FOUR);
}
