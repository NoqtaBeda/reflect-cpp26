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

#include <reflect_cpp26/enum/enum_cast.hpp>
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

enum class foo_1_key {
  x,
  z,
  w,
};

TEST(ClassLookupTableByEnum, Basic) {
  constexpr auto table_v = LOOKUP_TABLE(foo_1_t, "value_*", rfl::enum_cast<foo_1_key>);
  static_assert(std::is_same_v<int foo_1_t::*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  constexpr auto foo = foo_1_t{.value_x = 10, .value_y = 20, .value_z = 30, .size = 4};
  CHECK_MEMBER_VARIABLE_STATIC(10, foo, table_v[foo_1_key::x]);
  CHECK_MEMBER_VARIABLE_STATIC(30, foo, table_v[foo_1_key::z]);
  EXPECT_EQ_STATIC(nullptr, table_v[foo_1_key::w]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<foo_1_key>(-1)]);

  constexpr auto table_f = LOOKUP_TABLE(foo_1_t, "get_*_squared_plus_a", rfl::enum_cast<foo_1_key>);
  static_assert(std::is_same_v<int (foo_1_t::*)(int) const, decltype(table_f)::value_type>);
  static_assert(table_f.size() == 1);

  CHECK_MEMBER_FUNCTION_STATIC(105, foo, table_f[foo_1_key::x], 5);
  EXPECT_EQ_STATIC(nullptr, table_f[foo_1_key::z]);
  EXPECT_EQ_STATIC(nullptr, table_f[foo_1_key::w]);
  EXPECT_EQ_STATIC(nullptr, table_f[static_cast<foo_1_key>(-2)]);
}
