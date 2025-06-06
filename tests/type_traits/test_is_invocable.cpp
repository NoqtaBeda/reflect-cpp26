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
#include <cstdio>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/is_invocable.hpp>
#endif

namespace rfl = reflect_cpp26;

double foo_2(int, int);
double foo_3(int, long, double);
double foo_va(int, ...);
double bar_va(int, ...) noexcept;

struct bar_t {
  bar_t* operator()(int, int);
  bar_t* operator()(int, int, int) noexcept;
};

// ---- is_invocable_exactly_r_v ----

static_assert(rfl::is_invocable_exactly_r_v<
  double, decltype(foo_2), int, int>);
static_assert(NOT rfl::is_invocable_exactly_r_v<
  float, decltype(foo_2), int, int>);
static_assert(NOT rfl::is_invocable_exactly_r_v<bar_t*, bar_t, int>);
static_assert(rfl::is_invocable_exactly_r_v<bar_t*, bar_t, int, int>);
static_assert(rfl::is_invocable_exactly_r_v<bar_t*, bar_t, short, int, long>);
static_assert(NOT rfl::is_invocable_exactly_r_v<void*, bar_t, int, int>);
static_assert(NOT rfl::is_invocable_exactly_r_v<void*, bar_t, int, int, int>);

// ---- is_nothrow_invocable_exactly_r_v

static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<
  double, decltype(foo_2), int, int>);
static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<
  float, decltype(foo_2), int, int>); // Implicit conversion
static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<
  bar_t*, bar_t, int, int>); // Not noexcept
static_assert(rfl::is_nothrow_invocable_exactly_r_v<
  bar_t*, bar_t, int, int, int>);
static_assert(rfl::is_nothrow_invocable_exactly_r_v<
  bar_t*, bar_t, short, int, long>);
static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<
  void*, bar_t, int, int, int>); // Implicit conversion

// ---- is_invocable_n_v ----

// Implicitly convertible to arguments
static_assert(NOT rfl::is_invocable_n_v<decltype(foo_3), int, 1>);
static_assert(rfl::is_invocable_n_v<decltype(foo_2), int, 2>);
static_assert(rfl::is_invocable_n_v<decltype(foo_3), int, 3>);
static_assert(NOT rfl::is_invocable_n_v<decltype(foo_2), int, 4>);

// Not convertible to arguments
static_assert(NOT rfl::is_invocable_n_v<decltype(foo_3), const char*, 1>);
static_assert(NOT rfl::is_invocable_n_v<decltype(foo_2), const char*, 2>);
static_assert(NOT rfl::is_invocable_n_v<decltype(foo_3), const char*, 3>);
static_assert(NOT rfl::is_invocable_n_v<decltype(foo_2), const char*, 4>);

// va_args: function with ellipsis parameter
static_assert(rfl::is_invocable_n_v<decltype(foo_va), char, 1>);
static_assert(rfl::is_invocable_n_v<decltype(foo_va), short, 2>);
static_assert(rfl::is_invocable_n_v<decltype(foo_va), int, 3>);
static_assert(rfl::is_invocable_n_v<decltype(foo_va), long, 4>);

// Functors
static_assert(NOT rfl::is_invocable_n_v<bar_t, int, 1>);
static_assert(rfl::is_invocable_n_v<bar_t, int, 2>);
static_assert(rfl::is_invocable_n_v<bar_t, int, 3>);
static_assert(NOT rfl::is_invocable_n_v<bar_t, int, 4>);

// ---- is_nothrow_invocable_n_v ----

static_assert(NOT rfl::is_nothrow_invocable_n_v<decltype(foo_va), int, 1>);
static_assert(NOT rfl::is_nothrow_invocable_n_v<decltype(foo_va), int, 2>);

static_assert(NOT rfl::is_nothrow_invocable_n_v<decltype(bar_va), int, 0>);
static_assert(rfl::is_nothrow_invocable_n_v<decltype(bar_va), int, 1>);
static_assert(rfl::is_nothrow_invocable_n_v<decltype(bar_va), int, 2>);

static_assert(NOT rfl::is_nothrow_invocable_n_v<bar_t, int, 1>);
static_assert(NOT rfl::is_nothrow_invocable_n_v<bar_t, int, 2>);
static_assert(rfl::is_nothrow_invocable_n_v<bar_t, int, 3>); // noexcept
static_assert(NOT rfl::is_nothrow_invocable_n_v<bar_t, int, 4>);

// ---- is_invocable_r_n_v ----

static_assert(rfl::is_invocable_r_n_v<int, decltype(foo_va), unsigned, 3>);
static_assert(NOT rfl::is_invocable_r_n_v<int*, decltype(foo_va), unsigned, 3>);

static_assert(rfl::is_nothrow_invocable_r_n_v<void*, bar_t, int, 3>);
static_assert(NOT rfl::is_nothrow_invocable_r_n_v<char*, bar_t, int, 3>);

// ---- is_invocable_exactly_r_n_v ----

static_assert(rfl::is_invocable_exactly_r_n_v<
  double, decltype(foo_2), int, 2>);
static_assert(NOT rfl::is_invocable_exactly_r_n_v<
  float, decltype(foo_2), int, 2>);
static_assert(NOT rfl::is_invocable_exactly_r_n_v<bar_t*, bar_t, int, 1>);
static_assert(rfl::is_invocable_exactly_r_n_v<bar_t*, bar_t, int, 2>);
static_assert(rfl::is_invocable_exactly_r_n_v<bar_t*, bar_t, long, 3>);
static_assert(NOT rfl::is_invocable_exactly_r_n_v<void*, bar_t, int, 2>);
static_assert(NOT rfl::is_invocable_exactly_r_n_v<void*, bar_t, int, 3>);

// ---- is_nothrow_invocable_exactly_r_n_v

static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<
  double, decltype(foo_2), int, 2>);
static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<
  float, decltype(foo_2), int, 2>); // Implicit conversion
static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<
  bar_t*, bar_t, int, 2>); // Not noexcept
static_assert(rfl::is_nothrow_invocable_exactly_r_n_v<
  bar_t*, bar_t, long, 3>);
static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<
  void*, bar_t, int, 3>); // Implicit conversion

TEST(TypeTraits, IsInvocable) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
