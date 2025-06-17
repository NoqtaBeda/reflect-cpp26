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

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

namespace test_pointer {
struct foo_t {
  int x;
  int y;
  int z;
  int w;
};
struct AB {};

constexpr int values[] = {5, 15, 25, 35, 45, 55, 65, 75};
const int* p1 = values + 1;
const int* p2 = values + 3;
const int* p3 = values + 5;
const int* p4 = values + 7;
const int** pp1 = &p1;
const int** pp2 = &p3;
const int** pp3 = &p4;

constexpr auto mp1 = &foo_t::x;
constexpr auto mp2 = &foo_t::y;
constexpr auto mp3 = &foo_t::w;
constexpr auto pmp1 = &mp2;
constexpr auto pmp2 = &mp3;

constexpr auto fp1 = printf;
constexpr auto fp2 = scanf;
constexpr auto pfp1 = &fp1;
constexpr auto pfp2 = &fp2;
}  // namespace test_pointer

TEST(NamespaceLookupTableByName, WithPointers) {
  constexpr auto table_p =
      LOOKUP_TABLE(test_pointer, [](std::string_view identifier) -> std::optional<std::string> {
        if (identifier.length() == 2) {
          return std::string{"x"} + identifier[1];
        }
        return std::nullopt;
      });
  static_assert(std::is_same_v<const int**, decltype(table_p)::value_type>);
  static_assert(table_p.size() == 4);

  ASSERT_NE_STATIC(nullptr, table_p["x1"]);
  EXPECT_EQ(15, **table_p["x1"]);
  ASSERT_NE_STATIC(nullptr, table_p["x2"]);
  EXPECT_EQ(35, **table_p["x2"]);
  ASSERT_NE_STATIC(nullptr, table_p["x3"]);
  EXPECT_EQ(55, **table_p["x3"]);
  ASSERT_NE_STATIC(nullptr, table_p["x4"]);
  EXPECT_EQ(75, **table_p["x4"]);
  EXPECT_EQ_STATIC(nullptr, table_p["1"]);
  EXPECT_EQ_STATIC(nullptr, table_p["p2"]);

  constexpr auto table_pp = LOOKUP_TABLE(test_pointer, "pp*");
  static_assert(std::is_same_v<const int***, decltype(table_pp)::value_type>);
  static_assert(table_pp.size() == 3);

  ASSERT_NE_STATIC(nullptr, table_pp["1"]);
  EXPECT_EQ(15, ***table_pp["1"]);
  ASSERT_NE_STATIC(nullptr, table_pp["2"]);
  EXPECT_EQ(55, ***table_pp["2"]);
  ASSERT_NE_STATIC(nullptr, table_pp["3"]);
  EXPECT_EQ(75, ***table_pp["3"]);
  EXPECT_EQ_STATIC(nullptr, table_p["4"]);
  EXPECT_EQ_STATIC(nullptr, table_p["p1"]);
  EXPECT_EQ_STATIC(nullptr, table_p["pp2"]);

  constexpr auto table_mp = LOOKUP_TABLE(test_pointer, "mp*");
  static_assert(std::is_same_v<int test_pointer::foo_t::* const*, decltype(table_mp)::value_type>);
  static_assert(table_mp.size() == 3);

  constexpr auto foo = test_pointer::foo_t{.x = 12, .y = 24, .z = 36, .w = 48};
  ASSERT_NE_STATIC(nullptr, table_mp["1"]);
  EXPECT_EQ_STATIC(12, foo.*(*table_mp["1"]));
  ASSERT_NE_STATIC(nullptr, table_mp["2"]);
  EXPECT_EQ_STATIC(24, foo.*(*table_mp["2"]));
  ASSERT_NE_STATIC(nullptr, table_mp["3"]);
  EXPECT_EQ_STATIC(48, foo.*(*table_mp["3"]));
  EXPECT_EQ_STATIC(nullptr, table_mp["p1"]);
  EXPECT_EQ_STATIC(nullptr, table_mp["mp2"]);

  constexpr auto table_pmp = LOOKUP_TABLE(test_pointer,
                                          "pmp*",
                                          {
                                              .category = rfl::namespace_member_category::variables,
                                          });
  static_assert(
      std::is_same_v<int test_pointer::foo_t::* const* const*, decltype(table_pmp)::value_type>);
  static_assert(table_pmp.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_pmp["1"]);
  EXPECT_EQ_STATIC(24, foo.*(**table_pmp["1"]));
  ASSERT_NE_STATIC(nullptr, table_pmp["2"]);
  EXPECT_EQ_STATIC(48, foo.*(**table_pmp["2"]));
  EXPECT_EQ_STATIC(nullptr, table_pmp["3"]);
  EXPECT_EQ_STATIC(nullptr, table_pmp["p2"]);
  EXPECT_EQ_STATIC(nullptr, table_pmp["mp1"]);
  EXPECT_EQ_STATIC(nullptr, table_pmp["pmp2"]);

  constexpr auto table_fp = LOOKUP_TABLE(test_pointer, "fp*");
  static_assert(std::is_same_v<int (*const*)(const char*, ...), decltype(table_fp)::value_type>);
  static_assert(table_fp.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_fp["1"]);
  EXPECT_EQ_STATIC(std::printf, *table_fp["1"]);
  ASSERT_NE_STATIC(nullptr, table_fp["1"]);
  EXPECT_EQ_STATIC(std::scanf, *table_fp["2"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["3"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["p2"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["fp1"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["pfp2"]);

  constexpr auto table_pfp = LOOKUP_TABLE(test_pointer, "pfp*");
  static_assert(
      std::is_same_v<int (*const* const*)(const char*, ...), decltype(table_pfp)::value_type>);
  static_assert(table_pfp.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_pfp["1"]);
  EXPECT_EQ_STATIC(std::printf, **table_pfp["1"]);
  ASSERT_NE_STATIC(nullptr, table_pfp["2"]);
  EXPECT_EQ_STATIC(std::scanf, **table_pfp["2"]);
  EXPECT_EQ_STATIC(nullptr, table_pfp["3"]);
  EXPECT_EQ_STATIC(nullptr, table_pfp["p2"]);
  EXPECT_EQ_STATIC(nullptr, table_pfp["fp1"]);
  EXPECT_EQ_STATIC(nullptr, table_pfp["pfp2"]);
}
