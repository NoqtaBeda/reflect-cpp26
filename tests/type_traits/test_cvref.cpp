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

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/cvref.hpp>
#endif

namespace rfl = reflect_cpp26;

TEST(TypeTraits, CVRefFromNoCVRef) {
  static_assert(std::is_same_v<rfl::add_cvref_like_t<int, double>, int>);
  static_assert(std::is_same_v<rfl::add_cvref_like_t<int, const double>, const int>);
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<const int, volatile double>, const volatile int>);
  static_assert(std::is_same_v<rfl::add_cvref_like_t<volatile int, const volatile double>,
                               const volatile int>);

  static_assert(std::is_same_v<rfl::add_cvref_like_t<const int, double&>, const int&>);
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<volatile int, const double&>, const volatile int&>);
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<const int, volatile double&>, const volatile int&>);
  static_assert(std::is_same_v<rfl::add_cvref_like_t<volatile int, const volatile double&>,
                               const volatile int&>);

  static_assert(std::is_same_v<rfl::add_cvref_like_t<const int, double&&>, const int&&>);
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<volatile int, const double&&>, const volatile int&&>);
  static_assert(
      std::is_same_v<rfl::add_cvref_like_t<const int, volatile double&&>, const volatile int&&>);
  static_assert(std::is_same_v<rfl::add_cvref_like_t<volatile int, const volatile double&&>,
                               const volatile int&&>);
}
