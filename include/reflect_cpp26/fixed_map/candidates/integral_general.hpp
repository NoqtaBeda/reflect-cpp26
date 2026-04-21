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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_GENERAL_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_GENERAL_HPP

#include <reflect_cpp26/fixed_map/candidates/integral_dense.hpp>
#include <reflect_cpp26/fixed_map/candidates/integral_sparse.hpp>

namespace reflect_cpp26::impl::map {
template <class Dense, class LeftSparse, class RightSparse>
struct general_with_ikey {
  using key_type = typename Dense::key_type;
  using value_type = typename Dense::value_type;

public:
  constexpr auto size() const -> size_t {
    return dense_part.size() + left_sparse_part.size() + right_sparse_part.size();
  }

  constexpr auto find(key_type key) const -> const value_type* {
    if (key < dense_part.min_key) {
      return left_sparse_part.find(key);
    }
    if (key > dense_part.max_key) {
      return right_sparse_part.find(key);
    }
    return dense_part.find(key);
  }

  constexpr auto find(non_bool_integral auto key) const -> const value_type* {
    if (!in_range<key_type>(key)) {
      return nullptr;
    }
    return find(static_cast<key_type>(key));
  }

  constexpr auto operator[](non_bool_integral auto key) const -> const value_type& {
    auto* p = find(key);
    return p ? *p : default_v<value_type>;
  }

  Dense dense_part;
  LeftSparse left_sparse_part;
  RightSparse right_sparse_part;
};

// -------- Builder --------

struct general_with_ikey_options {
  bool adjusts_alignment;
  size_t binary_search_threshold;
};

template <auto Dense, auto LeftSparse, auto RightSparse>
constexpr auto general_with_ikey_v = general_with_ikey{Dense, LeftSparse, RightSparse};

template <class K, class V>
consteval auto make_general_with_ikey(std::span<const meta_tuple<K, V>> left_sparse_entries,
                                      std::span<const meta_tuple<K, V>> right_sparse_entries,
                                      general_with_ikey_options options) -> std::meta::info {
  auto sparse_options = sparse_with_ikey_options{
      .adjusts_alignment = options.adjusts_alignment,
      .binary_search_threshold = options.binary_search_threshold,
  };
  auto left_sparse = make_sparse_with_ikey(left_sparse_entries, sparse_options);
  auto right_sparse = make_sparse_with_ikey(right_sparse_entries, sparse_options);

  auto dense_options = dense_with_ikey_options{
      .adjusts_alignment = options.adjusts_alignment,
  };
  auto dense_span_entries = std::span{left_sparse_entries.end(), right_sparse_entries.begin()};
  auto dense = make_dense_with_ikey(dense_span_entries, dense_options);

  auto params_il = {dense, left_sparse, right_sparse};
  return std::meta::substitute(^^general_with_ikey_v, params_il);
}
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_GENERAL_HPP
