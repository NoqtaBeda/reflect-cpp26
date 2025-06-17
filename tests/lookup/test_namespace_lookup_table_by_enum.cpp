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
#include <bit>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/lookup.hpp>
#else
#include <reflect_cpp26/lookup/lookup_table_by_enum.hpp>
#endif

namespace rfl = reflect_cpp26;

#define LOOKUP_TABLE_BY_ENUM(...) \
  REFLECT_CPP26_NAMESPACE_LOOKUP_TABLE_BY_ENUM(__VA_ARGS__)
#define LOOKUP_TABLE_BY_ENUM_ALL(...) \
  REFLECT_CPP26_NAMESPACE_LOOKUP_TABLE_BY_ENUM_ALL(__VA_ARGS__)

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
} // namespace get_options
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
} // namespace bar
constexpr int get_non_zero(int value) {
  return value == 0 ? 1 : value;
}
volatile double value_a = 4.0;
} // namespace foo

TEST(NamespaceLookupTableByEnum, Basic)
{
  constexpr auto table_f = LOOKUP_TABLE_BY_ENUM(
    foo::bar, foo_get_fn_key, "get_", "");
  static_assert(std::is_same_v<int (*)(int), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 3);

  CHECK_FUNCTION_STATIC(17, table_f[foo_get_fn_key::odd], 16);
  CHECK_FUNCTION_STATIC(18, table_f[foo_get_fn_key::even], 17);
  CHECK_FUNCTION_STATIC(19, table_f[foo_get_fn_key::abs], -19);
  EXPECT_EQ_STATIC(nullptr, table_f[foo_get_fn_key::non_zero]);
  EXPECT_EQ_STATIC(nullptr, table_f[static_cast<foo_get_fn_key>(123)]);

  constexpr auto table_v = LOOKUP_TABLE_BY_ENUM(
    foo::bar, foo_value_key, "value_", "");
  static_assert(std::is_same_v<
    volatile double*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(1.0, table_v[foo_value_key::x]);
  CHECK_VARIABLE(3.0, table_v[foo_value_key::z]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<foo_value_key>(-123)]);
}

enum class baz_get_fn_key {
  odd = -1,
  even = 0,
  abs = 2,
  bit_one_count = 3,
};

enum class baz_value_key: uint64_t {
  x = std::numeric_limits<uint64_t>::min(),
  y = std::numeric_limits<uint64_t>::max(),
};

namespace baz {
namespace get_options_or_count {
constexpr int get_negative(int value) {
  return value >= 0 ? -1 : value;
}
volatile double value_w = -1.0;
} // namespace get_options_or_count
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
} // namespace baz

TEST(NamespaceLookupTableByName, FilterFlags)
{
  constexpr auto table_f = LOOKUP_TABLE_BY_ENUM_ALL(
    baz, baz_get_fn_key, 'f', "get_", "");
  static_assert(std::is_same_v<int (*)(int), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 4);

  CHECK_FUNCTION_STATIC(17, table_f[baz_get_fn_key::odd], 16);
  CHECK_FUNCTION_STATIC(18, table_f[baz_get_fn_key::even], 17);
  CHECK_FUNCTION_STATIC(19, table_f[baz_get_fn_key::abs], -19);
  CHECK_FUNCTION_STATIC(6, table_f[baz_get_fn_key::bit_one_count], 0b11101101);
  EXPECT_EQ_STATIC(nullptr, table_f[static_cast<baz_get_fn_key>(123)]);

  constexpr auto table_v = LOOKUP_TABLE_BY_ENUM_ALL(
    baz, baz_value_key, 'v', "get_", "_count");
  static_assert(std::is_same_v<double*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(1.0, table_v[baz_value_key::x]);
  CHECK_VARIABLE(2.0, table_v[baz_value_key::y]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<baz_value_key>(-123)]);
}

namespace ops {
enum class Opcode {
  ADD = 1,
  SUB = 2,
  BITWISE_AND = 4,
  BITWISE_OR = 8,
};

constexpr unsigned HandleAdd(unsigned x, unsigned y) {
  return x + y;
}
constexpr unsigned HandleSub(unsigned x, unsigned y) {
  return x - y;
}
constexpr unsigned HandleBitwiseAnd(unsigned x, unsigned y) {
  return x & y;
}
constexpr unsigned HandleBitwiseOr(unsigned x, unsigned y) {
  return x | y;
}
constexpr bool Handle(unsigned* dest, unsigned x, unsigned y, Opcode opcode)
{
  constexpr auto opcode_naming_rule =
    rfl::identifier_naming_rule::all_caps_snake_case;
  constexpr auto dispatch_table = LOOKUP_TABLE_BY_ENUM_ALL(
    ops, Opcode, "Handle", "", opcode_naming_rule);

  static_assert(std::is_same_v<
    unsigned (*)(unsigned, unsigned), decltype(dispatch_table)::value_type>);
  static_assert(dispatch_table.size() == 4);

  auto handle_fn = dispatch_table[opcode];
  if (dest == nullptr || handle_fn == nullptr) {
    return false;
  }
  *dest = handle_fn(x, y);
  return true;
}
} // namespace ops

TEST(NamespaceLookupTableByName, NameConversion)
{
  auto dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 15, 50, ops::Opcode::ADD));
  EXPECT_EQ(65u, dest);
  dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 15, 50, ops::Opcode::SUB));
  EXPECT_EQ(-35u, dest);
  dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 0b1010u, 0b1100u, ops::Opcode::BITWISE_AND));
  EXPECT_EQ(0b1000u, dest);
  dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 0b1010u, 0b1100u, ops::Opcode::BITWISE_OR));
  EXPECT_EQ(0b1110u, dest);

  EXPECT_FALSE_STATIC(ops::Handle(&dest, 1, 2, static_cast<ops::Opcode>(123)));
}

#define ANNOTATE(...) [[=(__VA_ARGS__)]]

namespace ptrs {
struct ps {};
namespace p0 {}

constexpr long values[6] = {10, 20, 30, 40, 50, 60};
ANNOTATE(0) const long* p1 = values;
const long* p2 = values + 1;
const long* p3 = values + 2;
ANNOTATE(1) const long* p4 = values + 3;
ANNOTATE(9) const long* p5 = values + 4;
ANNOTATE(2) const long* p6 = values + 5;
ANNOTATE(3) const long p_count = 6;
} // namespace ptrs

enum class ptrs_key {
  first = 0,
  second = 1,
  third = 2,
  fourth = 3,
  fifth = 4,
};

TEST(NamespaceLookupTableByEnum, CustomFilter)
{
  // (std::string_view identifier) -> std::optional<E>
  constexpr auto table_s = LOOKUP_TABLE_BY_ENUM(ptrs, ptrs_key,
    [](std::string_view identifier) -> std::optional<ptrs_key> {
      if (identifier.length() != 2) {
        return std::nullopt;
      }
      auto k = identifier[1] - '0';
      if (k % 2 != 0) {
        return std::nullopt; // -> {p2, p4, p6}
      }
      return rfl::enum_cast<ptrs_key>(k); // p2 -> third, p4 -> fifth
    });
  static_assert(std::is_same_v<const long**, decltype(table_s)::value_type>);
  static_assert(table_s.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_s[ptrs_key::third]);
  EXPECT_EQ(20, **table_s[ptrs_key::third]);
  ASSERT_NE_STATIC(nullptr, table_s[ptrs_key::fifth]);
  EXPECT_EQ(40, **table_s[ptrs_key::fifth]);
  EXPECT_EQ_STATIC(nullptr, table_s[ptrs_key::first]);
  EXPECT_EQ_STATIC(nullptr, table_s[ptrs_key::second]);
  EXPECT_EQ_STATIC(nullptr, table_s[ptrs_key::fourth]);
  EXPECT_EQ_STATIC(nullptr, table_s[static_cast<ptrs_key>(123)]);

  // (std::meta::info member) -> std::optional<E>
  constexpr auto table_m = LOOKUP_TABLE_BY_ENUM(ptrs, ptrs_key,
    [](std::meta::info member) -> std::optional<ptrs_key> {
      if (!is_pointer_type(type_of(member))) {
        return std::nullopt; // Filters out 
      }
      if (annotations_of(member).empty()) {
        return std::nullopt;
      }
      auto a = extract<int>(annotations_of(member)[0]);
      return rfl::enum_cast<ptrs_key>(a);
    });
  static_assert(std::is_same_v<const long**, decltype(table_m)::value_type>);
  static_assert(table_m.size() == 3);

  ASSERT_NE_STATIC(nullptr, table_m[ptrs_key::first]);
  EXPECT_EQ(10, **table_m[ptrs_key::first]);
  ASSERT_NE_STATIC(nullptr, table_m[ptrs_key::second]);
  EXPECT_EQ(40, **table_m[ptrs_key::second]);
  ASSERT_NE_STATIC(nullptr, table_m[ptrs_key::third]);
  EXPECT_EQ(60, **table_m[ptrs_key::third]);
  EXPECT_EQ_STATIC(nullptr, table_m[ptrs_key::fourth]);
  EXPECT_EQ_STATIC(nullptr, table_m[ptrs_key::fifth]);
  EXPECT_EQ_STATIC(nullptr, table_m[static_cast<ptrs_key>(123)]);
}
