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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_NAIVE_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_NAIVE_HPP

#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>

namespace reflect_cpp26::impl {
template <class Policy, class KVPair>
struct naive_string_key_map {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    for (const auto& cur: _entries) {
      if (Policy::equals(get_first(cur), key)) {
        return {get_second(cur), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE

  // Alignment is not needed for linear searching
  meta_span<KVPair> _entries;
};

// -------- Factory --------

template <class Policy, class KVPair>
constexpr auto naive_string_key_map_factory(meta_span<KVPair> entries)
  -> std::meta::info
{
  auto res = naive_string_key_map<Policy, KVPair>{._entries = entries};
  return std::meta::reflect_constant(res);
}

// -------- Builder --------

template <class KVPair>
consteval auto make_naive_string_key_map(
  const std::vector<KVPair>& kv_pairs, bool case_insensitive) -> std::meta::info
{
  using factory_fn_type = std::meta::info (*)(meta_span<KVPair>);
  auto policy = string_key_policy_type(case_insensitive);
  auto factory_fn = extract<factory_fn_type>(
    substitute(^^naive_string_key_map_factory, policy, ^^KVPair));

  auto entries = reflect_cpp26::define_static_array(kv_pairs);
  return factory_fn(entries);
}
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_NAIVE_HPP
