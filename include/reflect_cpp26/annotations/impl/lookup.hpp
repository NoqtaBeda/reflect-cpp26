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

#ifndef REFLECT_CPP26_ANNOTATIONS_IMPL_LOOKUP_HPP
#define REFLECT_CPP26_ANNOTATIONS_IMPL_LOOKUP_HPP

#include <reflect_cpp26/utils/config.h>

namespace reflect_cpp26::annotations::impl {
consteval auto find_annotation_of_type(
  std::meta::info annotation_type, std::meta::info target) -> std::meta::info
{
  auto res = std::meta::info{};
  for (auto cur: annotations_of(target)) {
    if (!is_same_type(type_of(cur), annotation_type)) {
      continue;
    }
    if (std::meta::info{} != res) {
      compile_error("Duplicated annotation type disallowed.");
    }
    res = cur;
  }
  return res;
}
} // namespace reflect_cpp26::annotations::impl

#endif // REFLECT_CPP26_ANNOTATIONS_IMPL_LOOKUP_HPP
