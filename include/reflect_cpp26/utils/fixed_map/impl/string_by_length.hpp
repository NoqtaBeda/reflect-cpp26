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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_LENGTH_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_LENGTH_HPP

#include <reflect_cpp26/utils/fixed_map/impl/string_policy.hpp>
#include <reflect_cpp26/utils/fixed_map/integral_key.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>

namespace reflect_cpp26::impl {
// Value type of wrapper is the whole underlying kv-pair since we need the
// wrapped key (which is a meta_string_view) for further checking.
template <class KVPair>
struct string_key_kv_pair_wrapper_by_length {
  using underlying_type = KVPair;
  using tuple_elements = type_tuple<size_t, KVPair>;

  KVPair underlying;

  static constexpr auto make(KVPair kv_pair) {
    return string_key_kv_pair_wrapper_by_length{std::move(kv_pair)};
  }

  template <size_t I, class Self>
  constexpr decltype(auto) get(this Self&& self)
  {
    if constexpr (I == 0) {
      return get_first(self.underlying).length();
    } else if constexpr (I == 1) {
      return std::forward_like<Self>(self.underlying);
    } else {
      static_assert(false, "Invalid index");
    }
  }
};

template <class Policy, class Underlying>
struct string_key_map_by_length {
  // => string_key_kv_pair_wrapper_by_length<KVPair>
  using kv_pair_wrapper_type = typename Underlying::kv_pair_type;
  using kv_pair_type = typename kv_pair_wrapper_type::underlying_type;

  using key_type = std::tuple_element_t<0, kv_pair_type>;
  using value_type = std::tuple_element_t<1, kv_pair_type>;
  using result_type = std::invoke_result_t<get_second_t, const kv_pair_type&>;
  using character_type = char_type_t<key_type>;

  Underlying _underlying;

  constexpr size_t size() const {
    return _underlying.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    auto [kv_pair, found] = _underlying.get(key.length());
    if (!found) {
      return {map_null_value_v<value_type>, false};
    }
    auto keys_are_equal =
      Policy::equals(get_first(kv_pair), key, Policy::equal_length);
    if (!keys_are_equal) {
      return {map_null_value_v<value_type>, false};
    }
    return {get_second(kv_pair), true};
  };

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// -------- Builder --------

template <class KVPair>
consteval bool key_lengths_are_all_distinct(const std::vector<KVPair>& kv_pairs)
{
  auto lengths = make_reserved_vector<size_t>(kv_pairs.size());
  for (const auto& [k, _]: kv_pairs) {
    lengths.push_back(k.length());
  }
  std::ranges::sort(lengths);
  auto dup_pos = std::ranges::adjacent_find(lengths);
  return dup_pos == lengths.end();
}

struct length_based_string_key_map_result {
  std::meta::info policy_type;
  std::meta::info underlying;
};

template <class KVPair>
consteval auto try_make_length_based_string_key_map(
  const std::vector<KVPair>& kv_pairs,
  integral_key_map_options options_for_underlying, bool case_insensitive)
  -> std::optional<length_based_string_key_map_result>
{
  if (!key_lengths_are_all_distinct(kv_pairs)) {
    return std::nullopt;
  }
  auto wrapped = kv_pairs | std::views::transform(
    string_key_kv_pair_wrapper_by_length<KVPair>::make);

  options_for_underlying.already_unique = true;
  options_for_underlying.default_value_is_always_invalid = true;
  auto underlying = make_integral_key_map(wrapped, options_for_underlying);

  auto policy_type = string_key_policy_type(case_insensitive);
  return length_based_string_key_map_result{
    .policy_type = policy_type,
    .underlying = underlying
  };
}

REFLECT_CPP26_EXTRACT_FIXED_MAP(length_based_string_key_map_result) =
  [] consteval {
    auto underlying = extract_integral_key_fixed_map_v<R.underlying>;
    using Policy = [: R.policy_type :];
    return string_key_map_by_length<Policy, decltype(underlying)>{underlying};
  }();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_LENGTH_HPP
