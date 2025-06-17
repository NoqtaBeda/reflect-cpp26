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

#ifndef REFLECT_CPP26_UTILS_IS_NULL_HPP
#define REFLECT_CPP26_UTILS_IS_NULL_HPP

#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>

/**
 * Predefined null-check functions.
 */

namespace reflect_cpp26 {
constexpr bool is_null(std::integral auto v) {
  return v == 0;  // value initialization result is 0
}

constexpr bool is_null(std::floating_point auto v) {
  return v == 0.0;  // value initialization result is 0.0
}

constexpr bool is_null(pointer_type auto v) {
  return v == nullptr;
}

template <class T, class C>
constexpr bool is_null(T C::* v) {
  return v == nullptr;
}

template <char_type CharT, class Traits>
constexpr bool is_null(std::basic_string_view<CharT, Traits> v) {
  // value initialization result is {nullptr, nullptr}
  return v.data() == nullptr;
}

template <char_type CharT>
constexpr bool is_null(meta_basic_string_view<CharT> v) {
  // value initialization result is {nullptr, nullptr}
  return v.head == nullptr;
}

template <char_type CharT, class Value>
constexpr bool is_null(const meta_tuple<meta_basic_string_view<CharT>, Value>& kv_pair) {
  // value initialization result is {nullptr, nullptr}
  return get<0>(kv_pair).head == nullptr;
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_IS_NULL_HPP
