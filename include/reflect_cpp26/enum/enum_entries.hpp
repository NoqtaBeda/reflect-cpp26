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

#ifndef REFLECT_CPP26_ENUM_ENUM_ENTRIES_HPP
#define REFLECT_CPP26_ENUM_ENUM_ENTRIES_HPP

#include <reflect_cpp26/enum/enum_names.hpp>
#include <reflect_cpp26/enum/enum_values.hpp>
#include <algorithm>
#include <span>
#include <utility>

namespace reflect_cpp26 {
template <enum_type E>
using enum_entry_t = std::pair<std::remove_cv_t<E>, std::string_view>;

namespace impl {
template <class E, enum_entry_order Order>
consteval auto make_enum_entries() /* -> std::array<enum_entry_t<E>, N> */
{
  auto entries = std::array<enum_entry_t<E>, enum_count_v<E>>{};
  auto values = enum_values<E, Order>();
  auto names = enum_names<E, Order>();
  for (auto i = 0zU; i < enum_count_v<E>; i++) {
    entries[i].first = values[i];
    entries[i].second = names[i];
  }
  return entries;
}

template <class E, enum_entry_order Order>
constexpr auto enum_entries_v = make_enum_entries<E, Order>();
} // namespace impl

/**
 * Gets the list of (value, name) pairs.
 */
template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr auto enum_entries() -> std::span<const enum_entry_t<E>>
{
  return impl::enum_entries_v<std::remove_cv_t<E>, Order>;
}

/**
 * Gets the i-th (value, name) pair.
 */
template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr auto enum_entry(size_t index) -> enum_entry_t<E>
{
  constexpr auto entries = enum_entries<E, Order>();
  return entries[index];
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_ENTRIES_HPP
