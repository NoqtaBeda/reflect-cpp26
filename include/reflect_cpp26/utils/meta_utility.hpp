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

#ifndef REFLECT_CPP26_UTILS_META_UTILITY_HPP
#define REFLECT_CPP26_UTILS_META_UTILITY_HPP

#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/config.hpp>
#include <reflect_cpp26/utils/ranges.hpp>

#define REFLECT_CPP26_CURRENT_CONTEXT std::meta::access_context::current()

namespace reflect_cpp26 {
// -------- Reflection with access control (P3547) --------

consteval auto unprivileged_context() {
  return std::meta::access_context::unprivileged();
}

consteval auto unchecked_context() {
  return std::meta::access_context::unchecked();
}

#define REFLECT_CPP26_DEFINE_QUERY_FN_WITH_ACCESS_CONTEXT(fn)            \
  /** Gets all direct members that are accessible in global scope */     \
  consteval auto public_direct_##fn##_of(std::meta::info a) {            \
    return std::meta::fn##_of(a, reflect_cpp26::unprivileged_context()); \
  }                                                                      \
  /* Gets all direct members regardless of their accessibility */        \
  consteval auto all_direct_##fn##_of(std::meta::info a) {               \
    return std::meta::fn##_of(a, reflect_cpp26::unchecked_context());    \
  }

#define REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(fn)                                      \
  template <class_or_union_type T>                                                                \
  constexpr auto public_direct_##fn##_v = std::define_static_array(public_direct_##fn##_of(^^T)); \
                                                                                                  \
  template <class_or_union_type T>                                                                \
  constexpr auto all_direct_##fn##_v = std::define_static_array(all_direct_##fn##_of(^^T));

/**
 * public_direct_*_of(std::meta::info a) -> std::vector<std::meta::info>
 *   Equivalent to std::meta::*_of(a, unprivileged_context()).
 *
 * all_direct_*_of(std::meta::info a) -> std::vector<std::meta::info>
 *   Equivalent to std::meta::*_of(a, unchecked_context()).
 *
 * * is one of macro parameters below.
 */
REFLECT_CPP26_DEFINE_QUERY_FN_WITH_ACCESS_CONTEXT(members)
REFLECT_CPP26_DEFINE_QUERY_FN_WITH_ACCESS_CONTEXT(bases)
REFLECT_CPP26_DEFINE_QUERY_FN_WITH_ACCESS_CONTEXT(static_data_members)
REFLECT_CPP26_DEFINE_QUERY_FN_WITH_ACCESS_CONTEXT(nonstatic_data_members)

/**
 * public_direct_*_v<T>
 *   Stores result of public_direct_*_v(^^T).
 * all_direct_*_v<T>
 *   Stores result of all_direct_*_v(^^T).
 *
 * * is one of macro parameters below.
 */
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(members)
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(bases)
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(static_data_members)
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(nonstatic_data_members)

#undef REFLECT_CPP26_DEFINE_QUERY_FN_WITH_ACCESS_CONTEXT

// -------- Extration helpers --------

/**
 * Equivalent to extract<T>(substitute(templ, {templ_params})).
 */
template <class T, std::same_as<std::meta::info>... Args>
consteval T extract(std::meta::info templ, Args... templ_params) {
  return std::meta::extract<T>(std::meta::substitute(templ, {templ_params...}));
}

// -------- Identifier helpers --------

/**
 * If has_identifier(m) is true, then its identifier is returned;
 * Otherwise, alt is returned.
 */
struct identifier_of_t {
  static consteval auto operator()(std::meta::info m, std::string_view alt = "")
      -> std::string_view {
    return has_identifier(m) ? std::meta::identifier_of(m) : alt;
  }
};
constexpr auto identifier_of = identifier_of_t{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_META_UTILITY_HPP
