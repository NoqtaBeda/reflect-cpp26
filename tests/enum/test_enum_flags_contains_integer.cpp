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

#include <climits>

#include "tests/enum/flags_test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_bitwise_operators.hpp>
#include <reflect_cpp26/enum/enum_flags_contains.hpp>
#endif

namespace rfl = reflect_cpp26;

TEST(EnumFlagsContainsInteger, D1) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(D1::one));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(D1::two));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(4));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(8));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(D1::one | D1::four));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(D1::two | D1::four | D1::eight));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(7));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(15));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(static_cast<D1>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>(static_cast<D1>(16)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>(0x8000'0001u));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>(0x1'0000'0002));
}

TEST(EnumFlagsContainsInteger, D2) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(D2::One));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(D2::Two));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(D2::Five));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(3));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(6));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(8));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(D2::Eight | D2::Three));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(D2::Eight | D2::Four));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(D2::Eight | D2::Five));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(9));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(15));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(static_cast<D2>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>(static_cast<D2>(16)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>(31));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>(0x1'0000'0004));
}

TEST(EnumFlagsContainsInteger, D3) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(D3::FIRST));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(D3::SECOND));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(0b0010'0100));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(D3::FIRST | D3::SECOND));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(D3::SECOND | D3::THIRD));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(0b0110'1101));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(0b1111'1111));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(static_cast<int8_t>(-1)));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(static_cast<D3>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D3>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D3>(static_cast<D3>(0b0000'1001)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D3>(static_cast<D3>(0b0000'1111)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D3>(0b1'0100'1001));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D3>(static_cast<int8_t>(-128)));
}

TEST(EnumFlagsContainsInteger, D4) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(D4::A));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(D4::B));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(0b0111'0000));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(0b1000'0000));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(D4::D));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(D4::G));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(D4::J | D4::A));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(0b1111'0000));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(0b1000'1111));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(0b1111'1111));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(static_cast<D4>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D4>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D4>(static_cast<D4>(0b0000'1001)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D4>(static_cast<D4>(0b0001'1111)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D4>(static_cast<D4>(0b1010'1010)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D4>(static_cast<D4>(0b0101'0101)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D4>(0b1'1100'0000));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D4>(0b1'0000'0000'0000'1100));
}

TEST(EnumFlagsContainsInteger, D5) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(D5::A));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(D5::B));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(D5::C));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(12));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(96));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(0b1000'0000'0000'0000));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(D5::A | D5::C));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(D5::B | D5::D | D5::E));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(D5::A | D5::B | D5::C | D5::D));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(14));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(99));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(110));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(0x800C));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(static_cast<D5>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D5>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D5>(static_cast<D5>(7)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D5>(static_cast<D5>(16)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D5>(static_cast<D5>(80)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D5>(35));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D5>(0b1000'0000'0000'0100));
}

TEST(EnumFlagsContainsInteger, E1) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(E1::A));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(E1::B));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(E1::C));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(0b1111));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(0b1100));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(0b1000));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(E1::A | E1::E));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(E1::B | E1::F));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(0b1101));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(0b1001));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(static_cast<E1>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E1>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E1>(static_cast<E1>(0b1'0000)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E1>(0b1110));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E1>(0b0010));
}

TEST(EnumFlagsContainsInteger, E2) {
  using namespace rfl::enum_bitwise_operators;
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::A));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::B));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::C));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::G));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0b0000'1000));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0b0001'1000));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0b1000'0000));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::A | E2::D));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::B | E2::C | E2::E));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::C | E2::D | E2::G));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::A | E2::B | E2::C | E2::D));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0b0000'1011));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0b0001'1010));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0b1001'1001));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0b1111'1111));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(E2::Zero));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<E2>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E2>(static_cast<E2>(0b0001'0001)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E2>(static_cast<E2>(0b1111'1110)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E2>(0b0010'0000));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E2>(0b0010'1111));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<E2>(0b0100'0100));
}

TEST(EnumFlagsContainsInteger, Empty) {
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<empty>(static_cast<empty>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<empty>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<empty>(static_cast<empty>(1)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<empty>(-1));
}

TEST(EnumFlagsContainsInteger, SingleOne) {
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one>(single_one::value));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one>(1));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one>(static_cast<single_one>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one>(1));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_one>(static_cast<single_one>(2)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_one>(-2));
}

TEST(EnumFlagsContainsInteger, SingleOneRep) {
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one_rep>(single_one_rep::Value));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one_rep>(1));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one_rep>(static_cast<single_one_rep>(0)));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_one_rep>(1));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_one_rep>(static_cast<single_one_rep>(2)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_one_rep>(-2));
}

TEST(EnumFlagsContainsInteger, SingleZero) {
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_zero>(single_zero::value));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_zero>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_zero>(static_cast<single_zero>(1)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_zero>(-1));
}

TEST(EnumFlagsContainsInteger, SingleZeroRep) {
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_zero_rep>(single_zero_rep::Zero));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<single_zero_rep>(0));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_zero_rep>(static_cast<single_zero_rep>(1)));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<single_zero_rep>(-1));
}
