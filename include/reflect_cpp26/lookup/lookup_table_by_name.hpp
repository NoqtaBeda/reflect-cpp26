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
#include <reflect_cpp26/fixed_map/string_key.hpp>
#include <reflect_cpp26/utils/tags.hpp>

// todo: remove this header after refactoring
#include <reflect_cpp26/utils/expand.hpp>

namespace reflect_cpp26 {
namespace impl::lookup::by_name {
template <class Target, class MemberItems>
consteval auto make_member_table(MemberItems member_items)
{
  return REFLECT_CPP26_STRING_KEY_FIXED_MAP(
    member_items.map([](auto entry) {
      auto cur_key = entry.value.first;
      auto cur_value = &[:entry.value.second:];
      static_assert(std::is_convertible_v<decltype(cur_value), Target>,
        "Inconsistent types.");
      return std::pair{cur_key, static_cast<Target>(cur_value)};
    }));
}

template <class T, class MemberItems>
consteval auto make_type_member_table(MemberItems member_items)
{
  static_assert(member_items.size() > 0, "Member list can not be empty.");
  constexpr auto first_member = get<0>(member_items).second;
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
  using MemberType = [: type_of(get<0>(member_items).second) :];
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

consteval auto decompose_prefix_suffix(std::string_view pattern)
  -> std::pair<std::string_view, std::string_view>
{
  auto star_pos = pattern.find('*');
  if (star_pos == npos) {
    compile_error("Invalid pattern: Expects format 'prefix*suffix'.");
  }
  auto next_star_pos = pattern.find('*', star_pos + 1);
  if (next_star_pos != npos) {
    compile_error("Multiple '*' is disallowed.");
  }
  return {pattern.substr(0, star_pos), pattern.substr(star_pos + 1)};
}

/**
 * FILTER_FN(T, pattern):
 * Filters all the members of type T whose identifier matches given pattern.
 * Pattern is a string of format 'prefix*suffix' where 'prefix' and 'suffix'
 * can be any valid C++ identifier substring and '*' refers to the matched part.
 * Example:
 * struct foo_t {
 *   void dump_add_result() const;             // (1)
 *   void dump_sub_result() const;             // (2)
 *   void dump_bitwise_xor_result() const;     // (3)
 *   void dump_arithmetic_result_impl() const; // (4) expects mismatch
 * };
 * Then the pattern 'dump_*_result' will match (1,2,3) and '*' is matched as
 * 'add', 'sub', 'bitwise_xor' respectively. The generated table will be [
 *   'add': &foo_t::dump_add_result,
 *   'sub': &foo_t::dump_sub_result,
 *   'bitwise_xor': &foo_t::dump_bitwise_xor_result,
 * ].
 * Anonymous or non-addressable members (nested classes, templates, bit-fields,
 * constructors, destructor, deleted functions, etc.) will be ignored.
 */
consteval auto get_type_member_table_entries(
  std::meta::info T,
  std::string_view pattern,
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
{
  auto pt = decompose_prefix_suffix(pattern);
  return get_type_member_table_entries_impl<std::string>(T, ctx.via(T),
    [pt](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(
        pt.first, pt.second, identifier);
    });
}

/**
 * FILTER_FN(ns, pattern):
 * Filters all members of namespace ns whose identifier matches given pattern.
 * Anonymous or non-addressable members (classes, nested namespaces, templates,
 * deleted functions, etc.) will be ignored.
 */
consteval auto get_namespace_member_table_entries(
  std::meta::info ns,
  std::string_view pattern,
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
{
  auto pt = decompose_prefix_suffix(pattern);
  return get_namespace_member_table_entries_impl<std::string>(ns, ctx,
    [pt](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(
        pt.first, pt.second, identifier);
    });
}

/**
 * FILTER_FN(T, tag, pattern)
 * Filters all the members of type T whose:
 * (1) category matches given tag;
 * (2) identifier matches given pattern.
 *
 * filter_tag should be one of (defined in utils/tags.hpp):
 * (1) nonstatic_data_members_only;
 * (2) nonstatic_member_functions_only;
 * (3) static_data_members_only;
 * (4) static_member_functions_only.
 * Anonymous or non-addressable members (nested classes, templates, bit-fields,
 * constructors, destructor, deleted functions, etc.) will be ignored.
 */
template <class_member_filter_tag FilterTag>
consteval auto get_type_member_table_entries(
  std::meta::info T,
  FilterTag filter_tag,
  std::string_view pattern,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  auto pt = decompose_prefix_suffix(pattern);
  return get_type_member_table_entries_impl<std::string>(
    T, ctx.via(T),
    is_functions_only(filter_tag),
    is_static_members_only(filter_tag),
    [pt](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(
        pt.first, pt.second, identifier);
    });
}

/**
 * FILTER_FN(ns, tag, pattern):
 * Filters all members of namespace ns whose:
 * (1) category matches given tag;
 * (2) identifier matches given pattern.
 * Anonymous or non-addressable members (classes, nested namespaces, templates,
 * deleted functions, etc.) will be ignored.
 */
template <non_class_member_filter_tag FilterTag>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns,
  FilterTag filter_tag,
  std::string_view pattern,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  auto pt = decompose_prefix_suffix(pattern);
  return get_namespace_member_table_entries_impl<std::string>(
    ns, ctx, is_functions_only(filter_tag),
    [pt](std::string_view identifier) {
      return rename_member_table_entry_by_pattern(
        pt.first, pt.second, identifier);
    });
}

/**
 * FILTER_FN(T, filter_fn)
 * Filters all the members of type T by filter_fn.
 * Supported call signatures of filter_fn:
 * (1) (std::meta::info member) -> std::optional<std::string>
 *     If member is to be preserved, its rename is returned;
 *     Otherwise, std::nullopt shall be returned.
 * (2) (std::string_view identifier) -> std::optional<std::string>
 *     Similar with above. Members without identifier will be filtered out.
 * Anonymous or non-addressable members will be ignored.
 */
template <filter_function_by_optional<std::string> FilterFn>
consteval auto get_type_member_table_entries(
  std::meta::info T,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<std::string>(
    T, ctx.via(T), filter_fn);
}

/**
 * FILTER_FN(ns, filter_fn)
 * Filters all the members of namespace ns by filter_fn.
 * Supported call signatures of filter_fn see above.
 * Anonymous or non-addressable members will be ignored.
 */
template <filter_function_by_optional<std::string> FilterFn>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<std::string>(
    ns, ctx, filter_fn);
}

/**
 * LOOKUP_TABLE_BY_NAME(T_or_ns, filter_fn)
 * Similar with above. Only members that match given filter flags are preserved.
 */
template <class_member_filter_tag FilterTag,
          filter_function_by_optional<std::string> FilterFn>
consteval auto get_type_member_table_entries(
  std::meta::info T,
  FilterTag filter_tag,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_type_member_table_entries_impl<std::string>(
    T, ctx.via(T),
    is_functions_only(filter_tag),
    is_static_members_only(filter_tag),
    filter_fn);
}

template <non_class_member_filter_tag FilterTag,
          filter_function_by_optional<std::string> FilterFn>
consteval auto get_namespace_member_table_entries(
  std::meta::info ns,
  FilterTag filter_tag,
  const FilterFn& filter_fn,
  std::meta::access_context ctx = std::meta::access_context::current())
{
  return get_namespace_member_table_entries_impl<std::string>(
    ns, ctx, is_functions_only(filter_tag), filter_fn);
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
