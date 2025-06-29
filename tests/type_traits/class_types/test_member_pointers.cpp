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

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/class_types/member_traits.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_t {
  int x;
  double y;
  char f_no_qualifier(int);
  char* f_const(int*) const;
  char** f_volatile(int**) volatile;
};

// ---- rfl::member_pointer_traits of int foo_t::x ----

using traits_of_foo_x = rfl::member_pointer_traits<decltype(&foo_t::x)>;
static_assert(traits_of_foo_x::category == rfl::member_category::object);
static_assert(std::is_same_v<traits_of_foo_x::target_type, int>);
static_assert(std::is_same_v<traits_of_foo_x::direct_parent_type, foo_t>);

// ---- rfl::member_pointer_traits of char A::a(int, float, double) ----

struct A {
  char a(int, float, double);
};

using traits_of_a = rfl::member_pointer_traits<decltype(&A::a)>;
static_assert(traits_of_a::category == rfl::member_category::function);
static_assert(std::is_same_v<traits_of_a::target_type, char>);
static_assert(std::is_same_v<traits_of_a::direct_parent_type, A>);
static_assert(traits_of_a::args_type::size == 3);
static_assert(std::is_same_v<
  rfl::ith_element_t<0, traits_of_a::args_type>, int>);
static_assert(std::is_same_v<
  rfl::ith_element_t<1, traits_of_a::args_type>, float>);
static_assert(std::is_same_v<
  rfl::ith_element_t<2, traits_of_a::args_type>, double>);
// Function traits
constexpr auto expected_function_traits_of_a = rfl::function_trait_flags{};
static_assert(traits_of_a::function_traits_type::flags
  == expected_function_traits_of_a);

// ---- rfl::member_pointer_traits of char B::b(int, float, ...) const ----

struct B {
  char b(const char*, const char*, volatile size_t*, ...) volatile;
};

using traits_of_b = rfl::member_pointer_traits<decltype(&B::b)>;
static_assert(traits_of_b::category == rfl::member_category::function);
static_assert(std::is_same_v<traits_of_b::target_type, char>);
static_assert(std::is_same_v<traits_of_b::direct_parent_type, B>);
static_assert(std::is_same_v<traits_of_b::args_type,
  rfl::type_tuple<const char*, const char*, volatile size_t*>>);
// Function traits
constexpr auto expected_function_traits_of_b = rfl::function_trait_flags{
  .has_ellipsis_parameter = true,
  .is_volatile = true,
};
static_assert(traits_of_b::function_traits_type::flags
  == expected_function_traits_of_b);

// ---- rfl::is_accessible_by_member_object_pointer_v ----

// Member object pointer with cv-qualifier
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  foo_t, &foo_t::y>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  const foo_t, &foo_t::y>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  volatile foo_t, &foo_t::y>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  const volatile foo_t, &foo_t::y>);

// Member object pointer with reference
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  foo_t&, &foo_t::y>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  const foo_t&, &foo_t::y>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  volatile foo_t&, &foo_t::y>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  const volatile foo_t&, &foo_t::y>);

// ---- rfl::is_accessible_by_member_function_pointer_v (1/2) ----

// char f_no_qualifier(int);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  foo_t, &foo_t::f_no_qualifier>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const foo_t, &foo_t::f_no_qualifier>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  volatile foo_t, &foo_t::f_no_qualifier>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const volatile foo_t, &foo_t::f_no_qualifier>);

// char* f_const(int*) const;
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  foo_t, &foo_t::f_const>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  const foo_t, &foo_t::f_const>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  volatile foo_t, &foo_t::f_const>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const volatile foo_t, &foo_t::f_const>);

// char** f_volatile(int**) volatile;
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  foo_t, &foo_t::f_volatile>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const foo_t, &foo_t::f_volatile>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  volatile foo_t, &foo_t::f_volatile>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const volatile foo_t, &foo_t::f_volatile>);

// ---- rfl::member_pointer_traits of derived type ----

struct bar_t : foo_t {
  int arr[12];
  int g_lvalue_ref(int) &;
  int* g_lvalue_cref(int*) const &;
  int** g_rvalue_ref(int**) &&;
  int*** g_rvalue_cref(int***) const &&;
};

using traits_of_bar_x = rfl::member_pointer_traits<decltype(&bar_t::x)>;
static_assert(std::is_same_v<traits_of_bar_x, traits_of_foo_x>);

using traits_of_bar_arr = rfl::member_pointer_traits<decltype(&bar_t::arr)>;
static_assert(traits_of_bar_arr::category == rfl::member_category::object);
static_assert(std::is_same_v<traits_of_bar_arr::target_type, int[12]>);
static_assert(std::is_same_v<traits_of_bar_arr::direct_parent_type, bar_t>);

// ---- is_accessible_by_member_object_pointer_v with inheritance ----

static_assert(rfl::is_accessible_by_member_object_pointer_v<
  bar_t, &bar_t::arr>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  bar_t, &foo_t::y>); // Derived -> Based is OK
static_assert(NOT rfl::is_accessible_by_member_object_pointer_v<
  foo_t, &bar_t::arr>); // Based -> Derived is clearly not.

// ---- is_accessible_by_member_function_pointer_v with inheritance ----

static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t, &foo_t::f_volatile>); // Derived -> Based is OK
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  foo_t, &bar_t::g_lvalue_ref>); // Based -> Derived is clearly not.

// ---- rfl::is_accessible_by_member_function_pointer_v (2/2) ----

// int g_lvalue_ref(int) &;
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  bar_t, &bar_t::g_lvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const bar_t, &bar_t::g_lvalue_ref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t&, &bar_t::g_lvalue_ref>); // non-const lvalue ref only
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&, &bar_t::g_lvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  bar_t&&, &bar_t::g_lvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&&, &bar_t::g_lvalue_ref>);

// int* g_lvalue_cref(int*) const &;
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t, &bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  const bar_t, &bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t&, &bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&, &bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t&&, &bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&&, &bar_t::g_lvalue_cref>);

// int** g_rvalue_ref(int**) &&;
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t, &bar_t::g_rvalue_ref>); // non-const rvalue ref only
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const bar_t, &bar_t::g_rvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  bar_t&, &bar_t::g_rvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&, &bar_t::g_rvalue_ref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t&&, &bar_t::g_rvalue_ref>); // non-const rvalue ref only
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&&, &bar_t::g_rvalue_ref>);

// int*** g_rvalue_cref(int***) const &&;
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t, &bar_t::g_rvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  const bar_t, &bar_t::g_rvalue_cref>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  bar_t&, &bar_t::g_rvalue_cref>);
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&, &bar_t::g_rvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  bar_t&&, &bar_t::g_rvalue_cref>);
static_assert(rfl::is_accessible_by_member_function_pointer_v<
  const bar_t&&, &bar_t::g_rvalue_cref>);

// ---- Diamond inheritance ----

struct baz_t : foo_t {
  const char* str;
};

// Diamond inheritance
struct qux_t : bar_t, baz_t {
  const wchar_t* str; // Shadows baz_t::str
};

using traits_of_baz_str = rfl::member_pointer_traits<decltype(&baz_t::str)>;
static_assert(std::is_same_v<traits_of_baz_str::target_type, char const*>);
static_assert(std::is_same_v<traits_of_baz_str::direct_parent_type, baz_t>);

using traits_of_qux_str = rfl::member_pointer_traits<decltype(&qux_t::str)>;
static_assert(std::is_same_v<traits_of_qux_str::target_type, wchar_t const*>);
static_assert(std::is_same_v<traits_of_qux_str::direct_parent_type, qux_t>);

// Diamond inheritance
static_assert(NOT rfl::is_accessible_by_member_object_pointer_v<
  qux_t, &bar_t::x>); // Fails due to ambiguity
static_assert(NOT rfl::is_accessible_by_member_function_pointer_v<
  qux_t, &bar_t::f_const>); // Fails due to ambiguity as well
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  qux_t, &baz_t::str>);
static_assert(rfl::is_accessible_by_member_object_pointer_v<
  qux_t, &qux_t::str>);

TEST(TypeTraitsClassTypes, MemberPointers) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
