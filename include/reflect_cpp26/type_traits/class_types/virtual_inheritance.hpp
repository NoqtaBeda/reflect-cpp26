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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_VIRTUAL_INHERITANCE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_VIRTUAL_INHERITANCE_HPP

#include <algorithm>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <type_traits>

namespace reflect_cpp26 {
namespace impl {
consteval bool is_class_without_virtual_inheritance(std::meta::info T);
}  // namespace impl

template <class T>
concept class_without_virtual_inheritance =
    impl::is_class_without_virtual_inheritance(remove_cv(^^T));

namespace impl {
consteval bool is_class_without_virtual_inheritance(std::meta::info T) {
  if (!is_class_type(T)) {
    return false;
  }
  auto check_fn = [](std::meta::info base) {
    if (is_virtual(base)) return false;
    return extract<bool>(^^class_without_virtual_inheritance, type_of(base));
  };
  return std::ranges::all_of(all_direct_bases_of(T), check_fn);
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_VIRTUAL_INHERITANCE_HPP
