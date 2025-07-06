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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_SPARSE_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_SPARSE_HPP

#include <reflect_cpp26/utils/fixed_map/impl/integral_empty.hpp>

namespace reflect_cpp26::impl {
template <class KVPair>
struct sparse_linear_search_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;

  // Alignment adjustment is not needed for linear searching.
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
      if (get_first(entry) == key) { return {get_second(entry), true}; }
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

// -------- Builder --------

template <class KVPairIter>
consteval auto make_sparse_integral_key_map(
  KVPairIter sorted_first, KVPairIter sorted_last,
  size_t binary_search_threshold) -> std::meta::info
{
  using KVPair = std::iter_value_t<KVPairIter>;
  // (1) Empty
  if (sorted_last - sorted_first <= 0) {
    return std::meta::reflect_constant(empty_integral_key_map<KVPair>{});
  }
  // (2) Binary searching
  if (sorted_last - sorted_first >= binary_search_threshold) {
    auto entries = alignment_adjusted_wrapper<KVPair>::make_static_array(
      sorted_first, sorted_last);
    auto res = sparse_binary_search_integral_key_map{._entries = entries};
    return std::meta::reflect_constant(res);
  }
  // (3) Linear searching
  auto entries = reflect_cpp26::define_static_array(
    std::ranges::subrange{sorted_first, sorted_last});
  auto res = sparse_linear_search_integral_key_map{._entries = entries};
  return std::meta::reflect_constant(res);
}
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_INTEGRAL_SPARSE_HPP
