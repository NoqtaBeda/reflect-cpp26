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

#include <reflect_cpp26/type_traits/class_types/ambiguous_inheritance.hpp>
#include <system_error>
#include <variant>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

// Non-class types
static_assert(NOT rfl::class_without_ambiguous_inheritance<int>);
static_assert(NOT rfl::class_without_ambiguous_inheritance<int[]>);
static_assert(NOT rfl::class_without_ambiguous_inheritance<const char*>);
static_assert(NOT rfl::class_without_ambiguous_inheritance<const std::errc>);

struct foo_t {
  int a;
  int b;
};

// Class types without base classes: no ambiguity by nature
static_assert(rfl::class_without_ambiguous_inheritance<foo_t>);
static_assert(rfl::class_without_ambiguous_inheritance<const volatile foo_t>);
// References: NOT class types
static_assert(NOT rfl::class_without_ambiguous_inheritance<foo_t&>);
static_assert(NOT rfl::class_without_ambiguous_inheritance<const volatile foo_t&&>);
// Pointers: NOT class types
static_assert(NOT rfl::class_without_ambiguous_inheritance<foo_t*>);
static_assert(NOT rfl::class_without_ambiguous_inheritance<const volatile foo_t**>);

struct bar_t : foo_t {
  int a;
  int b;
};

struct baz_t : bar_t {
  int a;
  int b;
};

// Single inheritance only: no ambiguity since the inheritance graph is
// definitely a tree. Name collision of data members does not matter.
static_assert(rfl::class_without_ambiguous_inheritance<bar_t>);
static_assert(rfl::class_without_ambiguous_inheritance<const bar_t>);
static_assert(rfl::class_without_ambiguous_inheritance<baz_t>);
static_assert(rfl::class_without_ambiguous_inheritance<volatile baz_t>);

struct qux_t : baz_t, private foo_t {};

// foo_t is inherited twice
static_assert(NOT rfl::class_without_ambiguous_inheritance<qux_t>);
static_assert(NOT rfl::class_without_ambiguous_inheritance<const qux_t>);

struct A {
  int x;
};
struct B : A {
  int y;
};
struct C : A {
  int z;
};
struct D : B, C {};

// Diamond inheritance: A is inherited twice indirectly.
// warning: direct base 'foo_t' is inaccessible due to ambiguity:
//   struct qux_t -> baz_t -> bar_t -> foo_t
//   struct qux_t -> foo_t [-Winaccessible-base]
static_assert(NOT rfl::class_without_ambiguous_inheritance<D>);
static_assert(NOT rfl::class_without_ambiguous_inheritance<const volatile D>);

namespace empty_ambiguous_base {
struct E : std::monostate {
  int e;
};
struct F : std::monostate {
  int f;
};
struct G : E, F, std::monostate {
  int g;
};

// std::monostate is inherited multiple times by struct G.
// Ambiguity happens even if std::monostate is an empty class.
// warning: direct base 'std::monostate' is inaccessible due to ambiguity:
//   struct G -> E -> std::monostate
//   struct G -> F -> std::monostate
//   struct G -> std::monostate [-Winaccessible-base]
static_assert(NOT rfl::class_without_ambiguous_inheritance<G>);
}  // namespace empty_ambiguous_base

struct E {
  int e;
};
struct F {
  int f;
};
struct G : E, F {
  int g;
};
struct H : public C, private G {
  int h;
};
struct I : protected H {
  int i;
};
struct J : private A, public G {
  int j;
};
struct K : public I {
  int k;
};
struct L : public K, protected J {
  int l;
};
struct M : private F {
  int m;
};
struct N : protected K, public M {
  int n;
};

// More complex cases.
static_assert(rfl::class_without_ambiguous_inheritance<H>);
static_assert(rfl::class_without_ambiguous_inheritance<I>);
static_assert(rfl::class_without_ambiguous_inheritance<J>);
static_assert(rfl::class_without_ambiguous_inheritance<K>);
static_assert(rfl::class_without_ambiguous_inheritance<M>);
// error: ambiguous conversion from derived class 'L' to base class 'A':
//   struct L -> K -> I -> H -> C -> A
//   struct L -> J -> A
static_assert(NOT rfl::class_without_ambiguous_inheritance<L>);
// error: ambiguous conversion from derived class 'N' to base class 'F':
//   struct N -> K -> I -> H -> G -> F
//   struct N -> M -> F
static_assert(NOT rfl::class_without_ambiguous_inheritance<N>);

struct AA {
  int a;
};
struct BB : virtual AA {
  int b;
};
struct CC : virtual AA {
  int c;
};
struct DD : BB, CC {
  int d;
};
struct EE : virtual AA, BB, CC {
  int e;
};
struct FF : AA, BB, CC {
  int f;
};

// With virtual inheritance
static_assert(rfl::class_without_ambiguous_inheritance<DD>);
static_assert(rfl::class_without_ambiguous_inheritance<EE>);
// warning: direct base 'AA' is inaccessible due to ambiguity:
//   struct FF -> AA
//   struct FF -> BB -> AA [-Winaccessible-base]
static_assert(NOT rfl::class_without_ambiguous_inheritance<FF>);

TEST(TypeTraitsClassTypes, ClassWithoutAmbiguousInheritance) {
  EXPECT_TRUE(true);  // All test cases done with static-asserts above
}
