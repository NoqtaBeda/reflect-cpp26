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

#include <reflect_cpp26/enum/enum_bitwise_operators.hpp>
#include <reflect_cpp26/enum/enum_flags_cast.hpp>

#include "tests/enum/flags_test_cases.hpp"
#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

TEST(EnumFlagsCastFromString, D1) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D1::one, rfl::enum_flags_cast<D1>("one"));
  EXPECT_EQ_STATIC(D1::two, rfl::enum_flags_cast<D1>("two"));
  EXPECT_EQ_STATIC(D1::four, rfl::enum_flags_cast<D1>("four"));
  EXPECT_EQ_STATIC(D1::eight, rfl::enum_flags_cast<D1>("eight"));

  EXPECT_EQ_STATIC(D1::one | D1::two, rfl::enum_flags_cast<D1>("one|two"));
  EXPECT_EQ_STATIC(D1::one | D1::four, rfl::enum_flags_cast<D1>("one | four"));
  EXPECT_EQ_STATIC(D1::two | D1::four, rfl::enum_flags_cast<D1>("two, four", ','));
  EXPECT_EQ_STATIC(D1::two | D1::four | D1::eight,
                   rfl::enum_flags_cast<D1>(" | two | four |      | eight | ", " | "));
  EXPECT_EQ_STATIC(D1::one | D1::four | D1::eight,
                   rfl::enum_flags_cast<D1>("one\nfour  \neight\n", '\n'));

  EXPECT_EQ_STATIC(static_cast<D1>(0), rfl::enum_flags_cast<D1>(""));
  EXPECT_EQ_STATIC(static_cast<D1>(0), rfl::enum_flags_cast<D1>(" ||  || "));
  EXPECT_EQ_STATIC(static_cast<D1>(0), rfl::enum_flags_cast<D1>(" ,, ,,", ','));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>("one two four"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>("one|two|three"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>("one|two|FOUR"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>("one|two|four", ','));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>("1|2|4"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>("one|two", " | "));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>("one \n two\nfour", ' '));
}

TEST(EnumFlagsCastFromString, D2) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D2::Zero, rfl::enum_flags_cast<D2>("Zero"));
  EXPECT_EQ_STATIC(D2::LING, rfl::enum_flags_cast<D2>("LING"));
  EXPECT_EQ_STATIC(D2::One, rfl::enum_flags_cast<D2>("One"));
  EXPECT_EQ_STATIC(D2::Two, rfl::enum_flags_cast<D2>("Two"));
  EXPECT_EQ_STATIC(D2::YI, rfl::enum_flags_cast<D2>("YI"));
  EXPECT_EQ_STATIC(D2::ER, rfl::enum_flags_cast<D2>("ER"));
  EXPECT_EQ_STATIC(D2::Three, rfl::enum_flags_cast<D2>("Three"));
  EXPECT_EQ_STATIC(D2::Four, rfl::enum_flags_cast<D2>("Four"));
  EXPECT_EQ_STATIC(D2::Five, rfl::enum_flags_cast<D2>("Five"));
  EXPECT_EQ_STATIC(D2::Six, rfl::enum_flags_cast<D2>("Six"));
  EXPECT_EQ_STATIC(D2::Seven, rfl::enum_flags_cast<D2>("Seven"));
  EXPECT_EQ_STATIC(D2::WU, rfl::enum_flags_cast<D2>("WU"));
  EXPECT_EQ_STATIC(D2::Liu, rfl::enum_flags_cast<D2>("Liu"));
  EXPECT_EQ_STATIC(D2::Qi, rfl::enum_flags_cast<D2>("Qi"));
  EXPECT_EQ_STATIC(D2::Eight, rfl::enum_flags_cast<D2>("Eight"));

  EXPECT_EQ_STATIC(D2::Eight | D2::Five, rfl::enum_flags_cast<D2>(" Eight | Five"));
  EXPECT_EQ_STATIC(D2::Eight | D2::Four | D2::ER,
                   rfl::enum_flags_cast<D2>(" Eight | Four | ER  | ", " | "));
  EXPECT_EQ_STATIC(D2::Eight | D2::Four | D2::ER | D2::YI,
                   rfl::enum_flags_cast<D2>(" Eight,Four\n,ER\t,YI\n ,,,", ','));
  EXPECT_EQ_STATIC(D2::Five, rfl::enum_flags_cast<D2>("Five Four WU YI\n One\n Zero LING\n", ' '));

  EXPECT_EQ_STATIC(D2::Zero, rfl::enum_flags_cast<D2>(""));
  EXPECT_EQ_STATIC(D2::Zero, rfl::enum_flags_cast<D2>(" ||  || "));
  EXPECT_EQ_STATIC(D2::Zero, rfl::enum_flags_cast<D2>(" ,, ,,", ','));
  EXPECT_EQ_STATIC(D2::Zero, rfl::enum_flags_cast<D2>("\tZero\n\n\t\n\nLING\t", '\n'));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>("ONE|two"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>("On|To"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>("1,2,3,4", ','));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>("Two|Four", ' '));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>("Two Four"));
}

TEST(EnumFlagsCastFromString, Empty) {
  EXPECT_EQ_STATIC(static_cast<empty>(0), rfl::enum_flags_cast<empty>(""));
  EXPECT_EQ_STATIC(static_cast<empty>(0), rfl::enum_flags_cast<empty>(" ||  || "));
  EXPECT_EQ_STATIC(static_cast<empty>(0), rfl::enum_flags_cast<empty>(" ,, \n ,, \t ,,", ','));
  EXPECT_EQ_STATIC(static_cast<empty>(0),
                   rfl::enum_flags_cast<empty>("ABABABABABABABABABABABABABAB", "AB"));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<empty>("0"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<empty>("empty"));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<empty>("| | |", ' '));
}
