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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_STRING_KEY_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_STRING_KEY_HPP

#include <reflect_cpp26/utils/fixed_map/impl/string_by_character.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/string_by_hash_search.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/string_by_hash_table.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/string_by_length.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/string_empty.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/string_naive.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/string_with_hash_collision.hpp>

namespace reflect_cpp26 {
struct string_key_map_options {
  // Whether the fixed map is built in a case-insensitive manner.
  bool case_insensitive = false;
  // Whether optimization is enabled by using the i-th character as key
  // where i is the index such that every input string differs at index i.
  bool enables_lookup_by_differed_character = false;
  // Mimimum load factor for length-based, ith-character-based or hash-based
  // string-key flat map, where holes are filled with value-initialized Value{}.
  double min_load_factor = 0.5;
  // Length threshold to enable optimized data structures. Naive linear list
  // searching is used if the input length is less than this threshold.
  size_t optimization_threshold = 4;
  // Used by the underlying integral-key data structure of length-based and
  // ith-character-based string-key flat map.
  size_t dense_part_threshold = 4;
  // Length threshold to enable binary search for hash-based or naive
  // string-key flat map. Linear search is applied otherwise.
  size_t binary_search_threshold = 8;
};
} // namespace reflect_cpp26

namespace reflect_cpp26::impl {
template <class KVPair>
consteval void check_string_key_map_input(
  const std::vector<KVPair>& kv_pairs, const string_key_map_options& options)
{
  if (options.case_insensitive) {
    for (const auto& [k, _]: kv_pairs) {
      if (is_ascii_string(k)) { continue; }
      compile_error("Only ASCII strings allowed.");
    }
  }
}

constexpr bool has_hash_collision(std::vector<uint64_t> hash_values)
{
  std::ranges::sort(hash_values);
  return std::ranges::adjacent_find(hash_values) != hash_values.end();
}

// Precondition: All keys in kv_pairs are lower case
// if options.case_insensitive is true.
template <class KVPair>
consteval auto make_string_key_map_impl(
  std::vector<KVPair> kv_pairs, string_key_map_options options)
  -> std::meta::info
{
  check_string_key_map_input(kv_pairs, options);
  // (1) Empty or naive
  if (kv_pairs.empty()) {
    auto res = make_empty_string_key_map<KVPair>();
    return std::meta::reflect_constant(res);
  }
  if (kv_pairs.size() < options.optimization_threshold) {
    auto res = make_naive_string_key_map(kv_pairs, options.case_insensitive);
    return std::meta::reflect_constant(res);
  }
  // (2) Length-based
  auto options_for_underlying = integral_key_map_options{
    .default_value_is_always_invalid = true,
    .min_load_factor = options.min_load_factor,
    .dense_part_threshold = options.dense_part_threshold,
    .binary_search_threshold = options.binary_search_threshold,
  };
  auto by_length_opt = try_make_length_based_string_key_map(
    kv_pairs, options_for_underlying, options.case_insensitive);
  if (by_length_opt) {
    return std::meta::reflect_constant(*by_length_opt);
  }
  // (3) ith-character-based
  if (options.enables_lookup_by_differed_character) {
    auto by_character_opt = try_make_character_based_string_key_map(
      kv_pairs, options_for_underlying, options.case_insensitive);
    if (by_character_opt) {
      return std::meta::reflect_constant(*by_character_opt);
    }
  }

  auto hash_values = make_reserved_vector<uint64_t>(kv_pairs.size());
  for (const auto& [k, _]: kv_pairs) {
    hash_values.push_back(bkdr_hash64(k));
  }
  // (4) Hash binary search: slow path due to hash collision
  if (has_hash_collision(hash_values)) {
    auto res = make_string_key_map_with_hash_collision(
      kv_pairs, hash_values, options.case_insensitive);
    return std::meta::reflect_constant(res);
  }
  // (5) Hash-table-based (hash collision is excluded)
  auto by_hash_table_opt = try_make_hash_table_based_string_key_map(
    kv_pairs, hash_values, options.case_insensitive, options.min_load_factor);
  if (by_hash_table_opt) {
    return std::meta::reflect_constant(*by_hash_table_opt);
  }
  // (6) Hash linear or binary search: fast path, without hash collision
  auto res = make_hash_search_based_string_key_map(
    kv_pairs, hash_values, options.case_insensitive,
    options.binary_search_threshold);
  return std::meta::reflect_constant(res);
}

template <std::ranges::input_range KVPairRange>
  requires (string_key_kv_pair<std::ranges::range_value_t<KVPairRange>>)
consteval auto make_string_key_map(
  KVPairRange&& kv_pairs, string_key_map_options options = {})
  -> std::meta::info
{
  using kv_pair_type = std::ranges::range_value_t<KVPairRange>;
  using kv_pair_key_type = std::tuple_element_t<0, kv_pair_type>;
  using kv_pair_value_type = std::tuple_element_t<1, kv_pair_type>;
  using element_type = typename kv_pair_key_type::value_type; // CharT

  using canonical_kv_pair_type =
    std::pair<meta_basic_string_view<element_type>, kv_pair_value_type>;
  auto converted = std::vector<canonical_kv_pair_type>();
  if (options.case_insensitive) {
    for (const auto& [k, v]: kv_pairs) {
      auto k_lower = reflect_cpp26::define_static_string(ascii_tolower(k));
      converted.emplace_back(k_lower, v);
    }
  } else {
    for (const auto& [k, v]: kv_pairs) {
      converted.emplace_back(reflect_cpp26::define_static_string(k), v);
    }
  }
  return make_string_key_map_impl(converted, options);
}

template <std::meta::info V>
constexpr auto extract_string_key_fixed_map_v =
  extract_fixed_map_v<extract<V>()>;
} // namespace reflect_cpp26::impl

#define REFLECT_CPP26_STRING_KEY_FIXED_MAP(kv_pairs, ...)                 \
  [&]() consteval {                                                       \
    constexpr auto meta_res = reflect_cpp26::impl::make_string_key_map(   \
        kv_pairs, ##__VA_ARGS__);                                         \
    return reflect_cpp26::impl::extract_string_key_fixed_map_v<meta_res>; \
  }()

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_STRING_KEY_HPP
