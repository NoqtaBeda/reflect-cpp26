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
#include <reflect_cpp26/enum/enum_json.hpp>
#endif

using namespace reflect_cpp26;

constexpr auto foo_signed_str_original =
    R"__({"zero":0,"one":1,"two":2,"four":4,"five":5,"six":6,)__"
    R"__("seven":7,"invalid":-1,"error":-2})__";
static_assert(foo_signed_str_original == enum_json<foo_signed, enum_entry_order::original>());
static_assert(foo_signed_str_original
              == enum_json_static_v<foo_signed, enum_entry_order::original>);

constexpr auto foo_signed_str_by_value =
    R"__({"error":-2,"invalid":-1,"zero":0,"one":1,"two":2,"four":4,)__"
    R"__("five":5,"six":6,"seven":7})__";
static_assert(foo_signed_str_by_value == enum_json<foo_signed, enum_entry_order::by_value>());
static_assert(foo_signed_str_by_value
              == enum_json_static_v<foo_signed, enum_entry_order::by_value>);

constexpr auto foo_signed_str_by_name =
    R"__({"error":-2,"five":5,"four":4,"invalid":-1,"one":1,"seven":7,)__"
    R"__("six":6,"two":2,"zero":0})__";
static_assert(foo_signed_str_by_name == enum_json<foo_signed, enum_entry_order::by_name>());
static_assert(foo_signed_str_by_name == enum_json_static_v<foo_signed, enum_entry_order::by_name>);

constexpr auto qux_unsigned_str_original =
    R"({"ling":0,"yi":1,"er":2,"san":3,"si":4,"fuyi":65535})";
static_assert(qux_unsigned_str_original == enum_json<qux_unsigned, enum_entry_order::original>());
static_assert(qux_unsigned_str_original
              == enum_json_static_v<qux_unsigned, enum_entry_order::original>);

constexpr auto qux_unsigned_str_by_value =
    R"({"ling":0,"yi":1,"er":2,"san":3,"si":4,"fuyi":65535})";
static_assert(qux_unsigned_str_by_value == enum_json<qux_unsigned, enum_entry_order::by_value>());
static_assert(qux_unsigned_str_by_value
              == enum_json_static_v<qux_unsigned, enum_entry_order::by_value>);

constexpr auto qux_unsigned_str_by_name = R"({"er":2,"fuyi":65535,"ling":0,"san":3,"si":4,"yi":1})";
static_assert(qux_unsigned_str_by_name == enum_json<qux_unsigned, enum_entry_order::by_name>());
static_assert(qux_unsigned_str_by_name
              == enum_json_static_v<qux_unsigned, enum_entry_order::by_name>);

static_assert("{}" == enum_json<empty, enum_entry_order::original>());
static_assert("{}" == enum_json_static_v<empty, enum_entry_order::by_name>);

static_assert(R"({"value":233})" == enum_json<single, enum_entry_order::original>());
static_assert(R"({"value":233})" == enum_json_static_v<single, enum_entry_order::by_value>);

TEST(EnumJson, StaticAll) {
  EXPECT_TRUE(true);  // All test cases done by static assertions above.
}
