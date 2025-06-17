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
#include <reflect_cpp26/enum/enum_name.hpp>
#endif

namespace rfl = reflect_cpp26;

// Note: this wrapper is required to convert result to std::string
// due to Google Test bug.
template <class E>
constexpr auto enum_name(E value) -> std::string {
  auto sv = reflect_cpp26::enum_name(value);
  return std::string{sv};
}

template <class E>
void test_foo_signed_common() {
  ASSERT_EQ_STATIC("six", enum_name(E::six));
  ASSERT_EQ_STATIC("zero", enum_name(E::zero));
  ASSERT_EQ_STATIC("seven", enum_name(E::seven));
  ASSERT_EQ_STATIC("four", enum_name(E::four));
  ASSERT_EQ_STATIC("error", enum_name(E::error));
  ASSERT_EQ_STATIC("five", enum_name(E::five));
  ASSERT_EQ_STATIC("invalid", enum_name(E::invalid));

  ASSERT_EQ_STATIC("", enum_name(static_cast<E>(-3)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<E>(3)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<E>(8)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<E>(1 << 31)));
}

TEST(EnumName, FooSigned) {
  test_foo_signed_common<foo_signed>();
  ASSERT_EQ_STATIC("one", enum_name(foo_signed::one));
  ASSERT_EQ_STATIC("two", enum_name(foo_signed::two));
}

TEST(EnumName, FooSignedReorder) {
  test_foo_signed_common<foo_signed_reorder>();
  ASSERT_EQ_STATIC("one", enum_name(foo_signed_reorder::one));
  ASSERT_EQ_STATIC("two", enum_name(foo_signed_reorder::two));
}

TEST(EnumName, FooSignedRep) {
  test_foo_signed_common<foo_signed_rep>();
  // 'one' and 'two' are defined before 'yi' and 'er'
  ASSERT_EQ_STATIC("one", enum_name(foo_signed_rep::yi));
  ASSERT_EQ_STATIC("two", enum_name(foo_signed_rep::er));
}

TEST(EnumName, BarUnsigned) {
  ASSERT_EQ_STATIC("three", enum_name(bar_unsigned::three));
  ASSERT_EQ_STATIC("twelve", enum_name(bar_unsigned::twelve));
  ASSERT_EQ_STATIC("fourteen", enum_name(bar_unsigned::fourteen));
  ASSERT_EQ_STATIC("ten", enum_name(bar_unsigned::ten));
  ASSERT_EQ_STATIC("thirteen", enum_name(bar_unsigned::thirteen));
  ASSERT_EQ_STATIC("eleven", enum_name(bar_unsigned::eleven));
  ASSERT_EQ_STATIC("one", enum_name(bar_unsigned::one));
  ASSERT_EQ_STATIC("two", enum_name(bar_unsigned::two));
  ASSERT_EQ_STATIC("invalid", enum_name(bar_unsigned::invalid));
  ASSERT_EQ_STATIC("zero", enum_name(bar_unsigned::zero));
  ASSERT_EQ_STATIC("error", enum_name(bar_unsigned::error));

  ASSERT_EQ_STATIC("", enum_name(static_cast<bar_unsigned>(-4)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<bar_unsigned>(4)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<bar_unsigned>(9)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<bar_unsigned>(1 << 31)));
}

TEST(EnumName, BazSigned) {
  ASSERT_EQ_STATIC("fuyi", enum_name(static_cast<baz_signed>(-1)));
  ASSERT_EQ_STATIC("ling", enum_name(baz_signed::ling));
  ASSERT_EQ_STATIC("fuyi", enum_name(baz_signed::fuyi));
  ASSERT_EQ_STATIC("san", enum_name(baz_signed::san));
  ASSERT_EQ_STATIC("er", enum_name(baz_signed::er));
  ASSERT_EQ_STATIC("yi", enum_name(baz_signed::yi));

  ASSERT_EQ_STATIC("", enum_name(static_cast<baz_signed>(-2)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<baz_signed>(4)));
}

TEST(EnumName, QuxUnsigned) {
  ASSERT_EQ_STATIC("san", enum_name(qux_unsigned::san));
  ASSERT_EQ_STATIC("fuyi", enum_name(qux_unsigned::fuyi));
  ASSERT_EQ_STATIC("fuyi", enum_name(static_cast<qux_unsigned>(-1)));
  ASSERT_EQ_STATIC("ling", enum_name(qux_unsigned::ling));
  ASSERT_EQ_STATIC("er", enum_name(qux_unsigned::er));
  ASSERT_EQ_STATIC("yi", enum_name(qux_unsigned::yi));
  ASSERT_EQ_STATIC("si", enum_name(qux_unsigned::si));

  ASSERT_EQ_STATIC("", enum_name(static_cast<qux_unsigned>(-2)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<qux_unsigned>(5)));
}

// Compilation error during static assertion since Clang 21:
// integer value -1 is outside the valid range of values [0, 1] for the
// enumeration type 'empty'
TEST(EnumName, Empty) {
  ASSERT_EQ("", enum_name(static_cast<empty>(-1)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<empty>(0)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<empty>(1)));
  ASSERT_EQ("", enum_name(static_cast<empty>(1 << 31)));
}

TEST(EnumName, Single) {
  ASSERT_EQ_STATIC("value", enum_name(single::value));
  ASSERT_EQ_STATIC("value", enum_name(static_cast<single>(233)));

  ASSERT_EQ_STATIC("", enum_name(static_cast<single>(0)));
  ASSERT_EQ("", enum_name(static_cast<single>(-1)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<single>(1)));
  ASSERT_EQ("", enum_name(static_cast<single>(1 << 31)));
}

TEST(EnumName, SingleRep) {
  // 'hahaha' is defined first
  ASSERT_EQ_STATIC("hahaha", enum_name(single_rep::again));
  ASSERT_EQ_STATIC("", enum_name(static_cast<single_rep>(0)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<single_rep>(-1)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<single_rep>(1)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<single_rep>(1 << 31)));
}

TEST(EnumName, TerminalColor) {
  ASSERT_EQ_STATIC("blue", enum_name(terminal_color::blue));
  ASSERT_EQ_STATIC("bright_white", enum_name(terminal_color::bright_white));
  ASSERT_EQ_STATIC("bright_blue", enum_name(terminal_color::bright_blue));
  ASSERT_EQ_STATIC("bright_yellow", enum_name(terminal_color::bright_yellow));
  ASSERT_EQ_STATIC("magenta", enum_name(terminal_color::magenta));
  ASSERT_EQ_STATIC("yellow", enum_name(terminal_color::yellow));
  ASSERT_EQ_STATIC("bright_green", enum_name(terminal_color::bright_green));
  ASSERT_EQ_STATIC("cyan", enum_name(terminal_color::cyan));
  ASSERT_EQ_STATIC("bright_black", enum_name(terminal_color::bright_black));
  ASSERT_EQ_STATIC("white", enum_name(terminal_color::white));
  ASSERT_EQ_STATIC("bright_cyan", enum_name(terminal_color::bright_cyan));
  ASSERT_EQ_STATIC("black", enum_name(terminal_color::black));
  ASSERT_EQ_STATIC("bright_magenta", enum_name(terminal_color::bright_magenta));
  ASSERT_EQ_STATIC("bright_red", enum_name(terminal_color::bright_red));
  ASSERT_EQ_STATIC("red", enum_name(terminal_color::red));
  ASSERT_EQ_STATIC("green", enum_name(terminal_color::green));

  ASSERT_EQ_STATIC("", enum_name(static_cast<terminal_color>(0)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<terminal_color>(18)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<terminal_color>(72)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<terminal_color>(108)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<terminal_color>(216)));
}

TEST(EnumName, Color) {
  ASSERT_EQ_STATIC("", enum_name(static_cast<color>(0x000001)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<color>(0x000100)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<color>(0x010000)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<color>(0xFFFFFE)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<color>(0xFFFFFFFF)));

  ASSERT_EQ_STATIC("light_pink", enum_name(color::light_pink));
  ASSERT_EQ_STATIC("peru", enum_name(color::peru));
  ASSERT_EQ_STATIC("bisque", enum_name(color::bisque));
  ASSERT_EQ_STATIC("violet", enum_name(color::violet));
  ASSERT_EQ_STATIC("lime_green", enum_name(color::lime_green));
  ASSERT_EQ_STATIC("olive", enum_name(color::olive));
  ASSERT_EQ_STATIC("pale_turquoise", enum_name(color::pale_turquoise));
  ASSERT_EQ_STATIC("azure", enum_name(color::azure));
  ASSERT_EQ_STATIC("antique_white", enum_name(color::antique_white));
  ASSERT_EQ_STATIC("burly_wood", enum_name(color::burly_wood));
  ASSERT_EQ_STATIC("red", enum_name(color::red));
  ASSERT_EQ_STATIC("tan", enum_name(color::tan));
  ASSERT_EQ_STATIC("hot_pink", enum_name(color::hot_pink));
  ASSERT_EQ_STATIC("alice_blue", enum_name(color::alice_blue));
  ASSERT_EQ_STATIC("medium_turquoise", enum_name(color::medium_turquoise));
  ASSERT_EQ_STATIC("light_steel_blue", enum_name(color::light_steel_blue));
  ASSERT_EQ_STATIC("light_green", enum_name(color::light_green));
  ASSERT_EQ_STATIC("light_yellow", enum_name(color::light_yellow));
  ASSERT_EQ_STATIC("yellow_green", enum_name(color::yellow_green));
  ASSERT_EQ_STATIC("light_sea_green", enum_name(color::light_sea_green));
  ASSERT_EQ_STATIC("white_smoke", enum_name(color::white_smoke));
  ASSERT_EQ_STATIC("blanched_almond", enum_name(color::blanched_almond));
  ASSERT_EQ_STATIC("peach_puff", enum_name(color::peach_puff));
  ASSERT_EQ_STATIC("navy", enum_name(color::navy));
  ASSERT_EQ_STATIC("black", enum_name(color::black));
  ASSERT_EQ_STATIC("lavender", enum_name(color::lavender));
  ASSERT_EQ_STATIC("sienna", enum_name(color::sienna));
  ASSERT_EQ_STATIC("sky_blue", enum_name(color::sky_blue));
  ASSERT_THAT(enum_name(color::fuchsia), testing::AnyOf("magenta", "fuchsia"));
  ASSERT_EQ_STATIC("medium_orchid", enum_name(color::medium_orchid));
  ASSERT_EQ_STATIC("mint_cream", enum_name(color::mint_cream));
  ASSERT_EQ_STATIC("light_sky_blue", enum_name(color::light_sky_blue));
  ASSERT_EQ_STATIC("lemon_chiffon", enum_name(color::lemon_chiffon));
  ASSERT_EQ_STATIC("moccasin", enum_name(color::moccasin));
  ASSERT_EQ_STATIC("dark_sea_green", enum_name(color::dark_sea_green));
  ASSERT_EQ_STATIC("honey_dew", enum_name(color::honey_dew));
  ASSERT_EQ_STATIC("chocolate", enum_name(color::chocolate));
  ASSERT_EQ_STATIC("misty_rose", enum_name(color::misty_rose));
  ASSERT_EQ_STATIC("papaya_whip", enum_name(color::papaya_whip));
  ASSERT_EQ_STATIC("plum", enum_name(color::plum));
  ASSERT_EQ_STATIC("dark_blue", enum_name(color::dark_blue));
  ASSERT_EQ_STATIC("slate_gray", enum_name(color::slate_gray));
  ASSERT_EQ_STATIC("dark_salmon", enum_name(color::dark_salmon));
  ASSERT_EQ_STATIC("aquamarine", enum_name(color::aquamarine));
  ASSERT_EQ_STATIC("medium_purple", enum_name(color::medium_purple));
  ASSERT_EQ_STATIC("orange", enum_name(color::orange));
  ASSERT_EQ_STATIC("powder_blue", enum_name(color::powder_blue));
  ASSERT_EQ_STATIC("dark_turquoise", enum_name(color::dark_turquoise));
  ASSERT_EQ_STATIC("light_slate_gray", enum_name(color::light_slate_gray));
  ASSERT_EQ_STATIC("dark_khaki", enum_name(color::dark_khaki));
  ASSERT_EQ_STATIC("beige", enum_name(color::beige));
  ASSERT_EQ_STATIC("medium_blue", enum_name(color::medium_blue));
  ASSERT_EQ_STATIC("light_gray", enum_name(color::light_gray));
  ASSERT_EQ_STATIC("dark_slate_blue", enum_name(color::dark_slate_blue));
  ASSERT_EQ_STATIC("teal", enum_name(color::teal));
  ASSERT_EQ_STATIC("ivory", enum_name(color::ivory));
  ASSERT_THAT(enum_name(color::aqua), testing::AnyOf("aqua", "cyan"));
  ASSERT_EQ_STATIC("dark_cyan", enum_name(color::dark_cyan));
  ASSERT_EQ_STATIC("medium_sea_green", enum_name(color::medium_sea_green));
  ASSERT_EQ_STATIC("dark_gray", enum_name(color::dark_gray));
  ASSERT_EQ_STATIC("linen", enum_name(color::linen));
  ASSERT_EQ_STATIC("orange_red", enum_name(color::orange_red));
  ASSERT_EQ_STATIC("light_blue", enum_name(color::light_blue));
  ASSERT_EQ_STATIC("midnight_blue", enum_name(color::midnight_blue));
  ASSERT_EQ_STATIC("sea_green", enum_name(color::sea_green));
  ASSERT_EQ_STATIC("maroon", enum_name(color::maroon));
  ASSERT_EQ_STATIC("golden_rod", enum_name(color::golden_rod));
  ASSERT_EQ_STATIC("purple", enum_name(color::purple));
  ASSERT_EQ_STATIC("spring_green", enum_name(color::spring_green));
  ASSERT_EQ_STATIC("dark_violet", enum_name(color::dark_violet));
  ASSERT_EQ_STATIC("dim_gray", enum_name(color::dim_gray));
  ASSERT_EQ_STATIC("turquoise", enum_name(color::turquoise));
  ASSERT_EQ_STATIC("lime", enum_name(color::lime));
  ASSERT_EQ_STATIC("blue", enum_name(color::blue));
  ASSERT_EQ_STATIC("green_yellow", enum_name(color::green_yellow));
  ASSERT_EQ_STATIC("olive_drab", enum_name(color::olive_drab));
  ASSERT_EQ_STATIC("light_cyan", enum_name(color::light_cyan));
  ASSERT_EQ_STATIC("silver", enum_name(color::silver));
  ASSERT_EQ_STATIC("medium_violet_red", enum_name(color::medium_violet_red));
  ASSERT_EQ_STATIC("lawn_green", enum_name(color::lawn_green));
  ASSERT_EQ_STATIC("deep_sky_blue", enum_name(color::deep_sky_blue));
  ASSERT_EQ_STATIC("dodger_blue", enum_name(color::dodger_blue));
  ASSERT_EQ_STATIC("yellow", enum_name(color::yellow));
  ASSERT_THAT(enum_name(color::cyan), testing::AnyOf("aqua", "cyan"));
  ASSERT_EQ_STATIC("steel_blue", enum_name(color::steel_blue));
  ASSERT_EQ_STATIC("light_coral", enum_name(color::light_coral));
  ASSERT_EQ_STATIC("medium_slate_blue", enum_name(color::medium_slate_blue));
  ASSERT_EQ_STATIC("forest_green", enum_name(color::forest_green));
  ASSERT_EQ_STATIC("fire_brick", enum_name(color::fire_brick));
  ASSERT_EQ_STATIC("dark_slate_gray", enum_name(color::dark_slate_gray));
  ASSERT_EQ_STATIC("dark_magenta", enum_name(color::dark_magenta));
  ASSERT_EQ_STATIC("crimson", enum_name(color::crimson));
  ASSERT_EQ_STATIC("dark_red", enum_name(color::dark_red));
  ASSERT_EQ_STATIC("khaki", enum_name(color::khaki));
  ASSERT_EQ_STATIC("old_lace", enum_name(color::old_lace));
  ASSERT_EQ_STATIC("gainsboro", enum_name(color::gainsboro));
  ASSERT_EQ_STATIC("royal_blue", enum_name(color::royal_blue));
  ASSERT_EQ_STATIC("rosy_brown", enum_name(color::rosy_brown));
  ASSERT_EQ_STATIC("gray", enum_name(color::gray));
  ASSERT_EQ_STATIC("medium_spring_green", enum_name(color::medium_spring_green));
  ASSERT_EQ_STATIC("indigo", enum_name(color::indigo));
  ASSERT_EQ_STATIC("floral_white", enum_name(color::floral_white));
  ASSERT_EQ_STATIC("gold", enum_name(color::gold));
  ASSERT_EQ_STATIC("coral", enum_name(color::coral));
  ASSERT_EQ_STATIC("wheat", enum_name(color::wheat));
  ASSERT_EQ_STATIC("dark_orange", enum_name(color::dark_orange));
  ASSERT_EQ_STATIC("cadet_blue", enum_name(color::cadet_blue));
  ASSERT_EQ_STATIC("light_salmon", enum_name(color::light_salmon));
  ASSERT_EQ_STATIC("slate_blue", enum_name(color::slate_blue));
  ASSERT_EQ_STATIC("pink", enum_name(color::pink));
  ASSERT_EQ_STATIC("orchid", enum_name(color::orchid));
  ASSERT_EQ_STATIC("dark_olive_green", enum_name(color::dark_olive_green));
  ASSERT_EQ_STATIC("pale_violet_red", enum_name(color::pale_violet_red));
  ASSERT_EQ_STATIC("deep_pink", enum_name(color::deep_pink));
  ASSERT_EQ_STATIC("cornflower_blue", enum_name(color::cornflower_blue));
  ASSERT_EQ_STATIC("indian_red", enum_name(color::indian_red));
  ASSERT_EQ_STATIC("cornsilk", enum_name(color::cornsilk));
  ASSERT_EQ_STATIC("chartreuse", enum_name(color::chartreuse));
  ASSERT_EQ_STATIC("medium_aquamarine", enum_name(color::medium_aquamarine));
  ASSERT_EQ_STATIC("thistle", enum_name(color::thistle));
  ASSERT_EQ_STATIC("tomato", enum_name(color::tomato));
  ASSERT_EQ_STATIC("salmon", enum_name(color::salmon));
  ASSERT_THAT(enum_name(color::magenta), testing::AnyOf("magenta", "fuchsia"));
  ASSERT_EQ_STATIC("sea_shell", enum_name(color::sea_shell));
  ASSERT_EQ_STATIC("sandy_brown", enum_name(color::sandy_brown));
  ASSERT_EQ_STATIC("pale_golden_rod", enum_name(color::pale_golden_rod));
  ASSERT_EQ_STATIC("saddle_brown", enum_name(color::saddle_brown));
  ASSERT_EQ_STATIC("rebecca_purple", enum_name(color::rebecca_purple));
  ASSERT_EQ_STATIC("lavender_blush", enum_name(color::lavender_blush));
  ASSERT_EQ_STATIC("light_golden_rod_yellow", enum_name(color::light_golden_rod_yellow));
  ASSERT_EQ_STATIC("white", enum_name(color::white));
  ASSERT_EQ_STATIC("dark_golden_rod", enum_name(color::dark_golden_rod));
  ASSERT_EQ_STATIC("dark_green", enum_name(color::dark_green));
  ASSERT_EQ_STATIC("green", enum_name(color::green));
  ASSERT_EQ_STATIC("pale_green", enum_name(color::pale_green));
  ASSERT_EQ_STATIC("brown", enum_name(color::brown));
  ASSERT_EQ_STATIC("navajo_white", enum_name(color::navajo_white));
  ASSERT_EQ_STATIC("snow", enum_name(color::snow));
  ASSERT_EQ_STATIC("ghost_white", enum_name(color::ghost_white));
  ASSERT_EQ_STATIC("blue_violet", enum_name(color::blue_violet));
  ASSERT_EQ_STATIC("dark_orchid", enum_name(color::dark_orchid));
}

TEST(EnumName, HashCollision) {
  ASSERT_EQ_STATIC("_cuFFJIHGp_jNJKS", enum_name(hash_collision::_cuFFJIHGp_jNJKS));
  ASSERT_EQ_STATIC("_wSYZDRpiQJf8Rfv", enum_name(hash_collision::_wSYZDRpiQJf8Rfv));

  ASSERT_EQ_STATIC("", enum_name(static_cast<hash_collision>(-1)));
  ASSERT_EQ_STATIC("", enum_name(static_cast<hash_collision>(1 << 31)));
}
