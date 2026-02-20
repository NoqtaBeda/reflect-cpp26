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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_GENERAL_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_GENERAL_HPP

#include <reflect_cpp26/fixed_map/impl/integral_dense.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/integral_sparse.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>

namespace reflect_cpp26::impl {
template <class DenseMap, class LeftSparseMap, class RightSparseMap>
struct general_integral_key_map {
  using kv_pair_type = typename DenseMap::kv_pair_type;
  using key_type = std::tuple_element_t<0, kv_pair_type>;
  using value_type = std::tuple_element_t<1, kv_pair_type>;
  using result_type = std::invoke_result_t<get_second_t, const kv_pair_type&>;

  constexpr size_t size() const {
    return _dense_part.size() + _left_sparse_part.size() + _right_sparse_part.size();
  }

  constexpr key_type min_key() const {
    constexpr auto left_is_empty = template_instance_of<LeftSparseMap, empty_integral_key_map>;
    if constexpr (left_is_empty) {
      return _dense_part.min_key();
    } else {
      return _left_sparse_part.min_key();
    }
  }

  constexpr key_type max_key() const {
    constexpr auto right_is_empty = template_instance_of<RightSparseMap, empty_integral_key_map>;
    if constexpr (^^RightSparseMap == ^^empty_integral_key_map) {
      return _dense_part.max_key();
    } else {
      return _right_sparse_part.max_key();
    }
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool> {
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

  // Constructor
  constexpr general_integral_key_map(DenseMap dense,
                                     LeftSparseMap left_sparse,
                                     RightSparseMap right_sparse)
      : _dense_part(dense), _left_sparse_part(left_sparse), _right_sparse_part(right_sparse) {}

  // Internal members (Note: keep them public for structural-ness).
  DenseMap _dense_part;
  LeftSparseMap _left_sparse_part;
  RightSparseMap _right_sparse_part;
};

// -------- Factory --------

template <auto Dense, auto LeftSparse, auto RightSparse>
constexpr auto general_integral_key_map_factory_v =
    std::meta::reflect_constant(general_integral_key_map{Dense, LeftSparse, RightSparse});

// -------- Builder --------

struct general_integral_key_fixed_map_options {
  bool adjusts_alignment;
  bool default_value_is_always_invalid;
  size_t binary_search_threshold;
};

// Requires: [first, last) is already sorted.
template <class KVPairIter>
consteval auto make_general_integral_key_map(KVPairIter first,
                                             KVPairIter dense_first,
                                             KVPairIter dense_last,
                                             KVPairIter last,
                                             general_integral_key_fixed_map_options options)
    -> std::meta::info {
  auto dense_options = dense_integral_key_fixed_map_options{
      .adjusts_alignment = options.adjusts_alignment,
      .default_value_is_always_invalid = options.default_value_is_always_invalid,
  };
  auto sparse_options = sparse_integral_key_fixed_map_options{
      .adjusts_alignment = options.adjusts_alignment,
      .binary_search_threshold = options.binary_search_threshold,
  };
  auto dense = make_dense_integral_key_map(dense_first, dense_last, dense_options);
  auto left_sparse = make_sparse_integral_key_map(first, dense_first, sparse_options);
  auto right_sparse = make_sparse_integral_key_map(dense_last, last, sparse_options);

  auto params_il = {dense, left_sparse, right_sparse};
  return extract<std::meta::info>(substitute(^^general_integral_key_map_factory_v, params_il));
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_GENERAL_HPP
