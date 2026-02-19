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
constexpr void test_invalid_cases_common() {
  EXPECT_FALSE_STATIC(enum_contains<E>("ZERO"));
  EXPECT_FALSE_STATIC(enum_contains<E>("hello_world"));
  EXPECT_FALSE_STATIC(enum_contains<E>(" zero "));
  EXPECT_FALSE_STATIC(enum_contains<E>("0"));
  EXPECT_FALSE_STATIC(enum_contains<E>(""));
  EXPECT_FALSE_STATIC(enum_contains<E>("-"));
}

template <class E>
constexpr void test_foo_signed_common() {
  EXPECT_TRUE_STATIC(enum_contains<E>("four"));
  EXPECT_TRUE_STATIC(enum_contains<E>("one"));
  EXPECT_TRUE_STATIC(enum_contains<E>("error"));
  EXPECT_TRUE_STATIC(enum_contains<E>("five"));
  EXPECT_TRUE_STATIC(enum_contains<E>("zero"));
  EXPECT_TRUE_STATIC(enum_contains<E>("seven"));
  EXPECT_TRUE_STATIC(enum_contains<E>("invalid"));
  EXPECT_TRUE_STATIC(enum_contains<E>("six"));
  EXPECT_TRUE_STATIC(enum_contains<E>("two"));
}

TEST(EnumContainsString, FooSigned) {
  test_foo_signed_common<foo_signed>();
  test_invalid_cases_common<foo_signed>();
}

TEST(EnumContainsString, FooSignedReorder) {
  test_foo_signed_common<foo_signed_reorder>();
  test_invalid_cases_common<foo_signed_reorder>();
}

TEST(EnumContainsString, FooSignedRep) {
  test_foo_signed_common<foo_signed_rep>();
  test_invalid_cases_common<foo_signed_rep>();
  EXPECT_TRUE_STATIC(enum_contains<foo_signed_rep>("yi"));
  EXPECT_TRUE_STATIC(enum_contains<foo_signed_rep>("er"));
}

TEST(EnumContainsString, BarUnsigned) {
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("fourteen"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("twelve"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("one"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("thirteen"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("zero"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("error"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("ten"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("eleven"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("three"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("invalid"));
  EXPECT_TRUE_STATIC(enum_contains<bar_unsigned>("two"));
  test_invalid_cases_common<bar_unsigned>();
}

TEST(EnumContainsString, BazSigned) {
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>("san"));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>("er"));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>("yi"));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>("ling"));
  EXPECT_TRUE_STATIC(enum_contains<baz_signed>("fuyi"));

  EXPECT_FALSE_STATIC(enum_contains<baz_signed>("one"));
  EXPECT_FALSE_STATIC(enum_contains<baz_signed>("si"));
  test_invalid_cases_common<baz_signed>();
}

TEST(EnumContainsString, QuxUnsigned) {
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>("er"));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>("fuyi"));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>("yi"));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>("san"));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>("ling"));
  EXPECT_TRUE_STATIC(enum_contains<qux_unsigned>("si"));

  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>("3"));
  EXPECT_FALSE_STATIC(enum_contains<qux_unsigned>("zero"));
  test_invalid_cases_common<qux_unsigned>();
}

TEST(EnumConstainsString, Empty) {
  test_invalid_cases_common<empty>();
  EXPECT_FALSE_STATIC(enum_contains<empty>("zero"));
  EXPECT_FALSE_STATIC(enum_contains<empty>("invalid"));
}

TEST(EnumConstainsString, Single) {
  EXPECT_TRUE_STATIC(enum_contains<single>("value"));
  EXPECT_FALSE_STATIC(enum_contains<single>("233"));
  test_invalid_cases_common<single>();
}

TEST(EnumConstainsString, SingleRep) {
  EXPECT_TRUE_STATIC(enum_contains<single_rep>("laugh"));
  EXPECT_TRUE_STATIC(enum_contains<single_rep>("hahaha"));
  EXPECT_TRUE_STATIC(enum_contains<single_rep>("over"));
  EXPECT_TRUE_STATIC(enum_contains<single_rep>("and_over"));
  EXPECT_TRUE_STATIC(enum_contains<single_rep>("again"));
  EXPECT_TRUE_STATIC(enum_contains<single_rep>("unstoppable"));

  EXPECT_FALSE_STATIC(enum_contains<single_rep>("value"));
  test_invalid_cases_common<single_rep>();
}

TEST(EnumConstainsString, Color) {
  EXPECT_TRUE_STATIC(enum_contains<color>("hot_pink"));
  EXPECT_TRUE_STATIC(enum_contains<color>("purple"));
  EXPECT_TRUE_STATIC(enum_contains<color>("blanched_almond"));
  EXPECT_TRUE_STATIC(enum_contains<color>("steel_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("alice_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("maroon"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("fire_brick"));
  EXPECT_TRUE_STATIC(enum_contains<color>("yellow_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_gray"));
  EXPECT_TRUE_STATIC(enum_contains<color>("salmon"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_slate_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_turquoise"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_sea_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_sea_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("pale_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_orange"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("olive"));
  EXPECT_TRUE_STATIC(enum_contains<color>("orange_red"));
  EXPECT_TRUE_STATIC(enum_contains<color>("mint_cream"));
  EXPECT_TRUE_STATIC(enum_contains<color>("tomato"));
  EXPECT_TRUE_STATIC(enum_contains<color>("lavender_blush"));
  EXPECT_TRUE_STATIC(enum_contains<color>("pale_violet_red"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_olive_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("moccasin"));
  EXPECT_TRUE_STATIC(enum_contains<color>("crimson"));
  EXPECT_TRUE_STATIC(enum_contains<color>("deep_sky_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("pink"));
  EXPECT_TRUE_STATIC(enum_contains<color>("burly_wood"));
  EXPECT_TRUE_STATIC(enum_contains<color>("sienna"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_golden_rod"));
  EXPECT_TRUE_STATIC(enum_contains<color>("old_lace"));
  EXPECT_TRUE_STATIC(enum_contains<color>("pale_golden_rod"));
  EXPECT_TRUE_STATIC(enum_contains<color>("gainsboro"));
  EXPECT_TRUE_STATIC(enum_contains<color>("honey_dew"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_coral"));
  EXPECT_TRUE_STATIC(enum_contains<color>("peach_puff"));
  EXPECT_TRUE_STATIC(enum_contains<color>("deep_pink"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_sea_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("gray"));
  EXPECT_TRUE_STATIC(enum_contains<color>("sky_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("lime"));
  EXPECT_TRUE_STATIC(enum_contains<color>("cornsilk"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_golden_rod_yellow"));
  EXPECT_TRUE_STATIC(enum_contains<color>("blue_violet"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_slate_gray"));
  EXPECT_TRUE_STATIC(enum_contains<color>("saddle_brown"));
  EXPECT_TRUE_STATIC(enum_contains<color>("navy"));
  EXPECT_TRUE_STATIC(enum_contains<color>("aquamarine"));
  EXPECT_TRUE_STATIC(enum_contains<color>("olive_drab"));
  EXPECT_TRUE_STATIC(enum_contains<color>("sandy_brown"));
  EXPECT_TRUE_STATIC(enum_contains<color>("powder_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_gray"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_purple"));
  EXPECT_TRUE_STATIC(enum_contains<color>("violet"));
  EXPECT_TRUE_STATIC(enum_contains<color>("midnight_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("chartreuse"));
  EXPECT_TRUE_STATIC(enum_contains<color>("spring_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("forest_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("cadet_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dim_gray"));
  EXPECT_TRUE_STATIC(enum_contains<color>("royal_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_yellow"));
  EXPECT_TRUE_STATIC(enum_contains<color>("lemon_chiffon"));
  EXPECT_TRUE_STATIC(enum_contains<color>("chocolate"));
  EXPECT_TRUE_STATIC(enum_contains<color>("thistle"));
  EXPECT_TRUE_STATIC(enum_contains<color>("peru"));
  EXPECT_TRUE_STATIC(enum_contains<color>("cornflower_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("gold"));
  EXPECT_TRUE_STATIC(enum_contains<color>("cyan"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_turquoise"));
  EXPECT_TRUE_STATIC(enum_contains<color>("turquoise"));
  EXPECT_TRUE_STATIC(enum_contains<color>("ghost_white"));
  EXPECT_TRUE_STATIC(enum_contains<color>("slate_gray"));
  EXPECT_TRUE_STATIC(enum_contains<color>("indian_red"));
  EXPECT_TRUE_STATIC(enum_contains<color>("wheat"));
  EXPECT_TRUE_STATIC(enum_contains<color>("white"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_pink"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_spring_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_aquamarine"));
  EXPECT_TRUE_STATIC(enum_contains<color>("magenta"));
  EXPECT_TRUE_STATIC(enum_contains<color>("khaki"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_violet_red"));
  EXPECT_TRUE_STATIC(enum_contains<color>("papaya_whip"));
  EXPECT_TRUE_STATIC(enum_contains<color>("navajo_white"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("teal"));
  EXPECT_TRUE_STATIC(enum_contains<color>("fuchsia"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dodger_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("medium_orchid"));
  EXPECT_TRUE_STATIC(enum_contains<color>("rebecca_purple"));
  EXPECT_TRUE_STATIC(enum_contains<color>("tan"));
  EXPECT_TRUE_STATIC(enum_contains<color>("red"));
  EXPECT_TRUE_STATIC(enum_contains<color>("indigo"));
  EXPECT_TRUE_STATIC(enum_contains<color>("ivory"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_khaki"));
  EXPECT_TRUE_STATIC(enum_contains<color>("green_yellow"));
  EXPECT_TRUE_STATIC(enum_contains<color>("brown"));
  EXPECT_TRUE_STATIC(enum_contains<color>("misty_rose"));
  EXPECT_TRUE_STATIC(enum_contains<color>("pale_turquoise"));
  EXPECT_TRUE_STATIC(enum_contains<color>("bisque"));
  EXPECT_TRUE_STATIC(enum_contains<color>("snow"));
  EXPECT_TRUE_STATIC(enum_contains<color>("rosy_brown"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_magenta"));
  EXPECT_TRUE_STATIC(enum_contains<color>("antique_white"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_salmon"));
  EXPECT_TRUE_STATIC(enum_contains<color>("orchid"));
  EXPECT_TRUE_STATIC(enum_contains<color>("silver"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_steel_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("linen"));
  EXPECT_TRUE_STATIC(enum_contains<color>("floral_white"));
  EXPECT_TRUE_STATIC(enum_contains<color>("white_smoke"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_salmon"));
  EXPECT_TRUE_STATIC(enum_contains<color>("orange"));
  EXPECT_TRUE_STATIC(enum_contains<color>("black"));
  EXPECT_TRUE_STATIC(enum_contains<color>("sea_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_sky_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_violet"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_slate_gray"));
  EXPECT_TRUE_STATIC(enum_contains<color>("aqua"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("slate_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_red"));
  EXPECT_TRUE_STATIC(enum_contains<color>("lavender"));
  EXPECT_TRUE_STATIC(enum_contains<color>("plum"));
  EXPECT_TRUE_STATIC(enum_contains<color>("sea_shell"));
  EXPECT_TRUE_STATIC(enum_contains<color>("yellow"));
  EXPECT_TRUE_STATIC(enum_contains<color>("coral"));
  EXPECT_TRUE_STATIC(enum_contains<color>("azure"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_orchid"));
  EXPECT_TRUE_STATIC(enum_contains<color>("lawn_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("light_cyan"));
  EXPECT_TRUE_STATIC(enum_contains<color>("lime_green"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_slate_blue"));
  EXPECT_TRUE_STATIC(enum_contains<color>("beige"));
  EXPECT_TRUE_STATIC(enum_contains<color>("dark_cyan"));
  EXPECT_TRUE_STATIC(enum_contains<color>("golden_rod"));
  EXPECT_TRUE_STATIC(enum_contains<color>("magenta"));
  test_invalid_cases_common<color>();
}

TEST(EnumConstainsString, TerminalColor) {
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("blue"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("bright_yellow"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("green"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("cyan"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("bright_green"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("black"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("bright_white"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("yellow"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("bright_blue"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("bright_red"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("red"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("bright_magenta"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("bright_black"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("white"));
  EXPECT_TRUE_STATIC(enum_contains<terminal_color>("magenta"));

  EXPECT_FALSE_STATIC(enum_contains<terminal_color>("fuchsia"));
  test_invalid_cases_common<terminal_color>();
}

TEST(EnumConstainsString, HashCollision) {
  EXPECT_TRUE_STATIC(enum_contains<hash_collision>("_wSYZDRpiQJf8Rfv"));
  EXPECT_TRUE_STATIC(enum_contains<hash_collision>("_cuFFJIHGp_jNJKS"));
  test_invalid_cases_common<hash_collision>();
}
