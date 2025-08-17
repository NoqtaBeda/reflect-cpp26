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

#ifndef REFLECT_CPP26_ENUM_IMPL_ENUM_MAPS_HPP
#define REFLECT_CPP26_ENUM_IMPL_ENUM_MAPS_HPP

#include <reflect_cpp26/enum/enum_names.hpp>
#include <reflect_cpp26/utils/fixed_map/integral_key.hpp>
#include <reflect_cpp26/utils/fixed_map/string_key.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <utility>

namespace reflect_cpp26::impl {
template <class T>
struct to_int64_or_uint64_traits;

template <std::signed_integral T>
struct to_int64_or_uint64_traits<T> {
  using type = int64_t;
};

template <std::unsigned_integral T>
struct to_int64_or_uint64_traits<T> {
  using type = uint64_t;
};

template <enum_type T>
struct to_int64_or_uint64_traits<T> {
  using underlying_type = std::underlying_type_t<T>;
  using type = typename to_int64_or_uint64_traits<underlying_type>::type;
};

template <class T>
using to_int64_or_uint64_t = typename to_int64_or_uint64_traits<T>::type;

template <class T>
constexpr auto to_int64_or_uint64(T value)
{
  static_assert(sizeof(T) <= sizeof(int64_t),
    "Extended big integer types are not supported.");
  if constexpr (std::is_enum_v<T>) {
    return to_int64_or_uint64(std::to_underlying(value));
  } else if constexpr (std::is_signed_v<T>) {
    return static_cast<int64_t>(value); // Sign bit extension
  } else {
    return static_cast<uint64_t>(value); // Zero extension
  }
}

template <class E>
consteval auto make_enum_name_map_kv_pairs()
{
  using kv_pair_t = std::pair<to_int64_or_uint64_t<E>, meta_string_view>;
  auto res = make_reserved_vector<kv_pair_t>(enum_count_v<E>);

  auto entries = enumerators_of(^^E);
  const auto& names = enum_names_v<E>;
  for (auto i = 0zU, n = enum_count_v<E>; i < n; i++) {
    auto ev = extract<E>(entries[i]);
    auto msv = meta_string_view::from_std_string_view(names[i]);
    res.emplace_back(to_int64_or_uint64(ev), msv);
  }
  std::ranges::stable_sort(res, {}, get_first);
  auto [dup_begin, dup_end] = std::ranges::unique(res, {}, get_first);
  res.erase(dup_begin, dup_end);
  return res;
}

template <class E>
consteval auto make_enum_name_map()
{
  constexpr auto options = integral_key_map_options{
    .already_sorted = true,
    .already_unique = true,
  };
  return REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(
    make_enum_name_map_kv_pairs<E>(), options);
}

template <class E, bool CaseInsensitive>
consteval auto make_enum_from_string_kv_pairs()
{
  using kv_pair_t = std::pair<meta_string_view, to_int64_or_uint64_t<E>>;
  auto res = make_reserved_vector<kv_pair_t>(enum_count_v<E>);

  auto entries = enumerators_of(^^E);
  const auto& names = []() -> const auto& {
    if constexpr (CaseInsensitive) { return enum_ic_names_v<E>; }
    else { return enum_names_v<E>; }
  }();
  for (auto i = 0zU, n = enum_count_v<E>; i < n; i++) {
    auto ev = extract<E>(entries[i]);
    auto msv = meta_string_view::from_std_string_view(names[i]);
    res.emplace_back(msv, to_int64_or_uint64(ev));
  }
  return res;
}

template <class E>
consteval auto make_enum_from_string_map()
{
  return REFLECT_CPP26_STRING_KEY_FIXED_MAP(
    make_enum_from_string_kv_pairs<E, false>());
}

template <class E>
consteval auto make_enum_from_ci_string_map()
{
  constexpr auto options = string_key_map_options{
    .case_insensitive = true,
  };
  return REFLECT_CPP26_STRING_KEY_FIXED_MAP(
    make_enum_from_string_kv_pairs<E, true>(), options);
}

struct enum_indices_t {
  alignas(8)
  uint16_t original = static_cast<uint16_t>(npos);
  uint16_t by_name = static_cast<uint16_t>(npos);
  uint16_t by_value = static_cast<uint16_t>(npos);
  uint16_t by_value_unique = static_cast<uint16_t>(npos);
};

template <enum_entry_order Order>
constexpr auto get(enum_indices_t indices)
{
  if constexpr (Order == enum_entry_order::original) {
    return indices.original;
  } else if constexpr (Order == enum_entry_order::by_name) {
    return indices.by_name;
  } else if constexpr (Order == enum_entry_order::by_value) {
    return indices.by_value;
  } else {
    static_assert(false, "Invalid enum entry order.");
  }
}

template <class E>
consteval auto make_enum_index_map_kv_pairs()
{
  using entry_tuple_t = std::pair<size_t, std::string_view>;
  auto entries = enumerators_of(^^E);
  auto entry_tuples = make_reserved_vector<entry_tuple_t>(entries.size());
  for (auto i = 0zU, n = entries.size(); i < n; i++) {
    entry_tuples.emplace_back(i, std::meta::identifier_of(entries[i]));
  }

  using kv_pair_t = std::pair<to_int64_or_uint64_t<E>, enum_indices_t>;
  auto res = make_reserved_vector<kv_pair_t>(entries.size());
  for (auto i = 0zU, n = entries.size(); i < n; i++) {
    auto cur = entries[i];
    res.emplace_back(
      to_int64_or_uint64(extract<E>(cur)),
      enum_indices_t{.original = static_cast<uint16_t>(i)});
  }
  std::ranges::sort(entry_tuples, {}, get_second);
  for (auto i = 0zU, n = entries.size(); i < n; i++) {
    res[entry_tuples[i].first].second.by_name = i;
  }
  std::ranges::stable_sort(res, {}, get_first);
  for (auto i = 0zU, n = entries.size(); i < n; i++) {
    res[i].second.by_value = i;
  }
  auto [dup_begin, dup_end] = std::ranges::unique(res, {}, get_first);
  res.erase(dup_begin, dup_end);
  for (auto i = 0zU, m = res.size(); i < m; i++) {
    res[i].second.by_value_unique = i;
  }
  return res;
}

constexpr auto enum_count_limit = std::numeric_limits<uint16_t>::max() - 1;

template <class E>
consteval auto make_enum_index_map()
{
  static_assert(enum_count_v<E> <= enum_count_limit, "Enum type too large.");
  constexpr auto options = integral_key_map_options{
    .already_sorted = true,
    .already_unique = true,
  };
  return REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(
    make_enum_index_map_kv_pairs<E>(), options);
}

template <class E>
constexpr auto enum_name_map_v = make_enum_name_map<E>();

template <class E>
constexpr auto enum_from_string_map_v = make_enum_from_string_map<E>();

template <class E>
constexpr auto enum_from_ci_string_map_v = make_enum_from_ci_string_map<E>();

template <class E>
constexpr auto enum_index_map_v = make_enum_index_map<E>();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_ENUM_IMPL_ENUM_MAPS_HPP
