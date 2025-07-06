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

#ifndef REFLECT_CPP26_ENUM_IMPL_ENUM_OPERATOR_MACROS_HPP
#define REFLECT_CPP26_ENUM_IMPL_ENUM_OPERATOR_MACROS_HPP

#include <type_traits>

#define REFLECT_CPP26_DEFINE_ENUM_ASSIGNMENT_OPERATOR(E, op)  \
  constexpr E& operator op##=(E& x, E y) {                    \
    using U = std::underlying_type_t<E>;                      \
    auto u = static_cast<U>(x) op static_cast<U>(y);          \
    x = static_cast<E>(u);                                    \
    return x;                                                 \
  }

#define REFLECT_CPP26_DEFINE_ENUM_BINARY_OPERATOR(E, op)      \
  constexpr E operator op(E x, E y) {                         \
    using U = std::underlying_type_t<E>;                      \
    auto u = static_cast<U>(x) op static_cast<U>(y);          \
    return static_cast<E>(u);                                 \
  }

#define REFLECT_CPP26_DEFINE_ENUM_BINARY_OPERATOR_R(E, R, op) \
  constexpr R operator op(E x, E y) {                         \
    using U = std::underlying_type_t<E>;                      \
    auto u = static_cast<U>(x) op static_cast<U>(y);          \
    return static_cast<R>(u);                                 \
  }                                                           \

#define REFLECT_CPP26_DEFINE_ENUM_UNARY_OPERATOR(E, R, op)  \
  constexpr R operator op(E x) {                            \
    using U = std::underlying_type_t<E>;                    \
    auto u = op static_cast<U>(x);                          \
    return static_cast<R>(u);                               \
  }

#endif // REFLECT_CPP26_ENUM_IMPL_ENUM_OPERATOR_MACROS_HPP
