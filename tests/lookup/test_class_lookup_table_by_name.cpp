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
#include <reflect_cpp26/utils/to_string.hpp>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/lookup.hpp>
#else
#include <reflect_cpp26/lookup/lookup_table_by_name.hpp>
#endif

#define LOOKUP_TABLE_BY_NAME(...) \
  REFLECT_CPP26_TYPE_LOOKUP_TABLE_BY_NAME(__VA_ARGS__)

namespace rfl = reflect_cpp26;

struct foo_1_t {
  // Nested classes are expected to be filtered out.
  struct value_nested_squared_plus_a {};

  int value_x;
  int value_y;
  int value_z;
  int size;

  constexpr int get_x_squared_plus_a(int a) const {
    return value_x * value_x + a;
  }
  constexpr int get_y_squared_plus_a(int a) const {
    return value_y * value_y + a;
  }
  constexpr int get_z_plus_a(int a) const {
    return value_z + a;
  }
};

TEST(ClassLookupTableByName, Basic)
{
  constexpr auto table_value = LOOKUP_TABLE_BY_NAME(foo_1_t, "value_", "");
  static_assert(std::is_same_v<
    int foo_1_t::*, decltype(table_value)::value_type>);
  static_assert(table_value.size() == 3);

  constexpr auto foo = foo_1_t{1, 2, 3, 4};
  // Note: Prefix and suffix ignored during lookup.
  CHECK_MEMBER_VARIABLE_STATIC(1, foo, table_value["x"]);
  CHECK_MEMBER_VARIABLE_STATIC(2, foo, table_value["y"]);
  CHECK_MEMBER_VARIABLE_STATIC(3, foo, table_value["z"]);
  EXPECT_EQ_STATIC(nullptr, table_value["a"]);
  EXPECT_EQ_STATIC(nullptr, table_value["size"]);
  EXPECT_EQ_STATIC(nullptr, table_value["value_x"]);

  constexpr auto table_fn =
    LOOKUP_TABLE_BY_NAME(foo_1_t, "get_", "_squared_plus_a");
  static_assert(std::is_same_v<
    int (foo_1_t::*)(int) const, decltype(table_fn)::value_type>);
  static_assert(table_fn.size() == 2);

  CHECK_MEMBER_FUNCTION_STATIC(11, foo, table_fn["x"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(14, foo, table_fn["y"], 10);
  EXPECT_EQ_STATIC(nullptr, table_fn["z"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_x_squared_plus_a"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_z_plus_a"]);
}

struct foo_2_t {
  static inline double value_a = 123.0;
  static inline double value_b = 456.0;
  double value_x;
  double value_y;
  double value_z;

  static double value_sum(double x, double y) {
    return x + y;
  }
  static double value_product(double x, double y) {
    return x * y;
  }
  constexpr double value_sum() const {
    return value_x + value_y + value_z;
  }
  constexpr double value_fma() const {
    return value_x * value_y + value_z;
  }
  constexpr double value_product() const {
    return value_x * value_y * value_z;
  }
};

TEST(ClassLookupTableByName, FilterFlags)
{
  // nv: Non-static (member) Variable
  constexpr auto table_nv = LOOKUP_TABLE_BY_NAME(foo_2_t, "nv", "value_", "");
  static_assert(std::is_same_v<
    double foo_2_t::*, decltype(table_nv)::value_type>);
  static_assert(table_nv.size() == 3);

  constexpr auto foo = foo_2_t{.value_x = 11, .value_y = 22, .value_z = 33};
  CHECK_MEMBER_VARIABLE_STATIC(11.0, foo, table_nv["x"]);
  CHECK_MEMBER_VARIABLE_STATIC(22.0, foo, table_nv["y"]);
  CHECK_MEMBER_VARIABLE_STATIC(33.0, foo, table_nv["z"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["a"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["sum"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["A"]);

  // nf: Non-static (member) Function
  constexpr auto table_nf = LOOKUP_TABLE_BY_NAME(foo_2_t, "nf", "value_", "");
  static_assert(std::is_same_v<
    double (foo_2_t::*)() const, decltype(table_nf)::value_type>);
  static_assert(table_nf.size() == 3);

  CHECK_MEMBER_FUNCTION_STATIC(66.0, foo, table_nf["sum"]);
  CHECK_MEMBER_FUNCTION_STATIC(275.0, foo, table_nf["fma"]);
  CHECK_MEMBER_FUNCTION_STATIC(7986.0, foo, table_nf["product"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["x"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["a"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["value_sum"]);
  EXPECT_EQ_STATIC(nullptr, table_nf["valueProduct"]);

  // sv: Static (member) variable
  constexpr auto table_sv = LOOKUP_TABLE_BY_NAME(foo_2_t, "sv", "value_", "");
  static_assert(std::is_same_v<double*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 2);

  CHECK_VARIABLE(123.0, table_sv["a"]);
  CHECK_VARIABLE(456.0, table_sv["b"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["y"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["add"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["_a"]);

  // sf: Static (member) function
  constexpr auto table_sf = LOOKUP_TABLE_BY_NAME(foo_2_t, "sf", "value_", "");
  static_assert(std::is_same_v<
    double (*)(double, double), decltype(table_sf)::value_type>);
  static_assert(table_sf.size() == 2);

  CHECK_FUNCTION(15.0, table_sf["sum"], 6.0, 9.0);
  CHECK_FUNCTION(54.0, table_sf["product"], 6.0, 9.0);
  EXPECT_EQ_STATIC(nullptr, table_sf["fma"]);
  EXPECT_EQ_STATIC(nullptr, table_sf["b"]);
  EXPECT_EQ_STATIC(nullptr, table_sf["y"]);
  EXPECT_EQ_STATIC(nullptr, table_sf[""]);
}

struct foo_3_t {
  static constexpr int32_t a1 = 10;
  static constexpr int32_t a2 = 20;

  const int32_t a3;
  const int32_t a4;
  const int32_t a5;
  volatile int64_t a6;
};

TEST(ClassLookupTableByName, CustomFilter)
{
  constexpr auto table_sv = LOOKUP_TABLE_BY_NAME(foo_3_t,
    [](std::string_view identifier) -> std::optional<std::string> {
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

  constexpr auto table_nv = LOOKUP_TABLE_BY_NAME(foo_3_t,
    [](std::meta::info member) -> std::optional<std::string> {
      if (is_constructor(member) || is_destructor(member)) {
        return std::nullopt;
      }
      if (is_static_member(member) || is_volatile_type(type_of(member))) {
        return std::nullopt;
      }
      if (!has_identifier(member)) {
        return std::nullopt;
      }
      return std::string{identifier_of(member).substr(1)};
    });
  static_assert(std::is_same_v<
    const int32_t foo_3_t::*, decltype(table_nv)::value_type>);
  static_assert(table_nv.size() == 3);

  auto foo = foo_3_t{.a3 = 1, .a4 = 3, .a5 = 6, .a6 = 10};
  CHECK_MEMBER_VARIABLE(1, foo, table_nv["3"]);
  CHECK_MEMBER_VARIABLE(3, foo, table_nv["4"]);
  CHECK_MEMBER_VARIABLE(6, foo, table_nv["5"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["a3"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["2"]);
  EXPECT_EQ_STATIC(nullptr, table_nv["6"]);
}

struct bar_1_t : foo_1_t {
  int value_a;
  int value_b;
  int value_c;
  int count;

  constexpr int get_z_squared_plus_a(int a) const {
    return value_z * value_z + a;
  }
  constexpr int get_a_squared_plus_a(int a) const {
    return value_a * value_a + a;
  }
};

TEST(ClassLookupTableByName, Inheritance1)
{
  constexpr auto table_value = LOOKUP_TABLE_BY_NAME(bar_1_t, "value_", "");
  static_assert(std::is_same_v<
    int bar_1_t::*, decltype(table_value)::value_type>);
  static_assert(table_value.size() == 6);

  constexpr auto bar = bar_1_t{{1, 2, 3, 4}, 5, 6, 7};
  CHECK_MEMBER_VARIABLE_STATIC(1, bar, table_value["x"]);
  CHECK_MEMBER_VARIABLE_STATIC(2, bar, table_value["y"]);
  CHECK_MEMBER_VARIABLE_STATIC(3, bar, table_value["z"]);
  CHECK_MEMBER_VARIABLE_STATIC(5, bar, table_value["a"]);
  CHECK_MEMBER_VARIABLE_STATIC(6, bar, table_value["b"]);
  CHECK_MEMBER_VARIABLE_STATIC(7, bar, table_value["c"]);

  EXPECT_EQ_STATIC(nullptr, table_value["count"]);
  EXPECT_EQ_STATIC(nullptr, table_value["value_a"]);
  EXPECT_EQ_STATIC(nullptr, table_value["value_x"]);

  constexpr auto table_fn =
    LOOKUP_TABLE_BY_NAME(bar_1_t, "get_", "_squared_plus_a");
  static_assert(std::is_same_v<
    int (bar_1_t::*)(int) const, decltype(table_fn)::value_type>);
  static_assert(table_fn.size() == 4);

  CHECK_MEMBER_FUNCTION_STATIC(11, bar, table_fn["x"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(14, bar, table_fn["y"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(19, bar, table_fn["z"], 10);
  CHECK_MEMBER_FUNCTION_STATIC(35, bar, table_fn["a"], 10);

  EXPECT_EQ_STATIC(nullptr, table_fn["b"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_a_squared_plus_a"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["get_z_plus_a"]);
}

struct A : std::monostate {
  long a_count;
};
struct B {
  long b_count;
};
struct C {
  long c_count;
};
struct D : A, B, C {
  static constexpr auto make(long a, long b, long c) -> D
  {
    auto res = D{};
    res.a_count = a;
    res.b_count = b;
    res.c_count = c;
    return res;
  }
};

TEST(ClassLookupTableByName, Inheritance2)
{
  constexpr auto table_value = LOOKUP_TABLE_BY_NAME(D, "", "_count");
  static_assert(std::is_same_v<long D::*, decltype(table_value)::value_type>);
  static_assert(table_value.size() == 3);

  constexpr auto d = D::make(1, 3, 5);
  CHECK_MEMBER_VARIABLE_STATIC(1, d, table_value["a"]);
  CHECK_MEMBER_VARIABLE_STATIC(3, d, table_value["b"]);
  CHECK_MEMBER_VARIABLE_STATIC(5, d, table_value["c"]);
  EXPECT_EQ_STATIC(nullptr, table_value["a_count"]);
  EXPECT_EQ_STATIC(nullptr, table_value["d"]);
  EXPECT_EQ_STATIC(nullptr, table_value[""]);
}

namespace test_access_context_1 {
struct A {
  explicit constexpr A(double base)
    : pub_a1(base),
      pub_a2(base + 1.0),
      pub_a3(base + 2.0),
      prot_a1(base + 3.0),
      prot_a2(base + 4.0),
      priv_a1(base + 5.0) {}

  constexpr bool get_value_from_A(std::string_view key, double* dest) const
  {
    constexpr auto table = LOOKUP_TABLE_BY_NAME(A, "nv", "", "");
    static_assert(std::is_same_v<double A::*, decltype(table)::value_type>);
    static_assert(table.size() == 6);

    auto mptr = table[key];
    if (mptr == nullptr || dest == nullptr) {
      return false;
    }
    *dest = this->*mptr;
    return true;
  }

public:
  double pub_a1;
  double pub_a2;
  double pub_a3;
protected:
  double prot_a1;
  double prot_a2;
private:
  double priv_a1;
};

struct B : A {
  explicit constexpr B(double base)
    : A(base),
      pub_b1(base + 10.0),
      pub_b2(base + 11.0),
      prot_b1(base + 12.0),
      priv_b1(base + 13.0) {}

  constexpr bool get_value_from_B(std::string_view key, double* dest) const
  {
    constexpr auto table = LOOKUP_TABLE_BY_NAME(B, "nv", "", "");
    static_assert(std::is_same_v<double B::*, decltype(table)::value_type>);
    // static_assert(table.size() == 9);

    auto mptr = table[key];
    if (mptr == nullptr || dest == nullptr) {
      return false;
    }
    *dest = this->*mptr;
    return true;
  }

public:
  double pub_b1;
  double pub_b2;
protected:
  double prot_b1;
private:
  double priv_b1;

  // All the members that are expected to be accessible in lookup table.
  static constexpr double B::* p1 = &B::pub_a1;
  static constexpr double B::* p2 = &B::pub_a2;
  static constexpr double B::* p3 = &B::pub_a3;
  static constexpr double B::* p4 = &B::pub_b1;
  static constexpr double B::* p5 = &B::pub_b2;
  static constexpr double B::* p6 = &B::prot_a1;
  static constexpr double B::* p7 = &B::prot_a2;
  static constexpr double B::* p8 = &B::prot_b1;
  static constexpr double B::* p9 = &B::priv_b1;
};
} // namespace test_access_context_1

TEST(ClassLookupTableByName, AccessContext1)
{
  constexpr auto a = test_access_context_1::A(10.0);
  constexpr auto b = test_access_context_1::B(10.0);

  auto test_get_value_from_A = [](const test_access_context_1::A& obj) {
    auto dest = 0.0;
    ASSERT_TRUE(obj.get_value_from_A("pub_a1", &dest));
    EXPECT_EQ(10.0, dest);
    ASSERT_TRUE(obj.get_value_from_A("pub_a2", &dest));
    EXPECT_EQ(11.0, dest);
    ASSERT_TRUE(obj.get_value_from_A("pub_a3", &dest));
    EXPECT_EQ(12.0, dest);
    ASSERT_TRUE(obj.get_value_from_A("priv_a1", &dest));
    EXPECT_EQ(15.0, dest);
    ASSERT_TRUE(obj.get_value_from_A("prot_a1", &dest));
    EXPECT_EQ(13.0, dest);
    ASSERT_TRUE(obj.get_value_from_A("prot_a2", &dest));
    EXPECT_EQ(14.0, dest);
    EXPECT_FALSE(obj.get_value_from_A("pub_b1", &dest));
    EXPECT_FALSE(obj.get_value_from_A("prot_b1", &dest));
    EXPECT_FALSE(obj.get_value_from_A("priv_b1", &dest));
  };
  auto test_get_value_from_B = [](const test_access_context_1::B& obj) {
    auto dest = 0.0;
    ASSERT_TRUE(obj.get_value_from_B("pub_a1", &dest));
    EXPECT_EQ(10.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("pub_a2", &dest));
    EXPECT_EQ(11.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("pub_a3", &dest));
    EXPECT_EQ(12.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("prot_a1", &dest));
    EXPECT_EQ(13.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("prot_a2", &dest));
    EXPECT_EQ(14.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("pub_b1", &dest));
    EXPECT_EQ(20.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("pub_b2", &dest));
    EXPECT_EQ(21.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("prot_b1", &dest));
    EXPECT_EQ(22.0, dest);
    ASSERT_TRUE(obj.get_value_from_B("priv_b1", &dest));
    EXPECT_EQ(23.0, dest);
    EXPECT_FALSE(obj.get_value_from_B("priv_a1", &dest));
  };
  test_get_value_from_A(a);
  test_get_value_from_A(b);
  test_get_value_from_B(b);

  constexpr auto table_public = LOOKUP_TABLE_BY_NAME(
    test_access_context_1::B, "nv", "", "");
  static_assert(std::is_same_v<
    double test_access_context_1::B::*, decltype(table_public)::value_type>);
  static_assert(table_public.size() == 5);

  auto test_access_public =
    [](const test_access_context_1::B& obj, const auto& table) constexpr {
      auto dest = 0.0;
      CHECK_MEMBER_VARIABLE(10.0, obj, table["pub_a1"]);
      CHECK_MEMBER_VARIABLE(11.0, obj, table["pub_a2"]);
      CHECK_MEMBER_VARIABLE(12.0, obj, table["pub_a3"]);
      CHECK_MEMBER_VARIABLE(20.0, obj, table["pub_b1"]);
      CHECK_MEMBER_VARIABLE(21.0, obj, table["pub_b2"]);
    };
  test_access_public(b, table_public);
  EXPECT_EQ_STATIC(nullptr, table_public["prot_a1"]);
  EXPECT_EQ_STATIC(nullptr, table_public["prot_a2"]);
  EXPECT_EQ_STATIC(nullptr, table_public["priv_a1"]);
  EXPECT_EQ_STATIC(nullptr, table_public["prot_b1"]);
  EXPECT_EQ_STATIC(nullptr, table_public["priv_b1"]);

  constexpr auto table_all = LOOKUP_TABLE_BY_NAME(
    test_access_context_1::B, "nv", "", "", rfl::unchecked_context());
  static_assert(std::is_same_v<
    double test_access_context_1::B::*, decltype(table_all)::value_type>);
  static_assert(table_all.size() == 10);

  test_access_public(b, table_all);
  CHECK_MEMBER_VARIABLE_STATIC(13.0, b, table_all["prot_a1"]);
  CHECK_MEMBER_VARIABLE_STATIC(14.0, b, table_all["prot_a2"]);
  CHECK_MEMBER_VARIABLE_STATIC(15.0, b, table_all["priv_a1"]);
  CHECK_MEMBER_VARIABLE_STATIC(22.0, b, table_all["prot_b1"]);
  CHECK_MEMBER_VARIABLE_STATIC(23.0, b, table_all["priv_b1"]);
}

struct with_templates_simple_t {
  template <class T>
  static constexpr size_t identifier_size = identifier_of(^^T).length();

  static constexpr size_t count_first = 42;
  static constexpr size_t count_second = 84;
};

TEST(ClassLookupTableByName, WithTemplates1)
{
  constexpr auto table_sv = LOOKUP_TABLE_BY_NAME(
    with_templates_simple_t, "", "");
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

TEST(ClassLookupTableByName, WithTemplates2)
{
  constexpr auto table_nf = LOOKUP_TABLE_BY_NAME(
    with_templates_t, "nf", "", "");
  static_assert(std::is_same_v<
    size_t (with_templates_t::*)(size_t) const,
    decltype(table_nf)::value_type>);
  static_assert(table_nf.size() == 2);

  constexpr auto foo = with_templates_t{.x = 4, .y = 5};
  CHECK_MEMBER_FUNCTION_STATIC(15, foo, table_nf["get_sum"], 6);
  CHECK_MEMBER_FUNCTION_STATIC(120, foo, table_nf["get_product"], 6);
  EXPECT_EQ_STATIC(nullptr, table_nf["get_max"]);

  constexpr auto table_sv = LOOKUP_TABLE_BY_NAME(
    with_templates_t, "sv", "", "");
  static_assert(std::is_same_v<const size_t*, decltype(table_sv)::value_type>);
  static_assert(table_sv.size() == 4);

  CHECK_VARIABLE(42, table_sv["count_first"]);
  CHECK_VARIABLE(84, table_sv["count_second"]);
  CHECK_VARIABLE(126, table_sv["count_third"]);
  CHECK_VARIABLE(168, table_sv["count_fourth"]);
  EXPECT_EQ_STATIC(nullptr, table_sv["identifier_size"]);

  constexpr auto table_sf = LOOKUP_TABLE_BY_NAME(
    with_templates_t, "sf", "", "");
  static_assert(std::is_same_v<
    std::string (*)(size_t, size_t), decltype(table_sf)::value_type>);
  static_assert(table_sf.size() == 2);

  CHECK_FUNCTION_STATIC("5", table_sf["sum_to_string"], 2, 3);
  CHECK_FUNCTION_STATIC("6", table_sf["product_to_string"], 2, 3);
  EXPECT_EQ_STATIC(nullptr, table_sf["max_to_string"]);
}

struct test_deleted_t {
  int x;
  int y;

  constexpr int min() const {
    return std::min(x, y);
  }

  int min_with(int z) const = delete;

  constexpr int max() const {
    return std::max(x, y);
  }

  int max_with(int z) const = delete;
};

TEST(ClassLookupTableByName, WithDeleted)
{
  constexpr auto table_fn = LOOKUP_TABLE_BY_NAME(test_deleted_t, "nf", "", "");
  static_assert(std::is_same_v<
    int (test_deleted_t::*)() const, decltype(table_fn)::value_type>);
  static_assert(table_fn.size() == 2);

  constexpr auto foo = test_deleted_t{.x = 12, .y = 34};
  CHECK_MEMBER_FUNCTION_STATIC(12, foo, table_fn["min"]);
  CHECK_MEMBER_FUNCTION_STATIC(34, foo, table_fn["max"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["min_with"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["max_with"]);
}

struct test_special_members_t {
  int x;
  int y;

  auto assign(const test_special_members_t& rhs) -> test_special_members_t& {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }

  template <class T>
  auto assign_by(const T& rhs) -> test_special_members_t& {
    x = rhs;
    y = rhs;
    return *this;
  }

  template <class T>
  auto assign_by_enum(const test_special_members_t& rhs)
    -> test_special_members_t&
  {
    x = static_cast<T>(rhs.x);
    y = static_cast<T>(rhs.y);
    return *this;
  }

  bool operator==(const test_special_members_t& rhs) const = default;
};

TEST(ClassLookupTableByName, WithSpecialMembers)
{
  using fn_signature = test_special_members_t& (const test_special_members_t&);
  constexpr auto table_fn = LOOKUP_TABLE_BY_NAME(
    test_special_members_t,
    [](std::meta::info member) -> std::optional<std::string> {
      if (!is_same_type(type_of(member), ^^fn_signature)) {
        return std::nullopt;
      }
      if (has_identifier(member)) {
        return std::string{identifier_of(member)};
      }
      if (is_operator_function(member)
          && operator_of(member) == std::meta::op_equals) {
        return "operator=";
      }
      return "<invalid>";
    });
  using expected_value_type =
    test_special_members_t& (test_special_members_t::*)(
      const test_special_members_t&);
  static_assert(std::is_same_v<
    expected_value_type, decltype(table_fn)::value_type>);
  static_assert(table_fn.size() == 2);

  auto foo = test_special_members_t{.x = 1, .y = 2};
  auto bar = test_special_members_t{};
  CHECK_MEMBER_FUNCTION(foo, bar, table_fn["operator="], foo);
  bar = {};
  CHECK_MEMBER_FUNCTION(foo, bar, table_fn["assign"], foo);
  EXPECT_EQ_STATIC(nullptr, table_fn["<invalid>"]);
}

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

TEST(ClassLookupTableByName, WithPointers)
{
  constexpr auto table_pp = LOOKUP_TABLE_BY_NAME(test_pointers_t, "pp", "");
  static_assert(std::is_same_v<
    volatile int***, decltype(table_pp)::value_type>);
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

  constexpr auto table_mptr = LOOKUP_TABLE_BY_NAME(test_pointers_t, "mp", "");
  static_assert(std::is_same_v<
    long test_pointers_t::* test_pointers_t::*,
    decltype(table_mptr)::value_type>);
  static_assert(table_mptr.size() == 2);

  constexpr auto foo = test_pointers_t{
    .a1 = 10, .a2 = 20, .a3 = 30, .a4 = 40,
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

  constexpr auto table_static_mptr = LOOKUP_TABLE_BY_NAME(
    test_pointers_t, "static_mp", "");
  static_assert(std::is_same_v<
    long test_pointers_t::**, decltype(table_static_mptr)::value_type>);
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

struct with_ref_and_bf_t {
  const unsigned value1;
  const unsigned value2;

  const unsigned& ref1;
  const unsigned& ref2;

  const unsigned bf1: 5;
  const unsigned bf2: 10;
  const unsigned bf3: 15;
};

TEST(ClassLookupTableByName, WithReferencesAndBitFields)
{
  constexpr auto table = LOOKUP_TABLE_BY_NAME(with_ref_and_bf_t, "", "");
  static_assert(std::is_same_v<
    const unsigned with_ref_and_bf_t::*, decltype(table)::value_type>);
  static_assert(table.size() == 2);

  static constexpr auto v1 = 12u;
  static constexpr auto v2 = 24u;
  constexpr auto foo = with_ref_and_bf_t{
    .value1 = 2, .value2 = 4,
    .ref1 = v1, .ref2 = v2,
    .bf1 = 5, .bf2 = 7, .bf3 = 11};

  CHECK_MEMBER_VARIABLE_STATIC(2, foo, table["value1"]);
  CHECK_MEMBER_VARIABLE_STATIC(4, foo, table["value2"]);
  EXPECT_EQ_STATIC(nullptr, table["ref1"]);
  EXPECT_EQ_STATIC(nullptr, table["ref2"]);
  EXPECT_EQ_STATIC(nullptr, table["bf1"]);
  EXPECT_EQ_STATIC(nullptr, table["bf2"]);
  EXPECT_EQ_STATIC(nullptr, table["bf3"]);
}
