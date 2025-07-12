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

#include "tests/enum/flags_test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_bitwise_operators.hpp>
#include <reflect_cpp26/enum/enum_flags_cast.hpp>
#endif

namespace rfl = reflect_cpp26;

template <class E>
constexpr auto enum_flags_cast_ci(std::string_view str) {
  return rfl::enum_flags_cast<E>(str, rfl::case_insensitive);
}

template <class E, class Delim>
constexpr auto enum_flags_cast_ci(std::string_view str, Delim delim) {
  return rfl::enum_flags_cast<E>(str, rfl::case_insensitive, delim);
}

TEST(EnumFlagsCastFromStringCI, D1)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D1::one, enum_flags_cast_ci<D1>("One"));
  EXPECT_EQ_STATIC(D1::two, enum_flags_cast_ci<D1>("TWO"));
  EXPECT_EQ_STATIC(D1::four, enum_flags_cast_ci<D1>("four"));
  EXPECT_EQ_STATIC(D1::eight, enum_flags_cast_ci<D1>("EIghT"));

  EXPECT_EQ_STATIC(D1::one | D1::two, enum_flags_cast_ci<D1>("one|TWO"));
  EXPECT_EQ_STATIC(D1::one | D1::four, enum_flags_cast_ci<D1>("one | Four"));
  EXPECT_EQ_STATIC(D1::two | D1::four,
    enum_flags_cast_ci<D1>("tWO, Four", ','));
  EXPECT_EQ_STATIC(D1::two | D1::four | D1::eight,
    enum_flags_cast_ci<D1>(" | tWo | fOUr |      | eiGhT | ", " | "));
  EXPECT_EQ_STATIC(D1::one | D1::four | D1::eight,
    enum_flags_cast_ci<D1>("oNe\nFour  \neIgHt\n", '\n'));

  EXPECT_EQ_STATIC(static_cast<D1>(0), enum_flags_cast_ci<D1>(""));
  EXPECT_EQ_STATIC(static_cast<D1>(0), enum_flags_cast_ci<D1>(" ||  || "));
  EXPECT_EQ_STATIC(static_cast<D1>(0), enum_flags_cast_ci<D1>(" ,, ,,", ','));

  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D1>("one two four"));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D1>("one|two|three"));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D1>("one|two|four", ','));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D1>("1|2|4"));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D1>("one|two", " | "));
  EXPECT_EQ_STATIC(std::nullopt,
    enum_flags_cast_ci<D1>("one \n two\nfour", ' '));
}

TEST(EnumFlagsCastFromStringCI, D2)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D2::Zero, enum_flags_cast_ci<D2>("zero"));
  EXPECT_EQ_STATIC(D2::LING, enum_flags_cast_ci<D2>("ling"));
  EXPECT_EQ_STATIC(D2::One, enum_flags_cast_ci<D2>("One"));
  EXPECT_EQ_STATIC(D2::Two, enum_flags_cast_ci<D2>("TWO"));
  EXPECT_EQ_STATIC(D2::YI, enum_flags_cast_ci<D2>("Yi"));
  EXPECT_EQ_STATIC(D2::ER, enum_flags_cast_ci<D2>("er"));
  EXPECT_EQ_STATIC(D2::Three, enum_flags_cast_ci<D2>("three"));
  EXPECT_EQ_STATIC(D2::Four, enum_flags_cast_ci<D2>("FOUr"));
  EXPECT_EQ_STATIC(D2::Five, enum_flags_cast_ci<D2>("FiVE"));
  EXPECT_EQ_STATIC(D2::Six, enum_flags_cast_ci<D2>("SiX"));
  EXPECT_EQ_STATIC(D2::Seven, enum_flags_cast_ci<D2>("SeVeN"));
  EXPECT_EQ_STATIC(D2::WU, enum_flags_cast_ci<D2>("Wu"));
  EXPECT_EQ_STATIC(D2::Liu, enum_flags_cast_ci<D2>("LIu"));
  EXPECT_EQ_STATIC(D2::Qi, enum_flags_cast_ci<D2>("qI"));
  EXPECT_EQ_STATIC(D2::Eight, enum_flags_cast_ci<D2>("EIGHT"));

  EXPECT_EQ_STATIC(D2::Eight | D2::Five,
    enum_flags_cast_ci<D2>(" eight | five"));
  EXPECT_EQ_STATIC(D2::Eight | D2::Four | D2::ER,
    enum_flags_cast_ci<D2>(" EIGHt | FoUr | Er  | ", " | "));
  EXPECT_EQ_STATIC(D2::Eight | D2::Four | D2::ER | D2::YI,
    enum_flags_cast_ci<D2>(" EighT,FoUr\n,er\t,YI\n ,,,", ','));
  EXPECT_EQ_STATIC(D2::Five,
    enum_flags_cast_ci<D2>("five four wu yi\n one\n zero LiNG\n", ' '));

  EXPECT_EQ_STATIC(D2::Zero, enum_flags_cast_ci<D2>(""));
  EXPECT_EQ_STATIC(D2::Zero, enum_flags_cast_ci<D2>(" ||  || "));
  EXPECT_EQ_STATIC(D2::Zero, enum_flags_cast_ci<D2>(" ,, ,,", ','));
  EXPECT_EQ_STATIC(D2::Zero,
    enum_flags_cast_ci<D2>("\tZero\n\n\t\n\nLING\t", '\n'));

  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D2>("Oné|two"));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D2>("On|To"));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D2>("1,2,3,4", ','));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D2>("Two|Four", ' '));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<D2>("Two Four"));
}

TEST(EnumFlagsCastFromStringCI, Empty)
{
  EXPECT_EQ_STATIC(static_cast<empty>(0), enum_flags_cast_ci<empty>(""));
  EXPECT_EQ_STATIC(static_cast<empty>(0),
    enum_flags_cast_ci<empty>(" ||  || "));
  EXPECT_EQ_STATIC(static_cast<empty>(0),
    enum_flags_cast_ci<empty>(" ,, \n ,, \t ,,", ','));
  EXPECT_EQ_STATIC(static_cast<empty>(0),
    enum_flags_cast_ci<empty>("ABABABABABABABABABABABABABAB", "AB"));

  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<empty>("0"));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<empty>("empty"));
  EXPECT_EQ_STATIC(std::nullopt, enum_flags_cast_ci<empty>("| | |", ' '));
}
