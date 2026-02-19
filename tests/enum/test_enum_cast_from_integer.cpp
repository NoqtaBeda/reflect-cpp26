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

#include <reflect_cpp26/enum/enum_cast.hpp>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

using namespace reflect_cpp26;

template <class E>
constexpr bool test_foo_signed_common() {
  static_assert(E::four == enum_cast<E>(4));
  static_assert(E::invalid == enum_cast<E>(-1));
  static_assert(E::six == enum_cast<E>(6));
  static_assert(E::five == enum_cast<E>(5));
  static_assert(E::zero == enum_cast<E>(0));
  static_assert(E::two == enum_cast<E>(2));
  static_assert(E::seven == enum_cast<E>(7));
  static_assert(E::error == enum_cast<E>(-2));
  static_assert(E::one == enum_cast<E>(1));

  static_assert(E::two == enum_cast<E>(uint8_t{2}));
  static_assert(E::four == enum_cast<E>(uint16_t{4}));
  static_assert(E::six == enum_cast<E>(uint64_t{6}));

  static_assert(E::error == enum_cast<E>(int8_t{-2}));
  static_assert(E::invalid == enum_cast<E>(int16_t{-1}));
  static_assert(E::error == enum_cast<E>(int64_t{-2}));

  static_assert(std::nullopt == enum_cast<E>(static_cast<uint8_t>(-1)));
  static_assert(std::nullopt == enum_cast<E>(static_cast<uint16_t>(-2)));
  static_assert(std::nullopt == enum_cast<E>(static_cast<uint32_t>(-1)));
  static_assert(std::nullopt == enum_cast<E>(static_cast<uint64_t>(-2)));

  static_assert(std::nullopt == enum_cast<E>(3));
  static_assert(std::nullopt == enum_cast<E>(8u));
  static_assert(std::nullopt == enum_cast<E>('0'));
  return true;
}

static_assert(test_foo_signed_common<foo_signed>());
static_assert(test_foo_signed_common<foo_signed_reorder>());
static_assert(test_foo_signed_common<foo_signed_rep>());

static_assert(bar_unsigned::ten == enum_cast<bar_unsigned>(10));
static_assert(bar_unsigned::twelve == enum_cast<bar_unsigned>(12));
static_assert(bar_unsigned::three == enum_cast<bar_unsigned>(3));
static_assert(bar_unsigned::two == enum_cast<bar_unsigned>(2));
static_assert(bar_unsigned::error == enum_cast<bar_unsigned>(-2u));
static_assert(bar_unsigned::one == enum_cast<bar_unsigned>(1));
static_assert(bar_unsigned::eleven == enum_cast<bar_unsigned>(11));
static_assert(bar_unsigned::invalid == enum_cast<bar_unsigned>(-1u));
static_assert(bar_unsigned::thirteen == enum_cast<bar_unsigned>(13));
static_assert(bar_unsigned::fourteen == enum_cast<bar_unsigned>(14));
static_assert(bar_unsigned::zero == enum_cast<bar_unsigned>(0));

static_assert(std::nullopt == enum_cast<bar_unsigned>(static_cast<uint8_t>(-1)));
static_assert(std::nullopt == enum_cast<bar_unsigned>(static_cast<uint16_t>(-1)));
static_assert(std::nullopt == enum_cast<bar_unsigned>(static_cast<uint64_t>(-1)));

static_assert(std::nullopt == enum_cast<bar_unsigned>(static_cast<int8_t>(-1)));
static_assert(std::nullopt == enum_cast<bar_unsigned>(static_cast<int16_t>(-1)));
static_assert(std::nullopt == enum_cast<bar_unsigned>(static_cast<int32_t>(-1)));
static_assert(std::nullopt == enum_cast<bar_unsigned>(static_cast<int64_t>(-1)));

static_assert(std::nullopt == enum_cast<bar_unsigned>(4));
static_assert(std::nullopt == enum_cast<bar_unsigned>(15u));
static_assert(std::nullopt == enum_cast<bar_unsigned>(L'1'));

static_assert(baz_signed::er == enum_cast<baz_signed>(2));
static_assert(baz_signed::fuyi == enum_cast<baz_signed>(-1));
static_assert(baz_signed::san == enum_cast<baz_signed>(3));
static_assert(baz_signed::ling == enum_cast<baz_signed>(0));
static_assert(baz_signed::yi == enum_cast<baz_signed>(1));

static_assert(baz_signed::fuyi == enum_cast<baz_signed>(int8_t{-1}));
static_assert(baz_signed::fuyi == enum_cast<baz_signed>(int32_t{-1}));
static_assert(baz_signed::fuyi == enum_cast<baz_signed>(int64_t{-1}));

static_assert(std::nullopt == enum_cast<baz_signed>(static_cast<uint8_t>(-1)));
static_assert(std::nullopt == enum_cast<baz_signed>(static_cast<uint16_t>(-1)));
static_assert(std::nullopt == enum_cast<baz_signed>(static_cast<uint32_t>(-1)));
static_assert(std::nullopt == enum_cast<baz_signed>(static_cast<uint64_t>(-1)));

static_assert(std::nullopt == enum_cast<baz_signed>(4));
static_assert(std::nullopt == enum_cast<baz_signed>(5uL));
static_assert(std::nullopt == enum_cast<baz_signed>(u'6'));

static_assert(qux_unsigned::ling == enum_cast<qux_unsigned>(0));
static_assert(qux_unsigned::fuyi == enum_cast<qux_unsigned>(static_cast<uint16_t>(-1)));
static_assert(qux_unsigned::yi == enum_cast<qux_unsigned>(1));
static_assert(qux_unsigned::san == enum_cast<qux_unsigned>(3));
static_assert(qux_unsigned::si == enum_cast<qux_unsigned>(4));
static_assert(qux_unsigned::er == enum_cast<qux_unsigned>(2));

static_assert(std::nullopt == enum_cast<qux_unsigned>(int8_t{-1}));
static_assert(std::nullopt == enum_cast<qux_unsigned>(int16_t{-1}));
static_assert(std::nullopt == enum_cast<qux_unsigned>(int32_t{-1}));
static_assert(std::nullopt == enum_cast<qux_unsigned>(int64_t{-1}));

static_assert(std::nullopt == enum_cast<qux_unsigned>(static_cast<uint8_t>(-1)));
static_assert(std::nullopt == enum_cast<qux_unsigned>(static_cast<uint32_t>(-1)));
static_assert(std::nullopt == enum_cast<qux_unsigned>(static_cast<uint64_t>(-1)));

static_assert(std::nullopt == enum_cast<qux_unsigned>(5LL));
static_assert(std::nullopt == enum_cast<qux_unsigned>(U'6'));

static_assert(std::nullopt == enum_cast<empty>(-1));
static_assert(std::nullopt == enum_cast<empty>(0));

static_assert(single::value == enum_cast<single>(233));

static_assert(std::nullopt == enum_cast<single>(-1));
static_assert(std::nullopt == enum_cast<single>(0));
static_assert(std::nullopt == enum_cast<single>(u8'2'));

static_assert(single_rep::hahaha == enum_cast<single_rep>(233));

static_assert(std::nullopt == enum_cast<single_rep>(-1));
static_assert(std::nullopt == enum_cast<single_rep>(0));
static_assert(std::nullopt == enum_cast<single_rep>(u8'3'));

static_assert(std::nullopt == enum_cast<color>(int8_t{-1}));
static_assert(std::nullopt == enum_cast<color>(int16_t{-1}));
static_assert(std::nullopt == enum_cast<color>(int32_t{-1}));
static_assert(std::nullopt == enum_cast<color>(int64_t{-1}));

static_assert(color::blue == enum_cast<color>(static_cast<uint8_t>(-1)));
static_assert(color::cyan == enum_cast<color>(static_cast<uint16_t>(-1)));
static_assert(std::nullopt == enum_cast<color>(static_cast<uint32_t>(-1)));
static_assert(std::nullopt == enum_cast<color>(static_cast<uint64_t>(-1)));

static_assert(color::slate_blue == enum_cast<color>(0x6A5ACD));
static_assert(color::coral == enum_cast<color>(0xFF7F50));
static_assert(color::light_coral == enum_cast<color>(0xF08080));
static_assert(color::gray == enum_cast<color>(0x808080));
static_assert(color::steel_blue == enum_cast<color>(0x4682B4));
static_assert(color::white_smoke == enum_cast<color>(0xF5F5F5));
static_assert(color::green == enum_cast<color>(0x008000));
static_assert(color::medium_turquoise == enum_cast<color>(0x48D1CC));
static_assert(color::bisque == enum_cast<color>(0xFFE4C4));
static_assert(color::maroon == enum_cast<color>(0x800000));
static_assert(color::lavender == enum_cast<color>(0xE6E6FA));
static_assert(color::salmon == enum_cast<color>(0xFA8072));
static_assert(color::azure == enum_cast<color>(0xF0FFFF));
static_assert(color::blue_violet == enum_cast<color>(0x8A2BE2));
static_assert(color::pale_green == enum_cast<color>(0x98FB98));
static_assert(color::dark_slate_blue == enum_cast<color>(0x483D8B));
static_assert(color::turquoise == enum_cast<color>(0x40E0D0));
static_assert(color::white == enum_cast<color>(0xFFFFFF));
static_assert(color::pale_golden_rod == enum_cast<color>(0xEEE8AA));
static_assert(color::lavender_blush == enum_cast<color>(0xFFF0F5));
static_assert(color::papaya_whip == enum_cast<color>(0xFFEFD5));
static_assert(color::lime == enum_cast<color>(0x00FF00));
static_assert(color::light_golden_rod_yellow == enum_cast<color>(0xFAFAD2));
static_assert(color::dark_salmon == enum_cast<color>(0xE9967A));
static_assert(color::magenta == enum_cast<color>(0xFF00FF));
static_assert(color::cornflower_blue == enum_cast<color>(0x6495ED));
static_assert(color::fuchsia == enum_cast<color>(0xFF00FF));
static_assert(color::light_cyan == enum_cast<color>(0xE0FFFF));
static_assert(color::blue == enum_cast<color>(0x0000FF));
static_assert(color::light_salmon == enum_cast<color>(0xFFA07A));
static_assert(color::medium_spring_green == enum_cast<color>(0x00FA9A));
static_assert(color::chartreuse == enum_cast<color>(0x7FFF00));
static_assert(color::dodger_blue == enum_cast<color>(0x1E90FF));
static_assert(color::aquamarine == enum_cast<color>(0x7FFFD4));
static_assert(color::olive == enum_cast<color>(0x808000));
static_assert(color::snow == enum_cast<color>(0xFFFAFA));
static_assert(color::yellow_green == enum_cast<color>(0x9ACD32));
static_assert(color::dark_violet == enum_cast<color>(0x9400D3));
static_assert(color::misty_rose == enum_cast<color>(0xFFE4E1));
static_assert(color::sandy_brown == enum_cast<color>(0xF4A460));
static_assert(color::sea_shell == enum_cast<color>(0xFFF5EE));
static_assert(color::dark_orange == enum_cast<color>(0xFF8C00));
static_assert(color::dark_gray == enum_cast<color>(0xA9A9A9));
static_assert(color::orchid == enum_cast<color>(0xDA70D6));
static_assert(color::medium_violet_red == enum_cast<color>(0xC71585));
static_assert(color::tan == enum_cast<color>(0xD2B48C));
static_assert(color::medium_sea_green == enum_cast<color>(0x3CB371));
static_assert(color::ghost_white == enum_cast<color>(0xF8F8FF));
static_assert(color::alice_blue == enum_cast<color>(0xF0F8FF));
static_assert(color::khaki == enum_cast<color>(0xF0E68C));
static_assert(color::indigo == enum_cast<color>(0x4B0082));
static_assert(color::royal_blue == enum_cast<color>(0x4169E1));
static_assert(color::aqua == enum_cast<color>(0x00FFFF));
static_assert(color::dim_gray == enum_cast<color>(0x696969));
static_assert(color::green_yellow == enum_cast<color>(0xADFF2F));
static_assert(color::dark_sea_green == enum_cast<color>(0x8FBC8F));
static_assert(color::chocolate == enum_cast<color>(0xD2691E));
static_assert(color::fire_brick == enum_cast<color>(0xB22222));
static_assert(color::dark_magenta == enum_cast<color>(0x8B008B));
static_assert(color::orange_red == enum_cast<color>(0xFF4500));
static_assert(color::blanched_almond == enum_cast<color>(0xFFEBCD));
static_assert(color::antique_white == enum_cast<color>(0xFAEBD7));
static_assert(color::silver == enum_cast<color>(0xC0C0C0));
static_assert(color::violet == enum_cast<color>(0xEE82EE));
static_assert(color::light_slate_gray == enum_cast<color>(0x778899));
static_assert(color::light_blue == enum_cast<color>(0xADD8E6));
static_assert(color::rebecca_purple == enum_cast<color>(0x663399));
static_assert(color::olive_drab == enum_cast<color>(0x6B8E23));
static_assert(color::cornsilk == enum_cast<color>(0xFFF8DC));
static_assert(color::golden_rod == enum_cast<color>(0xDAA520));
static_assert(color::light_steel_blue == enum_cast<color>(0xB0C4DE));
static_assert(color::lemon_chiffon == enum_cast<color>(0xFFFACD));
static_assert(color::sea_green == enum_cast<color>(0x2E8B57));
static_assert(color::dark_green == enum_cast<color>(0x006400));
static_assert(color::lime_green == enum_cast<color>(0x32CD32));
static_assert(color::navy == enum_cast<color>(0x000080));
static_assert(color::light_sky_blue == enum_cast<color>(0x87CEFA));
static_assert(color::dark_red == enum_cast<color>(0x8B0000));
static_assert(color::dark_cyan == enum_cast<color>(0x008B8B));
static_assert(color::forest_green == enum_cast<color>(0x228B22));
static_assert(color::sienna == enum_cast<color>(0xA0522D));
static_assert(color::teal == enum_cast<color>(0x008080));
static_assert(color::old_lace == enum_cast<color>(0xFDF5E6));
static_assert(color::cadet_blue == enum_cast<color>(0x5F9EA0));
static_assert(color::tomato == enum_cast<color>(0xFF6347));
static_assert(color::lawn_green == enum_cast<color>(0x7CFC00));
static_assert(color::slate_gray == enum_cast<color>(0x708090));
static_assert(color::yellow == enum_cast<color>(0xFFFF00));
static_assert(color::light_pink == enum_cast<color>(0xFFB6C1));
static_assert(color::purple == enum_cast<color>(0x800080));
static_assert(color::spring_green == enum_cast<color>(0x00FF7F));
static_assert(color::ivory == enum_cast<color>(0xFFFFF0));
static_assert(color::rosy_brown == enum_cast<color>(0xBC8F8F));
static_assert(color::dark_olive_green == enum_cast<color>(0x556B2F));
static_assert(color::dark_slate_gray == enum_cast<color>(0x2F4F4F));
static_assert(color::orange == enum_cast<color>(0xFFA500));
static_assert(color::deep_sky_blue == enum_cast<color>(0x00BFFF));
static_assert(color::powder_blue == enum_cast<color>(0xB0E0E6));
static_assert(color::red == enum_cast<color>(0xFF0000));
static_assert(color::pink == enum_cast<color>(0xFFC0CB));
static_assert(color::peach_puff == enum_cast<color>(0xFFDAB9));
static_assert(color::linen == enum_cast<color>(0xFAF0E6));
static_assert(color::medium_blue == enum_cast<color>(0x0000CD));
static_assert(color::burly_wood == enum_cast<color>(0xDEB887));
static_assert(color::crimson == enum_cast<color>(0xDC143C));
static_assert(color::wheat == enum_cast<color>(0xF5DEB3));
static_assert(color::honey_dew == enum_cast<color>(0xF0FFF0));
static_assert(color::light_sea_green == enum_cast<color>(0x20B2AA));
static_assert(color::dark_golden_rod == enum_cast<color>(0xB8860B));
static_assert(color::light_yellow == enum_cast<color>(0xFFFFE0));
static_assert(color::beige == enum_cast<color>(0xF5F5DC));
static_assert(color::black == enum_cast<color>(0x000000));
static_assert(color::floral_white == enum_cast<color>(0xFFFAF0));
static_assert(color::deep_pink == enum_cast<color>(0xFF1493));
static_assert(color::dark_orchid == enum_cast<color>(0x9932CC));
static_assert(color::moccasin == enum_cast<color>(0xFFE4B5));
static_assert(color::gold == enum_cast<color>(0xFFD700));
static_assert(color::cyan == enum_cast<color>(0x00FFFF));
static_assert(color::medium_orchid == enum_cast<color>(0xBA55D3));
static_assert(color::medium_slate_blue == enum_cast<color>(0x7B68EE));
static_assert(color::navajo_white == enum_cast<color>(0xFFDEAD));
static_assert(color::dark_khaki == enum_cast<color>(0xBDB76B));
static_assert(color::pale_violet_red == enum_cast<color>(0xDB7093));
static_assert(color::dark_blue == enum_cast<color>(0x00008B));
static_assert(color::brown == enum_cast<color>(0xA52A2A));
static_assert(color::peru == enum_cast<color>(0xCD853F));
static_assert(color::thistle == enum_cast<color>(0xD8BFD8));
static_assert(color::light_gray == enum_cast<color>(0xD3D3D3));
static_assert(color::pale_turquoise == enum_cast<color>(0xAFEEEE));
static_assert(color::medium_aquamarine == enum_cast<color>(0x66CDAA));
static_assert(color::dark_turquoise == enum_cast<color>(0x00CED1));
static_assert(color::gainsboro == enum_cast<color>(0xDCDCDC));
static_assert(color::saddle_brown == enum_cast<color>(0x8B4513));
static_assert(color::indian_red == enum_cast<color>(0xCD5C5C));
static_assert(color::midnight_blue == enum_cast<color>(0x191970));
static_assert(color::sky_blue == enum_cast<color>(0x87CEEB));
static_assert(color::mint_cream == enum_cast<color>(0xF5FFFA));
static_assert(color::plum == enum_cast<color>(0xDDA0DD));
static_assert(color::light_green == enum_cast<color>(0x90EE90));
static_assert(color::medium_purple == enum_cast<color>(0x9370DB));
static_assert(color::hot_pink == enum_cast<color>(0xFF69B4));

static_assert(std::nullopt == enum_cast<terminal_color>(int8_t{-1}));
static_assert(std::nullopt == enum_cast<terminal_color>(int16_t{-1}));
static_assert(std::nullopt == enum_cast<terminal_color>(int32_t{-1}));
static_assert(std::nullopt == enum_cast<terminal_color>(int64_t{-1}));

static_assert(terminal_color::green == enum_cast<terminal_color>(' '));
static_assert(terminal_color::yellow == enum_cast<terminal_color>(L'!'));
static_assert(terminal_color::bright_black == enum_cast<terminal_color>(u8'Z'));
static_assert(terminal_color::bright_red == enum_cast<terminal_color>(u'['));
static_assert(terminal_color::bright_yellow == enum_cast<terminal_color>(U']'));

static_assert(terminal_color::cyan == enum_cast<terminal_color>(36));
static_assert(terminal_color::magenta == enum_cast<terminal_color>(35));
static_assert(terminal_color::bright_yellow == enum_cast<terminal_color>(93));
static_assert(terminal_color::red == enum_cast<terminal_color>(31));
static_assert(terminal_color::bright_magenta == enum_cast<terminal_color>(95));
static_assert(terminal_color::white == enum_cast<terminal_color>(37));
static_assert(terminal_color::green == enum_cast<terminal_color>(32));
static_assert(terminal_color::yellow == enum_cast<terminal_color>(33));
static_assert(terminal_color::bright_white == enum_cast<terminal_color>(97));
static_assert(terminal_color::bright_black == enum_cast<terminal_color>(90));
static_assert(terminal_color::bright_cyan == enum_cast<terminal_color>(96));
static_assert(terminal_color::bright_blue == enum_cast<terminal_color>(94));
static_assert(terminal_color::bright_red == enum_cast<terminal_color>(91));
static_assert(terminal_color::black == enum_cast<terminal_color>(30));
static_assert(terminal_color::bright_green == enum_cast<terminal_color>(92));
static_assert(terminal_color::blue == enum_cast<terminal_color>(34));

static_assert(hash_collision::_wSYZDRpiQJf8Rfv == enum_cast<hash_collision>(0));
static_assert(hash_collision::_cuFFJIHGp_jNJKS == enum_cast<hash_collision>(1));
static_assert(std::nullopt == enum_cast<hash_collision>(-1));

TEST(EnumCastFromInteger, StaticAll) {
  EXPECT_TRUE(true);  // All test cases done by static assertions above.
}
