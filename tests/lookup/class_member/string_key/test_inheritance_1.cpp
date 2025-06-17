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

struct bar_1_t : foo_1_t {
  int value_a;
  int value_b;
  int value_c;
  int count;

  constexpr int get_z_squared_plus_a(int a) const {
    return value_z * value_z + a;
  }
  constexpr int get_a_squared_plus_a(int a) const {
    return value_a * value_a + a;
  }
};

TEST(ClassLookupTableByName, Inheritance1) {
  constexpr auto table_value = LOOKUP_TABLE(bar_1_t, "value_*");
  static_assert(std::is_same_v<int bar_1_t::*, decltype(table_value)::value_type>);
  static_assert(table_value.size() == 6);

  constexpr auto bar = bar_1_t{{1, 2, 3, 4}, 5, 6, 7};
  CHECK_MEMBER_VARIABLE_STATIC(1, bar, table_value["x"]);
  CHECK_MEMBER_VARIABLE_STATIC(2, bar, table_value["y"]);
  CHECK_MEMBER_VARIABLE_STATIC(3, bar, table_value["z"]);
  CHECK_MEMBER_VARIABLE_STATIC(5, bar, table_value["a"]);
  CHECK_MEMBER_VARIABLE_STATIC(6, bar, table_value["b"]);
  CHECK_MEMBER_VARIABLE_STATIC(7, bar, table_value["c"]);

  EXPECT_EQ_STATIC(nullptr, table_value["count"]);
  EXPECT_EQ_STATIC(nullptr, table_value["value_a"]);
  EXPECT_EQ_STATIC(nullptr, table_value["value_x"]);

  constexpr auto table_fn = LOOKUP_TABLE(bar_1_t, "get_*_squared_plus_a");
  static_assert(std::is_same_v<int (bar_1_t::*)(int) const, decltype(table_fn)::value_type>);
  static_assert(table_fn.size() == 4);

  CHECK_MEMBER_FUNCTION_STATIC(11, bar, table_fn["x"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(14, bar, table_fn["y"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(19, bar, table_fn["z"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(35, bar, table_fn["a"], 10);

  EXPECT_EQ_STATIC(nullptr, table_fn["b"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_a_squared_plus_a"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_z_plus_a"]);
}
