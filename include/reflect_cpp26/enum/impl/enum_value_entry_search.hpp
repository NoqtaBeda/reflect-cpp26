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

#ifndef REFLECT_CPP26_ENUM_IMPL_ENUM_VALUE_ENTRY_SEARCH_HPP
#define REFLECT_CPP26_ENUM_IMPL_ENUM_VALUE_ENTRY_SEARCH_HPP

#include <reflect_cpp26/enum/impl/constants.hpp>
#include <reflect_cpp26/enum/impl/enum_entry.hpp>
#include <reflect_cpp26/enum/impl/hash_collision_check.hpp>

namespace reflect_cpp26::impl {
constexpr auto enum_value_linear_search(
  meta_span<enum_value_entry> entries, uint64_t value)
  -> const enum_value_entry*
{
  auto [first, last] = entries;
  for (; first != last; ++first) {
    if (first->value == value) {
      return first;
    }
  }
  return nullptr;
}

template <class T>
  /* requires (is_same_as_one_of_v<T, int64_t, uint64_t>) */
constexpr auto enum_value_binary_search(
  meta_span<enum_value_entry> entries, T value) -> const enum_value_entry*
{
  auto [first, last] = entries;
  while (first < last) {
    auto mid = first + (last - first) / 2;
    if (mid->value == value) {
      return mid;
    }
    if (mid->value_as_signedness_of(value) < value) {
      first = mid + 1; // [mid+1, last)
    } else {
      last = mid; // [first, mid)
    }
  }
  return nullptr;
}

template <class T>
  /* requires (is_same_as_one_of_v<T, int64_t, uint64_t>) */
constexpr auto enum_value_search_dispatch(
  meta_span<enum_value_entry> entries, T value) -> const enum_value_entry*
{
  using namespace enum_constants;
  if (entries.size() >= enable_binary_search_threshold) {
    return enum_value_binary_search(entries, value);
  }
  return enum_value_linear_search(entries, value);
}

template <class E>
  /* requires (std::is_enum_v<E>) */
constexpr auto enum_value_search(E enum_value) -> const enum_value_entry*
{
  using namespace enum_constants;
  constexpr auto tb = enum_value_entry_table_v<E>;
  if (tb.entries.empty()) {
    return nullptr;
  }
  auto value = to_int64_or_uint64(enum_value);
  if (tb.continuous_size() < enable_value_table_lookup_threshold) {
    return enum_value_search_dispatch(tb.entries, value);
  }
  if (value < tb.continuous_min().value_as_signedness_of(value)) {
    return enum_value_search_dispatch(tb.sparse_left_segment(), value);
  }
  if (value > tb.continuous_max().value_as_signedness_of(value)) {
    return enum_value_search_dispatch(tb.sparse_right_segment(), value);
  }
  auto offset = value - tb.continuous_min().value_as_signedness_of(value);
  return tb.entries.data() + tb.continuous_head + offset;
}

template <class E>
  /* requires (std::is_enum_v<E>) */
constexpr auto enum_value_contains(E enum_value) -> bool
{
  using namespace enum_constants;
  constexpr auto tb = enum_value_entry_table_v<E>;
  if (tb.entries.empty()) {
    return false;
  }
  auto value = to_int64_or_uint64(enum_value);
  if (tb.continuous_size() < enable_value_table_lookup_threshold) {
    return enum_value_search_dispatch(tb.entries, value) != nullptr;
  }
  if (value < tb.continuous_min().value_as_signedness_of(value)) {
    auto pos = enum_value_search_dispatch(tb.sparse_left_segment(), value);
    return pos != nullptr;
  }
  if (value > tb.continuous_max().value_as_signedness_of(value)) {
    auto pos = enum_value_search_dispatch(tb.sparse_right_segment(), value);
    return pos != nullptr;
  }
  return true;
}
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_ENUM_IMPL_ENUM_VALUE_ENTRY_SEARCH_HPP
