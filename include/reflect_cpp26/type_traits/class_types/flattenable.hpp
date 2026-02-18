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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP

#include <reflect_cpp26/type_traits/class_types/ambiguous_inheritance.hpp>
#include <reflect_cpp26/type_traits/class_types/non_public_nsdm.hpp>
#include <reflect_cpp26/type_traits/class_types/virtual_inheritance.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval bool is_recursively_partially_flattenable(std::meta::info T);
consteval bool is_recursively_flattenable(std::meta::info T);
consteval bool is_recursively_flattenable_aggregate(std::meta::info T);
consteval bool is_flattenable_aggregate(std::meta::info T);
}  // namespace impl

template <class T>
concept partially_flattenable_class =
    class_without_virtual_inheritance<T> && class_without_ambiguous_inheritance<T>;

template <class T>
concept partially_flattenable =
    std::is_scalar_v<T> || std::is_array_v<T> || partially_flattenable_class<T>;

template <class T>
concept flattenable_class =
    partially_flattenable_class<T> && class_without_non_public_nonstatic_data_members<T>;

template <class T>
concept flattenable = std::is_scalar_v<T> || std::is_array_v<T> || flattenable_class<T>;

template <class T>
concept flattenable_aggregate_class =
    flattenable_class<T> && impl::is_flattenable_aggregate(remove_cv(^^T));

template <class T>
concept flattenable_aggregate = std::is_array_v<T> || flattenable_aggregate_class<T>;

template <class T>
concept recursively_partially_flattenable =
    impl::is_recursively_partially_flattenable(remove_cv(^^T));

template <class T>
concept recursively_partially_flattenable_class =
    partially_flattenable_class<T> && recursively_partially_flattenable<T>;

template <class T>
concept recursively_flattenable = impl::is_recursively_flattenable(remove_cv(^^T));

template <class T>
concept recursively_flattenable_class = flattenable_class<T> && recursively_flattenable<T>;

template <class T>
concept recursively_flattenable_aggregate =
    impl::is_recursively_flattenable_aggregate(remove_cv(^^T));

template <class T>
concept recursively_flattenable_aggregate_class =
    flattenable_aggregate_class<T> && recursively_flattenable_aggregate<T>;

namespace impl {
consteval bool is_recursively_partially_flattenable(std::meta::info T) {
  if (is_scalar_type(T)) {
    return true;
  }
  if (is_array_type(T)) {
    return extract_bool(^^recursively_partially_flattenable, remove_all_extents(T));
  }
  if (is_class_type(T)) {
    // 1. T itself should be partially flattenable
    if (!extract_bool(^^partially_flattenable_class, T)) {
      return false;
    }
    // 2. Each of public base classes should be recursively partially flattenable
    for (auto base : public_direct_bases_of(T)) {
      if (!extract_bool(^^recursively_partially_flattenable_class, type_of(base))) {
        return false;
      }
    }
    // 3. Each public non-static data member should be recursively partially flattenable
    for (auto member : public_direct_nonstatic_data_members_of(T)) {
      if (!extract_bool(^^recursively_partially_flattenable, type_of(member))) {
        return false;
      }
    }
    return true;
  }
  return false;
}

consteval bool is_recursively_flattenable(std::meta::info T) {
  if (is_scalar_type(T)) {
    return true;
  }
  if (is_array_type(T)) {
    return extract_bool(^^recursively_flattenable, remove_all_extents(T));
  }
  if (is_class_type(T)) {
    // 1. T itself should be flattenable
    if (!extract_bool(^^flattenable_class, T)) {
      return false;
    }
    // 2. Each of base classes should be recursively flattenable
    for (auto base : all_direct_bases_of(T)) {
      if (!extract_bool(^^recursively_flattenable_class, type_of(base))) {
        return false;
      }
    }
    // 3. Each non-static data member should be flattenable
    for (auto member : all_direct_nonstatic_data_members_of(T)) {
      if (!extract_bool(^^recursively_flattenable, type_of(member))) {
        return false;
      }
    }
    return true;
  }
  return false;
}

consteval bool is_recursively_flattenable_aggregate(std::meta::info T) {
  if (is_array_type(T)) {
    auto U = remove_all_extents(T);
    return is_scalar_type(U) || extract_bool(^^recursively_flattenable_aggregate, U);
  }
  if (is_class_type(T)) {
    // 1. T itself should be flattenable aggregate
    if (!extract_bool(^^flattenable_aggregate_class, T)) {
      return false;
    }
    // 2. Each of base classes should be recursively flattenable aggregate
    for (auto base : all_direct_bases_of(T)) {
      if (!extract_bool(^^recursively_flattenable_aggregate_class, type_of(base))) {
        return false;
      }
    }
    // 3. Each non-static data member should be recursively flattenable
    for (auto member : all_direct_nonstatic_data_members_of(T)) {
      auto M = type_of(member);
      if (!is_scalar_type(M) && !extract_bool(^^recursively_flattenable_aggregate, M)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

consteval bool is_recursively_flattenable(std::meta::info T, std::meta::info target_concept) {
  if (is_scalar_type(T)) {
    return true;
  }
  if (is_array_type(T)) {
    auto U = remove_all_extents(T);
    return extract_bool(target_concept, U);
  }
  return extract_bool(target_concept, T);
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
    if (!extract_bool(^^flattenable_aggregate, type_of(base))) {
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
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP
