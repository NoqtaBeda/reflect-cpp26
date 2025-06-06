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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CONDITIONAL_SWITCH_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CONDITIONAL_SWITCH_HPP

#include <type_traits>

namespace reflect_cpp26 {
template <auto C, class T>
struct condition_case {};

template <class T>
struct condition_default {};

template <auto V, class... Args>
struct conditional_switch {
  static_assert(false, "Value mismatch or invalid input.");
};

// Case match
template <auto V, auto C, class T, class... Args>
  requires (V == C)
struct conditional_switch<V, condition_case<C, T>, Args...> {
  using type = T;
};

// Case mismatch
template <auto V, auto C, class T, class... Args>
  requires (V != C)
struct conditional_switch<V, condition_case<C, T>, Args...> {
  using type = typename conditional_switch<V, Args...>::type;
};

// Default case: must be the last one
template <auto V, class T>
struct conditional_switch<V, condition_default<T>> {
  using type = T;
};

/**
 * Type switch-case with key V.
 * Args should be a series of conditional_switch<Ci, Ti> and an optional
 * trailing conditional_default<T>.
 * Example: See struct integral_to_integer below
 */
template <auto V, class... Args>
using conditional_switch_t = typename conditional_switch<V, Args...>::type;
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CONDITIONAL_SWITCH_HPP
