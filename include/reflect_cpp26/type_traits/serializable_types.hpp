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

#ifndef REFLECT_CPP26_TYPE_TRAITS_SERIALIZABLE_TYPES_HPP
#define REFLECT_CPP26_TYPE_TRAITS_SERIALIZABLE_TYPES_HPP

#include <optional>
#include <ranges>
#include <reflect_cpp26/type_traits/class_types/flattened_nsdm.hpp>
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <variant>

namespace reflect_cpp26 {
namespace impl {
consteval bool is_serializable_type(std::meta::info T);

template <class T>
consteval bool is_serializable_class_type();
}  // namespace impl

template <class T>
concept serializable = impl::is_serializable_type(remove_cv(^^T));

namespace impl {
consteval bool test_serializable_flattened_members(
    std::span<const flattened_data_member_info> members) {
  auto names = std::vector<std::string_view>{};
  for (auto M : members) {
    if (!extract<bool>(^^serializable, type_of(M.member))) {
      return false;
    }
    names.push_back(identifier_of(M.member));
  }
  // Checks name duplication
  std::ranges::sort(names);
  return std::ranges::adjacent_find(names) == names.end();
}

// Precondition: T satisfies flattenable_class
template <class T>
consteval bool is_serializable_class_type() {
  constexpr const auto& members = all_flattened_nonstatic_data_members_v<T>;
  return test_serializable_flattened_members(members);
}

consteval bool is_serializable_type(std::meta::info T) {
  // (1) std::monostate serves as null in practice
  if (T == ^^std::monostate) {
    return true;
  }
  // (2) Arithmetic types; (3) Enum types
  if (is_arithmetic_type(T) || is_enum_type(T)) {
    return true;  // Including boolean and character types
  }
  // (4) String-like types
  if (extract<bool>(^^string_like, T)) {
    return true;
  }
  if (is_array_type(T)) {
    auto U = remove_all_extents(T);
    return extract<bool>(^^serializable, U);
  }
  if (extract<bool>(^^std::ranges::range, T)) {
    auto params_il = {T};
    auto U = substitute(^^std::ranges::range_value_t, params_il);
    return extract<bool>(^^serializable, U);
  }
  // (6) Tuple-like types
  if (extract<bool>(^^tuple_like, T)) {
    auto n = tuple_size(T);
    for (auto i = 0zU; i < n; i++) {
      if (!extract<bool>(^^serializable, tuple_element(i, T))) {
        return false;
      }
    }
    return true;
  }
  // (7) std::optional
  if (has_template_arguments(T) && template_of(T) == ^^std::optional) {
    auto U = template_arguments_of(T)[0];
    return extract<bool>(^^serializable, U);
  }
  // (8) std::variant
  if (has_template_arguments(T) && template_of(T) == ^^std::variant) {
    auto Us = template_arguments_of(T);
    return std::ranges::all_of(Us, [](auto U) { return extract<bool>(^^serializable, U); });
  }
  // (9) Flattenable class types
  if (extract<bool>(^^flattenable_class, T)) {
    return extract<bool (*)()>(^^is_serializable_class_type, T)();
  }
  return false;
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_SERIALIZABLE_TYPES_HPP
