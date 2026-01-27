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

#include <bit>
#include <concepts>
#include <cstdio>
#include <reflect_cpp26/utils/addressable_member.hpp>
#include <system_error>
#include <type_traits>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

int g_first = 1;
const int g_second = 2;

int* g_first_ptr = &g_first;
const int* g_second_ptr = &g_second;

int& g_first_ref = g_first;
const int& g_second_ref = g_second;

void test_deleted() = delete;

// ✔️: Addressable; ❌: Not-addressable
struct A {
  // Non-static data member: ✔️
  int a;
};

struct B : A {
  // Non-static data member: ✔️
  int b;
  // Bit-fields: ❌
  unsigned x : 4;
  unsigned y : 4;
};

struct C : B {
  // Nested class: ❌
  class Inner;
  // Nested type alias: ❌
  using AliasType = A;

  // Static constant data member: ✔️
  static constexpr int constant = 42;
  // Static non-constant data member: ✔️
  static inline int non_constant = 21;

  // Non-static data member: ✔️
  int c;
  // References as non-static data member: ❌
  const int& d;

  // References as static data member
  static inline int& ref_first = g_first;
  static inline const int& ref_second = g_second;

  // Constructor: ❌
  explicit constexpr C(const int& d) : d(d) {}
  // Destructor: ❌
  constexpr ~C() {}

  // Static member function: ✔️
  static constexpr auto make(int a, int b, int c, const int& d, unsigned x, unsigned y) -> C {
    auto res = C{d};
    std::tie(res.a, res.b, res.c) = std::tuple(a, b, c);
    res.x = x;
    res.y = y;
    return res;
  }

  // Template before specialization: ❌
  // Template after specialization: ✔️
  template <size_t I>
  constexpr int get() {
    if constexpr (I == 0) {
      return this->a;
    } else if constexpr (I == 1) {
      return this->b;
    } else if constexpr (I == 2) {
      return this->c;
    } else if constexpr (I == 3) {
      return this->d;
    } else if constexpr (I == 4) {
      return this->x;
    } else {
      return this->y;
    }
  }

  // Deleted member function: ❌
  void dump() = delete;
};

TEST(UtilsMetaUtility, IsAddressableClassMember) {
  static constexpr auto d = 42;
  constexpr auto obj = C::make(1, 2, 3, d, 4, 5);

  // Non-static data member: ✔️
  static_assert(rfl::is_addressable_class_member(^^C::a));
  static_assert(rfl::is_addressable_class_member(^^C::b));
  static_assert(rfl::is_addressable_class_member(^^C::c));
  // Static members: ✔️
  static_assert(rfl::is_addressable_class_member(^^C::constant));
  static_assert(rfl::is_addressable_class_member(^^C::non_constant));
  static_assert(rfl::is_addressable_class_member(^^C::make));

  // Reference as static data members: ✔️
  static_assert(rfl::is_addressable_class_member(^^C::ref_first));
  static_assert(rfl::is_addressable_class_member(^^C::ref_second));
  // Addressable indeed.
  [[maybe_unused]] int* ref_first_fptr = &C::ref_first;
  [[maybe_unused]] const int* ref_second_fptr = &C::ref_second;

  // Non-static member functions (including implicitly defined ones): ✔️
  using P = std::pair<int, int>;
  constexpr auto copy_assignment_operator = [] consteval {
    for (auto member : members_of(^^P)) {
      if (is_copy_assignment(member)) return member;
    }
    rfl::compile_error("Implementation error");
  }();
  static_assert(static_cast<P& (P::*)(const P&) noexcept>(&P::operator=)
                == &[:copy_assignment_operator:]);
  static_assert(rfl::is_addressable_class_member(copy_assignment_operator));

  // Bit-field data members: ❌
  static_assert(NOT rfl::is_addressable_class_member(^^C::x));
  static_assert(NOT rfl::is_addressable_class_member(^^C::y));

  // Reference as non-static data members: ❌
  static_assert(NOT rfl::is_addressable_class_member(^^C::d));
  // ERROR: cannot form a pointer-to-member to member 'd' of reference type 'const int &'
  // constexpr auto _ = &C::d;

  // Constructors and destructures: ❌
  static_assert(NOT rfl::is_addressable_class_member(^^C::C));
  static_assert(NOT rfl::is_addressable_class_member(^^C::~C));
  // ERROR: qualified reference to 'C' is a constructor name rather than a type in this context
  // constexpr auto _ = &C::C;
  // ERROR: taking the address of a destructor
  // constexpr auto _ = &C::~C;

  // Deleted members: ❌
  static_assert(NOT rfl::is_addressable_class_member(^^C::dump));
  // ERROR: attempt to use a deleted function
  // constexpr auto _ = &C::dump;

  // Template members before specialization: ❌
  static_assert(NOT rfl::is_addressable_class_member(^^C::get));
  // Template members after specialization: ✔️
  static_assert(rfl::is_addressable_class_member(^^C::get<1>));

  // Nested classes or type aliases: ❌
  static_assert(NOT rfl::is_addressable_class_member(^^C::Inner));
  static_assert(NOT rfl::is_addressable_class_member(^^C::AliasType));

  // Not class member at all: ❌
  // Type
  static_assert(NOT rfl::is_addressable_class_member(^^int));
  static_assert(NOT rfl::is_addressable_class_member(^^C));
  static_assert(NOT rfl::is_addressable_class_member(^^std::errc));
  // Variable
  static_assert(NOT rfl::is_addressable_class_member(^^obj));
  static_assert(NOT rfl::is_addressable_class_member(^^g_first));
  // Reflector to constant
  static_assert(NOT rfl::is_addressable_class_member(std::meta::reflect_constant(obj.a)));
  // Namespace
  static_assert(NOT rfl::is_addressable_class_member(^^std));
  // Enum entry (enumerator)
  static_assert(NOT rfl::is_addressable_class_member(^^std::errc::invalid_argument));
  // Concept
  static_assert(NOT rfl::is_addressable_class_member(^^std::same_as));
}

TEST(UtilsMetaUtility, IsAddressableNonClassMember) {
  // Variables: ✔️
  static_assert(rfl::is_addressable_non_class_member(^^g_first));
  static_assert(rfl::is_addressable_non_class_member(^^g_second));
  // Pointers are variables as well.
  static_assert(rfl::is_addressable_non_class_member(^^g_first_ptr));
  static_assert(rfl::is_addressable_non_class_member(^^g_second_ptr));
  // References: ✔️
  static_assert(rfl::is_addressable_non_class_member(^^g_first_ref));
  static_assert(rfl::is_addressable_non_class_member(^^g_second_ref));
  // Addressable indded.
  [[maybe_unused]] int* p_first = &g_first_ref;
  [[maybe_unused]] const int* p_second = &g_second_ref;

  // Functions: ✔️
  static_assert(rfl::is_addressable_non_class_member(^^printf));
  // Deleted functions: ❌
  static_assert(NOT rfl::is_addressable_non_class_member(^^test_deleted));

  // Templates before specialization: ❌
  static_assert(NOT rfl::is_addressable_non_class_member(^^std::is_same_v));
  static_assert(NOT rfl::is_addressable_non_class_member(^^std::popcount));
  // Template after specialization: ✔️
  static_assert(rfl::is_addressable_non_class_member(^^std::is_same_v<int, int*>));
  static_assert(rfl::is_addressable_non_class_member(^^std::popcount<unsigned>));

  // Class members: ❌
  static_assert(NOT rfl::is_addressable_non_class_member(^^C::a));
  static_assert(NOT rfl::is_addressable_non_class_member(^^C::constant));
  static_assert(NOT rfl::is_addressable_non_class_member(^^C::C));
  static_assert(NOT rfl::is_addressable_non_class_member(^^C::~C));
  static_assert(NOT rfl::is_addressable_non_class_member(^^C::dump));
  static_assert(NOT rfl::is_addressable_non_class_member(^^C::get));
  static_assert(NOT rfl::is_addressable_non_class_member(^^C::get<1>));

  // Others: ❌
  // Types
  static_assert(NOT rfl::is_addressable_non_class_member(^^int));
  static_assert(NOT rfl::is_addressable_non_class_member(^^C));
  static_assert(NOT rfl::is_addressable_non_class_member(^^std::errc));
  // Reflector to constant
  static_assert(NOT rfl::is_addressable_non_class_member(std::meta::reflect_constant(42)));
  // Namespace
  static_assert(NOT rfl::is_addressable_non_class_member(^^std));
  // Enum entry (enumerator)
  static_assert(NOT rfl::is_addressable_non_class_member(^^std::errc::invalid_argument));
  // Concept
  static_assert(NOT rfl::is_addressable_non_class_member(^^std::same_as));
}
