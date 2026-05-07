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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_IMPL_SERIALIZE_TO_JSON_COMMON_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_IMPL_SERIALIZE_TO_JSON_COMMON_HPP

#include <ranges>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/string_builder.hpp>

namespace reflect_cpp26::impl::json {
template <class Derived>
struct indented_serializer_base {
  template <class CharT, class Allocator, class T>
  static constexpr bool append_range(basic_string_builder<CharT, Allocator>& dest,
                                     const T& value,
                                     int indent_level,
                                     int indent_size,
                                     CharT indent_char) {
    dest.append_char('[');
    indent_level += indent_size;
    auto is_first = true;

    using V = typename T::value_type;
    for (const V& elem : value) {
      if (is_first) {
        is_first = false;
        dest.reserve_at_least(indent_level + 1)
            .append_char_unsafe('\n')
            .append_char_unsafe(indent_char, indent_level);
      } else {
        dest.reserve_at_least(indent_level + 2)
            .append_char_unsafe(',')
            .append_char_unsafe('\n')
            .append_char_unsafe(indent_char, indent_level);
      }
      if (!Derived::operator()(dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.reserve_at_least(indent_level + 2)
        .append_char_unsafe('\n')
        .append_char_unsafe(indent_char, indent_level)
        .append_char(']');
    return true;
  }

  template <class CharT, class Allocator, class T>
  static constexpr bool append_map(basic_string_builder<CharT, Allocator>& dest,
                                   const T& value,
                                   int indent_level,
                                   int indent_size,
                                   CharT indent_char) {
    dest.append_char('{');
    indent_level += indent_size;
    auto is_first = true;

    for (const auto& [k, v] : value) {
      auto key_sv = make_string_view(k);
      if (is_first) {
        is_first = false;
        dest.reserve_at_least(indent_level + 6 * key_sv.length() + 5)
            .append_char_unsafe('\n')
            .append_char_unsafe(indent_char, indent_level);
      } else {
        dest.reserve_at_least(indent_level + 6 * key_sv.length() + 6)
            .append_char_unsafe(',')   // +1
            .append_char_unsafe('\n')  // +1
            .append_char_unsafe(indent_char, indent_level);
      }
      dest.append_char_unsafe('"')  // +1
          .append_utf_string_json_escaped_unsafe(key_sv)
          .append_c_string_unsafe("\": ");  // +3: '"', ':' and ' '
      if (!Derived::operator()(dest, v, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.reserve_at_least(indent_level + 2)
        .append_char_unsafe('\n')
        .append_char_unsafe(indent_char, indent_level)
        .append_char('}');
    return true;
  }

  template <class CharT, class Allocator, class T>
  static constexpr bool append_tuple(basic_string_builder<CharT, Allocator>& dest,
                                     const T& value,
                                     int indent_level,
                                     int indent_size,
                                     CharT indent_char) {
    constexpr auto N = std::tuple_size_v<T>;
    dest.append_char('[');
    indent_level += indent_size;

    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if constexpr (I == 0) {
        dest.reserve_at_least(indent_level + 1)
            .append_char_unsafe('\n')
            .append_char_unsafe(indent_char, indent_level);
      } else {
        dest.reserve_at_least(indent_level + 2)
            .append_char_unsafe(',')
            .append_char_unsafe('\n')
            .append_char_unsafe(indent_char, indent_level);
      }
      const auto& elem = get_ith_element<I>(value);
      if (!Derived::operator()(dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.reserve_at_least(indent_level + 2)
        .append_char_unsafe('\n')
        .append_char_unsafe(indent_char, indent_level)
        .append_char(']');
    return true;
  }
};

template <class Derived>
struct unindented_serializer_base {
  template <class CharT, class Allocator, class T>
  static constexpr bool append_range(basic_string_builder<CharT, Allocator>& dest, const T& value) {
    dest.append_char('[');
    auto is_first = true;

    using V = typename T::value_type;
    for (const V& elem : value) {
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      if (!Derived::operator()(dest, elem)) [[unlikely]] {
        return false;
      }
    }
    dest.append_char(']');
    return true;
  }

  template <class CharT, class Allocator, class T>
  static constexpr bool append_map(basic_string_builder<CharT, Allocator>& dest, const T& value) {
    dest.append_char('{');
    auto is_first = true;

    for (const auto& [k, v] : value) {
      auto key_sv = make_string_view(k);
      if (is_first) {
        is_first = false;
        dest.reserve_at_least(6 * key_sv.length() + 3);
      } else {
        dest.reserve_at_least(6 * key_sv.length() + 4).append_char_unsafe(',');  // +1
      }
      dest.append_char_unsafe('"')  // +1
          .append_utf_string_json_escaped_unsafe(k)
          .append_c_string_unsafe("\":");  // +2: '"' and ':'
      if (!Derived::operator()(dest, v)) [[unlikely]] {
        return false;
      }
    }
    dest.append_char('}');
    return true;
  }

  template <class CharT, class Allocator, class T>
  static constexpr bool append_tuple(basic_string_builder<CharT, Allocator>& dest, const T& value) {
    constexpr auto N = std::tuple_size_v<T>;
    dest.append_char('[');

    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      const auto& elem = get_ith_element<I>(value);
      if (!Derived::operator()(dest, elem)) [[unlikely]] {
        return false;
      }
    }
    dest.append_char(']');
    return true;
  }
};
}  // namespace reflect_cpp26::impl::json

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_IMPL_SERIALIZE_TO_JSON_COMMON_HPP
