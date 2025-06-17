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
#include <cstdio>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/lookup.hpp>
#else
#include <reflect_cpp26/lookup/lookup_table_by_name.hpp>
#endif

namespace rfl = reflect_cpp26;

#define LOOKUP_TABLE_BY_NAME(...) \
  REFLECT_CPP26_NAMESPACE_LOOKUP_TABLE_BY_NAME(__VA_ARGS__)

namespace foo {
namespace bar {
namespace get_options {
constexpr int get_negative(int value) {
  return value >= 0 ? -1 : value;
}
volatile double value_w = -1.0;
} // namespace get_options

namespace {
constexpr int get_incremented(int value) {
  return value + 1;
}
volatile double value_v = -2.0;
} // namespace
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
} // namespace bar
constexpr int get_non_zero(int value) {
  return value == 0 ? 1 : value;
}
volatile double value_z = 3.0;
} // namespace foo

TEST(NamespaceLookupTableByName, Basic)
{
  constexpr auto table_f = LOOKUP_TABLE_BY_NAME(foo::bar, "get_", "");
  static_assert(std::is_same_v<int (*)(int), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 3);

  CHECK_FUNCTION_STATIC(17, table_f["odd"], 16);
  CHECK_FUNCTION_STATIC(18, table_f["even"], 17);
  CHECK_FUNCTION_STATIC(19, table_f["abs"], -19);
  EXPECT_EQ_STATIC(nullptr, table_f[""]);
  EXPECT_EQ_STATIC(nullptr, table_f["options"]);
  EXPECT_EQ_STATIC(nullptr, table_f["dummy"]);
  EXPECT_EQ_STATIC(nullptr, table_f["negative"]);
  EXPECT_EQ_STATIC(nullptr, table_f["incremented"]);
  EXPECT_EQ_STATIC(nullptr, table_f["non_zero"]);

  constexpr auto table_v = LOOKUP_TABLE_BY_NAME(foo::bar, "value_", "");
  static_assert(std::is_same_v<
    volatile double*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(1.0, table_v["x"]);
  CHECK_VARIABLE(2.0, table_v["y"]);
  EXPECT_EQ_STATIC(nullptr, table_v[""]);
  EXPECT_EQ_STATIC(nullptr, table_v["v"]);
  EXPECT_EQ_STATIC(nullptr, table_v["w"]);
  EXPECT_EQ_STATIC(nullptr, table_v["z"]);
  EXPECT_EQ_STATIC(nullptr, table_v["options"]);
  EXPECT_EQ_STATIC(nullptr, table_v["dummy"]);
}

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
  constexpr auto table_f = LOOKUP_TABLE_BY_NAME(baz, 'f', "get_", "");
  static_assert(std::is_same_v<int (*)(int), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 4);

  CHECK_FUNCTION_STATIC(17, table_f["odd"], 16);
  CHECK_FUNCTION_STATIC(18, table_f["even"], 17);
  CHECK_FUNCTION_STATIC(19, table_f["abs"], -19);
  CHECK_FUNCTION_STATIC(6, table_f["bit_one_count"], 0b11101101);
  EXPECT_EQ_STATIC(nullptr, table_f[""]);
  EXPECT_EQ_STATIC(nullptr, table_f["options_or"]);
  EXPECT_EQ_STATIC(nullptr, table_f["options_or_count"]);
  EXPECT_EQ_STATIC(nullptr, table_f["dummy"]);
  EXPECT_EQ_STATIC(nullptr, table_f["dummy_count"]);
  EXPECT_EQ_STATIC(nullptr, table_f["bit_one"]);

  constexpr auto table_v = LOOKUP_TABLE_BY_NAME(baz, 'v', "", "_count");
  static_assert(std::is_same_v<double*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(1.0, table_v["get_x"]);
  CHECK_VARIABLE(2.0, table_v["get_y"]);
  EXPECT_EQ_STATIC(nullptr, table_v[""]);
  EXPECT_EQ_STATIC(nullptr, table_v["x"]);
  EXPECT_EQ_STATIC(nullptr, table_v["y"]);
  EXPECT_EQ_STATIC(nullptr, table_v["get_options_or"]);
  EXPECT_EQ_STATIC(nullptr, table_v["get_options_or_count"]);
  EXPECT_EQ_STATIC(nullptr, table_v["get_dummy"]);
  EXPECT_EQ_STATIC(nullptr, table_v["get_dummy_count"]);
}

namespace qux {
#define ANNOTATE(...) [[ =(__VA_ARGS__) ]]
constexpr long a = 1;
constexpr long b = 2;
constexpr long c = 3;
constexpr long d = 4;
ANNOTATE('?') constexpr long e = 5;

ANNOTATE('+')
constexpr long sum(long x, const long* y) {
  return y == nullptr ? x : x + *y;
}

ANNOTATE('-')
constexpr long subtract(long x, const long* y = nullptr) {
  return y == nullptr ? x : x - *y;
}
} // namespace qux

TEST(NamespaceLookupTableByName, CustomFilter)
{
  constexpr auto table_v = LOOKUP_TABLE_BY_NAME(qux,
    [](std::string_view identifier) -> std::optional<std::string> {
      if (identifier.length() == 1 && (identifier[0] - 'a') % 2 == 0) {
        return std::string{identifier};
      }
      return std::nullopt;
    });
  static_assert(std::is_same_v<const long*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 3);

  CHECK_VARIABLE_STATIC(1, table_v["a"]);
  CHECK_VARIABLE_STATIC(3, table_v["c"]);
  CHECK_VARIABLE_STATIC(5, table_v["e"]);
  EXPECT_EQ_STATIC(nullptr, table_v["b"]);
  EXPECT_EQ_STATIC(nullptr, table_v["d"]);

  constexpr auto table_f = LOOKUP_TABLE_BY_NAME(qux,
    [](std::meta::info member) -> std::optional<std::string> {
      if (is_variable(member) || annotations_of(member).size() != 1) {
        return std::nullopt;
      }
      auto annotation = extract<char>(annotations_of(member)[0]);
      return std::string(1zU, annotation);
    });
  static_assert(std::is_same_v<
    long (*)(long, const long*), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 2);

  constexpr auto y = 42L;
  CHECK_FUNCTION_STATIC(45, table_f["+"], 3, &y);
  CHECK_FUNCTION_STATIC(-39, table_f["-"], 3, &y);
  EXPECT_EQ_STATIC(nullptr, table_f["?"]);
  EXPECT_EQ_STATIC(nullptr, table_f["sum"]);
  EXPECT_EQ_STATIC(nullptr, table_f["subtract"]);
}

namespace test_template {
int x = 10;
int y = 20;

template <class T>
int count_of = 0;

constexpr long get_sum(long x, long y) {
  return x + y;
}

template <class T>
constexpr T get_sum_generic(T x, T y) {
  return x + y;
}

constexpr long get_product(long x, long y) {
  return x * y;
}
} // namespace test_template

TEST(NamespaceLookupTableByName, WithTemplates)
{
  constexpr auto table_v = LOOKUP_TABLE_BY_NAME(test_template, 'v', "", "");
  static_assert(std::is_same_v<int*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(10, table_v["x"]);
  CHECK_VARIABLE(20, table_v["y"]);
  EXPECT_EQ_STATIC(nullptr, table_v["count_of"]);

  constexpr auto table_f = LOOKUP_TABLE_BY_NAME(test_template, "get_", "");
  static_assert(std::is_same_v<
    long (*)(long, long), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 2);

  CHECK_FUNCTION(5, table_f["sum"], 2, 3);
  CHECK_FUNCTION(6, table_f["product"], 2, 3);
  EXPECT_EQ_STATIC(nullptr, table_f["sum_generic"]);
}

namespace test_deleted {
bool write_sum(int x, int y, int* dest) {
  if (dest != nullptr) {
    *dest = x + y;
    return true;
  }
  return false;
}

bool write_difference(int x, int y, int* dest) {
  if (dest != nullptr) {
    *dest = x - y;
    return true;
  }
  return false;
}

bool write_product(int x, int y, int* dest) = delete("Expected to be excluded");
} // namespace test_deleted

TEST(NamespaceLookupTableByName, WithDeleted)
{
  constexpr auto table_f = LOOKUP_TABLE_BY_NAME(test_deleted, "write_", "");
  static_assert(std::is_same_v<
    bool (*)(int, int, int*), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 2);

  int dest = 0;
  ASSERT_NE_STATIC(nullptr, table_f["sum"]);
  CHECK_FUNCTION(true, table_f["sum"], 2, 3, &dest);
  EXPECT_EQ(5, dest);
  dest = 0;
  ASSERT_NE_STATIC(nullptr, table_f["difference"]);
  CHECK_FUNCTION(true, table_f["difference"], 2, 3, &dest);
  EXPECT_EQ(-1, dest);

  EXPECT_EQ_STATIC(nullptr, table_f[""]);
  EXPECT_EQ_STATIC(nullptr, table_f["product"]);
  EXPECT_EQ_STATIC(nullptr, table_f["write_sum"]);
  EXPECT_EQ_STATIC(nullptr, table_f["write_product"]);
}

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
} // namespace test_pointer

TEST(NamespaceLookupTableByName, WithPointers)
{
  constexpr auto table_p = LOOKUP_TABLE_BY_NAME(test_pointer,
    [](std::string_view identifier) -> std::optional<std::string> {
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

  constexpr auto table_pp = LOOKUP_TABLE_BY_NAME(test_pointer, "pp", "");
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
 
  constexpr auto table_mp = LOOKUP_TABLE_BY_NAME(test_pointer, "mp", "");
  static_assert(std::is_same_v<
    int test_pointer::foo_t::* const*, decltype(table_mp)::value_type>);
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

  constexpr auto table_pmp = LOOKUP_TABLE_BY_NAME(test_pointer, 'v', "pmp", "");
  static_assert(std::is_same_v<
    int test_pointer::foo_t::* const* const*, decltype(table_pmp)::value_type>);
  static_assert(table_pmp.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_pmp["1"]);
  EXPECT_EQ_STATIC(24, foo.*(**table_pmp["1"]));
  ASSERT_NE_STATIC(nullptr, table_pmp["2"]);
  EXPECT_EQ_STATIC(48, foo.*(**table_pmp["2"]));
  EXPECT_EQ_STATIC(nullptr, table_pmp["3"]);
  EXPECT_EQ_STATIC(nullptr, table_pmp["p2"]);
  EXPECT_EQ_STATIC(nullptr, table_pmp["mp1"]);
  EXPECT_EQ_STATIC(nullptr, table_pmp["pmp2"]);

  constexpr auto table_fp = LOOKUP_TABLE_BY_NAME(test_pointer, "fp", "");
  static_assert(std::is_same_v<
    int (* const*)(const char*, ...), decltype(table_fp)::value_type>);
  static_assert(table_fp.size() == 2);

  ASSERT_NE_STATIC(nullptr, table_fp["1"]);
  EXPECT_EQ_STATIC(std::printf, *table_fp["1"]);
  ASSERT_NE_STATIC(nullptr, table_fp["1"]);
  EXPECT_EQ_STATIC(std::scanf, *table_fp["2"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["3"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["p2"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["fp1"]);
  EXPECT_EQ_STATIC(nullptr, table_fp["pfp2"]);

  constexpr auto table_pfp = LOOKUP_TABLE_BY_NAME(test_pointer, "pfp", "");
  static_assert(std::is_same_v<
    int (* const* const*)(const char*, ...), decltype(table_pfp)::value_type>);
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

namespace test_refs {
int v1 = 10;
int v2 = 20;
int v3 = 30;
int& ref1 = v1;
int& ref2 = v2;
int& ref3 = v3;
} // namespace test_refs

TEST(NamespaceLookupTableByName, WithReferences)
{
  constexpr auto table = LOOKUP_TABLE_BY_NAME(test_refs, "", "");
  static_assert(table.size() == 3);
  CHECK_VARIABLE(10, table["v1"]);
  CHECK_VARIABLE(20, table["v2"]);
  CHECK_VARIABLE(30, table["v3"]);
  EXPECT_EQ_STATIC(nullptr, table["ref1"]);
  EXPECT_EQ_STATIC(nullptr, table["ref2"]);
  EXPECT_EQ_STATIC(nullptr, table["ref3"]);
}
