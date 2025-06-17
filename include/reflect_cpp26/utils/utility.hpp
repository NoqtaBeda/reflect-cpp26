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
#include <vector>

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
struct in_range_t {
  static constexpr auto limit_min = std::numeric_limits<R>::min();
  static constexpr auto limit_max = std::numeric_limits<R>::max();

  template <std::integral T>
  static constexpr bool operator()(T t) noexcept {
    return cmp_greater_equal_t::operator()(t, limit_min)
        && cmp_less_equal_t::operator()(t, limit_max);
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

struct identity_t {
  template <class T>
  static constexpr decltype(auto) operator()(T&& value) {
    return std::forward<T>(value);
  }
};
constexpr auto identity = identity_t{};

struct to_underlying_t {
  template <enum_type E>
  static constexpr auto operator()(E e) {
    return std::to_underlying(e);
  }
};
constexpr auto to_underlying = to_underlying_t{};

template <std::unsigned_integral To, std::integral From>
constexpr auto zero_extend(From from) {
  if constexpr (std::is_signed_v<From>) {
    auto u = std::make_unsigned_t<From>(from);
    return static_cast<To>(u);
  } else {
    return static_cast<To>(from);
  }
}

template <std::signed_integral To, std::integral From>
constexpr auto sign_extend(From from) {
  if constexpr (std::is_signed_v<From>) {
    return static_cast<To>(from);
  } else {
    auto s = std::make_signed_t<From>(from);
    return static_cast<To>(s);
  }
}

template <class T, class Allocator = std::allocator<T>>
constexpr auto make_reserved_vector(size_t n) -> std::vector<T, Allocator> {
  auto res = std::vector<T, Allocator>{};
  res.reserve(n);
  return res;
}

template <class CharT = char,
          class Traits = std::char_traits<CharT>,
          class Allocator = std::allocator<CharT>>
constexpr auto make_reserved_string(size_t n) -> std::basic_string<CharT, Traits, Allocator> {
  auto res = std::basic_string<CharT, Traits, Allocator>{};
  res.reserve(n);
  return res;
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_UTILITY_HPP
