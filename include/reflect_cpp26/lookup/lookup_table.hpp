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

#ifndef REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_HPP
#define REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_HPP

#include <reflect_cpp26/fixed_map/all.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
enum class class_member_category {
  unspecified,
  nonstatic_data_members,
  static_data_members,
  nonstatic_member_functions,
  static_member_functions,
};

enum class namespace_member_category {
  unspecified,
  variables,
  functions,
};

struct class_member_lookup_table_options {
  // Selects which category of class members will be picked.
  class_member_category category = class_member_category::unspecified;
  // Options of the underlying fixed map.
  fixed_map_options_variant fixed_map_options;
};

struct namespace_member_lookup_table_options {
  // Selects which category of namespace members will be picked.
  namespace_member_category category = namespace_member_category::unspecified;
  // Options of the underlying fixed map.
  fixed_map_options_variant fixed_map_options;
};

namespace impl::lookup {
using direct_members_query_fn =
  std::vector<std::meta::info> (*)(std::meta::info, std::meta::access_context);

consteval bool is_valid_key_type(std::meta::info T)
{
  return extract_bool(^^integral_or_enum_type, T) ||
         extract_bool(^^string_like_of, T, ^^char);
}

consteval bool is_std_optional_instance(std::meta::info T) {
  return has_template_arguments(T) && template_of(T) == ^^std::optional;
}

consteval auto filter_function_ret_refl(std::meta::info Func) -> std::meta::info
{
  auto result_opt_type = std::meta::info{};
  if (is_invocable_type(Func, {^^std::meta::info})) {
    result_opt_type = invoke_result(Func, {^^std::meta::info});
  } else if (is_invocable_type(Func, {^^std::string_view})) {
    result_opt_type = invoke_result(Func, {^^std::string_view});
  } else {
    return std::meta::info{}; // Invalid
  }
  // Expects invoke result type to be std::optional<Ret>
  if (!is_std_optional_instance(result_opt_type)) {
    return std::meta::info{}; // Invalid
  }
  // Expects Ret to be integral, enum or string-like types.
  auto ret = template_arguments_of(result_opt_type)[0];
  return is_valid_key_type(ret) ? ret : std::meta::info{};
}

consteval auto transform_function_ret_refl(std::meta::info Func)
  -> std::meta::info
{
  auto result_type = std::meta::info{};
  if (is_invocable_type(Func, {^^std::string_view})) {
    result_type = invoke_result(Func, {^^std::string_view});
  } else {
    return std::meta::info{}; // Invalid
  }
  // Expects invoke result to be either Ret or std::optional<Ret>
  // where Ret is integral, enum or string-like types.
  if (is_std_optional_instance(result_type)) {
    auto ret = template_arguments_of(result_type)[0];
    return is_valid_key_type(ret) ? ret : std::meta::info{};
  }
  return is_valid_key_type(result_type) ? result_type : std::meta::info{};
}

template <class Func>
constexpr auto filter_function_ret_refl_v = filter_function_ret_refl(^^Func);

template <class Func>
constexpr auto transform_function_ret_refl_v =
  transform_function_ret_refl(^^Func);

template <class Func>
concept filter_function = std::meta::info{} != filter_function_ret_refl_v<Func>;

template <class Func>
concept transform_function =
  std::meta::info{} != transform_function_ret_refl_v<Func>;

using hint_handle = void (*)();

template <class T, class U>
consteval void hint_inconsistent_type() {
  compile_error("Inconsistent type.");
}

// -------- Lookup helpers --------

template <class KeyType, class FilterFn>
consteval void lookup_class_members_by_optional_impl(
  std::vector<std::pair<KeyType, std::meta::info>>& dest,
  std::meta::info T,
  std::meta::access_context ctx,
  direct_members_query_fn get_direct_members_fn,
  const FilterFn& filter_fn)
{
  for (auto member: get_direct_members_fn(T, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(*std::move(res), member);
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
    lookup_class_members_by_optional_impl(
      dest, B, ctx, get_direct_members_fn, filter_fn);
  }
}

template <class KeyType, class FilterFn>
consteval void lookup_class_members_by_optional(
  std::vector<std::pair<KeyType, std::meta::info>>& dest,
  std::meta::info T,
  std::meta::access_context ctx,
  direct_members_query_fn get_direct_members_fn,
  const FilterFn& filter_fn)
{
  if (is_class_type(T)) {
    lookup_class_members_by_optional_impl(
      dest, T, ctx, get_direct_members_fn, filter_fn);
    return;
  }
  if (!is_union_type(T)) {
    compile_error("T must be either of (1) class type; (2) or union type.");
  }
  // Traverses members of union types
  for (auto member: get_direct_members_fn(T, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(*res, member);
    }
  }
}

template <class Ret, class FilterFn>
consteval auto lookup_class_members_and_transform(
  std::meta::info T,
  std::meta::access_context ctx,
  direct_members_query_fn get_direct_members_fn,
  const FilterFn& filter_fn) /*-> std::vector<std::pair<Ret, std::meta::info>*/
{
  auto member_items = std::vector<std::pair<Ret, std::meta::info>>{};
  lookup_class_members_by_optional(
    member_items, T, ctx, get_direct_members_fn, filter_fn);
  return member_items;
}

template <class Ret, class FilterFn>
consteval auto lookup_namespace_members_and_transform(
  std::meta::info ns,
  std::meta::access_context ctx,
  const FilterFn& filter_fn) /*-> std::vector<std::pair<Ret, std::meta::info>*/
{
  auto member_items = std::vector<std::pair<Ret, std::meta::info>>{};
  for (auto member: std::meta::members_of(ns, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      member_items.emplace_back(*std::move(res), member);
    }
  }
  return member_items;
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
consteval auto get_class_member_table_entries(
  std::meta::info T,
  std::meta::access_context ctx,
  const class_member_lookup_table_options& options,
  const FilterFn& filter_fn) /*-> std::vector<std::pair<Ret, std::meta::info>*/
{
  auto functions_only = false;
  auto static_only = false;

  switch (options.category) {
    case class_member_category::unspecified:
      return lookup_class_members_and_transform<Ret>(
        T, ctx, std::meta::members_of,
        [&filter_fn](std::meta::info member) -> std::optional<Ret> {
          if (!is_addressable_class_member(member)) {
            return std::nullopt;
          }
          return invoke_filter_fn<Ret>(filter_fn, member);
        });
    case class_member_category::nonstatic_data_members:
      break;
    case class_member_category::static_data_members:
      static_only = true;
      break;
    case class_member_category::nonstatic_member_functions:
      functions_only = true;
      break;
    case class_member_category::static_member_functions:
      static_only = true;
      functions_only = true;
      break;
    default:
      compile_error("Invalid category.");
  }

  if (functions_only) {
    return lookup_class_members_and_transform<Ret>(
      T, ctx, std::meta::members_of,
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
    return lookup_class_members_and_transform<Ret>(
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
consteval auto get_namespace_member_table_entries(
  std::meta::info ns,
  std::meta::access_context ctx,
  const namespace_member_lookup_table_options& options,
  const FilterFn& filter_fn) /*-> std::vector<std::pair<Ret, std::meta::info>*/
{
  bool functions_only = false;
  switch (options.category) {
    case namespace_member_category::unspecified:
      return lookup_namespace_members_and_transform<Ret>(ns, ctx,
        [&filter_fn](std::meta::info member) -> std::optional<Ret> {
          if (!is_addressable_non_class_member(member)) {
            return std::nullopt;
          }
          return invoke_filter_fn<Ret>(filter_fn, member);
        });
    case namespace_member_category::variables:
      break;
    case namespace_member_category::functions:
      functions_only = true;
      break;
    default:
      compile_error("Invalid category.");
  }

  auto test_fn =
    functions_only ? std::meta::is_function : std::meta::is_variable;
  return lookup_namespace_members_and_transform<Ret>(ns, ctx,
    [test_fn, &filter_fn](std::meta::info member) {
      if (!test_fn(member) || !is_addressable_non_class_member(member)) {
        return std::optional<Ret>{};
      }
      return invoke_filter_fn<Ret>(filter_fn, member);
    });
}

// -------- Factories --------

template <class KeyType>
using table_factory_fn_type = std::meta::info (*)(
  const std::vector<std::pair<KeyType, std::meta::info>>&,
  const fixed_map_options_variant&);

// Note: Member reflected by MemRefl may be some base class of T.
// static_cast is necessary since extract<Mem T::*>(MemRefl) fails in current
// experimental compiler.
template <class T, class Mem, auto MemRefl>
constexpr auto to_pointer_to_member_v = static_cast<Mem T::*>(&[: MemRefl :]);

// Note: For static member functions, extract<T*>(MemRefl) fails in current
// experimental compiler, which is possibly a compiler bug.
template <class T, auto MemRefl>
constexpr auto to_pointer_v = static_cast<T*>(&[: MemRefl :]);

template <class T, class Mem>
constexpr auto to_pointer_to_member(std::meta::info member)
{
  auto Refl = std::meta::reflect_constant(member);
  return extract<Mem T::*>(^^to_pointer_to_member_v, ^^T, ^^Mem, Refl);
}

template <class T>
constexpr auto to_pointer(std::meta::info member)
{
  auto Refl = std::meta::reflect_constant(member);
  return extract<T*>(^^to_pointer_v, ^^T, Refl);
}

// (1) Non-static class members
template <class T, class Mem, class KeyType>
consteval auto nonstatic_class_member_table_factory(
  const std::vector<std::pair<KeyType, std::meta::info>>& entries,
  const fixed_map_options_variant& options)
  -> std::meta::info
{
  using InputKVPair = std::pair<KeyType, std::meta::info>;
  using DestKVPair = std::pair<KeyType, Mem T::*>;
  auto transform_fn = [](const InputKVPair& kv_pair) {
    auto mptr = to_pointer_to_member<T, Mem>(kv_pair.second);
    return DestKVPair{kv_pair.first, mptr};
  };
  return make_fixed_map(
    entries | std::views::transform(transform_fn), options);
}

// (2) Static class members
//     Note: this sub-factory is required by current experimental compiler.
//     May be redundant in official C++26 Reflection support.
template <class T, class KeyType>
consteval auto static_class_member_table_factory(
  const std::vector<std::pair<KeyType, std::meta::info>>& entries,
  const fixed_map_options_variant& options)
  -> std::meta::info
{
  using InputKVPair = std::pair<KeyType, std::meta::info>;
  using DestKVPair = std::pair<KeyType, T*>;
  auto transform_fn = [](const InputKVPair& kv_pair) {
    return DestKVPair{kv_pair.first, to_pointer<T>(kv_pair.second)};
  };
  return make_fixed_map(
    entries | std::views::transform(transform_fn), options);
}

// (3) Function namespace members
template <class F, class KeyType>
consteval auto function_table_factory(
  const std::vector<std::pair<KeyType, std::meta::info>>& entries,
  const fixed_map_options_variant& options)
  -> std::meta::info
{
  using InputKVPair = std::pair<KeyType, std::meta::info>;
  using DestKVPair = std::pair<KeyType, F*>;
  auto transform_fn = [](const InputKVPair& kv_pair) {
    return DestKVPair{kv_pair.first, extract<F*>(kv_pair.second)};
  };
  return make_fixed_map(
    entries | std::views::transform(transform_fn), options);
}

// (4) Variable namespace members
template <class T, class KeyType>
consteval auto variable_table_factory(
  const std::vector<std::pair<KeyType, std::meta::info>>& entries,
  const fixed_map_options_variant& options)
  -> std::meta::info
{
  using InputKVPair = std::pair<KeyType, std::meta::info>;
  using DestKVPair = std::pair<KeyType, T*>;
  auto transform_fn = [](const InputKVPair& kv_pair) {
    auto& var_ref = extract<T&>(kv_pair.second);
    return DestKVPair{kv_pair.first, &var_ref};
  };
  return make_fixed_map(
    entries | std::views::transform(transform_fn), options);
}

// -------- Builders --------

template <class KeyType>
consteval auto build_table(
  std::meta::info T_or_ns,
  const std::vector<std::pair<KeyType, std::meta::info>>& entries,
  const fixed_map_options_variant& options)
  -> std::meta::info
{
  if (entries.empty()) {
    using KVPair = std::pair<KeyType, void*>; // void* as placeholder
    return make_fixed_map(std::views::empty<KVPair>, options);
  }

  auto n = entries.size();
  auto first_member = entries[0].second;
  auto target_type = remove_reference(type_of(first_member));
  for (auto i = 1zU; i < n; i++) {
    auto cur_type = remove_reference(type_of(entries[i].second));
    if (!is_same_type(cur_type, target_type)) {
      extract<hint_handle>(^^hint_inconsistent_type, target_type, cur_type)();
    }
  }

  using factory_fn_type = table_factory_fn_type<KeyType>;
  factory_fn_type factory_fn = nullptr;
  if (is_class_member(first_member)) {
    // (1) Static data members or member variables
    if (is_static_member(first_member)) {
      factory_fn = extract<factory_fn_type>(
        ^^static_class_member_table_factory, target_type, ^^KeyType);
    } else {
      // (2) Non-static data members or member variables
      factory_fn = extract<factory_fn_type>(
        ^^nonstatic_class_member_table_factory,
        T_or_ns, target_type, ^^KeyType);
    }
  } else if (is_function(first_member)) {
    // (3) Free or static member functions
    factory_fn = extract<factory_fn_type>(
      ^^function_table_factory, target_type, ^^KeyType);
  } else {
    // (4) Global variables or static data members
    factory_fn = extract<factory_fn_type>(
      ^^variable_table_factory, target_type, ^^KeyType);
  }
  return factory_fn(entries, options);
}

consteval auto rename_member_table_entry_by_pattern(
  std::string_view prefix, std::string_view suffix, std::string_view name)
  -> std::optional<std::string_view>
{
  if (!name.starts_with(prefix) || !name.ends_with(suffix)) {
    return std::nullopt;
  }
  name.remove_prefix(prefix.length());
  name.remove_suffix(suffix.length());
  return name;
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
} // namespace impl::lookup

/**
 * Generates a lookup table of specified class members in compile-time.
 * Result V is the reflected constant of the lookup table generated, which can
 * be extracted via [: V :] or reflect_cpp26::extract<V>().
 *
 * This function will traverse all the accessible members (direct or inherited)
 * of class T via given access context and test each member whether it satisfies
 * all the conditions below:
 * (1) It is addressable, i.e. the following members will be ignored:
 *     * Constructor and destructor;
 *     * Template member functions and static data members;
 *     * Deleted member functions;
 *     * Nested classes and using declarations;
 *     * Non-static reference and bit-field members.
 * (2) It has identifier which matches given pattern. Pattern is a string of
 *     format "prefix*suffix" where '*' refers to the matched part. For example,
 *     pattern "get*" will match "getName", "getUserList", etc and pattern
 *     "is_*_member" matches "is_static_data_member", "is_valid_member", etc.
 * (3) It matches given options.
 *
 * Requirements:
 * (1) Pattern must contain exactly one matcher symbol '*'.
 *
 * Example:
 * struct Foo {
 *   int firstValue;
 *   int secondValue;
 *   // Filtered out due to pattern mismatch
 *   int valuesCount;
 *   // Filtered out due to category mismatch (specified in options)
 *   bool hasPositiveFirstValue() const;
 *   bool hasPositiveSecondValue() const;
 * private:
 *   // Filtered out due to inaccessibility
 *   int thirdValue;
 * };
 * constexpr auto table = REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(
 *   Foo, "*Value", {
 *     .category = class_member_category::nonstatic_data_members,
 *   });
 * auto mp1 = table["first"];    // &Foo::firstValue
 * auto mp2 = table["second"];   // &Foo::secondValue
 * auto mpNull = table["third"]; // nullptr
 */
consteval auto make_class_member_lookup_table(
  std::meta::info T,
  std::string_view pattern,
  const class_member_lookup_table_options& options = {},
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
  -> std::meta::info
{
  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries = impl::lookup::get_class_member_table_entries<std::string_view>(
    T, ctx.via(T), options,
    [pt](std::string_view identifier) {
      return impl::lookup::rename_member_table_entry_by_pattern(
        pt.first, pt.second, identifier);
    });
  return impl::lookup::build_table(T, entries, options.fixed_map_options);
}

/**
 * Generates a lookup table of specified namespace members in compile-time.
 * Result V is the reflected constant of the lookup table generated, which can
 * be extracted via [: V :] or reflect_cpp26::extract<V>().
 *
 * This function will traverse all the accessible members of namespace ns via
 * given access context and test each member whether it satisfies all the
 * conditions below:
 * (1) It is addressable, i.e. the following members will be ignored:
 *     * Templates;
 *     * Nested namespaces;
 *     * Class declarations or definitions;
 *     * Using declarations;
 *     * Deleted functions.
 * (2) It has identifier which matches given pattern (details see above);
 * (3) It matches given options.
 *
 * Requirements:
 * (1) Pattern must contain exactly one matcher symbol '*'.
 */
consteval auto make_namespace_member_lookup_table(
  std::meta::info ns,
  std::string_view pattern,
  const namespace_member_lookup_table_options& options = {},
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
  -> std::meta::info
{
  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries =
    impl::lookup::get_namespace_member_table_entries<std::string_view>(
      ns, ctx, options,
      [pt](std::string_view identifier) {
        return impl::lookup::rename_member_table_entry_by_pattern(
          pt.first, pt.second, identifier);
      });
  return impl::lookup::build_table(ns, entries, options.fixed_map_options);
}

/**
 * Generates a lookup table of specified class members in compile-time.
 * Result V is the reflected constant of the lookup table generated, which can
 * be extracted via [: V :] or reflect_cpp26::extract<V>().
 *
 * This function will traverse all the accessible members (direct or inherited)
 * of class T via given access context and test each member whether it satisfies
 * all the conditions below:
 * (1) It is addressable (details see above);
 * (2) It passes given filter function (details see below);
 * (3) It matches given options.
 *
 * Supported call signatures of filter_fn:
 * (1) (std::string_view identifier) -> std::optional<Ret>
 *     Ret is either of integral type, enum type, or string-like type whose
 *     character type is exactly char.
 *     For each traversed member with identifier, let
 *     R = filter_fn(identifier_of(member)). If R == std::nullopt, then the
 *     member is excluded. Otherwise, *R is the key for current member in the
 *     generated lookup table.
 *     Non-addressable or anonymous members will be excluded before invoking
 *     filter_fn.
 * (2) (std::meta::info member) -> std::optional<Ret>
 *     Expected type of Ret is same as above.
 *     For each traversed member, let R = filter_fn(member).
 *     If R == std::nullopt, then the member is discarded. Otherwise, *R is the
 *     key for current member in the generated lookup table.
 *     Non-addressable members will be excluded before invoking filter_fn.
 */
template <impl::lookup::filter_function FilterFn>
consteval auto make_class_member_lookup_table(
  std::meta::info T,
  const FilterFn& filter_fn,
  const class_member_lookup_table_options& options = {},
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
  -> std::meta::info
{
  using Ret = [: impl::lookup::filter_function_ret_refl_v<FilterFn> :];
  auto entries = impl::lookup::get_class_member_table_entries<Ret>(
    T, ctx.via(T), options, filter_fn);
  return impl::lookup::build_table(T, entries, options.fixed_map_options);
}

/**
 * Generates a lookup table of specified namespace members in compile-time.
 * Result V is the reflected constant of the lookup table generated, which can
 * be extracted via [: V :] or reflect_cpp26::extract<V>().
 *
 * This function will traverse all the accessible members of namespace ns via
 * given access context and test each member whether it satisfies all the
 * conditions below:
 * (1) It is addressable (details see above);
 * (2) It passes given filter function (details see above);
 * (3) It matches given options.
 */
template <impl::lookup::filter_function FilterFn>
consteval auto make_namespace_member_lookup_table(
  std::meta::info ns,
  const FilterFn& filter_fn,
  const namespace_member_lookup_table_options& options = {},
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
  -> std::meta::info
{
  using Ret = [: impl::lookup::filter_function_ret_refl_v<FilterFn> :];
  auto entries = impl::lookup::get_namespace_member_table_entries<Ret>(
    ns, ctx, options, filter_fn);
  return impl::lookup::build_table(ns, entries, options.fixed_map_options);
}

/**
 * Generates a lookup table of specified class members in compile-time.
 * Result V is the reflected constant of the lookup table generated, which can
 * be extracted via [: V :] or reflect_cpp26::extract<V>().
 *
 * This function will traverse all the accessible members (direct or inherited)
 * of class T via given access context and test each member whether it satisfies
 * all the conditions below:
 * (1) It is addressable (details see above);
 * (2) It has identifier which matches given pattern (details see above);
 * (3) Let p = the matched part of its identifier,
 *     transform_fn(p) != std::nullopt;
 * (4) It matches given options.
 *
 * Supported signatures of transform_fn:
 * (std::string_view p) -> Ret of std::optional<Ret>
 * where Ret is either of integral type, enum type, or string-like type whose
 * character type is exactly char.
 * If the result type is std::optional<Ret> and transform_fn(p) == std::nullopt,
 * then current member is discarded. Otherwise, transform_fn(p) (or
 * *transform_fn(p) if the result type is std::optional) is the key for current
 * member in the generated lookup table.
 * Non-addressable, anonymous or name-unmatched members will be excluded before
 * invoking transform_fn.
 *
 * Requirements:
 * (1) Pattern must contain exactly one matcher symbol '*'.
 */
template <impl::lookup::transform_function TransformFn>
consteval auto make_class_member_lookup_table(
  std::meta::info T,
  std::string_view pattern,
  const TransformFn& transform_fn,
  const class_member_lookup_table_options& options = {},
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
  -> std::meta::info
{
  using Ret = [: impl::lookup::transform_function_ret_refl_v<TransformFn> :];

  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries = impl::lookup::get_class_member_table_entries<Ret>(
    T, ctx.via(T), options,
    [&transform_fn, pt](std::string_view identifier) {
      auto opt = impl::lookup::rename_member_table_entry_by_pattern(
        pt.first, pt.second, identifier);

      constexpr auto returns_optional = template_instance_of<
        std::invoke_result_t<TransformFn, std::string_view>, std::optional>;
      if constexpr (returns_optional) {
        return opt.and_then(transform_fn);
      } else {
        return opt.transform(transform_fn);
      }
    });
  return impl::lookup::build_table(T, entries, options.fixed_map_options);
}

/**
 * Generates a lookup table of specified namespace members in compile-time.
 * Result V is the reflected constant of the lookup table generated, which can
 * be extracted via [: V :] or reflect_cpp26::extract<V>().
 *
 * This function will traverse all the accessible members of namespace ns via
 * given access context and test each member whether it satisfies all the
 * conditions below:
 * (1) It is addressable (details see above);
 * (2) It has identifier which matches given pattern (details see above);
 * (3) Let p = the matched part of its identifier,
 *     transform_fn(p) != std::nullopt (details see above);
 * (4) It matches given options.
 *
 * Requirements:
 * (1) Pattern must contain exactly one matcher symbol '*'.
 */
template <impl::lookup::transform_function TransformFn>
consteval auto make_namespace_member_lookup_table(
  std::meta::info ns,
  std::string_view pattern,
  const TransformFn& transform_fn,
  const namespace_member_lookup_table_options& options = {},
  std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT)
  -> std::meta::info
{
  using Ret = [: impl::lookup::transform_function_ret_refl_v<TransformFn> :];

  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries = impl::lookup::get_namespace_member_table_entries<Ret>(
    ns, ctx, options,
    [&transform_fn, pt](std::string_view identifier) {
      auto opt = impl::lookup::rename_member_table_entry_by_pattern(
        pt.first, pt.second, identifier);

      constexpr auto returns_optional = template_instance_of<
        std::invoke_result_t<TransformFn, std::string_view>, std::optional>;
      if constexpr (returns_optional) {
        return opt.and_then(transform_fn);
      } else {
        return opt.transform(transform_fn);
      }
    });
  return impl::lookup::build_table(ns, entries, options.fixed_map_options);
}

/**
 * Generates a lookup table of specified class members in compile-time and
 * extracts it immediately.
 * Details see above.
 */
#define REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(T, ...) \
  [: reflect_cpp26::make_class_member_lookup_table(^^T, ##__VA_ARGS__) :]

/**
 * Generates a lookup table of specified namespace members in compile-time and
 * extracts it immediately.
 * Details see above.
 */
#define REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(ns, ...) \
  [: reflect_cpp26::make_namespace_member_lookup_table(^^ns, ##__VA_ARGS__) :]
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_HPP
