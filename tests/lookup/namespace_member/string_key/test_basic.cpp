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

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

namespace foo {
namespace bar {
namespace get_options {
constexpr int get_negative(int value) {
  return value >= 0 ? -1 : value;
}
volatile double value_w = -1.0;
}  // namespace get_options

namespace {
constexpr int get_incremented(int value) {
  return value + 1;
}
volatile double value_v = -2.0;
}  // namespace
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
}  // namespace bar
constexpr int get_non_zero(int value) {
  return value == 0 ? 1 : value;
}
volatile double value_z = 3.0;
}  // namespace foo

TEST(NamespaceLookupTableByName, Basic) {
  constexpr auto table_f = LOOKUP_TABLE(foo::bar, "get_*");
  static_assert(std::is_same_v<int (*)(int), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 3);

  CHECK_FUNCTION_STATIC(17, table_f["odd"], 16);
  CHECK_FUNCTION_STATIC(18, table_f["even"], 17);
  CHECK_FUNCTION_STATIC(19, table_f["abs"], -19);
  EXPECT_EQ_STATIC(nullptr, table_f[""]);
  EXPECT_EQ_STATIC(nullptr, table_f["options"]);
  EXPECT_EQ_STATIC(nullptr, table_f["dummy"]);
  EXPECT_EQ_STATIC(nullptr, table_f["negative"]);
  EXPECT_EQ_STATIC(nullptr, table_f["incremented"]);
  EXPECT_EQ_STATIC(nullptr, table_f["non_zero"]);

  constexpr auto table_v = LOOKUP_TABLE(foo::bar, "value_*");
  static_assert(std::is_same_v<volatile double*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(1.0, table_v["x"]);
  CHECK_VARIABLE(2.0, table_v["y"]);
  EXPECT_EQ_STATIC(nullptr, table_v[""]);
  EXPECT_EQ_STATIC(nullptr, table_v["v"]);
  EXPECT_EQ_STATIC(nullptr, table_v["w"]);
  EXPECT_EQ_STATIC(nullptr, table_v["z"]);
  EXPECT_EQ_STATIC(nullptr, table_v["options"]);
  EXPECT_EQ_STATIC(nullptr, table_v["dummy"]);
}
