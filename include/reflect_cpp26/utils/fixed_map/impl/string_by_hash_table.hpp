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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_TABLE_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_TABLE_HPP

#include <reflect_cpp26/utils/fixed_map/impl/string_policy.hpp>

namespace reflect_cpp26::impl {
constexpr auto string_key_hash_map_probing_size = 1zU;
constexpr auto string_key_hash_map_probing_deltas = std::views::iota(
  0zU, string_key_hash_map_probing_size + 1);

template <class Policy, class KVPair, template <class, class> class Derived>
struct string_key_map_by_hash_table_base {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  using derived_type = Derived<Policy, KVPair>;
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;

  meta_span<wrapper_type> _entries;
  size_t _actual_size;

  constexpr size_t size() const {
    return _actual_size;
  }

  constexpr uint64_t get_modulo(uint64_t hash) const {
    return static_cast<const derived_type*>(this)->do_get_modulo(hash);
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    auto key_hash = Policy::hash(key);
    auto i = get_modulo(key_hash);
    template for (constexpr auto delta: string_key_hash_map_probing_deltas) {
      const auto& cur = _entries[i + delta].underlying;
      if (get_first(cur.underlying).head == nullptr) { break; }

      if (cur.hash == key_hash) {
        if (!Policy::equals(get_first(cur.underlying), key)) { break; }
        return {get_second(cur.underlying), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// By-hash-table (faster): Bucket size is always power of 2.
template <class Policy, class KVPair>
struct string_key_map_by_hash_table_fast
  : string_key_map_by_hash_table_base<
      Policy, KVPair, string_key_map_by_hash_table_fast>
{
  // Assertion: buchet_size_mask = 2^n - 1
  size_t _bucket_size_mask;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash & _bucket_size_mask;
  }
};

// By-hash-table (slower): Bucket size is arbitrary and modulo is more costly.
template <class Policy, class KVPair>
struct string_key_map_by_hash_table_slow
  : string_key_map_by_hash_table_base<
      Policy, KVPair, string_key_map_by_hash_table_slow>
{
  // bucket_size == entries.size() - string_key_hash_map_probing_size
  size_t _bucket_size;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash % _bucket_size;
  }
};

// -------- Builders --------

constexpr bool test_bucket_size(
  const std::vector<uint64_t>& hash_values, std::vector<uint8_t>& vis_buffer,
  size_t bucket_size)
{
  // We use uint8_t to prevent bit operation overhead with std::vector<bool>.
  std::ranges::fill_n(
    vis_buffer.begin(), bucket_size + string_key_hash_map_probing_size, 0);
  for (auto h: hash_values) {
    auto ok = false;
    auto base_index = h % bucket_size;
    for (auto delta = 0zU; delta <= string_key_hash_map_probing_size; delta++) {
      if (!vis_buffer[base_index + delta]) {
        vis_buffer[base_index + delta] = ok = true;
        break;
      }
    }
    if (!ok) { return false; }
  }
  return true;
}

constexpr auto find_best_bucket_size(
  const std::vector<uint64_t>& hash_values, double min_load_factor) -> size_t
{
  auto size = hash_values.size();
  auto limit = static_cast<size_t>(size / min_load_factor);
  // We use uint8_t to prevent bit operation overhead with std::vector<bool>.
  auto vis_buffer =
    std::vector<uint8_t>(limit + string_key_hash_map_probing_size);

  for (auto n = std::bit_ceil(size); n <= limit; n <<= 1) {
    if (test_bucket_size(hash_values, vis_buffer, n)) { return n; }
  }
  for (auto n = size + (size % 2 == 0); n <= limit; n += 2) {
    if (test_bucket_size(hash_values, vis_buffer, n)) { return n; }
  }
  return 0; // Failed
}

struct hash_table_based_string_key_map_result {
  std::meta::info type;
  std::meta::info entries;
  size_t actual_size;
  size_t bucket_size_or_mask;
};

template <class KVPair>
consteval auto make_hash_table_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  uint64_t bucket_size, bool case_insensitive)
  -> hash_table_based_string_key_map_result
{
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  auto table = std::vector<wrapper_type>(
    bucket_size + string_key_hash_map_probing_size);

  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    for (auto j = hash_values[i] % bucket_size; ; j++) {
      auto& cur = table[j].underlying;
      if (get_first(cur.underlying).head == nullptr) {
        cur.hash = hash_values[i];
        cur.underlying = kv_pairs[i];
        break;
      }
    }
  }
  auto is_fast = std::has_single_bit(bucket_size);
  auto policy_type = string_key_policy_type(case_insensitive);
  auto type_template = is_fast
    ? ^^string_key_map_by_hash_table_fast
    : ^^string_key_map_by_hash_table_slow;
  auto type = substitute(type_template, policy_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(table);
  return {
    .type = type,
    .entries = std::meta::reflect_constant(entries),
    .actual_size = kv_pairs.size(),
    .bucket_size_or_mask = bucket_size - !!is_fast,
  };
}

template <class KVPair>
consteval auto try_make_hash_table_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  bool case_insensitive, double min_load_factor)
  -> std::optional<hash_table_based_string_key_map_result>
{
  auto bucket_size = find_best_bucket_size(hash_values, min_load_factor);
  if (bucket_size != 0) {
    return make_hash_table_based_string_key_map(
      kv_pairs, hash_values, bucket_size, case_insensitive);
  }
  return std::nullopt;
}

REFLECT_CPP26_EXTRACT_FIXED_MAP(hash_table_based_string_key_map_result) =
  [] consteval {
    using Type = [: R.type :];
    return Type{{extract<R.entries>(), R.actual_size}, R.bucket_size_or_mask};
  }();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_TABLE_HPP
