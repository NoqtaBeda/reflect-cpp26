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

#ifndef REFLECT_CPP26_UTILS_EXPAND_HPP
#define REFLECT_CPP26_UTILS_EXPAND_HPP

#include <reflect_cpp26/utils/config.h> // IWYU pragma: keep
#include <reflect_cpp26/utils/constant.hpp>
#include <functional>

/**
 * Alternative to C++26 expansion statements (if adopted to standard).
 * See section 2.3 of:
 * https://open-std.org/JTC1/SC22/WG21/docs/papers/2025/p2996r9.html
 */
#define REFLECT_CPP26_EXPAND(...) \
  [: ::reflect_cpp26::make_for_each_replicator(__VA_ARGS__) :]

/**
 * Alternative to C++26 expansion statements for index from 0 to n-1.
 * Equivalent to:
 * template for (constexpr size_t i: iota(0, n))
 */
#define REFLECT_CPP26_EXPAND_I(n)               \
  [: ::reflect_cpp26::make_for_each_replicator( \
    std::views::iota(size_t{0}, size_t{n})      \
  ) :]

/**
 * Alternative to C++26 expansion statements for reversed index from n-1 to 0.
 * Equivalent to:
 * template for (constexpr size_t i: iota(0, n) | reverse)
 */
#define REFLECT_CPP26_EXPAND_REV_I(n)                             \
  [: ::reflect_cpp26::make_for_each_replicator(                   \
    std::views::iota(size_t{0}, size_t{n}) | std::views::reverse  \
  ) :]

namespace reflect_cpp26 {
namespace impl {
template<auto... Vs>
constexpr auto for_each_replicator = constant<Vs...>{};
} // namespace impl

template<std::ranges::input_range Range>
consteval auto make_for_each_replicator(const Range& range)
{
  return substitute(
    ^^impl::for_each_replicator,
    range | std::views::transform([](const auto& r) {
      return std::meta::reflect_constant(r);
    }));
}

template <class T>
consteval auto make_for_each_replicator(std::initializer_list<T> range)
{
  return substitute(
    ^^impl::for_each_replicator,
    range | std::views::transform([](const auto& r) {
      return std::meta::reflect_constant(r);
    }));
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_EXPAND_HPP
