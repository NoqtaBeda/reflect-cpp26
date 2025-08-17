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

#ifndef REFLECT_CPP26_ENUM_ENUM_NAMES_HPP
#define REFLECT_CPP26_ENUM_ENUM_NAMES_HPP

#include <reflect_cpp26/enum/enum_meta_entries.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/ctype.hpp>

namespace reflect_cpp26 {
namespace impl {
template <class E, enum_entry_order Order>
constexpr auto enum_names()
{
  auto res = std::array<std::string_view, enum_count_v<E>>{};
  auto index = 0zU;
  for (auto e: enum_meta_entries_v<E, Order>) {
    res[index++] = std::define_static_string(std::meta::identifier_of(e));
  }
  return res;
}

template <class E, enum_entry_order Order>
constexpr auto enum_ic_names()
{
  auto res = std::array<std::string_view, enum_count_v<E>>{};
  auto index = 0zU;
  for (auto e: enum_meta_entries_v<E, Order>) {
    auto s = ascii_tolower(std::meta::identifier_of(e));
    res[index++] = std::define_static_string(s);
  }
  return res;
}

template <class E, enum_entry_order Order = enum_entry_order::original>
constexpr auto enum_names_v = enum_names<E, Order>();

template <class E, enum_entry_order Order = enum_entry_order::original>
constexpr auto enum_ic_names_v = enum_ic_names<E, Order>();
} // namespace impl

/**
 * Gets the list of enum names.
 */
template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr auto enum_names() -> std::span<const std::string_view>
{
  return impl::enum_names_v<std::remove_cv_t<E>, Order>;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_NAMES_HPP
