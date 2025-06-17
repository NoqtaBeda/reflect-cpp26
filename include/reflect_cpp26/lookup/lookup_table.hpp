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
struct invalid_transform_function_hint_t {};

using direct_members_query_fn = std::vector<std::meta::info> (*)(std::meta::info,
                                                                 std::meta::access_context);

consteval bool is_valid_key_type(std::meta::info T) {
  return extract_bool(^^integral_or_enum_type, T) || extract_bool(^^string_like_of, T, ^^char);
}

consteval bool is_std_optional_instance(std::meta::info T) {
  return has_template_arguments(T) && template_of(T) == ^^std::optional;
}

consteval auto transform_function_invoke_result(std::meta::info Func, bool supports_meta_info_arg)
    -> std::meta::info {
  if (supports_meta_info_arg && is_invocable_type(Func, {^^std::meta::info})) {
    // (1) func(std::meta::info) -> Ret
    // (2) func(std::meta::info) -> std::optional<Ret>
    return invoke_result(Func, {^^std::meta::info});
  } else if (is_invocable_type(Func, {^^std::string_view})) {
    // (3) func(std::string_view) -> Ret
    // (4) func(std::string_view) -> std::optional<Ret>
    return invoke_result(Func, {^^std::string_view});
  } else {
    return ^^invalid_transform_function_hint_t;  // Placeholder
  }
}

consteval auto transform_function_key_type(std::meta::info invoke_result) -> std::meta::info {
  if (is_std_optional_instance(invoke_result)) {
    auto ret = template_arguments_of(invoke_result)[0];
    return is_valid_key_type(ret) ? ret : ^^invalid_transform_function_hint_t;
  }
  return is_valid_key_type(invoke_result) ? invoke_result : ^^invalid_transform_function_hint_t;
}

template <class TransformFn, bool SupportsMetaInfoArg>
struct transform_function_traits {
  static constexpr auto invoke_result_refl =
      transform_function_invoke_result(^^TransformFn, SupportsMetaInfoArg);
  static constexpr auto key_type_refl = transform_function_key_type(invoke_result_refl);

  using invoke_result = [:invoke_result_refl:];
  using key_type = [:key_type_refl:];

  static constexpr bool is_valid = !std::is_same_v<key_type, invalid_transform_function_hint_t>;

  static constexpr bool invoke_result_is_std_optional =
      is_std_optional_instance(invoke_result_refl);
};

template <class TransformFn>
concept transform_function = transform_function_traits<TransformFn, true>::is_valid;

template <class TransformFn>
concept string_transform_function = transform_function_traits<TransformFn, false>::is_valid;

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
    const FilterFn& filter_fn) {
  for (auto member : get_direct_members_fn(T, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      dest.emplace_back(*std::move(res), member);
    }
  }
  for (auto base : bases_of(T, ctx)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is not supported.");
    }
    // Protected and private base classes are ignored since we can not cast
    // T A::* to T B::* outside B if A is a non-public base class of B.
    if (!is_public(base)) {
      compile_error("Non-public inheritance is disallowed.");
    }
    auto B = type_of(base);
    lookup_class_members_by_optional_impl(dest, B, ctx, get_direct_members_fn, filter_fn);
  }
}

template <class KeyType, class FilterFn>
consteval void lookup_class_members_by_optional(
    std::vector<std::pair<KeyType, std::meta::info>>& dest,
    std::meta::info T,
    std::meta::access_context ctx,
    direct_members_query_fn get_direct_members_fn,
    const FilterFn& filter_fn) {
  if (is_class_type(T)) {
    lookup_class_members_by_optional_impl(dest, T, ctx, get_direct_members_fn, filter_fn);
    return;
  }
  if (!is_union_type(T)) {
    compile_error("T must be either of (1) class type; (2) or union type.");
  }
  // Traverses members of union types
  for (auto member : get_direct_members_fn(T, ctx)) {
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
  lookup_class_members_by_optional(member_items, T, ctx, get_direct_members_fn, filter_fn);
  return member_items;
}

template <class Ret, class FilterFn>
consteval auto lookup_namespace_members_and_transform(
    std::meta::info ns,
    std::meta::access_context ctx,
    const FilterFn& filter_fn) /*-> std::vector<std::pair<Ret, std::meta::info>*/
{
  auto member_items = std::vector<std::pair<Ret, std::meta::info>>{};
  for (auto member : std::meta::members_of(ns, ctx)) {
    auto res = filter_fn(member);
    if (res.has_value()) {
      member_items.emplace_back(*std::move(res), member);
    }
  }
  return member_items;
}

template <class Ret, class FilterFn>
consteval auto invoke_filter_fn(const FilterFn& filter_fn, std::meta::info member)
    -> std::optional<Ret> {
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
consteval auto get_class_member_table_entries(std::meta::info T,
                                              std::meta::access_context ctx,
                                              const class_member_lookup_table_options& options,
                                              const FilterFn& filter_fn)
    -> std::vector<std::pair<Ret, std::meta::info>> {
  // Default behavior when the member type is unspecified,
  // which simply checks all addressable members.
  auto default_do_filter_fn = [&filter_fn](std::meta::info member) -> std::optional<Ret> {
    if (!is_addressable_class_member(member)) {
      return std::nullopt;
    }
    return invoke_filter_fn<Ret>(filter_fn, member);
  };
  if (options.category == class_member_category::unspecified) {
    return lookup_class_members_and_transform<Ret>(
        T, ctx, std::meta::members_of, default_do_filter_fn);
  }

  auto functions_only = false;
  auto static_only = false;
  switch (options.category) {
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
    auto do_filter_fn = [static_only, &filter_fn](std::meta::info member) -> std::optional<Ret> {
      if (!is_function(member) || !is_addressable_class_member(member)) {
        return std::nullopt;
      }
      if (is_static_member(member) != static_only) {
        return std::nullopt;
      }
      return invoke_filter_fn<Ret>(filter_fn, member);
    };
    return lookup_class_members_and_transform<Ret>(T, ctx, std::meta::members_of, do_filter_fn);
  } else {
    auto get_direct_members_fn =
        static_only ? static_cast<direct_members_query_fn>(std::meta::static_data_members_of)
                    : static_cast<direct_members_query_fn>(std::meta::nonstatic_data_members_of);
    return lookup_class_members_and_transform<Ret>(
        T, ctx, get_direct_members_fn, default_do_filter_fn);
  }
}

template <class Ret, class FilterFn>
consteval auto get_namespace_member_table_entries(
    std::meta::info ns,
    std::meta::access_context ctx,
    const namespace_member_lookup_table_options& options,
    const FilterFn& filter_fn) -> std::vector<std::pair<Ret, std::meta::info>> {
  // Default behavior when the member type is unspecified,
  // which simply checks all addressable members.
  if (options.category == namespace_member_category::unspecified) {
    return lookup_namespace_members_and_transform<Ret>(
        ns, ctx, [&filter_fn](std::meta::info member) -> std::optional<Ret> {
          if (!is_addressable_non_class_member(member)) {
            return std::nullopt;
          }
          return invoke_filter_fn<Ret>(filter_fn, member);
        });
  }

  bool functions_only = false;
  switch (options.category) {
    case namespace_member_category::variables:
      functions_only = false;
      break;
    case namespace_member_category::functions:
      functions_only = true;
      break;
    default:
      compile_error("Invalid category.");
  }

  auto test_fn = functions_only ? std::meta::is_function : std::meta::is_variable;
  return lookup_namespace_members_and_transform<Ret>(
      ns, ctx, [test_fn, &filter_fn](std::meta::info member) {
        if (!test_fn(member) || !is_addressable_non_class_member(member)) {
          return std::optional<Ret>{};
        }
        return invoke_filter_fn<Ret>(filter_fn, member);
      });
}

// -------- Factories --------

template <class KeyType>
using table_factory_fn_type =
    std::meta::info (*)(const std::vector<std::pair<KeyType, std::meta::info>>& entries,
                        const fixed_map_options_variant& options);

#define REFLECT_CPP26_LOOKUP_TABLE_FACTORY_FN(fn_name)                                    \
  consteval auto fn_name(const std::vector<std::pair<KeyType, std::meta::info>>& entries, \
                         const fixed_map_options_variant& options) -> std::meta::info

// Note: parent_of(MemRefl) may be some base class of T.
// static_cast is necessary since extract<Mem T::*>(MemRefl) fails in current experimental compiler.
template <class T, class Mem, auto MemRefl>
constexpr auto to_pointer_to_member_v = static_cast<Mem T::*>(&[:MemRefl:]);

// Note: For static member functions, extract<T*>(MemRefl) fails in current
// experimental compiler, which is possibly a compiler bug.
template <class T, auto MemRefl>
constexpr auto to_pointer_v = static_cast<T*>(&[:MemRefl:]);

template <class T, class Mem>
constexpr auto to_pointer_to_member(std::meta::info member) {
  auto Refl = std::meta::reflect_constant(member);
  return extract<Mem T::*>(^^to_pointer_to_member_v, ^^T, ^^Mem, Refl);
}

template <class T>
constexpr auto to_pointer(std::meta::info member) {
  return extract<T*>(^^to_pointer_v, ^^T, std::meta::reflect_constant(member));
}

template <class T, class Mem, class KeyType>
REFLECT_CPP26_LOOKUP_TABLE_FACTORY_FN(nonstatic_class_member_table_factory) {
  // (1) Non-static class members
  auto transform_fn = [](const std::pair<KeyType, std::meta::info>& kv_pair) {
    auto mptr = to_pointer_to_member<T, Mem>(kv_pair.second);
    return std::pair<KeyType, Mem T::*>{kv_pair.first, mptr};
  };
  return make_fixed_map(entries | std::views::transform(transform_fn), options);
}

template <class T, class KeyType>
REFLECT_CPP26_LOOKUP_TABLE_FACTORY_FN(static_class_member_table_factory) {
  // (2) Static class members
  auto transform_fn = [](const std::pair<KeyType, std::meta::info>& kv_pair) {
    return std::pair<KeyType, T*>{kv_pair.first, to_pointer<T>(kv_pair.second)};
  };
  return make_fixed_map(entries | std::views::transform(transform_fn), options);
}

template <class F, class KeyType>
REFLECT_CPP26_LOOKUP_TABLE_FACTORY_FN(function_table_factory) {
  // (3) Function namespace members
  auto transform_fn = [](const std::pair<KeyType, std::meta::info>& kv_pair) {
    return std::pair<KeyType, F*>{kv_pair.first, extract<F*>(kv_pair.second)};
  };
  return make_fixed_map(entries | std::views::transform(transform_fn), options);
}

template <class T, class KeyType>
REFLECT_CPP26_LOOKUP_TABLE_FACTORY_FN(variable_table_factory) {
  // (4) Variable namespace members
  auto transform_fn = [](const std::pair<KeyType, std::meta::info>& kv_pair) {
    auto& var_ref = extract<T&>(kv_pair.second);
    return std::pair<KeyType, T*>{kv_pair.first, &var_ref};
  };
  return make_fixed_map(entries | std::views::transform(transform_fn), options);
}

#undef REFLECT_CPP26_LOOKUP_TABLE_FACTORY_FN

// -------- Builders --------

template <class KeyType>
consteval auto build_table(std::meta::info T_or_ns,
                           const std::vector<std::pair<KeyType, std::meta::info>>& entries,
                           const fixed_map_options_variant& options) -> std::meta::info {
  // Creates empty lookup table with void* as placeholder type
  if (entries.empty()) {
    using KVPair = std::pair<KeyType, void*>;
    return make_fixed_map(std::views::empty<KVPair>, options);
  }

  auto n = entries.size();
  auto first_member = entries[0].second;
  auto target_type = remove_reference(type_of(first_member));

  // Checks type consistency
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
      auto factory_fn_refl = ^^static_class_member_table_factory;
      factory_fn = extract<factory_fn_type>(factory_fn_refl, target_type, ^^KeyType);
    } else {
      // (2) Non-static data members or member variables
      factory_fn = extract<factory_fn_type>(
          ^^nonstatic_class_member_table_factory, T_or_ns, target_type, ^^KeyType);
    }
  } else if (is_function(first_member)) {
    // (3) Free or static member functions
    factory_fn = extract<factory_fn_type>(^^function_table_factory, target_type, ^^KeyType);
  } else {
    // (4) Global variables or static data members
    factory_fn = extract<factory_fn_type>(^^variable_table_factory, target_type, ^^KeyType);
  }
  return factory_fn(entries, options);
}

consteval auto rename_key_by_pattern(std::string_view prefix,
                                     std::string_view suffix,
                                     std::string_view name) -> std::optional<std::string_view> {
  if (!name.starts_with(prefix) || !name.ends_with(suffix)) {
    return std::nullopt;
  }
  name.remove_prefix(prefix.length());
  name.remove_suffix(suffix.length());
  return name;
}

consteval auto decompose_prefix_suffix(std::string_view pattern)
    -> std::pair<std::string_view, std::string_view> {
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
}  // namespace impl::lookup

// Overload (1.1) in docs/lookup_table.md
consteval auto make_class_member_lookup_table(
    std::meta::info T,
    std::string_view pattern,
    const class_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info {
  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries = impl::lookup::get_class_member_table_entries<std::string_view>(
      T, ctx.via(T), options, [pt](std::string_view identifier) {
        return impl::lookup::rename_key_by_pattern(pt.first, pt.second, identifier);
      });
  return impl::lookup::build_table(T, entries, options.fixed_map_options);
}

// Overload (1.2) in docs/lookup_table.md
consteval auto make_namespace_member_lookup_table(
    std::meta::info ns,
    std::string_view pattern,
    const namespace_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info {
  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries = impl::lookup::get_namespace_member_table_entries<std::string_view>(
      ns, ctx, options, [pt](std::string_view identifier) {
        return impl::lookup::rename_key_by_pattern(pt.first, pt.second, identifier);
      });
  return impl::lookup::build_table(ns, entries, options.fixed_map_options);
}

// Overload (2.1) in docs/lookup_table.md
template <impl::lookup::transform_function TransformFn>
consteval auto make_class_member_lookup_table(
    std::meta::info T,
    const TransformFn& transform_fn,
    const class_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info {
  using Traits = impl::lookup::transform_function_traits<TransformFn, true>;
  using Ret = typename Traits::key_type;

  auto entries =
      impl::lookup::get_class_member_table_entries<Ret>(T, ctx.via(T), options, transform_fn);
  return impl::lookup::build_table(T, entries, options.fixed_map_options);
}

// Overload (2.2) in docs/lookup_table.md
template <impl::lookup::transform_function TransformFn>
consteval auto make_namespace_member_lookup_table(
    std::meta::info ns,
    const TransformFn& transform_fn,
    const namespace_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info {
  using Traits = impl::lookup::transform_function_traits<TransformFn, true>;
  using Ret = typename Traits::key_type;

  auto entries =
      impl::lookup::get_namespace_member_table_entries<Ret>(ns, ctx, options, transform_fn);
  return impl::lookup::build_table(ns, entries, options.fixed_map_options);
}

// Overload (3.1) in docs/lookup_table.md
template <impl::lookup::string_transform_function TransformFn>
consteval auto make_class_member_lookup_table(
    std::meta::info T,
    std::string_view pattern,
    const TransformFn& transform_fn,
    const class_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info {
  using Traits = impl::lookup::transform_function_traits<TransformFn, false>;
  using Ret = typename Traits::key_type;

  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries = impl::lookup::get_class_member_table_entries<Ret>(
      T, ctx.via(T), options, [&transform_fn, pt](std::string_view identifier) {
        auto opt = impl::lookup::rename_key_by_pattern(pt.first, pt.second, identifier);
        if constexpr (Traits::invoke_result_is_std_optional) {
          return opt.and_then(transform_fn);
        } else {
          return opt.transform(transform_fn);
        }
      });
  return impl::lookup::build_table(T, entries, options.fixed_map_options);
}

// Overload (3.2) in docs/lookup_table.md
template <impl::lookup::string_transform_function TransformFn>
consteval auto make_namespace_member_lookup_table(
    std::meta::info ns,
    std::string_view pattern,
    const TransformFn& transform_fn,
    const namespace_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info {
  using Traits = impl::lookup::transform_function_traits<TransformFn, false>;
  using Ret = typename Traits::key_type;

  auto pt = impl::lookup::decompose_prefix_suffix(pattern);
  auto entries = impl::lookup::get_namespace_member_table_entries<Ret>(
      ns, ctx, options, [&transform_fn, pt](std::string_view identifier) {
        auto opt = impl::lookup::rename_key_by_pattern(pt.first, pt.second, identifier);
        if constexpr (Traits::invoke_result_is_std_optional) {
          return opt.and_then(transform_fn);
        } else {
          return opt.transform(transform_fn);
        }
      });
  return impl::lookup::build_table(ns, entries, options.fixed_map_options);
}

/**
 * Generates a lookup table of specified class members in compile-time.
 * See docs/lookup_table.md for details.
 */
#define REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(T, ...) \
  [:reflect_cpp26::make_class_member_lookup_table(^^T, ##__VA_ARGS__):]

/**
 * Generates a lookup table of specified namespace members in compile-time.
 * See docs/lookup_table.md for details.
 */
#define REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(ns, ...) \
  [:reflect_cpp26::make_namespace_member_lookup_table(^^ns, ##__VA_ARGS__):]
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_LOOKUP_LOOKUP_TABLE_HPP
