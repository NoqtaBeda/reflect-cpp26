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

#ifndef REFLECT_CPP26_UTILS_CONSTANT_HPP
#define REFLECT_CPP26_UTILS_CONSTANT_HPP

#include <limits>
#include <type_traits>

namespace reflect_cpp26 {
/**
 * Convenient wrapper for std::integral_constant.
 * Note: despite the naming 'integral constant', V can be anything that is
 * compile-time evaluable.
 */
template <auto V>
using constant_t = std::integral_constant<decltype(V), V>;

/**
 * Convenient wrapper for std::integral_constant.
 * Note: despite the naming 'integral constant', V can be anything that is
 * compile-time evaluable.
 */
template <auto V>
constexpr auto constant_v = constant_t<V>{};

/**
 * Null index.
 */
constexpr auto npos = std::numeric_limits<size_t>::max();
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_CONSTANT_HPP
