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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_SEARCH_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_SEARCH_HPP

#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>

namespace reflect_cpp26::impl {
// Precondition: No hash collision.
template <bool AlignmentAdjusted, class Policy, class KVPair>
struct string_key_map_by_hash_binary_search {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
      -> std::pair<result_type, bool> {
    auto key_hash = Policy::hash(key);
    for (auto head = _entries.head, tail = _entries.tail; head < tail;) {
      const auto* mid = head + (tail - head) / 2;
      const auto& [mid_hash, mid_kv_pair] = unwrap(*mid);
      if (mid_hash == key_hash) {
        if (!Policy::equals(get_first(mid_kv_pair), key)) {
          break;
        }
        return {get_second(mid_kv_pair), true};
      }
      (mid_hash < key_hash) ? (head = mid + 1) : (tail = mid);
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE

  using span_element_type = std::conditional_t<AlignmentAdjusted,
                                               alignment_adjusted_string_hash_wrapper<KVPair>,
                                               string_hash_wrapper<KVPair>>;

  meta_span<span_element_type> _entries;
};

// Precondition: No hash collision.
template <class Policy, class KVPair>
struct string_key_map_by_hash_linear_search {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
      -> std::pair<result_type, bool> {
    auto key_hash = Policy::hash(key);
    for (const auto& [hash, kv_pair] : _entries) {
      if (hash != key_hash) continue;
      if (!Policy::equals(get_first(kv_pair), key)) break;
      return {get_second(kv_pair), true};
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE

  // Alignment is not needed for linear search.
  using span_element_type = string_hash_wrapper<KVPair>;

  meta_span<span_element_type> _entries;
};

// -------- Factory --------

template <bool UsesBinarySearch, bool AlignmentAdjusted, class Policy, class KVPair>
constexpr auto string_key_map_by_hash_search_factory(const std::vector<KVPair>& kv_pairs,
                                                     const std::vector<uint64_t>& hash_values)
    -> std::meta::info {
  auto elems = std::vector<string_hash_wrapper<KVPair>>();
  auto n = kv_pairs.size();
  elems.reserve(n);
  for (auto i = 0zU; i < n; i++) {
    elems.push_back({
        .hash = hash_values[i],
        .underlying = kv_pairs[i],
    });
  }
  std::ranges::sort(elems, {}, &string_hash_wrapper<KVPair>::hash);

  if constexpr (!UsesBinarySearch || !AlignmentAdjusted) {
    using dest_type =
        std::conditional_t<UsesBinarySearch,
                           string_key_map_by_hash_binary_search<false, Policy, KVPair>,
                           string_key_map_by_hash_linear_search<Policy, KVPair>>;

    auto res = dest_type{._entries = reflect_cpp26::define_static_array(elems)};
    return std::meta::reflect_constant(res);
  } else {
    using dest_type = string_key_map_by_hash_binary_search<true, Policy, KVPair>;
    using span_element_type = typename dest_type::span_element_type;

    auto entries = span_element_type::make_static_array(elems);
    return std::meta::reflect_constant(dest_type{._entries = entries});
  }
}

// -------- Builders --------

struct string_key_map_by_hash_search_options {
  bool case_insensitive;
  bool adjusts_alignment;
  size_t binary_search_threshold;
};

template <class KVPair>
consteval auto make_string_key_map_by_hash_search(const std::vector<KVPair>& kv_pairs,
                                                  const std::vector<uint64_t>& hash_values,
                                                  string_key_map_by_hash_search_options options)
    -> std::meta::info {
  using factory_fn_type =
      std::meta::info (*)(const std::vector<KVPair>&, const std::vector<uint64_t>&);

  auto uses_binary_search = kv_pairs.size() >= options.binary_search_threshold;
  auto U = std::meta::reflect_constant(uses_binary_search);
  auto A = std::meta::reflect_constant(options.adjusts_alignment);
  auto policy = string_key_policy_type(options.case_insensitive);

  auto params_il = {U, A, policy, ^^KVPair};
  auto factory_fn =
      extract<factory_fn_type>(substitute(^^string_key_map_by_hash_search_factory, params_il));
  return factory_fn(kv_pairs, hash_values);
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_HASH_SEARCH_HPP
