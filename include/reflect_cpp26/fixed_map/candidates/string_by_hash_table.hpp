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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_BY_HASH_TABLE_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_BY_HASH_TABLE_HPP

#include <algorithm>
#include <optional>
#include <ranges>
#include <reflect_cpp26/fixed_map/candidates/string_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>

namespace reflect_cpp26::impl::map {
// Precondition: No input key is hashed to 0 (corollary: min_length >= 1)
template <bool A, size_t P, class CharT, class V, template <class> class Policy>
struct hash_table_with_skey {
  using key_type = meta_basic_string_view<CharT>;
  using value_type = V;

private:
  using raw_element_type = meta_tuple<size_t, meta_basic_string_view<CharT>, V>;
  using element_type = std::conditional_t<A, aligned<raw_element_type>, raw_element_type>;

public:
  constexpr auto size() const -> size_t {
    return actual_size;
  }

  constexpr auto find(std::basic_string_view<CharT> key) const -> const value_type* {
    auto len = key.length();
    if (len < min_length || len > max_length) {
      return nullptr;
    }
    auto key_hash = Policy<CharT>::hash(key);
    auto start_index = key_hash % modulo;
    template for (constexpr auto I : std::views::iota(0zU, P)) {
      const auto& cur = unwrap(entries[start_index + I * I]).elements;
      if (cur.first == 0) {
        return nullptr;
      }
      if (cur.first == key_hash) {
        if (Policy<CharT>::equals(cur.second, key)) {
          return std::addressof(cur.third);
        }
        return nullptr;
      }
    }
    return nullptr;
  }

  constexpr auto operator[](std::basic_string_view<CharT> key) const -> const value_type& {
    auto* p = find(key);
    return p ? *p : default_v<value_type>;
  }

  const element_type* entries;  // Entry range size = modulo + P^2
  size_t min_length;
  size_t max_length;
  size_t actual_size;
  size_t modulo;
};

// -------- Builder --------

struct hash_table_with_skey_options {
  bool ascii_case_insensitive;
  bool adjusts_alignment;
  double min_load_factor;
  size_t max_n_hash_probing_attempts;
  size_t max_n_iterations;
};

consteval auto test_hash_modulo(std::span<const size_t> hash_values, size_t m, size_t p) -> bool {
  auto filled = std::vector<uint8_t>(m + p * p, false);
  for (auto v : hash_values) {
    auto ok = false;
    v %= m;
    for (auto i = 0zU; !ok && i < p; i++) {
      auto p = v + i * i;
      if (!filled[p]) {
        filled[p] = true;
        ok = true;
      }
    }
    if (!ok) return false;
  }
  return true;
}

consteval auto find_best_hash_modulo(std::span<const size_t> hash_values,
                                     const hash_table_with_skey_options& options) -> size_t {
  auto n = hash_values.size();
  auto p = options.max_n_hash_probing_attempts;
  auto limit = static_cast<size_t>(n / options.min_load_factor);
  auto n_iterations = 0;

  for (auto m = std::bit_ceil(n); m <= limit; m *= 2) {
    if (n_iterations++ >= options.max_n_iterations) return 0;
    if (test_hash_modulo(hash_values, m, p)) return m;
  }
  for (auto m = n + !!(n % 2 == 0); m <= limit; m += 2) {
    if (n_iterations++ >= options.max_n_iterations) return 0;
    if (test_hash_modulo(hash_values, m, p)) return m;
  }
  return 0;
}

template <bool A, size_t P, class CharT, class V, template <class> class Policy>
consteval auto make_hash_table_with_skey_impl(
    std::span<const meta_tuple<meta_basic_string_view<CharT>, V>> kv_pairs,
    std::span<const size_t> hash_values,
    size_t modulo) -> std::meta::info {
  // Makes obj
  auto to_length = [](const auto& entry) { return entry.elements.first.length(); };
  auto [min_length, max_length] = std::ranges::minmax(kv_pairs | std::views::transform(to_length));

  auto res = hash_table_with_skey<A, P, CharT, V, Policy>{
      .min_length = min_length,
      .max_length = max_length,
      .actual_size = kv_pairs.size(),
      .modulo = modulo,
  };

  auto entries = std::vector<meta_tuple<size_t, meta_basic_string_view<CharT>, V>>(modulo + P * P);
  for (size_t i = 0, n = kv_pairs.size(); i < n; i++) {
    auto base_index = hash_values[i] % modulo;
    auto index = static_cast<size_t>(-1);

    for (size_t j = 0; j < P; j++) {
      auto cur_index = base_index + j * j;
      if (entries[cur_index].elements.first == 0) {
        index = cur_index;
        break;
      }
    }
    const auto& [k, v] = kv_pairs[i];
    entries[index] = meta_tuple{hash_values[i], k, v};
  }
  if constexpr (A) {
    res.entries = std::define_static_array(entries | to_aligned).data();
  } else {
    res.entries = std::define_static_array(entries).data();
  }
  return std::meta::reflect_constant(res);
}

template <class CharT, class V>
consteval auto try_make_hash_table_with_skey(
    std::span<const meta_tuple<meta_basic_string_view<CharT>, V>> kv_pairs,
    std::span<const size_t> hash_values,
    const hash_table_with_skey_options& options) -> std::optional<std::meta::info> {
  // (1) Empty
  if (kv_pairs.empty()) {
    return make_empty_with_skey<CharT, V>();
  }
  // (2) Hash table
  auto modulo = find_best_hash_modulo(hash_values, options);
  if (modulo == 0) {
    return std::nullopt;  // Not found
  }
  using call_signature =
      std::meta::info(std::span<const meta_tuple<meta_basic_string_view<CharT>, V>>,
                      std::span<const size_t>,
                      size_t);
  auto A = std::meta::reflect_constant(options.adjusts_alignment);
  auto P = std::meta::reflect_constant(options.max_n_hash_probing_attempts);
  auto policy = get_skey_policy_template(options.ascii_case_insensitive);
  auto fn = extract<call_signature*>(^^make_hash_table_with_skey_impl, A, P, ^^CharT, ^^V, policy);
  return fn(kv_pairs, hash_values, modulo);
}
};  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_BY_HASH_TABLE_HPP
