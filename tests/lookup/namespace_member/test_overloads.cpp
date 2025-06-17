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
#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/identifier_naming.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

inline namespace foobar {
constexpr int apple = 12;
constexpr int banana = 24;
constexpr int orange = 36;
constexpr int watermelon = 48;
constexpr int mango = 60;
}  // namespace foobar

// Overload (2.2) in lookup_table.hpp
TEST(NamespaceLookupTableOverloads, Overload2_MetaInfo2Key) {
  // (std::meta::info) -> std::string_view
  constexpr auto table_v = LOOKUP_TABLE(foobar, std::meta::identifier_of);
  static_assert(std::is_same_v<decltype(table_v)::value_type, const int*>);
  EXPECT_EQ_STATIC(5, table_v.size());

  CHECK_VARIABLE_STATIC(12, table_v["apple"]);
  CHECK_VARIABLE_STATIC(24, table_v["banana"]);
  CHECK_VARIABLE_STATIC(36, table_v["orange"]);
  CHECK_VARIABLE_STATIC(48, table_v["watermelon"]);
  CHECK_VARIABLE_STATIC(60, table_v["mango"]);
  EXPECT_EQ_STATIC(nullptr, table_v[""]);
  EXPECT_EQ_STATIC(nullptr, table_v["cat"]);
}

// Overload (2.2) in lookup_table.hpp
TEST(NamespaceLookupTableOverloads, Overload2_MetaInfo2OptionalKey) {
  // (std::meta::info) -> std::optional<std::string>
  constexpr auto table_v =
      LOOKUP_TABLE(foobar, [](std::meta::info member) -> std::optional<std::string> {
        return rfl::to_all_caps_snake_case_opt(identifier_of(member));
      });
  static_assert(std::is_same_v<decltype(table_v)::value_type, const int*>);
  EXPECT_EQ_STATIC(5, table_v.size());

  CHECK_VARIABLE_STATIC(12, table_v["APPLE"]);
  CHECK_VARIABLE_STATIC(24, table_v["BANANA"]);
  CHECK_VARIABLE_STATIC(36, table_v["ORANGE"]);
  CHECK_VARIABLE_STATIC(48, table_v["WATERMELON"]);
  CHECK_VARIABLE_STATIC(60, table_v["MANGO"]);
  EXPECT_EQ_STATIC(nullptr, table_v[""]);
  EXPECT_EQ_STATIC(nullptr, table_v["apple"]);
}

// Overload (2.2) in lookup_table.hpp
TEST(NamespaceLookupTableOverloads, Overload2_StringView2Key) {
  // (std::string_view) -> std::string
  constexpr auto table_v = LOOKUP_TABLE(foobar, rfl::ascii_toupper);
  static_assert(std::is_same_v<decltype(table_v)::value_type, const int*>);
  EXPECT_EQ_STATIC(5, table_v.size());

  CHECK_VARIABLE_STATIC(12, table_v["APPLE"]);
  CHECK_VARIABLE_STATIC(24, table_v["BANANA"]);
  CHECK_VARIABLE_STATIC(36, table_v["ORANGE"]);
  CHECK_VARIABLE_STATIC(48, table_v["WATERMELON"]);
  CHECK_VARIABLE_STATIC(60, table_v["MANGO"]);
  EXPECT_EQ_STATIC(nullptr, table_v[""]);
  EXPECT_EQ_STATIC(nullptr, table_v["apple"]);
}

// Overload (2.2) in lookup_table.hpp
TEST(NamespaceLookupTableOverloads, Overload2_StringView2OptionalKey) {
  // (std::string_view) -> std::optional<std::string>
  constexpr auto table_v = LOOKUP_TABLE(foobar, rfl::to_upper_camel_case_opt);
  static_assert(std::is_same_v<decltype(table_v)::value_type, const int*>);
  EXPECT_EQ_STATIC(5, table_v.size());

  CHECK_VARIABLE_STATIC(12, table_v["Apple"]);
  CHECK_VARIABLE_STATIC(24, table_v["Banana"]);
  CHECK_VARIABLE_STATIC(36, table_v["Orange"]);
  CHECK_VARIABLE_STATIC(48, table_v["Watermelon"]);
  CHECK_VARIABLE_STATIC(60, table_v["Mango"]);
  EXPECT_EQ_STATIC(nullptr, table_v[""]);
  EXPECT_EQ_STATIC(nullptr, table_v["apple"]);
  EXPECT_EQ_STATIC(nullptr, table_v["BANANA"]);
}
