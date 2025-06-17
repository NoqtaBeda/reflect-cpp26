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

#include "tests/enum/test_cases_ci.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_cast.hpp>
#endif

namespace rfl = reflect_cpp26;

template <class E>
constexpr auto enum_cast_ci(std::string_view str) {
  return rfl::enum_cast<E>(rfl::ascii_case_insensitive, str);
}

template <class E>
void test_invalid_cases_common() {
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<E>("ZER0"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<E>("hello_world"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<E>(" zero "));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<E>("0"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<E>(""));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<E>("-"));
}

TEST(EnumCastFromStringCI, FooSigned) {
  ASSERT_EQ_STATIC(foo_signed::four, enum_cast_ci<foo_signed>("four"));
  ASSERT_EQ_STATIC(foo_signed::one, enum_cast_ci<foo_signed>("One"));
  ASSERT_EQ_STATIC(foo_signed::error, enum_cast_ci<foo_signed>("ERROR"));
  ASSERT_EQ_STATIC(foo_signed::five, enum_cast_ci<foo_signed>("Five"));
  ASSERT_EQ_STATIC(foo_signed::zero, enum_cast_ci<foo_signed>("ZerO"));
  ASSERT_EQ_STATIC(foo_signed::seven, enum_cast_ci<foo_signed>("SeVeN"));
  ASSERT_EQ_STATIC(foo_signed::invalid, enum_cast_ci<foo_signed>("INVALID"));
  ASSERT_EQ_STATIC(foo_signed::six, enum_cast_ci<foo_signed>("sIx"));
  ASSERT_EQ_STATIC(foo_signed::two, enum_cast_ci<foo_signed>("tWo"));

  test_invalid_cases_common<foo_signed>();
}

TEST(EnumCastFromStringCI, FooSignedReorder) {
  ASSERT_EQ_STATIC(foo_signed_reorder::Four, enum_cast_ci<foo_signed_reorder>("four"));
  ASSERT_EQ_STATIC(foo_signed_reorder::One, enum_cast_ci<foo_signed_reorder>("One"));
  ASSERT_EQ_STATIC(foo_signed_reorder::Error, enum_cast_ci<foo_signed_reorder>("ERROR"));
  ASSERT_EQ_STATIC(foo_signed_reorder::Five, enum_cast_ci<foo_signed_reorder>("Five"));
  ASSERT_EQ_STATIC(foo_signed_reorder::Zero, enum_cast_ci<foo_signed_reorder>("ZerO"));
  ASSERT_EQ_STATIC(foo_signed_reorder::Seven, enum_cast_ci<foo_signed_reorder>("SeVeN"));
  ASSERT_EQ_STATIC(foo_signed_reorder::Invalid, enum_cast_ci<foo_signed_reorder>("INVALID"));
  ASSERT_EQ_STATIC(foo_signed_reorder::Six, enum_cast_ci<foo_signed_reorder>("sIx"));
  ASSERT_EQ_STATIC(foo_signed_reorder::Two, enum_cast_ci<foo_signed_reorder>("tWo"));

  test_invalid_cases_common<foo_signed_reorder>();
}

TEST(EnumCastFromStringCI, FooSignedRep) {
  ASSERT_EQ_STATIC(foo_signed_rep::FOUR, enum_cast_ci<foo_signed_rep>("four"));
  ASSERT_EQ_STATIC(foo_signed_rep::ONE, enum_cast_ci<foo_signed_rep>("One"));
  ASSERT_EQ_STATIC(foo_signed_rep::ERROR, enum_cast_ci<foo_signed_rep>("ERROR"));
  ASSERT_EQ_STATIC(foo_signed_rep::FIVE, enum_cast_ci<foo_signed_rep>("Five"));
  ASSERT_EQ_STATIC(foo_signed_rep::ZERO, enum_cast_ci<foo_signed_rep>("ZerO"));
  ASSERT_EQ_STATIC(foo_signed_rep::SEVEN, enum_cast_ci<foo_signed_rep>("SeVeN"));
  ASSERT_EQ_STATIC(foo_signed_rep::INVALID, enum_cast_ci<foo_signed_rep>("INVALID"));
  ASSERT_EQ_STATIC(foo_signed_rep::SIX, enum_cast_ci<foo_signed_rep>("sIx"));
  ASSERT_EQ_STATIC(foo_signed_rep::TWO, enum_cast_ci<foo_signed_rep>("tWo"));

  ASSERT_EQ_STATIC(foo_signed_rep::ONE, enum_cast_ci<foo_signed_rep>("yi"));
  ASSERT_EQ_STATIC(foo_signed_rep::TWO, enum_cast_ci<foo_signed_rep>("er"));
  ASSERT_EQ_STATIC(foo_signed_rep::ONE, enum_cast_ci<foo_signed_rep>("Yi"));
  ASSERT_EQ_STATIC(foo_signed_rep::TWO, enum_cast_ci<foo_signed_rep>("ER"));

  test_invalid_cases_common<foo_signed_rep>();
}

TEST(EnumCastFromStringCI, BarUnsigned) {
  ASSERT_EQ_STATIC(bar_unsigned::FoUrTeEn, enum_cast_ci<bar_unsigned>("fourteen"));
  ASSERT_EQ_STATIC(bar_unsigned::TwElVe, enum_cast_ci<bar_unsigned>("Twelve"));
  ASSERT_EQ_STATIC(bar_unsigned::OnE, enum_cast_ci<bar_unsigned>("ONE"));
  ASSERT_EQ_STATIC(bar_unsigned::ThIrTeEn, enum_cast_ci<bar_unsigned>("ThIRTeeN"));
  ASSERT_EQ_STATIC(bar_unsigned::ZeRo, enum_cast_ci<bar_unsigned>("zERo"));
  ASSERT_EQ_STATIC(bar_unsigned::ErRoR, enum_cast_ci<bar_unsigned>("ERROR"));
  ASSERT_EQ_STATIC(bar_unsigned::TeN, enum_cast_ci<bar_unsigned>("tEn"));
  ASSERT_EQ_STATIC(bar_unsigned::ElEvEn, enum_cast_ci<bar_unsigned>("ElEvEn"));
  ASSERT_EQ_STATIC(bar_unsigned::ThReE, enum_cast_ci<bar_unsigned>("thREE"));
  ASSERT_EQ_STATIC(bar_unsigned::InVaLiD, enum_cast_ci<bar_unsigned>("INVALID"));
  ASSERT_EQ_STATIC(bar_unsigned::TwO, enum_cast_ci<bar_unsigned>("tWO"));

  test_invalid_cases_common<bar_unsigned>();
}

TEST(EnumCastFromStringCI, BazSigned) {
  ASSERT_EQ_STATIC(baz_signed::sAn, enum_cast_ci<baz_signed>("san"));
  ASSERT_EQ_STATIC(baz_signed::eR, enum_cast_ci<baz_signed>("ER"));
  ASSERT_EQ_STATIC(baz_signed::yI, enum_cast_ci<baz_signed>("yI"));
  ASSERT_EQ_STATIC(baz_signed::LInG, enum_cast_ci<baz_signed>("LIng"));
  ASSERT_EQ_STATIC(baz_signed::fUyI, enum_cast_ci<baz_signed>("fUyI"));

  test_invalid_cases_common<baz_signed>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<baz_signed>("oNe"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<baz_signed>("Si"));
}

TEST(EnumCastFromStringCI, QuxUnsigned) {
  ASSERT_EQ_STATIC(qux_unsigned::ER, enum_cast_ci<qux_unsigned>("Er"));
  ASSERT_EQ_STATIC(qux_unsigned::fuYi, enum_cast_ci<qux_unsigned>("FuYi"));
  ASSERT_EQ_STATIC(qux_unsigned::YI, enum_cast_ci<qux_unsigned>("YI"));
  ASSERT_EQ_STATIC(qux_unsigned::SAN, enum_cast_ci<qux_unsigned>("sAn"));
  ASSERT_EQ_STATIC(qux_unsigned::ling, enum_cast_ci<qux_unsigned>("liNG"));
  ASSERT_EQ_STATIC(qux_unsigned::SI, enum_cast_ci<qux_unsigned>("sI"));

  test_invalid_cases_common<qux_unsigned>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<qux_unsigned>("3"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<qux_unsigned>("zero"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<qux_unsigned>("Zero"));
}

TEST(EnumCastFromStringCI, Empty) {
  test_invalid_cases_common<empty>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<empty>("zero"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<empty>("invalid"));
}

TEST(EnumCastFromStringCI, Single) {
  ASSERT_EQ_STATIC(single::Value, enum_cast_ci<single>("value"));
  ASSERT_EQ_STATIC(single::Value, enum_cast_ci<single>("Value"));
  ASSERT_EQ_STATIC(single::Value, enum_cast_ci<single>("VALUE"));
  test_invalid_cases_common<single>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<single>("233"));
}

TEST(EnumCastFromStringCI, SingleRep) {
  ASSERT_EQ_STATIC(single_rep::laUgh, enum_cast_ci<single_rep>("laugh"));
  ASSERT_EQ_STATIC(single_rep::hAhAhA, enum_cast_ci<single_rep>("HaHaHa"));
  ASSERT_EQ_STATIC(single_rep::oVer, enum_cast_ci<single_rep>("Over"));
  ASSERT_EQ_STATIC(single_rep::And_Over, enum_cast_ci<single_rep>("and_Over"));
  ASSERT_EQ_STATIC(single_rep::AGAIN, enum_cast_ci<single_rep>("AGAIN"));
  ASSERT_EQ_STATIC(single_rep::UNstoppable, enum_cast_ci<single_rep>("unStoppable"));

  test_invalid_cases_common<single_rep>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<single_rep>("value"));
}

TEST(EnumCastFromStringCI, Color) {
  ASSERT_EQ_STATIC(color::hot_pink, enum_cast_ci<color>("hot_pink"));
  ASSERT_EQ_STATIC(color::purple, enum_cast_ci<color>("PURPLE"));
  ASSERT_EQ_STATIC(color::blanched_almond, enum_cast_ci<color>("blanched_almond"));
  ASSERT_EQ_STATIC(color::steel_blue, enum_cast_ci<color>("steel_blue"));
  ASSERT_EQ_STATIC(color::alice_blue, enum_cast_ci<color>("alice_blue"));
  ASSERT_EQ_STATIC(color::maroon, enum_cast_ci<color>("Maroon"));
  ASSERT_EQ_STATIC(color::medium_blue, enum_cast_ci<color>("medium_blue"));
  ASSERT_EQ_STATIC(color::fire_brick, enum_cast_ci<color>("fire_brick"));
  ASSERT_EQ_STATIC(color::yellow_green, enum_cast_ci<color>("yellow_green"));
  ASSERT_EQ_STATIC(color::light_gray, enum_cast_ci<color>("light_gray"));
  ASSERT_EQ_STATIC(color::salmon, enum_cast_ci<color>("sALmon"));
  ASSERT_EQ_STATIC(color::medium_slate_blue, enum_cast_ci<color>("Medium_Slate_Blue"));
  ASSERT_EQ_STATIC(color::dark_turquoise, enum_cast_ci<color>("dark_turquoise"));
  ASSERT_EQ_STATIC(color::medium_sea_green, enum_cast_ci<color>("medium_sea_green"));
  ASSERT_EQ_STATIC(color::dark_sea_green, enum_cast_ci<color>("dark_sEA_green"));
  ASSERT_EQ_STATIC(color::pale_green, enum_cast_ci<color>("pale_green"));
  ASSERT_EQ_STATIC(color::dark_orange, enum_cast_ci<color>("dark_orange"));
  ASSERT_EQ_STATIC(color::light_blue, enum_cast_ci<color>("light_blue"));
  ASSERT_EQ_STATIC(color::olive, enum_cast_ci<color>("olive"));
  ASSERT_EQ_STATIC(color::orange_red, enum_cast_ci<color>("orange_red"));
  ASSERT_EQ_STATIC(color::mint_cream, enum_cast_ci<color>("mint_cream"));
  ASSERT_EQ_STATIC(color::tomato, enum_cast_ci<color>("Tomato"));
  ASSERT_EQ_STATIC(color::lavender_blush, enum_cast_ci<color>("lavender_blush"));
  ASSERT_EQ_STATIC(color::pale_violet_red, enum_cast_ci<color>("pale_violet_red"));
  ASSERT_EQ_STATIC(color::dark_olive_green, enum_cast_ci<color>("dark_olive_green"));
  ASSERT_EQ_STATIC(color::moccasin, enum_cast_ci<color>("moCCasin"));
  ASSERT_EQ_STATIC(color::crimson, enum_cast_ci<color>("crimson"));
  ASSERT_EQ_STATIC(color::deep_sky_blue, enum_cast_ci<color>("dEEp_sky_blue"));
  ASSERT_EQ_STATIC(color::pink, enum_cast_ci<color>("pink"));
  ASSERT_EQ_STATIC(color::burly_wood, enum_cast_ci<color>("burly_wOOd"));
  ASSERT_EQ_STATIC(color::sienna, enum_cast_ci<color>("sienna"));
  ASSERT_EQ_STATIC(color::dark_golden_rod, enum_cast_ci<color>("dark_Golden_rod"));
  ASSERT_EQ_STATIC(color::old_lace, enum_cast_ci<color>("old_lace"));
  ASSERT_EQ_STATIC(color::pale_golden_rod, enum_cast_ci<color>("pale_golden_rod"));
  ASSERT_EQ_STATIC(color::gainsboro, enum_cast_ci<color>("gainsboro"));
  ASSERT_EQ_STATIC(color::honey_dew, enum_cast_ci<color>("Honey_dew"));
  ASSERT_EQ_STATIC(color::dark_green, enum_cast_ci<color>("dark_GREEN"));
  ASSERT_EQ_STATIC(color::light_coral, enum_cast_ci<color>("light_coral"));
  ASSERT_EQ_STATIC(color::peach_puff, enum_cast_ci<color>("peach_puff"));
  ASSERT_EQ_STATIC(color::deep_pink, enum_cast_ci<color>("DEEP_PINK"));
  ASSERT_EQ_STATIC(color::light_sea_green, enum_cast_ci<color>("light_sea_green"));
  ASSERT_EQ_STATIC(color::gray, enum_cast_ci<color>("GRAY"));
  ASSERT_EQ_STATIC(color::sky_blue, enum_cast_ci<color>("sky_blue"));
  ASSERT_EQ_STATIC(color::lime, enum_cast_ci<color>("Lime"));
  ASSERT_EQ_STATIC(color::cornsilk, enum_cast_ci<color>("cornsilk"));
  ASSERT_EQ_STATIC(color::light_golden_rod_yellow, enum_cast_ci<color>("light_golden_rod_yellow"));
  ASSERT_EQ_STATIC(color::blue_violet, enum_cast_ci<color>("blue_Violet"));
  ASSERT_EQ_STATIC(color::light_slate_gray, enum_cast_ci<color>("light_slate_gray"));
  ASSERT_EQ_STATIC(color::saddle_brown, enum_cast_ci<color>("saDDle_brown"));
  ASSERT_EQ_STATIC(color::navy, enum_cast_ci<color>("NAVY"));
  ASSERT_EQ_STATIC(color::aquamarine, enum_cast_ci<color>("aquamarine"));
  ASSERT_EQ_STATIC(color::olive_drab, enum_cast_ci<color>("olive_drab"));
  ASSERT_EQ_STATIC(color::sandy_brown, enum_cast_ci<color>("sandy_brown"));
  ASSERT_EQ_STATIC(color::powder_blue, enum_cast_ci<color>("powder_BluE"));
  ASSERT_EQ_STATIC(color::dark_gray, enum_cast_ci<color>("dark_gray"));
  ASSERT_EQ_STATIC(color::medium_purple, enum_cast_ci<color>("medium_purple"));
  ASSERT_EQ_STATIC(color::violet, enum_cast_ci<color>("Violet"));
  ASSERT_EQ_STATIC(color::midnight_blue, enum_cast_ci<color>("midnight_blue"));
  ASSERT_EQ_STATIC(color::chartreuse, enum_cast_ci<color>("chartreuse"));
  ASSERT_EQ_STATIC(color::spring_green, enum_cast_ci<color>("spring_green"));
  ASSERT_EQ_STATIC(color::forest_green, enum_cast_ci<color>("forest_green"));
  ASSERT_EQ_STATIC(color::cadet_blue, enum_cast_ci<color>("cadet_blue"));
  ASSERT_EQ_STATIC(color::dim_gray, enum_cast_ci<color>("dim_gray"));
  ASSERT_EQ_STATIC(color::royal_blue, enum_cast_ci<color>("royal_blue"));
  ASSERT_EQ_STATIC(color::light_yellow, enum_cast_ci<color>("light_yellow"));
  ASSERT_EQ_STATIC(color::lemon_chiffon, enum_cast_ci<color>("lemon_chiffon"));
  ASSERT_EQ_STATIC(color::chocolate, enum_cast_ci<color>("chocolate"));
  ASSERT_EQ_STATIC(color::thistle, enum_cast_ci<color>("thistle"));
  ASSERT_EQ_STATIC(color::peru, enum_cast_ci<color>("Peru"));
  ASSERT_EQ_STATIC(color::cornflower_blue, enum_cast_ci<color>("cornflower_blue"));
  ASSERT_EQ_STATIC(color::gold, enum_cast_ci<color>("GOLD"));
  ASSERT_EQ_STATIC(color::cyan, enum_cast_ci<color>("cYaN"));
  ASSERT_EQ_STATIC(color::medium_turquoise, enum_cast_ci<color>("medium_turquoise"));
  ASSERT_EQ_STATIC(color::turquoise, enum_cast_ci<color>("turquoise"));
  ASSERT_EQ_STATIC(color::ghost_white, enum_cast_ci<color>("ghost_white"));
  ASSERT_EQ_STATIC(color::slate_gray, enum_cast_ci<color>("slate_gray"));
  ASSERT_EQ_STATIC(color::indian_red, enum_cast_ci<color>("indian_red"));
  ASSERT_EQ_STATIC(color::wheat, enum_cast_ci<color>("wheat"));
  ASSERT_EQ_STATIC(color::white, enum_cast_ci<color>("WHIte"));
  ASSERT_EQ_STATIC(color::light_pink, enum_cast_ci<color>("light_pink"));
  ASSERT_EQ_STATIC(color::medium_spring_green, enum_cast_ci<color>("medium_spring_green"));
  ASSERT_EQ_STATIC(color::medium_aquamarine, enum_cast_ci<color>("medium_aquamarine"));
  ASSERT_EQ_STATIC(color::magenta, enum_cast_ci<color>("magenta"));
  ASSERT_EQ_STATIC(color::khaki, enum_cast_ci<color>("KhaKi"));
  ASSERT_EQ_STATIC(color::medium_violet_red, enum_cast_ci<color>("medium_violet_red"));
  ASSERT_EQ_STATIC(color::papaya_whip, enum_cast_ci<color>("papaya_whip"));
  ASSERT_EQ_STATIC(color::navajo_white, enum_cast_ci<color>("navajo_white"));
  ASSERT_EQ_STATIC(color::light_green, enum_cast_ci<color>("light_green"));
  ASSERT_EQ_STATIC(color::green, enum_cast_ci<color>("green"));
  ASSERT_EQ_STATIC(color::teal, enum_cast_ci<color>("Teal"));
  ASSERT_EQ_STATIC(color::fuchsia, enum_cast_ci<color>("fuchsia"));
  ASSERT_EQ_STATIC(color::dodger_blue, enum_cast_ci<color>("dodger_blue"));
  ASSERT_EQ_STATIC(color::medium_orchid, enum_cast_ci<color>("medium_orchid"));
  ASSERT_EQ_STATIC(color::rebecca_purple, enum_cast_ci<color>("rebecca_purple"));
  ASSERT_EQ_STATIC(color::tan, enum_cast_ci<color>("tAN"));
  ASSERT_EQ_STATIC(color::red, enum_cast_ci<color>("RED"));
  ASSERT_EQ_STATIC(color::indigo, enum_cast_ci<color>("indigo"));
  ASSERT_EQ_STATIC(color::ivory, enum_cast_ci<color>("ivory"));
  ASSERT_EQ_STATIC(color::dark_khaki, enum_cast_ci<color>("dark_khaki"));
  ASSERT_EQ_STATIC(color::green_yellow, enum_cast_ci<color>("green_yellow"));
  ASSERT_EQ_STATIC(color::brown, enum_cast_ci<color>("brown"));
  ASSERT_EQ_STATIC(color::misty_rose, enum_cast_ci<color>("misty_rose"));
  ASSERT_EQ_STATIC(color::pale_turquoise, enum_cast_ci<color>("pale_turquoise"));
  ASSERT_EQ_STATIC(color::bisque, enum_cast_ci<color>("bisque"));
  ASSERT_EQ_STATIC(color::snow, enum_cast_ci<color>("Snow"));
  ASSERT_EQ_STATIC(color::rosy_brown, enum_cast_ci<color>("rosy_brown"));
  ASSERT_EQ_STATIC(color::dark_magenta, enum_cast_ci<color>("dark_magenta"));
  ASSERT_EQ_STATIC(color::antique_white, enum_cast_ci<color>("antique_white"));
  ASSERT_EQ_STATIC(color::light_salmon, enum_cast_ci<color>("light_salmon"));
  ASSERT_EQ_STATIC(color::orchid, enum_cast_ci<color>("orchid"));
  ASSERT_EQ_STATIC(color::silver, enum_cast_ci<color>("silver"));
  ASSERT_EQ_STATIC(color::light_steel_blue, enum_cast_ci<color>("light_steel_blue"));
  ASSERT_EQ_STATIC(color::linen, enum_cast_ci<color>("Linen"));
  ASSERT_EQ_STATIC(color::floral_white, enum_cast_ci<color>("floral_white"));
  ASSERT_EQ_STATIC(color::white_smoke, enum_cast_ci<color>("white_smoke"));
  ASSERT_EQ_STATIC(color::dark_salmon, enum_cast_ci<color>("dark_salmon"));
  ASSERT_EQ_STATIC(color::orange, enum_cast_ci<color>("orange"));
  ASSERT_EQ_STATIC(color::black, enum_cast_ci<color>("BLAcK"));
  ASSERT_EQ_STATIC(color::sea_green, enum_cast_ci<color>("sea_green"));
  ASSERT_EQ_STATIC(color::light_sky_blue, enum_cast_ci<color>("light_sky_blue"));
  ASSERT_EQ_STATIC(color::dark_violet, enum_cast_ci<color>("dark_violet"));
  ASSERT_EQ_STATIC(color::dark_slate_gray, enum_cast_ci<color>("dark_slate_gray"));
  ASSERT_EQ_STATIC(color::aqua, enum_cast_ci<color>("AquA"));
  ASSERT_EQ_STATIC(color::dark_blue, enum_cast_ci<color>("dark_blue"));
  ASSERT_EQ_STATIC(color::blue, enum_cast_ci<color>("BLUe"));
  ASSERT_EQ_STATIC(color::slate_blue, enum_cast_ci<color>("slate_blue"));
  ASSERT_EQ_STATIC(color::dark_red, enum_cast_ci<color>("dark_rEd"));
  ASSERT_EQ_STATIC(color::lavender, enum_cast_ci<color>("lavEndEr"));
  ASSERT_EQ_STATIC(color::plum, enum_cast_ci<color>("plum"));
  ASSERT_EQ_STATIC(color::sea_shell, enum_cast_ci<color>("sea_shell"));
  ASSERT_EQ_STATIC(color::yellow, enum_cast_ci<color>("yellow"));
  ASSERT_EQ_STATIC(color::coral, enum_cast_ci<color>("coral"));
  ASSERT_EQ_STATIC(color::azure, enum_cast_ci<color>("azure"));
  ASSERT_EQ_STATIC(color::dark_orchid, enum_cast_ci<color>("dark_orchid"));
  ASSERT_EQ_STATIC(color::lawn_green, enum_cast_ci<color>("lawn_GReen"));
  ASSERT_EQ_STATIC(color::light_cyan, enum_cast_ci<color>("light_cyan"));
  ASSERT_EQ_STATIC(color::lime_green, enum_cast_ci<color>("lime_grEEn"));
  ASSERT_EQ_STATIC(color::dark_slate_blue, enum_cast_ci<color>("dark_slate_blue"));
  ASSERT_EQ_STATIC(color::beige, enum_cast_ci<color>("beige"));
  ASSERT_EQ_STATIC(color::dark_cyan, enum_cast_ci<color>("dark_cyan"));
  ASSERT_EQ_STATIC(color::golden_rod, enum_cast_ci<color>("golden_rod"));

  ASSERT_EQ_STATIC(color::fuchsia, enum_cast_ci<color>("mAgEntA"));
  test_invalid_cases_common<color>();
}

TEST(EnumCastFromStringCI, TerminalColor) {
  ASSERT_EQ_STATIC(terminal_color::BLUe, enum_cast_ci<terminal_color>("blue"));
  ASSERT_EQ_STATIC(terminal_color::briGht_yellow, enum_cast_ci<terminal_color>("Bright_Yellow"));
  ASSERT_EQ_STATIC(terminal_color::grEEn, enum_cast_ci<terminal_color>("grEEn"));
  ASSERT_EQ_STATIC(terminal_color::cYan, enum_cast_ci<terminal_color>("Cyan"));
  ASSERT_EQ_STATIC(terminal_color::brIght_green, enum_cast_ci<terminal_color>("bright_grEEn"));
  ASSERT_EQ_STATIC(terminal_color::Black, enum_cast_ci<terminal_color>("blAck"));
  ASSERT_EQ_STATIC(terminal_color::bright_white, enum_cast_ci<terminal_color>("BRIGHT_white"));
  ASSERT_EQ_STATIC(terminal_color::yeLLow, enum_cast_ci<terminal_color>("yeLLow"));
  ASSERT_EQ_STATIC(terminal_color::brigHt_blue, enum_cast_ci<terminal_color>("bright_BLUE"));
  ASSERT_EQ_STATIC(terminal_color::bRight_red, enum_cast_ci<terminal_color>("Bright_Red"));
  ASSERT_EQ_STATIC(terminal_color::rEd, enum_cast_ci<terminal_color>("Red"));
  ASSERT_EQ_STATIC(terminal_color::brighT_magenta, enum_cast_ci<terminal_color>("brIght_MAgentA"));
  ASSERT_EQ_STATIC(terminal_color::Bright_black, enum_cast_ci<terminal_color>("bright_bLack"));
  ASSERT_EQ_STATIC(terminal_color::WHITE, enum_cast_ci<terminal_color>("WHITE"));
  ASSERT_EQ_STATIC(terminal_color::mAgEntA, enum_cast_ci<terminal_color>("maGEnta"));

  test_invalid_cases_common<terminal_color>();
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<terminal_color>("fuchsia"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<terminal_color>("BrightYellow"));
  ASSERT_EQ_STATIC(std::nullopt, enum_cast_ci<terminal_color>("brightgreen"));
}

TEST(EnumCastFromStringCI, HashCollision) {
  ASSERT_EQ_STATIC(hash_collision::_wSYZDRpiQJf8Rfv,
                   enum_cast_ci<hash_collision>("_wsyzdRpiQJf8Rfv"));
  ASSERT_EQ_STATIC(hash_collision::_cuFFJIHGp_jNJKS,
                   enum_cast_ci<hash_collision>("_cuffjihgp_jNJKS"));

  test_invalid_cases_common<hash_collision>();
}
