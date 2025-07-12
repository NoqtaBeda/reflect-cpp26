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

TEST(EnumFlagsCastFromInteger, D1)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D1::one, rfl::enum_flags_cast<D1>(1));
  EXPECT_EQ_STATIC(D1::two, rfl::enum_flags_cast<D1>(2));
  EXPECT_EQ_STATIC(D1::four, rfl::enum_flags_cast<D1>(4));
  EXPECT_EQ_STATIC(D1::eight, rfl::enum_flags_cast<D1>(8));

  EXPECT_EQ_STATIC(D1::one | D1::two, rfl::enum_flags_cast<D1>(3));
  EXPECT_EQ_STATIC(D1::one | D1::four, rfl::enum_flags_cast<D1>(5));
  EXPECT_EQ_STATIC(D1::two | D1::eight, rfl::enum_flags_cast<D1>(10));
  EXPECT_EQ_STATIC(D1::one | D1::two | D1::eight, rfl::enum_flags_cast<D1>(11));
  EXPECT_EQ_STATIC(D1::one | D1::four | D1::eight,
    rfl::enum_flags_cast<D1>(13u));
  EXPECT_EQ_STATIC(D1::one | D1::two | D1::four | D1::eight,
    rfl::enum_flags_cast<D1>(15uLL));

  EXPECT_EQ_STATIC(static_cast<D1>(0), rfl::enum_flags_cast<D1>(0));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>(16));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>(17));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D1>(0x1000'0000'000F));
}

TEST(EnumFlagsCastFromInteger, D2)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D2::One, rfl::enum_flags_cast<D2>(1));
  EXPECT_EQ_STATIC(D2::Two, rfl::enum_flags_cast<D2>(2));
  EXPECT_EQ_STATIC(D2::Three, rfl::enum_flags_cast<D2>(3));
  EXPECT_EQ_STATIC(D2::Four, rfl::enum_flags_cast<D2>(4));
  EXPECT_EQ_STATIC(D2::Five, rfl::enum_flags_cast<D2>(5));
  EXPECT_EQ_STATIC(D2::Six, rfl::enum_flags_cast<D2>(6));
  EXPECT_EQ_STATIC(D2::Seven, rfl::enum_flags_cast<D2>(7));
  EXPECT_EQ_STATIC(D2::Eight, rfl::enum_flags_cast<D2>(8));

  EXPECT_EQ_STATIC(D2::One | D2::Eight, rfl::enum_flags_cast<D2>(9));
  EXPECT_EQ_STATIC(D2::Two | D2::Eight, rfl::enum_flags_cast<D2>(10));
  EXPECT_EQ_STATIC(D2::Three | D2::Eight, rfl::enum_flags_cast<D2>(11));
  EXPECT_EQ_STATIC(D2::Four | D2::Eight, rfl::enum_flags_cast<D2>(12));
  EXPECT_EQ_STATIC(D2::Five | D2::Eight, rfl::enum_flags_cast<D2>(13));
  EXPECT_EQ_STATIC(D2::Six | D2::Eight, rfl::enum_flags_cast<D2>(14));
  EXPECT_EQ_STATIC(D2::Seven | D2::Eight, rfl::enum_flags_cast<D2>(15));

  EXPECT_EQ_STATIC(D2::Zero, rfl::enum_flags_cast<D2>(0));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>(16));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>(-15));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D2>(0x1000'0000'000F));
}

TEST(EnumFlagsCastFromInteger, D3)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D3::FIRST, rfl::enum_flags_cast<D3>(0b0100'1001));
  EXPECT_EQ_STATIC(D3::SECOND, rfl::enum_flags_cast<D3>(0b1001'0010));
  EXPECT_EQ_STATIC(D3::THIRD, rfl::enum_flags_cast<D3>(0b0010'0100));

  EXPECT_EQ_STATIC(D3::FIRST | D3::SECOND,
    rfl::enum_flags_cast<D3>(0b1101'1011));
  EXPECT_EQ_STATIC(D3::SECOND | D3::THIRD,
    rfl::enum_flags_cast<D3>(0b1011'0110));
  EXPECT_EQ_STATIC(D3::FIRST | D3::THIRD,
    rfl::enum_flags_cast<D3>(0b0110'1101));
  EXPECT_EQ_STATIC(D3::FIRST | D3::SECOND | D3::THIRD,
    rfl::enum_flags_cast<D3>(0b1111'1111));

  EXPECT_EQ_STATIC(static_cast<D3>(0),
    rfl::enum_flags_cast<D3>(0b0000'0000));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0b0100'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0b0100'1000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0b1000'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0b1001'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0b1111'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0b0000'1111));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0b0010'0100'1001));
  EXPECT_EQ_STATIC(std::nullopt,
    rfl::enum_flags_cast<D3>(0b0001'0000'0000'0100'1001));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0xFFFF));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D3>(0xFFFF'FFFF));
  EXPECT_EQ_STATIC(std::nullopt,
    rfl::enum_flags_cast<D3>(0xFFFF'FFFF'FFFF'FFFF));
}

TEST(EnumFlagsCastFromInteger, D4)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D4::A, rfl::enum_flags_cast<D4>(0b0000'0011));
  EXPECT_EQ_STATIC(D4::B, rfl::enum_flags_cast<D4>(0b0000'1100));
  EXPECT_EQ_STATIC(D4::C, rfl::enum_flags_cast<D4>(0b0111'0000));
  EXPECT_EQ_STATIC(D4::D, rfl::enum_flags_cast<D4>(0b0000'1111));
  EXPECT_EQ_STATIC(D4::E, rfl::enum_flags_cast<D4>(0b1000'0000));
  EXPECT_EQ_STATIC(D4::F, rfl::enum_flags_cast<D4>(0b1111'0000));
  EXPECT_EQ_STATIC(D4::G, rfl::enum_flags_cast<D4>(0b1111'1100));
  EXPECT_EQ_STATIC(D4::H, rfl::enum_flags_cast<D4>(0b1111'0011));
  EXPECT_EQ_STATIC(D4::I, rfl::enum_flags_cast<D4>(0b1111'1111));

  EXPECT_EQ_STATIC(D4::B | D4::C, rfl::enum_flags_cast<D4>(0b0111'1100));
  EXPECT_EQ_STATIC(D4::C | D4::D, rfl::enum_flags_cast<D4>(0b0111'1111));
  EXPECT_EQ_STATIC(D4::A | D4::E, rfl::enum_flags_cast<D4>(0b1000'0011));
  EXPECT_EQ_STATIC(D4::D | D4::E, rfl::enum_flags_cast<D4>(0b1000'1111));
  EXPECT_EQ_STATIC(D4::D | D4::E, rfl::enum_flags_cast<D4>(0b1000'1111));

  EXPECT_EQ_STATIC(static_cast<D4>(0), rfl::enum_flags_cast<D4>(0b0000'0000));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b1100'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b1111'1000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b1111'1110));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b0001'0011));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b0011'0011));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b0111'0111));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b1100'1100));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0b1110'1110));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0xFFFF));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D4>(0xFFFF'FFFF));
  EXPECT_EQ_STATIC(std::nullopt,
    rfl::enum_flags_cast<D4>(0xFFFF'FFFF'FFFF'FFFF));
}

TEST(EnumFlagsCastFromInteger, D5)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(D5::A, rfl::enum_flags_cast<D5>(1));
  EXPECT_EQ_STATIC(D5::B, rfl::enum_flags_cast<D5>(2));
  EXPECT_EQ_STATIC(D5::C, rfl::enum_flags_cast<D5>(3));
  EXPECT_EQ_STATIC(D5::D, rfl::enum_flags_cast<D5>(12));
  EXPECT_EQ_STATIC(D5::E, rfl::enum_flags_cast<D5>(96));
  EXPECT_EQ_STATIC(D5::F, rfl::enum_flags_cast<D5>(32768));

  EXPECT_EQ_STATIC(D5::B | D5::D, rfl::enum_flags_cast<D5>(14));
  EXPECT_EQ_STATIC(D5::C | D5::D, rfl::enum_flags_cast<D5>(15));
  EXPECT_EQ_STATIC(D5::A | D5::E, rfl::enum_flags_cast<D5>(97));
  EXPECT_EQ_STATIC(D5::C | D5::E, rfl::enum_flags_cast<D5>(99));
  EXPECT_EQ_STATIC(D5::D | D5::E, rfl::enum_flags_cast<D5>(108));
  EXPECT_EQ_STATIC(D5::B | D5::D | D5::E, rfl::enum_flags_cast<D5>(110));
  EXPECT_EQ_STATIC(D5::B | D5::F, rfl::enum_flags_cast<D5>(32770));
  EXPECT_EQ_STATIC(D5::C | D5::D | D5::F, rfl::enum_flags_cast<D5>(32783));

  EXPECT_EQ_STATIC(static_cast<D5>(0), rfl::enum_flags_cast<D5>(0));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(4));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(6));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(16));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(32));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(64));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(128));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(31));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(95));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<D5>(int32_t{-32768}));
}

TEST(EnumFlagsCastFromInteger, E1)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(E1::A, rfl::enum_flags_cast<E1>(0b0001));
  EXPECT_EQ_STATIC(E1::B, rfl::enum_flags_cast<E1>(0b0011));
  EXPECT_EQ_STATIC(E1::C, rfl::enum_flags_cast<E1>(0b0111));
  EXPECT_EQ_STATIC(E1::D, rfl::enum_flags_cast<E1>(0b1111));
  EXPECT_EQ_STATIC(E1::E, rfl::enum_flags_cast<E1>(0b1100));
  EXPECT_EQ_STATIC(E1::F, rfl::enum_flags_cast<E1>(0b1000));

  EXPECT_EQ_STATIC(E1::A | E1::E, rfl::enum_flags_cast<E1>(0b1101));
  EXPECT_EQ_STATIC(E1::A | E1::F, rfl::enum_flags_cast<E1>(0b1001));
  EXPECT_EQ_STATIC(E1::B | E1::F, rfl::enum_flags_cast<E1>(0b1011));

  EXPECT_EQ_STATIC(static_cast<E1>(0), rfl::enum_flags_cast<E1>(0));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E1>(0b0010));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E1>(0b0110));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E1>(0b1110));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E1>(0b1'0001));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E1>(0x1'0001));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E1>(0x1'0001'0001));
}

TEST(EnumFlagsCastFromInteger, E2)
{
  using namespace rfl::enum_bitwise_operators;
  EXPECT_EQ_STATIC(E2::A, rfl::enum_flags_cast<E2>(0b0000'0001));
  EXPECT_EQ_STATIC(E2::B, rfl::enum_flags_cast<E2>(0b0000'0010));
  EXPECT_EQ_STATIC(E2::C, rfl::enum_flags_cast<E2>(0b0000'0100));
  EXPECT_EQ_STATIC(E2::D, rfl::enum_flags_cast<E2>(0b0000'1000));
  EXPECT_EQ_STATIC(E2::E, rfl::enum_flags_cast<E2>(0b0001'1000));
  EXPECT_EQ_STATIC(E2::F, rfl::enum_flags_cast<E2>(0b1000'0000));
  EXPECT_EQ_STATIC(E2::G, rfl::enum_flags_cast<E2>(0b1111'1111));

  EXPECT_EQ_STATIC(E2::A | E2::B, rfl::enum_flags_cast<E2>(0b0000'0011));
  EXPECT_EQ_STATIC(E2::A | E2::C, rfl::enum_flags_cast<E2>(0b0000'0101));
  EXPECT_EQ_STATIC(E2::A | E2::D, rfl::enum_flags_cast<E2>(0b0000'1001));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::D,
    rfl::enum_flags_cast<E2>(0b0000'1011));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::E,
    rfl::enum_flags_cast<E2>(0b0001'1011));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::C | E2::D,
    rfl::enum_flags_cast<E2>(0b0000'1111));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::C | E2::E,
    rfl::enum_flags_cast<E2>(0b0001'1111));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::D | E2::F,
    rfl::enum_flags_cast<E2>(0b1000'1011));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::E | E2::F,
    rfl::enum_flags_cast<E2>(0b1001'1011));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::C | E2::D | E2::F,
    rfl::enum_flags_cast<E2>(0b1000'1111));
  EXPECT_EQ_STATIC(E2::A | E2::B | E2::C | E2::E | E2::F,
    rfl::enum_flags_cast<E2>(0b1001'1111));

  EXPECT_EQ_STATIC(E2::Zero, rfl::enum_flags_cast<E2>(0));

  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E2>(0b0001'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E2>(0b0010'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E2>(0b0100'0000));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E2>(0xFFFF));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<E2>(0xFFFF'FFFF));
}

TEST(EnumFlagsCastFromInteger, Empty)
{
  EXPECT_EQ_STATIC(static_cast<empty>(0), rfl::enum_flags_cast<empty>(0));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<empty>(1));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<empty>(-1));
}

TEST(EnumFlagsCastFromInteger, SingleOne)
{
  EXPECT_EQ_STATIC(single_one::value, rfl::enum_flags_cast<single_one>(1));
  EXPECT_EQ_STATIC(static_cast<single_one>(0),
    rfl::enum_flags_cast<single_one>(0));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_one>(2));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_one>(-2));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_one>(257));
}

TEST(EnumFlagsCastFromInteger, SingleOneRep)
{
  EXPECT_EQ_STATIC(single_one_rep::Value,
    rfl::enum_flags_cast<single_one_rep>(1));
  EXPECT_EQ_STATIC(static_cast<single_one_rep>(0),
    rfl::enum_flags_cast<single_one_rep>(0));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_one_rep>(2));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_one_rep>(-2));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_one_rep>(257));
}

TEST(EnumFlagsCastFromInteger, SingleZero)
{
  EXPECT_EQ_STATIC(single_zero::value, rfl::enum_flags_cast<single_zero>(0));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_zero>(1));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_zero>(-1));
}

TEST(EnumFlagsCastFromInteger, SingleZeroRep)
{
  EXPECT_EQ_STATIC(single_zero_rep::Zero,
    rfl::enum_flags_cast<single_zero_rep>(0));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_zero_rep>(1));
  EXPECT_EQ_STATIC(std::nullopt, rfl::enum_flags_cast<single_zero_rep>(-1));
}
