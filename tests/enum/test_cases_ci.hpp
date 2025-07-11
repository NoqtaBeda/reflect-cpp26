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

#pragma once

#include <cstdint>

#define ENUM_TEST_CASES_FOR_EACH(F) \
  F(foo_signed)                     \
  F(foo_signed_reorder)             \
  F(foo_signed_rep)                 \
  F(bar_unsigned)                   \
  F(baz_signed)                     \
  F(qux_unsigned)                   \
  F(empty)                          \
  F(single)                         \
  F(single_rep)                     \
  F(color)                          \
  F(terminal_color)                 \
  F(hash_collision)

enum class foo_signed: int32_t {
  zero = 0,
  one = 1,
  two = 2,
  four = 4,
  five = 5,
  six = 6,
  seven = 7,
  invalid = -1,
  error = -2,
};

enum class foo_signed_reorder: int32_t {
  Two = 2,
  Five = 5,
  Zero = 0,
  Seven = 7,
  One = 1,
  Six = 6,
  Invalid = -1,
  Four = 4,
  Error = -2,
};

enum class foo_signed_rep: int32_t {
  ZERO = 0,
  ONE = 1,
  TWO = 2,
  YI = 1,
  ER = 2,
  FOUR = 4,
  FIVE = 5,
  SIX = 6,
  SEVEN = 7,
  INVALID = -1,
  ERROR = -2,
};

enum class bar_unsigned: uint32_t {
  ZeRo = 0,
  OnE = 1,
  TwO = 2,
  ThReE = 3,
  TeN = 10,
  ElEvEn = 11,
  TwElVe = 12,
  ThIrTeEn = 13,
  FoUrTeEn = 14,
  InVaLiD = -1u,
  ErRoR = -2u,
};

enum class baz_signed: int16_t {
  LInG = 0,
  yI = 1,
  eR = 2,
  sAn = 3,
  fUyI = -1,
};

enum class qux_unsigned: uint16_t {
  ling = 0,
  YI = 1,
  ER = 2,
  SAN = 3,
  SI = 4,
  fuYi = static_cast<uint16_t>(-1u),
};

enum empty {};

enum single {
  Value = 233,
};

enum class single_rep {
  hAhAhA = 233,
  laUgh = 233,
  oVer = 233,
  And_Over = 233,
  AGAIN = 233,
  UNstoppable = 233,
};

// From: https://github.com/fmtlib/fmt/blob/master/include/fmt/color.h
enum class color : uint32_t {
  alice_blue = 0xF0F8FF,               // rgb(240,248,255)
  antique_white = 0xFAEBD7,            // rgb(250,235,215)
  aqua = 0x00FFFF,                     // rgb(0,255,255)
  aquamarine = 0x7FFFD4,               // rgb(127,255,212)
  azure = 0xF0FFFF,                    // rgb(240,255,255)
  beige = 0xF5F5DC,                    // rgb(245,245,220)
  bisque = 0xFFE4C4,                   // rgb(255,228,196)
  black = 0x000000,                    // rgb(0,0,0)
  blanched_almond = 0xFFEBCD,          // rgb(255,235,205)
  blue = 0x0000FF,                     // rgb(0,0,255)
  blue_violet = 0x8A2BE2,              // rgb(138,43,226)
  brown = 0xA52A2A,                    // rgb(165,42,42)
  burly_wood = 0xDEB887,               // rgb(222,184,135)
  cadet_blue = 0x5F9EA0,               // rgb(95,158,160)
  chartreuse = 0x7FFF00,               // rgb(127,255,0)
  chocolate = 0xD2691E,                // rgb(210,105,30)
  coral = 0xFF7F50,                    // rgb(255,127,80)
  cornflower_blue = 0x6495ED,          // rgb(100,149,237)
  cornsilk = 0xFFF8DC,                 // rgb(255,248,220)
  crimson = 0xDC143C,                  // rgb(220,20,60)
  cyan = 0x00FFFF,                     // rgb(0,255,255)
  dark_blue = 0x00008B,                // rgb(0,0,139)
  dark_cyan = 0x008B8B,                // rgb(0,139,139)
  dark_golden_rod = 0xB8860B,          // rgb(184,134,11)
  dark_gray = 0xA9A9A9,                // rgb(169,169,169)
  dark_green = 0x006400,               // rgb(0,100,0)
  dark_khaki = 0xBDB76B,               // rgb(189,183,107)
  dark_magenta = 0x8B008B,             // rgb(139,0,139)
  dark_olive_green = 0x556B2F,         // rgb(85,107,47)
  dark_orange = 0xFF8C00,              // rgb(255,140,0)
  dark_orchid = 0x9932CC,              // rgb(153,50,204)
  dark_red = 0x8B0000,                 // rgb(139,0,0)
  dark_salmon = 0xE9967A,              // rgb(233,150,122)
  dark_sea_green = 0x8FBC8F,           // rgb(143,188,143)
  dark_slate_blue = 0x483D8B,          // rgb(72,61,139)
  dark_slate_gray = 0x2F4F4F,          // rgb(47,79,79)
  dark_turquoise = 0x00CED1,           // rgb(0,206,209)
  dark_violet = 0x9400D3,              // rgb(148,0,211)
  deep_pink = 0xFF1493,                // rgb(255,20,147)
  deep_sky_blue = 0x00BFFF,            // rgb(0,191,255)
  dim_gray = 0x696969,                 // rgb(105,105,105)
  dodger_blue = 0x1E90FF,              // rgb(30,144,255)
  fire_brick = 0xB22222,               // rgb(178,34,34)
  floral_white = 0xFFFAF0,             // rgb(255,250,240)
  forest_green = 0x228B22,             // rgb(34,139,34)
  fuchsia = 0xFF00FF,                  // rgb(255,0,255)
  gainsboro = 0xDCDCDC,                // rgb(220,220,220)
  ghost_white = 0xF8F8FF,              // rgb(248,248,255)
  gold = 0xFFD700,                     // rgb(255,215,0)
  golden_rod = 0xDAA520,               // rgb(218,165,32)
  gray = 0x808080,                     // rgb(128,128,128)
  green = 0x008000,                    // rgb(0,128,0)
  green_yellow = 0xADFF2F,             // rgb(173,255,47)
  honey_dew = 0xF0FFF0,                // rgb(240,255,240)
  hot_pink = 0xFF69B4,                 // rgb(255,105,180)
  indian_red = 0xCD5C5C,               // rgb(205,92,92)
  indigo = 0x4B0082,                   // rgb(75,0,130)
  ivory = 0xFFFFF0,                    // rgb(255,255,240)
  khaki = 0xF0E68C,                    // rgb(240,230,140)
  lavender = 0xE6E6FA,                 // rgb(230,230,250)
  lavender_blush = 0xFFF0F5,           // rgb(255,240,245)
  lawn_green = 0x7CFC00,               // rgb(124,252,0)
  lemon_chiffon = 0xFFFACD,            // rgb(255,250,205)
  light_blue = 0xADD8E6,               // rgb(173,216,230)
  light_coral = 0xF08080,              // rgb(240,128,128)
  light_cyan = 0xE0FFFF,               // rgb(224,255,255)
  light_golden_rod_yellow = 0xFAFAD2,  // rgb(250,250,210)
  light_gray = 0xD3D3D3,               // rgb(211,211,211)
  light_green = 0x90EE90,              // rgb(144,238,144)
  light_pink = 0xFFB6C1,               // rgb(255,182,193)
  light_salmon = 0xFFA07A,             // rgb(255,160,122)
  light_sea_green = 0x20B2AA,          // rgb(32,178,170)
  light_sky_blue = 0x87CEFA,           // rgb(135,206,250)
  light_slate_gray = 0x778899,         // rgb(119,136,153)
  light_steel_blue = 0xB0C4DE,         // rgb(176,196,222)
  light_yellow = 0xFFFFE0,             // rgb(255,255,224)
  lime = 0x00FF00,                     // rgb(0,255,0)
  lime_green = 0x32CD32,               // rgb(50,205,50)
  linen = 0xFAF0E6,                    // rgb(250,240,230)
  magenta = 0xFF00FF,                  // rgb(255,0,255)
  maroon = 0x800000,                   // rgb(128,0,0)
  medium_aquamarine = 0x66CDAA,        // rgb(102,205,170)
  medium_blue = 0x0000CD,              // rgb(0,0,205)
  medium_orchid = 0xBA55D3,            // rgb(186,85,211)
  medium_purple = 0x9370DB,            // rgb(147,112,219)
  medium_sea_green = 0x3CB371,         // rgb(60,179,113)
  medium_slate_blue = 0x7B68EE,        // rgb(123,104,238)
  medium_spring_green = 0x00FA9A,      // rgb(0,250,154)
  medium_turquoise = 0x48D1CC,         // rgb(72,209,204)
  medium_violet_red = 0xC71585,        // rgb(199,21,133)
  midnight_blue = 0x191970,            // rgb(25,25,112)
  mint_cream = 0xF5FFFA,               // rgb(245,255,250)
  misty_rose = 0xFFE4E1,               // rgb(255,228,225)
  moccasin = 0xFFE4B5,                 // rgb(255,228,181)
  navajo_white = 0xFFDEAD,             // rgb(255,222,173)
  navy = 0x000080,                     // rgb(0,0,128)
  old_lace = 0xFDF5E6,                 // rgb(253,245,230)
  olive = 0x808000,                    // rgb(128,128,0)
  olive_drab = 0x6B8E23,               // rgb(107,142,35)
  orange = 0xFFA500,                   // rgb(255,165,0)
  orange_red = 0xFF4500,               // rgb(255,69,0)
  orchid = 0xDA70D6,                   // rgb(218,112,214)
  pale_golden_rod = 0xEEE8AA,          // rgb(238,232,170)
  pale_green = 0x98FB98,               // rgb(152,251,152)
  pale_turquoise = 0xAFEEEE,           // rgb(175,238,238)
  pale_violet_red = 0xDB7093,          // rgb(219,112,147)
  papaya_whip = 0xFFEFD5,              // rgb(255,239,213)
  peach_puff = 0xFFDAB9,               // rgb(255,218,185)
  peru = 0xCD853F,                     // rgb(205,133,63)
  pink = 0xFFC0CB,                     // rgb(255,192,203)
  plum = 0xDDA0DD,                     // rgb(221,160,221)
  powder_blue = 0xB0E0E6,              // rgb(176,224,230)
  purple = 0x800080,                   // rgb(128,0,128)
  rebecca_purple = 0x663399,           // rgb(102,51,153)
  red = 0xFF0000,                      // rgb(255,0,0)
  rosy_brown = 0xBC8F8F,               // rgb(188,143,143)
  royal_blue = 0x4169E1,               // rgb(65,105,225)
  saddle_brown = 0x8B4513,             // rgb(139,69,19)
  salmon = 0xFA8072,                   // rgb(250,128,114)
  sandy_brown = 0xF4A460,              // rgb(244,164,96)
  sea_green = 0x2E8B57,                // rgb(46,139,87)
  sea_shell = 0xFFF5EE,                // rgb(255,245,238)
  sienna = 0xA0522D,                   // rgb(160,82,45)
  silver = 0xC0C0C0,                   // rgb(192,192,192)
  sky_blue = 0x87CEEB,                 // rgb(135,206,235)
  slate_blue = 0x6A5ACD,               // rgb(106,90,205)
  slate_gray = 0x708090,               // rgb(112,128,144)
  snow = 0xFFFAFA,                     // rgb(255,250,250)
  spring_green = 0x00FF7F,             // rgb(0,255,127)
  steel_blue = 0x4682B4,               // rgb(70,130,180)
  tan = 0xD2B48C,                      // rgb(210,180,140)
  teal = 0x008080,                     // rgb(0,128,128)
  thistle = 0xD8BFD8,                  // rgb(216,191,216)
  tomato = 0xFF6347,                   // rgb(255,99,71)
  turquoise = 0x40E0D0,                // rgb(64,224,208)
  violet = 0xEE82EE,                   // rgb(238,130,238)
  wheat = 0xF5DEB3,                    // rgb(245,222,179)
  white = 0xFFFFFF,                    // rgb(255,255,255)
  white_smoke = 0xF5F5F5,              // rgb(245,245,245)
  yellow = 0xFFFF00,                   // rgb(255,255,0)
  yellow_green = 0x9ACD32              // rgb(154,205,50)
}; // enum class color

// From: https://github.com/fmtlib/fmt/blob/master/include/fmt/color.h
enum class terminal_color : uint8_t {
  Black = 30,
  rEd,
  grEEn,
  yeLLow,
  BLUe,
  mAgEntA,
  cYan,
  WHITE,
  Bright_black = 90,
  bRight_red,
  brIght_green,
  briGht_yellow,
  brigHt_blue,
  brighT_magenta,
  BRIGHT_cyan,
  bright_white
};

enum class hash_collision {
  _wSYZDRpiQJf8Rfv,
  _cuFFJIHGp_jNJKS,
};
