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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CVREF_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CVREF_HPP

#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/config.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval auto add_cv(std::meta::info dest, bool has_const, bool has_volatile) -> std::meta::info {
  if (has_const) {
    dest = add_const(dest);
  }
  if (has_volatile) {
    dest = add_volatile(dest);
  }
  return dest;
}

consteval auto add_cv_or_cvref_like(std::meta::info dest, std::meta::info src, bool adds_ref)
    -> std::meta::info {
  if (!is_type(dest)) {
    compile_error("dest must be a type.");
  }
  if (!is_type(src)) {
    compile_error("src must be a type.");
  }
  if (is_reference_type(dest)) {
    compile_error("dest can not be a reference type.");
  }

  auto has_const = is_const(dest) || is_const(remove_reference(src));
  auto has_volatile = is_volatile(dest) || is_volatile(remove_reference(src));

  dest = add_cv(dest, has_const, has_volatile);
  if (adds_ref && is_lvalue_reference_type(src)) {
    dest = add_lvalue_reference(dest);
  } else if (adds_ref && is_rvalue_reference_type(src)) {
    dest = add_rvalue_reference(dest);
  }
  return dest;
}

consteval auto propagate_cv(std::meta::info dest, bool has_const, bool has_volatile)
    -> std::meta::info {
  if (is_pointer_type(dest)) {
    // (1) Pointer: T** -> const T* const* const
    auto propagated = propagate_cv(remove_pointer(dest), has_const, has_volatile);
    has_const |= is_const(dest);
    has_volatile |= is_volatile(dest);
    return add_cv(add_pointer(propagated), has_const, has_volatile);
  } else if (is_reference_type(dest)) {
    // (2) Reference: T**& -> const T* const* const&
    auto propagated = propagate_cv(remove_reference(dest), has_const, has_volatile);
    bool L = is_lvalue_reference_type(dest);
    return L ? add_lvalue_reference(propagated) : add_rvalue_reference(propagated);
  } else {
    // (3) Others: T -> const T
    return add_cv(dest, has_const, has_volatile);
  }
}

consteval auto propagate_cv_like(std::meta::info dest, std::meta::info src) -> std::meta::info {
  if (!is_type(dest)) {
    compile_error("dest must be a type.");
  }
  if (!is_type(src)) {
    compile_error("src must be a type.");
  }
  auto has_const = is_const(remove_reference(src));
  auto has_volatile = is_volatile(remove_reference(src));
  return propagate_cv(dest, has_const, has_volatile);
}
}  // namespace impl

consteval auto add_cv_like(std::meta::info dest, std::meta::info src) -> std::meta::info {
  return impl::add_cv_or_cvref_like(dest, src, false);
}

consteval auto add_cvref_like(std::meta::info dest, std::meta::info src) -> std::meta::info {
  return impl::add_cv_or_cvref_like(dest, src, true);
}

consteval auto propagate_cv_like(std::meta::info dest, std::meta::info src) -> std::meta::info {
  return impl::propagate_cv_like(dest, src);
}

template <non_reference_type Dest, class Src>
using add_cv_like_t = [:add_cv_like(^^Dest, ^^Src):];

template <non_reference_type Dest, class Src>
using add_cvref_like_t = [:add_cvref_like(^^Dest, ^^Src):];

template <class Dest, class Src>
using propagate_cv_like_t = [:propagate_cv_like(^^Dest, ^^Src):];
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CVREF_HPP
