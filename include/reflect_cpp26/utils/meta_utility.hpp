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

enum class access_mode {
  unprivileged,
  unchecked,
};

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
  }                                                                      \
  /* Gets all direct members of specified access mode */                 \
  consteval auto direct_##fn##_of(access_mode mode, std::meta::info a) { \
    if (mode == access_mode::unprivileged) {                             \
      return public_direct_##fn##_of(a);                                 \
    } else if (mode == access_mode::unchecked) {                         \
      return all_direct_##fn##_of(a);                                    \
    } else {                                                             \
      compile_error("Invalid access mode.");                             \
    }                                                                    \
  }

#define REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(fn)                                      \
  template <class_or_union_type T>                                                                \
  constexpr auto public_direct_##fn##_v = std::define_static_array(public_direct_##fn##_of(^^T)); \
                                                                                                  \
  template <class_or_union_type T>                                                                \
  constexpr auto all_direct_##fn##_v = std::define_static_array(all_direct_##fn##_of(^^T));       \
                                                                                                  \
  template <access_mode Mode, class_or_union_type T>                                              \
  constexpr auto direct_##fn##_v = []() consteval {                                               \
    if constexpr (Mode == access_mode::unprivileged) {                                            \
      return public_direct_##fn##_v<T>;                                                           \
    } else if constexpr (Mode == access_mode::unchecked) {                                        \
      return all_direct_##fn##_v<T>;                                                              \
    } else {                                                                                      \
      return compile_error("Invalid mode.");                                                      \
    }                                                                                             \
  };

/**
 * public_direct_*_of(std::meta::info a) -> std::vector<std::meta::info>
 *   Equivalent to std::meta::*_of(a, unprivileged_context()).
 *
 * all_direct_*_of(std::meta::info a) -> std::vector<std::meta::info>
 *   Equivalent to std::meta::*_of(a, unchecked_context()).
 *
 * direct_*_of(access_mode mode, std::meta::info a)
 *   -> std::vector<std::meta::info>
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
 * direct_*_v<Mode, T>
 *   Stores result of direct_*_v(Mode, ^^T).
 *
 * * is one of macro parameters below.
 */
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(members)
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(bases)
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(static_data_members)
REFLECT_CPP26_DEFINE_QUERY_V_WITH_ACCESS_CONTEXT(nonstatic_data_members)

#undef REFLECT_CPP26_DEFINE_QUERY_FN_WITH_ACCESS_CONTEXT

// -------- Member reflection property query --------

/**
 * Whether m is a class member (data member or member function, either static
 * or not) that supports addressing, i.e. &[:m:] is a valid expression.
 */
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
    // Static data members: always supports addressing even if it's a reference
    return true;
  }
}

/**
 * Whether m is a non-class member that supports addressing, i.e.
 * &[:m:] is a valid expression.
 */
consteval bool is_addressable_non_class_member(std::meta::info m) {
  if (is_type(m) || is_namespace(m) || is_template(m)) {
    return false;
  }
  if (is_class_member(m)) {
    return false;
  }
  if (is_function(m) && is_deleted(m)) {
    return false;
  }
  return true;
}

// -------- Reflecting pointers-to-member --------

/**
 * Transforms a pointer to member to its corresponding reflection such that
 * reflect_pointer_to_direct_member(&Foo::bar) == ^^Foo::bar.
 * Both non-static data member and non-static member functions are supported.
 *
 * Note: If not specified manually, T is always deduced as the direct class type
 * where the given member is defined. Example:
 * struct Base { int x; };
 * struct Derived : Base {};
 * // T = Base (where x is defined directly), Member = int
 * constexpr auto m = reflect_pointer_to_member(&Derived::x);
 */
template <class T, class Member>
consteval auto reflect_pointer_to_member(Member T::* mptr) -> std::meta::info {
  // todo: Is O(1) lookup possible?
  auto target = std::meta::reflect_constant(mptr);
  template for (constexpr auto cur : all_direct_members_v<T>) {
    if constexpr (is_addressable_class_member(cur)) {
      auto cur_refl = std::meta::reflect_constant(&[:cur:]);
      if (cur_refl == target) {
        return cur;
      }
    }
  }
  compile_error("Not found.");
  return {};  // Dummy
}

// -------- Extration helpers --------

template <std::meta::info V>
consteval auto extract() {
  using T = [:type_of(V):];
  return std::meta::extract<T>(V);
}

/**
 * Equivalent to extract<T>(substitute(templ, {templ_params})).
 */
template <class T, std::same_as<std::meta::info>... Args>
consteval T extract(std::meta::info templ, Args... templ_params) {
  return std::meta::extract<T>(std::meta::substitute(templ, {templ_params...}));
}

/**
 * Equivalent to extract<bool>(substitute(templ, {templ_params})).
 * Frequently used with type traits and concepts.
 */
template <std::same_as<std::meta::info>... Args>
consteval bool extract_bool(std::meta::info templ, Args... templ_params) {
  return std::meta::extract<bool>(std::meta::substitute(templ, {templ_params...}));
}

// -------- Substitution helpers --------

/**
 * Equivalent to substitute(templ, {templ_params}).
 */
template <std::same_as<std::meta::info>... Args>
consteval auto substitute(std::meta::info templ, Args... templ_params) -> std::meta::info {
  return std::meta::substitute(templ, {templ_params...});
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
