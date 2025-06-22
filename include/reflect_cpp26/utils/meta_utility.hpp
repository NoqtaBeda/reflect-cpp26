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

#ifndef REFLECT_CPP26_UTILS_META_UTILITY_HPP
#define REFLECT_CPP26_UTILS_META_UTILITY_HPP

#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/config.h>
#include <reflect_cpp26/utils/constant.hpp>
#include <reflect_cpp26/utils/ranges.hpp>

namespace reflect_cpp26 {
// -------- Reflection with access control (P3547) --------

consteval auto unprivileged_context() {
  return std::meta::access_context::unprivileged();
}

consteval auto unchecked_context() {
  return std::meta::access_context::unchecked();
}

/**
 * Note: You need to be cautious when trying to cache member information
 * of namespaces with constexpr variables. For example:
 *   constexpr auto std_member_count =
 *     all_direct_members_of(^^std).size(); // direct members of namespace std
 *
 * The actual values may be inconsistent due to ODR violation: std_member_count
 * can be instantiated in multiple translation units with different visible
 * subsets of namespace std.
 *
 * Similarly, ODR violation may happen when you cache member information of
 * class types in different access contexts.
 */
#define REFLECT_CPP26_DEFINE_QUERY_WITH_ACCESS_CONTEXT(fn, renamed)           \
  /* Gets all direct members that are accessible in current scope */          \
  consteval auto accessible_direct_##renamed##_of(                            \
      std::meta::info a,                                                      \
      std::meta::access_context ctx = std::meta::access_context::current()) { \
    return std::meta::fn##_of(a, ctx);                                        \
  }                                                                           \
  /** Gets all direct members that are accessible in global scope */          \
  consteval auto public_direct_##renamed##_of(std::meta::info a) {            \
    return std::meta::fn##_of(a, reflect_cpp26::unprivileged_context());      \
  }                                                                           \
  /* Gets all direct members regardless of their accessibility */             \
  consteval auto all_direct_##renamed##_of(std::meta::info a) {               \
    return std::meta::fn##_of(a, reflect_cpp26::unchecked_context());         \
  }                                                                           \
  template <class_or_union_type T>                                            \
  constexpr auto public_direct_##renamed##_v =                                \
    std::define_static_array(public_direct_##renamed##_of(^^T));              \
                                                                              \
  template <class_or_union_type T>                                            \
  constexpr auto all_direct_##renamed##_v =                                   \
    std::define_static_array(all_direct_##renamed##_of(^^T));

// Expanded dfinitions see above
REFLECT_CPP26_DEFINE_QUERY_WITH_ACCESS_CONTEXT(members, members)
REFLECT_CPP26_DEFINE_QUERY_WITH_ACCESS_CONTEXT(bases, bases)
REFLECT_CPP26_DEFINE_QUERY_WITH_ACCESS_CONTEXT(static_data_members, sdm)
REFLECT_CPP26_DEFINE_QUERY_WITH_ACCESS_CONTEXT(nonstatic_data_members, nsdm)

#undef REFLECT_CPP26_DEFINE_QUERY_WITH_ACCESS_CONTEXT

// -------- Reflecting pointers-to-member --------

/**
 * Transforms a pointer to member to its corresponding reflection such that
 * reflect_pointer_to_direct_member(&Foo::bar) == ^Foo::bar
 */
template <non_function_type T, class Member>
consteval auto reflect_pointer_to_direct_member(Member T::*mptr)
  -> std::meta::info
{
  // todo: Is O(1) lookup possible?
  auto res = std::meta::info{};
  auto target = std::meta::reflect_constant(mptr);
  template for (constexpr auto cur: all_direct_nsdm_v<T>) {
    if constexpr (!is_reference_type(type_of(cur)) && !is_bit_field(cur)) {
      if (std::meta::reflect_constant(&[:cur:]) == target) {
        res = cur;
        break;
      }
    }
  }
  if (std::meta::info{} == res) {
    compile_error("Not found.");
  }
  return res;
}

// -------- Extration helpers --------

template <std::meta::info V>
consteval auto extract()
{
  using T = [:type_of(V):];
  return std::meta::extract<T>(V);
}

template <std::meta::info V>
consteval auto extract(constant<V>)
{
  using T = [:type_of(V):];
  return std::meta::extract<T>(V);
}

/**
 * Equivalent to extract<T>(substitute(templ, {templ_params})).
 */
template <class T, std::same_as<std::meta::info>... Args>
consteval T extract(std::meta::info templ, Args... templ_params)
{
  return std::meta::extract<T>(
    std::meta::substitute(templ, {templ_params...}));
}

/**
 * Equivalent to extract<bool>(substitute(templ, {templ_params})).
 * Frequently used with type traits and concepts.
 */
template <std::same_as<std::meta::info>... Args>
consteval bool extract_bool(std::meta::info templ, Args... templ_params)
{
  return std::meta::extract<bool>(
    std::meta::substitute(templ, {templ_params...}));
}

// -------- Substitution helpers --------

template <std::same_as<std::meta::info>... Args>
consteval auto substitute(std::meta::info templ, Args... templ_params)
  -> std::meta::info {
  return std::meta::substitute(templ, {templ_params...});
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_META_UTILITY_HPP
