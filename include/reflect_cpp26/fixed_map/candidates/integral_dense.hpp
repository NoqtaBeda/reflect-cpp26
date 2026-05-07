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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_DENSE_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_DENSE_HPP

#include <optional>
#include <reflect_cpp26/fixed_map/candidates/integral_empty.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26::impl::map {
template <bool A, class K, class V>
struct fully_dense_with_ikey {
  using key_type = K;
  using value_type = V;

private:
  using element_type = std::conditional_t<A, aligned<V>, V>;

public:
  constexpr auto size() const -> size_t {
    return static_cast<size_t>(max_key - min_key + 1);
  }

  constexpr auto find(key_type key) const -> std::optional<const value_type&> {
    if (key >= min_key && key <= max_key) {
      return unwrap(entries[key - min_key]);
    }
    return std::nullopt;
  }

  constexpr auto find(non_bool_integral auto key) const -> std::optional<const value_type&> {
    if (!in_range<key_type>(key)) {
      return std::nullopt;
    }
    return find(static_cast<key_type>(key));
  }

  constexpr auto operator[](non_bool_integral auto key) const -> const value_type& {
    auto p = find(key);
    return p ? *p : default_v<value_type>;
  }

  const element_type* entries;
  key_type min_key;
  key_type max_key;
};

template <bool A, class K, class V>
struct non_null_dense_with_ikey {
  using key_type = K;
  using value_type = V;

private:
  using element_type = std::conditional_t<A, aligned<V>, V>;

public:
  constexpr auto size() const -> size_t {
    return actual_size;
  }

  constexpr auto find(key_type key) const -> std::optional<const value_type&> {
    if (key >= min_key && key <= max_key) {
      const auto& target = unwrap(entries[key - min_key]);
      if (!(target == default_v<value_type>)) {
        return target;
      }
    }
    return std::nullopt;
  }

  constexpr auto find(non_bool_integral auto key) const -> std::optional<const value_type&> {
    if (!in_range<key_type>(key)) {
      return std::nullopt;
    }
    return find(static_cast<key_type>(key));
  }

  constexpr auto operator[](non_bool_integral auto key) const -> const value_type& {
    auto p = find(key);
    return p ? *p : default_v<value_type>;
  }

  const element_type* entries;
  size_t actual_size;
  key_type min_key;
  key_type max_key;
};

template <bool A, class K, class V>
struct dense_with_ikey {
  using key_type = K;
  using value_type = V;

private:
  using element_pair_type = meta_tuple<V, bool>;
  using element_type = std::conditional_t<A, aligned<element_pair_type>, element_pair_type>;

public:
  constexpr auto size() const -> size_t {
    return actual_size;
  }

  constexpr auto find(key_type key) const -> std::optional<const value_type&> {
    if (key >= min_key && key <= max_key) {
      const auto& target_entry = unwrap(entries[key - min_key]);
      if (target_entry.elements.second) {
        return target_entry.elements.first;
      }
    }
    return std::nullopt;
  }

  constexpr auto find(non_bool_integral auto key) const -> std::optional<const value_type&> {
    if (!in_range<key_type>(key)) {
      return std::nullopt;
    }
    return find(static_cast<key_type>(key));
  }

  constexpr auto operator[](non_bool_integral auto key) const -> const value_type& {
    auto p = find(key);
    return p ? *p : default_v<value_type>;
  }

  const element_type* entries;
  size_t actual_size;
  key_type min_key;
  key_type max_key;
};

// -------- Builder --------

struct dense_with_ikey_options {
  bool adjusts_alignment;
};

template <class K, class V>
consteval auto make_dense_with_ikey(std::span<const meta_tuple<K, V>> sorted_entries,
                                    dense_with_ikey_options options) -> std::meta::info {
  auto n = sorted_entries.size();
  if (n == 0) {
    // (1) Empty
    return make_empty_with_ikey<V>();
  }
  auto min_key = sorted_entries.front().elements.first;
  auto max_key = sorted_entries.back().elements.first;
  if (max_key - min_key + 1 == n) {
    // (2) Fully dense
    if (options.adjusts_alignment) {
      // (2.1) with alignment optimization
      auto entries = std::define_static_array(sorted_entries | to_values | to_aligned);
      auto obj = fully_dense_with_ikey<true, K, V>{entries.data(), min_key, max_key};
      return std::meta::reflect_constant(obj);
    } else {
      // (2.2) without alignment optimization
      auto entries = std::define_static_array(sorted_entries | to_values);
      auto obj = fully_dense_with_ikey<false, K, V>{entries.data(), min_key, max_key};
      return std::meta::reflect_constant(obj);
    }
  }

  using entry_pair_type = meta_tuple<V, bool>;
  auto values = std::vector<entry_pair_type>(max_key - min_key + 1);
  auto has_false_holes = !is_equal_comparable_v<V, V>;

  for (const auto& [k, v] : sorted_entries) {
    values[k - min_key].elements.first = v;
    values[k - min_key].elements.second = true;
    // If operator== is unsupported, we take the worst consumption that false hole may appear.
    if constexpr (is_equal_comparable_v<V, V>) {
      if (v == default_v<V>) has_false_holes = true;
    }
  }

  if (has_false_holes) {
    // (3) Dense (with an additional flag for validation)
    if (options.adjusts_alignment) {
      // (3.1) with alignment optimization
      auto entries = std::define_static_array(values | to_aligned);
      auto obj = dense_with_ikey<true, K, V>{entries.data(), n, min_key, max_key};
      return std::meta::reflect_constant(obj);
    } else {
      // (3.2) without alignment optimization
      auto entries = std::define_static_array(values);
      auto obj = dense_with_ikey<false, K, V>{entries.data(), n, min_key, max_key};
      return std::meta::reflect_constant(obj);
    }
  }
  // (4) Holey dense (all holes are "real", i.e. no value happen to be equal to default_v<V>)
  if (options.adjusts_alignment) {
    // (4.1) with alignment optimization
    auto entries = std::define_static_array(values | to_keys | to_aligned);
    auto obj = non_null_dense_with_ikey<true, K, V>{entries.data(), n, min_key, max_key};
    return std::meta::reflect_constant(obj);
  } else {
    // (4.2) without alignment optimization
    auto entries = std::define_static_array(values | to_keys);
    auto obj = non_null_dense_with_ikey<false, K, V>{entries.data(), n, min_key, max_key};
    return std::meta::reflect_constant(obj);
  }
}
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_DENSE_HPP
