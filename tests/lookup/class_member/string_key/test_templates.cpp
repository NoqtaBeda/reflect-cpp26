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
#include <reflect_cpp26/utils/to_string.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

struct with_templates_simple_t {
  template <class T>
  static constexpr size_t identifier_size = rfl::identifier_of(^^T).length();

  static constexpr size_t count_first = 42;
  static constexpr size_t count_second = 84;
};

TEST(ClassLookupTableByName, WithTemplates1) {
  constexpr auto table_sv = LOOKUP_TABLE(with_templates_simple_t, "*");
  static_assert(std::is_same_v<const size_t*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 2);

  CHECK_VARIABLE(42, table_sv["count_first"]);
  CHECK_VARIABLE(84, table_sv["count_second"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["count_third"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["count_fourth"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["identifier_size"]);
}

struct with_templates_t : with_templates_simple_t {
  template <class T>
  struct nested_template_t {};

  struct nested_class_t {};

  static constexpr size_t count_third = 126;
  static constexpr size_t count_fourth = 168;

  size_t x;
  size_t y;

  constexpr size_t get_sum(size_t z) const {
    return x + y + z;
  }
  constexpr size_t get_product(size_t z) const {
    return x * y * z;
  }
  template <class T>
  constexpr size_t get_max(T z) const {
    return std::max({x, y, static_cast<size_t>(z)});
  }

  static constexpr std::string sum_to_string(size_t x, size_t y) {
    return rfl::to_string(x + y);
  }
  static constexpr std::string product_to_string(size_t x, size_t y) {
    return rfl::to_string(x * y);
  }
  template <class T>
  static constexpr std::string max_to_string(T x, T y) {
    return rfl::to_string(std::max(x, y));
  }
};

TEST(ClassLookupTableByName, WithTemplates2) {
  constexpr auto table_nf =
      LOOKUP_TABLE(with_templates_t,
                   "*",
                   {
                       .category = rfl::class_member_category::nonstatic_member_functions,
                   });
  static_assert(
      std::is_same_v<size_t (with_templates_t::*)(size_t) const, decltype(table_nf)::value_type>);
  static_assert(table_nf.size() == 2);

  constexpr auto foo = with_templates_t{.x = 4, .y = 5};
  CHECK_MEMBER_FUNCTION_STATIC(15, foo, table_nf["get_sum"], 6);
  CHECK_MEMBER_FUNCTION_STATIC(120, foo, table_nf["get_product"], 6);
  EXPECT_EQ_STATIC(nullptr, table_nf["get_max"]);

  constexpr auto table_sv =
      LOOKUP_TABLE(with_templates_t,
                   "*",
                   {
                       .category = rfl::class_member_category::static_data_members,
                   });
  static_assert(std::is_same_v<const size_t*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 4);

  CHECK_VARIABLE(42, table_sv["count_first"]);
  CHECK_VARIABLE(84, table_sv["count_second"]);
  CHECK_VARIABLE(126, table_sv["count_third"]);
  CHECK_VARIABLE(168, table_sv["count_fourth"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["identifier_size"]);

  constexpr auto table_sf =
      LOOKUP_TABLE(with_templates_t,
                   "*",
                   {
                       .category = rfl::class_member_category::static_member_functions,
                   });
  static_assert(std::is_same_v<std::string (*)(size_t, size_t), decltype(table_sf)::value_type>);
  static_assert(table_sf.size() == 2);

  CHECK_FUNCTION_STATIC("5", table_sf["sum_to_string"], 2, 3);
  CHECK_FUNCTION_STATIC("6", table_sf["product_to_string"], 2, 3);
  EXPECT_EQ_STATIC(nullptr, table_sf["max_to_string"]);
}
