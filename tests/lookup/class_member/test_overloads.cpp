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
#include <reflect_cpp26/utils/ctype.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

template <class T>
struct point3d_t {
  T x;
  T y;
  T z;
};

enum class axis {
  x,
  y,
  z,
};

// Overload (2.1) in lookup_table.hpp
TEST(ClassLookupTableOverloads, Overload2_MetaInfo2Key) {
  constexpr auto options = rfl::class_member_lookup_table_options{
      .category = rfl::class_member_category::nonstatic_data_members,
  };
  // (std::meta::info) -> std::string_view
  constexpr auto table_v = LOOKUP_TABLE(point3d_t<int>, std::meta::identifier_of, options);
  static_assert(std::is_same_v<decltype(table_v)::value_type, int point3d_t<int>::*>);
  EXPECT_EQ_STATIC(3, table_v.size());

  constexpr auto pt = point3d_t<int>{.x = 12, .y = 34, .z = 56};
  CHECK_MEMBER_VARIABLE_STATIC(12, pt, table_v["x"]);
  CHECK_MEMBER_VARIABLE_STATIC(34, pt, table_v["y"]);
  CHECK_MEMBER_VARIABLE_STATIC(56, pt, table_v["z"]);
  EXPECT_EQ_STATIC(nullptr, table_v["w"]);
}

// Overload (2.1) in lookup_table.hpp
TEST(ClassLookupTableOverloads, Overload2_MetaInfo2OptionalKey) {
  constexpr auto options = rfl::class_member_lookup_table_options{
      .category = rfl::class_member_category::nonstatic_data_members,
  };
  // (std::meta::info) -> std::optional<axis>
  constexpr auto table_v = LOOKUP_TABLE(
      point3d_t<int>,
      [](std::meta::info member) { return rfl::enum_cast<axis>(identifier_of(member)); },
      options);
  static_assert(std::is_same_v<decltype(table_v)::value_type, int point3d_t<int>::*>);
  EXPECT_EQ_STATIC(3, table_v.size());

  constexpr auto pt = point3d_t<int>{.x = 12, .y = 34, .z = 56};
  CHECK_MEMBER_VARIABLE_STATIC(12, pt, table_v[axis::x]);
  CHECK_MEMBER_VARIABLE_STATIC(34, pt, table_v[axis::y]);
  CHECK_MEMBER_VARIABLE_STATIC(56, pt, table_v[axis::z]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<axis>(-1)]);
}

// Overload (2.1) in lookup_table.hpp
TEST(ClassLookupTableOverloads, Overload2_StringView2Key) {
  constexpr auto options = rfl::class_member_lookup_table_options{
      .category = rfl::class_member_category::nonstatic_data_members,
  };
  // (std::string_view) -> std::string
  constexpr auto table_v = LOOKUP_TABLE(point3d_t<int>, rfl::ascii_toupper, options);
  static_assert(std::is_same_v<decltype(table_v)::value_type, int point3d_t<int>::*>);
  EXPECT_EQ_STATIC(3, table_v.size());

  constexpr auto pt = point3d_t<int>{.x = 12, .y = 34, .z = 56};
  CHECK_MEMBER_VARIABLE_STATIC(12, pt, table_v["X"]);
  CHECK_MEMBER_VARIABLE_STATIC(34, pt, table_v["Y"]);
  CHECK_MEMBER_VARIABLE_STATIC(56, pt, table_v["Z"]);
  EXPECT_EQ_STATIC(nullptr, table_v["x"]);
}

// Overload (2.1) in lookup_table.hpp
TEST(ClassLookupTableOverloads, Overload2_StringView2OptionalKey) {
  constexpr auto options = rfl::class_member_lookup_table_options{
      .category = rfl::class_member_category::nonstatic_data_members,
  };
  // (std::string_view) -> std::optional<axis>
  constexpr auto table_v = LOOKUP_TABLE(point3d_t<int>, rfl::enum_cast<axis>, options);
  static_assert(std::is_same_v<decltype(table_v)::value_type, int point3d_t<int>::*>);
  EXPECT_EQ_STATIC(3, table_v.size());

  constexpr auto pt = point3d_t<int>{.x = 12, .y = 34, .z = 56};
  CHECK_MEMBER_VARIABLE_STATIC(12, pt, table_v[axis::x]);
  CHECK_MEMBER_VARIABLE_STATIC(34, pt, table_v[axis::y]);
  CHECK_MEMBER_VARIABLE_STATIC(56, pt, table_v[axis::z]);
  EXPECT_EQ_STATIC(nullptr, table_v[static_cast<axis>(-1)]);
}
