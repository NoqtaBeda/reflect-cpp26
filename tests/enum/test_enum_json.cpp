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

// Note: This non-static test case shall be preserved
// since we will check the binary executable file with external tools on
// whether the complete JSON string is generated in compile-time
// (which is unexpected behavior).

#include <reflect_cpp26/enum/enum_json.hpp>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

using namespace reflect_cpp26;

TEST(EnumJson, All) {
  constexpr auto foo_signed_str_original =
      R"__({"zero":0,"one":1,"two":2,"four":4,"five":5,"six":6,)__"
      R"__("seven":7,"invalid":-1,"error":-2})__";
  auto res_foo_signed_1 = enum_json<foo_signed, enum_entry_order::original>();
  EXPECT_EQ(foo_signed_str_original, res_foo_signed_1);

  constexpr auto foo_signed_str_by_value =
      R"__({"error":-2,"invalid":-1,"zero":0,"one":1,"two":2,"four":4,)__"
      R"__("five":5,"six":6,"seven":7})__";
  auto res_foo_signed_2 = enum_json<foo_signed, enum_entry_order::by_value>();
  EXPECT_EQ(foo_signed_str_by_value, res_foo_signed_2);

  constexpr auto foo_signed_str_by_name =
      R"__({"error":-2,"five":5,"four":4,"invalid":-1,"one":1,"seven":7,)__"
      R"__("six":6,"two":2,"zero":0})__";
  auto res_foo_signed_3 = enum_json<foo_signed, enum_entry_order::by_name>();
  EXPECT_EQ(foo_signed_str_by_name, res_foo_signed_3);

  constexpr auto qux_unsigned_str_original =
      R"({"ling":0,"yi":1,"er":2,"san":3,"si":4,"fuyi":65535})";
  auto res_qux_unsigned_1 = enum_json<qux_unsigned, enum_entry_order::original>();
  EXPECT_EQ(qux_unsigned_str_original, res_qux_unsigned_1);

  constexpr auto qux_unsigned_str_by_value =
      R"({"ling":0,"yi":1,"er":2,"san":3,"si":4,"fuyi":65535})";
  auto res_qux_unsigned_2 = enum_json<qux_unsigned, enum_entry_order::by_value>();
  EXPECT_EQ(qux_unsigned_str_by_value, res_qux_unsigned_2);

  constexpr auto qux_unsigned_str_by_name =
      R"({"er":2,"fuyi":65535,"ling":0,"san":3,"si":4,"yi":1})";
  auto res_qux_unsigned_3 = enum_json<qux_unsigned, enum_entry_order::by_name>();
  EXPECT_EQ(qux_unsigned_str_by_name, res_qux_unsigned_3);

  auto res_empty = enum_json<empty, enum_entry_order::original>();
  EXPECT_EQ("{}", res_empty);

  auto res_single = enum_json<single, enum_entry_order::original>();
  EXPECT_EQ(R"({"value":233})", res_single);
}
