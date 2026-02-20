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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_STRING_WITH_HASH_COLLISION_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_STRING_WITH_HASH_COLLISION_HPP

#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26::impl {
template <bool AlignmentAdjusted, class Policy, class KVPair>
struct string_key_map_with_hash_collision {
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
    auto pos = std::ranges::lower_bound(
        _entries, key_hash, {}, [](const auto& entry) { return unwrap(entry).hash; });
    for (; pos < _entries.end() && unwrap(*pos).hash == key_hash; ++pos) {
      const auto& cur = unwrap(*pos).underlying;
      if (Policy::equals(get_first(cur), key)) {
        return {get_second(cur), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE

  using span_element_type = std::conditional_t<AlignmentAdjusted,
                                               alignment_adjusted_string_hash_wrapper<KVPair>,
                                               string_hash_wrapper<KVPair>>;

  meta_span<span_element_type> _entries;
};

// -------- Factory --------

template <bool AlignmentAdjusted, class Policy, class KVPair>
constexpr auto string_key_map_with_hash_collision_factory(const std::vector<KVPair>& kv_pairs,
                                                          const std::vector<uint64_t>& hash_values)
    -> std::meta::info {
  using dest_type = string_key_map_with_hash_collision<AlignmentAdjusted, Policy, KVPair>;
  using span_element_type = typename dest_type::span_element_type;

  auto entries_vec = std::vector<string_hash_wrapper<KVPair>>{};
  entries_vec.reserve(kv_pairs.size());
  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    entries_vec.push_back({.hash = hash_values[i], .underlying = kv_pairs[i]});
  }
  std::ranges::sort(entries_vec, {}, [](const auto& v) { return v.hash; });

  if constexpr (AlignmentAdjusted) {
    auto entries = span_element_type::make_static_array(entries_vec);
    return std::meta::reflect_constant(dest_type{._entries = entries});
  } else {
    auto entries = reflect_cpp26::define_static_array(entries_vec);
    return std::meta::reflect_constant(dest_type{._entries = entries});
  }
}

// -------- Builder --------

struct string_key_map_with_hash_collision_options {
  bool case_insensitive;
  bool adjusts_alignment;
};

template <class KVPair>
consteval auto make_string_key_map_with_hash_collision(
    const std::vector<KVPair>& kv_pairs,
    const std::vector<uint64_t>& hash_values,
    string_key_map_with_hash_collision_options options) -> std::meta::info {
  using factory_fn_type =
      std::meta::info (*)(const std::vector<KVPair>&, const std::vector<uint64_t>&);

  auto adjusts_alignment = std::meta::reflect_constant(options.adjusts_alignment);
  auto policy_type = string_key_policy_type(options.case_insensitive);

  auto factory_fn = extract<factory_fn_type>(
      ^^string_key_map_with_hash_collision_factory, adjusts_alignment, policy_type, ^^KVPair);

  return factory_fn(kv_pairs, hash_values);
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_STRING_WITH_HASH_COLLISION_HPP
