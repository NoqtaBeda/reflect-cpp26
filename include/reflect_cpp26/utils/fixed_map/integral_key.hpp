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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP

#include <reflect_cpp26/utils/fixed_map/impl/integral_dense.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/integral_empty.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/integral_general.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/integral_sparse.hpp>

namespace reflect_cpp26 {
struct integral_key_map_options {
  // Whether input key-value-pair range is already sorted.
  // UB or wrong result may occur if this flag is set as true but the input
  // range is not sorted actually.
  bool already_sorted = false;
  // Whether input key-value-pair range is already deduplicated.
  // UB or wrong result may occur if this flag is set as true but the input
  // range is not deduplicated actually.
  bool already_unique = false;
  // Whether the value-initialized default value is always invalid value.
  bool default_value_is_always_invalid = false;
  // Mimimum load factor for dense integral-key flat map, where holes are
  // filled with value-initialized Value{}.
  double min_load_factor = 0.5;
  // Dense subrange length threshold. If the longest dense subrange
  // has length no less than this threshold, then fast lookup is enabled
  // for this dense subrange.
  size_t dense_part_threshold = 4;
  // Sparse subrange length threshold. If the length of a sparse (sub-)range
  // is no less than this threshold, binary search is applied during lookup.
  // Linear search is applied otherwise.
  size_t binary_search_threshold = 8;
};

namespace impl {
template <enum_type E, class Nested>
struct integral_key_map_wrapper {
  using key_type = E;
  using value_type = typename Nested::value_type;

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

template <enum_type E, class Nested>
consteval auto make_integral_key_map_wrapper(Nested nested) {
  return integral_key_map_wrapper<E, Nested>{._nested = std::move(nested)};
}

template <class KVPairRange>
consteval auto find_longest_dense_subrange(
  const KVPairRange& sorted_kv_pairs, double min_load_factor)
{
  auto is_dense_closed_range = [min_load_factor](auto head, auto tail) {
    auto n_slots = static_cast<double>(get_first(*tail))
                 - static_cast<double>(get_first(*head)) + 1.0;
    auto n_non_hole_entries = static_cast<double>(tail - head + 1);
    return n_slots * min_load_factor <= n_non_hole_entries;
  };
  auto [begin, end] = std::ranges::subrange{sorted_kv_pairs};
  // Fast path: fully continuous
  if (is_dense_closed_range(begin, end - 1)) {
    return std::ranges::subrange{begin, end};
  }
  auto max_len_head = begin;
  auto max_len_tail = begin;
  for (auto head = begin, tail = begin + 1; tail < end; ++tail) {
    // Finds longest [head, tail]
    for (; head < tail && !is_dense_closed_range(head, tail); ++head) {}
    if (tail - head > max_len_tail - max_len_head) {
      max_len_head = head;
      max_len_tail = tail;
    }
  }
  return std::ranges::subrange{max_len_head, max_len_tail + 1};
}

template <integral_key_kv_pair KVPair>
consteval auto make_integral_key_map_impl(
  std::vector<KVPair> kv_pairs, integral_key_map_options options)
  -> std::meta::info
{
  // (1) Empty
  if (kv_pairs.empty()) {
    auto res = make_empty_integral_key_map<KVPair>();
    return std::meta::reflect_constant(res);
  }
  if (!options.already_sorted) {
    std::ranges::sort(kv_pairs, {}, get_first);
  }
  if (!options.already_unique) {
    auto dup_pos = std::ranges::adjacent_find(kv_pairs, {}, get_first);
    if (dup_pos != kv_pairs.end()) {
      compile_error("Duplicated keys are not allowed.");
    }
  }
  auto [dense_begin, dense_end] =
    find_longest_dense_subrange(kv_pairs, options.min_load_factor);
  // (2) Dense
  if (kv_pairs.size() == dense_end - dense_begin) {
    auto res = make_dense_integral_key_map(
      dense_begin, dense_end, options.default_value_is_always_invalid);
    return std::meta::reflect_constant(res);
  }
  // (3) Sparse
  if (dense_end - dense_begin < options.dense_part_threshold) {
    auto res = make_sparse_integral_key_map(
      kv_pairs.cbegin(), kv_pairs.cend(), options.binary_search_threshold);
    return std::meta::reflect_constant(res);
  }
  // (4) General
  auto general_options = general_integral_key_map_options{
    .default_value_is_always_invalid = options.default_value_is_always_invalid,
    .binary_search_threshold = options.binary_search_threshold,
  };
  auto general_res = make_general_integral_key_map(
    kv_pairs.cbegin(), dense_begin, dense_end, kv_pairs.cend(),
    general_options);
  return std::meta::reflect_constant(general_res);
}

template <std::ranges::input_range KVPairRange>
consteval auto make_integral_key_map(
  KVPairRange&& kv_pairs, integral_key_map_options options = {})
  -> std::meta::info
{
  constexpr auto input_is_vector =
    is_template_instance_of_v<KVPairRange, std::vector>;
  if constexpr (input_is_vector) {
    return make_integral_key_map_impl(
      std::forward<KVPairRange>(kv_pairs), options);
  } else {
    auto vec = std::forward<KVPairRange>(kv_pairs)
             | std::ranges::to<std::vector>();
    return make_integral_key_map_impl(std::move(vec), options);
  }
}

template <std::meta::info V>
constexpr auto extract_integral_key_fixed_map_v =
  extract_fixed_map_v<extract<V>()>;
} // namespace impl
} // namespace reflect_cpp26

#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ...)                 \
  [&]() consteval {                                                         \
    constexpr auto meta_res =                                               \
      reflect_cpp26::impl::make_integral_key_map(kv_pairs, ##__VA_ARGS__);  \
    return reflect_cpp26::impl::extract_integral_key_fixed_map_v<meta_res>; \
  }()

#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP_WRAPPER(E, kv_pairs, ...) \
  reflect_cpp26::impl::make_integral_key_map_wrapper<E>(               \
    REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ##__VA_ARGS__))

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP
