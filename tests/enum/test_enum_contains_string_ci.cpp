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

#include "tests/enum/test_cases_ci.hpp"
#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

template <class E>
constexpr bool enum_contains_ci(std::string_view str) {
  return rfl::enum_contains<E>(rfl::ascii_case_insensitive, str);
}

template <class E>
constexpr void test_invalid_cases_common() {
  EXPECT_FALSE_STATIC(enum_contains_ci<E>("ZER0"));
  EXPECT_FALSE_STATIC(enum_contains_ci<E>("hello_world"));
  EXPECT_FALSE_STATIC(enum_contains_ci<E>(" zero "));
  EXPECT_FALSE_STATIC(enum_contains_ci<E>("0"));
  EXPECT_FALSE_STATIC(enum_contains_ci<E>(""));
  EXPECT_FALSE_STATIC(enum_contains_ci<E>("-"));
}

template <class E>
constexpr void test_foo_signed_common() {
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("four"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("one"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("error"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("five"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("zero"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("seven"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("invalid"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("six"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("two"));

  EXPECT_TRUE_STATIC(enum_contains_ci<E>("FOUR"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("One"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("ERROR"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("Five"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("Zero"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("SeVeN"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("INVALiD"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("sIx"));
  EXPECT_TRUE_STATIC(enum_contains_ci<E>("TwO"));
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
  EXPECT_TRUE_STATIC(enum_contains_ci<foo_signed_rep>("yi"));
  EXPECT_TRUE_STATIC(enum_contains_ci<foo_signed_rep>("YI"));
  EXPECT_TRUE_STATIC(enum_contains_ci<foo_signed_rep>("er"));
  EXPECT_TRUE_STATIC(enum_contains_ci<foo_signed_rep>("ER"));
}

TEST(EnumContainsString, BarUnsigned) {
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("fourteen"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("twelve"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("ONE"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("thirtEEn"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("zErO"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("eRRoR"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("TEN"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("ELEven"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("thrEE"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("InvaLId"));
  EXPECT_TRUE_STATIC(enum_contains_ci<bar_unsigned>("TWO"));
  test_invalid_cases_common<bar_unsigned>();
}

TEST(EnumContainsString, BazSigned) {
  EXPECT_TRUE_STATIC(enum_contains_ci<baz_signed>("san"));
  EXPECT_TRUE_STATIC(enum_contains_ci<baz_signed>("ER"));
  EXPECT_TRUE_STATIC(enum_contains_ci<baz_signed>("YI"));
  EXPECT_TRUE_STATIC(enum_contains_ci<baz_signed>("LInG"));
  EXPECT_TRUE_STATIC(enum_contains_ci<baz_signed>("fuyI"));

  EXPECT_FALSE_STATIC(enum_contains_ci<baz_signed>("One"));
  EXPECT_FALSE_STATIC(enum_contains_ci<baz_signed>("sI"));
  test_invalid_cases_common<baz_signed>();
}

TEST(EnumContainsString, QuxUnsigned) {
  EXPECT_TRUE_STATIC(enum_contains_ci<qux_unsigned>("eR"));
  EXPECT_TRUE_STATIC(enum_contains_ci<qux_unsigned>("fuyi"));
  EXPECT_TRUE_STATIC(enum_contains_ci<qux_unsigned>("Yi"));
  EXPECT_TRUE_STATIC(enum_contains_ci<qux_unsigned>("San"));
  EXPECT_TRUE_STATIC(enum_contains_ci<qux_unsigned>("LinG"));
  EXPECT_TRUE_STATIC(enum_contains_ci<qux_unsigned>("si"));

  EXPECT_FALSE_STATIC(enum_contains_ci<qux_unsigned>("3"));
  EXPECT_FALSE_STATIC(enum_contains_ci<qux_unsigned>("zEro"));
  test_invalid_cases_common<qux_unsigned>();
}

TEST(EnumConstainsString, Empty) {
  test_invalid_cases_common<empty>();
  EXPECT_FALSE_STATIC(enum_contains_ci<empty>("zero"));
  EXPECT_FALSE_STATIC(enum_contains_ci<empty>("invalid"));
}

TEST(EnumConstainsString, Single) {
  EXPECT_TRUE_STATIC(enum_contains_ci<single>("value"));
  EXPECT_TRUE_STATIC(enum_contains_ci<single>("Value"));
  EXPECT_TRUE_STATIC(enum_contains_ci<single>("VALUE"));
  EXPECT_FALSE_STATIC(enum_contains_ci<single>("233"));
  test_invalid_cases_common<single>();
}

TEST(EnumConstainsString, SingleRep) {
  EXPECT_TRUE_STATIC(enum_contains_ci<single_rep>("Laugh"));
  EXPECT_TRUE_STATIC(enum_contains_ci<single_rep>("HaHaHa"));
  EXPECT_TRUE_STATIC(enum_contains_ci<single_rep>("Over"));
  EXPECT_TRUE_STATIC(enum_contains_ci<single_rep>("And_Over"));
  EXPECT_TRUE_STATIC(enum_contains_ci<single_rep>("Again"));
  EXPECT_TRUE_STATIC(enum_contains_ci<single_rep>("UnStoppable"));

  EXPECT_FALSE_STATIC(enum_contains_ci<single_rep>("value"));
  test_invalid_cases_common<single_rep>();
}

TEST(EnumConstainsString, Color) {
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("hot_pink"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("purple"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("blanched_almond"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("steel_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("alice_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("marOOn"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("fire_brick"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("yellow_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_gray"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("salMon"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_slate_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_turquoise"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_Sea_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_Sea_Green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("pale_grEEn"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dARk_orange"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_bLUE"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("oLIve"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("orange_red"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("mint_cream"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("tomato"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("lavender_blush"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("pale_violet_red"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_olive_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("moccasin"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("crimson"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("deep_sky_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("PINK"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("burly_wood"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("sieNNa"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_golden_rod"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("old_lace"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("pale_golden_rod"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("gainsboro"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("honey_dew"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_coral"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("peach_puFF"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("deep_pink"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_SEA_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("gray"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("sky_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("lime"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("cornsilk"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_golden_rod_yellow"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("blue_violet"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_slate_gray"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("saDDLe_brown"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("navy"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("aquamarine"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("olive_drab"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("sandy_broWN"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("powder_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_gray"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_purple"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("violet"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("midnight_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("chaRTreuse"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("spring_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("forest_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("cadet_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("diM_gray"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("royaL_bLue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_yellow"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("Lemon_Chiffon"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("chocolate"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("thistle"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("PERU"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("cornflower_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("Gold"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("Cyan"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_turquoise"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("turquoise"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("ghost_white"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("slate_gray"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("indian_red"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("whEAt"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("WHite"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_pink"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_spring_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_aquamarine"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("magenTa"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("khaki"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_violet_red"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("papaya_whip"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("navajo_white"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("teal"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("fuchsia"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dodger_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("medium_orchid"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("rebecca_purple"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("TAn"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("red"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("indigo"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("ivory"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_khaki"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("green_yellow"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("brown"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("misty_rose"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("pale_turquoise"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("bisque"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("snow"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("rosy_brown"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_magenta"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("antique_white"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_salmon"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("orchid"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("siLver"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_steel_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("linen"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("floral_white"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("white_smoke"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_salmon"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("orange"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("blAck"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("sea_GREEN"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_sky_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_violet"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_slate_gray"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("aqua"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("slate_bLUe"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_red"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("lavender"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("plum"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("sea_shell"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("yellow"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("Coral"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("azure"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_orchid"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("lawn_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("light_cyan"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("lime_green"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_slate_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("beIge"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("dark_cyan"));
  EXPECT_TRUE_STATIC(enum_contains_ci<color>("golden_rod"));
  test_invalid_cases_common<color>();
}

TEST(EnumConstainsString, TerminalColor) {
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("bright_yellow"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("grEEn"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("Cyan"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("bright_GREEN"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("black"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("bright_white"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("yeLLow"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("briGHT_blue"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("bright_RED"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("RED"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("bright_magenta"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("bright_black"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("whIte"));
  EXPECT_TRUE_STATIC(enum_contains_ci<terminal_color>("magenta"));

  EXPECT_FALSE_STATIC(enum_contains_ci<terminal_color>("fuchsIA"));
  test_invalid_cases_common<terminal_color>();
}

TEST(EnumConstainsString, HashCollision) {
  EXPECT_TRUE_STATIC(enum_contains_ci<hash_collision>("_wSYZDRpiqjf8rfv"));
  EXPECT_TRUE_STATIC(enum_contains_ci<hash_collision>("_cuffjihgp_jnjks"));
  test_invalid_cases_common<hash_collision>();
}
