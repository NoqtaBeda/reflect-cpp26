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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_SEARCH_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_SEARCH_HPP

#include <reflect_cpp26/utils/fixed_map/impl/string_policy.hpp>

namespace reflect_cpp26::impl {
// Precondition: No hash collision.
template <class Policy, class KVPair>
struct string_key_map_by_hash_binary_search {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  meta_span<wrapper_type> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    auto key_hash = Policy::hash(key);
    for (auto head = _entries.head, tail = _entries.tail; head < tail; ) {
      const auto* mid = head + (tail - head) / 2;
      const auto& [mid_hash, mid_kv_pair] = mid->underlying;
      if (mid_hash == key_hash) {
        if (!Policy::equals(get_first(mid_kv_pair), key)) { break; }
        return {get_second(mid_kv_pair), true};
      }
      (mid_hash < key_hash) ? (head = mid + 1) : (tail = mid);
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// Precondition: No hash collision.
template <class Policy, class KVPair>
struct string_key_map_by_hash_linear_search {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  // Alignment is not needed for linear search.
  using wrapper_type = string_key_kv_pair_wrapper_with_hash<KVPair>;
  meta_span<wrapper_type> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    auto key_hash = Policy::hash(key);
    for (const auto& [hash, kv_pair]: _entries) {
      if (hash != key_hash) {
        continue;
      }
      if (!Policy::equals(get_first(kv_pair), key)) { break; }
      return {get_second(kv_pair), true};
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// -------- Builders --------

struct hash_search_based_string_key_map_result {
  std::meta::info type;
  std::meta::info entries;
};

template <class KVPair>
consteval auto make_hash_linear_search_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  bool case_insensitive) -> hash_search_based_string_key_map_result
{
  using wrapper_type = string_key_kv_pair_wrapper_with_hash<KVPair>;
  auto entries_vec = make_reserved_vector<wrapper_type>(kv_pairs.size());
  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    entries_vec.push_back({.hash = hash_values[i], .underlying = kv_pairs[i]});
  }
  auto policy_type = string_key_policy_type(case_insensitive);
  auto type = substitute(
    ^^string_key_map_by_hash_linear_search, policy_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(entries_vec);
  return {.type = type, .entries = std::meta::reflect_constant(entries)};
}

template <class KVPair>
consteval auto make_hash_search_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  bool case_insensitive, size_t binary_search_threshold)
  -> hash_search_based_string_key_map_result
{
  // Linear search
  if (kv_pairs.size() < binary_search_threshold) {
    return make_hash_linear_search_based_string_key_map(
      kv_pairs, hash_values, case_insensitive);
  }
  // ... or binary search
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  auto entries_vec = std::vector<wrapper_type>{};
  entries_vec.reserve(kv_pairs.size());
  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    entries_vec.push_back(
      {{.hash = hash_values[i], .underlying = kv_pairs[i]}});
  }
  std::ranges::sort(entries_vec, {}, [](const auto& v) {
    return v.underlying.hash;
  });

  auto policy_type = string_key_policy_type(case_insensitive);
  auto type = substitute(
    ^^string_key_map_by_hash_binary_search, policy_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(entries_vec);
  return {.type = type, .entries = std::meta::reflect_constant(entries)};
}

REFLECT_CPP26_EXTRACT_FIXED_MAP(hash_search_based_string_key_map_result) =
  [] consteval {
    using Type = [: R.type :];
    return Type{._entries = extract<R.entries>()};
  }();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_SEARCH_HPP
