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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_GENERAL_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_GENERAL_HPP

#include <reflect_cpp26/utils/fixed_map/impl/integral_dense.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/integral_empty.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/integral_sparse.hpp>

namespace reflect_cpp26::impl {
template <bool UsesFlag, class KVPair,
          template <class> class LeftSparseMapType,
          template <class> class RightSparseMapType>
struct general_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;

  dense_integral_key_map_common<UsesFlag, KVPair> _dense_part;
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

// -------- Builder --------

struct general_integral_key_map_options {
  bool default_value_is_always_invalid;
  size_t binary_search_threshold;
};

struct general_integral_key_map_result {
  std::meta::info dense_part;
  std::meta::info left_sparse_part;
  std::meta::info right_sparse_part;
};

template <class KVPairIter>
consteval auto make_general_integral_key_map(
  KVPairIter first, KVPairIter dense_first, KVPairIter dense_last,
  KVPairIter last, general_integral_key_map_options options)
  -> general_integral_key_map_result
{
  auto dense = make_dense_integral_key_map(
    dense_first, dense_last, options.default_value_is_always_invalid);
  auto left_sparse = make_sparse_integral_key_map(
    first, dense_first, options.binary_search_threshold);
  auto right_sparse = make_sparse_integral_key_map(
    dense_last, last, options.binary_search_threshold);

  return {
    .dense_part = dense,
    .left_sparse_part = left_sparse,
    .right_sparse_part = right_sparse,
  };
}

REFLECT_CPP26_EXTRACT_FIXED_MAP(general_integral_key_map_result) =
  general_integral_key_map{
    ._dense_part = extract<R.dense_part>(),
    ._left_sparse_part = extract<R.left_sparse_part>(),
    ._right_sparse_part = extract<R.right_sparse_part>(),
  };
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_GENERAL_HPP
