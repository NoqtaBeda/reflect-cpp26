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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_FULLY_DENSE_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_FULLY_DENSE_HPP

#include <reflect_cpp26/fixed_map/impl/integral_common.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_empty.hpp>

namespace reflect_cpp26::impl {
template <bool AlignmentAdjusted, class KVPair>
struct fully_dense_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = const value_type&;

  constexpr size_t size() const {
    return _max_key - _min_key + 1;
  }

  constexpr key_type min_key() const {
    return _min_key;
  }

  constexpr key_type max_key() const {
    return _max_key;
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool> {
    if (key < _min_key || key > _max_key) {
      return {map_null_value_v<value_type>, false};
    }
    auto index = key - _min_key;
    return {unwrap(_entries[index]), true};
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE

  using span_element_type =
      std::conditional_t<AlignmentAdjusted, alignment_adjusted_wrapper<value_type>, value_type>;

  // Internal members (Note: keep them public for structural-ness).
  meta_span<span_element_type> _entries;
  key_type _min_key;
  key_type _max_key;
};

// -------- Factory --------

template <bool AlignmentAdjusted, class KVPair>
constexpr auto fully_dense_integral_key_map_factory(meta_span<KVPair> sorted_entries)
    -> std::meta::info {
  using dest_type = fully_dense_integral_key_map<AlignmentAdjusted, KVPair>;
  using value_type = typename dest_type::value_type;
  using span_element_type = typename dest_type::span_element_type;

  auto res = dest_type{};
  res._min_key = get_first(sorted_entries.front());
  res._max_key = get_first(sorted_entries.back());

  auto values = std::vector<value_type>{};
  values.reserve(sorted_entries.size());
  for (const auto& [k, v] : sorted_entries) {
    values.push_back(v);
  }
  if constexpr (AlignmentAdjusted) {
    res._entries = span_element_type::make_static_array(values);
  } else {
    res._entries = reflect_cpp26::define_static_array(values);
  }
  return std::meta::reflect_constant(res);
}

// -------- Builder --------

template <class KVPairIter>
consteval auto make_fully_dense_integral_key_map(KVPairIter sorted_first,
                                                 KVPairIter sorted_last,
                                                 bool adjusts_alignment) -> std::meta::info {
  using KVPair = std::iter_value_t<KVPairIter>;
  using factory_fn_type = std::meta::info (*)(meta_span<KVPair>);

  if (sorted_first == sorted_last) {
    return make_empty_integral_key_map<KVPair>();
  }

  auto AlignmentAdjusted = std::meta::reflect_constant(adjusts_alignment);
  auto params_il = {AlignmentAdjusted, ^^KVPair};
  auto factory_fn =
      extract<factory_fn_type>(substitute(^^fully_dense_integral_key_map_factory, params_il));

  auto subrange = std::ranges::subrange(sorted_first, sorted_last);
  auto entries = reflect_cpp26::define_static_array(subrange);
  return factory_fn(entries);
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_FULLY_DENSE_HPP
