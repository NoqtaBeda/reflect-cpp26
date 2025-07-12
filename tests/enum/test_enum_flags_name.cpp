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
#include <climits>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_bitwise_operators.hpp>
#include <reflect_cpp26/enum/enum_flags_name.hpp>
#endif

namespace rfl = reflect_cpp26;

TEST(EnumFlagsName, D1)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC("one", rfl::enum_flags_name(D1::one));
  EXPECT_EQ_STATIC("two", rfl::enum_flags_name(D1::two, ';'));
  EXPECT_EQ_STATIC("four", rfl::enum_flags_name(D1::four, ',', "<n/a>"));
  EXPECT_EQ_STATIC("eight", rfl::enum_flags_name(D1::eight, ", ", "<n/a>"));

  EXPECT_EQ_STATIC("four|two|one",
    rfl::enum_flags_name(D1::one | D1::two | D1::four));
  EXPECT_EQ_STATIC("eight;two;one",
    rfl::enum_flags_name(D1::one | D1::two | D1::eight, ';'));
  EXPECT_EQ_STATIC("four,two",
    rfl::enum_flags_name(D1::two | D1::four, ',', "<n/a>"));
  EXPECT_EQ_STATIC("eight | four | two | one",
    rfl::enum_flags_name(D1::one | D1::two | D1::four | D1::eight, " | "));
  EXPECT_EQ_STATIC("eight + four",
    rfl::enum_flags_name(D1::four | D1::eight, " + ", "<n/a>"));

  // Note: 0 is considered as valid input (which represents empty set).
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D1>(0)));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D1>(0), '|', "<n/a>"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D1>(UINT_MAX), ';'));
  EXPECT_EQ_STATIC("<n/a>",
    rfl::enum_flags_name(static_cast<D1>(16), '|', "<n/a>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<D1>(31), "; ", "<invalid>"));
}

TEST(EnumFlagsName, D2)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_THAT(rfl::enum_flags_name(D2::One), testing::AnyOf("One", "YI"));
  EXPECT_THAT(rfl::enum_flags_name(D2::Two), testing::AnyOf("Two", "ER"));
  EXPECT_EQ_STATIC("Four", rfl::enum_flags_name(D2::Four));
  EXPECT_EQ_STATIC("Eight", rfl::enum_flags_name(D2::Eight));

  EXPECT_THAT(rfl::enum_flags_name(D2::Three),
    testing::AnyOf("Three", "Two|One", "ER|YI", "Two|YI", "ER|One"));
  EXPECT_THAT(rfl::enum_flags_name(D2::Six, ';'),
    testing::AnyOf("Six", "Liu", "Four;Two", "Four;ER"));
  EXPECT_THAT(rfl::enum_flags_name(D2::Eight | D2::Five, " + ", "<n/a>"),
    testing::AnyOf("Eight + Five", "Eight + WU",
                   "Eight + Four + One", "Eight + Four + YI"));
  // Incomplete list below
  EXPECT_THAT(rfl::enum_flags_name(D2::Eight | D2::Four | D2::Two | D2::One),
    testing::AnyOf("Eight|Seven", "Eight|Qi",
                   "Eight|Four|Two|One", "Eight|Four|ER|YI",
                   "Eight|Four|ER|One", "Eight|Four|Two|YI"));

  // Note: 0 is considered as valid input (which represents empty set).
  EXPECT_THAT(rfl::enum_flags_name(static_cast<D2>(0)),
    testing::AnyOf("Zero", "LING", ""));
  EXPECT_THAT(rfl::enum_flags_name(static_cast<D2>(0), '|', "<n/a>"),
    testing::AnyOf("Zero", "LING", ""));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D2>(INT_MAX), ';'));
  EXPECT_EQ_STATIC("<n/a>",
    rfl::enum_flags_name(static_cast<D2>(16), '|', "<n/a>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<D2>(31), "; ", "<invalid>"));
}

TEST(EnumFlagsName, D3)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC("FIRST", rfl::enum_flags_name(D3::FIRST));
  EXPECT_EQ_STATIC("SECOND", rfl::enum_flags_name(D3::SECOND));
  EXPECT_EQ_STATIC("THIRD", rfl::enum_flags_name(D3::THIRD));

  EXPECT_EQ_STATIC("SECOND,THIRD",
    rfl::enum_flags_name(static_cast<D3>(0b1011'0110), ','));
  EXPECT_EQ_STATIC("FIRST, THIRD",
    rfl::enum_flags_name(static_cast<D3>(0b0110'1101), ", "));
  EXPECT_EQ_STATIC("SECOND | FIRST",
    rfl::enum_flags_name(static_cast<D3>(0b1101'1011), " | ", "<n/a>"));
  EXPECT_EQ_STATIC("SECOND|FIRST|THIRD",
    rfl::enum_flags_name(static_cast<D3>(0b1111'1111)));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D3>(0)));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D3>(0), "|", "<n/a>"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D3>(0b1001)));
  EXPECT_EQ_STATIC("<n/a>",
    rfl::enum_flags_name(static_cast<D3>(0b1111), ';', "<n/a>"));
  EXPECT_EQ_STATIC("<X>",
    rfl::enum_flags_name(static_cast<D3>(0b1111'0000), ';', "<X>"));
  EXPECT_EQ_STATIC("<Y>",
    rfl::enum_flags_name(static_cast<D3>(0b0100'1000), "|", "<Y>"));
  EXPECT_EQ_STATIC("<Z>",
    rfl::enum_flags_name(static_cast<D3>(0b0100'1011), "+", "<Z>"));
}

TEST(EnumFlagsName, D4)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC("A", rfl::enum_flags_name(D4::A));
  EXPECT_EQ_STATIC("B", rfl::enum_flags_name(D4::B));
  EXPECT_THAT(rfl::enum_flags_name(D4::C), testing::AnyOf("C", "J"));
  EXPECT_EQ_STATIC("E", rfl::enum_flags_name(D4::E));

  // Note: The following candidate lists may be incomplete.
  EXPECT_THAT(rfl::enum_flags_name(D4::D), testing::AnyOf("D", "B|A"));
  EXPECT_THAT(rfl::enum_flags_name(D4::F, ';'),
    testing::AnyOf("F", "E;C", "E;J"));
  EXPECT_THAT(rfl::enum_flags_name(D4::G, ", "),
    testing::AnyOf("G", "E, C, B", "E, J, B", "F, B"));
  EXPECT_THAT(rfl::enum_flags_name(D4::H, " | ", "<n/a>"),
    testing::AnyOf("H", "L", "E | C | A", "E | J | A", "F | A"));
  EXPECT_THAT(rfl::enum_flags_name(D4::I, "--> ", "<invalid>"),
    testing::AnyOf("I", "M", "E--> C--> B--> A", "E--> J--> B--> A",
                   "G--> A", "H--> B", "L--> B",
                   "F--> D", "F--> B--> A"));
  EXPECT_THAT(rfl::enum_flags_name(D4::C | D4::A, "** "),
    testing::AnyOf("C** A", "J** A"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D4>(0)));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D4>(0), "|", "<n/a>"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D4>(0b1110'0000)));
  EXPECT_EQ_STATIC("x",
    rfl::enum_flags_name(static_cast<D4>(0b1111'1000), '|', "x"));
  EXPECT_EQ_STATIC("y",
    rfl::enum_flags_name(static_cast<D4>(0b0000'0111), '|', "y"));
  EXPECT_EQ_STATIC("z",
    rfl::enum_flags_name(static_cast<D4>(0b1110'1111), '|', "z"));
}

TEST(EnumFlagsName, D5)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC("A", rfl::enum_flags_name(D5::A));
  EXPECT_EQ_STATIC("B", rfl::enum_flags_name(D5::B));
  EXPECT_THAT(rfl::enum_flags_name(D5::C), testing::AnyOf("C", "B|A"));
  EXPECT_EQ_STATIC("D", rfl::enum_flags_name(D5::D));
  EXPECT_EQ_STATIC("E", rfl::enum_flags_name(D5::E));
  EXPECT_EQ_STATIC("F", rfl::enum_flags_name(D5::F));

  EXPECT_THAT(rfl::enum_flags_name(D5::A | D5::B | D5::E, " | "),
    testing::AnyOf("E | C", "E | B | A"));
  EXPECT_EQ_STATIC("E,D",
    rfl::enum_flags_name(D5::D | D5::E, ',', "<invalid>"));
  EXPECT_EQ_STATIC("F, E, D",
    rfl::enum_flags_name(D5::D | D5::E | D5::F, ", ", "<invalid>"));
  EXPECT_THAT(rfl::enum_flags_name(D5::C | D5::D | D5::E | D5::F, "-- "),
    testing::AnyOf("F-- E-- D-- C", "F-- E-- D-- B-- A"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D5>(0)));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D5>(0), "|", "<n/a>"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<D5>(16)));
  EXPECT_EQ_STATIC("x", rfl::enum_flags_name(static_cast<D5>(48), '|', "x"));
  EXPECT_EQ_STATIC("y", rfl::enum_flags_name(static_cast<D5>(255), '|', "y"));
  EXPECT_EQ_STATIC("z", rfl::enum_flags_name(static_cast<D5>(8), '|', "z"));
  EXPECT_EQ_STATIC("a", rfl::enum_flags_name(static_cast<D5>(64), '|', "a"));
  EXPECT_EQ_STATIC("b", rfl::enum_flags_name(static_cast<D5>(72), '|', "b"));
}

TEST(EnumFlagsName, E1)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC("A", rfl::enum_flags_name(E1::A));
  EXPECT_EQ_STATIC("B", rfl::enum_flags_name(E1::B));
  EXPECT_EQ_STATIC("C", rfl::enum_flags_name(E1::C));
  EXPECT_EQ_STATIC("D", rfl::enum_flags_name(E1::D));
  EXPECT_EQ_STATIC("E", rfl::enum_flags_name(E1::E));
  EXPECT_EQ_STATIC("F", rfl::enum_flags_name(E1::F));

  EXPECT_EQ_STATIC("E|A", rfl::enum_flags_name(E1::E | E1::A));
  EXPECT_EQ_STATIC("F,A", rfl::enum_flags_name(E1::F | E1::A, ','));
  EXPECT_EQ_STATIC("F, B", rfl::enum_flags_name(E1::F | E1::B, ", "));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<E1>(0)));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<E1>(0), "|", "<n/a>"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<E1>(0b1010)));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E1>(0b1010), '|', "<invalid>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E1>(0b0110), '|', "<invalid>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E1>(0b0101), '|', "<invalid>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E1>(0b1'0000), '|', "<invalid>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E1>(0b1'1111), '|', "<invalid>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E1>(0b1'0000'0001), '|', "<invalid>"));
}

TEST(EnumFlagsName, E2)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC("A", rfl::enum_flags_name(E2::A));
  EXPECT_EQ_STATIC("B", rfl::enum_flags_name(E2::B));
  EXPECT_EQ_STATIC("C", rfl::enum_flags_name(E2::C));
  EXPECT_EQ_STATIC("D", rfl::enum_flags_name(E2::D));
  EXPECT_EQ_STATIC("E", rfl::enum_flags_name(E2::E));
  EXPECT_EQ_STATIC("F", rfl::enum_flags_name(E2::F));
  EXPECT_EQ_STATIC("G", rfl::enum_flags_name(E2::G));

  EXPECT_EQ_STATIC("D|B|A", rfl::enum_flags_name(E2::A | E2::B | E2::D));
  EXPECT_EQ_STATIC("E,C,A", rfl::enum_flags_name(E2::A | E2::C | E2::E, ','));
  EXPECT_EQ_STATIC("F | D | B",
    rfl::enum_flags_name(E2::B | E2::D | E2::F, " | "));
  EXPECT_EQ_STATIC("G", rfl::enum_flags_name(E2::G));
  EXPECT_EQ_STATIC("D__ C__ B__ A",
    rfl::enum_flags_name(E2::A | E2::B | E2::C | E2::D, "__ "));
  EXPECT_EQ_STATIC("E__ C__ B__ A",
    rfl::enum_flags_name(E2::A | E2::B | E2::C | E2::E, "__ ", "<n/a>"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(E2::Zero));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(E2::Zero, "|", "<n/a>"));

  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<E2>(0b1001'0110), '|'));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E2>(0b0101'1010), '|', "<invalid>"));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<E2>(0b1100'0000), '|', "<invalid>"));
}

TEST(EnumFlagsName, Empty)
{
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<empty>(0)));
  EXPECT_EQ_STATIC("",
    rfl::enum_flags_name(static_cast<empty>(0), ", ", "<invalid>"));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<empty>(1)));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<empty>(1), ", ", "<invalid>"));
  EXPECT_EQ_STATIC("<n/a>",
    rfl::enum_flags_name(static_cast<empty>(-1), ',', "<n/a>"));
}

TEST(EnumFlagsName, SingleOne)
{
  EXPECT_EQ_STATIC("value", rfl::enum_flags_name(single_one::value));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<single_one>(0)));
  EXPECT_EQ_STATIC("",
    rfl::enum_flags_name(static_cast<single_one>(0), '|', "<invalid>"));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<single_one>(2)));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<single_one>(2), " | ", "<invalid>"));
}

TEST(EnumFlagsName, SingleOneRep)
{
  EXPECT_THAT(rfl::enum_flags_name(single_one_rep::the),
    testing::AnyOf("the", "ONLY", "Value", "IS", "One"));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<single_one_rep>(0)));
  EXPECT_EQ_STATIC("",
    rfl::enum_flags_name(static_cast<single_one_rep>(0), ", ", "<invalid>"));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<single_one_rep>(2)));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<single_one_rep>(2), ", ", "<invalid>"));
}

TEST(EnumFlagsName, SingleZero)
{
  EXPECT_THAT(rfl::enum_flags_name(single_zero::value, '|', "<invalid>"),
    testing::AnyOf("", "value"));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<single_zero>(1)));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<single_zero>(1), ',', "<invalid>"));
}

TEST(EnumFlagsName, SingleZeroRep)
{
  EXPECT_THAT(rfl::enum_flags_name(single_zero_rep::ONLY, '|', "<invalid>"),
    testing::AnyOf("", "ONLY", "Zero", "inside"));
  EXPECT_EQ_STATIC("", rfl::enum_flags_name(static_cast<single_zero_rep>(1)));
  EXPECT_EQ_STATIC("<invalid>",
    rfl::enum_flags_name(static_cast<single_zero_rep>(1), ',', "<invalid>"));
}
