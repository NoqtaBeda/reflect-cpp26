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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_CHARACTER_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_CHARACTER_HPP

#include <reflect_cpp26/utils/fixed_map/impl/string_policy.hpp>
#include <reflect_cpp26/utils/fixed_map/integral_key.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>
#include <climits>

namespace reflect_cpp26::impl {
template <size_t J, class KVPair>
struct string_key_kv_pair_wrapper_by_character {
  using underlying_type = KVPair;
  using character_type = char_type_t<std::tuple_element_t<0, underlying_type>>;
  using tuple_elements = type_tuple<character_type, KVPair>;

  static constexpr auto differed_index = J;
  KVPair underlying;

  static constexpr auto make(KVPair kv_pair) {
    return string_key_kv_pair_wrapper_by_character{std::move(kv_pair)};
  }

  template <size_t I, class Self>
  constexpr decltype(auto) get(this Self&& self)
  {
    if constexpr (I == 0) {
      return get_first(self.underlying)[J];
    } else if constexpr (I == 1) {
      return std::forward_like<Self>(self.underlying);
    } else {
      static_assert(false, "Invalid index");
    }
  }
};

template <class Policy, class Underlying>
struct string_key_map_by_character {
  // => string_key_kv_pair_wrapper_by_character<I, KVPair>
  using kv_pair_wrapper_type = typename Underlying::kv_pair_type;
  using kv_pair_type = typename kv_pair_wrapper_type::underlying_type;

  using key_type = std::tuple_element_t<0, kv_pair_type>;
  using value_type = std::tuple_element_t<1, kv_pair_type>;
  using result_type = std::invoke_result_t<get_second_t, const kv_pair_type&>;
  using character_type = char_type_t<key_type>;

  static constexpr auto differed_index = kv_pair_wrapper_type::differed_index;
  Underlying _underlying;

  constexpr size_t size() const {
    return _underlying.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    if (key.length() <= differed_index) {
      return {map_null_value_v<value_type>, false};
    }

    auto c = Policy::convert_char(key[differed_index]);
    auto [kv_pair, found] = _underlying.get(c);
    if (!found || !Policy::equals(get_first(kv_pair), key)) {
      return {map_null_value_v<value_type>, false};
    }
    return {get_second(kv_pair), true};
  };

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// -------- Builder --------

template <class KVPair>
consteval bool characters_are_all_different(
  const std::vector<KVPair>& kv_pairs, size_t index)
{
  auto table = std::array<bool, (1u << CHAR_BIT)>{};
  for (const auto& cur: kv_pairs) {
    auto c = get_first(cur)[index];
    if (table[c - CHAR_MIN]) { return false; }
    table[c - CHAR_MIN] = true;
  }
  return true;
}

struct character_based_string_key_map_result {
  std::meta::info policy_type;
  std::meta::info underlying;
};

template <size_t I, class KVPair>
consteval auto make_character_based_string_key_map_impl(
  const std::vector<KVPair>& kv_pairs,
  integral_key_map_options options_for_underlying, bool case_insensitive)
  -> character_based_string_key_map_result
{
  auto wrapped = kv_pairs | std::views::transform(
    string_key_kv_pair_wrapper_by_character<I, KVPair>::make);

  options_for_underlying.already_unique = true;
  options_for_underlying.default_value_is_always_invalid = true;
  auto underlying = make_integral_key_map(wrapped, options_for_underlying);

  auto policy_type = string_key_policy_type(case_insensitive);
  return {.policy_type = policy_type, .underlying = underlying};
}

template <class KVPair>
consteval auto make_character_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, size_t index,
  const integral_key_map_options& options_for_underlying, bool case_insensitive)
  -> character_based_string_key_map_result
{
  using fn_signature = character_based_string_key_map_result (*)(
    const std::vector<KVPair>&, integral_key_map_options, bool);

  auto I = std::meta::reflect_constant(index);
  auto fn = extract<fn_signature>(
    ^^make_character_based_string_key_map_impl, I, ^^KVPair);

  return fn(kv_pairs, options_for_underlying, case_insensitive);
}

template <class KVPair>
consteval auto try_make_character_based_string_key_map(
  const std::vector<KVPair>& kv_pairs,
  const integral_key_map_options& options_for_underlying, bool case_insensitive)
  -> std::optional<character_based_string_key_map_result>
{
  auto to_length_fn = [](const KVPair& p) {
    return get_first(p).length();
  };
  auto min_length = std::ranges::min(
    kv_pairs | std::views::transform(to_length_fn));
  for (auto i = 0zU; i < min_length; i++) {
    if (characters_are_all_different(kv_pairs, i)) {
      return make_character_based_string_key_map(
        kv_pairs, i, options_for_underlying, case_insensitive);
    }
  }
  return std::nullopt;
}

REFLECT_CPP26_EXTRACT_FIXED_MAP(character_based_string_key_map_result) =
  [] consteval {
    auto underlying = extract_integral_key_fixed_map_v<R.underlying>;
    using Policy = [: R.policy_type :];
    using Underlying = decltype(underlying);
    return string_key_map_by_character<Policy, Underlying>{underlying};
  }();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_BY_CHARACTER_HPP
