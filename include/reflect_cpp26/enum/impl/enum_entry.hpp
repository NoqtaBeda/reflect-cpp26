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

#ifndef REFLECT_CPP26_ENUM_IMPL_ENUM_ENTRY_HPP
#define REFLECT_CPP26_ENUM_IMPL_ENUM_ENTRY_HPP

#include <reflect_cpp26/enum/enum_entries.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/string_hash.hpp>
#include <reflect_cpp26/utils/utility.hpp>
#include <bit>
#include <utility>

namespace reflect_cpp26::impl {
template <class T>
  /* requires std::is_integral_v<T> || std::is_enum_v<T> */
constexpr auto to_int64_or_uint64(T value)
{
  static_assert(sizeof(T) <= sizeof(int64_t),
    "Extended big integer types are not supported.");
  if constexpr (std::is_enum_v<T>) {
    return to_int64_or_uint64(std::to_underlying(value));
  } else if constexpr (std::is_signed_v<T>) {
    return static_cast<int64_t>(value); // Sign bit extension
  } else {
    return static_cast<uint64_t>(value); // Zero extension
  }
}

template <class Derived>
struct enum_entry_interface {
  template <class T>
    /* requires std::is_integral_v<T> || std::is_enum_v<T> */
  static constexpr uint64_t make_value(T input)
  {
    static_assert(sizeof(T) <= sizeof(int64_t),
      "Extended big integer types are not supported.");
    if constexpr (std::is_enum_v<T>) {
      return make_value(std::to_underlying(input));
    } else if constexpr (std::is_signed_v<T>) {
      auto s = static_cast<int64_t>(input); // Sign bit extension
      return static_cast<uint64_t>(s);
    } else {
      return static_cast<uint64_t>(input); // Zero extension
    }
  }

  constexpr int64_t value_as_signed() const {
    return static_cast<int64_t>(static_cast<const Derived*>(this)->value);
  }

  template <class T>
    /* requires std::is_integral_v<T> || std::is_enum_v<T> */
  constexpr auto value_as_signedness_of(T) const
  {
    if constexpr (std::is_enum_v<T>) {
      return value_as_signedness_of(std::underlying_type_t<T>{});
    } else if constexpr (std::is_signed_v<T>) {
      return value_as_signed();
    } else {
      return static_cast<const Derived*>(this)->value;
    }
  }
};

struct alignas(32) enum_value_entry : enum_entry_interface<enum_value_entry> {
  uint64_t value; // value as primary key
  meta_string_view name;
  uint16_t index_original_order;
  uint16_t index_sorted_by_value;
  uint16_t index_sorted_by_value_unique;
  uint16_t index_sorted_by_name;

  template <enum_entry_order Order>
  constexpr auto index_sorted_by() const -> uint16_t
  {
    if constexpr (Order == enum_entry_order::original) {
      return index_original_order;
    } else if constexpr (Order == enum_entry_order::by_value) {
      return index_sorted_by_value;
    } else if constexpr (Order == enum_entry_order::by_name) {
      return index_sorted_by_name;
    } else {
      static_assert(false, "Invalid enum entry order.");
    }
  }
};

constexpr size_t enum_hash_entry_alignment = std::bit_ceil(
  sizeof(uint64_t) + sizeof(uint64_t) + sizeof(meta_string_view));

struct alignas(32) enum_hash_entry : enum_entry_interface<enum_hash_entry> {
  uint64_t name_hash; // name hash as primary key
  uint64_t value;
  meta_string_view name;

  static constexpr auto compare_by_hash_strong_order(
    const enum_hash_entry& x, const enum_hash_entry& y) -> std::strong_ordering
  {
    auto cmp_hash = (x.name_hash <=> y.name_hash);
    if (cmp_hash != 0) {
      return cmp_hash;
    }
    return x.name <=> y.name;
  }

  static constexpr auto less_by_hash_strong_order(
    const enum_hash_entry& x, const enum_hash_entry& y) -> bool
  {
    return compare_by_hash_strong_order(x, y) < 0;
  }
};

template <class E>
consteval auto make_enum_value_entry_list() -> std::vector<enum_value_entry>
{
  static_assert(in_range<uint16_t>(enum_count_v<E>),
    "Enum types with more than 65535 entries are not supported.");

  auto entry_list = std::vector<enum_value_entry>{};
  auto cur_index = uint16_t{0};
  for (auto [e, str]: enum_entries<E>()) {
    entry_list.push_back({
      .value = enum_value_entry::make_value(e),
      .name = meta_string_view::from_std_string_view(str),
      .index_original_order = cur_index++,
    });
  }
  std::ranges::sort(entry_list, {}, &enum_value_entry::name);
  for (auto i = 0zU, n = entry_list.size(); i < n; i++) {
    entry_list[i].index_sorted_by_name = static_cast<uint16_t>(i);
  }
  if constexpr (std::is_signed_v<std::underlying_type_t<E>>) {
    std::ranges::sort(entry_list, {}, &enum_value_entry::value_as_signed);
  } else {
    std::ranges::sort(entry_list, {}, &enum_value_entry::value);
  }
  for (auto i = 0zU, n = entry_list.size(); i < n; i++) {
    entry_list[i].index_sorted_by_value = static_cast<uint16_t>(i);
  }
  auto [s, t] = std::ranges::unique(entry_list, {}, &enum_value_entry::value);
  entry_list.erase(s, t);
  for (auto i = 0zU, n = entry_list.size(); i < n; i++) {
    entry_list[i].index_sorted_by_value_unique = static_cast<uint16_t>(i);
  }
  return entry_list;
}

template <class E>
consteval auto make_enum_hash_entry_list() -> std::vector<enum_hash_entry>
{
  auto entry_list = std::vector<enum_hash_entry>{};
  for (auto [e, str]: enum_entries<E>()) {
    auto name = meta_string_view::from_std_string_view(str);
    entry_list.push_back({
      .name_hash = bkdr_hash64(str),
      .value = enum_hash_entry::make_value(e),
      .name = name,
    });
  }
  std::ranges::sort(entry_list, &enum_hash_entry::less_by_hash_strong_order);
  return entry_list;
}

struct enum_value_entry_table {
  meta_span<enum_value_entry> entries;
  size_t continuous_head = 0;
  size_t continuous_tail = 0;

  static constexpr auto make(meta_span<enum_value_entry> entries)
    -> enum_value_entry_table
  {
    auto res = enum_value_entry_table{};
    res.entries = entries;
    auto n = entries.size();
    if (n <= 1) {
      res.continuous_tail = n;
      return res;
    }
    size_t max_len_head = 0, max_len_tail = 0;
    size_t cur_head = 0, cur_tail = 1;
    while (cur_tail < n) {
      while (cur_tail < n
          && entries[cur_tail].value == entries[cur_tail - 1].value + 1) {
        ++cur_tail;
      }
      if (cur_tail - cur_head > max_len_tail - max_len_head) {
        max_len_head = cur_head;
        max_len_tail = cur_tail;
      }
      cur_head = cur_tail;
      cur_tail += 1;
    }
    res.continuous_head = max_len_head;
    res.continuous_tail = max_len_tail;
    return res;
  }

  constexpr size_t sparse_left_size() const {
    return continuous_head;
  }

  constexpr size_t continuous_size() const {
    return continuous_tail - continuous_head;
  }

  constexpr size_t sparse_right_size() const {
    return entries.size() - continuous_tail;
  }

  constexpr auto sparse_left_segment() const -> meta_span<enum_value_entry> {
    return entries.first(continuous_head);
  }

  constexpr auto continuous_segment() const -> meta_span<enum_value_entry> {
    return entries.subspan(continuous_head, continuous_size());
  }

  constexpr auto sparse_right_segment() const -> meta_span<enum_value_entry> {
    return entries.subspan(continuous_tail);
  }

  constexpr auto continuous_min() const -> const enum_value_entry& {
    return entries[continuous_head];
  }

  constexpr auto continuous_max() const -> const enum_value_entry& {
    return entries[continuous_tail - 1];
  }
};

template <class E>
constexpr auto enum_value_entry_table_entries_v =
  reflect_cpp26::define_static_array(make_enum_value_entry_list<E>());

template <class E>
constexpr auto enum_value_entry_table_v =
  enum_value_entry_table::make(enum_value_entry_table_entries_v<E>);
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_ENUM_IMPL_ENUM_ENTRY_HPP
