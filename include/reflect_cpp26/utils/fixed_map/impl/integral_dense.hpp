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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_DENSE_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_DENSE_HPP

#include <reflect_cpp26/utils/fixed_map/impl/common.hpp>

namespace reflect_cpp26::impl {
template <class ValueType>
struct value_wrapper_with_flag {
  ValueType underlying;
  bool is_valid;
};

template <class ValueType>
using aligned_value_wrapper_with_flag =
  alignment_adjusted_wrapper<value_wrapper_with_flag<ValueType>>;

template <bool UsesFlag, class KVPair>
struct dense_integral_key_map_common {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = const value_type&;

  using wrapper_type = std::conditional_t<
    UsesFlag,
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
    if constexpr (UsesFlag) {
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

// -------- Builder --------

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
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_DENSE_HPP
