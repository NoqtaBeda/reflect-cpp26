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

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

enum class foo_get_fn_key {
  abs,
  even,
  non_zero,
  odd,
};

enum class foo_value_key {
  x,
  z,
};

namespace foo {
namespace bar {
namespace get_options {
constexpr int get_negative(int value) {
  return value >= 0 ? -1 : value;
}
volatile double value_w = -1.0;
}  // namespace get_options
class get_dummy {};

constexpr int get_odd(int value) {
  return value % 2 == 0 ? value + 1 : value;
}
constexpr int get_even(int value) {
  return value % 2 == 0 ? value : value + 1;
}
constexpr int get_abs(int value) {
  return value >= 0 ? value : -value;
}
volatile double value_x = 1.0;
volatile double value_y = 2.0;
volatile double value_z = 3.0;
}  // namespace bar
constexpr int get_non_zero(int value) {
  return value == 0 ? 1 : value;
}
volatile double value_a = 4.0;
}  // namespace foo

TEST(NamespaceLookupTableByEnum, Basic) {
  constexpr auto table_f = LOOKUP_TABLE(foo::bar, "get_*", rfl::enum_cast<foo_get_fn_key>);
  static_assert(std::is_same_v<int (*)(int), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 3);

  CHECK_FUNCTION_STATIC(17, table_f[foo_get_fn_key::odd], 16);
  CHECK_FUNCTION_STATIC(18, table_f[foo_get_fn_key::even], 17);
  CHECK_FUNCTION_STATIC(19, table_f[foo_get_fn_key::abs], -19);
  EXPECT_EQ_STATIC(nullptr, table_f[foo_get_fn_key::non_zero]);
  EXPECT_EQ_STATIC(nullptr, table_f[static_cast<foo_get_fn_key>(123)]);

  constexpr auto table_v = LOOKUP_TABLE(foo::bar, "value_*", rfl::enum_cast<foo_value_key>);
  static_assert(std::is_same_v<volatile double*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(1.0, table_v[foo_value_key::x]);
  CHECK_VARIABLE(3.0, table_v[foo_value_key::z]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<foo_value_key>(-123)]);
}
