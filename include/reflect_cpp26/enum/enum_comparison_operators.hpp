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

#ifndef REFLECT_CPP26_ENUM_ENUM_COMPARISON_OPERATORS_HPP
#define REFLECT_CPP26_ENUM_ENUM_COMPARISON_OPERATORS_HPP

#include <compare>
#include <reflect_cpp26/enum/impl/enum_operator_macros.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

/**
 * Supports comparison operators (operator <=>, ==) for any enum type by
 * using namespace reflect_cpp26::enum_comparison_operators.
 */
namespace reflect_cpp26::enum_comparison_operators {
template <enum_type E>
REFLECT_CPP26_DEFINE_ENUM_BINARY_OPERATOR_R(E, std::strong_ordering, <=>)

template <enum_type E>
REFLECT_CPP26_DEFINE_ENUM_BINARY_OPERATOR_R(E, bool, ==)
}  // namespace reflect_cpp26::enum_comparison_operators

/**
 * Defines free functions of comparison operators for enum type E
 * (operator <=>, ==).
 */
#define REFLECT_CPP26_DEFINE_ENUM_COMPARISON_OPERATORS(E)                   \
  REFLECT_CPP26_DEFINE_ENUM_BINARY_OPERATOR_R(E, std::strong_ordering, <=>) \
  REFLECT_CPP26_DEFINE_ENUM_BINARY_OPERATOR_R(E, bool, ==)

#endif  // REFLECT_CPP26_ENUM_ENUM_COMPARISON_OPERATORS_HPP
