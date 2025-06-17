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

#ifndef REFLECT_CPP26_ENUM_ENUM_INDEX_HPP
#define REFLECT_CPP26_ENUM_ENUM_INDEX_HPP

#include <reflect_cpp26/enum/impl/enum_maps.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26 {
/**
 * Gets the index of given enum value in specified order.
 * Returns npos if the value is not found.
 */
template <enum_entry_order Order = enum_entry_order::original, enum_type E>
constexpr auto enum_index(E value) -> size_t
{
  using ENoCV = std::remove_cv_t<E>;
  auto [indices, found] =
    impl::enum_index_map_v<ENoCV>.get(impl::to_int64_or_uint64(value));
  return found ? get<Order>(indices) : npos;
}

/**
 * Gets the index of given enum value in value order, duplicated value removed.
 * Returns npos if the value is not found.
 */
template <enum_type E>
constexpr auto enum_unique_index(E value) -> size_t
{
  using ENoCV = std::remove_cv_t<E>;
  auto [indices, found] =
    impl::enum_index_map_v<ENoCV>.get(impl::to_int64_or_uint64(value));
  return found ? indices.by_value_unique : npos;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_INDEX_HPP
