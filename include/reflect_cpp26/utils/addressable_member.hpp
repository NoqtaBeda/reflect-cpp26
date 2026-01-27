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

#ifndef REFLECT_CPP26_UTILS_ADDRESSABLE_MEMBER_HPP
#define REFLECT_CPP26_UTILS_ADDRESSABLE_MEMBER_HPP

#include <reflect_cpp26/utils/config.hpp>

namespace reflect_cpp26 {
consteval bool is_addressable_class_member(std::meta::info m) {
  if (!is_class_member(m) || is_template(m) || is_type(m)) {
    return false;
  }
  if (is_nonstatic_data_member(m)) {
    return !is_reference_type(type_of(m)) && !is_bit_field(m);
  } else if (is_function(m)) {
    // Member functions (including static and non-static)
    return !is_constructor(m) && !is_destructor(m) && !is_deleted(m);
  } else {
    // Static data members: always supports addressing (even if it's a reference)
    return true;
  }
}

consteval bool is_addressable_non_class_member(std::meta::info m) {
  if (is_class_member(m) || is_template(m)) {
    return false;
  }
  return is_variable(m) || (is_function(m) && !is_deleted(m));
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_ADDRESSABLE_MEMBER_HPP
