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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_LENGTH_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_LENGTH_HPP

#include <reflect_cpp26/fixed_map/impl/string_common.hpp>
#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>
#include <reflect_cpp26/fixed_map/integral_key.hpp>
#include <reflect_cpp26/utils/constant.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>

namespace reflect_cpp26::impl {
template <class Policy, class Underlying>
struct string_key_map_by_length {
  // Underlying: size_t -> KVPair
  using kv_pair_type = typename Underlying::value_type;

  using key_type = std::tuple_element_t<0, kv_pair_type>;
  using value_type = std::tuple_element_t<1, kv_pair_type>;
  using result_type = std::invoke_result_t<get_second_t, const kv_pair_type&>;
  using character_type = char_type_t<key_type>;

  constexpr size_t size() const {
    return _underlying.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
      -> std::pair<result_type, bool> {
    auto [kv_pair, found] = _underlying.get(key.length());
    if (!found) {
      return {map_null_value_v<value_type>, false};
    }
    auto keys_are_equal = Policy::equals(get_first(kv_pair), key, Policy::equal_length);
    if (!keys_are_equal) {
      return {map_null_value_v<value_type>, false};
    }
    return {get_second(kv_pair), true};
  };

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE

  Underlying _underlying;
};

// -------- Factory --------

template <class Policy, auto Underlying>
constexpr auto string_key_map_by_length_factory_v =
    std::meta::reflect_constant(string_key_map_by_length<Policy, decltype(Underlying)>{Underlying});

// -------- Builder --------

template <class KVPairRange>
consteval bool key_lengths_are_all_distinct(const KVPairRange& kv_pairs) {
  auto lengths = make_reserved_vector<size_t>(std::ranges::size(kv_pairs));
  for (const auto& [k, _] : kv_pairs) {
    lengths.push_back(k.length());
  }
  std::ranges::sort(lengths);
  auto dup_pos = std::ranges::adjacent_find(lengths);
  return dup_pos == lengths.end();
}

template <class KVPairRange>
consteval auto try_make_string_key_map_by_length(
    const KVPairRange& kv_pairs,
    integral_key_fixed_map_options options_for_underlying,
    bool case_insensitive) -> std::optional<std::meta::info> {
  using KVPair = std::ranges::range_value_t<KVPairRange>;
  if (!key_lengths_are_all_distinct(kv_pairs)) {
    return std::nullopt;
  }
  using KVPairWrapper = std::pair<size_t, KVPair>;
  auto wrapped = make_reserved_vector<KVPairWrapper>(kv_pairs.size());
  for (const auto& kv_pair : kv_pairs) {
    wrapped.emplace_back(get_first(kv_pair).length(), kv_pair);
  }
  options_for_underlying.already_unique = true;
  options_for_underlying.default_value_is_always_invalid = true;
  auto underlying = make_integral_key_fixed_map(wrapped, options_for_underlying);

  auto policy_type = string_key_policy_type(case_insensitive);
  return extract<std::meta::info>(^^string_key_map_by_length_factory_v, policy_type, underlying);
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_LENGTH_HPP
