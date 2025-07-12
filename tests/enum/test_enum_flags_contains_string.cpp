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

#include "tests/enum/flags_test_cases.hpp"
#include "tests/test_options.hpp"
#include <climits>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_flags_contains.hpp>
#endif

namespace rfl = reflect_cpp26;

TEST(EnumFlagsContainsString, D1)
{
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("two"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("four"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("eight"));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one|two"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one|two|four"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one|two|four|eight"));
  // Custom delimiter
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one,four", ','));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one, four, eight", ','));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(" two | four | eight ", "|"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("eight+four + two", '+'));
  // Empty tokens
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one || two", "|"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>("one, four, eight, ", ','));
  // Empty string
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(""));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(" ", '|'));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D1>(" \n\t \n\t ", " | "));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>("one|two|three"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>("One|Two|Four"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>("one|eight", " | "));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>("one, four, eight", '|'));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D1>("one, four | eight", ",|"));
}

TEST(EnumFlagsContainsString, D2)
{
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Zero"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("LING"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("One"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Two"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("YI"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("ER"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Three"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Four"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Five"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Six"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Seven"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("WU"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Liu"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Qi"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("Eight"));

  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("One | Two | Four"));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("One | Three | Four", " | "));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>("WU, Qi", ','));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(
    " Two + Six + Eight ", " + "));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(
    " Two + Four + Eight + ER + YI ", '+'));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(
    "\nOne \n\nThree\nSeven\nWU  \n\n", '\n'));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(
    "\nOne \n\nThree Seven WU  \n\n", ' '));

  // Empty string
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(""));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(" ", '|'));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<D2>(" \n\t \n\t ", " | "));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("zero"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("One|TWO"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("Wu|Liu|Qi"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("EIGHT"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("Jiu"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("8"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("One Two Three"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>("One Two Four"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<D2>(
    "\nOne \n\nThree\nSeven\nWU  \n\n", ' '));
}

TEST(EnumFlagsContainsString, Empty)
{
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<empty>(""));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<empty>("  "));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<empty>("  ", '|'));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<empty>(" \n\t \n\t \n\t ", ' '));
  EXPECT_TRUE_STATIC(rfl::enum_flags_contains<empty>(" \n\t \n\t \n\t ", '|'));

  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<empty>("zero"));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<empty>(" empty "));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<empty>(" 0 ", '|'));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<empty>(" n\t n\t ", ' '));
  EXPECT_FALSE_STATIC(rfl::enum_flags_contains<empty>(" n\t n\t ", '|'));
}
