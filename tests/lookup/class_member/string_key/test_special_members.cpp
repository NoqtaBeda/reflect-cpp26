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

struct test_special_members_t {
  int x;
  int y;

  // Matched
  auto assign(const test_special_members_t& rhs) noexcept -> test_special_members_t& {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }

  // Filtered out due to type mismatch
  auto assign_without_noexcept(const test_special_members_t& rhs) -> test_special_members_t& {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }

  // Filtered out due to being template (not addressable)
  template <class T>
  auto assign_by(const T& rhs) noexcept -> test_special_members_t& {
    x = rhs;
    y = rhs;
    return *this;
  }

  // Filtered out due to being template (not addressable)
  template <class T>
  auto assign_by_enum(const test_special_members_t& rhs) noexcept -> test_special_members_t& {
    x = static_cast<T>(rhs.x);
    y = static_cast<T>(rhs.y);
    return *this;
  }

  bool operator==(const test_special_members_t& rhs) const noexcept = default;
};

TEST(ClassLookupTableByName, WithSpecialMembers) {
  using fn_signature = test_special_members_t&(const test_special_members_t&) noexcept;
  constexpr auto table_fn = LOOKUP_TABLE(
      test_special_members_t, [](std::meta::info member) -> std::optional<std::string> {
        if (!is_same_type(type_of(member), ^^fn_signature)) {
          return std::nullopt;
        }
        // Matches test_special_members_t::assign
        if (has_identifier(member)) {
          return std::string{std::meta::identifier_of(member)};
        }
        if (is_operator_function(member)) {
          // Matches the implicit copy assignment function:
          // test_special_members_t& operator=(const test_special_members_t&) noexcept;
          if (operator_of(member) == std::meta::op_equals) {
            return "operator=";
          }
          // Does NOT match operator== due to type mismatch
          // (const-qualified vs. non-const-qualified)
          if (operator_of(member) == std::meta::op_equals_equals) {
            return "operator==";
          }
        }
        // Expects nothing to match this fallback branch
        return "<invalid>";
      });
  using expected_value_type =
      test_special_members_t& (test_special_members_t::*)(const test_special_members_t&) noexcept;
  static_assert(std::is_same_v<expected_value_type, decltype(table_fn)::value_type>);
  EXPECT_EQ_STATIC(2, table_fn.size());

  auto foo = test_special_members_t{.x = 1, .y = 2};
  auto bar = test_special_members_t{};
  CHECK_MEMBER_FUNCTION(foo, bar, table_fn["operator="], foo);
  bar = {};
  CHECK_MEMBER_FUNCTION(foo, bar, table_fn["assign"], foo);
  EXPECT_EQ_STATIC(nullptr, table_fn["operator=="]);
  EXPECT_EQ_STATIC(nullptr, table_fn["<invalid>"]);
}
