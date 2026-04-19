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

#ifndef REFLECT_CPP26_UTILS_COMPARE_TUPLE_HPP
#define REFLECT_CPP26_UTILS_COMPARE_TUPLE_HPP

#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/compare.hpp>

#define REFLECT_CPP26_EXTRACT(target_concept, ...) \
  std::meta::extract<bool>(std::meta::substitute(target_concept, {__VA_ARGS__}))

namespace reflect_cpp26 {
namespace impl {
consteval bool is_elementwise_comparable_by(  // Precondition: T and U are tuple_like
    std::meta::info T,
    std::meta::info U,
    std::meta::info target_concept) {
  auto N = tuple_size(T);
  if (tuple_size(U) != N) {
    return false;
  }
  for (auto i = 0zU; i < N; i++) {
    auto A = tuple_element(i, T);
    auto B = tuple_element(i, U);
    if (!REFLECT_CPP26_EXTRACT(target_concept, A, B)) {
      return false;
    }
  }
  return true;
}
}  // namespace impl

template <class T, class U>
constexpr auto is_elementwise_operator_eq_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_operator_eq_comparable_v);

template <class T, class U>
constexpr auto is_elementwise_operator_ne_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_operator_ne_comparable_v);

template <class T, class U>
constexpr auto is_elementwise_operator_lt_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_operator_lt_comparable_v);

template <class T, class U>
constexpr auto is_elementwise_operator_gt_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_operator_gt_comparable_v);

template <class T, class U>
constexpr auto is_elementwise_operator_le_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_operator_le_comparable_v);

template <class T, class U>
constexpr auto is_elementwise_operator_ge_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_operator_ge_comparable_v);

template <class T, class U>
constexpr auto is_elementwise_operator_3way_comparable_v =
    tuple_like<T> && tuple_like<U>
    && impl::is_elementwise_comparable_by(^^T, ^^U, ^^is_operator_3way_comparable_v);
}  // namespace reflect_cpp26

#undef REFLECT_CPP26_EXTRACT

#endif  // REFLECT_CPP26_UTILS_COMPARE_TUPLE_HPP
