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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_TABLE_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_TABLE_HPP

#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>

namespace reflect_cpp26::impl {
constexpr auto string_key_hash_map_probing_size = 1zU;
constexpr auto string_key_hash_map_probing_deltas =
    std::views::iota(0zU, string_key_hash_map_probing_size + 1);

template <bool AlignmentAdjusted,
          class Policy,
          class KVPair,
          template <bool, class, class> class Derived>
struct string_key_map_by_hash_table_base {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  using derived_type = Derived<AlignmentAdjusted, Policy, KVPair>;

  constexpr size_t size() const {
    return _actual_size;
  }

  constexpr uint64_t get_modulo(uint64_t hash) const {
    return static_cast<const derived_type*>(this)->do_get_modulo(hash);
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
      -> std::pair<result_type, bool> {
    auto key_hash = Policy::hash(key);
    auto i = get_modulo(key_hash);
    // Hash collision excluded
    template for (constexpr auto delta : string_key_hash_map_probing_deltas) {
      const auto& cur = unwrap(_entries[i + delta]);
      if (get_first(cur.underlying).head == nullptr) break;
      if (cur.hash == key_hash) {
        if (!Policy::equals(get_first(cur.underlying), key)) break;
        return {get_second(cur.underlying), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE

  using span_element_type = std::conditional_t<AlignmentAdjusted,
                                               alignment_adjusted_string_hash_wrapper<KVPair>,
                                               string_hash_wrapper<KVPair>>;

  meta_span<span_element_type> _entries;
  size_t _actual_size;
};

// By-hash-table (faster): Bucket size is always power of 2.
template <bool AlignmentAdjusted, class Policy, class KVPair>
struct string_key_map_by_hash_table_fast
    : string_key_map_by_hash_table_base<AlignmentAdjusted,
                                        Policy,
                                        KVPair,
                                        string_key_map_by_hash_table_fast> {
  // Assertion: buchet_size_mask = 2^n - 1
  size_t _bucket_size_mask;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash & _bucket_size_mask;
  }
};

// By-hash-table (slower): Bucket size is arbitrary and modulo is more costly.
template <bool AlignmentAdjusted, class Policy, class KVPair>
struct string_key_map_by_hash_table_slow
    : string_key_map_by_hash_table_base<AlignmentAdjusted,
                                        Policy,
                                        KVPair,
                                        string_key_map_by_hash_table_slow> {
  // bucket_size == entries.size() - string_key_hash_map_probing_size
  size_t _bucket_size;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash % _bucket_size;
  }
};

// -------- Factory --------

template <bool AA, class Policy, class KVPair>
constexpr auto string_key_map_by_hash_table_factory(const std::vector<KVPair>& kv_pairs,
                                                    const std::vector<uint64_t>& hash_values,
                                                    uint64_t bucket_size) -> std::meta::info {
  auto table =
      std::vector<string_hash_wrapper<KVPair>>(bucket_size + string_key_hash_map_probing_size);

  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    for (auto j = hash_values[i] % bucket_size;; j++) {
      auto& cur = table[j];
      if (get_first(cur.underlying).head != nullptr) {
        continue;
      }
      cur.hash = hash_values[i];
      cur.underlying = kv_pairs[i];
      break;
    }
  }
  auto entries = [&table]() {
    if constexpr (AA) {
      using span_element_type = alignment_adjusted_string_hash_wrapper<KVPair>;
      return span_element_type::make_static_array(table);
    } else {
      return reflect_cpp26::define_static_array(table);
    }
  }();
  auto n = kv_pairs.size();
  if (std::has_single_bit(bucket_size)) {
    auto res = string_key_map_by_hash_table_fast<AA, Policy, KVPair>{{entries, n}, bucket_size - 1};
    return std::meta::reflect_constant(res);
  } else {
    auto res = string_key_map_by_hash_table_slow<AA, Policy, KVPair>{{entries, n}, bucket_size};
    return std::meta::reflect_constant(res);
  }
}

// -------- Builders --------

struct string_key_map_by_hash_table_options {
  bool case_insensitive;
  bool adjusts_alignment;
  double min_load_factor;
  size_t max_n_iterations;
};

constexpr bool test_bucket_size(const std::vector<uint64_t>& hash_values,
                                std::vector<uint8_t>& vis_buffer,
                                size_t bucket_size) {
  constexpr auto P = string_key_hash_map_probing_size;
  // We use uint8_t to prevent bit operation overhead with std::vector<bool>.
  std::ranges::fill_n(vis_buffer.begin(), bucket_size + P, 0);
  for (auto h : hash_values) {
    auto ok = false;
    auto base_index = h % bucket_size;
    for (auto delta = 0zU; delta <= P; delta++) {
      if (!vis_buffer[base_index + delta]) {
        vis_buffer[base_index + delta] = ok = true;
        break;
      }
    }
    if (!ok) {
      return false;
    }
  }
  return true;
}

constexpr auto find_best_bucket_size(const std::vector<uint64_t>& hash_values,
                                     double min_load_factor,
                                     size_t max_n_iterations) -> size_t {
  auto size = hash_values.size();
  auto limit = static_cast<size_t>(size / min_load_factor);
  // We use uint8_t to prevent bit operation overhead with std::vector<bool>.
  auto vis_buffer = std::vector<uint8_t>(limit + string_key_hash_map_probing_size);

  auto n_iterations = 0zU;
  for (auto n = std::bit_ceil(size); n <= limit; n <<= 1) {
    if ((n_iterations++) >= max_n_iterations) {
      break;
    }
    if (test_bucket_size(hash_values, vis_buffer, n)) {
      return n;
    }
  }
  for (auto n = size + (size % 2 == 0); n <= limit; n += 2) {
    if ((n_iterations++) >= max_n_iterations) {
      break;
    }
    if (test_bucket_size(hash_values, vis_buffer, n)) {
      return n;
    }
  }
  return 0;  // Failed
}

template <class KVPair>
consteval auto make_string_key_map_by_hash_table(
    const std::vector<KVPair>& kv_pairs,
    const std::vector<uint64_t>& hash_values,
    const string_key_map_by_hash_table_options& options,
    uint64_t bucket_size) -> std::meta::info {
  using factory_fn_type =
      std::meta::info (*)(const std::vector<KVPair>&, const std::vector<uint64_t>&, uint64_t);

  auto AA = std::meta::reflect_constant(options.adjusts_alignment);
  auto policy = string_key_policy_type(options.case_insensitive);

  auto params_il = {AA, policy, ^^KVPair};
  auto factory_fn =
      extract<factory_fn_type>(substitute(^^string_key_map_by_hash_table_factory, params_il));
  return factory_fn(kv_pairs, hash_values, bucket_size);
}

template <class KVPair>
consteval auto try_make_string_key_map_by_hash_table(const std::vector<KVPair>& kv_pairs,
                                                     const std::vector<uint64_t>& hash_values,
                                                     string_key_map_by_hash_table_options options)
    -> std::optional<std::meta::info> {
  auto bucket_size =
      find_best_bucket_size(hash_values, options.min_load_factor, options.max_n_iterations);
  if (bucket_size != 0) {
    return make_string_key_map_by_hash_table(kv_pairs, hash_values, options, bucket_size);
  }
  return std::nullopt;
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_TABLE_HPP
