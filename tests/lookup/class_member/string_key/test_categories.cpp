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

struct foo_2_t {
  static inline double value_a = 123.0;
  static inline double value_b = 456.0;
  double value_x;
  double value_y;
  double value_z;

  static double value_sum(double x, double y) {
    return x + y;
  }
  static double value_product(double x, double y) {
    return x * y;
  }
  constexpr double value_sum() const {
    return value_x + value_y + value_z;
  }
  constexpr double value_fma() const {
    return value_x * value_y + value_z;
  }
  constexpr double value_product() const {
    return value_x * value_y * value_z;
  }
};

TEST(ClassLookupTableByName, NonStaticDataMembersOnly) {
  constexpr auto table_nv =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   {
                       .category = rfl::class_member_category::nonstatic_data_members,
                   });
  static_assert(std::is_same_v<double foo_2_t::*, decltype(table_nv)::value_type>);
  static_assert(table_nv.size() == 3);

  constexpr auto foo = foo_2_t{.value_x = 11, .value_y = 22, .value_z = 33};
  CHECK_MEMBER_VARIABLE_STATIC(11.0, foo, table_nv["x"]);
  CHECK_MEMBER_VARIABLE_STATIC(22.0, foo, table_nv["y"]);
  CHECK_MEMBER_VARIABLE_STATIC(33.0, foo, table_nv["z"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["a"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["sum"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["A"]);
}

TEST(ClassLookupTableByName, StaticDataMembersOnly) {
  constexpr auto table_nf =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   {
                       .category = rfl::class_member_category::nonstatic_member_functions,
                   });
  static_assert(std::is_same_v<double (foo_2_t::*)() const, decltype(table_nf)::value_type>);
  static_assert(table_nf.size() == 3);

  constexpr auto foo = foo_2_t{.value_x = 11, .value_y = 22, .value_z = 33};
  CHECK_MEMBER_FUNCTION_STATIC(66.0, foo, table_nf["sum"]);
  CHECK_MEMBER_FUNCTION_STATIC(275.0, foo, table_nf["fma"]);
  CHECK_MEMBER_FUNCTION_STATIC(7986.0, foo, table_nf["product"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["x"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["a"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["value_sum"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["valueProduct"]);
}

TEST(ClassLookupTableByName, NonStaticMemberFunctionsOnly) {
  constexpr auto table_sv =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   {
                       .category = rfl::class_member_category::static_data_members,
                   });
  static_assert(std::is_same_v<double*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 2);

  constexpr auto foo = foo_2_t{.value_x = 11, .value_y = 22, .value_z = 33};
  CHECK_VARIABLE(123.0, table_sv["a"]);
  CHECK_VARIABLE(456.0, table_sv["b"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["y"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["add"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["_a"]);
}

TEST(ClassLookupTableByName, StaticMemberFunctionsOnly) {
  constexpr auto table_sf =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   {
                       .category = rfl::class_member_category::static_member_functions,
                   });
  static_assert(std::is_same_v<double (*)(double, double), decltype(table_sf)::value_type>);
  static_assert(table_sf.size() == 2);

  constexpr auto foo = foo_2_t{.value_x = 11, .value_y = 22, .value_z = 33};
  CHECK_FUNCTION(15.0, table_sf["sum"], 6.0, 9.0);
  CHECK_FUNCTION(54.0, table_sf["product"], 6.0, 9.0);
  EXPECT_EQ_STATIC(nullptr, table_sf["fma"]);
  EXPECT_EQ_STATIC(nullptr, table_sf["b"]);
  EXPECT_EQ_STATIC(nullptr, table_sf["y"]);
  EXPECT_EQ_STATIC(nullptr, table_sf[""]);
}
