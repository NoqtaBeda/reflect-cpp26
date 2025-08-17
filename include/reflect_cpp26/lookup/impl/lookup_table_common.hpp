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

#ifndef REFLECT_CPP26_LOOKUP_IMPL_LOOKUP_TABLE_COMMON_HPP
#define REFLECT_CPP26_LOOKUP_IMPL_LOOKUP_TABLE_COMMON_HPP

#include <reflect_cpp26/lookup/lookup.hpp>
#include <functional>

namespace reflect_cpp26::impl::lookup {
struct type_member_filter_flags {
  bool is_static;
  bool is_function;

  consteval type_member_filter_flags(std::string_view flags)
  {
    if (flags.length() != 2) {
      compile_error("Invalid flags.");
    }
    // 'n': Non-static; 's': Static
    switch (flags[0]) {
      case 's': is_static = true; break;
      case 'n': is_static = false; break;
      default: compile_error("Invalid flags[0].");
    }
    // 'v': Variable; 'f': Function
    switch (flags[1]) {
      case 'f': is_function = true; break;
      case 'v': is_function = false; break;
      default: compile_error("Invalid flags[1].");
    }
  }

  consteval type_member_filter_flags(const char* flags)
    : type_member_filter_flags(std::string_view{flags}) {}
};

struct namespace_member_filter_flags {
  bool is_function;

private:
  consteval void init(char flag)
  {
    switch (flag) {
      case 'f': is_function = true; break;
      case 'v': is_function = false; break;
      default: compile_error("Invalid flag.");
    }
  }

public:
  consteval namespace_member_filter_flags(char flag) {
    init(flag);
  }

  consteval namespace_member_filter_flags(std::string_view flags)
  {
    if (flags.length() != 1) {
      compile_error("Invalid flag.");
    }
    init(flags[0]);
  }

  consteval namespace_member_filter_flags(const char* flags)
    : namespace_member_filter_flags(std::string_view{flags}) {}
};

using direct_members_query_function =
  std::vector<std::meta::info> (*)(std::meta::info, std::meta::access_context);

template <class Func, class T>
concept filter_function_by_optional =
  std::is_invocable_r_v<std::optional<T>, Func, std::meta::info> ||
  std::is_invocable_r_v<std::optional<T>, Func, std::string_view>;

template <class ValueType, class FilterFn>
consteval void lookup_type_members_by_optional_impl(
  std::vector<std::pair<std::meta::info, ValueType>>& dest,
  std::meta::info T, std::meta::access_context ctx,
  direct_members_query_function get_direct_members_fn,
  const FilterFn& filter_fn)
{
  for (auto member: get_direct_members_fn(T, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(member, *res);
    }
  }
  for (auto base: bases_of(T, ctx)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is not supported.");
    }
    // Protected and private base classes are ignored since we can not cast
    // T A::* to T B::* outside B if A is a non-public base class of B.
    if (!is_public(base)) {
      compile_error("Non-public inheritance is disallowed.");
    }
    auto B = type_of(base);
    lookup_type_members_by_optional_impl(
      dest, B, ctx, get_direct_members_fn, filter_fn);
  }
}

template <class ValueType, class FilterFn>
consteval void lookup_type_members_by_optional(
  std::vector<std::pair<std::meta::info, ValueType>>& dest,
  std::meta::info T, std::meta::access_context ctx,
  direct_members_query_function get_direct_members_fn,
  const FilterFn& filter_fn)
{
  if (is_class_type(T)) {
    lookup_type_members_by_optional_impl(
      dest, T, ctx, get_direct_members_fn, filter_fn);
    return;
  }
  if (!is_union_type(T)) {
    compile_error("T must be either of (1) class type; (2) or union type.");
  }
  for (auto member: get_direct_members_fn(T, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(member, *res);
    }
  }
}

template <class ValueType, class FilterFn>
consteval void lookup_namespace_members_by_optional(
  std::vector<std::pair<std::meta::info, ValueType>>& dest, std::meta::info ns,
  std::meta::access_context ctx, const FilterFn& filter_fn)
{
  if (!is_namespace(ns)) {
    compile_error("ns must be namespace.");
  }
  for (auto member: std::meta::members_of(ns, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(member, *res);
    }
  }
}

template <class Ret, class FilterFn>
consteval auto lookup_members_and_transform(
  std::meta::info T_or_ns, std::meta::access_context ctx,
  direct_members_query_function get_direct_members_fn,
  const FilterFn& filter_fn)
{
  auto member_items = std::vector<std::pair<std::meta::info, Ret>>{};
  if (is_namespace(T_or_ns)) {
    lookup_namespace_members_by_optional(member_items, T_or_ns, ctx, filter_fn);
  } else {
    lookup_type_members_by_optional(
      member_items, T_or_ns, ctx, get_direct_members_fn, filter_fn);
  }
  if constexpr (std::is_same_v<std::string, Ret>) {
    auto transform_fn = [](const auto& pair) {
      auto static_rename = reflect_cpp26::define_static_string(pair.second);
      return std::pair{pair.first, static_rename};
    };
    return member_items
      | std::views::transform(transform_fn)
      | std::ranges::to<std::vector>();
  } else {
    return member_items;
  }
}

template <class Ret, class FilterFn>
consteval auto lookup_members_and_transform(
  std::meta::info T_or_ns, std::meta::access_context ctx,
  const FilterFn& filter_fn)
{
  return lookup_members_and_transform<Ret>(
    T_or_ns, ctx, std::meta::members_of, filter_fn);
}

// Precondition: parent_of(member) is class or union type.
consteval bool type_member_function_is_commonly_excluded(std::meta::info member)
{
  return is_template(member)
    || is_constructor(member) || is_destructor(member)
    || is_deleted(member);
}

// Precondition: parent_of(member) is class or union type.
consteval bool type_data_member_is_commonly_excluded(std::meta::info member)
{
  return is_template(member)
    || is_bit_field(member) || is_reference_type(type_of(member));
}

consteval bool is_commonly_excluded_type_member(std::meta::info member) {
  if (is_type(member)) {
    return true;
  }
  return is_function(member)
    ? type_member_function_is_commonly_excluded(member)
    : type_data_member_is_commonly_excluded(member);
}

// Precondition: parent_of(member) is a namespace
consteval bool namespace_function_is_commonly_excluded(std::meta::info member) {
  return is_template(member) || is_deleted(member);
}

// Precondition: parent_of(member) is a namespace.
consteval bool namespace_variable_is_commonly_excluded(std::meta::info member) {
  return is_template(member) || is_reference_type(type_of(member));
}

consteval bool is_commonly_excluded_namespace_member(std::meta::info member) {
  if (is_type(member) || is_namespace(member)) {
    return true;
  }
  return is_function(member)
    ? namespace_function_is_commonly_excluded(member)
    : namespace_variable_is_commonly_excluded(member);
}

template <class Ret, class FilterFn>
consteval auto invoke_filter_fn(
  const FilterFn& filter_fn, std::meta::info member) -> std::optional<Ret>
{
  if constexpr (std::is_invocable_v<FilterFn, std::meta::info>) {
    return filter_fn(member);
  } else {
    static_assert(std::is_invocable_v<FilterFn, std::string_view>);
    if (has_identifier(member)) {
      return filter_fn(std::meta::identifier_of(member));
    }
    return std::nullopt;
  }
}

template <class Ret, class FilterFn>
consteval auto get_type_member_table_entries_impl(
  std::meta::info T, std::meta::access_context ctx, const FilterFn& filter_fn)
{
  return lookup_members_and_transform<Ret>(T, ctx,
    [&filter_fn](std::meta::info member) -> std::optional<Ret> {
      if (is_commonly_excluded_type_member(member)) {
        return std::nullopt;
      }
      return invoke_filter_fn<Ret>(filter_fn, member);
    });
}

template <class Ret, class FilterFn>
consteval auto get_namespace_member_table_entries_impl(
  std::meta::info ns, std::meta::access_context ctx, const FilterFn& filter_fn)
{
  return lookup_members_and_transform<Ret>(ns, ctx,
    [&filter_fn](std::meta::info member) -> std::optional<Ret> {
      if (is_commonly_excluded_namespace_member(member)) {
        return std::nullopt;
      }
      return invoke_filter_fn<Ret>(filter_fn, member);
    });
}

template <class Ret, class FilterFn>
consteval auto get_type_member_table_entries_impl(
  std::meta::info T, std::meta::access_context ctx,
  type_member_filter_flags filter_flags, const FilterFn& filter_fn)
{
  if (filter_flags.is_function) {
    return lookup_members_and_transform<Ret>(T, ctx,
      [filter_flags, &filter_fn](std::meta::info member) -> std::optional<Ret> {
        if (!is_function(member)
            || type_member_function_is_commonly_excluded(member)
            || is_static_member(member) != filter_flags.is_static) {
          return std::nullopt;
        }
        return invoke_filter_fn<Ret>(filter_fn, member);
      });
  } else {
    auto get_direct_members_fn = filter_flags.is_static
      ? (direct_members_query_function) std::meta::static_data_members_of
      : (direct_members_query_function) std::meta::nonstatic_data_members_of;
    return lookup_members_and_transform<Ret>(
      T, ctx, get_direct_members_fn,
      [&filter_fn](std::meta::info member) -> std::optional<Ret> {
        if (type_data_member_is_commonly_excluded(member)) {
          return std::nullopt;
        }
        return invoke_filter_fn<Ret>(filter_fn, member);
      });
  }
}

template <class Ret, class FilterFn>
consteval auto get_namespace_member_table_entries_impl(
  std::meta::info ns, std::meta::access_context ctx,
  namespace_member_filter_flags filter_flags, const FilterFn& filter_fn)
{
  auto test_fn = filter_flags.is_function
    ? std::meta::is_function
    : std::meta::is_variable;
  auto exclude_fn = filter_flags.is_function
    ? namespace_function_is_commonly_excluded
    : namespace_variable_is_commonly_excluded;

  return lookup_members_and_transform<Ret>(ns, ctx,
    [test_fn, exclude_fn, &filter_fn](std::meta::info member) {
      if (!test_fn(member) || exclude_fn(member)) {
        return std::optional<Ret>{};
      }
      return invoke_filter_fn<Ret>(filter_fn, member);
    });
}
} // namespace reflect_cpp26::impl::lookup

#endif // REFLECT_CPP26_LOOKUP_IMPL_LOOKUP_TABLE_COMMON_HPP
