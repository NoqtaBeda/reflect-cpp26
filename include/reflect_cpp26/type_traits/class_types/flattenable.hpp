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

#include <reflect_cpp26/type_traits/class_types/has_ambiguous_inheritance.hpp>
#include <reflect_cpp26/type_traits/class_types/has_non_public_nsdm.hpp>
#include <reflect_cpp26/type_traits/class_types/has_virtual_inheritance.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval bool is_flattenable_aggregate(std::meta::info T);
}  // namespace impl

/**
 * Whether T is a partially flattenable class type, i.e. T is a non-union class
 * type that satisfies all the constraints below:
 *   (1) No virtual base classes;
 *   (2) No non-empty ambiguous base;
 *   (3) Every direct base class is partially flattenable, recursively.
 *
 * Note: identifier duplication is allowed. Example:
 *   struct A { int x; int y; };
 *   struct B : A { int* x; int* y; };
 *   static_assert(is_partially_flattenable_class_v<B>); // PASS
 *
 *   constexpr auto specs = public_flattened_nonstatic_data_members_v<B>;
 *   auto b = B{};
 *   auto some_int = 42;
 *   b.[: get<0>(specs).member :] = 42;         // A::x
 *   b.[: get<2>(specs).member :] = &some_int;  // B::x
 */
template <class T>
concept partially_flattenable_class =
    std::is_class_v<T> && !has_virtual_inheritance_v<T> && !has_ambiguous_inheritance_v<T>;

/**
 * Whether T is a partially flattenable type.
 * Type T is partially flattenable if either condition below is satisfied:
 * 1. T is a scalar type, including:
 *   (1) arithmetic types;
 *   (2) enum types;
 *   (3) pointers (including std::nullptr_t and function pointers);
 *   (4) pointers to member (variable or function).
 * 2. T is an array type (bounded U[N] or unbounded U[]);
 * 3. T is a partially flattenable class type (see above).
 */
template <class T>
concept partially_flattenable =
    std::is_scalar_v<T> || std::is_array_v<T> || partially_flattenable_class<T>;

/**
 * Whether T is a flattenable class type, i.e. partially flattenable class type
 * that satisfies all the additional constraints below:
 *   (1) No private or protected direct non-static data members;
 *   (2) No private or protected direct base classes that are not empty;
 *   (3) Every direct base class is a flattenable type, recursively.
 */
template <class T>
concept flattenable_class =
    partially_flattenable_class<T> && !has_non_public_nonstatic_data_members_v<T>;

/**
 * Whether T is a flattenable type.
 * Type T is flattenable if either condition below is satisfied:
 * 1. T is a scalar type;
 * 2. T is an array type (bounded U[N] or unbounded U[]);
 * 3. T is a flattenable class type (see above).
 */
template <class T>
concept flattenable = std::is_scalar_v<T> || std::is_array_v<T> || flattenable_class<T>;

/**
 * Whether T is a flattenable aggregate type, i.e. flattenable class type that
 * satisfies all the additional constraints below:
 *   (1) T is an aggregate;
 *   (2) Every direct base class is a flattenable aggregate, recursively.
 */
template <class T>
concept flattenable_aggregate_class =
    flattenable_class<T> && impl::is_flattenable_aggregate(remove_cv(^^T));

/**
 * Whether T is a flattenable aggregate type.
 * Type T is a flattenable aggregate if either condition below is satisfied:
 * 1. T is an array type (bounded U[N] or unbounded U[]);
 * 2. T is a flattenable aggregate class type (see above).
 */
template <class T>
concept flattenable_aggregate = std::is_array_v<T> || flattenable_aggregate_class<T>;

namespace impl {
consteval bool is_flattenable_aggregate(std::meta::info T) {
  if (!is_class_type(T)) {
    compile_error("Non-class types are not allowed here.");
  }
  if (!is_aggregate_type(T)) {
    return false;
  }
  for (auto base : all_direct_bases_of(T)) {
    auto ok = is_public(base) && !is_virtual(base)
           && extract_bool(^^flattenable_aggregate, type_of(base));
    if (!ok) {
      return false;
    }
  }
  for (auto member : all_direct_nonstatic_data_members_of(T)) {
    auto ok = is_public(member);
    if (!ok) {
      return false;
    }
  }
  return true;
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP
