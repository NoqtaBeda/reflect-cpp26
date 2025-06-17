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

#include <reflect_cpp26/utils/meta_utility.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

struct A {
  int a;
};

struct B : A {
  int b;
  unsigned x : 4;
  unsigned y : 4;
};

struct C : B {
  // Nested class
  class Inner;
  // Nested type alias
  using AliasType = A;

  static constexpr int constant = 42;
  static inline int non_constant = 21;

  int c;
  const int& d;

  explicit constexpr C(const int& d) : d(d) {}
  constexpr ~C() {}

  static constexpr auto make(int a, int b, int c, const int& d, unsigned x, unsigned y) -> C {
    auto res = C{d};
    std::tie(res.a, res.b, res.c) = std::tuple(a, b, c);
    res.x = x;
    res.y = y;
    return res;
  }

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

  void dump() = delete;
};

TEST(UtilsMetaUtility, ReflectPointerToMember) {
  static constexpr auto d = 42;
  constexpr auto obj = C::make(1, 2, 3, d, 4, 5);
  EXPECT_EQ_STATIC(1, obj.[:rfl::reflect_pointer_to_member(&C::a):]);
  EXPECT_EQ_STATIC(2, obj.[:rfl::reflect_pointer_to_member(&C::b):]);
  EXPECT_EQ_STATIC(3, obj.[:rfl::reflect_pointer_to_member(&C::c):]);
}

TEST(UtilsMetaUtility, IsAddressableNonStaticMember) {
  static constexpr auto d = 42;
  constexpr auto obj = C::make(1, 2, 3, d, 4, 5);

  static_assert(rfl::is_addressable_class_member(^^C::a));
  static_assert(rfl::is_addressable_class_member(^^C::b));
  static_assert(rfl::is_addressable_class_member(^^C::c));
  // Static members
  static_assert(rfl::is_addressable_class_member(^^C::constant));
  static_assert(rfl::is_addressable_class_member(^^C::non_constant));
  static_assert(rfl::is_addressable_class_member(^^C::make));

  // Bit-field members can not be addressed
  static_assert(NOT rfl::is_addressable_class_member(^^C::x));
  static_assert(NOT rfl::is_addressable_class_member(^^C::y));
  // Reference members can not be addressed
  static_assert(NOT rfl::is_addressable_class_member(^^C::d));
  // Constructors and destructures can not be addressed
  static_assert(NOT rfl::is_addressable_class_member(^^C::C));
  static_assert(NOT rfl::is_addressable_class_member(^^C::~C));
  // Deleted members can not be addressed
  static_assert(NOT rfl::is_addressable_class_member(^^C::dump));
  // Template members can not be addressed before instantiation
  static_assert(NOT rfl::is_addressable_class_member(^^C::get));
  static_assert(rfl::is_addressable_class_member(^^C::get<1>));

  // Not class member at all
  static_assert(NOT rfl::is_addressable_class_member(^^C::Inner));
  static_assert(NOT rfl::is_addressable_class_member(^^C::AliasType));
  static_assert(NOT rfl::is_addressable_class_member(^^C));
  static_assert(NOT rfl::is_addressable_class_member(^^obj));
  constexpr auto obj_a_refl = std::meta::reflect_constant(obj.a);
  static_assert(NOT rfl::is_addressable_class_member(obj_a_refl));
  static_assert(NOT rfl::is_addressable_class_member(^^std));
}
