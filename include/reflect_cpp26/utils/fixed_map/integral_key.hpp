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

#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/type_traits/type_initialization.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/common.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/utility.hpp>

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
template <class KVPair>
struct empty_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;

  static constexpr size_t size() {
    return 0;
  }

  static constexpr auto get(non_bool_integral auto)
    -> std::pair<const value_type&, bool>
  {
    return {map_null_value_v<value_type>, false};
  }

  static constexpr auto operator[](non_bool_integral auto)
    -> const value_type&
  {
    return map_null_value_v<value_type>;
  }
};

#define REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE                 \
  constexpr auto get(non_bool_integral auto key) const                  \
    -> std::pair<result_type, bool>                                     \
  {                                                                     \
    if (!in_range<key_type>(key)) {                                     \
      return {map_null_value_v<value_type>, false};                     \
    }                                                                   \
    return get(static_cast<key_type>(key));                             \
  }                                                                     \
                                                                        \
  constexpr result_type operator[](non_bool_integral auto key) const {  \
    return get(key).first;                                              \
  }

template <class ValueType>
struct value_wrapper_with_flag {
  ValueType underlying;
  bool is_valid;
};

template <class ValueType>
using aligned_value_wrapper_with_flag =
  alignment_adjusted_wrapper<value_wrapper_with_flag<ValueType>>;

template <bool UsesWrapper, class KVPair>
struct dense_integral_key_map_common {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = const value_type&;

  using wrapper_type = std::conditional_t<
    UsesWrapper,
    aligned_value_wrapper_with_flag<value_type>,
    alignment_adjusted_wrapper<value_type>>;

  meta_span<wrapper_type> _values;
  size_t _actual_size;
  key_type _min_key;
  key_type _max_key;

  constexpr size_t size() const {
    return _actual_size;
  }

  constexpr key_type min_key() const {
    return _min_key;
  }

  constexpr key_type max_key() const {
    return _max_key;
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool>
  {
    if (key < _min_key || key > _max_key) {
      return {map_null_value_v<value_type>, false};
    }
    auto index = static_cast<key_type>(key) - _min_key;
    const auto& p = _values[index].underlying;
    if constexpr (UsesWrapper) {
      return {p.underlying, p.is_valid};
    } else {
      return {p, !is_null(p)};
    }
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE
};

template <class KVPair>
using dense_integral_key_map =
  dense_integral_key_map_common<true, KVPair>;

template <class KVPair>
using compact_dense_integral_key_map =
  dense_integral_key_map_common<false, KVPair>;

template <class KVPair>
struct sparse_linear_search_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;

  meta_span<KVPair> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr key_type min_key() const {
    return get_first(_entries.front());
  }

  constexpr key_type max_key() const {
    return get_first(_entries.back());
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool>
  {
    for (const auto& entry: _entries) {
      if (get_first(entry) == key) {
        return {get_second(entry), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE
};

template <class KVPair>
struct sparse_binary_search_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;

  meta_span<alignment_adjusted_wrapper<KVPair>> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr key_type min_key() const {
    return get_first(_entries.front().underlying);
  }

  constexpr key_type max_key() const {
    return get_first(_entries.back().underlying);
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool>
  {
    for (auto head = _entries.begin(), tail = _entries.end(); head < tail; ) {
      auto mid = head + (tail - head) / 2;
      const auto& mid_entry = mid->underlying;
      if (get_first(mid_entry) == key) {
        return {get_second(mid_entry), true};
      }
      (get_first(mid_entry) < key) ? (head = mid + 1) : (tail = mid);
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE
};

template <bool UsesWrapper, class KVPair,
          template <class> class LeftSparseMapType,
          template <class> class RightSparseMapType>
struct general_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;

  dense_integral_key_map_common<UsesWrapper, KVPair> _dense_part;
  LeftSparseMapType<KVPair> _left_sparse_part;
  RightSparseMapType<KVPair> _right_sparse_part;

  constexpr size_t size() const
  {
    return _dense_part.size()
      + _left_sparse_part.size() + _right_sparse_part.size();
  }

  constexpr key_type min_key() const
  {
    if constexpr (^^LeftSparseMapType == ^^empty_integral_key_map) {
      return _dense_part.min_key();
    } else {
      return _left_sparse_part.min_key();
    }
  }

  constexpr key_type max_key() const
  {
    if constexpr (^^RightSparseMapType == ^^empty_integral_key_map) {
      return _dense_part.max_key();
    } else {
      return _right_sparse_part.max_key();
    }
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool>
  {
    if (key < _dense_part._min_key) {
      return _left_sparse_part.get(key);
    }
    if (key > _dense_part._max_key) {
      return _right_sparse_part.get(key);
    }
    auto [value, found] = _dense_part.get(key);
    return {value, found};
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE
};

#undef REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE

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

struct integral_key_map_meta_result {
  bool is_general;
  union {
    std::meta::info as_non_general;
    struct {
      std::meta::info dense_part;
      std::meta::info left_sparse_part;
      std::meta::info right_sparse_part;
    } as_general;
  };
};

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

template <class KVPairIter>
consteval auto make_compact_dense_integral_key_map_impl(
  KVPairIter sorted_first, KVPairIter sorted_last) -> std::meta::info
{
  using KVPair = std::iter_value_t<KVPairIter>;
  using Value = std::tuple_element_t<1, KVPair>;
  using ValueWrapper = alignment_adjusted_wrapper<Value>;

  auto min_key = get_first(*sorted_first);
  auto max_key = get_first(*std::prev(sorted_last));
  // max_key - min_key performs integer promotion for char and short types.
  auto values = std::vector<ValueWrapper>(max_key - min_key + 1);
  for (auto it = sorted_first; it < sorted_last; ++it) {
    values[get_first(*it) - min_key].underlying = get_second(*it);
  }
  auto res = compact_dense_integral_key_map<KVPair>{
    ._values = reflect_cpp26::define_static_array(values),
    ._actual_size = static_cast<size_t>(sorted_last - sorted_first),
    ._min_key = min_key,
    ._max_key = max_key,
  };
  return std::meta::reflect_constant(res);
}

template <class KVPairIter>
consteval auto make_dense_integral_key_map_impl(
  KVPairIter sorted_first, KVPairIter sorted_last) -> std::meta::info
{
  using KVPair = std::iter_value_t<KVPairIter>;
  using Value = std::tuple_element_t<1, KVPair>;
  using ValueWrapper = aligned_value_wrapper_with_flag<Value>;

  auto min_key = get_first(*sorted_first);
  auto max_key = get_first(*std::prev(sorted_last));
  // max_key - min_key performs integer promotion for char and short types.
  auto values = std::vector<ValueWrapper>(max_key - min_key + 1);
  for (auto it = sorted_first; it < sorted_last; ++it) {
    auto& cur = values[get_first(*it) - min_key].underlying;
    cur.underlying = get_second(*it);
    cur.is_valid = true;
  }
  auto res = dense_integral_key_map<KVPair>{
    ._values = reflect_cpp26::define_static_array(values),
    ._actual_size = static_cast<size_t>(sorted_last - sorted_first),
    ._min_key = min_key,
    ._max_key = max_key,
  };
  return std::meta::reflect_constant(res);
}

template <class KVPairIter>
consteval auto make_dense_integral_key_map(
  KVPairIter sorted_first, KVPairIter sorted_last,
  bool default_value_is_always_invalid) -> std::meta::info
{
  return default_value_is_always_invalid
    ? make_compact_dense_integral_key_map_impl(sorted_first, sorted_last)
    : make_dense_integral_key_map_impl(sorted_first, sorted_last);
}

template <class KVPairIter>
consteval auto make_sparse_integral_key_map(
  KVPairIter sorted_first, KVPairIter sorted_last,
  size_t binary_search_threshold) -> std::meta::info
{
  using KVPair = std::iter_value_t<KVPairIter>;
  if (sorted_last - sorted_first <= 0) {
    return std::meta::reflect_constant(empty_integral_key_map<KVPair>{});
  }
  if (sorted_last - sorted_first >= binary_search_threshold) {
    auto entries = reflect_cpp26::define_static_array(
      std::ranges::subrange{sorted_first, sorted_last}
        | std::views::transform(alignment_adjusted_wrapper<KVPair>::make));
    auto res = sparse_binary_search_integral_key_map{._entries = entries};
    return std::meta::reflect_constant(res);
  }
  auto entries = reflect_cpp26::define_static_array(
    std::ranges::subrange{sorted_first, sorted_last});
  auto res = sparse_linear_search_integral_key_map{._entries = entries};
  return std::meta::reflect_constant(res);
}

template <integral_key_kv_pair KVPair>
consteval auto make_integral_key_map_impl(
  std::vector<KVPair> kv_pairs, integral_key_map_options options)
  -> integral_key_map_meta_result
{
  // (1) Empty
  if (kv_pairs.empty()) {
    auto res = empty_integral_key_map<KVPair>{};
    return {.as_non_general = std::meta::reflect_constant(res)};
  }
  auto get_key_fn = [](const KVPair& kv_pair) {
    return get_first(kv_pair);
  };
  if (!options.already_sorted) {
    std::ranges::sort(kv_pairs, {}, get_key_fn);
  }
  if (!options.already_unique) {
    auto dup_pos = std::ranges::adjacent_find(kv_pairs, {}, get_key_fn);
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
    return {.as_non_general = res};
  }
  // (3) Sparse
  if (dense_end - dense_begin < options.dense_part_threshold) {
    auto res = make_sparse_integral_key_map(
      kv_pairs.cbegin(), kv_pairs.cend(), options.binary_search_threshold);
    return {.as_non_general = res};
  }
  // (4) Generic
  auto dense = make_dense_integral_key_map(
    dense_begin, dense_end, options.default_value_is_always_invalid);
  auto left_sparse = make_sparse_integral_key_map(
    kv_pairs.cbegin(), dense_begin, options.binary_search_threshold);
  auto right_sparse = make_sparse_integral_key_map(
    dense_end, kv_pairs.cend(), options.binary_search_threshold);
  return {
    .is_general = true,
    .as_general = {
      .dense_part = dense,
      .left_sparse_part = left_sparse,
      .right_sparse_part = right_sparse,
    },
  };
}

template <std::ranges::input_range KVPairRange>
consteval auto make_integral_key_map(
  KVPairRange&& kv_pairs, integral_key_map_options options = {})
  -> integral_key_map_meta_result
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

template <integral_key_map_meta_result V>
constexpr auto extract_integral_key_map_v = []() {
  if constexpr (V.is_general) {
    return general_integral_key_map{
      ._dense_part = extract<V.as_general.dense_part>(),
      ._left_sparse_part = extract<V.as_general.left_sparse_part>(),
      ._right_sparse_part = extract<V.as_general.right_sparse_part>(),
    };
  } else {
    return extract<V.as_non_general>();
  }
}();
} // namespace impl
} // namespace reflect_cpp26

#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ...)            \
  [&]() consteval {                                                    \
    constexpr auto meta_res =                                          \
      reflect_cpp26::impl::make_integral_key_map(                      \
        kv_pairs, ##__VA_ARGS__);                                      \
    return reflect_cpp26::impl::extract_integral_key_map_v<meta_res>;  \
  }()

#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP_WRAPPER(E, kv_pairs, ...) \
  reflect_cpp26::impl::make_integral_key_map_wrapper<E>(               \
    REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ##__VA_ARGS__))

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_INTEGRAL_KEY_HPP
