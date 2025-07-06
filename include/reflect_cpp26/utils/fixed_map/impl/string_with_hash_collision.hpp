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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_WITH_HASH_COLLISION_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_WITH_HASH_COLLISION_HPP

#include <reflect_cpp26/utils/fixed_map/impl/string_policy.hpp>

namespace reflect_cpp26::impl {
template <class Policy, class KVPair>
struct string_key_map_with_hash_collision {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  meta_span<wrapper_type> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    auto key_hash = Policy::hash(key);
    auto pos = std::ranges::lower_bound(_entries, key_hash, {},
      [](const auto& entry) {
        return entry.underlying.hash;
      });
    for (; pos < _entries.end() && pos->underlying.hash == key_hash; ++pos) {
      const auto& cur = pos->underlying.underlying;
      if (Policy::equals(get_first(cur), key)) {
        return {get_second(cur), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// -------- Builder --------

struct string_key_map_with_hash_collision_result {
  std::meta::info type;
  std::meta::info entries;
};

template <class KVPair>
consteval auto make_string_key_map_with_hash_collision(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  bool case_insensitive) -> string_key_map_with_hash_collision_result
{
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  auto entries_vec = std::vector<wrapper_type>{};
  entries_vec.reserve(kv_pairs.size());
  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    entries_vec.push_back(
      {{.hash = hash_values[i], .underlying = kv_pairs[i]}});
  }
  std::ranges::sort(entries_vec, {}, [](const auto& v) {
    return v.underlying.hash;
  });

  auto policy_type = string_key_policy_type(case_insensitive);
  auto type = substitute(
    ^^string_key_map_with_hash_collision, policy_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(entries_vec);
  return {.type = type, .entries = std::meta::reflect_constant(entries)};
}

REFLECT_CPP26_EXTRACT_FIXED_MAP(string_key_map_with_hash_collision_result) =
  [] consteval {
    using Type = [: R.type :];
    return Type{._entries = extract<R.entries>()};
  }();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_WITH_HASH_COLLISION_HPP
