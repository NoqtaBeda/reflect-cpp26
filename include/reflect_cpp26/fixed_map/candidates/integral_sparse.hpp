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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_SPARSE_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_SPARSE_HPP

#include <reflect_cpp26/fixed_map/candidates/integral_empty.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26::impl::map {
template <class K, class V>
struct linear_search_with_ikey {
  using key_type = K;
  using value_type = V;

private:
  using element_type = meta_tuple<K, V>;

public:
  constexpr auto size() const -> size_t {
    return entries.size();
  }

  constexpr auto find(key_type key) const -> const value_type* {
    for (const auto& cur : entries) {
      if (key == cur.elements.first) {
        return std::addressof(cur.elements.second);
      }
    }
    return nullptr;
  }

  constexpr auto find(non_bool_integral auto key) const -> const value_type* {
    if (!in_range<key_type>(key)) {
      return nullptr;
    }
    return find(static_cast<key_type>(key));
  }

  constexpr auto operator[](non_bool_integral auto key) const -> const value_type& {
    auto* p = find(key);
    return p ? *p : default_v<value_type>;
  }

  meta_span<element_type> entries;
};

template <bool A, class K, class V>
struct binary_search_with_ikey {
  using key_type = K;
  using value_type = V;

private:
  using element_type = std::conditional_t<A, aligned<meta_tuple<K, V>>, meta_tuple<K, V>>;

public:
  constexpr auto size() const -> size_t {
    return entries.size();
  }

  constexpr auto find(key_type key) const -> const value_type* {
    const auto* head = entries.begin();
    const auto* tail = entries.end();
    while (head < tail) {
      const auto* mid = head + (tail - head) / 2;
      const auto& entry = unwrap(*mid);
      if (key == entry.elements.first) {
        return std::addressof(entry.elements.second);
      }
      if (key > entry.elements.first) {
        head = mid + 1;
      } else {
        tail = mid;
      }
    }
    return nullptr;
  }

  constexpr auto find(non_bool_integral auto key) const -> const value_type* {
    if (!in_range<key_type>(key)) {
      return nullptr;
    }
    return find(static_cast<key_type>(key));
  }

  constexpr auto operator[](non_bool_integral auto key) const -> const value_type& {
    auto* p = find(key);
    return p ? *p : default_v<value_type>;
  }

  meta_span<element_type> entries;
};

// -------- Builder --------

struct sparse_with_ikey_options {
  bool adjusts_alignment;
  size_t binary_search_threshold;
};

template <class K, class V>
consteval auto make_sparse_with_ikey(std::span<const meta_tuple<K, V>> sorted_entries,
                                     sparse_with_ikey_options options) -> std::meta::info {
  auto n = sorted_entries.size();
  if (n == 0) {
    // (1) Empty
    return make_empty_with_ikey<V>();
  }
  if (n < options.binary_search_threshold) {
    // (2) Linear
    auto entries = reflect_cpp26::define_static_array(sorted_entries);
    auto obj = linear_search_with_ikey<K, V>{entries};
    return std::meta::reflect_constant(obj);
  }
  // (3) Binary search
  if (options.adjusts_alignment) {
    // (3.1) with alignment optimization
    auto entries = reflect_cpp26::define_static_array(sorted_entries | to_aligned);
    auto obj = binary_search_with_ikey<true, K, V>{entries};
    return std::meta::reflect_constant(obj);
  }
  // (3.2) without alignment optimization
  auto entries = reflect_cpp26::define_static_array(sorted_entries);
  auto obj = binary_search_with_ikey<false, K, V>{entries};
  return std::meta::reflect_constant(obj);
}
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_INTEGRAL_SPARSE_HPP
