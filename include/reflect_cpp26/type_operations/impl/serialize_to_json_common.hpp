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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_IMPL_SERIALIZE_COMMON_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_IMPL_SERIALIZE_COMMON_HPP

#include <ranges>
#include <reflect_cpp26/type_traits/class_types/flattened_nsdm.hpp>
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
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      dest.append_char('\n').append_char(indent_char, indent_level);
      if (!Derived::operator()(dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level).append_char(']');
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
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      dest.append_char('\n').append_char(indent_char, indent_level);
      dest.append_char('"').append_utf_string_json_escaped(k).append_utf_string("\": ");
      if (!Derived::operator()(dest, v, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level).append_char('}');
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
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      dest.append_char('\n').append_char(indent_char, indent_level);
      const auto& elem = get_ith_element<I>(value);
      if (!Derived::operator()(dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level).append_char(']');
    return true;
  }

  template <class CharT, class Allocator, class T>
  static constexpr bool append_struct(basic_string_builder<CharT, Allocator>& dest,
                                      const T& value,
                                      int indent_level,
                                      int indent_size,
                                      CharT indent_char) {
    constexpr const auto& members = all_flattened_nonstatic_data_members_v<T>;
    constexpr auto N = std::size(members);
    dest.append_char('{');
    indent_level += indent_size;

    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      dest.append_char('\n').append_char(indent_char, indent_level);
      constexpr auto cur_member = members[I];
      if constexpr (Derived::quotes_field_name) {
        dest.append_char('"').append_utf_string(identifier_of(cur_member.member)).append_char('"');
      } else {
        dest.append_utf_string(identifier_of(cur_member.member));
      }
      dest.append_utf_string(": ");
      const auto& elem = value.[:cur_member.member:];
      if (!Derived::operator()(dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level).append_char('}');
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
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      dest.append_char('"').append_utf_string_json_escaped(k).append_utf_string("\":");
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

  template <class CharT, class Allocator, class T>
  static constexpr bool append_struct(basic_string_builder<CharT, Allocator>& dest,
                                      const T& value) {
    constexpr const auto& members = all_flattened_nonstatic_data_members_v<T>;
    constexpr auto N = std::size(members);
    dest.append_char('{');

    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      constexpr auto cur_member = members[I];
      if constexpr (Derived::quotes_field_name) {
        dest.append_char('"').append_utf_string(identifier_of(cur_member.member)).append_char('"');
      } else {
        dest.append_utf_string(identifier_of(cur_member.member));
      }
      dest.append_char(':');
      const auto& elem = value.[:cur_member.member:];
      if (!Derived::operator()(dest, elem)) [[unlikely]] {
        return false;
      }
    }
    dest.append_char('}');
    return true;
  }
};
}  // namespace reflect_cpp26::impl::json

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_IMPL_SERIALIZE_COMMON_HPP
