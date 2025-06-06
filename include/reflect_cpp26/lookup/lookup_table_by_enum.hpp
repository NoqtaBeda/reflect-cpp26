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

#ifndef REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_BY_ENUM_HPP
#define REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_BY_ENUM_HPP

#include <reflect_cpp26/enum/enum_cast.hpp>
#include <reflect_cpp26/lookup/impl/lookup_table_common.hpp>
#include <reflect_cpp26/utils/identifier_naming.hpp>

namespace reflect_cpp26 {
template <enum_type E, class Pointer>
struct identifier_lookup_table_by_enum {
  using key_type = E;
  using value_type = Pointer;

  struct entry_t {
    E key;
    Pointer value;
  };
  // Sorted by underlying value of key
  meta_span<entry_t> entries;

  // Returns nullptr if not found
  constexpr auto operator[](E key) const -> Pointer
  {
    auto [head, tail] = entries;
    while (head < tail) {
      auto mid = head + (tail - head) / 2;
      if (key == mid->key) {
        return mid->value;
      }
      (std::to_underlying(key) < std::to_underlying(mid->key))
        ? (tail = mid) : (head = mid + 1);
    }
    return nullptr;
  }

  constexpr auto size() const -> size_t {
    return entries.size();
  }
};

namespace impl::lookup::by_enum {
template <class E, class Target, class MemberItems>
consteval auto make_member_table(MemberItems member_items)
{
  using ResultType = identifier_lookup_table_by_enum<E, Target>;
  using ResultEntryType = typename ResultType::entry_t;

  auto entries = std::vector<ResultEntryType>{};
  entries.reserve(member_items.size());

  member_items.for_each([&entries](auto item) {
    auto cur = &[:item.value.first:];
    static_assert(std::is_convertible_v<decltype(cur), Target>,
      "Inconsistent types.");
    entries.push_back({.key = item.value.second, .value = cur});
  });
  std::ranges::sort(entries, [](const auto& lhs, const auto& rhs) {
    return std::to_underlying(lhs.key) < std::to_underlying(rhs.key);
  });
  // Checks duplicated keys
  auto pos = std::ranges::adjacent_find(entries, {}, &ResultEntryType::key);
  if (entries.end() != pos) {
    compile_error("Duplicated keys detected.");
  }
  return ResultType{.entries = reflect_cpp26::define_static_array(entries)};
}

template <class T, class E, class MemberItems>
consteval auto make_type_member_table(MemberItems member_items)
{
  static_assert(member_items.size() > 0, "Member list can not be empty.");
  constexpr auto first_member = get<0>(member_items).first;
  using MemberType = [: type_of(first_member) :];

  if constexpr (is_static_member(first_member)) {
    using TargetType = MemberType*;
    return make_member_table<E, TargetType>(member_items);
  } else {
    using TargetType = MemberType T::*;
    return make_member_table<E, TargetType>(member_items);
  }
}

template <class E, class MemberItems>
consteval auto make_namespace_member_table(MemberItems member_items)
{
  static_assert(member_items.size() > 0, "Member list can not be empty.");
  using MemberType = [: type_of(get<0>(member_items).first) :];
  using TargetType = MemberType*;
  return make_member_table<E, TargetType>(member_items);
}

template <class E>
consteval auto enum_cast_table_entry_by_pattern(
  std::string_view prefix, std::string_view suffix, std::string_view name,
  identifier_naming_rule enum_naming_rule) -> std::optional<E>
{
  if (name.length() <= prefix.length() + suffix.length()) {
    return std::nullopt;
  }
  if (!name.starts_with(prefix) || !name.ends_with(suffix)) {
    return std::nullopt;
  }
  name.remove_prefix(prefix.length());
  name.remove_suffix(suffix.length());
  return enum_cast<E>(convert_identifier(name, enum_naming_rule));
}

/**
 * LOOKUP_TABLE_BY_ENUM(T_or_ns, E, prefix, suffix) or
 * LOOKUP_TABLE_BY_ENUM(T_or_ns, E, prefix, suffix, enum_naming_rule)
 * Filters all the members of type T or namespace ns whose identifier
 * (if exists) match the pattern prefix + $1 + suffix, and $1 can be casted
 * to an entry of enum type E.
 *
 * You can specify the naming rule of entries in E to perform identifier
 * conversion. For example, for enum class Opcode { BIT_AND, BIT_OR },
 * you can set enum_naming_rule = identifier_naming_rule::all_caps_snake_case
 * to make functions doBitAnd() and doBitOr() match the Opcode entries by
 * identifier conversion "BitAnd" -> "BIT_AND" and "BitOr" -> "BIT_OR".
 */
template <enum_type E>
consteval auto get_type_member_table_entries(
  std::meta::info T, std::string_view prefix, std::string_view suffix,
  identifier_naming_rule enum_naming_rule = identifier_naming_rule::no_change,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<E>(T, ctx.via(T),
    [prefix, suffix, enum_naming_rule](std::string_view identifier) {
      return enum_cast_table_entry_by_pattern<E>(
        prefix, suffix, identifier, enum_naming_rule);
    });
}

template <enum_type E>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns, std::string_view prefix, std::string_view suffix,
  identifier_naming_rule enum_naming_rule = identifier_naming_rule::no_change,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<E>(ns, ctx,
    [prefix, suffix, enum_naming_rule](std::string_view identifier) {
      return enum_cast_table_entry_by_pattern<E>(
        prefix, suffix, identifier, enum_naming_rule);
    });
}

/**
 * LOOKUP_TABLE_BY_ENUM(T_or_ns, E, filter, prefix, suffix) or
 * LOOKUP_TABLE_BY_ENUM(T_or_ns, E, filter, prefix, suffix, enum_naming_rule)
 * Filters all the members of type T or namespace ns whose:
 * (1) member category matches given filter;
 * (2) identifier (if exists) match the pattern prefix + $1 + suffix,
 *     and $1 can be casted to an entry of enum type E.
 * Description of enum_naming_rule see above.
 */
template <enum_type E>
consteval auto get_type_member_table_entries(
  std::meta::info T, type_member_filter_flags filter_flags,
  std::string_view prefix, std::string_view suffix,
  identifier_naming_rule enum_naming_rule = identifier_naming_rule::no_change,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<E>(
    T, ctx.via(T), filter_flags,
    [prefix, suffix, enum_naming_rule](std::string_view identifier) {
      return enum_cast_table_entry_by_pattern<E>(
        prefix, suffix, identifier, enum_naming_rule);
    });
}

template <enum_type E>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns, namespace_member_filter_flags filter_flags,
  std::string_view prefix, std::string_view suffix,
  identifier_naming_rule enum_naming_rule = identifier_naming_rule::no_change,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<E>(
    ns, ctx, filter_flags,
    [prefix, suffix, enum_naming_rule](std::string_view identifier) {
      return enum_cast_table_entry_by_pattern<E>(
        prefix, suffix, identifier, enum_naming_rule);
    });
}

/**
 * LOOKUP_TABLE_BY_ENUM(T_or_ns, E, filter_fn)
 * Filters all the members of type T or namespace ns by filter_fn.
 * Supported call signatures of filter_fn:
 * (1) (std::meta::info member) -> std::optional<E>
 *     If member is to be preserved, its key entry is returned;
 *     Otherwise, std::nullopt shall be returned.
 * (2) (std::string_view identifier) -> std::optional<E>
 *     Similar with above. Members without identifier will be filtered out.
 */
template <enum_type E, filter_function_by_optional<E> FilterFn>
consteval auto get_type_member_table_entries(
  std::meta::info T, const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<E>(T, ctx.via(T), filter_fn);
}

template <enum_type E, filter_function_by_optional<E> FilterFn>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns, const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<E>(ns, ctx, filter_fn);
}

/**
 * LOOKUP_TABLE_BY_ENUM(T_or_ns, E, filter_fn)
 * Similar with above. Only members that match the filter flags are preserved.
 */
template <enum_type E, filter_function_by_optional<E> FilterFn>
consteval auto get_type_member_table_entries(
  std::meta::info T, type_member_filter_flags filter_flags,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<E>(
    T, ctx.via(T), filter_flags, filter_fn);
}

template <enum_type E, filter_function_by_optional<E> FilterFn>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns, namespace_member_filter_flags filter_flags,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<E>(
    ns, ctx, filter_flags, filter_fn);
}
} // namespace impl::lookup::by_enum
} // namespace reflect_cpp26

/**
 * Makes a lookup table of members of class type T (including those inherited
 * from bases of T).
 * See tests/lookup/test_class_lookup_table_by_enum.cpp for examples.
 */
#define REFLECT_CPP26_TYPE_LOOKUP_TABLE_BY_ENUM(T, E, ...)                  \
  [&]() consteval {                                                         \
    static_assert(is_class_type(^^T), "T must be a class type.");           \
    static_assert(is_enum_type(^^E), "E must be enum type.");               \
    constexpr auto member_items = REFLECT_CPP26_EXPAND(                     \
      reflect_cpp26::impl::lookup::by_enum::                                \
        get_type_member_table_entries<E>(^^T, __VA_ARGS__));                \
    return reflect_cpp26::impl::lookup::by_enum::                           \
      make_type_member_table<T, E>(member_items);                           \
  }();

/**
 * Similar with above. Size of dispatch table must be equal to the number of
 * unique entries in enum type E.
 */
#define REFLECT_CPP26_TYPE_LOOKUP_TABLE_BY_ENUM_ALL(T, E, ...)              \
  [&]() consteval {                                                         \
    constexpr auto res = REFLECT_CPP26_TYPE_LOOKUP_TABLE_BY_ENUM(           \
      T, E, __VA_ARGS__);                                                   \
    static_assert(res.size() == reflect_cpp26::enum_unique_count<E>(),      \
      "Expects all unique entries of E to be covered.");                    \
    return res;                                                             \
  }();

/**
 * Makes a lookup table of direct members of namespace ns.
 * Types are nested namespaces are excluded.
 * See tests/lookup/test_namespace_lookup_table_by_enum.cpp for examples.
 */
#define REFLECT_CPP26_NAMESPACE_LOOKUP_TABLE_BY_ENUM(ns, E, ...)            \
  [&]() consteval {                                                         \
    static_assert(is_namespace(^^ns), "ns must be a namespace.");           \
    static_assert(is_enum_type(^^E), "E must be enum type.");               \
    constexpr auto member_items = REFLECT_CPP26_EXPAND(                     \
      reflect_cpp26::impl::lookup::by_enum::                                \
        get_namespace_member_table_entries<E>(^^ns, __VA_ARGS__));          \
    return reflect_cpp26::impl::lookup::by_enum::                           \
      make_namespace_member_table<E>(member_items);                         \
  }();

/**
 * Similar with above. Size of dispatch table must be equal to the number of
 * unique entries in enum type E.
 */
#define REFLECT_CPP26_NAMESPACE_LOOKUP_TABLE_BY_ENUM_ALL(ns, E, ...)        \
  [&]() consteval {                                                         \
    constexpr auto res = REFLECT_CPP26_NAMESPACE_LOOKUP_TABLE_BY_ENUM(      \
      ns, E, __VA_ARGS__);                                                  \
    static_assert(res.size() == reflect_cpp26::enum_unique_count<E>(),      \
      "Expects all unique entries of E to be covered.");                    \
    return res;                                                             \
  }();

#endif // REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_BY_ENUM_HPP
