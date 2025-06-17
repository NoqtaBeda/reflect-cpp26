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

#include <reflect_cpp26/lookup/lookup_table.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

struct foo_1_t {
  // Nested classes are expected to be filtered out.
  struct value_nested_squared_plus_a {};

  int value_x;
  int value_y;
  int value_z;
  int size;

  constexpr int get_x_squared_plus_a(int a) const {
    return value_x * value_x + a;
  }
  constexpr int get_y_squared_plus_a(int a) const {
    return value_y * value_y + a;
  }
  constexpr int get_z_plus_a(int a) const {
    return value_z + a;
  }
};

TEST(ClassLookupTableByName, CaseInsensitive) {
  constexpr auto table_value = LOOKUP_TABLE(foo_1_t,
                                            "value_*",
                                            {.fixed_map_options = rfl::string_key_fixed_map_options{
                                                 .ascii_case_insensitive = true,
                                             }});

  static_assert(std::is_same_v<int foo_1_t::*, decltype(table_value)::value_type>);
  static_assert(table_value.size() == 3);

  constexpr auto foo = foo_1_t{1, 2, 3, 4};
  // Note: Prefix and suffix ignored during lookup.
  CHECK_MEMBER_VARIABLE_STATIC(1, foo, table_value["X"]);
  CHECK_MEMBER_VARIABLE_STATIC(2, foo, table_value["Y"]);
  CHECK_MEMBER_VARIABLE_STATIC(3, foo, table_value["Z"]);
  EXPECT_EQ_STATIC(nullptr, table_value["a"]);
  EXPECT_EQ_STATIC(nullptr, table_value["size"]);
  EXPECT_EQ_STATIC(nullptr, table_value["value_x"]);

  constexpr auto table_fn = LOOKUP_TABLE(foo_1_t,
                                         "get_*_plus_a",
                                         {.fixed_map_options = rfl::string_key_fixed_map_options{
                                              .ascii_case_insensitive = true,
                                          }});
  static_assert(std::is_same_v<int (foo_1_t::*)(int) const, decltype(table_fn)::value_type>);
  static_assert(table_fn.size() == 3);

  CHECK_MEMBER_FUNCTION_STATIC(11, foo, table_fn["X_SQUARED"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(14, foo, table_fn["y_Squared"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(13, foo, table_fn["Z"], 10);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_x_squared_plus_a"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_z_plus_a"]);
}
