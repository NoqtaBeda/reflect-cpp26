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

#ifndef REFLECT_CPP26_ENUM_IMPL_ENUM_HASH_ENTRY_SEARCH_HPP
#define REFLECT_CPP26_ENUM_IMPL_ENUM_HASH_ENTRY_SEARCH_HPP

#include <reflect_cpp26/enum/impl/constants.hpp>
#include <reflect_cpp26/enum/impl/enum_entry.hpp>
#include <reflect_cpp26/enum/impl/hash_collision_check.hpp>
#include <reflect_cpp26/enum/enum_count.hpp>

namespace reflect_cpp26::impl {
// Precondition: no hash collision
consteval auto is_valid_hash_modulo(
  std::span<const enum_hash_entry> entries, size_t mod) -> bool
{
  auto vis = std::vector<bool>(mod);
  for (const auto& e: entries) {
    auto i = e.name_hash % mod;
    if (vis[i]) {
      return false;
    }
    vis[i] = true;
  }
  return true;
}

consteval size_t get_best_hash_modulo(std::span<const enum_hash_entry> entries)
{
  using namespace enum_constants;
  auto n = entries.size();
  auto limit = n * inv_min_load_factor;
  for (auto cur = std::bit_ceil(n); cur <= limit; cur *= 2) {
    if (is_valid_hash_modulo(entries, cur)) {
      return cur;
    }
  }
  for (auto cur = n + (n % 2 == 0); cur <= limit; cur += 2) {
    if (is_valid_hash_modulo(entries, cur)) {
      return cur;
    }
  }
  return npos;
}

template <class E>
  /* requires (std::is_enum_v<E>) */
consteval auto make_enum_hash_entry_sparse_list()
  -> std::vector<enum_hash_entry>
{
  // Precondition: no hash collision or zero hash value.
  auto entries = make_enum_hash_entry_list<E>();
  auto mod = get_best_hash_modulo(entries);
  if (mod == npos) {
    return {}; // Empty hash table on failure
  }
  auto res = std::vector<enum_hash_entry>(mod);
  for (const auto& e: entries) {
    res[e.name_hash % mod] = e;
  }
  return res;
}

// Linear sorted list
template <class E>
constexpr auto enum_hash_entry_dense_list_v =
  reflect_cpp26::define_static_array(make_enum_hash_entry_list<E>());

// Hash table
template <class E>
constexpr auto enum_hash_entry_sparse_list_v =
  reflect_cpp26::define_static_array(make_enum_hash_entry_sparse_list<E>());

constexpr auto enum_hash_binary_search_with_collision(
  meta_span<enum_hash_entry> entries, std::string_view str)
  -> const enum_hash_entry*
{
  auto str_hash = bkdr_hash64(str);
  auto [first, last] = std::ranges::equal_range(
    entries, str_hash, {}, &enum_hash_entry::name_hash);
  for (; first < last; ++first) {
    if (str == first->name) {
      return first;
    }
  }
  return nullptr;
}

constexpr auto enum_hash_linear_search(
  meta_span<enum_hash_entry> entries, std::string_view str)
  -> const enum_hash_entry*
{
  auto str_hash = bkdr_hash64(str);
  auto [first, last] = entries;
  for (; first < last; ++first) {
    if (first->name_hash == str_hash && first->name == str) {
      return first;
    }
  }
  return nullptr;
}

constexpr auto enum_hash_binary_search(
  meta_span<enum_hash_entry> entries, std::string_view str)
  -> const enum_hash_entry*
{
  auto str_hash = bkdr_hash64(str);
  auto [first, last] = entries;
  while (first < last) {
    const auto* mid = first + (last - first) / 2;
    if (mid->name_hash == str_hash) {
      return (mid->name == str) ? mid : nullptr;
    }
    if (mid->name_hash < str_hash) {
      first = mid + 1; // [mid+1, right)
    } else {
      last = mid; // [left, mid)
    }
  }
  return nullptr;
}

constexpr auto enum_hash_search_dispatch(
  meta_span<enum_hash_entry> entries, std::string_view str)
  -> const enum_hash_entry*
{
  using namespace enum_constants;
  return entries.size() >= enable_binary_search_threshold
    ? enum_hash_binary_search(entries, str)
    : enum_hash_linear_search(entries, str);
}

// Precondition: str is non-empty
constexpr auto enum_hash_table_search(
  meta_span<enum_hash_entry> entries, std::string_view str)
  -> const enum_hash_entry*
{
  auto str_hash = bkdr_hash64(str);
  const auto* pos = entries.data() + str_hash % entries.size();
  return (pos->name_hash == str_hash && pos->name == str) ? pos : nullptr;
}

template <class E>
  /* requires (std::is_enum_v<E>) */
constexpr auto enum_hash_search(std::string_view str) -> const enum_hash_entry*
{
  using namespace enum_constants;
  constexpr auto enables_table_lookup =
    enum_count<E>() >= enable_hash_table_lookup_threshold &&
    enum_count<E>() < disable_hash_table_lookup_threshold;

  if (str.empty() || enum_count<E>() == 0) {
    return nullptr;
  }
  if constexpr (enum_name_has_hash_collision_v<E>) {
    return enum_hash_binary_search_with_collision(
      enum_hash_entry_dense_list_v<E>, str);
  } else if constexpr (!enables_table_lookup) {
    return enum_hash_search_dispatch(enum_hash_entry_dense_list_v<E>, str);
  } else {
    constexpr auto hash_table = enum_hash_entry_sparse_list_v<E>;
    if constexpr (hash_table.empty()) { // Fallback on failure
      return enum_hash_search_dispatch(enum_hash_entry_dense_list_v<E>, str);
    } else {
      return enum_hash_table_search(hash_table, str);
    }
  }
}
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_ENUM_IMPL_ENUM_HASH_ENTRY_SEARCH_HPP
