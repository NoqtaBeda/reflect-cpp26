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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_HAS_NON_PUBLIC_NSDM_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_HAS_NON_PUBLIC_NSDM_HPP

#include <reflect_cpp26/type_traits/class_types/has_virtual_inheritance.hpp>

// NSDM: Non-Static Data Members
namespace reflect_cpp26 {
namespace impl {
consteval bool is_class_type_with_non_public_nsdm(std::meta::info T);
}  // namespace impl

/**
 * Checks whether a non-union class T has inaccessible non-static data members,
 * including:
 * (1) direct protected or private data members;
 * (2) all data members of protected or private direct base classes;
 * (3) protected or private data members from public base classes, recursively.
 *
 * Always false for non-class types:
 * scalar types, references, arrays, unions, etc.
 */
template <class T>
constexpr auto has_non_public_nonstatic_data_members_v =
    impl::is_class_type_with_non_public_nsdm(^^std::remove_cv_t<T>);

/**
 * Whether T is a class type without non-static data members of non-public
 * access. Details see above.
 */
template <class T>
concept class_without_non_public_nonstatic_data_members =
    std::is_class_v<T> && !has_non_public_nonstatic_data_members_v<T>;

namespace impl {
consteval bool is_class_type_with_non_public_nsdm(std::meta::info T) {
  if (!is_class_type(T)) {
    return false;
  }
  auto has_direct_non_public_nsdm =
      !std::ranges::all_of(all_direct_nonstatic_data_members_of(T), std::meta::is_public);
  if (has_direct_non_public_nsdm) {
    return true;
  }
  return std::ranges::any_of(all_direct_bases_of(T), [](std::meta::info base) {
    if (is_public(base)) {
      return extract_bool(^^has_non_public_nonstatic_data_members_v, type_of(base));
    }
    return !is_empty_type(type_of(base));
  });
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_HAS_NON_PUBLIC_NSDM_HPP
