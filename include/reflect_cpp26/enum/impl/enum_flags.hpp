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

#ifndef REFLECT_CPP26_ENUM_IMPL_ENUM_FLAGS_HPP
#define REFLECT_CPP26_ENUM_IMPL_ENUM_FLAGS_HPP

#include <bit>
#include <reflect_cpp26/enum/enum_names.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26::impl {
template <class T>
struct unsigned_promoted_traits;

template <std::integral T>
struct unsigned_promoted_traits<T> {
  using type = std::conditional_t<sizeof(T) <= sizeof(size_t), size_t, std::make_unsigned_t<T>>;
};

template <enum_type T>
struct unsigned_promoted_traits<T> {
  using type = typename unsigned_promoted_traits<std::underlying_type_t<T>>::type;
};

template <class T>
using unsigned_promoted_t = typename unsigned_promoted_traits<T>::type;

template <class T>
constexpr auto unsigned_promoted(T value) {
  static_assert(sizeof(T) <= sizeof(int64_t), "Extended big integer types are not supported.");
  if constexpr (enum_type<T>) {
    return unsigned_promoted(std::to_underlying(value));
  } else if constexpr (std::signed_integral<T>) {
    auto u = static_cast<std::make_unsigned_t<T>>(value);
    return static_cast<unsigned_promoted_t<T>>(u);
  } else {
    return static_cast<unsigned_promoted_t<T>>(value);
  }
}

struct empty_enum_flags_tag_t {};

template <class U>
struct enum_flags_entry {
  U underlying;
  meta_string_view name;
};

template <class U>
struct regular_enum_flags_units {
  meta_span<enum_flags_entry<U>> units;
  U full_set;
};

template <class U>
struct irregular_enum_flags_units {
  meta_span<enum_flags_entry<U>> units;
  meta_span<size_t> heads;
  meta_span<size_t> subset_indices;
  U full_set;
};

template <class E>
consteval auto make_enum_flags_entries() /* -> std::vector<enum_flags_entry, U> */ {
  using U = unsigned_promoted_t<E>;
  auto res = std::vector<enum_flags_entry<U>>{};

  auto entries = enumerators_of(^^E);
  auto names = reflect_cpp26::enum_names_v<E>;
  auto n = entries.size();
  res.reserve(n);

  for (auto i = 0zU; i < n; i++) {
    auto e = extract<E>(entries[i]);
    auto u = unsigned_promoted(std::to_underlying(e));
    auto name = meta_string_view::from_std_string_view(names[i]);
    res.push_back({.underlying = u, .name = name});
  }
  return res;
}

template <class U>
consteval auto try_decompose_regular_enum_flags_units(std::vector<enum_flags_entry<U>> entries)
    -> std::optional<regular_enum_flags_units<U>> {
  // Entries with less popcount shall be traversed earlier
  std::ranges::stable_sort(
      entries, {}, [](const auto& entry) { return std::popcount(entry.underlying); });

  auto full_set = U{0};
  auto units = std::vector<enum_flags_entry<U>>{};
  for (const auto& e : entries) {
    if (e.underlying == 0) {
      continue;
    }
    // No intersection with all the previous entries
    if ((full_set & e.underlying) == 0) {
      units.push_back(e);
      full_set |= e.underlying;
      continue;
    }
    // Partial intersection: irregular, since diff(full_set, e.underlying) is not atom
    if ((full_set & e.underlying) != e.underlying) {
      return std::nullopt;  // <--- (1)
    }
    auto remaining = e.underlying;
    // Checks whether e.underlying can be fully partitioned to disjoint units
    for (const auto& u : units) {
      if ((remaining & u.underlying) == u.underlying) {
        remaining ^= u.underlying;
        if (remaining == 0) {
          break;
        }
      } else if ((remaining & u.underlying) != 0) {
        return std::nullopt;
      }
    }
    // assert(remaining == 0), otherwise the procedure halts at (1) above.
  }
  // Let s = a | b | c, then "s" has higher priority than "a | b | c" during conversion to string.
  std::ranges::sort(units, greater, &enum_flags_entry<U>::underlying);
  return regular_enum_flags_units<U>{
      .units = reflect_cpp26::define_static_array(units),
      .full_set = full_set,
  };
}

template <class U>
consteval auto decompose_irregular_enum_flags_units(std::vector<enum_flags_entry<U>> entries)
    -> irregular_enum_flags_units<U> {
  // i < j  <==>  entries[i].underlying > entries[j].underlying
  std::ranges::stable_sort(entries, greater, &enum_flags_entry<U>::underlying);
  auto [dup_begin, dup_end] = std::ranges::unique(entries, {}, &enum_flags_entry<U>::underlying);
  entries.erase(dup_begin, dup_end);

  auto n = entries.size();
  auto full_set = U{0};
  auto adjacency_list = std::vector<std::vector<size_t>>(n);
  // Adds an directed edge i -> j if entries[i] is superset of entries[j]
  for (auto i = 0zU; i < n - 1; i++) {
    auto u = entries[i].underlying;
    full_set |= u;
    for (auto j = i + 1; j < n; j++) {
      auto v = entries[j].underlying;
      if ((u & v) == v) {
        adjacency_list[i].push_back(j);
      }
    }
  }
  auto indices = std::vector<size_t>();
  auto heads = std::vector<size_t>(n + 1);
  // Flattens the adjacency list
  for (size_t i = 0zU; i < n; i++) {
    heads[i + 1] = heads[i] + adjacency_list[i].size();
    indices.append_range(adjacency_list[i]);
  }

  return irregular_enum_flags_units<U>{
      .units = reflect_cpp26::define_static_array(entries),
      .heads = reflect_cpp26::define_static_array(heads),
      .subset_indices = reflect_cpp26::define_static_array(indices),
      .full_set = full_set,
  };
}

template <class E>
consteval auto make_enum_flags_decomposer() -> std::meta::info {
  auto entries = make_enum_flags_entries<E>();
  auto is_non_empty = std::ranges::any_of(entries, [](const auto& e) { return e.underlying != 0; });
  if (!is_non_empty) {
    return std::meta::reflect_constant(empty_enum_flags_tag_t{});
  }
  auto as_regular = try_decompose_regular_enum_flags_units(entries);
  if (as_regular) {
    return std::meta::reflect_constant(*as_regular);
  }
  auto as_irregular = decompose_irregular_enum_flags_units(entries);
  return std::meta::reflect_constant(as_irregular);
}

template <class E>
constexpr auto enum_flags_decomposer_v = [:make_enum_flags_decomposer<E>():];

template <class E>
constexpr auto enum_flags_is_empty_v =
    std::is_same_v<std::remove_cvref_t<decltype(enum_flags_decomposer_v<E>)>,
                   empty_enum_flags_tag_t>;

template <class E>
constexpr auto enum_flags_is_regular_v =
    template_instance_of<std::remove_cvref_t<decltype(enum_flags_decomposer_v<E>)>,
                         regular_enum_flags_units>;

template <class E>
constexpr auto enum_flags_is_irregular_v =
    template_instance_of<std::remove_cvref_t<decltype(enum_flags_decomposer_v<E>)>,
                         irregular_enum_flags_units>;

template <class E>
consteval auto enum_flag_names_sum_length() -> size_t {
  if constexpr (enum_flags_is_empty_v<E>) {
    return 0zU;
  } else {
    auto res = 0zU;
    for (const auto& e : enum_flags_decomposer_v<E>.units) {
      res += e.name.length();
    }
    return res;
  }
}

template <class E>
constexpr auto enum_flag_names_sum_length_v = enum_flag_names_sum_length<E>();
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_ENUM_IMPL_ENUM_FLAGS_HPP
