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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_AMBIGUOUS_INHERITANCE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_AMBIGUOUS_INHERITANCE_HPP

#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval auto has_ambiguous_base(std::meta::info T) {
  if (!is_class_type(T)) {
    return false;
  }
  auto bases = std::vector{std::from_range, all_direct_bases_of(T)};
  auto head = 0zU;
  for (; head < bases.size(); ++head) {
    auto cur_candidate_bases = all_direct_bases_of(type_of(bases[head]));
    for (auto c : cur_candidate_bases) {
      auto is_unique = true;
      for (auto b : bases) {
        if (type_of(c) != type_of(b)) {
          continue;
        }
        if (!is_virtual(c) || !is_virtual(b)) {
          return true;
        } else {
          is_unique = false;
        }
      }
      if (is_unique) {
        bases.push_back(c);
      }
    }
  }
  return false;
}
}  // namespace impl

template <class T>
concept class_without_ambiguous_inheritance =
    std::is_class_v<T> && !impl::has_ambiguous_base(^^std::remove_cv_t<T>);
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_AMBIGUOUS_INHERITANCE_HPP
