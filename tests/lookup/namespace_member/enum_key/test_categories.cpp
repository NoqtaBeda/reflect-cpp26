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

enum class baz_get_fn_key {
  odd = -1,
  even = 0,
  abs = 2,
  bit_one_count = 3,
};

enum class baz_value_key : uint64_t {
  x = std::numeric_limits<uint64_t>::min(),
  y = std::numeric_limits<uint64_t>::max(),
};

namespace baz {
namespace get_options_or_count {
constexpr int get_negative(int value) {
  return value >= 0 ? -1 : value;
}
volatile double value_w = -1.0;
}  // namespace get_options_or_count
class get_dummy_count {};

constexpr int get_odd(int value) {
  return value % 2 == 0 ? value + 1 : value;
}
constexpr int get_even(int value) {
  return value % 2 == 0 ? value : value + 1;
}
constexpr int get_abs(int value) {
  return value >= 0 ? value : -value;
}
constexpr int get_bit_one_count(int value) {
  return std::popcount(static_cast<unsigned>(value));
}
double get_x_count = 1.0;
double get_y_count = 2.0;
}  // namespace baz

TEST(NamespaceLookupTableByName, FunctionsOnly) {
  constexpr auto table_f = LOOKUP_TABLE(baz, "get_*", rfl::enum_cast<baz_get_fn_key>);
  static_assert(std::is_same_v<int (*)(int), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 4);

  CHECK_FUNCTION_STATIC(17, table_f[baz_get_fn_key::odd], 16);
  CHECK_FUNCTION_STATIC(18, table_f[baz_get_fn_key::even], 17);
  CHECK_FUNCTION_STATIC(19, table_f[baz_get_fn_key::abs], -19);
  CHECK_FUNCTION_STATIC(6, table_f[baz_get_fn_key::bit_one_count], 0b11101101);
  EXPECT_EQ_STATIC(nullptr, table_f[static_cast<baz_get_fn_key>(123)]);
}

TEST(NamespaceLookupTableByName, VariablesOnly) {
  constexpr auto table_v = LOOKUP_TABLE(baz,
                                        "get_*_count",
                                        rfl::enum_cast<baz_value_key>,
                                        {
                                            .category = rfl::namespace_member_category::variables,
                                        });
  static_assert(std::is_same_v<double*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(1.0, table_v[baz_value_key::x]);
  CHECK_VARIABLE(2.0, table_v[baz_value_key::y]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<baz_value_key>(-123)]);
}
