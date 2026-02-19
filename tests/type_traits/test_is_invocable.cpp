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
#include <reflect_cpp26/type_traits/is_invocable.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

double foo_2(int, int);
double foo_3(int, long, double);
double foo_va(int, ...);
double bar_va(int, ...) noexcept;

struct bar_t {
  bar_t* operator()(int, int);
  bar_t* operator()(int, int, int) noexcept;
};

int regular_func(int x) {
  return x + 1;
}
int noexcept_func(int x) noexcept {
  return x + 1;
}
int takes_two(int x, int y) {
  return x + y;
}

struct lambda_like {
  int operator()(int x) const {
    return x * 2;
  }
};

struct with_overloads {
  int operator()(int x) {
    return x;
  }
  int operator()(int x, int y) noexcept {
    return x + y;
  }
};

// ---- Variable Templates ----

// ---- IsInvocableExactlyRV ----

TEST(TypeTraits, IsInvocableExactlyRV) {
  static_assert(rfl::is_invocable_exactly_r_v<double, decltype(foo_2), int, int>);
  static_assert(rfl::is_invocable_exactly_r_v<bar_t*, bar_t, int, int>);
  static_assert(rfl::is_invocable_exactly_r_v<bar_t*, bar_t, short, int, long>);

  static_assert(NOT rfl::is_invocable_exactly_r_v<float, decltype(foo_2), int, int>);
  static_assert(NOT rfl::is_invocable_exactly_r_v<bar_t*, bar_t, int>);
  static_assert(NOT rfl::is_invocable_exactly_r_v<void*, bar_t, int, int>);
  static_assert(NOT rfl::is_invocable_exactly_r_v<void*, bar_t, int, int, int>);
}

// ---- IsNothrowInvocableExactlyRV ----

TEST(TypeTraits, IsNothrowInvocableExactlyRV) {
  static_assert(rfl::is_nothrow_invocable_exactly_r_v<bar_t*, bar_t, int, int, int>);
  static_assert(rfl::is_nothrow_invocable_exactly_r_v<bar_t*, bar_t, short, int, long>);

  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<double, decltype(foo_2), int, int>);
  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<float, decltype(foo_2), int, int>);
  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<bar_t*, bar_t, int, int>);
  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_v<void*, bar_t, int, int, int>);
}

// ---- IsInvocableNV ----

TEST(TypeTraits, IsInvocableNV) {
  static_assert(rfl::is_invocable_n_v<decltype(foo_2), int, 2>);
  static_assert(rfl::is_invocable_n_v<decltype(foo_3), int, 3>);
  static_assert(rfl::is_invocable_n_v<decltype(foo_va), char, 1>);
  static_assert(rfl::is_invocable_n_v<decltype(foo_va), short, 2>);
  static_assert(rfl::is_invocable_n_v<decltype(foo_va), int, 3>);
  static_assert(rfl::is_invocable_n_v<decltype(foo_va), long, 4>);
  static_assert(rfl::is_invocable_n_v<bar_t, int, 2>);
  static_assert(rfl::is_invocable_n_v<bar_t, int, 3>);

  static_assert(NOT rfl::is_invocable_n_v<decltype(foo_3), int, 1>);
  static_assert(NOT rfl::is_invocable_n_v<decltype(foo_2), int, 4>);
  static_assert(NOT rfl::is_invocable_n_v<decltype(foo_3), const char*, 1>);
  static_assert(NOT rfl::is_invocable_n_v<decltype(foo_2), const char*, 2>);
  static_assert(NOT rfl::is_invocable_n_v<decltype(foo_3), const char*, 3>);
  static_assert(NOT rfl::is_invocable_n_v<decltype(foo_2), const char*, 4>);
  static_assert(NOT rfl::is_invocable_n_v<bar_t, int, 1>);
  static_assert(NOT rfl::is_invocable_n_v<bar_t, int, 4>);
}

// ---- IsNothrowInvocableNV ----

TEST(TypeTraits, IsNothrowInvocableNV) {
  static_assert(rfl::is_nothrow_invocable_n_v<decltype(bar_va), int, 1>);
  static_assert(rfl::is_nothrow_invocable_n_v<decltype(bar_va), int, 2>);
  static_assert(rfl::is_nothrow_invocable_n_v<bar_t, int, 3>);

  static_assert(NOT rfl::is_nothrow_invocable_n_v<decltype(foo_3), int, 1>);
  static_assert(NOT rfl::is_nothrow_invocable_n_v<decltype(foo_3), int, 2>);
  static_assert(NOT rfl::is_nothrow_invocable_n_v<decltype(bar_va), int, 0>);
  static_assert(NOT rfl::is_nothrow_invocable_n_v<bar_t, int, 1>);
  static_assert(NOT rfl::is_nothrow_invocable_n_v<bar_t, int, 2>);
  static_assert(NOT rfl::is_nothrow_invocable_n_v<bar_t, int, 4>);
}

// ---- IsInvocableRNV ----

TEST(TypeTraits, IsInvocableRNV) {
  static_assert(rfl::is_invocable_r_n_v<int, decltype(foo_va), unsigned, 3>);
  static_assert(rfl::is_nothrow_invocable_r_n_v<void*, bar_t, int, 3>);

  static_assert(NOT rfl::is_invocable_r_n_v<int*, decltype(foo_va), unsigned, 3>);
  static_assert(NOT rfl::is_invocable_r_n_v<char*, bar_t, int, 3>);
}

// ---- IsInvocableExactlyRNV ----

TEST(TypeTraits, IsInvocableExactlyRNV) {
  static_assert(rfl::is_invocable_exactly_r_n_v<double, decltype(foo_2), int, 2>);
  static_assert(NOT rfl::is_invocable_exactly_r_n_v<float, decltype(foo_2), int, 2>);
}

// ---- IsNothrowInvocableExactlyRNV ----

TEST(TypeTraits, IsNothrowInvocableExactlyRNV) {
  static_assert(rfl::is_nothrow_invocable_exactly_r_n_v<bar_t*, bar_t, long, 3>);
  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<double, decltype(foo_2), int, 2>);
  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<float, decltype(foo_2), int, 2>);
  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<bar_t*, bar_t, int, 2>);
  static_assert(NOT rfl::is_nothrow_invocable_exactly_r_n_v<void*, bar_t, int, 3>);
}

// ---- Concepts ----

// ---- Invocable ----

TEST(TypeTraits, Invocable) {
  static_assert(rfl::invocable<decltype(regular_func), int>);
  static_assert(rfl::invocable<decltype(takes_two), int, int>);
  static_assert(rfl::invocable<decltype(foo_2), int, int>);
  static_assert(rfl::invocable<bar_t, int, int>);
  static_assert(rfl::invocable<bar_t, int, int, int>);
  static_assert(rfl::invocable<lambda_like, int>);
  static_assert(rfl::invocable<with_overloads, int>);
  static_assert(rfl::invocable<with_overloads, int, int>);
  static_assert(NOT rfl::invocable<int>);
  static_assert(NOT rfl::invocable<int, int, int>);
  static_assert(NOT rfl::invocable<bar_t, int>);
}

// ---- NothrowInvocable ----

TEST(TypeTraits, NothrowInvocable) {
  static_assert(rfl::nothrow_invocable<decltype(noexcept_func), int>);
  static_assert(rfl::nothrow_invocable<with_overloads, int, int>);
  static_assert(NOT rfl::nothrow_invocable<decltype(regular_func), int>);
  static_assert(NOT rfl::nothrow_invocable<decltype(foo_2), int, int>);
  static_assert(NOT rfl::nothrow_invocable<with_overloads, int>);
}

// ---- InvocableR ----

TEST(TypeTraits, InvocableR) {
  static_assert(rfl::invocable_r<decltype(regular_func), int, int>);
  static_assert(rfl::invocable_r<decltype(regular_func), long, int>);
  static_assert(rfl::invocable_r<lambda_like, int, int>);
  static_assert(rfl::invocable_r<void (*)(), void>);
  static_assert(NOT rfl::invocable_r<decltype(regular_func), std::string, int>);
}

// ---- InvocableExactlyR ----

TEST(TypeTraits, InvocableExactlyR) {
  static_assert(rfl::invocable_exactly_r<decltype(regular_func), int, int>);
  static_assert(rfl::invocable_exactly_r<decltype(foo_2), double, int, int>);
  static_assert(NOT rfl::invocable_exactly_r<decltype(regular_func), long, int>);
}

// ---- NothrowInvocableR ----

TEST(TypeTraits, NothrowInvocableR) {
  static_assert(rfl::nothrow_invocable_r<decltype(noexcept_func), int, int>);
  static_assert(rfl::nothrow_invocable_r<decltype(noexcept_func), long, int>);
  static_assert(NOT rfl::nothrow_invocable_r<decltype(regular_func), int, int>);
}

// ---- NothrowInvocableExactlyR ----

TEST(TypeTraits, NothrowInvocableExactlyR) {
  static_assert(rfl::nothrow_invocable_exactly_r<decltype(noexcept_func), int, int>);
  static_assert(rfl::nothrow_invocable_exactly_r<decltype(bar_va), double, int, int, int>);
  static_assert(NOT rfl::nothrow_invocable_exactly_r<decltype(regular_func), int, int>);
}

// ---- InvocableN ----

TEST(TypeTraits, InvocableN) {
  static_assert(rfl::invocable_n<decltype(foo_va), int, 3>);
  static_assert(rfl::invocable_n<decltype(foo_2), int, 2>);
  static_assert(rfl::invocable_n<bar_t, int, 2>);
}

// ---- NothrowInvocableN ----

TEST(TypeTraits, NothrowInvocableN) {
  static_assert(rfl::nothrow_invocable_n<decltype(bar_va), int, 1>);
  static_assert(rfl::nothrow_invocable_n<decltype(bar_va), int, 2>);
}

// ---- InvocableRN ----

TEST(TypeTraits, InvocableRN) {
  static_assert(rfl::invocable_r_n<decltype(foo_2), int, int, 2>);
  static_assert(rfl::invocable_r_n<bar_t, bar_t*, int, 2>);
}

// ---- InvocableExactlyRN ----

TEST(TypeTraits, InvocableExactlyRN) {
  static_assert(rfl::invocable_exactly_r_n<decltype(foo_2), double, int, 2>);
  static_assert(rfl::invocable_exactly_r_n<bar_t, bar_t*, int, 2>);
}

// ---- NothrowInvocableRN ----

TEST(TypeTraits, NothrowInvocableRN) {
  // OK: bar_t* -> void*
  static_assert(rfl::nothrow_invocable_r_n<bar_t, void*, int, 3>);
  // NOT OK: bar_t* -> char*
  static_assert(NOT rfl::nothrow_invocable_r_n<bar_t, char*, int, 3>);
}

// ---- NothrowInvocableExactlyRN ----

TEST(TypeTraits, NothrowInvocableExactlyRN) {
  static_assert(rfl::nothrow_invocable_exactly_r_n<bar_t, bar_t*, long, 3>);
  // Inexact result type
  static_assert(NOT rfl::nothrow_invocable_exactly_r_n<bar_t, void*, long, 3>);
  // bar_t* is not callable
  static_assert(NOT rfl::nothrow_invocable_exactly_r_n<bar_t*, bar_t*, long, 3>);
}
