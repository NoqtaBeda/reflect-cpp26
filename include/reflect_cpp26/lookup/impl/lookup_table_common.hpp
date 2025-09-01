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

#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <functional>
#include <optional>

namespace reflect_cpp26::impl::lookup {
using direct_members_query_fn =
  std::vector<std::meta::info> (*)(std::meta::info, std::meta::access_context);

template <class Func, class T>
concept filter_function_by_optional =
  std::is_invocable_r_v<std::optional<T>, Func, std::meta::info> ||
  std::is_invocable_r_v<std::optional<T>, Func, std::string_view>;

template <class T, class Member>
using pointer_to_member_t = Member T::*;

consteval auto to_pointer_type(std::meta::info T, std::meta::info Member)
{
  if (is_class_member(Member) && !is_static_member(Member)) {
    return substitute(^^pointer_to_member_t, {T, type_of(Member)});
  }
  return add_pointer(type_of(Member));
}

template <class ValueType, class FilterFn>
consteval void lookup_type_members_by_optional_impl(
  std::vector<std::pair<ValueType, std::meta::info>>& dest,
  std::meta::info T,
  std::meta::access_context ctx,
  direct_members_query_fn get_direct_members_fn,
  const FilterFn& filter_fn)
{
  for (auto member: get_direct_members_fn(T, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(*res, member);
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
  std::vector<std::pair<ValueType, std::meta::info>>& dest,
  std::meta::info T,
  std::meta::access_context ctx,
  direct_members_query_fn get_direct_members_fn,
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
      dest.emplace_back(*res, member);
    }
  }
}

template <class ValueType, class FilterFn>
consteval void lookup_namespace_members_by_optional(
  std::vector<std::pair<ValueType, std::meta::info>>& dest,
  std::meta::info ns,
  std::meta::access_context ctx,
  const FilterFn& filter_fn)
{
  if (!is_namespace(ns)) {
    compile_error("ns must be namespace.");
  }
  for (auto member: std::meta::members_of(ns, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(*res, member);
    }
  }
}

template <class Ret, class FilterFn>
consteval auto lookup_members_and_transform(
  std::meta::info T_or_ns,
  std::meta::access_context ctx,
  direct_members_query_fn get_direct_members_fn,
  const FilterFn& filter_fn)
{
  auto member_items = std::vector<std::pair<Ret, std::meta::info>>{};
  if (is_namespace(T_or_ns)) {
    lookup_namespace_members_by_optional(member_items, T_or_ns, ctx, filter_fn);
  } else {
    lookup_type_members_by_optional(
      member_items, T_or_ns, ctx, get_direct_members_fn, filter_fn);
  }
  if constexpr (std::is_same_v<std::string, Ret>) {
    auto transform_fn = [](const auto& pair) {
      auto static_rename = std::define_static_string(pair.first);
      return std::pair{static_rename, pair.second};
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
  std::meta::info T_or_ns,
  std::meta::access_context ctx,
  const FilterFn& filter_fn)
{
  return lookup_members_and_transform<Ret>(
    T_or_ns, ctx, std::meta::members_of, filter_fn);
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
      if (!is_addressable_class_member(member)) {
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
      if (!is_addressable_non_class_member(member)) {
        return std::nullopt;
      }
      return invoke_filter_fn<Ret>(filter_fn, member);
    });
}

template <class Ret, class FilterFn>
consteval auto get_type_member_table_entries_impl(
  std::meta::info T,
  std::meta::access_context ctx,
  bool functions_only,
  bool static_only,
  const FilterFn& filter_fn)
{
  if (functions_only) {
    return lookup_members_and_transform<Ret>(T, ctx,
      [static_only, &filter_fn](std::meta::info member) -> std::optional<Ret> {
        if (!is_function(member) ||
            !is_addressable_class_member(member) ||
            is_static_member(member) != static_only) {
          return std::nullopt;
        }
        return invoke_filter_fn<Ret>(filter_fn, member);
      });
  } else {
    auto get_direct_members_fn = static_only
      ? (direct_members_query_fn) std::meta::static_data_members_of
      : (direct_members_query_fn) std::meta::nonstatic_data_members_of;
    return lookup_members_and_transform<Ret>(
      T, ctx, get_direct_members_fn,
      [&filter_fn](std::meta::info member) -> std::optional<Ret> {
        if (!is_addressable_class_member(member)) {
          return std::nullopt;
        }
        return invoke_filter_fn<Ret>(filter_fn, member);
      });
  }
}

template <class Ret, class FilterFn>
consteval auto get_namespace_member_table_entries_impl(
  std::meta::info ns,
  std::meta::access_context ctx,
  bool functions_only,
  const FilterFn& filter_fn)
{
  auto test_fn =
    functions_only ? std::meta::is_function : std::meta::is_variable;
  return lookup_members_and_transform<Ret>(ns, ctx,
    [test_fn, &filter_fn](std::meta::info member) {
      if (!test_fn(member) || !is_addressable_non_class_member(member)) {
        return std::optional<Ret>{};
      }
      return invoke_filter_fn<Ret>(filter_fn, member);
    });
}
} // namespace reflect_cpp26::impl::lookup

#endif // REFLECT_CPP26_LOOKUP_IMPL_LOOKUP_TABLE_COMMON_HPP
