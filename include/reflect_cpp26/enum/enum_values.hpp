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

#ifndef REFLECT_CPP26_ENUM_ENUM_VALUES_HPP
#define REFLECT_CPP26_ENUM_ENUM_VALUES_HPP

#include <reflect_cpp26/enum/enum_meta_entries.hpp>
#include <reflect_cpp26/utils/expand.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <ranges>

namespace reflect_cpp26 {
namespace impl {
template <class E>
consteval auto make_enum_values(std::span<const std::meta::info> entries)
  /* -> std::array<E, N> */
{
  auto res = std::array<E, enum_count_v<E>>{};
  auto index = 0zU;
  for (auto e: entries) {
    res[index++] = extract<E>(e);
  }
  return res;
}

template <class E, enum_entry_order Order>
constexpr auto enum_values_v =
  make_enum_values<E>(enum_meta_entries_v<E, Order>);
} // namespace impl

/**
 * Gets the list of enum values.
 */
template <class E, enum_entry_order Order = enum_entry_order::original>
constexpr auto enum_values() -> std::span<const std::remove_cv_t<E>>
{
  const auto& values = impl::enum_values_v<std::remove_cv_t<E>, Order>;
  return {values.begin(), values.end()};
}

/**
 * Gets the i-th enum value.
 */
template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr auto enum_value(size_t index) -> std::remove_cv_t<E>
{
  constexpr auto values = enum_values<E, Order>();
  return values[index];
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_VALUES_HPP
