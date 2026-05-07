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

#ifndef REFLECT_CPP26_TYPE_TRAITS_ENUM_TYPES_HPP
#define REFLECT_CPP26_TYPE_TRAITS_ENUM_TYPES_HPP

#include <concepts>
#include <reflect_cpp26/utils/concepts.hpp>  // enum_type, scoped_enum_type
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <type_traits>

#define REFLECT_CPP26_AS_ENUM_FLAG [[= ::reflect_cpp26::impl::enum_flag_annotation_tag_t{}]]

#ifdef REFLECT_CPP26_IMPORT_MACROS
#define AS_ENUM_FLAG REFLECT_CPP26_AS_ENUM_FLAG
#endif

namespace reflect_cpp26 {
// Specialize is_enum_flag_v to mark an enum type as flag type.
template <class T>
constexpr auto is_enum_flag_v = false;

namespace impl {
struct enum_flag_annotation_tag_t {};

consteval bool is_enum_flag_type(std::meta::info T) {
  if (!is_enum_type(T)) {
    return false;
  }
  if (extract<bool>(^^is_enum_flag_v, T)) {
    return true;
  }
  // Alternative: Annotate the enum type via macro REFLECT_CPP26_AS_ENUM_FLAG
  return !annotations_of_with_type(T, ^^enum_flag_annotation_tag_t).empty();
}
}  // namespace impl

template <class T>
concept enum_flag_type = impl::is_enum_flag_type(remove_cv(^^T));
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_ENUM_TYPES_HPP
