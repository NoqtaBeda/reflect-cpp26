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

#include "tests/enum/enum_test_options.hpp"
#include "tests/enum/test_cases.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_contains.hpp>
#endif

using namespace reflect_cpp26;

template <class E>
constexpr bool test_foo_signed_common()
{
  // Note: enum_contains(e) is not massively tested
  // since enum_contains<E>(i) forwards to it.
  static_assert(enum_contains(E::four));
  static_assert(NOT enum_contains(static_cast<E>(-3)));

  static_assert(enum_contains<E>(4));
  static_assert(enum_contains<E>(-1));
  static_assert(enum_contains<E>(6));
  static_assert(enum_contains<E>(5));
  static_assert(enum_contains<E>(0));
  static_assert(enum_contains<E>(2));
  static_assert(enum_contains<E>(7));
  static_assert(enum_contains<E>(-2));
  static_assert(enum_contains<E>(1));

  static_assert(enum_contains<E>(uint8_t{2}));
  static_assert(enum_contains<E>(uint16_t{4}));
  static_assert(enum_contains<E>(uint64_t{6}));

  static_assert(enum_contains<E>(int8_t{-2}));
  static_assert(enum_contains<E>(int16_t{-1}));
  static_assert(enum_contains<E>(int64_t{-2}));

  static_assert(NOT enum_contains<E>(static_cast<uint8_t>(-1)));
  static_assert(NOT enum_contains<E>(static_cast<uint16_t>(-2)));
  static_assert(NOT enum_contains<E>(static_cast<uint32_t>(-1)));
  static_assert(NOT enum_contains<E>(static_cast<uint64_t>(-2)));

  static_assert(NOT enum_contains<E>(3));
  static_assert(NOT enum_contains<E>(8u));
  static_assert(NOT enum_contains<E>('0'));
  return true;
}

static_assert(test_foo_signed_common<foo_signed>());
static_assert(test_foo_signed_common<foo_signed_reorder>());
static_assert(test_foo_signed_common<foo_signed_rep>());

static_assert(enum_contains<bar_unsigned>(10));
static_assert(enum_contains<bar_unsigned>(12));
static_assert(enum_contains<bar_unsigned>(3));
static_assert(enum_contains<bar_unsigned>(2));
static_assert(enum_contains<bar_unsigned>(-2u));
static_assert(enum_contains<bar_unsigned>(1));
static_assert(enum_contains<bar_unsigned>(11));
static_assert(enum_contains<bar_unsigned>(-1u));
static_assert(enum_contains<bar_unsigned>(13));
static_assert(enum_contains<bar_unsigned>(14));
static_assert(enum_contains<bar_unsigned>(0));

static_assert(NOT enum_contains<bar_unsigned>(static_cast<uint8_t>(-1)));
static_assert(NOT enum_contains<bar_unsigned>(static_cast<uint16_t>(-1)));
static_assert(NOT enum_contains<bar_unsigned>(static_cast<uint64_t>(-1)));

static_assert(NOT enum_contains<bar_unsigned>(static_cast<int8_t>(-1)));
static_assert(NOT enum_contains<bar_unsigned>(static_cast<int16_t>(-1)));
static_assert(NOT enum_contains<bar_unsigned>(static_cast<int32_t>(-1)));
static_assert(NOT enum_contains<bar_unsigned>(static_cast<int64_t>(-1)));

static_assert(NOT enum_contains<bar_unsigned>(4));
static_assert(NOT enum_contains<bar_unsigned>(15u));
static_assert(NOT enum_contains<bar_unsigned>(L'1'));

static_assert(enum_contains<baz_signed>(2));
static_assert(enum_contains<baz_signed>(-1));
static_assert(enum_contains<baz_signed>(3));
static_assert(enum_contains<baz_signed>(0));
static_assert(enum_contains<baz_signed>(1));

static_assert(enum_contains<baz_signed>(int8_t{-1}));
static_assert(enum_contains<baz_signed>(int32_t{-1}));
static_assert(enum_contains<baz_signed>(int64_t{-1}));

static_assert(NOT enum_contains<baz_signed>(static_cast<uint8_t>(-1)));
static_assert(NOT enum_contains<baz_signed>(static_cast<uint16_t>(-1)));
static_assert(NOT enum_contains<baz_signed>(static_cast<uint32_t>(-1)));
static_assert(NOT enum_contains<baz_signed>(static_cast<uint64_t>(-1)));

static_assert(NOT enum_contains<baz_signed>(4));
static_assert(NOT enum_contains<baz_signed>(5uL));
static_assert(NOT enum_contains<baz_signed>(u'6'));

static_assert(enum_contains<qux_unsigned>(0));
static_assert(enum_contains<qux_unsigned>(static_cast<uint16_t>(-1)));
static_assert(enum_contains<qux_unsigned>(1));
static_assert(enum_contains<qux_unsigned>(3));
static_assert(enum_contains<qux_unsigned>(4));
static_assert(enum_contains<qux_unsigned>(2));

static_assert(NOT enum_contains<qux_unsigned>(int8_t{-1}));
static_assert(NOT enum_contains<qux_unsigned>(int16_t{-1}));
static_assert(NOT enum_contains<qux_unsigned>(int32_t{-1}));
static_assert(NOT enum_contains<qux_unsigned>(int64_t{-1}));

static_assert(NOT enum_contains<qux_unsigned>(static_cast<uint8_t>(-1)));
static_assert(NOT enum_contains<qux_unsigned>(static_cast<uint32_t>(-1)));
static_assert(NOT enum_contains<qux_unsigned>(static_cast<uint64_t>(-1)));

static_assert(NOT enum_contains<qux_unsigned>(5LL));
static_assert(NOT enum_contains<qux_unsigned>(U'6'));

static_assert(NOT enum_contains<empty>(-1));
static_assert(NOT enum_contains<empty>(0));

static_assert(enum_contains<single>(233));

static_assert(NOT enum_contains<single>(-1));
static_assert(NOT enum_contains<single>(0));
static_assert(NOT enum_contains<single>(u8'2'));

static_assert(enum_contains<single_rep>(233));

static_assert(NOT enum_contains<single_rep>(-1));
static_assert(NOT enum_contains<single_rep>(0));
static_assert(NOT enum_contains<single_rep>(u8'3'));

static_assert(NOT enum_contains<color>(int8_t{-1}));
static_assert(NOT enum_contains<color>(int16_t{-1}));
static_assert(NOT enum_contains<color>(int32_t{-1}));
static_assert(NOT enum_contains<color>(int64_t{-1}));

static_assert(enum_contains<color>(static_cast<uint8_t>(-1)));
static_assert(enum_contains<color>(static_cast<uint16_t>(-1)));
static_assert(NOT enum_contains<color>(static_cast<uint32_t>(-1)));
static_assert(NOT enum_contains<color>(static_cast<uint64_t>(-1)));

static_assert(enum_contains<color>(0x6A5ACD));
static_assert(enum_contains<color>(0xFF7F50));
static_assert(enum_contains<color>(0xF08080));
static_assert(enum_contains<color>(0x808080));
static_assert(enum_contains<color>(0x4682B4));
static_assert(enum_contains<color>(0xF5F5F5));
static_assert(enum_contains<color>(0x008000));
static_assert(enum_contains<color>(0x48D1CC));
static_assert(enum_contains<color>(0xFFE4C4));
static_assert(enum_contains<color>(0x800000));
static_assert(enum_contains<color>(0xE6E6FA));
static_assert(enum_contains<color>(0xFA8072));
static_assert(enum_contains<color>(0xF0FFFF));
static_assert(enum_contains<color>(0x8A2BE2));
static_assert(enum_contains<color>(0x98FB98));
static_assert(enum_contains<color>(0x483D8B));
static_assert(enum_contains<color>(0x40E0D0));
static_assert(enum_contains<color>(0xFFFFFF));
static_assert(enum_contains<color>(0xEEE8AA));
static_assert(enum_contains<color>(0xFFF0F5));
static_assert(enum_contains<color>(0xFFEFD5));
static_assert(enum_contains<color>(0x00FF00));
static_assert(enum_contains<color>(0xFAFAD2));
static_assert(enum_contains<color>(0xE9967A));
static_assert(enum_contains<color>(0xFF00FF));
static_assert(enum_contains<color>(0x6495ED));
static_assert(enum_contains<color>(0xFF00FF));
static_assert(enum_contains<color>(0xE0FFFF));
static_assert(enum_contains<color>(0x0000FF));
static_assert(enum_contains<color>(0xFFA07A));
static_assert(enum_contains<color>(0x00FA9A));
static_assert(enum_contains<color>(0x7FFF00));
static_assert(enum_contains<color>(0x1E90FF));
static_assert(enum_contains<color>(0x7FFFD4));
static_assert(enum_contains<color>(0x808000));
static_assert(enum_contains<color>(0xFFFAFA));
static_assert(enum_contains<color>(0x9ACD32));
static_assert(enum_contains<color>(0x9400D3));
static_assert(enum_contains<color>(0xFFE4E1));
static_assert(enum_contains<color>(0xF4A460));
static_assert(enum_contains<color>(0xFFF5EE));
static_assert(enum_contains<color>(0xFF8C00));
static_assert(enum_contains<color>(0xA9A9A9));
static_assert(enum_contains<color>(0xDA70D6));
static_assert(enum_contains<color>(0xC71585));
static_assert(enum_contains<color>(0xD2B48C));
static_assert(enum_contains<color>(0x3CB371));
static_assert(enum_contains<color>(0xF8F8FF));
static_assert(enum_contains<color>(0xF0F8FF));
static_assert(enum_contains<color>(0xF0E68C));
static_assert(enum_contains<color>(0x4B0082));
static_assert(enum_contains<color>(0x4169E1));
static_assert(enum_contains<color>(0x00FFFF));
static_assert(enum_contains<color>(0x696969));
static_assert(enum_contains<color>(0xADFF2F));
static_assert(enum_contains<color>(0x8FBC8F));
static_assert(enum_contains<color>(0xD2691E));
static_assert(enum_contains<color>(0xB22222));
static_assert(enum_contains<color>(0x8B008B));
static_assert(enum_contains<color>(0xFF4500));
static_assert(enum_contains<color>(0xFFEBCD));
static_assert(enum_contains<color>(0xFAEBD7));
static_assert(enum_contains<color>(0xC0C0C0));
static_assert(enum_contains<color>(0xEE82EE));
static_assert(enum_contains<color>(0x778899));
static_assert(enum_contains<color>(0xADD8E6));
static_assert(enum_contains<color>(0x663399));
static_assert(enum_contains<color>(0x6B8E23));
static_assert(enum_contains<color>(0xFFF8DC));
static_assert(enum_contains<color>(0xDAA520));
static_assert(enum_contains<color>(0xB0C4DE));
static_assert(enum_contains<color>(0xFFFACD));
static_assert(enum_contains<color>(0x2E8B57));
static_assert(enum_contains<color>(0x006400));
static_assert(enum_contains<color>(0x32CD32));
static_assert(enum_contains<color>(0x000080));
static_assert(enum_contains<color>(0x87CEFA));
static_assert(enum_contains<color>(0x8B0000));
static_assert(enum_contains<color>(0x008B8B));
static_assert(enum_contains<color>(0x228B22));
static_assert(enum_contains<color>(0xA0522D));
static_assert(enum_contains<color>(0x008080));
static_assert(enum_contains<color>(0xFDF5E6));
static_assert(enum_contains<color>(0x5F9EA0));
static_assert(enum_contains<color>(0xFF6347));
static_assert(enum_contains<color>(0x7CFC00));
static_assert(enum_contains<color>(0x708090));
static_assert(enum_contains<color>(0xFFFF00));
static_assert(enum_contains<color>(0xFFB6C1));
static_assert(enum_contains<color>(0x800080));
static_assert(enum_contains<color>(0x00FF7F));
static_assert(enum_contains<color>(0xFFFFF0));
static_assert(enum_contains<color>(0xBC8F8F));
static_assert(enum_contains<color>(0x556B2F));
static_assert(enum_contains<color>(0x2F4F4F));
static_assert(enum_contains<color>(0xFFA500));
static_assert(enum_contains<color>(0x00BFFF));
static_assert(enum_contains<color>(0xB0E0E6));
static_assert(enum_contains<color>(0xFF0000));
static_assert(enum_contains<color>(0xFFC0CB));
static_assert(enum_contains<color>(0xFFDAB9));
static_assert(enum_contains<color>(0xFAF0E6));
static_assert(enum_contains<color>(0x0000CD));
static_assert(enum_contains<color>(0xDEB887));
static_assert(enum_contains<color>(0xDC143C));
static_assert(enum_contains<color>(0xF5DEB3));
static_assert(enum_contains<color>(0xF0FFF0));
static_assert(enum_contains<color>(0x20B2AA));
static_assert(enum_contains<color>(0xB8860B));
static_assert(enum_contains<color>(0xFFFFE0));
static_assert(enum_contains<color>(0xF5F5DC));
static_assert(enum_contains<color>(0x000000));
static_assert(enum_contains<color>(0xFFFAF0));
static_assert(enum_contains<color>(0xFF1493));
static_assert(enum_contains<color>(0x9932CC));
static_assert(enum_contains<color>(0xFFE4B5));
static_assert(enum_contains<color>(0xFFD700));
static_assert(enum_contains<color>(0x00FFFF));
static_assert(enum_contains<color>(0xBA55D3));
static_assert(enum_contains<color>(0x7B68EE));
static_assert(enum_contains<color>(0xFFDEAD));
static_assert(enum_contains<color>(0xBDB76B));
static_assert(enum_contains<color>(0xDB7093));
static_assert(enum_contains<color>(0x00008B));
static_assert(enum_contains<color>(0xA52A2A));
static_assert(enum_contains<color>(0xCD853F));
static_assert(enum_contains<color>(0xD8BFD8));
static_assert(enum_contains<color>(0xD3D3D3));
static_assert(enum_contains<color>(0xAFEEEE));
static_assert(enum_contains<color>(0x66CDAA));
static_assert(enum_contains<color>(0x00CED1));
static_assert(enum_contains<color>(0xDCDCDC));
static_assert(enum_contains<color>(0x8B4513));
static_assert(enum_contains<color>(0xCD5C5C));
static_assert(enum_contains<color>(0x191970));
static_assert(enum_contains<color>(0x87CEEB));
static_assert(enum_contains<color>(0xF5FFFA));
static_assert(enum_contains<color>(0xDDA0DD));
static_assert(enum_contains<color>(0x90EE90));
static_assert(enum_contains<color>(0x9370DB));
static_assert(enum_contains<color>(0xFF69B4));

static_assert(NOT enum_contains<terminal_color>(int8_t{-1}));
static_assert(NOT enum_contains<terminal_color>(int16_t{-1}));
static_assert(NOT enum_contains<terminal_color>(int32_t{-1}));
static_assert(NOT enum_contains<terminal_color>(int64_t{-1}));

static_assert(enum_contains<terminal_color>(' '));
static_assert(enum_contains<terminal_color>(L'!'));
static_assert(enum_contains<terminal_color>(u8'Z'));
static_assert(enum_contains<terminal_color>(u'['));
static_assert(enum_contains<terminal_color>(U']'));

static_assert(enum_contains<terminal_color>(36));
static_assert(enum_contains<terminal_color>(35));
static_assert(enum_contains<terminal_color>(93));
static_assert(enum_contains<terminal_color>(31));
static_assert(enum_contains<terminal_color>(95));
static_assert(enum_contains<terminal_color>(37));
static_assert(enum_contains<terminal_color>(32));
static_assert(enum_contains<terminal_color>(33));
static_assert(enum_contains<terminal_color>(97));
static_assert(enum_contains<terminal_color>(90));
static_assert(enum_contains<terminal_color>(96));
static_assert(enum_contains<terminal_color>(94));
static_assert(enum_contains<terminal_color>(91));
static_assert(enum_contains<terminal_color>(30));
static_assert(enum_contains<terminal_color>(92));
static_assert(enum_contains<terminal_color>(34));

static_assert(enum_contains<hash_collision>(0));
static_assert(enum_contains<hash_collision>(1));
static_assert(NOT enum_contains<hash_collision>(-1));

TEST(EnumContainsInteger, StaticAll) {
  EXPECT_TRUE(true); // All test cases done by static assertions above.
}
