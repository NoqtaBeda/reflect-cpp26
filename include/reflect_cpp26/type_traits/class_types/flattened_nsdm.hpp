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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENED_NSDM_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENED_NSDM_HPP

#include <climits>
#include <reflect_cpp26/type_traits/class_types/ambiguous_inheritance.hpp>
#include <reflect_cpp26/type_traits/class_types/non_public_nsdm.hpp>
#include <reflect_cpp26/type_traits/class_types/virtual_inheritance.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
struct flattened_data_member_info {
  std::meta::info member;
  std::meta::member_offset actual_offset;

  consteval auto type() const -> std::meta::info {
    return type_of(member);
  }

  // Which class current member is defined directly in.
  consteval auto direct_parent() const -> std::meta::info {
    return parent_of(member);
  }

  consteval auto offset_in_parent() const -> std::meta::member_offset {
    return offset_of(member);
  }

  consteval size_t offset_bits_in_parent() const {
    auto offset = offset_in_parent();
    return offset.bytes * CHAR_BIT + offset.bits;
  }

  consteval size_t offset_bytes_in_parent() const {
    auto offset = offset_in_parent();
    if (offset.bits != 0) {
      compile_error("Can not get offset bytes of bit-fields.");
    }
    return offset.bytes;
  }

  consteval size_t actual_offset_bits() const {
    return actual_offset.bytes * CHAR_BIT + actual_offset.bits;
  }

  consteval size_t actual_offset_bytes() const {
    if (actual_offset.bits != 0) {
      compile_error("Can not get offset bytes of bit-fields.");
    }
    return actual_offset.bytes;
  }
};

namespace impl {
consteval bool is_memberwise_partially_flattenable(std::meta::info T);
consteval bool is_memberwise_flattenable(std::meta::info T);
consteval bool is_memberwise_flattenable_aggregate(std::meta::info T);
consteval bool is_flattenable_aggregate(std::meta::info T);

consteval auto walk_all_nsdm(std::meta::info T) -> std::meta::info;
consteval auto walk_public_nsdm(std::meta::info T) -> std::meta::info;

template <class T>
constexpr auto all_flattened_nonstatic_data_members_refl_v = walk_all_nsdm(^^T);

template <class T>
constexpr auto public_flattened_nonstatic_data_members_refl_v = walk_public_nsdm(^^T);
}  // namespace impl

// -------- Partially Flattenable --------

template <class T>
concept partially_flattenable_class =
    class_without_virtual_inheritance<T> && class_without_ambiguous_inheritance<T>;

template <class T>
concept partially_flattenable =
    std::is_scalar_v<T> || std::is_array_v<T> || partially_flattenable_class<T>;

template <class T>
concept memberwise_partially_flattenable =
    impl::is_memberwise_partially_flattenable(remove_cv(^^T));

template <class T>
concept memberwise_partially_flattenable_class =
    partially_flattenable_class<T> && memberwise_partially_flattenable<T>;

// -------- (Fully) Flattenable --------

template <class T>
concept flattenable_class =
    partially_flattenable_class<T> && class_without_non_public_nonstatic_data_members<T>;

template <class T>
concept flattenable = std::is_scalar_v<T> || std::is_array_v<T> || flattenable_class<T>;

template <class T>
concept memberwise_flattenable = impl::is_memberwise_flattenable(remove_cv(^^T));

template <class T>
concept memberwise_flattenable_class = flattenable_class<T> && memberwise_flattenable<T>;

// -------- (Fully) Flattenable Aggregate --------

template <class T>
concept flattenable_aggregate_class =
    flattenable_class<T> && impl::is_flattenable_aggregate(remove_cv(^^T));

template <class T>
concept flattenable_aggregate = std::is_array_v<T> || flattenable_aggregate_class<T>;

template <class T>
concept memberwise_flattenable_aggregate =
    impl::is_memberwise_flattenable_aggregate(remove_cv(^^T));

template <class T>
concept memberwise_flattenable_aggregate_class =
    flattenable_aggregate_class<T> && memberwise_flattenable_aggregate<T>;

// -------- Flattened Member List --------

// clang-format off
// Reference to C-style array
template <partially_flattenable_class T>
constexpr const auto& all_flattened_nonstatic_data_members_v =
    [:impl::all_flattened_nonstatic_data_members_refl_v<std::remove_cv_t<T>>:];

// Reference to C-style array
template <partially_flattenable_class T>
constexpr const auto& public_flattened_nonstatic_data_members_v =
    [:impl::public_flattened_nonstatic_data_members_refl_v<std::remove_cv_t<T>>:];
// clang-format on

namespace impl {
consteval bool is_memberwise_partially_flattenable(std::meta::info T) {
  if (is_scalar_type(T)) {
    return true;
  }
  if (is_array_type(T)) {
    return extract<bool>(^^memberwise_partially_flattenable, remove_all_extents(T));
  }
  if (is_class_type(T)) {
    // 1. T itself should be partially flattenable
    if (!extract<bool>(^^partially_flattenable_class, T)) {
      return false;
    }
    // 2. Each of public base classes should be memberwise partially flattenable
    for (auto base : public_direct_bases_of(T)) {
      if (!extract<bool>(^^memberwise_partially_flattenable_class, type_of(base))) {
        return false;
      }
    }
    // 3. Each public non-static data member should be memberwise partially flattenable
    for (auto member : public_direct_nonstatic_data_members_of(T)) {
      if (!extract<bool>(^^memberwise_partially_flattenable, type_of(member))) {
        return false;
      }
    }
    return true;
  }
  return false;
}

consteval bool is_memberwise_flattenable(std::meta::info T) {
  if (is_scalar_type(T)) {
    return true;
  }
  if (is_array_type(T)) {
    return extract<bool>(^^memberwise_flattenable, remove_all_extents(T));
  }
  if (is_class_type(T)) {
    // 1. T itself should be flattenable
    if (!extract<bool>(^^flattenable_class, T)) {
      return false;
    }
    // 2. Each of base classes should be memberwise flattenable
    for (auto base : all_direct_bases_of(T)) {
      if (!extract<bool>(^^memberwise_flattenable_class, type_of(base))) {
        return false;
      }
    }
    // 3. Each non-static data member should be flattenable
    for (auto member : all_direct_nonstatic_data_members_of(T)) {
      if (!extract<bool>(^^memberwise_flattenable, type_of(member))) {
        return false;
      }
    }
    return true;
  }
  return false;
}

consteval bool is_memberwise_flattenable_aggregate(std::meta::info T) {
  if (is_array_type(T)) {
    auto U = remove_all_extents(T);
    return is_scalar_type(U) || extract<bool>(^^memberwise_flattenable_aggregate, U);
  }
  if (is_class_type(T)) {
    // 1. T itself should be flattenable aggregate
    if (!extract<bool>(^^flattenable_aggregate_class, T)) {
      return false;
    }
    // 2. Each of base classes should be memberwise flattenable aggregate
    for (auto base : all_direct_bases_of(T)) {
      if (!extract<bool>(^^memberwise_flattenable_aggregate_class, type_of(base))) {
        return false;
      }
    }
    // 3. Each non-static data member should be memberwise flattenable
    for (auto member : all_direct_nonstatic_data_members_of(T)) {
      auto M = type_of(member);
      if (!is_scalar_type(M) && !extract<bool>(^^memberwise_flattenable_aggregate, M)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

consteval bool is_memberwise_flattenable(std::meta::info T, std::meta::info target_concept) {
  if (is_scalar_type(T)) {
    return true;
  }
  if (is_array_type(T)) {
    auto U = remove_all_extents(T);
    return extract<bool>(target_concept, U);
  }
  return extract<bool>(target_concept, T);
}

consteval bool is_flattenable_aggregate(std::meta::info T) {
  if (!is_class_type(T)) {
    compile_error("Non-class types are not allowed here.");
  }
  if (!is_aggregate_type(T)) {
    return false;
  }
  for (auto base : all_direct_bases_of(T)) {
    if (!is_public(base) || is_virtual(base)) {
      return false;
    }
    if (!extract<bool>(^^flattenable_aggregate, type_of(base))) {
      return false;
    }
  }
  for (auto member : all_direct_nonstatic_data_members_of(T)) {
    if (!is_public(member)) {
      return false;
    }
  }
  return true;
}

consteval void walk_all_nsdm_dfs(std::vector<flattened_data_member_info>& dest,
                                 std::meta::info T,
                                 ptrdiff_t base_offset_bytes) {
  for (auto base : all_direct_bases_of(T)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    auto next_base_offset_bytes = base_offset_bytes + offset_of(base).bytes;
    walk_all_nsdm_dfs(dest, type_of(base), next_base_offset_bytes);
  }
  for (auto member : all_direct_nonstatic_data_members_of(T)) {
    auto actual_offset = std::meta::member_offset{
        .bytes = offset_of(member).bytes + base_offset_bytes,
        .bits = offset_of(member).bits,
    };
    dest.push_back({.member = member, .actual_offset = actual_offset});
  }
}

consteval void walk_public_nsdm_dfs(std::vector<flattened_data_member_info>& dest,
                                    std::meta::info T,
                                    ptrdiff_t base_offset_bytes) {
  for (auto base : public_direct_bases_of(T)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    auto next_base_offset_bytes = base_offset_bytes + offset_of(base).bytes;
    walk_public_nsdm_dfs(dest, type_of(base), next_base_offset_bytes);
  }
  for (auto member : public_direct_nonstatic_data_members_of(T)) {
    auto actual_offset = std::meta::member_offset{
        .bytes = offset_of(member).bytes + base_offset_bytes,
        .bits = offset_of(member).bits,
    };
    dest.push_back({.member = member, .actual_offset = actual_offset});
  }
}

consteval auto walk_all_nsdm(std::meta::info T) -> std::meta::info {
  auto members = std::vector<flattened_data_member_info>{};
  walk_all_nsdm_dfs(members, T, 0);
  return std::meta::reflect_constant_array(members);
}

consteval auto walk_public_nsdm(std::meta::info T) -> std::meta::info {
  auto members = std::vector<flattened_data_member_info>{};
  walk_public_nsdm_dfs(members, T, 0);
  return std::meta::reflect_constant_array(members);
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENED_NSDM_HPP
