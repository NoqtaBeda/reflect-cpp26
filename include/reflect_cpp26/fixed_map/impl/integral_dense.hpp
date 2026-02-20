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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_DENSE_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_DENSE_HPP

#include <reflect_cpp26/fixed_map/impl/integral_common.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_fully_dense.hpp>
#include <reflect_cpp26/fixed_map/is_valid.hpp>  // Predefined predicates

namespace reflect_cpp26::impl {
consteval auto dense_integral_key_map_span_element_type(bool validity_stored,
                                                        bool alignment_adjusted,
                                                        std::meta::info value_type)
    -> std::meta::info {
  auto params_il = {value_type};
  return validity_stored
           ? (alignment_adjusted ? substitute(^^alignment_adjusted_validity_wrapper, params_il)
                                 : substitute(^^validity_wrapper, params_il))
           : (alignment_adjusted ? substitute(^^alignment_adjusted_wrapper, params_il)
                                 : value_type);
}

template <bool ValidityStored, bool AlignmentAdjusted, class KVPair>
struct dense_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = const value_type&;

  constexpr size_t size() const {
    return _actual_size;
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
    const auto& p = unwrap_once(_entries[index]);
    if constexpr (ValidityStored) {
      return {unwrap(p), p.is_valid};
    } else {
      const auto& v = unwrap(p);
      return {v, is_valid(v)};
    }
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE

  using span_element_type = [:dense_integral_key_map_span_element_type(
                                  ValidityStored, AlignmentAdjusted, ^^value_type):];

  // Internal members (Note: keep them public for structural-ness).
  meta_span<span_element_type> _entries;
  size_t _actual_size;
  key_type _min_key;
  key_type _max_key;
};

// -------- Factory --------

template <bool ValidityStored, bool AlignmentAdjusted, class KVPair>
constexpr auto dense_integral_key_map_factory(meta_span<KVPair> sorted_entries) -> std::meta::info {
  using dest_type = dense_integral_key_map<ValidityStored, AlignmentAdjusted, KVPair>;
  using value_type = typename dest_type::value_type;
  using span_element_type = typename dest_type::span_element_type;

  auto res = dest_type{};
  res._actual_size = sorted_entries.size();
  res._min_key = get_first(sorted_entries.front());
  res._max_key = get_first(sorted_entries.back());

  auto n_grids = res._max_key - res._min_key + 1;
  if constexpr (ValidityStored) {
    auto validity_wrappers = std::vector<validity_wrapper<value_type>>(n_grids);
    for (const auto& kv_pair : sorted_entries) {
      auto& cur = validity_wrappers[get_first(kv_pair) - res._min_key];
      cur.is_valid = true;
      cur.underlying = get_second(kv_pair);
    }
    if constexpr (AlignmentAdjusted) {
      res._entries = span_element_type::make_static_array(validity_wrappers);
    } else {
      res._entries = reflect_cpp26::define_static_array(validity_wrappers);
    }
  } else {
    auto grid_entries = std::vector<value_type>(n_grids);
    for (const auto& kv_pair : sorted_entries) {
      grid_entries[get_first(kv_pair) - res._min_key] = get_second(kv_pair);
    }
    if constexpr (AlignmentAdjusted) {
      res._entries = span_element_type::make_static_array(grid_entries);
    } else {
      res._entries = reflect_cpp26::define_static_array(grid_entries);
    }
  }
  return std::meta::reflect_constant(res);
}

// -------- Builder --------

struct dense_integral_key_fixed_map_options {
  bool adjusts_alignment;
  bool default_value_is_always_invalid;
};

template <class KVPairIter>
consteval auto make_dense_integral_key_map(KVPairIter sorted_first,
                                           KVPairIter sorted_last,
                                           dense_integral_key_fixed_map_options options)
    -> std::meta::info {
  using KVPair = std::iter_value_t<KVPairIter>;
  using factory_fn_type = std::meta::info (*)(meta_span<KVPair>);

  // Note: Emptiness check here is necessary when make_dense_integral_key_map() is called by
  //       another fixed map maker function.
  if (sorted_first == sorted_last) {
    return make_empty_integral_key_map<KVPair>();
  }
  auto first_key = get_first(*sorted_first);
  auto last_key = get_first(*std::prev(sorted_last));
  if (last_key - first_key == sorted_last - sorted_first - 1) {
    return make_fully_dense_integral_key_map(sorted_first, sorted_last, options.adjusts_alignment);
  }

  auto ValidityStored = std::meta::reflect_constant(!options.default_value_is_always_invalid);
  auto AlignmentAdjusted = std::meta::reflect_constant(options.adjusts_alignment);
  auto params_il = {ValidityStored, AlignmentAdjusted, ^^KVPair};
  auto factory_fn =
      extract<factory_fn_type>(substitute(^^dense_integral_key_map_factory, params_il));

  auto subrange = std::ranges::subrange(sorted_first, sorted_last);
  auto entries = reflect_cpp26::define_static_array(subrange);
  return factory_fn(entries);
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_DENSE_HPP
