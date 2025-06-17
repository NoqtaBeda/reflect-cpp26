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

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_contains.hpp>
#endif

using namespace reflect_cpp26;

template <class E>
constexpr bool test_invalid_cases_common()
{
  static_assert(NOT enum_contains<E>("ZERO"));
  static_assert(NOT enum_contains<E>("hello_world"));
  static_assert(NOT enum_contains<E>(" zero "));
  static_assert(NOT enum_contains<E>("0"));
  static_assert(NOT enum_contains<E>(""));
  static_assert(NOT enum_contains<E>("-"));
  return true;
}

template <class E>
constexpr bool test_foo_signed_common()
{
  static_assert(enum_contains<E>("four"));
  static_assert(enum_contains<E>("one"));
  static_assert(enum_contains<E>("error"));
  static_assert(enum_contains<E>("five"));
  static_assert(enum_contains<E>("zero"));
  static_assert(enum_contains<E>("seven"));
  static_assert(enum_contains<E>("invalid"));
  static_assert(enum_contains<E>("six"));
  static_assert(enum_contains<E>("two"));
  return test_invalid_cases_common<E>();
}

static_assert(test_foo_signed_common<foo_signed>());
static_assert(test_foo_signed_common<foo_signed_reorder>());
static_assert(test_foo_signed_common<foo_signed_rep>());
static_assert(enum_contains<foo_signed_rep>("yi"));
static_assert(enum_contains<foo_signed_rep>("er"));

static_assert(enum_contains<bar_unsigned>("fourteen"));
static_assert(enum_contains<bar_unsigned>("twelve"));
static_assert(enum_contains<bar_unsigned>("one"));
static_assert(enum_contains<bar_unsigned>("thirteen"));
static_assert(enum_contains<bar_unsigned>("zero"));
static_assert(enum_contains<bar_unsigned>("error"));
static_assert(enum_contains<bar_unsigned>("ten"));
static_assert(enum_contains<bar_unsigned>("eleven"));
static_assert(enum_contains<bar_unsigned>("three"));
static_assert(enum_contains<bar_unsigned>("invalid"));
static_assert(enum_contains<bar_unsigned>("two"));

static_assert(enum_contains<baz_signed>("san"));
static_assert(enum_contains<baz_signed>("er"));
static_assert(enum_contains<baz_signed>("yi"));
static_assert(enum_contains<baz_signed>("ling"));
static_assert(enum_contains<baz_signed>("fuyi"));

static_assert(test_invalid_cases_common<baz_signed>());
static_assert(NOT enum_contains<baz_signed>("one"));
static_assert(NOT enum_contains<baz_signed>("si"));

static_assert(enum_contains<qux_unsigned>("er"));
static_assert(enum_contains<qux_unsigned>("fuyi"));
static_assert(enum_contains<qux_unsigned>("yi"));
static_assert(enum_contains<qux_unsigned>("san"));
static_assert(enum_contains<qux_unsigned>("ling"));
static_assert(enum_contains<qux_unsigned>("si"));

static_assert(test_invalid_cases_common<qux_unsigned>());
static_assert(NOT enum_contains<qux_unsigned>("3"));
static_assert(NOT enum_contains<qux_unsigned>("zero"));

static_assert(test_invalid_cases_common<empty>());
static_assert(NOT enum_contains<empty>("zero"));
static_assert(NOT enum_contains<empty>("invalid"));

static_assert(enum_contains<single>("value"));
static_assert(test_invalid_cases_common<single>());
static_assert(NOT enum_contains<single>("233"));

static_assert(enum_contains<single_rep>("laugh"));
static_assert(enum_contains<single_rep>("hahaha"));
static_assert(enum_contains<single_rep>("over"));
static_assert(enum_contains<single_rep>("and_over"));
static_assert(enum_contains<single_rep>("again"));
static_assert(enum_contains<single_rep>("unstoppable"));

static_assert(test_invalid_cases_common<single_rep>());
static_assert(NOT enum_contains<single_rep>("value"));

static_assert(enum_contains<color>("hot_pink"));
static_assert(enum_contains<color>("purple"));
static_assert(enum_contains<color>("blanched_almond"));
static_assert(enum_contains<color>("steel_blue"));
static_assert(enum_contains<color>("alice_blue"));
static_assert(enum_contains<color>("maroon"));
static_assert(enum_contains<color>("medium_blue"));
static_assert(enum_contains<color>("fire_brick"));
static_assert(enum_contains<color>("yellow_green"));
static_assert(enum_contains<color>("light_gray"));
static_assert(enum_contains<color>("salmon"));
static_assert(enum_contains<color>("medium_slate_blue"));
static_assert(enum_contains<color>("dark_turquoise"));
static_assert(enum_contains<color>("medium_sea_green"));
static_assert(enum_contains<color>("dark_sea_green"));
static_assert(enum_contains<color>("pale_green"));
static_assert(enum_contains<color>("dark_orange"));
static_assert(enum_contains<color>("light_blue"));
static_assert(enum_contains<color>("olive"));
static_assert(enum_contains<color>("orange_red"));
static_assert(enum_contains<color>("mint_cream"));
static_assert(enum_contains<color>("tomato"));
static_assert(enum_contains<color>("lavender_blush"));
static_assert(enum_contains<color>("pale_violet_red"));
static_assert(enum_contains<color>("dark_olive_green"));
static_assert(enum_contains<color>("moccasin"));
static_assert(enum_contains<color>("crimson"));
static_assert(enum_contains<color>("deep_sky_blue"));
static_assert(enum_contains<color>("pink"));
static_assert(enum_contains<color>("burly_wood"));
static_assert(enum_contains<color>("sienna"));
static_assert(enum_contains<color>("dark_golden_rod"));
static_assert(enum_contains<color>("old_lace"));
static_assert(enum_contains<color>("pale_golden_rod"));
static_assert(enum_contains<color>("gainsboro"));
static_assert(enum_contains<color>("honey_dew"));
static_assert(enum_contains<color>("dark_green"));
static_assert(enum_contains<color>("light_coral"));
static_assert(enum_contains<color>("peach_puff"));
static_assert(enum_contains<color>("deep_pink"));
static_assert(enum_contains<color>("light_sea_green"));
static_assert(enum_contains<color>("gray"));
static_assert(enum_contains<color>("sky_blue"));
static_assert(enum_contains<color>("lime"));
static_assert(enum_contains<color>("cornsilk"));
static_assert(enum_contains<color>("light_golden_rod_yellow"));
static_assert(enum_contains<color>("blue_violet"));
static_assert(enum_contains<color>("light_slate_gray"));
static_assert(enum_contains<color>("saddle_brown"));
static_assert(enum_contains<color>("navy"));
static_assert(enum_contains<color>("aquamarine"));
static_assert(enum_contains<color>("olive_drab"));
static_assert(enum_contains<color>("sandy_brown"));
static_assert(enum_contains<color>("powder_blue"));
static_assert(enum_contains<color>("dark_gray"));
static_assert(enum_contains<color>("medium_purple"));
static_assert(enum_contains<color>("violet"));
static_assert(enum_contains<color>("midnight_blue"));
static_assert(enum_contains<color>("chartreuse"));
static_assert(enum_contains<color>("spring_green"));
static_assert(enum_contains<color>("forest_green"));
static_assert(enum_contains<color>("cadet_blue"));
static_assert(enum_contains<color>("dim_gray"));
static_assert(enum_contains<color>("royal_blue"));
static_assert(enum_contains<color>("light_yellow"));
static_assert(enum_contains<color>("lemon_chiffon"));
static_assert(enum_contains<color>("chocolate"));
static_assert(enum_contains<color>("thistle"));
static_assert(enum_contains<color>("peru"));
static_assert(enum_contains<color>("cornflower_blue"));
static_assert(enum_contains<color>("gold"));
static_assert(enum_contains<color>("cyan"));
static_assert(enum_contains<color>("medium_turquoise"));
static_assert(enum_contains<color>("turquoise"));
static_assert(enum_contains<color>("ghost_white"));
static_assert(enum_contains<color>("slate_gray"));
static_assert(enum_contains<color>("indian_red"));
static_assert(enum_contains<color>("wheat"));
static_assert(enum_contains<color>("white"));
static_assert(enum_contains<color>("light_pink"));
static_assert(enum_contains<color>("medium_spring_green"));
static_assert(enum_contains<color>("medium_aquamarine"));
static_assert(enum_contains<color>("magenta"));
static_assert(enum_contains<color>("khaki"));
static_assert(enum_contains<color>("medium_violet_red"));
static_assert(enum_contains<color>("papaya_whip"));
static_assert(enum_contains<color>("navajo_white"));
static_assert(enum_contains<color>("light_green"));
static_assert(enum_contains<color>("green"));
static_assert(enum_contains<color>("teal"));
static_assert(enum_contains<color>("fuchsia"));
static_assert(enum_contains<color>("dodger_blue"));
static_assert(enum_contains<color>("medium_orchid"));
static_assert(enum_contains<color>("rebecca_purple"));
static_assert(enum_contains<color>("tan"));
static_assert(enum_contains<color>("red"));
static_assert(enum_contains<color>("indigo"));
static_assert(enum_contains<color>("ivory"));
static_assert(enum_contains<color>("dark_khaki"));
static_assert(enum_contains<color>("green_yellow"));
static_assert(enum_contains<color>("brown"));
static_assert(enum_contains<color>("misty_rose"));
static_assert(enum_contains<color>("pale_turquoise"));
static_assert(enum_contains<color>("bisque"));
static_assert(enum_contains<color>("snow"));
static_assert(enum_contains<color>("rosy_brown"));
static_assert(enum_contains<color>("dark_magenta"));
static_assert(enum_contains<color>("antique_white"));
static_assert(enum_contains<color>("light_salmon"));
static_assert(enum_contains<color>("orchid"));
static_assert(enum_contains<color>("silver"));
static_assert(enum_contains<color>("light_steel_blue"));
static_assert(enum_contains<color>("linen"));
static_assert(enum_contains<color>("floral_white"));
static_assert(enum_contains<color>("white_smoke"));
static_assert(enum_contains<color>("dark_salmon"));
static_assert(enum_contains<color>("orange"));
static_assert(enum_contains<color>("black"));
static_assert(enum_contains<color>("sea_green"));
static_assert(enum_contains<color>("light_sky_blue"));
static_assert(enum_contains<color>("dark_violet"));
static_assert(enum_contains<color>("dark_slate_gray"));
static_assert(enum_contains<color>("aqua"));
static_assert(enum_contains<color>("dark_blue"));
static_assert(enum_contains<color>("blue"));
static_assert(enum_contains<color>("slate_blue"));
static_assert(enum_contains<color>("dark_red"));
static_assert(enum_contains<color>("lavender"));
static_assert(enum_contains<color>("plum"));
static_assert(enum_contains<color>("sea_shell"));
static_assert(enum_contains<color>("yellow"));
static_assert(enum_contains<color>("coral"));
static_assert(enum_contains<color>("azure"));
static_assert(enum_contains<color>("dark_orchid"));
static_assert(enum_contains<color>("lawn_green"));
static_assert(enum_contains<color>("light_cyan"));
static_assert(enum_contains<color>("lime_green"));
static_assert(enum_contains<color>("dark_slate_blue"));
static_assert(enum_contains<color>("beige"));
static_assert(enum_contains<color>("dark_cyan"));
static_assert(enum_contains<color>("golden_rod"));
static_assert(enum_contains<color>("magenta"));
static_assert(test_invalid_cases_common<color>());

static_assert(enum_contains<terminal_color>("blue"));
static_assert(enum_contains<terminal_color>("bright_yellow"));
static_assert(enum_contains<terminal_color>("green"));
static_assert(enum_contains<terminal_color>("cyan"));
static_assert(enum_contains<terminal_color>("bright_green"));
static_assert(enum_contains<terminal_color>("black"));
static_assert(enum_contains<terminal_color>("bright_white"));
static_assert(enum_contains<terminal_color>("yellow"));
static_assert(enum_contains<terminal_color>("bright_blue"));
static_assert(enum_contains<terminal_color>("bright_red"));
static_assert(enum_contains<terminal_color>("red"));
static_assert(enum_contains<terminal_color>("bright_magenta"));
static_assert(enum_contains<terminal_color>("bright_black"));
static_assert(enum_contains<terminal_color>("white"));
static_assert(enum_contains<terminal_color>("magenta"));

static_assert(test_invalid_cases_common<terminal_color>());
static_assert(NOT enum_contains<terminal_color>("fuchsia"));

static_assert(enum_contains<hash_collision>("_wSYZDRpiQJf8Rfv"));
static_assert(enum_contains<hash_collision>("_cuFFJIHGp_jNJKS"));
static_assert(test_invalid_cases_common<hash_collision>());

TEST(EnumContainsString, StaticAll) {
  EXPECT_TRUE(true); // All test cases done by static assertions above.
}
