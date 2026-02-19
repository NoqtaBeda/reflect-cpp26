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

#include <reflect_cpp26/type_traits/cvref.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

TEST(TypeTraits, AddCVLike) {
  static_assert(std::is_same_v<rfl::add_cv_like_t<int, double>, int>);
  // int + const -> const int
  static_assert(std::is_same_v<rfl::add_cv_like_t<int, const double>, const int>);
  // const int + volatile -> const volatile int
  static_assert(std::is_same_v<rfl::add_cv_like_t<const int, volatile double>, const volatile int>);
  // volatile int + const volatile -> const volatile int
  static_assert(
      std::is_same_v<rfl::add_cv_like_t<volatile int, const volatile double>, const volatile int>);

  static_assert(std::is_same_v<rfl::add_cv_like_t<const int, double&>, const int>);
  // volatile int + const -> const volatile int
  static_assert(
      std::is_same_v<rfl::add_cv_like_t<volatile int, const double&>, const volatile int>);
  // const int + volatile -> const volatile int
  static_assert(
      std::is_same_v<rfl::add_cv_like_t<const int, volatile double&>, const volatile int>);
  // volatile int + const volatile -> const volatile int
  static_assert(
      std::is_same_v<rfl::add_cv_like_t<volatile int, const volatile double&>, const volatile int>);

  static_assert(std::is_same_v<rfl::add_cv_like_t<const int, double&&>, const int>);
  // volatile int + const -> const volatile int
  static_assert(
      std::is_same_v<rfl::add_cv_like_t<volatile int, const double&&>, const volatile int>);
  // const int + volatile -> const volatile int
  static_assert(
      std::is_same_v<rfl::add_cv_like_t<const int, volatile double&&>, const volatile int>);
  // volatile int + const volatile -> const volatile int
  static_assert(std::is_same_v<rfl::add_cv_like_t<volatile int, const volatile double&&>,
                               const volatile int>);
}

TEST(TypeTraits, AddCVRefLike) {
  static_assert(std::is_same_v<rfl::add_cvref_like_t<int, double>, int>);
  // int + const -> const int
  static_assert(std::is_same_v<rfl::add_cvref_like_t<int, const double>, const int>);
  // const int + volatile -> const volatile int
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<const int, volatile double>, const volatile int>);
  // volatile int + const volatile -> const volatile int
  static_assert(std::is_same_v<rfl::add_cvref_like_t<volatile int, const volatile double>,
                               const volatile int>);

  // const int + & -> const int &
  static_assert(std::is_same_v<rfl::add_cvref_like_t<const int, double&>, const int&>);
  // volatile int + const + & -> const volatile int &
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<volatile int, const double&>, const volatile int&>);
  // const int + volatile + & -> const volatile int &
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<const int, volatile double&>, const volatile int&>);
  // volatile int + const volatile + & -> const volatile int &
  static_assert(std::is_same_v<rfl::add_cvref_like_t<volatile int, const volatile double&>,
                               const volatile int&>);

  // const int + && -> const int &&
  static_assert(std::is_same_v<rfl::add_cvref_like_t<const int, double&&>, const int&&>);
  // volatile int + const + && -> const volatile int &&
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<volatile int, const double&&>, const volatile int&&>);
  // const int + volatile + && -> const volatile int &&
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<const int, volatile double&&>, const volatile int&&>);
  // volatile int + const volatile + && -> const volatile int &&
  static_assert(std::is_same_v<rfl::add_cvref_like_t<volatile int, const volatile double&&>,
                               const volatile int&&>);
}

TEST(TypeTraits, PropagateCVLike) {
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<int, double>, int>);
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<int, const double>, const int>);
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<int, volatile double&>, volatile int>);
  static_assert(
      std::is_same_v<rfl::propagate_cv_like_t<int, const volatile double&&>, const volatile int>);

  static_assert(std::is_same_v<rfl::propagate_cv_like_t<const int**, double>, const int**>);
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<const int**&, double>, const int**&>);
  // _____ int * _____ volatile * _____
  //   ^           ^                ^
  // const       const            const
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<int* volatile*, const double>,
                               const int* const volatile* const>);
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<int* volatile*&&, const double>,
                               const int* const volatile* const&&>);
  // ________ const int * ________ * ________ const
  //    ^                    ^          ^
  // volatile             volatile   volatile
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<const int** const, volatile double&>,
                               const volatile int* volatile* const volatile>);
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<const int**&, volatile double&>,
                               const volatile int* volatile* volatile&>);
  // ______________ int * ______________ * ______________
  //       ^                    ^                ^
  // const volatile       const volatile   const volatile
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<int**, const volatile double&&>,
                               const volatile int* const volatile* const volatile>);
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<int**&&, const volatile double&&>,
                               const volatile int* const volatile* const volatile&&>);
}

// The following part is synchronized with docs/type_traits.md
TEST(TypeTraits, CVRefExamples) {
  // Adds const-qualifier to T = int*
  static_assert(std::is_same_v<rfl::add_cv_like_t<int*, const double&>,
                               int* const>);  // const T = int* const

  // Adds volatile-qualifier to T = const int*
  static_assert(std::is_same_v<rfl::add_cv_like_t<const int*, volatile double&&>,
                               const int* volatile>);  // volatile T = const int* volatile

  // Adds volatile-qualifier to T = int* const
  static_assert(std::is_same_v<rfl::add_cv_like_t<int* const, volatile double&&>,
                               int* const volatile>);  // volatile T = int* const volatile

  // Adds const-qualifier and lvalue-reference qualifier to T = int*
  static_assert(std::is_same_v<rfl::add_cvref_like_t<int*, const double&>,
                               int* const&>);  // const T& = int* const &

  // Adds volatile-qualifier and rvalue-reference qualifier to T = const int*
  static_assert(std::is_same_v<rfl::add_cvref_like_t<const int*, volatile double&&>,
                               const int* volatile&&>);  // volatile T&& = const int* volatile &&

  // Propagates volatile-qualifier to every level, including the pointer type itself.
  // For a k-level pointer, the cv-qualifiers are applied (k+1) times.
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<const int***, volatile double>,
                               const volatile int* volatile* volatile* volatile>);

  // Propagates volatile-qualifier to the rvalue-referenced type.
  // Note that the result is still a rvalue-reference.
  static_assert(std::is_same_v<rfl::propagate_cv_like_t<const int***&&, volatile double&>,
                               const volatile int* volatile* volatile* volatile&&>);
}
