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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_STRING_KEY_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_STRING_KEY_HPP

#include <reflect_cpp26/utils/fixed_map/impl/common.hpp>
#include <reflect_cpp26/utils/fixed_map/integral_key.hpp>
#include <reflect_cpp26/utils/string_hash.hpp>
#include <climits>

namespace reflect_cpp26 {
struct string_key_map_options {
  // Mimimum load factor for length-based, ith-character-based or hash-based
  // string-key flat map, where holes are filled with value-initialized Value{}.
  double min_load_factor = 0.5;
  // Length threshold to enable optimized data structures. Naive linear list
  // searching is used if the input length is less than this threshold.
  size_t optimization_threshold = 4;
  // Used by the underlying integral-key data structure of length-based and
  // ith-character-based string-key flat map.
  size_t dense_part_threshold = 4;
  // Length threshold to enable binary search for hash-based or naive
  // string-key flat map. Linear search is applied otherwise.
  size_t binary_search_threshold = 8;
};
} // namespace reflect_cpp26

namespace reflect_cpp26::impl {
// Value type of wrapper is the whole underlying kv-pair since we need the
// wrapped key (which is a meta_string_view) for further checking.
template <class KVPair>
struct string_key_kv_pair_wrapper_by_length {
  using underlying_type = KVPair;
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

template <size_t J, class KVPair>
struct string_key_kv_pair_wrapper_by_character {
  using underlying_type = KVPair;

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
} // namespace reflect_cpp26::impl

template <class KVPair>
struct std::tuple_size<
  reflect_cpp26::impl::string_key_kv_pair_wrapper_by_length<KVPair>>
  : std::integral_constant<size_t, 2> {};

template <size_t J, class KVPair>
struct std::tuple_size<
  reflect_cpp26::impl::string_key_kv_pair_wrapper_by_character<J, KVPair>>
  : std::integral_constant<size_t, 2> {};

template <size_t I, class KVPair>
  requires (I < 2)
struct std::tuple_element<I,
    reflect_cpp26::impl::string_key_kv_pair_wrapper_by_length<KVPair>> {
  using type = std::conditional_t<(I == 0), size_t, KVPair>;
};

template <size_t I, size_t J, class KVPair>
  requires (I < 2)
struct std::tuple_element<I,
    reflect_cpp26::impl::string_key_kv_pair_wrapper_by_character<J, KVPair>> {
  using type = std::conditional_t<(I == 0), char, KVPair>;
};

namespace reflect_cpp26::impl {
template <class KVPair>
struct empty_string_key_map {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = const value_type&;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  static constexpr size_t size() {
    return 0;
  }
  static constexpr auto get(std::string_view) -> std::pair<result_type, bool> {
    return {map_null_value_v<value_type>, false};
  }
  static constexpr auto operator[](std::string_view) -> const value_type& {
    return map_null_value_v<value_type>;
  }
};

#define REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE                     \
  constexpr auto operator[](std::string_view key) const -> result_type {  \
    return get(key).first;                                                \
  }

template <class KVPair>
struct naive_string_key_map {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  meta_span<KVPair> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::string_view key) const -> std::pair<result_type, bool>
  {
    for (const auto& cur: _entries) {
      if (get_first(cur) == key) {
        return {get_second(cur), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

template <class Underlying>
struct string_key_map_by_length {
  // => string_key_kv_pair_wrapper_by_length<KVPair>
  using kv_pair_wrapper_type = typename Underlying::kv_pair_type;
  using kv_pair_type = typename kv_pair_wrapper_type::underlying_type;

  using key_type = std::tuple_element_t<0, kv_pair_type>;
  using value_type = std::tuple_element_t<1, kv_pair_type>;
  using result_type = std::invoke_result_t<get_second_t, const kv_pair_type&>;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  Underlying _underlying;

  constexpr size_t size() const {
    return _underlying.size();
  }

  constexpr auto get(std::string_view key) const -> std::pair<result_type, bool>
  {
    auto [kv_pair, found] = _underlying.get(key.length());
    if (!found || get_first(kv_pair) != key) {
      return {map_null_value_v<value_type>, false};
    }
    return {get_second(kv_pair), true};
  };

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

template <class Underlying>
struct string_key_map_by_character {
  // => string_key_kv_pair_wrapper_by_character<I, KVPair>
  using kv_pair_wrapper_type = typename Underlying::kv_pair_type;
  using kv_pair_type = typename kv_pair_wrapper_type::underlying_type;

  using key_type = std::tuple_element_t<0, kv_pair_type>;
  using value_type = std::tuple_element_t<1, kv_pair_type>;
  using result_type = std::invoke_result_t<get_second_t, const kv_pair_type&>;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  static constexpr auto differed_index = kv_pair_wrapper_type::differed_index;
  Underlying _underlying;

  constexpr size_t size() const {
    return _underlying.size();
  }

  constexpr auto get(std::string_view key) const -> std::pair<result_type, bool>
  {
    if (key.length() <= differed_index) {
      return {map_null_value_v<value_type>, false};
    }

    auto [kv_pair, found] = _underlying.get(key[differed_index]);
    if (!found || get_first(kv_pair) != key) {
      return {map_null_value_v<value_type>, false};
    }
    return {get_second(kv_pair), true};
  };

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

constexpr auto string_key_hash_map_probing_size = 1zU;
constexpr auto string_key_hash_map_probing_deltas = std::views::iota(
  0zU, string_key_hash_map_probing_size + 1);

template <class KVPair>
struct string_key_kv_pair_wrapper_with_hash {
  uint64_t hash;
  KVPair underlying;
};

template <class KVPair>
using string_key_kv_pair_aligned_wrapper_with_hash =
  alignment_adjusted_wrapper<string_key_kv_pair_wrapper_with_hash<KVPair>>;

template <class KVPair, template <class> class Derived>
struct string_key_map_by_hash_base {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  using derived_type = Derived<KVPair>;
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;

  meta_span<wrapper_type> _entries;
  size_t _actual_size;

  constexpr size_t size() const {
    return _actual_size;
  }

  constexpr uint64_t get_modulo(uint64_t hash) const {
    return static_cast<const derived_type*>(this)->do_get_modulo(hash);
  }

  constexpr auto get(std::string_view key) const -> std::pair<result_type, bool>
  {
    auto key_hash = bkdr_hash64(key);
    auto i = get_modulo(key_hash);
    template for (constexpr auto delta: string_key_hash_map_probing_deltas) {
      const auto& cur = _entries[i + delta].underlying;
      if (get_first(cur.underlying).head == nullptr) {
        return {map_null_value_v<value_type>, false};
      }
      if (cur.hash == key_hash) {
        if (get_first(cur.underlying) == key) {
          return {get_second(cur.underlying), true};
        }
        return {map_null_value_v<value_type>, false};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

template <class KVPair>
struct string_key_map_by_hash_fast
  : string_key_map_by_hash_base<KVPair, string_key_map_by_hash_fast>
{
  // Assertion: buchet_size_mask = 2^n - 1
  size_t _bucket_size_mask;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash & _bucket_size_mask;
  }
};

template <class KVPair>
struct string_key_map_by_hash_slow
  : string_key_map_by_hash_base<KVPair, string_key_map_by_hash_slow>
{
  // bucket_size == entries.size() - string_key_hash_map_probing_size
  size_t _bucket_size;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash % _bucket_size;
  }
};

// Assertion: No hash collision
template <class KVPair>
struct string_key_map_by_hash_binary_search_fast {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  meta_span<wrapper_type> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::string_view key) const -> std::pair<result_type, bool>
  {
    auto key_hash = bkdr_hash64(key);
    for (auto head = _entries.head, tail = _entries.tail; head < tail; ) {
      const auto* mid = head + (tail - head) / 2;
      const auto& [mid_hash, mid_kv_pair] = mid->underlying;
      if (mid_hash == key_hash) {
        if (get_first(mid_kv_pair) == key) {
          return {get_second(mid_kv_pair), true};
        }
        return {map_null_value_v<value_type>, false};
      }
      (mid_hash < key_hash) ? (head = mid + 1) : (tail = mid);
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

template <class KVPair>
struct string_key_map_by_hash_binary_search_slow {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  meta_span<wrapper_type> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::string_view key) const -> std::pair<result_type, bool>
  {
    auto key_hash = bkdr_hash64(key);
    auto pos = std::ranges::lower_bound(_entries, key_hash, {},
      [](const auto& entry) {
        return entry.underlying.hash;
      });
    for (; pos < _entries.end() && pos->underlying.hash == key_hash; ++pos) {
      const auto& cur = pos->underlying.underlying;
      if (get_first(cur) == key) {
        return {get_second(cur), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// Assertion: No hash collision
template <class KVPair>
struct string_key_map_by_hash_linear_search {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  static_assert(std::is_same_v<key_type, meta_string_view>);

  // Alignment is not needed for linear search.
  using wrapper_type = string_key_kv_pair_wrapper_with_hash<KVPair>;
  meta_span<wrapper_type> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::string_view key) const -> std::pair<result_type, bool>
  {
    auto key_hash = bkdr_hash64(key);
    for (const auto& [hash, kv_pair]: _entries) {
      if (hash != key_hash) {
        continue;
      }
      if (get_first(kv_pair) == key) {
        return {get_second(kv_pair), true};
      }
      return {map_null_value_v<value_type>, false};
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

struct string_key_map_meta_result {
  enum {
    direct,
    length_based,
    character_based,
  } type;

  union {
    std::meta::info as_direct;
    integral_key_map_meta_result as_length_based;
    integral_key_map_meta_result as_character_based;
  };
};

template <class KVPair>
consteval auto make_naive_string_key_map(
  const std::vector<KVPair>& kv_pairs) -> std::meta::info
{
  if (kv_pairs.empty()) {
    return std::meta::reflect_constant(empty_string_key_map<KVPair>{});
  }
  auto res = naive_string_key_map<KVPair>{
    ._entries = reflect_cpp26::define_static_array(kv_pairs)};
  return std::meta::reflect_constant(res);
}

template <class KVPair>
consteval auto make_length_based_string_key_map(
  const std::vector<KVPair>& length_sorted_kv_pairs,
  const string_key_map_options& options)
  -> integral_key_map_meta_result
{
  auto wrapped = length_sorted_kv_pairs | std::views::transform(
    string_key_kv_pair_wrapper_by_length<KVPair>::make);

  return make_integral_key_map(wrapped, {
    .already_sorted = true,
    .already_unique = true,
    .default_value_is_always_invalid = true,
    .min_load_factor = options.min_load_factor,
    .dense_part_threshold = options.dense_part_threshold,
    .binary_search_threshold = options.binary_search_threshold,
  });
}

template <class KVPair>
consteval bool characters_are_all_different(
  const std::vector<KVPair>& kv_pairs, size_t index)
{
  auto table = std::array<bool, (1u << CHAR_BIT)>{};
  for (const auto& cur: kv_pairs) {
    auto c = get_first(cur)[index];
    if (table[c - CHAR_MIN]) {
      return false;
    }
    table[c - CHAR_MIN] = true;
  }
  return true;
}

template <size_t I, class KVPair>
consteval auto make_character_based_string_key_map_impl(
  const std::vector<KVPair>& kv_pairs,
  const string_key_map_options& options)
  -> integral_key_map_meta_result
{
  auto wrapped = kv_pairs | std::views::transform(
    string_key_kv_pair_wrapper_by_character<I, KVPair>::make);

  return make_integral_key_map(wrapped, {
    .default_value_is_always_invalid = true,
    .min_load_factor = options.min_load_factor,
    .dense_part_threshold = options.dense_part_threshold,
    .binary_search_threshold = options.binary_search_threshold,
  });
}

template <class KVPair>
consteval auto make_character_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, size_t index,
  const string_key_map_options& options)
{
  using fn_signature = integral_key_map_meta_result (*)(
    const std::vector<KVPair>&, const string_key_map_options&);

  auto fn = extract_substituted<fn_signature>(
    ^^make_character_based_string_key_map_impl,
    std::meta::reflect_constant(index), ^^KVPair);

  return fn(kv_pairs, options);
}

constexpr bool has_hash_collision(std::vector<uint64_t> hash_values)
{
  std::ranges::sort(hash_values);
  return std::ranges::adjacent_find(hash_values) != hash_values.end();
}

template <class KVPair>
consteval auto make_hash_linear_search_based_string_key_map(
  const std::vector<KVPair>& kv_pairs,
  const std::vector<uint64_t>& hash_values) -> std::meta::info
{
  using wrapper_type = string_key_kv_pair_wrapper_with_hash<KVPair>;
  auto entries = std::vector<wrapper_type>{};
  entries.reserve(kv_pairs.size());
  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    entries.push_back({.hash = hash_values[i], .underlying = kv_pairs[i]});
  }
  auto res = string_key_map_by_hash_linear_search<KVPair>{
    ._entries = reflect_cpp26::define_static_array(entries)
  };
  return std::meta::reflect_constant(res);
}

template <class KVPair>
consteval auto make_hash_search_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  bool has_hash_collision, size_t binary_search_threshold) -> std::meta::info
{
  if (kv_pairs.size() < binary_search_threshold) {
    return make_hash_linear_search_based_string_key_map(
      kv_pairs, hash_values);
  }
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  auto entries = std::vector<wrapper_type>{};
  entries.reserve(kv_pairs.size());
  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    entries.push_back({{.hash = hash_values[i], .underlying = kv_pairs[i]}});
  }
  std::ranges::sort(entries, {}, [](const auto& v) {
    return v.underlying.hash;
  });
  auto static_entries = reflect_cpp26::define_static_array(entries);
  if (has_hash_collision) {
    auto res = string_key_map_by_hash_binary_search_slow{static_entries};
    return std::meta::reflect_constant(res);
  }
  auto res = string_key_map_by_hash_binary_search_fast{static_entries};
  return std::meta::reflect_constant(res);
}

constexpr bool test_bucket_size(
  const std::vector<uint64_t>& hash_values, size_t bucket_size)
{
  // We use uint8_t to prevent bit operation overhead in std::vector<bool>.
  auto vis = std::vector<uint8_t>(
    bucket_size + string_key_hash_map_probing_size);
  for (auto h: hash_values) {
    auto ok = false;
    for (auto delta: string_key_hash_map_probing_deltas) {
      if (!vis[h % bucket_size + delta]) {
        vis[h % bucket_size + delta] = ok = true;
        break;
      }
    }
    if !consteval {
      std::println("h = {:016x}: {}", h, ok ? "OK" : "NOT OK");
      std::println("vis = {}", vis);
    }
    if (!ok) { return false; }
  }
  return true;
}

consteval auto find_best_bucket_size(
  const std::vector<uint64_t>& hash_values, double min_load_factor) -> size_t
{
  auto size = hash_values.size();
  auto limit = static_cast<size_t>(size / min_load_factor);
  for (auto n = std::bit_ceil(size); n <= limit; n <<= 1) {
    if (test_bucket_size(hash_values, n)) {
      return n;
    }
  }
  for (auto n = size + (size % 2 == 0); n <= limit; n += 2) {
    if (test_bucket_size(hash_values, n)) {
      return n;
    }
  }
  return 0; // Failed
}

template <class KVPair>
consteval auto make_hash_table_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  uint64_t bucket_size) -> std::meta::info
{
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;
  auto table = std::vector<wrapper_type>(
    bucket_size + string_key_hash_map_probing_size);

  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    for (auto j = hash_values[i] % bucket_size; ; j++) {
      auto& cur = table[j].underlying;
      if (get_first(cur.underlying).head == nullptr) {
        cur.hash = hash_values[i];
        cur.underlying = kv_pairs[i];
        break;
      }
    }
  }
  auto underlying_range = reflect_cpp26::define_static_array(table);
  if (std::has_single_bit(bucket_size)) {
    auto res = string_key_map_by_hash_fast<KVPair>{
      {._entries = underlying_range, ._actual_size = kv_pairs.size()},
      /* ._bucket_size_mask = */ bucket_size - 1,
    };
    return std::meta::reflect_constant(res);
  }
  auto res = string_key_map_by_hash_slow<KVPair>{
    {._entries = underlying_range, ._actual_size = kv_pairs.size()},
    /* ._bucket_size = */ bucket_size,
  };
  return std::meta::reflect_constant(res);
}

template <string_key_kv_pair KVPair>
consteval auto make_string_key_map_impl(
  std::vector<KVPair> kv_pairs, string_key_map_options options)
  -> string_key_map_meta_result
{
  // (1) Empty or naive
  if (kv_pairs.size() < options.optimization_threshold) {
    return {
      .type = string_key_map_meta_result::direct,
      .as_direct = make_naive_string_key_map(kv_pairs),
    };
  }
  auto proj_length_fn = [](const KVPair& p) {
    return get_first(p).length();
  };
  std::ranges::sort(kv_pairs, {}, proj_length_fn);
  auto len_dup_pos = std::ranges::adjacent_find(kv_pairs, {}, proj_length_fn);
  // (2) Length-based
  if (kv_pairs.end() == len_dup_pos) {
    auto res = make_length_based_string_key_map(kv_pairs, options);
    return {
      .type = string_key_map_meta_result::length_based,
      .as_length_based = res,
    };
  }
  // (3) ith-character-based
  auto min_length = get_first(kv_pairs.front()).length();
  for (auto i = 0zU; i < min_length; i++) {
    if (!characters_are_all_different(kv_pairs, i)) {
      continue;
    }
    auto res = make_character_based_string_key_map(kv_pairs, i, options);
    return {
      .type = string_key_map_meta_result::character_based,
      .as_character_based = res,
    };
  }
  auto to_hash_fn = [](const KVPair& p) { return bkdr_hash64(get_first(p)); };
  auto hash_values = kv_pairs
    | std::views::transform(to_hash_fn)
    | std::ranges::to<std::vector>();
  // (4) Hash binary search: slow path due to hash collision
  if (has_hash_collision(hash_values)) {
    auto res = make_hash_search_based_string_key_map(
      kv_pairs, hash_values, true, options.binary_search_threshold);
    return {
      .type = string_key_map_meta_result::direct,
      .as_direct = res,
    };
  }
  auto best_bucket_size =
    find_best_bucket_size(hash_values, options.min_load_factor);
  // (5) Hash-table-based (hash collision is excluded)
  if (best_bucket_size != 0) {
    auto res = make_hash_table_based_string_key_map(
      kv_pairs, hash_values, best_bucket_size);
    return {
      .type = string_key_map_meta_result::direct,
      .as_direct = res,
    };
  }
  // (6) Hash binary search: fast path, without hash collision
  auto res = make_hash_search_based_string_key_map(
    kv_pairs, hash_values, false, options.binary_search_threshold);
  return {
    .type = string_key_map_meta_result::direct,
    .as_direct = res,
  };
}

template <std::ranges::input_range KVPairRange>
consteval auto make_string_key_map(
  KVPairRange&& kv_pairs, string_key_map_options options = {})
  -> string_key_map_meta_result
{
  constexpr auto input_is_vector =
    is_template_instance_of_v<KVPairRange, std::vector>;
  if constexpr (input_is_vector) {
    return make_string_key_map_impl(
      std::forward<KVPairRange>(kv_pairs), options);
  } else {
    auto vec = std::forward<KVPairRange>(kv_pairs)
             | std::ranges::to<std::vector>();
    return make_string_key_map_impl(std::move(vec), options);
  }
}

#define REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(matched_type)   \
  template <string_key_map_meta_result V>                          \
    requires (V.type == string_key_map_meta_result::matched_type)  \
  constexpr auto extract_string_key_map_v<V>

template <string_key_map_meta_result V>
constexpr auto extract_string_key_map_v = compile_error("Placeholder");

REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(direct) =
  extract<V.as_direct>();

REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(length_based) =
  string_key_map_by_length{
    ._underlying = extract_integral_key_map_v<V.as_length_based>
  };

REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(character_based) =
  string_key_map_by_character{
    ._underlying = extract_integral_key_map_v<V.as_character_based>
  };

#undef REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT
} // namespace reflect_cpp26::impl

#define REFLECT_CPP26_STRING_KEY_FIXED_MAP(kv_pairs, ...)                \
  [&]() consteval {                                                      \
    constexpr auto meta_res = reflect_cpp26::impl::make_string_key_map(  \
        kv_pairs, ##__VA_ARGS__);                                        \
    return reflect_cpp26::impl::extract_string_key_map_v<meta_res>;      \
  }()

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_STRING_KEY_HPP
