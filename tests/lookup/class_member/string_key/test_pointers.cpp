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

struct test_pointers_t {
  static inline int values[] = {10, 20, 30, 40, 50, 60, 70, 80};

  static inline volatile int* p1 = values + 1;
  static inline volatile int* p2 = values + 3;
  static inline volatile int* p3 = values + 5;

  static inline volatile int** pp1 = &p1;
  static inline volatile int** pp2 = &p3;

  long a1;
  long a2;
  long a3;
  long a4;
  long test_pointers_t::* mp1;
  long test_pointers_t::* mp2;

  static inline long test_pointers_t::* static_mp1 = &test_pointers_t::a1;
  static inline long test_pointers_t::* static_mp2 = &test_pointers_t::a3;
};

TEST(ClassLookupTableByName, WithPointers) {
  constexpr auto table_pp = LOOKUP_TABLE(test_pointers_t, "pp*");
  static_assert(std::is_same_v<volatile int***, decltype(table_pp)::value_type>);
  static_assert(table_pp.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_pp["1"]);
  EXPECT_EQ(20, ***table_pp["1"]);
  ASSERT_NE_STATIC(nullptr, table_pp["2"]);
  EXPECT_EQ(60, ***table_pp["2"]);

  EXPECT_EQ_STATIC(nullptr, table_pp["p1"]);
  EXPECT_EQ_STATIC(nullptr, table_pp["pp2"]);
  EXPECT_EQ_STATIC(nullptr, table_pp["a1"]);
  EXPECT_EQ_STATIC(nullptr, table_pp["mp2"]);
  EXPECT_EQ_STATIC(nullptr, table_pp["static_mp1"]);

  constexpr auto table_mptr = LOOKUP_TABLE(test_pointers_t, "mp*");
  static_assert(
      std::is_same_v<long test_pointers_t::* test_pointers_t::*, decltype(table_mptr)::value_type>);
  static_assert(table_mptr.size() == 2);

  constexpr auto foo = test_pointers_t{.a1 = 10,
                                       .a2 = 20,
                                       .a3 = 30,
                                       .a4 = 40,
                                       .mp1 = &test_pointers_t::a2,
                                       .mp2 = &test_pointers_t::a4};
  ASSERT_NE_STATIC(nullptr, table_mptr["1"]);
  EXPECT_EQ_STATIC(20, foo.*(foo.*table_mptr["1"]));
  ASSERT_NE_STATIC(nullptr, table_mptr["2"]);
  EXPECT_EQ_STATIC(40, foo.*(foo.*table_mptr["2"]));

  EXPECT_EQ_STATIC(nullptr, table_mptr["p1"]);
  EXPECT_EQ_STATIC(nullptr, table_mptr["pp2"]);
  EXPECT_EQ_STATIC(nullptr, table_mptr["a1"]);
  EXPECT_EQ_STATIC(nullptr, table_mptr["mp2"]);
  EXPECT_EQ_STATIC(nullptr, table_mptr["static_mp1"]);

  constexpr auto table_static_mptr = LOOKUP_TABLE(test_pointers_t, "static_mp*");
  static_assert(std::is_same_v<long test_pointers_t::**, decltype(table_static_mptr)::value_type>);
  static_assert(table_static_mptr.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_static_mptr["1"]);
  EXPECT_EQ(10, foo.*(*table_static_mptr["1"]));
  ASSERT_NE_STATIC(nullptr, table_static_mptr["2"]);
  EXPECT_EQ(30, foo.*(*table_static_mptr["2"]));

  EXPECT_EQ_STATIC(nullptr, table_static_mptr["p1"]);
  EXPECT_EQ_STATIC(nullptr, table_static_mptr["pp2"]);
  EXPECT_EQ_STATIC(nullptr, table_static_mptr["a1"]);
  EXPECT_EQ_STATIC(nullptr, table_static_mptr["mp2"]);
  EXPECT_EQ_STATIC(nullptr, table_static_mptr["static_mp1"]);
}
