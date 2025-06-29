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

#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/common.hpp>
#include <reflect_cpp26/utils/fixed_map/integral_key.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <reflect_cpp26/utils/string_hash.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>
#include <climits>

namespace reflect_cpp26 {
struct string_key_map_options {
  // Whether the fixed map is built in a case-insensitive manner.
  bool case_insensitive = false;
  // Whether optimization is enabled by using the i-th character as key
  // where i is the index such that every input string differs at index i.
  bool enables_lookup_by_differed_character = false;
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

struct string_key_equal_t {
  struct equal_length_tag_t {};
  static constexpr auto equal_length = equal_length_tag_t{};

  static constexpr uint64_t hash(const string_like auto& u) {
    return bkdr_hash64(u);
  }

  static constexpr auto convert_char(char_type auto c) {
    return c;
  }

  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool operator()(
    meta_basic_string_view<CharT> t, const StringU& u)
  {
    return t == u;
  }

  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool operator()(
    meta_basic_string_view<CharT> t, const StringU& u, equal_length_tag_t)
  {
    const auto* iu = std::ranges::data(u);
    for (const auto *it = t.head; it < t.tail; ++it, ++iu) {
      if (*it != *iu) { return false; }
    }
    return true;
  }
};

struct string_key_ascii_ci_equal_t {
  struct equal_length_tag_t {};
  static constexpr auto equal_length = equal_length_tag_t{};

  static constexpr uint64_t hash(const string_like auto& u) {
    return ascii_ci_bkdr_hash64(u);
  }

  static constexpr auto convert_char(char_type auto c) {
    return ascii_tolower(c);
  }

  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool operator()(
    meta_basic_string_view<CharT> t, const StringU& u)
  {
    return t.length() == u.length() && operator()(t, u, equal_length);
  }

  // t has already been converted to all-lower case.
  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool operator()(
    meta_basic_string_view<CharT> t, const StringU& u, equal_length_tag_t)
  {
    const auto* iu = std::ranges::data(u);
    for (const auto *it = t.head; it < t.tail; ++it, ++iu) {
      if (*it != ascii_tolower(*iu)) { return false; }
    }
    return true;
  }
};

// -------- Implementation of various string-key fixed maps --------

// Empty: Input is an empty kv-pair list.
template <class KVPair>
struct empty_string_key_map {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = const value_type&;
  using character_type = char_type_t<key_type>;

  static constexpr size_t size() {
    return 0;
  }
  static constexpr auto get(std::basic_string_view<character_type>)
    -> std::pair<result_type, bool> {
    return {map_null_value_v<value_type>, false};
  }

  static constexpr auto operator[](std::basic_string_view<character_type>)
    -> const value_type& {
    return map_null_value_v<value_type>;
  }
};

#define REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE                         \
  constexpr auto operator[](std::basic_string_view<character_type> key) const \
    -> result_type {                                                          \
    return get(key).first;                                                    \
  }

// Naive: Compares string contents one-by-one.
template <class EqComp, class KVPair>
struct naive_string_key_map {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  // Alignment is not needed for linear searching
  meta_span<KVPair> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    for (const auto& cur: _entries) {
      if (EqComp::operator()(get_first(cur), key)) {
        return {get_second(cur), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// By-length: Candidate data structure when all the keys have distinct length.
// The underlying data structure takes key length as key.
template <class EqComp, class Underlying>
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
      EqComp::operator()(get_first(kv_pair), key, EqComp::equal_length);
    if (!keys_are_equal) {
      return {map_null_value_v<value_type>, false};
    }
    return {get_second(kv_pair), true};
  };

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// By-character: Candidate data structure when all the keys have distinct
// characters at index I. The underlying data structure takes the I-th character
// of each string as key.
template <class EqComp, class Underlying>
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

    auto c = EqComp::convert_char(key[differed_index]);
    auto [kv_pair, found] = _underlying.get(c);
    if (!found || !EqComp::operator()(get_first(kv_pair), key)) {
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

// By-hash-table: Hash table is built for all the keys with open addressing.
// Precondition: No hash collision.
template <class EqComp, class KVPair, template <class, class> class Derived>
struct string_key_map_by_hash_base {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  using derived_type = Derived<EqComp, KVPair>;
  using wrapper_type = string_key_kv_pair_aligned_wrapper_with_hash<KVPair>;

  meta_span<wrapper_type> _entries;
  size_t _actual_size;

  constexpr size_t size() const {
    return _actual_size;
  }

  constexpr uint64_t get_modulo(uint64_t hash) const {
    return static_cast<const derived_type*>(this)->do_get_modulo(hash);
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    auto key_hash = EqComp::hash(key);
    auto i = get_modulo(key_hash);
    template for (constexpr auto delta: string_key_hash_map_probing_deltas) {
      const auto& cur = _entries[i + delta].underlying;
      if (get_first(cur.underlying).head == nullptr) { break; }

      if (cur.hash == key_hash) {
        if (!EqComp::operator()(get_first(cur.underlying), key)) { break; }
        return {get_second(cur.underlying), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// By-hash-table (faster): Bucket size is always power of 2.
template <class EqComp, class KVPair>
struct string_key_map_by_hash_fast
  : string_key_map_by_hash_base<EqComp, KVPair, string_key_map_by_hash_fast>
{
  // Assertion: buchet_size_mask = 2^n - 1
  size_t _bucket_size_mask;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash & _bucket_size_mask;
  }
};

// By-hash-table (slower): Bucket size is arbitrary and modulo is more costly.
template <class EqComp, class KVPair>
struct string_key_map_by_hash_slow
  : string_key_map_by_hash_base<EqComp, KVPair, string_key_map_by_hash_slow>
{
  // bucket_size == entries.size() - string_key_hash_map_probing_size
  size_t _bucket_size;
  constexpr uint64_t do_get_modulo(uint64_t hash) const {
    return hash % _bucket_size;
  }
};

// By-hash with binary search:
// Precondition: No hash collision.
template <class EqComp, class KVPair>
struct string_key_map_by_hash_binary_search_fast {
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
    auto key_hash = EqComp::hash(key);
    for (auto head = _entries.head, tail = _entries.tail; head < tail; ) {
      const auto* mid = head + (tail - head) / 2;
      const auto& [mid_hash, mid_kv_pair] = mid->underlying;
      if (mid_hash == key_hash) {
        if (!EqComp::operator()(get_first(mid_kv_pair), key)) { break; }
        return {get_second(mid_kv_pair), true};
      }
      (mid_hash < key_hash) ? (head = mid + 1) : (tail = mid);
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// By-hash with binary search and hash collision.
template <class EqComp, class KVPair>
struct string_key_map_by_hash_binary_search_slow {
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
    auto key_hash = EqComp::hash(key);
    auto pos = std::ranges::lower_bound(_entries, key_hash, {},
      [](const auto& entry) {
        return entry.underlying.hash;
      });
    for (; pos < _entries.end() && pos->underlying.hash == key_hash; ++pos) {
      const auto& cur = pos->underlying.underlying;
      if (EqComp::operator()(get_first(cur), key)) {
        return {get_second(cur), true};
      }
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// By-hash with binary search:
// Precondition: No hash collision.
template <class EqComp, class KVPair>
struct string_key_map_by_hash_linear_search {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = std::invoke_result_t<get_second_t, const KVPair&>;
  using character_type = char_type_t<key_type>;

  // Alignment is not needed for linear search.
  using wrapper_type = string_key_kv_pair_wrapper_with_hash<KVPair>;
  meta_span<wrapper_type> _entries;

  constexpr size_t size() const {
    return _entries.size();
  }

  constexpr auto get(std::basic_string_view<character_type> key) const
    -> std::pair<result_type, bool>
  {
    auto key_hash = EqComp::hash(key);
    for (const auto& [hash, kv_pair]: _entries) {
      if (hash != key_hash) {
        continue;
      }
      if (!EqComp::operator()(get_first(kv_pair), key)) { break; }
      return {get_second(kv_pair), true};
    }
    return {map_null_value_v<value_type>, false};
  }

  REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE
};

// -------- Selecting and building fixed map data structure --------

struct string_key_map_meta_result {
  struct with_entries_t {
    std::meta::info type;
    std::meta::info entries;
  };

  struct with_integral_key_t {
    std::meta::info type_template;
    std::meta::info eq_type;
    integral_key_map_meta_result underlying;
  };

  struct as_hash_table_t {
    std::meta::info type;
    std::meta::info entries;
    size_t actual_size;
    size_t bucket_size_or_mask;
  };

  enum class result_type {
    direct,
    with_entries,
    with_integral_key,
    as_hash_table,
  } type;

  union {
    std::meta::info direct;
    with_entries_t with_entries;
    with_integral_key_t with_integral_key;
    as_hash_table_t as_hash_table;
  };
};

template <class KVPair>
consteval void check_string_key_map_input(
  const std::vector<KVPair>& kv_pairs, const string_key_map_options& options)
{
  if (options.case_insensitive) {
    for (const auto& [k, _]: kv_pairs) {
      if (is_ascii_string(k)) { continue; }
      compile_error("Only ASCII strings allowed.");
    }
  }
}

consteval auto string_key_eq_type(bool case_insensitive)
{
  return case_insensitive
    ? ^^string_key_ascii_ci_equal_t
    : ^^string_key_equal_t;
}

template <class KVPair>
consteval auto make_naive_string_key_map(
  const std::vector<KVPair>& kv_pairs, const string_key_map_options& options)
  -> string_key_map_meta_result
{
  using enum string_key_map_meta_result::result_type;
  if (kv_pairs.empty()) {
    auto res = std::meta::reflect_constant(empty_string_key_map<KVPair>{});
    return {.type = direct, .direct = res};
  }
  auto eq_type = string_key_eq_type(options.case_insensitive);
  auto type = substitute(^^naive_string_key_map, eq_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(kv_pairs);
  return {
    .type = with_entries,
    .with_entries = {
      .type = type,
      .entries = std::meta::reflect_constant(entries),
    }
  };
}

template <class KVPair>
consteval bool key_lengths_are_all_distinct(
  const std::vector<KVPair>& kv_pairs, size_t* min_length)
{
  auto lengths = make_reserved_vector<size_t>(kv_pairs.size());
  for (const auto& [k, _]: kv_pairs) {
    lengths.push_back(k.length());
  }
  std::ranges::sort(lengths);
  *min_length = lengths.front();
  auto dup_pos = std::ranges::adjacent_find(lengths);
  return dup_pos == lengths.end();
}

template <class KVPair>
consteval auto make_length_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const string_key_map_options& options)
  -> string_key_map_meta_result
{
  auto wrapped = kv_pairs | std::views::transform(
    string_key_kv_pair_wrapper_by_length<KVPair>::make);

  auto underlying = make_integral_key_map(wrapped, {
    .already_unique = true,
    .default_value_is_always_invalid = true,
    .min_load_factor = options.min_load_factor,
    .dense_part_threshold = options.dense_part_threshold,
    .binary_search_threshold = options.binary_search_threshold,
  });

  auto eq_type = string_key_eq_type(options.case_insensitive);
  return {
    .type = string_key_map_meta_result::result_type::with_integral_key,
    .with_integral_key = {
      .type_template = ^^string_key_map_by_length,
      .eq_type = eq_type,
      .underlying = underlying,
    },
  };
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
  const std::vector<KVPair>& kv_pairs, const string_key_map_options& options)
  -> string_key_map_meta_result
{
  auto wrapped = kv_pairs | std::views::transform(
    string_key_kv_pair_wrapper_by_character<I, KVPair>::make);

  auto underlying = make_integral_key_map(wrapped, {
    .already_unique = true,
    .default_value_is_always_invalid = true,
    .min_load_factor = options.min_load_factor,
    .dense_part_threshold = options.dense_part_threshold,
    .binary_search_threshold = options.binary_search_threshold,
  });

  auto eq_type = string_key_eq_type(options.case_insensitive);
  return {
    .type = string_key_map_meta_result::result_type::with_integral_key,
    .with_integral_key = {
      .type_template = ^^string_key_map_by_character,
      .eq_type = eq_type,
      .underlying = underlying,
    },
  };
}

template <class KVPair>
consteval auto make_character_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, size_t index,
  const string_key_map_options& options) -> string_key_map_meta_result
{
  using fn_signature = string_key_map_meta_result (*)(
    const std::vector<KVPair>&, const string_key_map_options&);

  auto I = std::meta::reflect_constant(index);
  auto fn = extract<fn_signature>(
    ^^make_character_based_string_key_map_impl, I, ^^KVPair);

  return fn(kv_pairs, options);
}

constexpr bool has_hash_collision(std::vector<uint64_t> hash_values)
{
  std::ranges::sort(hash_values);
  return std::ranges::adjacent_find(hash_values) != hash_values.end();
}

template <class KVPair>
consteval auto make_hash_linear_search_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  const string_key_map_options& options) -> string_key_map_meta_result
{
  using wrapper_type = string_key_kv_pair_wrapper_with_hash<KVPair>;
  auto entries_vec = std::vector<wrapper_type>{};
  entries_vec.reserve(kv_pairs.size());
  for (auto i = 0zU, n = kv_pairs.size(); i < n; i++) {
    entries_vec.push_back({.hash = hash_values[i], .underlying = kv_pairs[i]});
  }
  auto eq_type = string_key_eq_type(options.case_insensitive);
  auto type = substitute(
    ^^string_key_map_by_hash_linear_search, eq_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(entries_vec);
  return {
    .type = string_key_map_meta_result::result_type::with_entries,
    .with_entries = {
      .type = type,
      .entries = std::meta::reflect_constant(entries),
    },
  };
}

template <class KVPair>
consteval auto make_hash_search_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  bool has_hash_collision, const string_key_map_options& options)
  -> string_key_map_meta_result
{
  // Linear search
  if (kv_pairs.size() < options.binary_search_threshold) {
    return make_hash_linear_search_based_string_key_map(
      kv_pairs, hash_values, options);
  }
  // ... or binary search
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

  auto eq_type = string_key_eq_type(options.case_insensitive);
  auto type_template = has_hash_collision
    ? ^^string_key_map_by_hash_binary_search_slow
    : ^^string_key_map_by_hash_binary_search_fast;
  auto type = substitute(type_template, eq_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(entries_vec);
  return {
    .type = string_key_map_meta_result::result_type::with_entries,
    .with_entries = {
      .type = type,
      .entries = std::meta::reflect_constant(entries),
    },
  };
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
    if (test_bucket_size(hash_values, n)) { return n; }
  }
  for (auto n = size + (size % 2 == 0); n <= limit; n += 2) {
    if (test_bucket_size(hash_values, n)) { return n; }
  }
  return 0; // Failed
}

template <class KVPair>
consteval auto make_hash_table_based_string_key_map(
  const std::vector<KVPair>& kv_pairs, const std::vector<uint64_t>& hash_values,
  uint64_t bucket_size, const string_key_map_options& options)
  -> string_key_map_meta_result
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
  auto is_fast = std::has_single_bit(bucket_size);
  auto eq_type = string_key_eq_type(options.case_insensitive);
  auto type_template = is_fast
    ? ^^string_key_map_by_hash_fast
    : ^^string_key_map_by_hash_slow;
  auto type = substitute(type_template, eq_type, ^^KVPair);
  auto entries = reflect_cpp26::define_static_array(table);
  return {
    .type = string_key_map_meta_result::result_type::as_hash_table,
    .as_hash_table = {
      .type = type,
      .entries = std::meta::reflect_constant(entries),
      .actual_size = kv_pairs.size(),
      .bucket_size_or_mask = bucket_size - !!is_fast,
    }
  };
}

// Precondition: All keys in kv_pairs are lower case
// if options.case_insensitive is true.
template <class KVPair>
consteval auto make_string_key_map_impl(
  std::vector<KVPair> kv_pairs, string_key_map_options options)
  -> string_key_map_meta_result
{
  check_string_key_map_input(kv_pairs, options);
  // (1) Empty or naive
  if (kv_pairs.size() < options.optimization_threshold) {
    return make_naive_string_key_map(kv_pairs, options);
  }
  // (2) Length-based
  auto min_length = size_t{};
  if (key_lengths_are_all_distinct(kv_pairs, &min_length)) {
    return make_length_based_string_key_map(kv_pairs, options);
  }
  // (3) ith-character-based
  if (options.enables_lookup_by_differed_character) {
    for (auto i = 0zU; i < min_length; i++) {
      if (!characters_are_all_different(kv_pairs, i)) { continue; }
      return make_character_based_string_key_map(kv_pairs, i, options);
    }
  }

  auto to_hash_fn = [](const KVPair& p) { return bkdr_hash64(get_first(p)); };
  auto hash_values = kv_pairs
    | std::views::transform(to_hash_fn)
    | std::ranges::to<std::vector>();
  // (4) Hash binary search: slow path due to hash collision
  if (has_hash_collision(hash_values)) {
    return make_hash_search_based_string_key_map(
      kv_pairs, hash_values, true, options);
  }
  auto best_bucket_size =
    find_best_bucket_size(hash_values, options.min_load_factor);
  // (5) Hash-table-based (hash collision is excluded)
  if (best_bucket_size != 0) {
    return make_hash_table_based_string_key_map(
      kv_pairs, hash_values, best_bucket_size, options);
  }
  // (6) Hash linear or binary search: fast path, without hash collision
  return make_hash_search_based_string_key_map(
    kv_pairs, hash_values, false, options);
}

template <std::ranges::input_range KVPairRange>
  requires (string_key_kv_pair<std::ranges::range_value_t<KVPairRange>>)
consteval auto make_string_key_map(
  KVPairRange&& kv_pairs, string_key_map_options options = {})
  -> string_key_map_meta_result
{
  using kv_pair_type = std::ranges::range_value_t<KVPairRange>;
  using kv_pair_key_type = std::tuple_element_t<0, kv_pair_type>;
  using kv_pair_value_type = std::tuple_element_t<1, kv_pair_type>;
  using element_type = typename kv_pair_key_type::value_type; // CharT

  using canonical_kv_pair_type =
    std::pair<meta_basic_string_view<element_type>, kv_pair_value_type>;
  auto converted = std::vector<canonical_kv_pair_type>();
  if (options.case_insensitive) {
    for (const auto& [k, v]: kv_pairs) {
      auto k_lower = reflect_cpp26::define_static_string(ascii_tolower(k));
      converted.emplace_back(k_lower, v);
    }
  } else {
    for (const auto& [k, v]: kv_pairs) {
      converted.emplace_back(reflect_cpp26::define_static_string(k), v);
    }
  }
  return make_string_key_map_impl(converted, options);
}

#define REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(matched_type)               \
  template <string_key_map_meta_result V>                                      \
    requires (V.type == string_key_map_meta_result::result_type::matched_type) \
  constexpr auto extract_string_key_map_v<V>

template <string_key_map_meta_result V>
constexpr auto extract_string_key_map_v = compile_error("Placeholder");

REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(direct) =
  extract<V.direct>();

REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(with_entries) = []() {
  using map_type = [: V.with_entries.type :];
  return map_type{._entries = extract<V.with_entries.entries>()};
}();

REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(with_integral_key) = []() {
  constexpr auto U = V.with_integral_key;
  constexpr auto underlying = extract_integral_key_map_v<U.underlying>;
  using map_type =
    [: substitute(U.type_template, U.eq_type, ^^decltype(underlying)) :];
  return map_type{._underlying = underlying};
}();

REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT(as_hash_table) = []() {
  constexpr auto U = V.as_hash_table;
  using map_type = [: U.type :];
  return map_type{{extract<U.entries>(), U.actual_size}, U.bucket_size_or_mask};
}();

#undef REFLECT_CPP26_STRING_KEY_FIXED_MAP_EXTRACT
} // namespace reflect_cpp26::impl

#define REFLECT_CPP26_STRING_KEY_FIXED_MAP(kv_pairs, ...)                \
  [&]() consteval {                                                      \
    constexpr auto meta_res = reflect_cpp26::impl::make_string_key_map(  \
        kv_pairs, ##__VA_ARGS__);                                        \
    return reflect_cpp26::impl::extract_string_key_map_v<meta_res>;      \
  }()

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_STRING_KEY_HPP
