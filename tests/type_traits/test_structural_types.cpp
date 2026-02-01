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

#include <cstdio>
#include <reflect_cpp26/utils/constant.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>
#include <system_error>

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/structural_types.hpp>
#endif

namespace rfl = reflect_cpp26;

// void
static_assert(NOT rfl::structural_type<void>);
static_assert(NOT rfl::structural_type<const void>);

// integral type
static_assert(rfl::structural_type<int>);
static_assert(rfl::structural_type<const char16_t>);
static_assert(rfl::structural_type<volatile long long>);
// floating-point type (since C++20)
static_assert(rfl::structural_type<double>);
static_assert(rfl::structural_type<const volatile long double>);

// pointers and nullptr
using std_array_int_4 = std::array<int, 4>;
using std_vector_int = std::vector<int>;
constexpr auto printf_constant = rfl::constant_v<std::printf>;
constexpr auto printf_fptr = std::printf;
// Note: decltype(printf) -> int (const char *, ...)
static_assert(NOT rfl::structural_type<decltype(printf)>);

static_assert(rfl::structural_type<void*>);
static_assert(rfl::structural_type<const char*>);
static_assert(rfl::structural_type<const char* const>);
static_assert(rfl::structural_type<volatile double*>);
static_assert(rfl::structural_type<volatile double* volatile>);
static_assert(rfl::structural_type<const volatile std_array_int_4*>);
static_assert(rfl::structural_type<const std_vector_int*>);
static_assert(rfl::structural_type<void (*)(int)>);
static_assert(rfl::structural_type<decltype(printf_fptr)>);
static_assert(rfl::structural_type<std::nullptr_t>);
static_assert(rfl::structural_type<const volatile std::nullptr_t>);

// lvalue references
static_assert(rfl::structural_type<double&>);
static_assert(rfl::structural_type<const int&>);
using std_array_int_4 = std::array<int, 4>;
static_assert(rfl::structural_type<const std_array_int_4&>);
using std_vector_int = std::vector<int>;
static_assert(rfl::structural_type<volatile std_vector_int&>);
static_assert(rfl::structural_type<const volatile std_vector_int&>);

// rvalue references are not structural types.
static_assert(NOT rfl::structural_type<int&&>);
static_assert(NOT rfl::structural_type<const int&&>);

// pointer-to-members
using std_pair_double = std::pair<double, double>;
static_assert(rfl::structural_type<double std_pair_double::*>);
static_assert(rfl::structural_type<const char * rfl::meta_string_view::* volatile>);

// enumerators
static_assert(rfl::structural_type<std::errc>);
static_assert(rfl::structural_type<volatile std::errc>);

// lambdas with no capture
void test_lambdas() {
  auto some_value = 0;
  auto some_lambda = [](int, float, std::string, std::vector<std::string>) {};
  auto some_lambda_with_capture = [&some_value](int) {};
  static_assert(rfl::structural_type<decltype(some_lambda)>);
  static_assert(NOT rfl::structural_type<decltype(some_lambda_with_capture)>);
}

// C-style arrays are not structural types
static_assert(NOT rfl::structural_type<int[4]>);
static_assert(NOT rfl::structural_type<int[]>);

struct foo_t {
  size_t index;
  double value;
  const foo_t* prev;
};

// non-closure literal class type
static_assert(rfl::structural_type<const foo_t>);
static_assert(rfl::structural_type<volatile std_array_int_4>);
static_assert(NOT rfl::structural_type<std_vector_int>);

using meta_tuple_cilfd = rfl::meta_tuple<char, int, long, float, double>;
static_assert(rfl::structural_type<rfl::meta_string_view>);
static_assert(NOT rfl::structural_type<std::string_view>);
static_assert(rfl::structural_type<rfl::meta_span<int>>);
static_assert(NOT rfl::structural_type<std::span<const int>>);
static_assert(rfl::structural_type<meta_tuple_cilfd>);

struct bar_with_const_t {
  const int c_int;
  const long c_long;
  const double* const c_pcdouble;
};

// bar_with_cv_t is not structural due to its volatile members.
struct bar_with_volatile_t {
  const int c_int;
  volatile long v_long;
  const volatile double cv_double;
};

// bar_with_mutable_t is not structural due to its mutable member.
struct bar_with_mutable_t {
  int some_int;
  mutable long some_mutable_long;
};

// class types: with cv-qualified or mutable members
static_assert(rfl::structural_type<bar_with_const_t>);
static_assert(rfl::structural_type<volatile bar_with_const_t>);
static_assert(NOT rfl::structural_type<bar_with_volatile_t>);
static_assert(NOT rfl::structural_type<bar_with_mutable_t>);

union baz_union_1_t {
  bar_with_volatile_t as_bar;  // not literal type due to its volatile members
  foo_t as_foo;                //  is literal type
};
constexpr auto baz_union_1_constant_1 = rfl::constant_v<baz_union_1_t{}>;
constexpr auto baz_union_1_constant_2 = rfl::constant_v<baz_union_1_t{.as_foo = {}}>;

// Not structural since no literal type member.
union baz_union_2_t {
  bar_with_volatile_t as_bar;  // not literal type due to its volatile members
};

struct baz_struct_1_t {
  baz_union_1_t baz_union;
  size_t index;
};

struct baz_struct_2_t {
  baz_union_2_t baz_union;
  size_t index;
};

// union types
static_assert(rfl::structural_type<baz_union_1_t>);
static_assert(NOT rfl::structural_type<baz_union_2_t>);
static_assert(rfl::structural_type<baz_struct_1_t>);
static_assert(NOT rfl::structural_type<baz_struct_2_t>);

struct has_array_1_t {
  int sum;
  int values[16];
};

struct has_array_2_t {
  int sum;
  has_array_1_t items[4][4][4];
};

struct has_array_3_t {
  int sum;
  baz_union_1_t baz_items[16][16];
};

struct has_array_4_t {
  int sum;
  baz_union_2_t baz_items[16];  // Not literal type
};

// class types: with C-style array members
static_assert(rfl::structural_type<has_array_1_t>);
static_assert(rfl::structural_type<has_array_2_t>);
static_assert(rfl::structural_type<has_array_3_t>);
static_assert(NOT rfl::structural_type<has_array_4_t>);

struct struct_not_destructible_t {
  int x;
  int y;
  ~struct_not_destructible_t() = delete;
};

struct struct_not_trivially_destructible_t {
  int x;
  int y;
  const int& some_ref;

  constexpr struct_not_trivially_destructible_t(int x, int y, const int& r)
      : x(x), y(y), some_ref(r) {}

  constexpr ~struct_not_trivially_destructible_t() {
    if !consteval {
      std::println("some_ref = {}", some_ref);
    }
  }
};
static_assert(NOT std::is_trivially_destructible_v<struct_not_trivially_destructible_t>,
              "Incorrect test case.");
constexpr auto some_global_int = 42;
constexpr auto struct_not_trivially_destructible_constant =
    rfl::constant_v<struct_not_trivially_destructible_t{10, 20, some_global_int}>;

union union_not_destructible_1_t {
  struct_not_destructible_t as_struct;
  int as_int;
};

union union_not_destructible_2_t {
  struct_not_trivially_destructible_t as_struct;
  int as_int;
};

// class types: not (trivially) destructible
static_assert(NOT rfl::structural_type<struct_not_destructible_t>);
static_assert(rfl::structural_type<struct_not_trivially_destructible_t>);
static_assert(NOT rfl::structural_type<union_not_destructible_1_t>);
static_assert(NOT rfl::structural_type<union_not_destructible_2_t>);

struct derived_structural_1_t : struct_not_trivially_destructible_t {
  int rating;
  int usage_count;
  std::byte byte_data[256];
};

struct derived_structural_2_t : derived_structural_1_t, foo_t {
  baz_union_1_t some_union;
};

struct derived_not_structural_1_t : protected struct_not_trivially_destructible_t {
  int rating;
  int usage_count;
  std::byte byte_data[256];
};

struct derived_not_structural_2_t : derived_not_structural_1_t, foo_t {
  baz_union_1_t some_union;
};

// class types: with inheritance
static_assert(rfl::structural_type<derived_structural_1_t>);
static_assert(rfl::structural_type<derived_structural_2_t>);
static_assert(NOT rfl::structural_type<derived_not_structural_1_t>);
static_assert(NOT rfl::structural_type<derived_not_structural_2_t>);

struct foo_A_t : virtual foo_t {
  size_t a;
};

struct foo_B_t : virtual foo_t {
  size_t b;
};

struct foo_C_t : foo_A_t, foo_B_t {
  size_t c;
};

// note: struct with virtual base class is not a literal type
static_assert(NOT rfl::structural_type<foo_A_t>);
static_assert(NOT rfl::structural_type<foo_B_t>);
static_assert(NOT rfl::structural_type<foo_C_t>);

struct default_ctor_not_constexpr_t {
  int x = 0;
  int y = 0;

  default_ctor_not_constexpr_t() {
    std::println("default_ctor_not_constexpr_t() is non-constexpr.");
  }
  constexpr default_ctor_not_constexpr_t(int value) : x(value), y(-value) {
    if !consteval {
      std::println("default_ctor_not_constexpr_t(int) is constexpr instead.");
    }
  }
};
constexpr auto default_ctor_not_constexpr_constant_2 =
    rfl::constant_v<default_ctor_not_constexpr_t{42}>;

// class types: default constructor is not constexpr-constructible
static_assert(rfl::structural_type<default_ctor_not_constexpr_t>);

TEST(TypeTraitsClassTypes, IsstructuralType) {
  EXPECT_TRUE(true);  // All test cases done with static assertions above
}
