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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CVREF_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CVREF_HPP

#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/config.h>

namespace reflect_cpp26 {
consteval auto add_cvref_like(std::meta::info dest, std::meta::info src)
{
  if (!is_type(dest)) {
    compile_error("dest must be a type.");
  }
  if (!is_type(src)) {
    compile_error("src must be a type.");
  }
  if (is_reference_type(dest)) {
    compile_error("dest can not be a reference type.");
  }

  auto is_lref = is_lvalue_reference_type(src);
  auto is_rref = is_rvalue_reference_type(src);
  auto has_const = is_const(remove_reference(dest)) ||
    is_const(remove_reference(src));
  auto has_volatile = is_volatile(remove_reference(dest)) ||
    is_volatile(remove_reference(src));

  dest = remove_cv(dest);
  if (has_const) {
    dest = add_const(dest);
  }
  if (has_volatile) {
    dest = add_volatile(dest);
  }
  if (is_lref) {
    dest = add_lvalue_reference(dest);
  } else if (is_rref) {
    dest = add_rvalue_reference(dest);
  }
  return dest;
}

template <non_reference_type Dest, class Src>
using add_cvref_like_t = [: add_cvref_like(^^Dest, ^^Src) :];
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CVREF_HPP
