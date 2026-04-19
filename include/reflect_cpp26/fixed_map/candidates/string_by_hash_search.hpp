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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_BY_HASH_SEARCH_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_BY_HASH_SEARCH_HPP

#include <reflect_cpp26/fixed_map/candidates/string_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>

namespace reflect_cpp26::impl::map {
// Precondition: No hash collision
template <class CharT, class V, template <class> class Policy>
struct linear_hash_search_with_skey {
  using key_type = meta_basic_string_view<CharT>;
  using value_type = V;

private:
  using element_type = meta_tuple<size_t, meta_basic_string_view<CharT>, V>;
  using result_type = std::pair<const value_type&, bool>;

public:
  constexpr auto size() const -> size_t {
    return entries.size();
  }

  constexpr auto get(std::basic_string_view<CharT> key) const -> result_type {
    auto len = key.length();
    if (len < min_length || len > max_length) {
      return {default_v<value_type>, false};
    }
    auto hash = Policy<CharT>::hash(key);
    for (const auto& cur : entries) {
      if (hash != cur.elements.first) continue;
      if (Policy<CharT>::equals(cur.elements.second, key)) {
        return {cur.elements.third, true};
      }
      break;
    }
    return {default_v<value_type>, false};
  }

  constexpr auto operator[](std::basic_string_view<CharT> key) const -> const value_type& {
    return get(key).first;
  }

  meta_span<element_type> entries;
  size_t min_length;
  size_t max_length;
};

template <bool A, bool C, class CharT, class V, template <class> class Policy>
struct binary_hash_search_with_skey {
  using key_type = meta_basic_string_view<CharT>;
  using value_type = V;

private:
  using raw_element_type = meta_tuple<size_t, meta_basic_string_view<CharT>, V>;
  using element_type = std::conditional_t<A, aligned<raw_element_type>, raw_element_type>;
  using result_type = std::pair<const value_type&, bool>;

public:
  constexpr auto size() const -> size_t {
    return entries.size();
  }

  constexpr auto get(std::basic_string_view<CharT> key) const -> result_type {
    auto len = key.length();
    if (len < min_length || len > max_length) {
      return {default_v<value_type>, false};
    }
    auto hash = Policy<CharT>::hash(key);
    constexpr auto hash_proj = [](const auto& entry) { return unwrap(entry).elements.first; };
    if constexpr (C) {
      auto range = std::ranges::equal_range(entries, hash, {}, hash_proj);
      for (const auto& entry : range) {
        const auto& [_, k, v] = unwrap(entry);
        if (Policy<CharT>::equals(k, key)) return {v, true};
      }
      return {default_v<value_type>, false};
    } else {
      auto pos = std::ranges::lower_bound(entries, hash, {}, hash_proj);
      if (entries.end() == pos) {
        return {default_v<value_type>, false};
      }
      const auto& cur = unwrap(*pos).elements;
      if (cur.first == hash && Policy<CharT>::equals(cur.second, key)) {
        return {cur.third, true};
      }
      return {default_v<value_type>, false};
    }
  }

  constexpr auto operator[](std::basic_string_view<CharT> key) const -> const value_type& {
    return get(key).first;
  }

  meta_span<element_type> entries;
  size_t min_length;
  size_t max_length;
};

// -------- Builder --------

struct hash_search_with_skey_options {
  bool ascii_case_insensitive;
  bool adjusts_alignment;
  size_t binary_search_threshold;
};

template <class CharT, class V, template <class> class Policy>
consteval auto make_linear_hash_search_with_skey(
    std::span<const meta_tuple<meta_basic_string_view<CharT>, V>> kv_pairs,
    std::span<const size_t> hash_values) -> std::meta::info {
  // Makes obj
  auto to_length = [](const auto& entry) { return entry.elements.first.length(); };
  auto [min_length, max_length] = std::ranges::minmax(kv_pairs | std::views::transform(to_length));

  auto n = kv_pairs.size();
  auto entries = std::vector<meta_tuple<size_t, meta_basic_string_view<CharT>, V>>{};
  entries.reserve(n);
  for (auto i = 0zU; i < n; i++) {
    const auto& [k, v] = kv_pairs[i];
    entries.push_back(meta_tuple{hash_values[i], k, v});
  }

  auto obj = linear_hash_search_with_skey<CharT, V, Policy>{
      .entries = reflect_cpp26::define_static_array(entries),
      .min_length = min_length,
      .max_length = max_length,
  };
  return std::meta::reflect_constant(obj);
}

template <bool A, bool C, class CharT, class V, template <class> class Policy>
consteval auto make_binary_hash_search_with_skey(
    std::span<const meta_tuple<meta_basic_string_view<CharT>, V>> kv_pairs,
    std::span<const size_t> hash_values) -> std::meta::info {
  // Makes obj
  auto to_length = [](const auto& entry) { return entry.elements.first.length(); };
  auto [min_length, max_length] = std::ranges::minmax(kv_pairs | std::views::transform(to_length));

  auto n = kv_pairs.size();
  auto entries = std::vector<meta_tuple<size_t, meta_basic_string_view<CharT>, V>>{};
  entries.reserve(n);
  for (auto i = 0zU; i < n; i++) {
    const auto& [k, v] = kv_pairs[i];
    entries.push_back(meta_tuple{hash_values[i], k, v});
  }
  // Sorts by hash value
  std::ranges::sort(entries, {}, get_first);

  auto obj = binary_hash_search_with_skey<A, C, CharT, V, Policy>{
      .min_length = min_length,
      .max_length = max_length,
  };
  if constexpr (A) {
    obj.entries = reflect_cpp26::define_static_array(entries | to_aligned);
  } else {
    obj.entries = reflect_cpp26::define_static_array(entries);
  }
  return std::meta::reflect_constant(obj);
}

template <class CharT, class V>
consteval auto make_hash_search_with_skey(
    std::span<const meta_tuple<meta_basic_string_view<CharT>, V>> kv_pairs,
    std::span<const size_t> hash_values,
    bool has_hash_collision,
    const hash_search_with_skey_options& options) -> std::meta::info {
  // (1) Empty
  if (kv_pairs.empty()) {
    return make_empty_with_skey<CharT, V>();
  }

  using call_signature = std::meta::info(
      std::span<const meta_tuple<meta_basic_string_view<CharT>, V>>, std::span<const size_t>);
  // (2) Linear search
  if (!has_hash_collision && kv_pairs.size() < options.binary_search_threshold) {
    auto policy = get_skey_policy_template(options.ascii_case_insensitive);
    auto fn = extract<call_signature*>(^^make_linear_hash_search_with_skey, ^^CharT, ^^V, policy);
    return fn(kv_pairs, hash_values);
  }
  // (3) Binary search
  auto A = std::meta::reflect_constant(options.adjusts_alignment);
  auto C = std::meta::reflect_constant(has_hash_collision);
  auto policy = get_skey_policy_template(options.ascii_case_insensitive);
  auto fn =
      extract<call_signature*>(^^make_binary_hash_search_with_skey, A, C, ^^CharT, ^^V, policy);
  return fn(kv_pairs, hash_values);
}
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_BY_HASH_SEARCH_HPP
