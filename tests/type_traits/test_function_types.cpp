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
#include <reflect_cpp26/type_traits/function_types.hpp>
#endif

namespace rfl = reflect_cpp26;

// ---- int printf(int, ...) ----

using printf_traits = rfl::function_traits<decltype(printf)>;
static_assert(printf_traits::has_ellipsis_parameter);
static_assert(NOT printf_traits::is_const);
static_assert(NOT printf_traits::is_volatile);
static_assert(NOT printf_traits::is_lvalue_reference);
static_assert(NOT printf_traits::is_rvalue_reference);
static_assert(NOT printf_traits::is_noexcept);
static_assert(std::is_same_v<printf_traits::result_type, int>);
static_assert(std::is_same_v<
  printf_traits::args_type, rfl::type_tuple<const char*>>);

// ---- int fprintf_wrapper(FILE*, const char*, ...) noexcept ----

int fprintf_wrapper(FILE* file, const char* msg, ...) noexcept;

using fprintf_wrapper_traits = rfl::function_traits<decltype(fprintf_wrapper)>;
static_assert(fprintf_wrapper_traits::has_ellipsis_parameter);
static_assert(NOT fprintf_wrapper_traits::is_const);
static_assert(NOT fprintf_wrapper_traits::is_volatile);
static_assert(NOT fprintf_wrapper_traits::is_lvalue_reference);
static_assert(NOT fprintf_wrapper_traits::is_rvalue_reference);
static_assert(fprintf_wrapper_traits::is_noexcept);
static_assert(std::is_same_v<fprintf_wrapper_traits::result_type, int>);
static_assert(std::is_same_v<
  fprintf_wrapper_traits::args_type, rfl::type_tuple<FILE*, const char*>>);

// ---- void A::foo(int) ----

struct A {
  void a(int z);
};

using a_traits = rfl::function_traits<rfl::to_function_t<decltype(&A::a)>>;
static_assert(NOT a_traits::has_ellipsis_parameter);
static_assert(NOT a_traits::is_const);
static_assert(NOT a_traits::is_volatile);
static_assert(NOT a_traits::is_lvalue_reference);
static_assert(NOT a_traits::is_rvalue_reference);
static_assert(NOT a_traits::is_noexcept);
static_assert(std::is_same_v<a_traits::result_type, void>);
static_assert(std::is_same_v<a_traits::args_type, rfl::type_tuple<int>>);

// ---- void B::b(int, int*) const ----

struct B {
  void b(int z, int* dest) const;
};

using b_traits = rfl::function_traits<rfl::to_function_t<decltype(&B::b)>>;
static_assert(NOT b_traits::has_ellipsis_parameter);
static_assert(b_traits::is_const);
static_assert(NOT b_traits::is_volatile);
static_assert(NOT b_traits::is_lvalue_reference);
static_assert(NOT b_traits::is_rvalue_reference);
static_assert(NOT b_traits::is_noexcept);
static_assert(std::is_same_v<b_traits::result_type, void>);
static_assert(std::is_same_v<b_traits::args_type, rfl::type_tuple<int, int*>>);

// ---- char C::c(bool) volatile & ----

struct C {
  char c(bool flag) volatile &;
};

using c_traits = rfl::function_traits<rfl::to_function_t<decltype(&C::c)>>;
static_assert(NOT c_traits::has_ellipsis_parameter);
static_assert(NOT c_traits::is_const);
static_assert(c_traits::is_volatile);
static_assert(c_traits::is_lvalue_reference);
static_assert(NOT c_traits::is_rvalue_reference);
static_assert(NOT c_traits::is_noexcept);
static_assert(std::is_same_v<c_traits::result_type, char>);
static_assert(std::is_same_v<c_traits::args_type, rfl::type_tuple<bool>>);

// ---- long D::d(int, bool) const volatile && ----

struct D {
  long d(int z, bool flag) const volatile &&;
};

using d_traits = rfl::function_traits<rfl::to_function_t<decltype(&D::d)>>;
static_assert(NOT d_traits::has_ellipsis_parameter);
static_assert(d_traits::is_const);
static_assert(d_traits::is_volatile);
static_assert(NOT d_traits::is_lvalue_reference);
static_assert(d_traits::is_rvalue_reference);
static_assert(NOT d_traits::is_noexcept);
static_assert(std::is_same_v<d_traits::result_type, long>);
static_assert(std::is_same_v<d_traits::args_type, rfl::type_tuple<int, bool>>);

// ---- void E::e(const int*, const int*) const noexcept;

struct E {
  void e(const int* first, const int* last) const noexcept;
};

using e_traits = rfl::function_traits<rfl::to_function_t<decltype(&E::e)>>;
static_assert(NOT e_traits::has_ellipsis_parameter);
static_assert(e_traits::is_const);
static_assert(NOT e_traits::is_volatile);
static_assert(NOT e_traits::is_lvalue_reference);
static_assert(NOT e_traits::is_rvalue_reference);
static_assert(e_traits::is_noexcept);
static_assert(std::is_same_v<e_traits::result_type, void>);
static_assert(std::is_same_v<e_traits::args_type,
  rfl::type_tuple<const int*, const int*>>);

// ---- int* F::f(int*) volatile && noexcept;

struct F {
  int* f(int* dest) volatile && noexcept;
};

using f_traits = rfl::function_traits<rfl::to_function_t<decltype(&F::f)>>;
static_assert(NOT f_traits::has_ellipsis_parameter);
static_assert(NOT f_traits::is_const);
static_assert(f_traits::is_volatile);
static_assert(NOT f_traits::is_lvalue_reference);
static_assert(f_traits::is_rvalue_reference);
static_assert(f_traits::is_noexcept);
static_assert(std::is_same_v<f_traits::result_type, int*>);
static_assert(std::is_same_v<f_traits::args_type, rfl::type_tuple<int*>>);

// ---- long G::g(const char*, ...) const;

struct G {
  long g(const char* fmt, ...) const;
};

using g_traits = rfl::function_traits<rfl::to_function_t<decltype(&G::g)>>;
static_assert(g_traits::has_ellipsis_parameter);
static_assert(g_traits::is_const);
static_assert(NOT g_traits::is_volatile);
static_assert(NOT g_traits::is_lvalue_reference);
static_assert(NOT g_traits::is_rvalue_reference);
static_assert(NOT g_traits::is_noexcept);
static_assert(std::is_same_v<g_traits::result_type, long>);
static_assert(std::is_same_v<g_traits::args_type,
  rfl::type_tuple<const char*>>);

// ---- to_funtion_pointer_t ----

constexpr auto printf_fptr = printf;
// Type identity (with cvref)
static_assert(std::is_same_v<
  rfl::to_function_pointer_t<decltype(printf_fptr)>,
  int (*)(const char*, ...)>);
static_assert(std::is_same_v<
  rfl::to_function_pointer_t<volatile decltype(printf_fptr)>,
  int (*)(const char*, ...)>);
static_assert(std::is_same_v<
  rfl::to_function_pointer_t<const volatile decltype(printf_fptr) &>,
  int (*)(const char*, ...)>);

// From function types (with ref)
static_assert(std::is_same_v<
  rfl::to_function_pointer_t<decltype(fprintf_wrapper)>,
  int (*)(FILE*, const char*, ...) noexcept>);
static_assert(std::is_same_v<
  rfl::to_function_pointer_t<void(&)(const int, long, const double*)>,
  void (*)(int, long, const double*)>);

// Non-function types
static_assert(NOT rfl::is_convertible_to_function_pointer_v<void>);
static_assert(NOT rfl::is_convertible_to_function_pointer_v<int>);
static_assert(NOT rfl::is_convertible_to_function_pointer_v<const char*>);

// Member function types with cvref qualifiers
static_assert(NOT rfl::is_convertible_to_function_pointer_v<int(int) const>);
static_assert(NOT rfl::is_convertible_to_function_pointer_v<int(int) volatile>);
static_assert(NOT rfl::is_convertible_to_function_pointer_v<int(int) &>);
static_assert(NOT rfl::is_convertible_to_function_pointer_v<int(int) &&>);

// ---- to_function_t ----

// Type identity (with reference)
static_assert(std::is_same_v<
  rfl::to_function_t<decltype(fprintf_wrapper)>,
  int(FILE*, const char*, ...) noexcept>);
static_assert(std::is_same_v<
  rfl::to_function_t<decltype(fprintf_wrapper) &>,
  int(FILE*, const char*, ...) noexcept>);

// From function pointer (with cvref)
static_assert(std::is_same_v<
  rfl::to_function_t<const volatile decltype(printf_fptr) &&>,
  int(const char*, ...)>);

// From function pointers (with cvref)
struct H {
  int h(int, const char*, ...) const & noexcept;
};
static_assert(std::is_same_v<
  rfl::to_function_t<decltype(&H::h)>,
  int(int, const char*, ...) const & noexcept>);
static_assert(std::is_same_v<
  rfl::to_function_t<const decltype(&H::h)>,
  int(int, const char*, ...) const & noexcept>);
static_assert(std::is_same_v<
  rfl::to_function_t<volatile decltype(&H::h) &>,
  int(int, const char*, ...) const & noexcept>);
static_assert(std::is_same_v<
  rfl::to_function_t<const volatile decltype(&H::h) &&>,
  int(int, const char*, ...) const & noexcept>);

// Non-function types
static_assert(NOT rfl::is_convertible_to_function_v<void>);
static_assert(NOT rfl::is_convertible_to_function_v<int>);
static_assert(NOT rfl::is_convertible_to_function_v<const char*>);

TEST(TypeTraits, FunctionTypes) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
