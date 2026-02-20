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

#ifndef REFLECT_CPP26_FIXED_MAP_STRING_KEY_HPP
#define REFLECT_CPP26_FIXED_MAP_STRING_KEY_HPP

#include <reflect_cpp26/fixed_map/impl/string_by_hash_search.hpp>
#include <reflect_cpp26/fixed_map/impl/string_by_hash_table.hpp>
#include <reflect_cpp26/fixed_map/impl/string_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/string_naive.hpp>
#include <reflect_cpp26/fixed_map/impl/string_with_hash_collision.hpp>
#include <reflect_cpp26/type_operations/to_structural.hpp>

namespace reflect_cpp26 {
struct string_key_fixed_map_options {
  bool already_ascii_only = false;
  bool already_unique = false;
  bool ascii_case_insensitive = false;
  bool adjusts_alignment = false;
  double min_load_factor = 0.5;
  size_t max_n_iterations = 64;
  size_t optimization_threshold = 4;
  size_t binary_search_threshold = 8;
};

namespace impl {
constexpr bool has_hash_collision(std::vector<uint64_t> hash_values) {
  std::ranges::sort(hash_values);
  return std::ranges::adjacent_find(hash_values) != hash_values.end();
}

// Precondition: All keys in kv_pairs are lower case
// if options.ascii_case_insensitive is true.
template <class KVPair>
consteval auto make_string_key_fixed_map_impl(std::vector<KVPair> kv_pairs,
                                              string_key_fixed_map_options options)
    -> std::meta::info {
  if (!options.already_unique) {
    std::ranges::sort(kv_pairs, {}, get_first);
    auto dup_pos = std::ranges::adjacent_find(kv_pairs, {}, get_first);
    if (dup_pos != kv_pairs.end()) {
      compile_error("Duplicated keys are not allowed.");
    }
  }
  if (options.ascii_case_insensitive && !options.already_ascii_only) {
    for (const auto& [k, _] : kv_pairs) {
      if (!is_ascii_string(k)) {
        compile_error("Only ASCII strings allowed.");
      }
    }
  }

  // (1) Empty or naive
  if (kv_pairs.empty()) {
    return make_empty_string_key_map<KVPair>();
  }
  if (kv_pairs.size() < options.optimization_threshold) {
    return make_naive_string_key_map(kv_pairs, options.ascii_case_insensitive);
  }

  auto hash_values = make_reserved_vector<uint64_t>(kv_pairs.size());
  for (const auto& [k, _] : kv_pairs) {
    hash_values.push_back(bkdr_hash64(k));
  }
  // (2) Hash binary search: slow path due to hash collision
  if (has_hash_collision(hash_values)) {
    auto sub_options = string_key_map_with_hash_collision_options{
        .case_insensitive = options.ascii_case_insensitive,
        .adjusts_alignment = options.adjusts_alignment,
    };
    return make_string_key_map_with_hash_collision(kv_pairs, hash_values, sub_options);
  }

  // (3) Hash-table-based (hash collision is excluded)
  auto hash_table_options = string_key_map_by_hash_table_options{
      .case_insensitive = options.ascii_case_insensitive,
      .adjusts_alignment = options.adjusts_alignment,
      .min_load_factor = options.min_load_factor,
      .max_n_iterations = options.max_n_iterations,
  };
  auto by_hash_table_opt =
      try_make_string_key_map_by_hash_table(kv_pairs, hash_values, hash_table_options);
  if (by_hash_table_opt) {
    return *by_hash_table_opt;
  }

  // (4) Hash linear or binary search: fast path without hash collision
  auto hash_search_options = string_key_map_by_hash_search_options{
      .case_insensitive = options.ascii_case_insensitive,
      .adjusts_alignment = options.adjusts_alignment,
      .binary_search_threshold = options.binary_search_threshold,
  };
  return make_string_key_map_by_hash_search(kv_pairs, hash_values, hash_search_options);
}
}  // namespace impl

template <std::ranges::input_range KVPairRange>
  requires(impl::string_key_kv_pair<std::ranges::range_value_t<KVPairRange>>)
consteval auto make_string_key_fixed_map(KVPairRange&& kv_pairs,
                                         string_key_fixed_map_options options = {})
    -> std::meta::info {
  if (options.ascii_case_insensitive) {
    auto transform_fn = [](const auto& kv_pair) {
      const auto& [k, v] = kv_pair;
      auto k_lower = reflect_cpp26::define_static_string(ascii_tolower(k));
      return std::pair{k_lower, to_structural(v)};
    };
    return impl::make_string_key_fixed_map_impl(
        kv_pairs | std::views::transform(transform_fn) | std::ranges::to<std::vector>(), options);
  } else {
    auto transform_fn = [](const auto& kv_pair) {
      const auto& [k, v] = kv_pair;
      auto k_static = reflect_cpp26::define_static_string(k);
      return std::pair{k_static, to_structural(v)};
    };
    return impl::make_string_key_fixed_map_impl(
        kv_pairs | std::views::transform(transform_fn) | std::ranges::to<std::vector>(), options);
  }
}
}  // namespace reflect_cpp26

#define REFLECT_CPP26_STRING_KEY_FIXED_MAP(kv_pairs, ...) \
  [:reflect_cpp26::make_string_key_fixed_map(kv_pairs, ##__VA_ARGS__):]

#endif  // REFLECT_CPP26_FIXED_MAP_STRING_KEY_HPP
