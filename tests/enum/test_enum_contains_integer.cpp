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

#include <reflect_cpp26/enum/enum_contains.hpp>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

using namespace reflect_cpp26;

template <class E>
constexpr bool test_foo_signed_common() {
  // Note: enum_contains(e) is not massively tested
  // since enum_contains<E>(i) forwards to it.
  EXPECT_TRUE_STATIC(enum_contains<E>(E::four));
  EXPECT_FALSE_STATIC(enum_contains<E>(static_cast<E>(-3)));

  EXPECT_TRUE_STATIC(enum_contains<E>(4));
  EXPECT_TRUE_STATIC(enum_contains<E>(-1));
  EXPECT_TRUE_STATIC(enum_contains<E>(6));
  EXPECT_TRUE_STATIC(enum_contains<E>(5));
  EXPECT_TRUE_STATIC(enum_contains<E>(0));
  EXPECT_TRUE_STATIC(enum_contains<E>(2));
  EXPECT_TRUE_STATIC(enum_contains<E>(7));
  EXPECT_TRUE_STATIC(enum_contains<E>(-2));
  EXPECT_TRUE_STATIC(enum_contains<E>(1));

  EXPECT_TRUE_STATIC(enum_contains<E>(uint8_t{2}));
  EXPECT_TRUE_STATIC(enum_contains<E>(uint16_t{4}));
  EXPECT_TRUE_STATIC(enum_contains<E>(uint64_t{6}));

  EXPECT_TRUE_STATIC(enum_contains<E>(int8_t{-2}));
  EXPECT_TRUE_STATIC(enum_contains<E>(int16_t{-1}));
  EXPECT_TRUE_STATIC(enum_contains<E>(int64_t{-2}));

  EXPECT_FALSE_STATIC(enum_contains<E>(static_cast<uint8_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<E>(static_cast<uint16_t>(-2)));
  EXPECT_FALSE_STATIC(enum_contains<E>(static_cast<uint32_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<E>(static_cast<uint64_t>(-2)));

  EXPECT_FALSE_STATIC(enum_contains<E>(3));
  EXPECT_FALSE_STATIC(enum_contains<E>(8u));
  EXPECT_FALSE_STATIC(enum_contains<E>('0'));
  return true;
}

TEST(EnumContainsInteger, FooSigned) {
  test_foo_signed_common<foo_signed>();
}

TEST(EnumContainsInteger, FooSignedReorder) {
  test_foo_signed_common<foo_signed_reorder>();
}

TEST(EnumContainsInteger, FooSignedRep) {
  test_foo_signed_common<foo_signed_rep>();
}

TEST(EnumContainsInteger, BarUnsigned) {
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(10));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(12));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(3));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(2));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(-2u));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(1));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(11));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(-1u));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(13));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(14));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>(0));

  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(static_cast<uint8_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(static_cast<uint16_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(static_cast<uint64_t>(-1)));

  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(static_cast<int8_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(static_cast<int16_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(static_cast<int32_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(static_cast<int64_t>(-1)));

  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(4));
  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(15u));
  EXPECT_FALSE_STATIC(enum_contains<bar_unsigned>(L'1'));
}

TEST(EnumContainsInteger, BazSigned) {
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(2));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(-1));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(3));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(0));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(1));

  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(int8_t{-1}));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(int32_t{-1}));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>(int64_t{-1}));

  EXPECT_FALSE_STATIC(enum_contains<baz_signed>(static_cast<uint8_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<baz_signed>(static_cast<uint16_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<baz_signed>(static_cast<uint32_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<baz_signed>(static_cast<uint64_t>(-1)));

  EXPECT_FALSE_STATIC(enum_contains<baz_signed>(4));
  EXPECT_FALSE_STATIC(enum_contains<baz_signed>(5uL));
  EXPECT_FALSE_STATIC(enum_contains<baz_signed>(u'6'));
}

TEST(EnumContainsInteger, QuxUnsigned) {
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>(0));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>(static_cast<uint16_t>(-1)));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>(1));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>(3));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>(4));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>(2));

  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(int8_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(int16_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(int32_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(int64_t{-1}));

  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(static_cast<uint8_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(static_cast<uint32_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(static_cast<uint64_t>(-1)));

  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(5LL));
  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>(U'6'));
}

TEST(EnumContainsInteger, Empty) {
  EXPECT_FALSE_STATIC(enum_contains<empty>(-1));
  EXPECT_FALSE_STATIC(enum_contains<empty>(0));
}

TEST(EnumCastFromInteger, Single) {
  EXPECT_TRUE_STATIC(enum_contains<single>(233));

  EXPECT_FALSE_STATIC(enum_contains<single>(-1));
  EXPECT_FALSE_STATIC(enum_contains<single>(0));
  EXPECT_FALSE_STATIC(enum_contains<single>(u8'2'));
}

TEST(EnumCastFromInteger, SingleRep) {
  EXPECT_TRUE_STATIC(enum_contains<single_rep>(233));

  EXPECT_FALSE_STATIC(enum_contains<single_rep>(-1));
  EXPECT_FALSE_STATIC(enum_contains<single_rep>(0));
  EXPECT_FALSE_STATIC(enum_contains<single_rep>(u8'3'));
}

TEST(EnumCastFromInteger, Color) {
  EXPECT_FALSE_STATIC(enum_contains<color>(int8_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<color>(int16_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<color>(int32_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<color>(int64_t{-1}));

  EXPECT_TRUE_STATIC(enum_contains<color>(static_cast<uint8_t>(-1)));
  EXPECT_TRUE_STATIC(enum_contains<color>(static_cast<uint16_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<color>(static_cast<uint32_t>(-1)));
  EXPECT_FALSE_STATIC(enum_contains<color>(static_cast<uint64_t>(-1)));

  ASSERT_TRUE_STATIC(enum_contains<color>(0x000000));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x000080));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x00008B));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x0000CD));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x0000FF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x006400));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x008000));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x008080));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x008B8B));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x00BFFF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x00CED1));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x00FA9A));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x00FF00));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x00FF7F));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x00FFFF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x191970));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x1E90FF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x20B2AA));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x228B22));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x2E8B57));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x2F4F4F));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x32CD32));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x3CB371));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x40E0D0));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x4169E1));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x4682B4));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x483D8B));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x48D1CC));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x4B0082));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x556B2F));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x5F9EA0));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x6495ED));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x663399));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x66CDAA));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x696969));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x6A5ACD));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x6B8E23));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x708090));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x778899));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x7B68EE));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x7CFC00));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x7FFF00));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x7FFFD4));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x800000));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x800080));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x808000));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x808080));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x87CEEB));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x87CEFA));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x8A2BE2));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x8B0000));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x8B008B));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x8B4513));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x8FBC8F));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x90EE90));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x9370DB));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x9400D3));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x98FB98));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x9932CC));
  ASSERT_TRUE_STATIC(enum_contains<color>(0x9ACD32));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xA0522D));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xA52A2A));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xA9A9A9));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xADD8E6));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xADFF2F));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xAFEEEE));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xB0C4DE));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xB0E0E6));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xB22222));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xB8860B));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xBA55D3));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xBC8F8F));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xBDB76B));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xC0C0C0));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xC71585));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xCD5C5C));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xCD853F));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xD2691E));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xD2B48C));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xD3D3D3));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xD8BFD8));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xDA70D6));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xDAA520));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xDB7093));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xDC143C));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xDCDCDC));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xDDA0DD));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xDEB887));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xE0FFFF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xE6E6FA));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xE9967A));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xEE82EE));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xEEE8AA));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF08080));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF0E68C));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF0F8FF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF0FFF0));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF0FFFF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF4A460));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF5DEB3));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF5F5DC));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF5F5F5));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF5FFFA));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xF8F8FF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFA8072));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFAEBD7));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFAF0E6));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFAFAD2));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFDF5E6));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF0000));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF00FF));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF1493));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF4500));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF6347));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF69B4));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF7F50));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFF8C00));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFA07A));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFA500));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFB6C1));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFC0CB));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFD700));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFDAB9));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFDEAD));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFE4B5));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFE4C4));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFE4E1));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFEBCD));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFEFD5));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFF0F5));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFF5EE));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFF8DC));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFFACD));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFFAF0));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFFAFA));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFFF00));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFFFE0));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFFFF0));
  ASSERT_TRUE_STATIC(enum_contains<color>(0xFFFFFF));
}

TEST(EnumCastFromInteger, TerminalColor) {
  EXPECT_FALSE_STATIC(enum_contains<terminal_color>(int8_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<terminal_color>(int16_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<terminal_color>(int32_t{-1}));
  EXPECT_FALSE_STATIC(enum_contains<terminal_color>(int64_t{-1}));

  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(' '));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(L'!'));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(u8'Z'));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(u'['));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(U']'));

  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(36));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(35));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(93));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(31));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(95));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(37));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(32));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(33));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(97));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(90));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(96));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(94));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(91));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(30));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(92));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>(34));
}

TEST(EnumCastFromInteger, HashCollision) {
  EXPECT_TRUE_STATIC(enum_contains<hash_collision>(0));
  EXPECT_TRUE_STATIC(enum_contains<hash_collision>(1));
  EXPECT_FALSE_STATIC(enum_contains<hash_collision>(-1));
}
