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

#ifndef REFLECT_CPP26_FIXED_MAP_INTEGRAL_KEY_HPP
#define REFLECT_CPP26_FIXED_MAP_INTEGRAL_KEY_HPP

#include <reflect_cpp26/fixed_map/candidates/enum_wrapper.hpp>
#include <reflect_cpp26/fixed_map/candidates/integral_general.hpp>
#include <reflect_cpp26/type_operations/to_structural.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>

namespace reflect_cpp26 {
struct integral_key_fixed_map_options {
  bool already_sorted = false;
  bool already_unique = false;
  bool adjusts_alignment = false;
  double min_load_factor = 0.5;
  size_t dense_lookup_threshold = 4;
  size_t binary_search_threshold = 8;
};

namespace impl::map {
consteval bool is_kv_pair_with_ikey(std::meta::info T) {
  if (!extract<bool>(^^pair_like, T)) {
    return false;
  }
  auto K = remove_cvref(tuple_element(0, T));
  if (is_enum_type(K)) {
    return true;
  }
  if (is_same_type(K, ^^bool)) {
    return false;
  }
  return is_integral_type(K);
}

template <class KVPair>
concept kv_pair_with_ikey = is_kv_pair_with_ikey(std::meta::remove_cv(^^KVPair));

template <class K, class V>
consteval auto find_longest_dense_subrange(const std::vector<meta_tuple<K, V>>& sorted_kv_pairs,
                                           double min_load_factor)
/* -> std::pair<iterator, iterator> */
{
  auto is_dense_closed_range = [min_load_factor](auto head, auto tail) {
    auto n_slots =
        static_cast<double>(tail->elements.first) - static_cast<double>(head->elements.first) + 1.0;
    auto n_non_hole_entries = static_cast<double>(tail - head + 1);
    return n_slots * min_load_factor <= n_non_hole_entries;
  };
  auto begin = sorted_kv_pairs.begin();
  auto end = sorted_kv_pairs.end();
  // Fast path: fully continuous
  if (is_dense_closed_range(begin, end - 1)) {
    return std::pair{begin, end};
  }
  auto max_len_head = begin;
  auto max_len_tail = begin;
  for (auto head = begin, tail = begin + 1; tail < end; ++tail) {
    // Finds longest [head, tail]
    for (; head < tail && !is_dense_closed_range(head, tail); ++head) {
    }
    if (tail - head > max_len_tail - max_len_head) {
      max_len_head = head;
      max_len_tail = tail;
    }
  }
  return std::pair{max_len_head, max_len_tail + 1};
}

template <class K, class V>
consteval auto make_with_ikey(std::vector<meta_tuple<K, V>> kv_pairs,
                              const integral_key_fixed_map_options& options) -> std::meta::info {
  // (1) Empty
  if (kv_pairs.empty()) {
    return make_empty_with_ikey<V>();
  }
  // Preprocessing & duplication check
  if (!options.already_sorted) {
    std::ranges::sort(kv_pairs, {}, get_first);
  }
  if (!options.already_unique) {
    auto dup_pos = std::ranges::adjacent_find(kv_pairs, {}, get_first);
    if (dup_pos != kv_pairs.end()) {
      compile_error("Duplicated keys are not allowed.");
    }
  }
  auto [dense_begin, dense_end] = find_longest_dense_subrange(kv_pairs, options.min_load_factor);
  if (kv_pairs.size() == dense_end - dense_begin) {
    // (2) Dense
    auto dense_options = dense_with_ikey_options{
        .adjusts_alignment = options.adjusts_alignment,
    };
    return make_dense_with_ikey(std::span{std::as_const(kv_pairs)}, dense_options);
  }
  if (dense_end - dense_begin < options.dense_lookup_threshold) {
    // (3) Sparse
    auto sparse_options = sparse_with_ikey_options{
        .adjusts_alignment = options.adjusts_alignment,
        .binary_search_threshold = options.binary_search_threshold,
    };
    return make_sparse_with_ikey(std::span{std::as_const(kv_pairs)}, sparse_options);
  }
  // (4) General
  auto general_options = general_with_ikey_options{
      .adjusts_alignment = options.adjusts_alignment,
      .binary_search_threshold = options.binary_search_threshold,
  };
  auto left_sparse = std::span{kv_pairs.cbegin(), dense_begin};
  auto right_sparse = std::span{dense_end, kv_pairs.cend()};
  return make_general_with_ikey(left_sparse, right_sparse, general_options);
}
}  // namespace impl::map

template <std::ranges::input_range KVPairRange>
  requires(impl::map::kv_pair_with_ikey<std::ranges::range_value_t<KVPairRange>>)
consteval auto make_integral_key_fixed_map(const KVPairRange& kv_pairs,
                                           const integral_key_fixed_map_options& options = {})
    -> std::meta::info {
  using KVPair = std::ranges::range_value_t<KVPairRange>;
  using K = std::tuple_element_t<0, KVPair>;

  if constexpr (std::is_enum_v<K>) {
    // (1) Enum key
    auto convert_fn = [](const auto& kv_pair) {
      auto [k, v] = kv_pair;
      return meta_tuple{std::to_underlying(k), to_structural(v)};
    };
    auto converted = kv_pairs | std::views::transform(convert_fn) | std::ranges::to<std::vector>();
    auto underlying = impl::map::make_with_ikey(std::move(converted), options);

    auto params_il = {^^K, underlying};
    return std::meta::substitute(^^impl::map::enum_wrapper_v, params_il);
  } else {
    // (2) Integeral key
    auto converted =
        kv_pairs | std::views::transform(to_structural) | std::ranges::to<std::vector>();
    return impl::map::make_with_ikey(std::move(converted), options);
  }
}
}  // namespace reflect_cpp26

#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ...) \
  [:reflect_cpp26::make_integral_key_fixed_map(kv_pairs, ##__VA_ARGS__):]

#endif  // REFLECT_CPP26_FIXED_MAP_INTEGRAL_KEY_HPP
