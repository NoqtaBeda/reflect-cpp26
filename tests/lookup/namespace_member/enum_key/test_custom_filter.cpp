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
#include <reflect_cpp26/utils/identifier_naming.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

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
}  // namespace ptrs

enum class ptrs_key {
  first = 0,
  second = 1,
  third = 2,
  fourth = 3,
  fifth = 4,
};

TEST(NamespaceLookupTableByEnum, CustomFilter) {
  // (std::string_view identifier) -> std::optional<E>
  constexpr auto table_s =
      LOOKUP_TABLE(ptrs, [](std::string_view identifier) -> std::optional<ptrs_key> {
        if (identifier.length() != 2) {
          return std::nullopt;
        }
        auto k = identifier[1] - '0';
        if (k % 2 != 0) {
          return std::nullopt;  // -> {p2, p4, p6}
        }
        return rfl::enum_cast<ptrs_key>(k);  // p2 -> third, p4 -> fifth
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
  constexpr auto table_m =
      LOOKUP_TABLE(ptrs, [](std::meta::info member) -> std::optional<ptrs_key> {
        if (!is_pointer_type(type_of(member))) {
          return std::nullopt;  // Filters out
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
