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
#include <system_error>
#include <variant>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/class_types/has_ambiguous_inheritance.hpp>
#endif

namespace rfl = reflect_cpp26;

// Non-class types: no ambiguous base class by nature
static_assert(NOT rfl::has_ambiguous_inheritance_v<int>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<int[]>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<const char*>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<const std::errc>);

struct foo_t {
  int a;
  int b;
};

// Class types without base classes: no ambiguity by nature as well
static_assert(NOT rfl::has_ambiguous_inheritance_v<foo_t>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<const volatile foo_t>);

struct bar_t : foo_t {
  int a;
  int b;
};

struct baz_t : bar_t {
  int a;
  int b;
};

// Single inheritance only: no ambiguity since the inheritance graph is
// definitely a tree. Name collision does not matter.
static_assert(NOT rfl::has_ambiguous_inheritance_v<bar_t>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<const bar_t>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<baz_t>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<volatile baz_t>);

struct qux_t : baz_t, private foo_t {};

// foo_t is inherited twice
static_assert(rfl::has_ambiguous_inheritance_v<qux_t>);
static_assert(rfl::has_ambiguous_inheritance_v<const qux_t>);
// Pointers and references are always evaluated as false since they are not
// considered as class type.
static_assert(NOT rfl::has_ambiguous_inheritance_v<const qux_t&>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<volatile qux_t*>);

struct A { int x; };
struct B : A { int y; };
struct C : A { int z; };
struct D : B, C {};

// Diamond inheritance: A is inherited twice indirectly.
static_assert(rfl::has_ambiguous_inheritance_v<D>);
static_assert(rfl::has_ambiguous_inheritance_v<const volatile D>);

struct E : std::monostate { int e; };
struct F : std::monostate { int f; };
struct G : E, F, std::monostate { int g; };

// Empty base class has no effect.
static_assert(NOT rfl::has_ambiguous_inheritance_v<G>);

struct H : public C, private G { int h; };
struct I : protected H { int i; };
struct J : private A, public G { int j; };
struct K : public I { int k; };
struct L : public K, protected J { int l; };
struct M : private F { int m; };
struct N : protected K, public M { int n; };

// More complex cases.
static_assert(NOT rfl::has_ambiguous_inheritance_v<H>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<I>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<J>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<K>);
static_assert(rfl::has_ambiguous_inheritance_v<L>); // A inherited twice
static_assert(NOT rfl::has_ambiguous_inheritance_v<M>);
static_assert(rfl::has_ambiguous_inheritance_v<N>); // F inherited twice

struct AA { int a; };
struct BB : virtual AA { int b; };
struct CC : virtual AA { int c; };
struct DD : BB, CC { int d; };
struct EE : virtual AA, BB, CC { int e; };
struct FF : AA, BB, CC { int f; };

// With virtual inheritance
static_assert(NOT rfl::has_ambiguous_inheritance_v<DD>);
static_assert(NOT rfl::has_ambiguous_inheritance_v<EE>);
static_assert(rfl::has_ambiguous_inheritance_v<FF>);

TEST(TypeTraitsClassTypes, HasAmbiguousInheritance) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
