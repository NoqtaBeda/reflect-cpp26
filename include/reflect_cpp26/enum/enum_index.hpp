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

#ifndef REFLECT_CPP26_ENUM_ENUM_INDEX_HPP
#define REFLECT_CPP26_ENUM_ENUM_INDEX_HPP

#include <reflect_cpp26/enum/impl/enum_maps.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26 {
template <enum_entry_order Order>
struct enum_index_by_t {
  template <enum_type E>
  static constexpr size_t operator()(E value) {
    auto key = impl::to_int64_or_uint64(value);
    auto [indices, found] = impl::enum_index_map_v<E>.get(key);
    return found ? get<Order>(indices) : npos;
  }
};

template <enum_entry_order Order>
struct enum_index_opt_by_t {
  template <enum_type E>
  static constexpr auto operator()(E value) -> std::optional<size_t> {
    auto key = impl::to_int64_or_uint64(value);
    auto [indices, found] = impl::enum_index_map_v<E>.get(key);
    if (found) {
      return get<Order>(indices);
    }
    return std::nullopt;
  }
};

struct enum_unique_index_t {
  template <enum_type E>
  static constexpr size_t operator()(E value) {
    auto key = impl::to_int64_or_uint64(value);
    auto [indices, found] = impl::enum_index_map_v<E>.get(key);
    return found ? indices.by_value_unique : npos;
  }
};

struct enum_unique_index_opt_t {
  template <enum_type E>
  static constexpr auto operator()(E value) -> std::optional<size_t> {
    auto key = impl::to_int64_or_uint64(value);
    auto [indices, found] = impl::enum_index_map_v<E>.get(key);
    if (found) {
      return indices.by_value_unique;
    }
    return std::nullopt;
  }
};

template <enum_entry_order Order>
constexpr auto enum_index_by = enum_index_by_t<Order>{};

template <enum_entry_order Order>
constexpr auto enum_index_opt_by = enum_index_opt_by_t<Order>{};

constexpr auto enum_index = enum_index_by<enum_entry_order::original>;
constexpr auto enum_index_opt = enum_index_opt_by<enum_entry_order::original>;

constexpr auto enum_unique_index = enum_unique_index_t{};
constexpr auto enum_unique_index_opt = enum_unique_index_opt_t{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_ENUM_ENUM_INDEX_HPP
