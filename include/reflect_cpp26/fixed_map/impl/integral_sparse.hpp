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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_SPARSE_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_SPARSE_HPP

#include <reflect_cpp26/fixed_map/impl/integral_empty.hpp>

namespace reflect_cpp26::impl {
template <class KVPair>
struct linear_search_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr key_type min_key() const {
    return get_first(_entries.front());
  }

  constexpr key_type max_key() const {
    return get_first(_entries.back());
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool> {
    for (const auto& entry : _entries) {
      if (get_first(entry) == key) {
        return {get_second(entry), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE

  // Internal members (Note: keep them public for structural-ness).
  meta_span<KVPair> _entries;
};

template <bool AlignmentAdjusted, class KVPair>
struct binary_search_integral_key_map {
  using kv_pair_type = KVPair;
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr key_type min_key() const {
    return get_first(unwrap(_entries.front()));
  }

  constexpr key_type max_key() const {
    return get_first(unwrap(_entries.back()));
  }

  constexpr auto get(key_type key) const -> std::pair<result_type, bool> {
    for (auto head = _entries.begin(), tail = _entries.end(); head < tail;) {
      auto mid = head + (tail - head) / 2;
      const auto& mid_entry = unwrap(*mid);
      if (get_first(mid_entry) == key) {
        return {get_second(mid_entry), true};
      }
      (get_first(mid_entry) < key) ? (head = mid + 1) : (tail = mid);
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE

  using span_element_type =
      std::conditional_t<AlignmentAdjusted, alignment_adjusted_wrapper<KVPair>, KVPair>;

  // Internal members (Note: keep them public for structural-ness).
  meta_span<span_element_type> _entries;
};

// -------- Factory --------

template <class KVPair>
constexpr auto linear_search_integral_key_map_factory(meta_span<KVPair> sorted_entries)
    -> std::meta::info {
  auto res = linear_search_integral_key_map<KVPair>{
      ._entries = sorted_entries,
  };
  return std::meta::reflect_constant(res);
}

template <bool AlignmentAdjusted, class KVPair>
constexpr auto binary_search_integral_key_map_factory(meta_span<KVPair> sorted_entries)
    -> std::meta::info {
  using dest_type = binary_search_integral_key_map<AlignmentAdjusted, KVPair>;
  using span_element_type = typename dest_type::span_element_type;

  if constexpr (AlignmentAdjusted) {
    auto res = dest_type{
        ._entries = span_element_type::make_static_array(sorted_entries),
    };
    return std::meta::reflect_constant(res);
  } else {
    auto res = dest_type{._entries = sorted_entries};
    return std::meta::reflect_constant(res);
  }
}

// -------- Builder --------

struct sparse_integral_key_fixed_map_options {
  bool adjusts_alignment;
  size_t binary_search_threshold;
};

template <class KVPairIter>
consteval auto make_sparse_integral_key_map(KVPairIter sorted_first,
                                            KVPairIter sorted_last,
                                            sparse_integral_key_fixed_map_options options)
    -> std::meta::info {
  using KVPair = std::iter_value_t<KVPairIter>;
  // (1) Empty. Note that emptiness check here is necessary when make_dense_integral_key_map()
  //     is called by another fixed map maker function.
  if (sorted_first == sorted_last) {
    return std::meta::reflect_constant(empty_integral_key_map<KVPair>{});
  }

  // (2) Binary searching, or (3) linear searching
  auto factory_fn = [&sorted_first, &sorted_last, options]() {
    auto dist = std::distance(sorted_first, sorted_last);
    if (dist < options.binary_search_threshold) {
      return linear_search_integral_key_map_factory<KVPair>;
    }
    if (options.adjusts_alignment) {
      return binary_search_integral_key_map_factory<true, KVPair>;
    } else {
      return binary_search_integral_key_map_factory<false, KVPair>;
    }
  }();
  auto sorted_subrange = std::ranges::subrange{sorted_first, sorted_last};
  auto entries = reflect_cpp26::define_static_array(sorted_subrange);
  return factory_fn(entries);
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_SPARSE_HPP
