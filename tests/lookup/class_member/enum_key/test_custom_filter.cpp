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

TEST(ClassLookupTableByEnum, CustomFilter) {
  // (std::string_view identifier) -> std::optional<E>
  constexpr auto table_dump = LOOKUP_TABLE(
      operand_t, [](std::string_view identifier) -> std::optional<operand_dump_fn_key> {
        auto str = std::string{"operand_"} + identifier;
        return rfl::enum_cast<operand_dump_fn_key>(str);
      });
  static_assert(
      std::is_same_v<std::string (operand_t::*)() const, decltype(table_dump)::value_type>);
  static_assert(table_dump.size() == 2);

  auto foo = operand_t{.value = 42};
  CHECK_MEMBER_FUNCTION("value = 42", foo, table_dump[operand_dump_full]);
  CHECK_MEMBER_FUNCTION("42", foo, table_dump[operand_dump_value]);
  EXPECT_EQ_STATIC(nullptr, table_dump[static_cast<operand_dump_fn_key>(2)]);

  // (std::meta::info member) -> std::optional<E>
  constexpr auto table_op =
      LOOKUP_TABLE(operand_t, [](std::meta::info member) -> std::optional<std::meta::operators> {
        if (is_operator_function(member) && !is_assignment(member)) {
          return operator_of(member);
        }
        return std::nullopt;
      });
  static_assert(std::is_same_v<operand_t& (operand_t::*)(long), decltype(table_op)::value_type>);
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
