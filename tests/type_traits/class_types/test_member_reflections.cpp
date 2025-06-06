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

// ---- Member variable (1/2) ----

// Member object pointer with cv-qualifier
static_assert(rfl::is_accessible_by_member_reflection_v<
  foo_t, ^^foo_t::y>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const foo_t, ^^foo_t::y>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  volatile foo_t, ^^foo_t::y>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const volatile foo_t, ^^foo_t::y>);

// Member object pointer with reference
static_assert(rfl::is_accessible_by_member_reflection_v<
  foo_t&, ^^foo_t::y>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const foo_t&, ^^foo_t::y>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  volatile foo_t&, ^^foo_t::y>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const volatile foo_t&, ^^foo_t::y>);

// ---- Member function (1/2) ----

// char f_no_qualifier(int);
static_assert(rfl::is_accessible_by_member_reflection_v<
  foo_t, ^^foo_t::f_no_qualifier>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const foo_t, ^^foo_t::f_no_qualifier>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  volatile foo_t, ^^foo_t::f_no_qualifier>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const volatile foo_t, ^^foo_t::f_no_qualifier>);

// char* f_const(int*) const;
static_assert(rfl::is_accessible_by_member_reflection_v<
  foo_t, ^^foo_t::f_const>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const foo_t, ^^foo_t::f_const>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  volatile foo_t, ^^foo_t::f_const>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const volatile foo_t, ^^foo_t::f_const>);

// char** f_volatile(int**) volatile;
static_assert(rfl::is_accessible_by_member_reflection_v<
  foo_t, ^^foo_t::f_volatile>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const foo_t, ^^foo_t::f_volatile>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  volatile foo_t, ^^foo_t::f_volatile>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const volatile foo_t, ^^foo_t::f_volatile>);

struct bar_t : foo_t {
  int arr[12];
  int g_lvalue_ref(int) &;
  int* g_lvalue_cref(int*) const &;
  int** g_rvalue_ref(int**) &&;
  int*** g_rvalue_cref(int***) const &&;
};

// ---- Member variable (2/2) ----

static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t, ^^bar_t::arr>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t, ^^foo_t::y>); // Derived -> Based is OK
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  foo_t, ^^bar_t::arr>); // Based -> Derived is clearly not.

static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t, ^^foo_t::f_volatile>); // Derived -> Based is OK
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  foo_t, ^^bar_t::g_lvalue_ref>); // Based -> Derived is clearly not.

// ---- Member function (2/2) ----

// int g_lvalue_ref(int) &;
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  bar_t, ^^bar_t::g_lvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const bar_t, ^^bar_t::g_lvalue_ref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t&, ^^bar_t::g_lvalue_ref>); // non-const lvalue ref only
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const bar_t&, ^^bar_t::g_lvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  bar_t&&, ^^bar_t::g_lvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const bar_t&&, ^^bar_t::g_lvalue_ref>);

// int* g_lvalue_cref(int*) const &;
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t, ^^bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const bar_t, ^^bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t&, ^^bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const bar_t&, ^^bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t&&, ^^bar_t::g_lvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const bar_t&&, ^^bar_t::g_lvalue_cref>);

// int** g_rvalue_ref(int**) &&;
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t, ^^bar_t::g_rvalue_ref>); // non-const rvalue ref only
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const bar_t, ^^bar_t::g_rvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  bar_t&, ^^bar_t::g_rvalue_ref>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const bar_t&, ^^bar_t::g_rvalue_ref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t&&, ^^bar_t::g_rvalue_ref>); // non-const rvalue ref only
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const bar_t&&, ^^bar_t::g_rvalue_ref>);

// int*** g_rvalue_cref(int***) const &&;
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t, ^^bar_t::g_rvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const bar_t, ^^bar_t::g_rvalue_cref>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  bar_t&, ^^bar_t::g_rvalue_cref>);
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  const bar_t&, ^^bar_t::g_rvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bar_t&&, ^^bar_t::g_rvalue_cref>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  const bar_t&&, ^^bar_t::g_rvalue_cref>);

// ---- Diamond inheritance ----

struct baz_t : foo_t {
  const char* str;
};

// Diamond inheritance
struct qux_t : bar_t, baz_t {
  const wchar_t* str; // Shadows baz_t::str
};

static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  qux_t, ^^bar_t::x>); // Fails due to ambiguity
static_assert(NOT rfl::is_accessible_by_member_reflection_v<
  qux_t, ^^bar_t::f_const>); // Fails due to ambiguity as well
static_assert(rfl::is_accessible_by_member_reflection_v<
  qux_t, ^^baz_t::str>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  qux_t, ^^qux_t::str>);

// ---- References ----

struct references_A_t {
  int& i;
  const long& cl;
};

struct references_B_t : references_A_t {
  volatile float& vf;
  const volatile double& cvd;
};

// Note: we cannot form a pointer-to-member to member of reference type,
// thus is_accessible_by_member_pointer_v is not usable with reference members.
static_assert(rfl::is_accessible_by_member_reflection_v<
  references_B_t, ^^references_A_t::i>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  references_B_t, ^^references_A_t::cl>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  references_B_t, ^^references_B_t::vf>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  references_B_t, ^^references_B_t::cvd>);

// ---- Bit-fields ----

struct bit_fields_A_t {
  int32_t x: 1;
  int32_t y: 2;
};

struct bit_fields_B_t : bit_fields_A_t {
  int32_t a: 4;
  int32_t b: 8;
};

// Note: Address of bit-field can not be requested,
// thus is_accessible_by_member_pointer_v is not usable with bit-field members.
static_assert(rfl::is_accessible_by_member_reflection_v<
  bit_fields_B_t, ^^bit_fields_A_t::x>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bit_fields_B_t, ^^bit_fields_A_t::y>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bit_fields_B_t, ^^bit_fields_B_t::a>);
static_assert(rfl::is_accessible_by_member_reflection_v<
  bit_fields_B_t, ^^bit_fields_B_t::b>);

TEST(TypeTraitsClassTypes, MemberReflections) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
