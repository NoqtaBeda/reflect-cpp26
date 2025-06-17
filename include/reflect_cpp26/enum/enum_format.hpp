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

#ifndef REFLECT_CPP26_ENUM_ENUM_FORMAT_HPP
#define REFLECT_CPP26_ENUM_ENUM_FORMAT_HPP

#include <format>
#include <reflect_cpp26/enum/enum_flags_name.hpp>
#include <reflect_cpp26/enum/enum_name.hpp>
#include <reflect_cpp26/enum/enum_type_name.hpp>

#if __has_include(<fmt/format.h>)
#include <fmt/format.h>
#endif

namespace reflect_cpp26 {
namespace impl {
template <class Iter, class E>
constexpr auto enum_alt_name_to(Iter iter, E value) -> Iter {
  *iter++ = '(';
  iter = std::ranges::copy(enum_type_name_v<E>, iter).out;
  *iter++ = ')';

  std::array<char, max_decimal_digits_int64> buffer;
  auto buffer_digits_end =
      std::to_chars(buffer.begin(), buffer.end(), std::to_underlying(value)).ptr;
  return std::ranges::copy(buffer.begin(), buffer_digits_end, iter).out;
}
}  // namespace impl

/**
 * std::format and fmt::format support for enum types.
 * Enable by including this header.
 */
template <class FormatError>
struct enum_common_formatter {
  std::optional<std::string> custom_delim;
  bool as_flags = false;

  template <class ParseContext>
  constexpr auto parse(ParseContext& ctx) -> typename ParseContext::iterator {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it == end) {
      throw FormatError("Incomplete format args for enum types.");
    }
    if (*it == '}') {
      return it;
    }
    if (auto c = *it++; c == 'F' || c == 'f') {
      as_flags = true;
    } else {
      throw FormatError("Enum flag args must start with 'F' or 'f'.");
    }
    auto head = it;
    for (; it != end && *it != '}'; ++it) {
    }
    if (it == end) {
      throw FormatError("Incomplete format args for enum types.");
    }
    if (head != it) {
      custom_delim = std::string{head, it};
    }
    return it;
  }

  template <enum_type E, class FormatContext>
  auto format(E value, FormatContext& ctx) const -> typename FormatContext::iterator {
    using iterator_type = typename FormatContext::iterator;
    auto dest = ctx.out();
    if (as_flags) {
      auto do_output = [value, dest](auto delim) {
        auto [ec, out] = enum_flags_name_to(dest, std::unreachable_sentinel, value, delim);
        if (std::errc{} != ec) {
          // Expects ec == std::errc::invalid_argument, buffer unchanged
          return impl::enum_alt_name_to(dest, value);
        } else {
          return out;
        }
      };
      return custom_delim.has_value() ? do_output(*custom_delim) : do_output('|');
    } else {
      auto name = enum_name(value);
      if (name.empty()) {
        return impl::enum_alt_name_to(dest, value);
      }
      return std::ranges::copy(name, dest).out;
    }
  }
};
}  // namespace reflect_cpp26

template <reflect_cpp26::enum_type E>
struct std::formatter<E> : reflect_cpp26::enum_common_formatter<std::format_error> {};

#if __has_include(<fmt/format.h>)
template <reflect_cpp26::enum_type E>
struct fmt::formatter<E> : reflect_cpp26::enum_common_formatter<fmt::format_error> {};
#endif

#endif  // REFLECT_CPP26_ENUM_ENUM_FORMAT_HPP
