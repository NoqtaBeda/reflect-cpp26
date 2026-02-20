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

#ifndef REFLECT_CPP26_TYPE_TRAITS_STRUCTURAL_TYPES_HPP
#define REFLECT_CPP26_TYPE_TRAITS_STRUCTURAL_TYPES_HPP

#include <algorithm>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval bool is_structural_type(std::meta::info T);

template <auto V>
struct structural_type_test_helper {};

template <class T>
constexpr auto is_value_initializable_structural_type_v =
    requires { structural_type_test_helper<T{}>{}; };
}  // namespace impl

template <class T>
concept structural_type = impl::is_structural_type(^^std::remove_cv_t<T>);

namespace impl {
consteval bool is_structural_nsdm(std::meta::info m) {
  return is_public(m) && !is_volatile(m) && !is_mutable_member(m)
      && extract<bool>(^^structural_type, remove_all_extents(type_of(m)));
}

consteval bool is_structural_class_type(std::meta::info T) {
  if (!is_destructible_type(T)) {
    return false;
  }
  auto bases_are_structural = std::ranges::all_of(all_direct_bases_of(T), [](std::meta::info base) {
    return is_public(base) && !is_virtual(base) && extract<bool>(^^structural_type, type_of(base));
  });
  return bases_are_structural
      && std::ranges::all_of(all_direct_nonstatic_data_members_of(T), is_structural_nsdm);
}

consteval bool is_structural_union_type(std::meta::info T) {
  if (!is_trivially_destructible_type(T)) {
    return false;
  }
  return std::ranges::any_of(all_direct_nonstatic_data_members_of(T), is_structural_nsdm);
}

consteval bool is_structural_type(std::meta::info T) {
  if (is_reference_type(T)) {
    return is_lvalue_reference_type(T);  // Filters out rvalue references
  }
  if (extract<bool>(^^is_value_initializable_structural_type_v, T)) {
    return true;  // Including all scalar types
  }
  if (is_union_type(T)) {
    return is_structural_union_type(remove_cv(T));
  }
  if (is_class_type(T)) {
    return is_structural_class_type(remove_cv(T));
  }
  return false;
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_STRUCTURAL_TYPES_HPP
