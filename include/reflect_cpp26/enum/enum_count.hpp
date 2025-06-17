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

#ifndef REFLECT_CPP26_ENUM_ENUM_COUNT_HPP
#define REFLECT_CPP26_ENUM_ENUM_COUNT_HPP

#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/config.h>
#include <ranges>
#include <utility>

namespace reflect_cpp26 {
template <class E>
constexpr auto enum_count_v = enumerators_of(^^E).size();

template <class E>
consteval auto enum_unique_count_impl() -> size_t
{
  auto values = enumerators_of(^^E)
    | std::views::transform([](std::meta::info e) {
        return std::to_underlying(extract<E>(e));
      })
    | std::ranges::to<std::vector>();
  std::ranges::sort(values);
  auto [pos, _] = std::ranges::unique(values);
  return pos - values.begin();
}

template <class E>
constexpr auto enum_unique_count_v = enum_unique_count_impl<E>();

/**
 * Gets the number of entries of enum type E.
 */
template <enum_type E>
constexpr auto enum_count() -> size_t {
  return enum_count_v<std::remove_cv_t<E>>;
}

/**
 * Gets the number of entries of enum type E with unique value.
 */
template <enum_type E>
constexpr auto enum_unique_count() -> size_t {
  return enum_unique_count_v<std::remove_cv_t<E>>;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_COUNT_HPP
