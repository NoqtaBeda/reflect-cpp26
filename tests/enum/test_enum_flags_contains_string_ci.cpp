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

#include <climits>

#include "tests/enum/flags_test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_flags_contains.hpp>
#endif

namespace rfl = reflect_cpp26;

template <class E>
constexpr bool enum_flags_contains_ci(std::string_view str) {
  return rfl::enum_flags_contains<E>(rfl::ascii_case_insensitive, str);
}

template <class E, class Delim>
constexpr bool enum_flags_contains_ci(std::string_view str, Delim delim) {
  return rfl::enum_flags_contains<E>(rfl::ascii_case_insensitive, str, delim);
}

TEST(EnumFlagsContainsStringCI, D1) {
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("One"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("TWO"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("four"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("eIGht"));

  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("ONE|two"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("one|TWO|four"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("one|two|FOUR|eight"));
  // Custom delimiter
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("one,Four", ','));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("One, four, EIGHT", ','));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>(" two | FOur | eight ", "|"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("eighT+fouR + TWO", '+'));
  // Empty tokens
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("One || tWo", "|"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>("oNe, foUr, eigHt, ", ','));
  // Empty string
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>(""));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>(" ", '|'));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D1>(" \n\t \n\t ", " | "));

  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D1>("one|two|three"));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D1>("one|eight", " | "));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D1>("one, four, eight", '|'));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D1>("one, four | eight", ",|"));
}

TEST(EnumFlagsContainsStringCI, D2) {
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("zero"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("LiNG"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("one"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("TWO"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("yi"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>(" er "));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("\nTHREE\t"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("\n FouR\n "));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("FivE"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("SiX"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("SeVeN"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("Wu"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("LIU"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("QI"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("  EighT  "));

  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("ONE | TWO | FOUR"));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("One | ThrEE | FouR", " | "));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("WU, qi", ','));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>(" TWo + SiX + EiGht ", " + "));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>(" TwO + FoUr + EigHt + er + yi ", '+'));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("\nOne \n\nThrEE\nSevEN\nWU  \n\n", '\n'));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>("\nONE \n\nThreE SeveN WU  \n\n", ' '));

  // Empty string
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>(""));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>(" ", '|'));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<D2>(" \n\t \n\t ", " | "));

  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D2>("Jiu"));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D2>("One Two Three"));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D2>("One Two Four"));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<D2>("\nOne \n\nThree\nSeven\nWU  \n\n", ' '));
}

TEST(EnumFlagsContainsStringCI, Empty) {
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<empty>(""));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<empty>("  "));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<empty>("  ", '|'));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<empty>(" \n\t \n\t \n\t ", ' '));
  EXPECT_TRUE_STATIC(enum_flags_contains_ci<empty>(" \n\t \n\t \n\t ", '|'));

  EXPECT_FALSE_STATIC(enum_flags_contains_ci<empty>("zero"));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<empty>(" empty "));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<empty>(" 0 ", '|'));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<empty>(" n\t n\t ", ' '));
  EXPECT_FALSE_STATIC(enum_flags_contains_ci<empty>(" n\t n\t ", '|'));
}
