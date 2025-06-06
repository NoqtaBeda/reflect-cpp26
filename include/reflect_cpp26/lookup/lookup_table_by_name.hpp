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

#ifndef REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_BY_NAME_HPP
#define REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_BY_NAME_HPP

#include <reflect_cpp26/lookup/impl/lookup_table_common.hpp>

namespace reflect_cpp26 {
template <class Pointer>
struct identifier_lookup_table_by_name {
  using value_type = Pointer;

  struct entry_t {
    meta_string_view key;
    Pointer value;
  };
  // Sorted in lexicological order of key
  meta_span<entry_t> entries;

  // Returns nullptr if not found
  constexpr auto operator[](std::string_view key) const -> Pointer
  {
    auto [head, tail] = entries;
    while (head < tail) {
      auto mid = head + (tail - head) / 2;
      if (key == mid->key) {
        return mid->value;
      }
      (key < mid->key) ? (tail = mid) : (head = mid + 1);
    }
    return nullptr;
  }

  constexpr auto size() const -> size_t {
    return entries.size();
  }
};

namespace impl::lookup::by_name {
template <class Target, class MemberItems>
consteval auto make_member_table(MemberItems member_items)
{
  using ResultType = identifier_lookup_table_by_name<Target>;
  using ResultEntryType = typename ResultType::entry_t;

  auto entries = std::vector<ResultEntryType>{};
  entries.reserve(member_items.size());

  member_items.for_each([&entries](auto item) {
    auto cur = &[:item.value.first:];
    static_assert(std::is_convertible_v<decltype(cur), Target>,
      "Inconsistent types.");
    entries.push_back({.key = item.value.second, .value = cur});
  });
  std::ranges::sort(entries, {}, &ResultEntryType::key);
  // Checks duplicated keys
  auto pos = std::ranges::adjacent_find(entries, {}, &ResultEntryType::key);
  if (entries.end() != pos) {
    compile_error("Duplicated keys detected.");
  }
  return ResultType{.entries = reflect_cpp26::define_static_array(entries)};
}

template <class T, class MemberItems>
consteval auto make_type_member_table(MemberItems member_items)
{
  static_assert(member_items.size() > 0, "Member list can not be empty.");
  constexpr auto first_member = get<0>(member_items).first;
  using MemberType = [: type_of(first_member) :];

  if constexpr (is_static_member(first_member)) {
    using TargetType = MemberType*;
    return make_member_table<TargetType>(member_items);
  } else {
    using TargetType = MemberType T::*;
    return make_member_table<TargetType>(member_items);
  }
}

template <class MemberItems>
consteval auto make_namespace_member_table(MemberItems member_items)
{
  static_assert(member_items.size() > 0, "Member list can not be empty.");
  using MemberType = [: type_of(get<0>(member_items).first) :];
  using TargetType = MemberType*;
  return make_member_table<TargetType>(member_items);
}

consteval auto rename_member_table_entry_by_pattern(
  std::string_view prefix, std::string_view suffix, std::string_view name)
  -> std::optional<std::string>
{
  if (!name.starts_with(prefix) || !name.ends_with(suffix)) {
    return std::nullopt;
  }
  name.remove_prefix(prefix.length());
  name.remove_suffix(suffix.length());
  return std::string{name};
}

/**
 * LOOKUP_TABLE_BY_NAME(T_or_ns, prefix, suffix)
 * Filters all the members of type T or namespace ns whose identifier matches
 * given prefix and given suffix.
 */
consteval auto get_type_member_table_entries(
  std::meta::info T, std::string_view prefix, std::string_view suffix,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<std::string>(T, ctx.via(T),
    [prefix, suffix](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(prefix, suffix, identifier);
    });
}

consteval auto get_namespace_member_table_entries(
  std::meta::info ns, std::string_view prefix, std::string_view suffix,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<std::string>(ns, ctx,
    [prefix, suffix](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(prefix, suffix, identifier);
    });
}

/**
 * LOOKUP_TABLE_BY_NAME(T_or_ns, filter, prefix, suffix)
 * Filters all the members of type T or namespace ns whose:
 * (1) member category matches given filter;
 * (2) identifier matches given prefix and given suffix.
 */
consteval auto get_type_member_table_entries(
  std::meta::info T, type_member_filter_flags filter_flags,
  std::string_view prefix, std::string_view suffix,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<std::string>(
    T, ctx.via(T), filter_flags,
    [prefix, suffix](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(prefix, suffix, identifier);
    });
}

consteval auto get_namespace_member_table_entries(
  std::meta::info ns, namespace_member_filter_flags filter_flags,
  std::string_view prefix, std::string_view suffix,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<std::string>(
    ns, ctx, filter_flags,
    [prefix, suffix](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(prefix, suffix, identifier);
    });
}

/**
 * LOOKUP_TABLE_BY_NAME(T_or_ns, filter_fn)
 * Filters all the members of type T or namespace ns by filter_fn.
 * Supported call signatures of filter_fn:
 * (1) (std::meta::info member) -> std::optional<std::string>
 *     If member is to be preserved, its rename is returned;
 *     Otherwise, std::nullopt shall be returned.
 * (2) (std::string_view identifier) -> std::optional<std::string>
 *     Similar with above. Members without identifier will be filtered out.
 */
template <filter_function_by_optional<std::string> FilterFn>
consteval auto get_type_member_table_entries(
  std::meta::info T, const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<std::string>(
    T, ctx.via(T), filter_fn);
}

template <filter_function_by_optional<std::string> FilterFn>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns, const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<std::string>(
    ns, ctx, filter_fn);
}

/**
 * LOOKUP_TABLE_BY_NAME(T_or_ns, filter_fn)
 * Similar with above. Only members that match given filter flags are preserved.
 */
template <filter_function_by_optional<std::string> FilterFn>
consteval auto get_type_member_table_entries(
  std::meta::info T, type_member_filter_flags filter_flags,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<std::string>(
    T, ctx.via(T), filter_flags, filter_fn);
}

template <filter_function_by_optional<std::string> FilterFn>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns, namespace_member_filter_flags filter_flags,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<std::string>(
    ns, ctx, filter_flags, filter_fn);
}
} // namespace impl::lookup::by_name
} // namespace reflect_cpp26

/**
 * Makes a lookup table of members of class type T (including those inherited
 * from bases of T).
 * Supported call signatures see above.
 * See tests/lookup/test_class_lookup_table_by_name.cpp for examples.
 */
#define REFLECT_CPP26_TYPE_LOOKUP_TABLE_BY_NAME(T, ...)                     \
  [&]() consteval {                                                         \
    static_assert(is_class_type(^^T), "T must be a type name.");            \
    constexpr auto member_items = REFLECT_CPP26_EXPAND(                     \
      reflect_cpp26::impl::lookup::by_name::get_type_member_table_entries(  \
        ^^T, __VA_ARGS__));                                                 \
    return reflect_cpp26::impl::lookup::by_name::make_type_member_table<T>( \
      member_items);                                                        \
  }();

/**
 * Makes a lookup table of direct members of namespace ns.
 * Types are nested namespaces are excluded.
 * Supported call signatures see above.
 * See tests/lookup/test_namespace_lookup_table_by_name.cpp for examples.
 */
#define REFLECT_CPP26_NAMESPACE_LOOKUP_TABLE_BY_NAME(ns, ...)               \
  [&]() consteval {                                                         \
    static_assert(is_namespace(^^ns), "ns must be a namespace.");           \
    constexpr auto member_items = REFLECT_CPP26_EXPAND(                     \
      reflect_cpp26::impl::lookup::by_name::                                \
        get_namespace_member_table_entries(^^ns, __VA_ARGS__));             \
    return reflect_cpp26::impl::lookup::by_name::                           \
      make_namespace_member_table(member_items);                            \
  }();

#endif // REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_BY_NAME_HPP
