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

struct foo_3_t {
  static constexpr int32_t a1 = 10;
  static constexpr int32_t a2 = 20;

  const int32_t a3;
  const int32_t a4;
  const int32_t a5;
  volatile int64_t a6;
};

TEST(ClassLookupTableByName, CustomFilter) {
  constexpr auto table_sv =
      LOOKUP_TABLE(foo_3_t, [](std::string_view identifier) -> std::optional<std::string> {
        if (identifier[1] <= '2') {
          return std::string{identifier};
        }
        return std::nullopt;
      });
  static_assert(std::is_same_v<const int32_t*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 2);

  CHECK_VARIABLE_STATIC(10, table_sv["a1"]);
  CHECK_VARIABLE_STATIC(20, table_sv["a2"]);
  EXPECT_EQ_STATIC(nullptr, table_sv[""]);
  EXPECT_EQ_STATIC(nullptr, table_sv["1"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["a4"]);

  constexpr auto table_nv =
      LOOKUP_TABLE(foo_3_t, [](std::meta::info member) -> std::optional<std::string> {
        // Note: you don't need to test whether member is non-addressable
        // (constructors, destructors, deleted, etc.) as they are already filtered
        // out by the internal implementation of reflect_cpp26.
        if (is_static_member(member) || is_volatile_type(type_of(member))) {
          return std::nullopt;
        }
        if (!has_identifier(member)) {
          return std::nullopt;
        }
        return std::string{std::meta::identifier_of(member).substr(1)};
      });
  static_assert(std::is_same_v<const int32_t foo_3_t::*, decltype(table_nv)::value_type>);
  static_assert(table_nv.size() == 3);

  auto foo = foo_3_t{.a3 = 1, .a4 = 3, .a5 = 6, .a6 = 10};
  CHECK_MEMBER_VARIABLE(1, foo, table_nv["3"]);
  CHECK_MEMBER_VARIABLE(3, foo, table_nv["4"]);
  CHECK_MEMBER_VARIABLE(6, foo, table_nv["5"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["a3"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["2"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["6"]);
}
