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

#include <reflect_cpp26/type_traits/class_types/member_access.hpp>
#include <system_error>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

// -------- Demonstration of Design Retionale --------

// The following group of demonstration displays the retionale of
// evaluation with bit-field members.

// A simple struct with two bit-field members, one of which is const-qualified.
struct demo_t {
  const int foo : 4;
  int bar : 12;
};

// (1) Object with lvalue reference T& :
//     * cv-qualifiers of T are expected to propagate to bar;
//     * The evaluation result is expected to be lvalue reference as well
//       to match C++ behavior that the following example shows:
// auto d = demo_t{.foo = 3, .bar = 42};
// volatile auto& r = d;
// r.bar += 21; // Memory load/store can not be optimized out as r is volatile
// assert(d.bar == 63); // Modified via lvalue reference r
using demo1_with_lvalue_ref = decltype((std::declval<volatile demo_t&>().bar));
static_assert(std::is_same_v<volatile int&, demo1_with_lvalue_ref>);

// (2) Object with rvalue reference T&& :
//     * cv-qualifiers of T are expected to propagate to bar;
//     * The evaluation result is expected to be rvalue reference as well.
using demo1_with_rvalue_ref = decltype((std::declval<volatile demo_t&&>().bar));
static_assert(std::is_same_v<volatile int&&, demo1_with_rvalue_ref>);

// (3) Similar to (2). Note that the rvalue reference qualifier in resulted
//     type 'volatile int&&' is produced by C++ decltype(()) syntax.
//     Expected evaluation result of
//     rfl::ith_*_nonstatic_data_member_type_t<1, volatile demo_t>
//     is 'volatile int' without rvalue-ref qualifier.
using demo1_without_ref = decltype((std::declval<volatile demo_t>().bar));
static_assert(std::is_same_v<volatile int&&, demo1_without_ref>);

// (1) Similar to demo1.
// Exmple:
// auto d = demo_t{.foo = 3, .bar = 42};
// volatile auto& r = d;
// int b = r.bar; // OK: Memory load can not be optimized out as r is volatile
// r.bar += 21;   // COMPILE ERROR: bar is const-qualified.
using demo2_with_lvalue_ref = decltype((std::declval<volatile demo_t&>().foo));
static_assert(std::is_same_v<const volatile int&, demo2_with_lvalue_ref>);

// (2) Similar to demo1.
using demo2_with_rvalue_ref = decltype((std::declval<volatile demo_t&&>().foo));
static_assert(std::is_same_v<const volatile int&&, demo2_with_rvalue_ref>);

// (3) Similar to demo1.
using demo2_without_ref = decltype((std::declval<volatile demo_t>().foo));
static_assert(std::is_same_v<const volatile int&&, demo2_without_ref>);

// -------- Unit Test Cases --------

struct foo_t {
  int x;

private:
  int y;

protected:
  const foo_t* prev;

public:
  const foo_t** next;
};

TEST(TypeTraitsClassTypes, MemberAccessFooPublic) {
  static_assert(std::is_same_v<rfl::ith_public_direct_nonstatic_data_member_type_t<0, foo_t>, int>);
  // const foo_t** plus const and lvalue-ref qualifiers,
  // analogous to qualifiers of const foo_t&
  static_assert(std::is_same_v<rfl::ith_public_direct_nonstatic_data_member_type_t<1, const foo_t&>,
                               const foo_t** const&>);
}

TEST(TypeTraitsClassTypes, MemberAccessFooAll) {
  static_assert(std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<0, foo_t>, int>);
  // int plus lvalue-ref qualifier, analogous to foo_t&
  static_assert(std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<1, foo_t&>, int&>);
  // const foo_t* plus const and lvalue-ref qualifiers,
  // analogous to const foo_t&
  static_assert(
      std::is_same_v<rfl::ith_nonstatic_data_member_type_t<2, const foo_t&>, const foo_t* const&>);
  // const foo_t** plus volatile and rvalue-ref qualifiers,
  // analogous to qualifiers of volatile foo_t&&
  static_assert(std::is_same_v<rfl::ith_nonstatic_data_member_type_t<3, volatile foo_t&&>,
                               const foo_t** volatile&&>);
}

struct bar_t : foo_t {
  volatile double value;

  explicit bar_t(unsigned x, const foo_t& a1, volatile foo_t&& a2)
      : value(x),
        ok_flag(x & 1),
        done_flag((x >> 1) & 1),
        label(x >> 2),
        associated_1(a1),
        associated_2(std::move(a2)) {}

private:
  unsigned ok_flag : 1;

protected:
  volatile int done_flag : 1;

public:
  const unsigned label : 30;
  const foo_t& associated_1;

private:
  volatile foo_t&& associated_2;
};

TEST(TypeTraitsClassTypes, MemberAccessBarPublic) {
  static_assert(std::is_same_v<rfl::ith_public_direct_nonstatic_data_member_type_t<0, bar_t>,
                               volatile double>);
  // const unsigned (underlying of bit-field 'label') plus volatile qualifier,
  // analogous to volatile bar_t
  static_assert(
      std::is_same_v<rfl::ith_public_direct_nonstatic_data_member_type_t<1, volatile bar_t>,
                     const volatile unsigned>);
  // const foo_t& (of reference 'associated_1') keeps unchanged regardless of
  // qualifiers of foo_t
  static_assert(
      std::is_same_v<rfl::ith_public_direct_nonstatic_data_member_type_t<2, bar_t&>, const foo_t&>);

  // Inherited
  static_assert(
      std::is_same_v<rfl::ith_public_nonstatic_data_member_type_t<0, const bar_t>, const int>);
  static_assert(std::is_same_v<rfl::ith_public_nonstatic_data_member_type_t<1, volatile bar_t>,
                               const foo_t** volatile>);
  // Direct
  static_assert(std::is_same_v<rfl::ith_public_nonstatic_data_member_type_t<2, const bar_t&>,
                               const volatile double&>);
  static_assert(std::is_same_v<rfl::ith_public_nonstatic_data_member_type_t<3, const bar_t&>,
                               const unsigned&>);
  static_assert(std::is_same_v<rfl::ith_public_nonstatic_data_member_type_t<4, volatile bar_t&&>,
                               const foo_t&>);
};

TEST(TypeTraitsClassTypes, MemberAccessBarAll) {
  static_assert(
      std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<0, bar_t>, volatile double>);
  static_assert(std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<1, bar_t>, unsigned>);
  static_assert(
      std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<2, bar_t>, volatile int>);
  static_assert(
      std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<3, bar_t>, const unsigned>);
  static_assert(
      std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<4, bar_t>, const foo_t&>);
  static_assert(
      std::is_same_v<rfl::ith_direct_nonstatic_data_member_type_t<5, bar_t>, volatile foo_t&&>);

  static_assert(std::is_same_v<rfl::ith_nonstatic_data_member_type_t<0, const bar_t&>, const int&>);
  static_assert(
      std::is_same_v<rfl::ith_nonstatic_data_member_type_t<1, const bar_t&&>, const int&&>);
  static_assert(std::is_same_v<rfl::ith_nonstatic_data_member_type_t<2, volatile bar_t&>,
                               const foo_t* volatile&>);
  static_assert(std::is_same_v<rfl::ith_nonstatic_data_member_type_t<3, volatile bar_t&&>,
                               const foo_t** volatile&&>);
  static_assert(std::is_same_v<rfl::ith_nonstatic_data_member_type_t<4, const bar_t&>,
                               const volatile double&>);
  static_assert(std::is_same_v<rfl::ith_nonstatic_data_member_type_t<5, bar_t&>, unsigned&>);
  // Bit-field demo case (1) with bar_t::done_flag whose underlying type is
  // volatile int: volatile int + const & -> const volatile int &
  static_assert(
      std::is_same_v<rfl::ith_nonstatic_data_member_type_t<6, const bar_t&>, const volatile int&>);
  // Bit-field demo case (2) with bar_t::label whose underlying type is
  // const unsigned: const int + volatile && -> const volatile unsigned &&
  static_assert(std::is_same_v<rfl::ith_nonstatic_data_member_type_t<7, volatile bar_t&&>,
                               const volatile unsigned&&>);
  static_assert(
      std::is_same_v<rfl::ith_nonstatic_data_member_type_t<8, volatile bar_t&&>, const foo_t&>);
  static_assert(
      std::is_same_v<rfl::ith_nonstatic_data_member_type_t<9, const bar_t&>, volatile foo_t&&>);
}
