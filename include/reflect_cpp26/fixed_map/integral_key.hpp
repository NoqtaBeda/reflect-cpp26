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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP

#include <reflect_cpp26/fixed_map/impl/integral_dense.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_fully_dense.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_general.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_sparse.hpp>
#include <reflect_cpp26/type_operations/to_structural.hpp>

namespace reflect_cpp26 {
struct integral_key_fixed_map_options {
  bool already_sorted = false;
  bool already_unique = false;
  bool adjusts_alignment = false;
  bool default_value_is_always_invalid = false;
  double min_load_factor = 0.5;
  size_t dense_lookup_threshold = 4;
  size_t binary_search_threshold = 8;
};

namespace impl {
template <enum_type E, class Nested>
struct integral_key_map_enum_wrapper {
  using key_type = E;
  using value_type = typename Nested::value_type;
  using result_type = typename Nested::result_type;

  Nested _nested;

  constexpr size_t size() const {
    return _nested.size();
  }

  constexpr decltype(auto) get(E key) const {
    return _nested.get(std::to_underlying(key));
  }

  constexpr decltype(auto) operator[](E key) const {
    return _nested[std::to_underlying(key)];
  }
};

template <enum_type E, auto Nested>
constexpr auto integral_key_map_enum_wrapper_factory_v = std::meta::reflect_constant(
    integral_key_map_enum_wrapper<E, decltype(Nested)>{._nested = Nested});

template <class KVPairRange>
consteval auto find_longest_dense_subrange(const KVPairRange& sorted_kv_pairs,
                                           double min_load_factor)
/* -> std::pair<KVPairIter, KVPairIter> */
{
  auto is_dense_closed_range = [min_load_factor](auto head, auto tail) {
    auto n_slots =
        static_cast<double>(get_first(*tail)) - static_cast<double>(get_first(*head)) + 1.0;
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

template <integral_key_kv_pair KVPair>
consteval auto make_integral_key_fixed_map_impl(std::vector<KVPair> kv_pairs,
                                                integral_key_fixed_map_options options)
    -> std::meta::info {
  // (1) Empty
  if (kv_pairs.empty()) {
    return make_empty_integral_key_map<KVPair>();
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
  // (2) Dense
  auto dense_options = dense_integral_key_fixed_map_options{
      .adjusts_alignment = options.adjusts_alignment,
      .default_value_is_always_invalid = options.default_value_is_always_invalid,
  };
  if (kv_pairs.size() == dense_end - dense_begin) {
    return make_dense_integral_key_map(dense_begin, dense_end, dense_options);
  }
  // (3) Sparse
  auto sparse_options = sparse_integral_key_fixed_map_options{
      .adjusts_alignment = options.adjusts_alignment,
      .binary_search_threshold = options.binary_search_threshold,
  };
  if (dense_end - dense_begin < options.dense_lookup_threshold) {
    return make_sparse_integral_key_map(kv_pairs.cbegin(), kv_pairs.cend(), sparse_options);
  }
  // (4) General
  auto general_options = general_integral_key_fixed_map_options{
      .adjusts_alignment = options.adjusts_alignment,
      .default_value_is_always_invalid = options.default_value_is_always_invalid,
      .binary_search_threshold = options.binary_search_threshold,
  };
  return make_general_integral_key_map(
      kv_pairs.cbegin(), dense_begin, dense_end, kv_pairs.cend(), general_options);
}
}  // namespace impl

template <std::ranges::input_range KVPairRange>
  requires(impl::integral_key_kv_pair<std::ranges::range_value_t<KVPairRange>>)
consteval auto make_integral_key_fixed_map(KVPairRange kv_pairs,
                                           integral_key_fixed_map_options options = {})
    -> std::meta::info {
  using KVPair = std::ranges::range_value_t<KVPairRange>;
  using Key = std::tuple_element_t<0, KVPair>;

  if constexpr (std::is_enum_v<Key>) {
    auto transform_fn = [](const auto& kv_pair) {
      const auto& [k, v] = kv_pair;
      return std::pair{std::to_underlying(k), to_structural(v)};
    };
    auto nested_res = impl::make_integral_key_fixed_map_impl(
        kv_pairs | std::views::transform(transform_fn) | std::ranges::to<std::vector>(), options);

    auto params_il = {^^Key, nested_res};
    return extract<std::meta::info>(
        substitute(^^impl::integral_key_map_enum_wrapper_factory_v, params_il));
  } else {
    return impl::make_integral_key_fixed_map_impl(
        kv_pairs | std::views::transform(to_structural) | std::ranges::to<std::vector>(), options);
  }
}
}  // namespace reflect_cpp26

#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ...) \
  [:reflect_cpp26::make_integral_key_fixed_map(kv_pairs, ##__VA_ARGS__):]

#endif  // REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP
