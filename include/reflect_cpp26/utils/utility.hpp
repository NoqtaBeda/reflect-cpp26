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

#ifndef REFLECT_CPP26_UTILS_UTILITY_HPP
#define REFLECT_CPP26_UTILS_UTILITY_HPP

#include <compare>
#include <functional>
#include <limits>
#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <utility>

namespace reflect_cpp26 {
// -------- Relaxed alternative of integer comparison  --------
// bool and character types are allowed

struct cmp_equal_t {
  template <std::integral T, std::integral U>
  static constexpr bool operator()(T t, U u) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
      return t == u;
    else if constexpr (std::is_signed_v<T>)
      return t >= 0 && std::make_unsigned_t<T>(t) == u;
    else
      return u >= 0 && std::make_unsigned_t<U>(u) == t;
  }
};

struct cmp_not_equal_t {
  template <std::integral T, std::integral U>
  static constexpr bool operator()(T t, U u) noexcept {
    return !cmp_equal_t::operator()(t, u);
  }
};

struct cmp_less_t {
  // integral_to_integer_t is used to eliminate -Wcharacter-conversion.
  template <std::integral T, std::integral U>
  static constexpr bool operator()(T t, U u) noexcept {
    return do_compare(integral_to_integer_t<T>(t), integral_to_integer_t<U>(u));
  }

private:
  template <class T, class U>
  static constexpr bool do_compare(T t, U u) noexcept {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
      return t < u;
    else if constexpr (std::is_signed_v<T>)
      return t < 0 || std::make_unsigned_t<T>(t) < u;
    else
      return u >= 0 && t < std::make_unsigned_t<U>(u);
  }
};

struct cmp_greater_t {
  template <std::integral T, std::integral U>
  static constexpr bool operator()(T t, U u) noexcept {
    return cmp_less_t::operator()(u, t);
  }
};

struct cmp_less_equal_t {
  template <std::integral T, std::integral U>
  static constexpr bool operator()(T t, U u) noexcept {
    return !cmp_less_t::operator()(u, t);
  }
};

struct cmp_greater_equal_t {
  template <std::integral T, std::integral U>
  static constexpr bool operator()(T t, U u) noexcept {
    return !cmp_less_t::operator()(t, u);
  }
};

// Integral three-way comparison is named with 'cmp_' prefix
// analogous to other integral comparators in C++ standard library.
struct cmp_three_way_t {
  // integral_to_integer_t is used to eliminate -Wcharacter-conversion.
  template <std::integral T, std::integral U>
  static constexpr auto operator()(T t, U u) noexcept {
    return do_compare(integral_to_integer_t<T>(t), integral_to_integer_t<U>(u));
  }

private:
  template <class T, class U>
  static constexpr auto do_compare(T t, U u) noexcept -> std::strong_ordering {
    if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
      return t <=> u;
    } else if constexpr (std::is_signed_v<T>) {
      if (t < 0) {
        return std::strong_ordering::less;
      }
      return std::make_unsigned_t<T>(t) <=> u;
    } else {
      if (u < 0) {
        return std::strong_ordering::greater;
      }
      return t <=> std::make_unsigned_t<U>(u);
    }
  }
};

template <std::integral R>
  requires(!std::is_const_v<R> && !std::is_volatile_v<R>)
struct in_range_t {
  static constexpr auto limit_min = std::numeric_limits<R>::min();
  static constexpr auto limit_max = std::numeric_limits<R>::max();

  template <std::integral T>
  static constexpr bool operator()(T t) noexcept {
    return cmp_greater_equal_t::operator()(t, limit_min)
        && cmp_less_equal_t::operator()(t, limit_max);
  }
};

template <>
struct in_range_t<bool> {
  template <std::integral T>
  static constexpr bool operator()(T t) noexcept {
    return t == 0 || t == 1;
  }
};

constexpr auto cmp_less = cmp_less_t{};
constexpr auto cmp_greater = cmp_greater_t{};
constexpr auto cmp_less_equal = cmp_less_equal_t{};
constexpr auto cmp_greater_equal = cmp_greater_equal_t{};
constexpr auto cmp_equal = cmp_equal_t{};
constexpr auto cmp_not_equal = cmp_not_equal_t{};
constexpr auto cmp_three_way = cmp_three_way_t{};
template <class R>
constexpr auto in_range = in_range_t<R>{};

struct to_underlying_t {
  template <enum_type E>
  static constexpr auto operator()(E e) {
    return std::to_underlying(e);
  }
};
constexpr auto to_underlying = to_underlying_t{};

template <std::unsigned_integral To>
struct zero_extend_t {
  template <std::integral From>
  static constexpr auto operator()(From from) {
    if constexpr (std::is_signed_v<From>) {
      auto u = std::make_unsigned_t<From>(from);
      return static_cast<To>(u);
    } else {
      return static_cast<To>(from);
    }
  }
};
template <class To>
constexpr auto zero_extend = zero_extend_t<To>{};

template <std::signed_integral To>
struct sign_extend_t {
  template <std::integral From>
  static constexpr auto operator()(From from) {
    if constexpr (std::is_signed_v<From>) {
      return static_cast<To>(from);
    } else {
      auto s = std::make_signed_t<From>(from);
      return static_cast<To>(s);
    }
  }
};
template <class To>
constexpr auto sign_extend = sign_extend_t<To>{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_UTILITY_HPP
