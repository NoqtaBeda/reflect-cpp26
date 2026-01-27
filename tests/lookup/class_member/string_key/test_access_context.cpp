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

struct A {
  explicit constexpr A(double base)
      : pub_a1(base),
        pub_a2(base + 1.0),
        pub_a3(base + 2.0),
        prot_a1(base + 3.0),
        prot_a2(base + 4.0),
        priv_a1(base + 5.0) {}

  constexpr bool get_value_from_A(std::string_view key, double* dest) const {
    // In current context: All the 6 non-static data members of struct A are accessible.
    constexpr auto table_options = rfl::class_member_lookup_table_options{
        .category = rfl::class_member_category::nonstatic_data_members,
    };
    constexpr auto table = LOOKUP_TABLE(A, "*", table_options);
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

  constexpr bool get_value_from_B(std::string_view key, double* dest) const {
    // Accessible members in current context:
    // * All non-static data members defined in struct B
    // * Public & protected non-static data members defined in struct A
    constexpr auto table_options = rfl::class_member_lookup_table_options{
        .category = rfl::class_member_category::nonstatic_data_members,
    };
    constexpr auto table = LOOKUP_TABLE(B, "*", table_options);
    static_assert(std::is_same_v<double B::*, decltype(table)::value_type>);
    static_assert(table.size() == 9);

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
};

TEST(ClassLookupTableByName, AccessContext1) {
  constexpr auto a = A(10.0);
  constexpr auto b = B(10.0);

  auto test_get_value_from_A = [](const A& obj) {
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
  auto test_get_value_from_B = [](const B& obj) {
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
    // Private member in struct A is accessible
    EXPECT_FALSE(obj.get_value_from_B("priv_a1", &dest));
  };
  test_get_value_from_A(a);
  test_get_value_from_A(b);
  test_get_value_from_B(b);

  constexpr auto table_options = rfl::class_member_lookup_table_options{
      .category = rfl::class_member_category::nonstatic_data_members,
  };
  // Only public non-static data members are accessible in current context.
  constexpr auto table_public = LOOKUP_TABLE(B, "*", table_options);
  static_assert(std::is_same_v<double B::*, decltype(table_public)::value_type>);
  static_assert(table_public.size() == 5);

  auto test_access_public = [](const B& obj, const auto& table) constexpr {
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

  // Manually specified access context.
  constexpr auto table_all = LOOKUP_TABLE(B, "*", table_options, rfl::unchecked_context());
  static_assert(std::is_same_v<double B::*, decltype(table_all)::value_type>);
  static_assert(table_all.size() == 10);

  test_access_public(b, table_all);
  CHECK_MEMBER_VARIABLE_STATIC(13.0, b, table_all["prot_a1"]);
  CHECK_MEMBER_VARIABLE_STATIC(14.0, b, table_all["prot_a2"]);
  CHECK_MEMBER_VARIABLE_STATIC(15.0, b, table_all["priv_a1"]);
  CHECK_MEMBER_VARIABLE_STATIC(22.0, b, table_all["prot_b1"]);
  CHECK_MEMBER_VARIABLE_STATIC(23.0, b, table_all["priv_b1"]);
}
