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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_STRUCTURED_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_STRUCTURED_HPP

#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval bool is_structured_type(std::meta::info T);

template <auto V>
struct structured_type_test_helper {};

template <class T>
constexpr auto is_value_initializable_structured_type_v =
    requires { structured_type_test_helper<T{}>{}; };
}  // namespace impl

/**
 * Tests whether T is a structured type.
 * Structured types are those whose value can be non-type template parameters.
 * See: https://en.cppreference.com/w/cpp/language/template_parameters
 */
template <class T>
concept structured_type = impl::is_structured_type(^^std::remove_cv_t<T>);

namespace impl {
consteval bool is_structured_nsdm(std::meta::info m) {
  return is_public(m) && !is_volatile(m) && !is_mutable_member(m)
      && extract_bool(^^structured_type, remove_all_extents(type_of(m)));
}

consteval bool is_structured_class_type(std::meta::info T) {
  if (!is_destructible_type(T)) {
    return false;
  }
  auto bases_are_structured = std::ranges::all_of(all_direct_bases_of(T), [](std::meta::info base) {
    return is_public(base) && !is_virtual(base) && extract_bool(^^structured_type, type_of(base));
  });
  return bases_are_structured
      && std::ranges::all_of(all_direct_nonstatic_data_members_of(T), is_structured_nsdm);
}

consteval bool is_structured_union_type(std::meta::info T) {
  if (!is_trivially_destructible_type(T)) {
    return false;
  }
  return std::ranges::any_of(all_direct_nonstatic_data_members_of(T), is_structured_nsdm);
}

consteval bool is_structured_type(std::meta::info T) {
  if (is_reference_type(T)) {
    return is_lvalue_reference_type(T);  // Filters out rvalue references
  }
  if (extract_bool(^^is_value_initializable_structured_type_v, T)) {
    return true;  // Including all scalar types
  }
  if (is_union_type(T)) {
    return is_structured_union_type(remove_cv(T));
  }
  if (is_class_type(T)) {
    return is_structured_class_type(remove_cv(T));
  }
  return false;
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_STRUCTURED_HPP
