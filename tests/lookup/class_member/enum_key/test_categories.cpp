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

struct foo_2_t {
  static inline double value_a = 123.0;
  static inline double value_b = 456.0;
  double value_x;
  double value_y;
  double value_z;

  static constexpr double value_sum(double x, double y) {
    return x + y;
  }
  static constexpr double value_product(double x, double y) {
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

enum class foo_2_value_key : unsigned {
  a = 0,
  b = 1,
  c = 4,
  x = 9,
  y = 16,
};
enum class foo_2_fn_key : unsigned {
  sum = 0,
  product = 10,
  min = 1000,
  max = 100000,
  fma = std::numeric_limits<unsigned>::max(),
};

TEST(ClassLookupTableByEnum, NonStaticDataMembersOnly) {
  constexpr auto table_nv =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   rfl::enum_cast<foo_2_value_key>,
                   {
                       .category = rfl::class_member_category::nonstatic_data_members,
                   });
  static_assert(std::is_same_v<double foo_2_t::*, decltype(table_nv)::value_type>);
  static_assert(table_nv.size() == 2);

  constexpr auto foo = foo_2_t{.value_x = 1.5, .value_y = 2.5, .value_z = 3.5};
  CHECK_MEMBER_VARIABLE_STATIC(1.5, foo, table_nv[foo_2_value_key::x]);
  CHECK_MEMBER_VARIABLE_STATIC(2.5, foo, table_nv[foo_2_value_key::y]);
  EXPECT_EQ_STATIC(nullptr, table_nv[foo_2_value_key::a]);
  EXPECT_EQ_STATIC(nullptr, table_nv[foo_2_value_key::b]);
  EXPECT_EQ_STATIC(nullptr, table_nv[foo_2_value_key::c]);
  EXPECT_EQ_STATIC(nullptr, table_nv[static_cast<foo_2_value_key>(5)]);
}

TEST(ClassLookupTableByEnum, StaticDataMembersOnly) {
  constexpr auto table_sv =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   rfl::enum_cast<foo_2_value_key>,
                   {
                       .category = rfl::class_member_category::static_data_members,
                   });
  static_assert(std::is_same_v<double*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 2);

  constexpr auto foo = foo_2_t{.value_x = 1.5, .value_y = 2.5, .value_z = 3.5};
  CHECK_VARIABLE(123.0, table_sv[foo_2_value_key::a]);
  CHECK_VARIABLE(456.0, table_sv[foo_2_value_key::b]);
  EXPECT_EQ_STATIC(nullptr, table_sv[foo_2_value_key::c]);
  EXPECT_EQ_STATIC(nullptr, table_sv[foo_2_value_key::x]);
  EXPECT_EQ_STATIC(nullptr, table_sv[foo_2_value_key::y]);
  EXPECT_EQ_STATIC(nullptr, table_sv[static_cast<foo_2_value_key>(-5)]);
}

TEST(ClassLookupTableByEnum, NonStaticMemberFunctionsOnly) {
  constexpr auto table_nf =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   rfl::enum_cast<foo_2_fn_key>,
                   {
                       .category = rfl::class_member_category::nonstatic_member_functions,
                   });
  static_assert(std::is_same_v<double (foo_2_t::*)() const, decltype(table_nf)::value_type>);
  static_assert(table_nf.size() == 3);

  constexpr auto foo = foo_2_t{.value_x = 1.5, .value_y = 2.5, .value_z = 3.5};
  CHECK_MEMBER_FUNCTION_STATIC(7.5, foo, table_nf[foo_2_fn_key::sum]);
  CHECK_MEMBER_FUNCTION_STATIC(13.125, foo, table_nf[foo_2_fn_key::product]);
  CHECK_MEMBER_FUNCTION_STATIC(7.25, foo, table_nf[foo_2_fn_key::fma]);
  EXPECT_EQ_STATIC(nullptr, table_nf[foo_2_fn_key::min]);
  EXPECT_EQ_STATIC(nullptr, table_nf[foo_2_fn_key::max]);
  EXPECT_EQ_STATIC(nullptr, table_nf[static_cast<foo_2_fn_key>(5)]);
}

TEST(ClassLookupTableByEnum, StaticMemberFunctionsOnly) {
  constexpr auto table_sf =
      LOOKUP_TABLE(foo_2_t,
                   "value_*",
                   rfl::enum_cast<foo_2_fn_key>,
                   {
                       .category = rfl::class_member_category::static_member_functions,
                   });
  static_assert(std::is_same_v<double (*)(double, double), decltype(table_sf)::value_type>);
  static_assert(table_sf.size() == 2);

  constexpr auto foo = foo_2_t{.value_x = 1.5, .value_y = 2.5, .value_z = 3.5};
  CHECK_FUNCTION_STATIC(50.0, table_sf[foo_2_fn_key::sum], 20.0, 30.0);
  CHECK_FUNCTION_STATIC(600.0, table_sf[foo_2_fn_key::product], 20.0, 30.0);
  EXPECT_EQ_STATIC(nullptr, table_sf[foo_2_fn_key::min]);
  EXPECT_EQ_STATIC(nullptr, table_sf[foo_2_fn_key::max]);
  EXPECT_EQ_STATIC(nullptr, table_sf[foo_2_fn_key::fma]);
  EXPECT_EQ_STATIC(nullptr, table_sf[static_cast<foo_2_fn_key>(-1)]);
}
