/**
 * Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
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

#include "tests/lookup/lookup_test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/lookup.hpp>
#else
#include <reflect_cpp26/lookup/lookup_table_by_enum.hpp>
#endif

#define LOOKUP_TABLE_BY_ENUM(...) \
  REFLECT_CPP26_TYPE_LOOKUP_TABLE_BY_ENUM(__VA_ARGS__)
#define LOOKUP_TABLE_BY_ENUM_ALL(...) \
  REFLECT_CPP26_TYPE_LOOKUP_TABLE_BY_ENUM_ALL(__VA_ARGS__)

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
  x, z, w,
};

TEST(ClassLookupTableByEnum, Basic)
{
  constexpr auto table_v = LOOKUP_TABLE_BY_ENUM(
    foo_1_t, foo_1_key, "value_", "");
  static_assert(std::is_same_v<int foo_1_t::*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  constexpr auto foo = foo_1_t{
    .value_x = 10, .value_y = 20, .value_z = 30, .size = 4};
  CHECK_MEMBER_VARIABLE_STATIC(10, foo, table_v[foo_1_key::x]);
  CHECK_MEMBER_VARIABLE_STATIC(30, foo, table_v[foo_1_key::z]);
  EXPECT_EQ_STATIC(nullptr, table_v[foo_1_key::w]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<foo_1_key>(-1)]);

  constexpr auto table_f = LOOKUP_TABLE_BY_ENUM(
    foo_1_t, foo_1_key, "get_", "_squared_plus_a");
  static_assert(std::is_same_v<
    int (foo_1_t::*)(int) const, decltype(table_f)::value_type>);
  static_assert(table_f.size() == 1);

  CHECK_MEMBER_FUNCTION_STATIC(105, foo, table_f[foo_1_key::x], 5);
  EXPECT_EQ_STATIC(nullptr, table_f[foo_1_key::z]);
  EXPECT_EQ_STATIC(nullptr, table_f[foo_1_key::w]);
  EXPECT_EQ_STATIC(nullptr, table_f[static_cast<foo_1_key>(-2)]);
}

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

enum class foo_2_value_key: unsigned {
  a = 0, b = 1, c = 4, x = 9, y = 16,
};
enum class foo_2_fn_key: unsigned {
  sum = 0,
  product = 10,
  min = 1000,
  max = 100000,
  fma = std::numeric_limits<unsigned>::max(),
};

TEST(ClassLookupTableByEnum, FilterFlags)
{
  constexpr auto table_nv = LOOKUP_TABLE_BY_ENUM(
    foo_2_t, foo_2_value_key, "nv", "value_", "");
  static_assert(std::is_same_v<
    double foo_2_t::*, decltype(table_nv)::value_type>);
  static_assert(table_nv.size() == 2);

  constexpr auto foo = foo_2_t{.value_x = 1.5, .value_y = 2.5, .value_z = 3.5};
  CHECK_MEMBER_VARIABLE_STATIC(1.5, foo, table_nv[foo_2_value_key::x]);
  CHECK_MEMBER_VARIABLE_STATIC(2.5, foo, table_nv[foo_2_value_key::y]);
  EXPECT_EQ_STATIC(nullptr, table_nv[foo_2_value_key::a]);
  EXPECT_EQ_STATIC(nullptr, table_nv[foo_2_value_key::b]);
  EXPECT_EQ_STATIC(nullptr, table_nv[foo_2_value_key::c]);
  EXPECT_EQ_STATIC(nullptr, table_nv[static_cast<foo_2_value_key>(5)]);

  constexpr auto table_sv = LOOKUP_TABLE_BY_ENUM(
    foo_2_t, foo_2_value_key, "sv", "value_", "");
  static_assert(std::is_same_v<double*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 2);

  CHECK_VARIABLE(123.0, table_sv[foo_2_value_key::a]);
  CHECK_VARIABLE(456.0, table_sv[foo_2_value_key::b]);
  EXPECT_EQ_STATIC(nullptr, table_sv[foo_2_value_key::c]);
  EXPECT_EQ_STATIC(nullptr, table_sv[foo_2_value_key::x]);
  EXPECT_EQ_STATIC(nullptr, table_sv[foo_2_value_key::y]);
  EXPECT_EQ_STATIC(nullptr, table_sv[static_cast<foo_2_value_key>(-5)]);

  constexpr auto table_nf = LOOKUP_TABLE_BY_ENUM(
    foo_2_t, foo_2_fn_key, "nf", "value_", "");
  static_assert(std::is_same_v<
    double (foo_2_t::*)() const, decltype(table_nf)::value_type>);
  static_assert(table_nf.size() == 3);

  CHECK_MEMBER_FUNCTION_STATIC(7.5, foo, table_nf[foo_2_fn_key::sum]);
  CHECK_MEMBER_FUNCTION_STATIC(13.125, foo, table_nf[foo_2_fn_key::product]);
  CHECK_MEMBER_FUNCTION_STATIC(7.25, foo, table_nf[foo_2_fn_key::fma]);
  EXPECT_EQ_STATIC(nullptr, table_nf[foo_2_fn_key::min]);
  EXPECT_EQ_STATIC(nullptr, table_nf[foo_2_fn_key::max]);
  EXPECT_EQ_STATIC(nullptr, table_nf[static_cast<foo_2_fn_key>(5)]);

  constexpr auto table_sf = LOOKUP_TABLE_BY_ENUM(
    foo_2_t, foo_2_fn_key, "sf", "value_", "");
  static_assert(std::is_same_v<
    double (*)(double, double), decltype(table_sf)::value_type>);
  static_assert(table_sf.size() == 2);

  CHECK_FUNCTION_STATIC(50.0, table_sf[foo_2_fn_key::sum], 20.0, 30.0);
  CHECK_FUNCTION_STATIC(600.0, table_sf[foo_2_fn_key::product], 20.0, 30.0);
  EXPECT_EQ_STATIC(nullptr, table_sf[foo_2_fn_key::min]);
  EXPECT_EQ_STATIC(nullptr, table_sf[foo_2_fn_key::max]);
  EXPECT_EQ_STATIC(nullptr, table_sf[foo_2_fn_key::fma]);
  EXPECT_EQ_STATIC(nullptr, table_sf[static_cast<foo_2_fn_key>(-1)]);
}

struct operand_t {
  long value;

  operand_t& operator+=(long x) {
    value += x;
    return *this;
  }
  operand_t& operator-=(long x) {
    value -= x;
    return *this;
  }
  operand_t& operator*=(long x) {
    value *= x;
    return *this;
  }
  operand_t& operator/=(long x) {
    value /= x;
    return *this;
  }
  auto dump_full() const -> std::string {
    return std::format("value = {}", value);
  }
  auto dump_value() const -> std::string {
    return std::format("{}", value);
  }
};

enum operand_dump_fn_key {
  operand_dump_full = -10,
  operand_dump_value = 10,
};

TEST(ClassLookupTableByEnum, CustomFilter)
{
  // (std::string_view identifier) -> std::optional<E>
  constexpr auto table_dump = LOOKUP_TABLE_BY_ENUM_ALL(
    operand_t, operand_dump_fn_key,
    [](std::string_view identifier) -> std::optional<operand_dump_fn_key> {
      auto str = std::string{"operand_"} + identifier;
      return rfl::enum_cast<operand_dump_fn_key>(str); 
    });
  static_assert(std::is_same_v<
    std::string (operand_t::*)() const, decltype(table_dump)::value_type>);
  static_assert(table_dump.size() == 2);

  auto foo = operand_t{.value = 42};
  CHECK_MEMBER_FUNCTION("value = 42", foo, table_dump[operand_dump_full]);
  CHECK_MEMBER_FUNCTION("42", foo, table_dump[operand_dump_value]);
  EXPECT_EQ_STATIC(nullptr, table_dump[static_cast<operand_dump_fn_key>(2)]);

  // (std::meta::info member) -> std::optional<E>
  constexpr auto table_op = LOOKUP_TABLE_BY_ENUM(
    operand_t, std::meta::operators,
    [](std::meta::info member) -> std::optional<std::meta::operators> {
      if (is_operator_function(member) && !is_assignment(member)) {
        return operator_of(member);
      }
      return std::nullopt;
    });
  static_assert(std::is_same_v<
    operand_t& (operand_t::*)(long), decltype(table_op)::value_type>);
  static_assert(table_op.size() == 4);

  foo.value = 42;
  ASSERT_NE_STATIC(nullptr, table_op[std::meta::op_plus_equals]);
  EXPECT_EQ(45, (foo.*table_op[std::meta::op_plus_equals])(3).value);
  foo.value = 42;
  ASSERT_NE_STATIC(nullptr, table_op[std::meta::op_minus_equals]);
  EXPECT_EQ(39, (foo.*table_op[std::meta::op_minus_equals])(3).value);
  foo.value = 42;
  ASSERT_NE_STATIC(nullptr, table_op[std::meta::op_star_equals]);
  EXPECT_EQ(126, (foo.*table_op[std::meta::op_star_equals])(3).value);
  foo.value = 42;
  ASSERT_NE_STATIC(nullptr, table_op[std::meta::op_slash_equals]);
  EXPECT_EQ(14, (foo.*table_op[std::meta::op_slash_equals])(3).value);

  EXPECT_EQ_STATIC(nullptr, table_op[std::meta::op_less]);
  EXPECT_EQ_STATIC(nullptr, table_op[std::meta::op_equals]);
  EXPECT_EQ_STATIC(nullptr, table_op[std::meta::op_co_await]);
}

struct A : std::monostate {
  long a_count;
};
struct B {
  long b_count;
};
struct C {
  long c_count;
};
struct D : A, B, C {
  enum key_t {
    a = std::numeric_limits<int64_t>::min(),
    b = 0,
    c = std::numeric_limits<int64_t>::max(),
  };

  static constexpr auto make(long a, long b, long c) -> D
  {
    auto res = D{};
    res.a_count = a;
    res.b_count = b;
    res.c_count = c;
    return res;
  }
};

TEST(ClassLookupTableByEnum, Inheritance)
{
  constexpr auto table_value =
    LOOKUP_TABLE_BY_ENUM_ALL(D, D::key_t, "", "_count");
  static_assert(std::is_same_v<long D::*, decltype(table_value)::value_type>);
  static_assert(table_value.size() == 3);

  constexpr auto d = D::make(1, 3, 5);
  CHECK_MEMBER_VARIABLE_STATIC(1, d, table_value[D::a]);
  CHECK_MEMBER_VARIABLE_STATIC(3, d, table_value[D::b]);
  CHECK_MEMBER_VARIABLE_STATIC(5, d, table_value[D::c]);
  EXPECT_EQ_STATIC(nullptr, table_value[static_cast<D::key_t>(-2)]);
}
