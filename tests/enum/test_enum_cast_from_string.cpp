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

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_cast.hpp>
#endif

using namespace reflect_cpp26;

template <class E>
void test_invalid_cases_common() {
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<E>("ZERO"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<E>("hello_world"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<E>(" zero "));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<E>("0"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<E>(""));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<E>("-"));
}

template <class E>
void test_foo_signed_common() {
  ASSERT_EQ_STATIC(E::four, enum_cast<E>("four"));
  ASSERT_EQ_STATIC(E::one, enum_cast<E>("one"));
  ASSERT_EQ_STATIC(E::error, enum_cast<E>("error"));
  ASSERT_EQ_STATIC(E::five, enum_cast<E>("five"));
  ASSERT_EQ_STATIC(E::zero, enum_cast<E>("zero"));
  ASSERT_EQ_STATIC(E::seven, enum_cast<E>("seven"));
  ASSERT_EQ_STATIC(E::invalid, enum_cast<E>("invalid"));
  ASSERT_EQ_STATIC(E::six, enum_cast<E>("six"));
  ASSERT_EQ_STATIC(E::two, enum_cast<E>("two"));

  test_invalid_cases_common<E>();
}

TEST(EnumCastFromString, FooSigned) {
  test_foo_signed_common<foo_signed>();
}

TEST(EnumCastFromString, FooSignedReorder) {
  test_foo_signed_common<foo_signed_reorder>();
}

TEST(EnumCastFromString, FooSignedRep) {
  test_foo_signed_common<foo_signed_rep>();
  ASSERT_EQ_STATIC(foo_signed_rep::one, enum_cast<foo_signed_rep>("yi"));
  ASSERT_EQ_STATIC(foo_signed_rep::two, enum_cast<foo_signed_rep>("er"));
}

TEST(EnumCastFromString, BarUnsigned) {
  ASSERT_EQ_STATIC(bar_unsigned::fourteen, enum_cast<bar_unsigned>("fourteen"));
  ASSERT_EQ_STATIC(bar_unsigned::twelve, enum_cast<bar_unsigned>("twelve"));
  ASSERT_EQ_STATIC(bar_unsigned::one, enum_cast<bar_unsigned>("one"));
  ASSERT_EQ_STATIC(bar_unsigned::thirteen, enum_cast<bar_unsigned>("thirteen"));
  ASSERT_EQ_STATIC(bar_unsigned::zero, enum_cast<bar_unsigned>("zero"));
  ASSERT_EQ_STATIC(bar_unsigned::error, enum_cast<bar_unsigned>("error"));
  ASSERT_EQ_STATIC(bar_unsigned::ten, enum_cast<bar_unsigned>("ten"));
  ASSERT_EQ_STATIC(bar_unsigned::eleven, enum_cast<bar_unsigned>("eleven"));
  ASSERT_EQ_STATIC(bar_unsigned::three, enum_cast<bar_unsigned>("three"));
  ASSERT_EQ_STATIC(bar_unsigned::invalid, enum_cast<bar_unsigned>("invalid"));
  ASSERT_EQ_STATIC(bar_unsigned::two, enum_cast<bar_unsigned>("two"));

  test_invalid_cases_common<bar_unsigned>();
}

TEST(EnumCastFromString, BazSigned) {
  ASSERT_EQ_STATIC(baz_signed::san, enum_cast<baz_signed>("san"));
  ASSERT_EQ_STATIC(baz_signed::er, enum_cast<baz_signed>("er"));
  ASSERT_EQ_STATIC(baz_signed::yi, enum_cast<baz_signed>("yi"));
  ASSERT_EQ_STATIC(baz_signed::ling, enum_cast<baz_signed>("ling"));
  ASSERT_EQ_STATIC(baz_signed::fuyi, enum_cast<baz_signed>("fuyi"));

  test_invalid_cases_common<baz_signed>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<baz_signed>("one"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<baz_signed>("si"));
}

TEST(EnumCastFromString, QuxUnsigned) {
  ASSERT_EQ_STATIC(qux_unsigned::er, enum_cast<qux_unsigned>("er"));
  ASSERT_EQ_STATIC(qux_unsigned::fuyi, enum_cast<qux_unsigned>("fuyi"));
  ASSERT_EQ_STATIC(qux_unsigned::yi, enum_cast<qux_unsigned>("yi"));
  ASSERT_EQ_STATIC(qux_unsigned::san, enum_cast<qux_unsigned>("san"));
  ASSERT_EQ_STATIC(qux_unsigned::ling, enum_cast<qux_unsigned>("ling"));
  ASSERT_EQ_STATIC(qux_unsigned::si, enum_cast<qux_unsigned>("si"));

  test_invalid_cases_common<qux_unsigned>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<qux_unsigned>("3"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<qux_unsigned>("zero"));
}

TEST(EnumCastFromString, Empty) {
  test_invalid_cases_common<empty>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<empty>("zero"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<empty>("invalid"));
}

TEST(EnumCastFromString, Single) {
  ASSERT_EQ_STATIC(single::value, enum_cast<single>("value"));
  test_invalid_cases_common<single>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<single>("233"));
}

TEST(EnumCastFromString, SingleRep) {
  ASSERT_EQ_STATIC(single_rep::hahaha, enum_cast<single_rep>("laugh"));
  ASSERT_EQ_STATIC(single_rep::hahaha, enum_cast<single_rep>("hahaha"));
  ASSERT_EQ_STATIC(single_rep::hahaha, enum_cast<single_rep>("over"));
  ASSERT_EQ_STATIC(single_rep::hahaha, enum_cast<single_rep>("and_over"));
  ASSERT_EQ_STATIC(single_rep::hahaha, enum_cast<single_rep>("again"));
  ASSERT_EQ_STATIC(single_rep::hahaha, enum_cast<single_rep>("unstoppable"));

  test_invalid_cases_common<single_rep>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<single_rep>("value"));
}

TEST(EnumCastFromString, Color) {
  ASSERT_EQ_STATIC(color::hot_pink, enum_cast<color>("hot_pink"));
  ASSERT_EQ_STATIC(color::purple, enum_cast<color>("purple"));
  ASSERT_EQ_STATIC(color::blanched_almond, enum_cast<color>("blanched_almond"));
  ASSERT_EQ_STATIC(color::steel_blue, enum_cast<color>("steel_blue"));
  ASSERT_EQ_STATIC(color::alice_blue, enum_cast<color>("alice_blue"));
  ASSERT_EQ_STATIC(color::maroon, enum_cast<color>("maroon"));
  ASSERT_EQ_STATIC(color::medium_blue, enum_cast<color>("medium_blue"));
  ASSERT_EQ_STATIC(color::fire_brick, enum_cast<color>("fire_brick"));
  ASSERT_EQ_STATIC(color::yellow_green, enum_cast<color>("yellow_green"));
  ASSERT_EQ_STATIC(color::light_gray, enum_cast<color>("light_gray"));
  ASSERT_EQ_STATIC(color::salmon, enum_cast<color>("salmon"));
  ASSERT_EQ_STATIC(color::medium_slate_blue, enum_cast<color>("medium_slate_blue"));
  ASSERT_EQ_STATIC(color::dark_turquoise, enum_cast<color>("dark_turquoise"));
  ASSERT_EQ_STATIC(color::medium_sea_green, enum_cast<color>("medium_sea_green"));
  ASSERT_EQ_STATIC(color::dark_sea_green, enum_cast<color>("dark_sea_green"));
  ASSERT_EQ_STATIC(color::pale_green, enum_cast<color>("pale_green"));
  ASSERT_EQ_STATIC(color::dark_orange, enum_cast<color>("dark_orange"));
  ASSERT_EQ_STATIC(color::light_blue, enum_cast<color>("light_blue"));
  ASSERT_EQ_STATIC(color::olive, enum_cast<color>("olive"));
  ASSERT_EQ_STATIC(color::orange_red, enum_cast<color>("orange_red"));
  ASSERT_EQ_STATIC(color::mint_cream, enum_cast<color>("mint_cream"));
  ASSERT_EQ_STATIC(color::tomato, enum_cast<color>("tomato"));
  ASSERT_EQ_STATIC(color::lavender_blush, enum_cast<color>("lavender_blush"));
  ASSERT_EQ_STATIC(color::pale_violet_red, enum_cast<color>("pale_violet_red"));
  ASSERT_EQ_STATIC(color::dark_olive_green, enum_cast<color>("dark_olive_green"));
  ASSERT_EQ_STATIC(color::moccasin, enum_cast<color>("moccasin"));
  ASSERT_EQ_STATIC(color::crimson, enum_cast<color>("crimson"));
  ASSERT_EQ_STATIC(color::deep_sky_blue, enum_cast<color>("deep_sky_blue"));
  ASSERT_EQ_STATIC(color::pink, enum_cast<color>("pink"));
  ASSERT_EQ_STATIC(color::burly_wood, enum_cast<color>("burly_wood"));
  ASSERT_EQ_STATIC(color::sienna, enum_cast<color>("sienna"));
  ASSERT_EQ_STATIC(color::dark_golden_rod, enum_cast<color>("dark_golden_rod"));
  ASSERT_EQ_STATIC(color::old_lace, enum_cast<color>("old_lace"));
  ASSERT_EQ_STATIC(color::pale_golden_rod, enum_cast<color>("pale_golden_rod"));
  ASSERT_EQ_STATIC(color::gainsboro, enum_cast<color>("gainsboro"));
  ASSERT_EQ_STATIC(color::honey_dew, enum_cast<color>("honey_dew"));
  ASSERT_EQ_STATIC(color::dark_green, enum_cast<color>("dark_green"));
  ASSERT_EQ_STATIC(color::light_coral, enum_cast<color>("light_coral"));
  ASSERT_EQ_STATIC(color::peach_puff, enum_cast<color>("peach_puff"));
  ASSERT_EQ_STATIC(color::deep_pink, enum_cast<color>("deep_pink"));
  ASSERT_EQ_STATIC(color::light_sea_green, enum_cast<color>("light_sea_green"));
  ASSERT_EQ_STATIC(color::gray, enum_cast<color>("gray"));
  ASSERT_EQ_STATIC(color::sky_blue, enum_cast<color>("sky_blue"));
  ASSERT_EQ_STATIC(color::lime, enum_cast<color>("lime"));
  ASSERT_EQ_STATIC(color::cornsilk, enum_cast<color>("cornsilk"));
  ASSERT_EQ_STATIC(color::light_golden_rod_yellow, enum_cast<color>("light_golden_rod_yellow"));
  ASSERT_EQ_STATIC(color::blue_violet, enum_cast<color>("blue_violet"));
  ASSERT_EQ_STATIC(color::light_slate_gray, enum_cast<color>("light_slate_gray"));
  ASSERT_EQ_STATIC(color::saddle_brown, enum_cast<color>("saddle_brown"));
  ASSERT_EQ_STATIC(color::navy, enum_cast<color>("navy"));
  ASSERT_EQ_STATIC(color::aquamarine, enum_cast<color>("aquamarine"));
  ASSERT_EQ_STATIC(color::olive_drab, enum_cast<color>("olive_drab"));
  ASSERT_EQ_STATIC(color::sandy_brown, enum_cast<color>("sandy_brown"));
  ASSERT_EQ_STATIC(color::powder_blue, enum_cast<color>("powder_blue"));
  ASSERT_EQ_STATIC(color::dark_gray, enum_cast<color>("dark_gray"));
  ASSERT_EQ_STATIC(color::medium_purple, enum_cast<color>("medium_purple"));
  ASSERT_EQ_STATIC(color::violet, enum_cast<color>("violet"));
  ASSERT_EQ_STATIC(color::midnight_blue, enum_cast<color>("midnight_blue"));
  ASSERT_EQ_STATIC(color::chartreuse, enum_cast<color>("chartreuse"));
  ASSERT_EQ_STATIC(color::spring_green, enum_cast<color>("spring_green"));
  ASSERT_EQ_STATIC(color::forest_green, enum_cast<color>("forest_green"));
  ASSERT_EQ_STATIC(color::cadet_blue, enum_cast<color>("cadet_blue"));
  ASSERT_EQ_STATIC(color::dim_gray, enum_cast<color>("dim_gray"));
  ASSERT_EQ_STATIC(color::royal_blue, enum_cast<color>("royal_blue"));
  ASSERT_EQ_STATIC(color::light_yellow, enum_cast<color>("light_yellow"));
  ASSERT_EQ_STATIC(color::lemon_chiffon, enum_cast<color>("lemon_chiffon"));
  ASSERT_EQ_STATIC(color::chocolate, enum_cast<color>("chocolate"));
  ASSERT_EQ_STATIC(color::thistle, enum_cast<color>("thistle"));
  ASSERT_EQ_STATIC(color::peru, enum_cast<color>("peru"));
  ASSERT_EQ_STATIC(color::cornflower_blue, enum_cast<color>("cornflower_blue"));
  ASSERT_EQ_STATIC(color::gold, enum_cast<color>("gold"));
  ASSERT_EQ_STATIC(color::cyan, enum_cast<color>("cyan"));
  ASSERT_EQ_STATIC(color::medium_turquoise, enum_cast<color>("medium_turquoise"));
  ASSERT_EQ_STATIC(color::turquoise, enum_cast<color>("turquoise"));
  ASSERT_EQ_STATIC(color::ghost_white, enum_cast<color>("ghost_white"));
  ASSERT_EQ_STATIC(color::slate_gray, enum_cast<color>("slate_gray"));
  ASSERT_EQ_STATIC(color::indian_red, enum_cast<color>("indian_red"));
  ASSERT_EQ_STATIC(color::wheat, enum_cast<color>("wheat"));
  ASSERT_EQ_STATIC(color::white, enum_cast<color>("white"));
  ASSERT_EQ_STATIC(color::light_pink, enum_cast<color>("light_pink"));
  ASSERT_EQ_STATIC(color::medium_spring_green, enum_cast<color>("medium_spring_green"));
  ASSERT_EQ_STATIC(color::medium_aquamarine, enum_cast<color>("medium_aquamarine"));
  ASSERT_EQ_STATIC(color::magenta, enum_cast<color>("magenta"));
  ASSERT_EQ_STATIC(color::khaki, enum_cast<color>("khaki"));
  ASSERT_EQ_STATIC(color::medium_violet_red, enum_cast<color>("medium_violet_red"));
  ASSERT_EQ_STATIC(color::papaya_whip, enum_cast<color>("papaya_whip"));
  ASSERT_EQ_STATIC(color::navajo_white, enum_cast<color>("navajo_white"));
  ASSERT_EQ_STATIC(color::light_green, enum_cast<color>("light_green"));
  ASSERT_EQ_STATIC(color::green, enum_cast<color>("green"));
  ASSERT_EQ_STATIC(color::teal, enum_cast<color>("teal"));
  ASSERT_EQ_STATIC(color::fuchsia, enum_cast<color>("fuchsia"));
  ASSERT_EQ_STATIC(color::dodger_blue, enum_cast<color>("dodger_blue"));
  ASSERT_EQ_STATIC(color::medium_orchid, enum_cast<color>("medium_orchid"));
  ASSERT_EQ_STATIC(color::rebecca_purple, enum_cast<color>("rebecca_purple"));
  ASSERT_EQ_STATIC(color::tan, enum_cast<color>("tan"));
  ASSERT_EQ_STATIC(color::red, enum_cast<color>("red"));
  ASSERT_EQ_STATIC(color::indigo, enum_cast<color>("indigo"));
  ASSERT_EQ_STATIC(color::ivory, enum_cast<color>("ivory"));
  ASSERT_EQ_STATIC(color::dark_khaki, enum_cast<color>("dark_khaki"));
  ASSERT_EQ_STATIC(color::green_yellow, enum_cast<color>("green_yellow"));
  ASSERT_EQ_STATIC(color::brown, enum_cast<color>("brown"));
  ASSERT_EQ_STATIC(color::misty_rose, enum_cast<color>("misty_rose"));
  ASSERT_EQ_STATIC(color::pale_turquoise, enum_cast<color>("pale_turquoise"));
  ASSERT_EQ_STATIC(color::bisque, enum_cast<color>("bisque"));
  ASSERT_EQ_STATIC(color::snow, enum_cast<color>("snow"));
  ASSERT_EQ_STATIC(color::rosy_brown, enum_cast<color>("rosy_brown"));
  ASSERT_EQ_STATIC(color::dark_magenta, enum_cast<color>("dark_magenta"));
  ASSERT_EQ_STATIC(color::antique_white, enum_cast<color>("antique_white"));
  ASSERT_EQ_STATIC(color::light_salmon, enum_cast<color>("light_salmon"));
  ASSERT_EQ_STATIC(color::orchid, enum_cast<color>("orchid"));
  ASSERT_EQ_STATIC(color::silver, enum_cast<color>("silver"));
  ASSERT_EQ_STATIC(color::light_steel_blue, enum_cast<color>("light_steel_blue"));
  ASSERT_EQ_STATIC(color::linen, enum_cast<color>("linen"));
  ASSERT_EQ_STATIC(color::floral_white, enum_cast<color>("floral_white"));
  ASSERT_EQ_STATIC(color::white_smoke, enum_cast<color>("white_smoke"));
  ASSERT_EQ_STATIC(color::dark_salmon, enum_cast<color>("dark_salmon"));
  ASSERT_EQ_STATIC(color::orange, enum_cast<color>("orange"));
  ASSERT_EQ_STATIC(color::black, enum_cast<color>("black"));
  ASSERT_EQ_STATIC(color::sea_green, enum_cast<color>("sea_green"));
  ASSERT_EQ_STATIC(color::light_sky_blue, enum_cast<color>("light_sky_blue"));
  ASSERT_EQ_STATIC(color::dark_violet, enum_cast<color>("dark_violet"));
  ASSERT_EQ_STATIC(color::dark_slate_gray, enum_cast<color>("dark_slate_gray"));
  ASSERT_EQ_STATIC(color::aqua, enum_cast<color>("aqua"));
  ASSERT_EQ_STATIC(color::dark_blue, enum_cast<color>("dark_blue"));
  ASSERT_EQ_STATIC(color::blue, enum_cast<color>("blue"));
  ASSERT_EQ_STATIC(color::slate_blue, enum_cast<color>("slate_blue"));
  ASSERT_EQ_STATIC(color::dark_red, enum_cast<color>("dark_red"));
  ASSERT_EQ_STATIC(color::lavender, enum_cast<color>("lavender"));
  ASSERT_EQ_STATIC(color::plum, enum_cast<color>("plum"));
  ASSERT_EQ_STATIC(color::sea_shell, enum_cast<color>("sea_shell"));
  ASSERT_EQ_STATIC(color::yellow, enum_cast<color>("yellow"));
  ASSERT_EQ_STATIC(color::coral, enum_cast<color>("coral"));
  ASSERT_EQ_STATIC(color::azure, enum_cast<color>("azure"));
  ASSERT_EQ_STATIC(color::dark_orchid, enum_cast<color>("dark_orchid"));
  ASSERT_EQ_STATIC(color::lawn_green, enum_cast<color>("lawn_green"));
  ASSERT_EQ_STATIC(color::light_cyan, enum_cast<color>("light_cyan"));
  ASSERT_EQ_STATIC(color::lime_green, enum_cast<color>("lime_green"));
  ASSERT_EQ_STATIC(color::dark_slate_blue, enum_cast<color>("dark_slate_blue"));
  ASSERT_EQ_STATIC(color::beige, enum_cast<color>("beige"));
  ASSERT_EQ_STATIC(color::dark_cyan, enum_cast<color>("dark_cyan"));
  ASSERT_EQ_STATIC(color::golden_rod, enum_cast<color>("golden_rod"));

  ASSERT_EQ_STATIC(color::fuchsia, enum_cast<color>("magenta"));
  test_invalid_cases_common<color>();
}

TEST(EnumCastFromString, TerminalColor) {
  ASSERT_EQ_STATIC(terminal_color::blue, enum_cast<terminal_color>("blue"));
  ASSERT_EQ_STATIC(terminal_color::bright_yellow, enum_cast<terminal_color>("bright_yellow"));
  ASSERT_EQ_STATIC(terminal_color::green, enum_cast<terminal_color>("green"));
  ASSERT_EQ_STATIC(terminal_color::cyan, enum_cast<terminal_color>("cyan"));
  ASSERT_EQ_STATIC(terminal_color::bright_green, enum_cast<terminal_color>("bright_green"));
  ASSERT_EQ_STATIC(terminal_color::black, enum_cast<terminal_color>("black"));
  ASSERT_EQ_STATIC(terminal_color::bright_white, enum_cast<terminal_color>("bright_white"));
  ASSERT_EQ_STATIC(terminal_color::yellow, enum_cast<terminal_color>("yellow"));
  ASSERT_EQ_STATIC(terminal_color::bright_blue, enum_cast<terminal_color>("bright_blue"));
  ASSERT_EQ_STATIC(terminal_color::bright_red, enum_cast<terminal_color>("bright_red"));
  ASSERT_EQ_STATIC(terminal_color::red, enum_cast<terminal_color>("red"));
  ASSERT_EQ_STATIC(terminal_color::bright_magenta, enum_cast<terminal_color>("bright_magenta"));
  ASSERT_EQ_STATIC(terminal_color::bright_black, enum_cast<terminal_color>("bright_black"));
  ASSERT_EQ_STATIC(terminal_color::white, enum_cast<terminal_color>("white"));
  ASSERT_EQ_STATIC(terminal_color::magenta, enum_cast<terminal_color>("magenta"));

  test_invalid_cases_common<terminal_color>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast<terminal_color>("fuchsia"));
}

TEST(EnumCastFromString, HashCollision) {
  ASSERT_EQ_STATIC(hash_collision::_wSYZDRpiQJf8Rfv, enum_cast<hash_collision>("_wSYZDRpiQJf8Rfv"));
  ASSERT_EQ_STATIC(hash_collision::_cuFFJIHGp_jNJKS, enum_cast<hash_collision>("_cuFFJIHGp_jNJKS"));

  test_invalid_cases_common<hash_collision>();
}
