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

#ifndef REFLECT_CPP26_ENUM_ENUM_SWITCH_HPP
#define REFLECT_CPP26_ENUM_ENUM_SWITCH_HPP

#include <reflect_cpp26/enum/enum_meta_entries.hpp>
#include <reflect_cpp26/utils/constant.hpp>
#include <reflect_cpp26/utils/utility.hpp>
#include <type_traits>

namespace reflect_cpp26 {
namespace impl {
template <class R, class E, class Func>
constexpr auto enum_switch_is_invocable_r() -> bool {
  template for (constexpr auto entry : enum_meta_entries_v<E>) {
    constexpr auto ev = extract<E>(entry);
    if (!std::is_invocable_r_v<R, Func, constant_t<ev>>) {
      return false;
    }
  }
  return true;
}

template <class R, class E, class Func>
concept enum_switch_invocable_r = enum_switch_is_invocable_r<R, E, Func>();

template <class E, class Func>
REFLECT_CPP26_ALWAYS_INLINE constexpr auto enum_switch_void(Func&& func, E value) -> void {
  template for (constexpr auto entry : enum_meta_entries_v<E>) {
    constexpr auto ev = extract<E>(entry);
    if (ev == value) {
      REFLECT_CPP26_ALWAYS_INLINE_CALL func(constant_v<ev>);
      return;
    }
  }
}

template <class E, class Func, class FallbackFunc>
REFLECT_CPP26_ALWAYS_INLINE constexpr auto enum_switch_void_with_fallback_func(
    Func&& func, FallbackFunc&& fallback, E value) -> void {
  template for (constexpr auto entry : enum_meta_entries_v<E>) {
    constexpr auto ev = extract<E>(entry);
    if (ev == value) {
      REFLECT_CPP26_ALWAYS_INLINE_CALL func(constant_v<ev>);
      return;
    }
  }
  REFLECT_CPP26_ALWAYS_INLINE_CALL fallback(value);
}

template <class T, class E, class Func>
REFLECT_CPP26_ALWAYS_INLINE constexpr auto enum_switch_optional(Func&& func, E value)
    -> std::optional<T> {
  auto res = std::optional<T>{};
  template for (constexpr auto entry : enum_meta_entries_v<E>) {
    constexpr auto ev = extract<E>(entry);
    if (ev == value) {
      REFLECT_CPP26_ALWAYS_INLINE_CALL res = func(constant_v<ev>);
      break;
    }
  }
  return res;
}

template <class R, class T, class E, class Func>
REFLECT_CPP26_ALWAYS_INLINE constexpr auto enum_switch_value(Func&& func, E value, T&& init) -> R {
  template for (constexpr auto entry : enum_meta_entries_v<E>) {
    constexpr auto ev = extract<E>(entry);
    if (ev == value) {
      REFLECT_CPP26_ALWAYS_INLINE_CALL return func(constant_v<ev>);
    }
  }
  return init;
}
}  // namespace impl

template <enum_type E, class Func, class FallbackFunc>
  requires(impl::enum_switch_invocable_r<void, E, Func>
           && std::is_invocable_r_v<void, FallbackFunc, E>)
REFLECT_CPP26_ALWAYS_INLINE constexpr auto enum_switch(Func&& func,
                                                       FallbackFunc&& fallback,
                                                       E value) {
  return impl::enum_switch_void_with_fallback_func(
      std::forward<Func>(func), std::forward<FallbackFunc>(fallback), value);
}

template <non_reference_type T = void, enum_type E, class Func>
  requires(impl::enum_switch_invocable_r<T, E, Func>)
REFLECT_CPP26_ALWAYS_INLINE constexpr auto enum_switch(Func&& func, E value) {
  if constexpr (std::is_void_v<T>) {
    return impl::enum_switch_void(std::forward<Func>(func), value);
  } else {
    return impl::enum_switch_optional<T>(std::forward<Func>(func), value);
  }
}

template <class T, enum_type E, class Func>
  requires(impl::enum_switch_invocable_r<std::decay_t<T>, E, Func>)
REFLECT_CPP26_ALWAYS_INLINE constexpr auto enum_switch(Func&& func, E value, T&& default_value) {
  return impl::enum_switch_value<std::decay_t<T>>(
      std::forward<Func>(func), value, std::forward<T>(default_value));
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_ENUM_ENUM_SWITCH_HPP
