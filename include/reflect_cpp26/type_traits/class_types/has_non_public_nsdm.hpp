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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_HAS_non_public_nsdm_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_HAS_non_public_nsdm_HPP

#include <reflect_cpp26/type_traits/class_types/has_virtual_inheritance.hpp>

// NSDM: Non-Static Data Members
namespace reflect_cpp26 {
namespace impl {
consteval bool has_non_public_nsdm(std::meta::info T);
} // namespace impl

/**
 * Checks whether a non-union class T without virtual inheritance has
 * inaccessible non-static data members (NSDM), including:
 * (1) direct protected or private data members;
 * (2) all data members of protected or private direct base classes;
 * (3) protected or private data members from public base classes, recursively.
 *
 * Always false for non-class types:
 * scalar types, references, arrays, unions, etc.
 */
template <class T>
constexpr auto has_non_public_nsdm_v = false;

template <class_type T>
constexpr auto has_non_public_nsdm_v<T> =
  impl::has_non_public_nsdm(^^std::remove_cv_t<T>);

template <class T>
concept class_with_non_public_nsdm =
  std::is_class_v<T> && has_non_public_nsdm_v<T>;

template <class T>
concept class_without_non_public_nsdm =
  std::is_class_v<T> && !has_non_public_nsdm_v<T>;

namespace impl {
consteval bool has_non_public_nsdm(std::meta::info T)
{
  auto has_non_public_nsdm = !std::ranges::all_of(
    all_direct_nsdm_of(T), std::meta::is_public);

  return has_non_public_nsdm || std::ranges::any_of(all_direct_bases_of(T),
    [](std::meta::info base) {
      if (is_virtual(base)) {
        compile_error("Virtual base class is not allowed.");
      }
      return is_public(base)
        ? extract_bool(^^has_non_public_nsdm_v, type_of(base))
        : !is_empty_type(type_of(base));
    });
}
} // namespace impl
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_HAS_non_public_nsdm_HPP
