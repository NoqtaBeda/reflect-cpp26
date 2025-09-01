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

#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_dense.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_fully_dense.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_general.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_sparse.hpp>

namespace reflect_cpp26 {
struct integral_key_map_options {
  // Whether input key-value-pair range is already sorted.
  // UB or wrong result may occur if this flag is set as true but the input
  // range is not sorted actually.
  bool already_sorted = false;
  // Whether input keys are already deduplicated.
  // UB or wrong result may occur if this flag is set as true but the input
  // keys are not deduplicated actually.
  bool already_unique = false;
  // Whether alignment optimization is enabled.
  // If enabled, then the elements of underlying ranges will be aligned to
  // 2^n bytes for maximized random-access performance.
  bool adjusts_alignment = false;
  // Whether the value-initialized default value is always regarded as invalid.
  // Some underlying data structures (currently, dense flat map) may require an
  // extra field 'is_valid' for each underlying element to check whether the
  // element represents valid user input or a hole.
  // To eliminate the 'is_valid' field to reduce memory usage, you need to:
  // (1) set this option to true;
  // (2) provide a user-defined predicate is_null(v) for the value type which
  //     can be found via argument-depend lookup (ADL). For primitive types and
  //     some frequently used C++ standard types, is_null(v) is predefined in
  //     reflect_cpp26/fixed_map/is_null.hpp;
  // (3) make sure that for each value v in input kv-pairs, v != T{} always
  //     holds where T is input value type. UB or wrong result may occur
  //     otherwise.
  bool default_value_is_always_invalid = false;
  // Mimimum load factor for dense flat map.
  double min_load_factor = 0.5;
  // Dense subrange length threshold. If the longest dense subrange in input
  // kv-pair list has length no less than this threshold, then fast lookup is
  // enabled for this dense subrange.
  size_t dense_lookup_threshold = 4;
  // Sparse subrange length threshold. If the length of a sparse (sub-)range
  // is no less than this threshold, binary search is applied during lookup.
  // Linear search is applied otherwise.
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
constexpr auto integral_key_map_enum_wrapper_factory_v =
  std::meta::reflect_constant(
    integral_key_map_enum_wrapper<E, decltype(Nested)>{._nested = Nested});

template <class KVPairRange>
consteval auto find_longest_dense_subrange(
  const KVPairRange& sorted_kv_pairs, double min_load_factor)
  /* -> std::pair<KVPairIter, KVPairIter> */
{
  auto is_dense_closed_range = [min_load_factor](auto head, auto tail) {
    auto n_slots = static_cast<double>(get_first(*tail))
                 - static_cast<double>(get_first(*head)) + 1.0;
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
    for (; head < tail && !is_dense_closed_range(head, tail); ++head) {}
    if (tail - head > max_len_tail - max_len_head) {
      max_len_head = head;
      max_len_tail = tail;
    }
  }
  return std::pair{max_len_head, max_len_tail + 1};
}

template <integral_key_kv_pair KVPair>
consteval auto make_integral_key_map_impl(
  std::vector<KVPair> kv_pairs, integral_key_map_options options)
  -> std::meta::info
{
  // (1) Empty
  if (kv_pairs.empty()) {
    return make_empty_integral_key_map<KVPair>();
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
  auto dense_options = dense_integral_key_map_options{
    .adjusts_alignment = options.adjusts_alignment,
    .default_value_is_always_invalid = options.default_value_is_always_invalid,
  };
  if (kv_pairs.size() == dense_end - dense_begin) {
    return make_dense_integral_key_map(dense_begin, dense_end, dense_options);
  }
  // (3) Sparse
  auto sparse_options = sparse_integral_key_map_options{
    .adjusts_alignment = options.adjusts_alignment,
    .binary_search_threshold = options.binary_search_threshold,
  };
  if (dense_end - dense_begin < options.dense_lookup_threshold) {
    return make_sparse_integral_key_map(
      kv_pairs.cbegin(), kv_pairs.cend(), sparse_options);
  }
  // (4) General
  auto general_options = general_integral_key_map_options{
    .adjusts_alignment = options.adjusts_alignment,
    .default_value_is_always_invalid = options.default_value_is_always_invalid,
    .binary_search_threshold = options.binary_search_threshold,
  };
  return make_general_integral_key_map(
    kv_pairs.cbegin(), dense_begin, dense_end, kv_pairs.cend(),
    general_options);
}
} // namespace impl

/**
 * Generates a fixed map in compile-time whose key is of integral or enum type.
 * Result V is the reflected constant of the fixed map generated, which can be
 * extracted via [: V :] or reflect_cpp26::extract<V>().
 *
 * Input kv_pairs should be a kv-pair list whose key is either integral type
 * or enum type (scoped or unscoped), and whose value can be converted to
 * structured type during compile-time
 * (see reflect_cpp26/type_operations/to_structured.hpp for details).
 *
 * The generated fixed map supports the following operations:
 *   size() -> size_t
 *   get(key) -> std::pair{value, found}
 *   operator[](key) -> value
 * If entry is not found for input key, returned value is value-initialized.
 * For most cases, value is returned as a const-reference.
 */
template <std::ranges::input_range KVPairRange>
  requires (impl::integral_key_kv_pair<std::ranges::range_value_t<KVPairRange>>)
consteval auto make_integral_key_map(
  KVPairRange kv_pairs, integral_key_map_options options = {})
  -> std::meta::info
{
  using KVPair = std::ranges::range_value_t<KVPairRange>;
  using Key = std::tuple_element_t<0, KVPair>;

  if constexpr (std::is_enum_v<Key>) {
    auto structured_kv_pairs = kv_pairs
      | std::views::transform([](const auto& kv_pair) {
          const auto& [k, v] = kv_pair;
          return std::pair{std::to_underlying(k), to_structured(v)};
        })
      | std::ranges::to<std::vector>();
    auto nested_res =
      impl::make_integral_key_map_impl(structured_kv_pairs, options);
    return extract<std::meta::info>(substitute(
      ^^impl::integral_key_map_enum_wrapper_factory_v, {^^Key, nested_res}));
  } else {
    auto structured_kv_pairs = kv_pairs
      | std::views::transform([](const auto& kv_pair) {
          return to_structured(kv_pair);
        })
      | std::ranges::to<std::vector>();
    return impl::make_integral_key_map_impl(structured_kv_pairs, options);
  }
}
} // namespace reflect_cpp26

/**
 * Generates a fixed map in compile-time whose key is of integral or enum type
 * and then extracts it immediately.
 * Details see above.
 */
#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ...)           \
  [: reflect_cpp26::make_integral_key_map(kv_pairs, ##__VA_ARGS__) :]

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP
