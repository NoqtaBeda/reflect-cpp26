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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_DUMP_TO_JSON_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_DUMP_TO_JSON_HPP

#include <map>
#include <optional>
#include <reflect_cpp26/enum/enum_flags_name.hpp>
#include <reflect_cpp26/enum/enum_name.hpp>
#include <reflect_cpp26/enum/enum_type_name.hpp>
#include <reflect_cpp26/type_operations/impl/serialize_to_json_common.hpp>
#include <reflect_cpp26/type_traits/enum_types.hpp>
#include <reflect_cpp26/type_traits/serializable_types.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/addressable_member.hpp>
#include <variant>

namespace reflect_cpp26 {
namespace impl::json {
template <class CharT, class Allocator, class T>
constexpr void dump_char(basic_string_builder<CharT, Allocator>& dest, T value) {
  dest.append_char('\'');
  dest.template append_char_by<escaping_mode::display_char>(value);
  dest.append_char('\'');
}

template <class CharT, class Allocator, class T>
constexpr void dump_enum(basic_string_builder<CharT, Allocator>& dest, T value) {
  if constexpr (enum_flag<T>) {
    auto str = enum_flags_name(value);
    if (str.has_value()) {
      dest.append_utf_string(*str);
    } else {
      dest.append_char('(')
          .append_utf_string(enum_type_name_v<T>)
          .append_char(')')
          .append_integer(std::to_underlying(value));
    }
  } else {
    auto str = enum_name(value);
    if (!str.empty()) {
      dest.append_utf_string(str);
    } else {
      dest.append_char('(')
          .append_utf_string(enum_type_name_v<T>)
          .append_char(')')
          .append_integer(std::to_underlying(value));
    }
  }
}

template <class CharT, class Allocator, class T>
constexpr void dumper_dispatch_arithmetic(basic_string_builder<CharT, Allocator>& dest,
                                          const T& value) {
  if constexpr (std::is_same_v<T, bool>) {
    // (2.1) bool
    dest.append_utf_string(value ? "true" : "false");
  } else if constexpr (char_type<T>) {
    // (2.2) Character types
    dump_char(dest, value);
  } else if constexpr (std::is_integral_v<T>) {
    // (2.3) Integer types
    dest.append_integer(value);
  } else if constexpr (std::is_floating_point_v<T>) {
    // (2.4) Floating-point types
    dest.append_floating_point(value);
  } else {
    static_assert(false, "Invalid or unsupported value type.");
  }
}

// pointers can not be dumped in constant evaluation
template <class CharT, class Allocator>
void dump_pointer(basic_string_builder<CharT, Allocator>& dest, const volatile void* value) {
  if (value == nullptr) {
    dest.append_utf_string("nullptr");
  } else {
    auto p = reinterpret_cast<uintptr_t>(value);
    dest.append_utf_string("0x").append_integer(p, 16);
  }
}

// pointers can not be dumped in constant evaluation
template <class CharT, class Allocator, class MemT, class ClassT>
void dump_pointer_to_member(basic_string_builder<CharT, Allocator>& dest, MemT ClassT::* value) {
  if (value == nullptr) {
    dest.append_utf_string("nullptr");
    return;
  }
  if constexpr (std::is_function_v<MemT>) {
    // todo
  } else if constexpr (partially_flattenable_class<ClassT>) {
    auto offset = *reinterpret_cast<uintptr_t*>(&value);
    constexpr const auto& all_members = all_flattened_nonstatic_data_members_v<ClassT>;
    template for (constexpr auto M : all_members) {
      constexpr auto is_candidate =
          is_same_type(M.type(), ^^MemT) && is_addressable_class_member(M.member);

      if constexpr (is_candidate) {
        if (M.actual_offset_bytes() != offset) {
          continue;  // Continues template-for
        }
        dest.append_char('&')
            .append_utf_string(display_string_of(parent_of(M.member)))
            .append_char(':', 2)
            .append_utf_string(identifier_of(M.member));
        return;
      }
    }
  } else {
    dest.append_utf_string("(...)");
    return;
  }
  // Mismatch
  dest.append_utf_string("(malformed pointer-to-member)");
}

template <class Parent, class CharT, class Allocator, class T, class... Args>
constexpr void dumper_dispatch(basic_string_builder<CharT, Allocator>& dest,
                               const T& value,
                               const Args&... args) {
  if constexpr (std::is_same_v<T, std::monostate>) {
    // (1) std::monostate
    dest.append_utf_string("null");
  } else if constexpr (std::is_arithmetic_v<T>) {
    // (2) arithmetic types
    dumper_dispatch_arithmetic(dest, value);
  } else if constexpr (std::is_enum_v<T>) {
    // (3) Enum types
    dump_enum(dest, value);
  } else if constexpr (string_like<T>) {
    // (4) String-like types
    auto str = std::basic_string_view{value};
    dest.append_char('"');
    dest.template append_utf_string_by<escaping_mode::display_string>(str);
    dest.append_char('"');
  } else if constexpr (std::is_pointer_v<T>) {
    // (4) Pointer types. Put after string branch as const char* shall be dumped as string.
    dump_pointer(dest, value);
  } else if constexpr (std::is_member_pointer_v<T>) {
    // (5) Pointer to members
    dump_pointer_to_member(dest, value);
  } else if constexpr (template_instance_of<T, std::optional>) {
    // (7) std::optional
    if (value.has_value()) {
      Parent::operator()(dest, *value, args...);
    } else {
      dest.append_utf_string("nullopt");
    }
  } else if constexpr (template_instance_of<T, std::variant>) {
    // (8) std::variant
    auto visit_fn = [&](const auto& v) -> void { Parent::operator()(dest, v, args...); };
    if (value.valueless_by_exception()) [[unlikely]] {
      dest.append_utf_string("(valueless by exception)");
    } else {
      std::visit(visit_fn, value);
    }
  } else if constexpr (template_instance_of<T, std::map>) {
    if constexpr (string_like<typename T::key_type>) {
      // (5.1) std::map<K, V> where K is string-like type: Serialized to JSON object
      Parent::append_map(dest, value, args...);
    } else {
      // (5.2.1) std::map<K, V> where K is not string-like type: Serialized to JSON nested array
      Parent::append_range(dest, value, args...);
    }
  } else if constexpr (std::ranges::range<T>) {
    // (5.2.2) Other range types (including C-style arrays)
    Parent::append_range(dest, value, args...);
  } else if constexpr (tuple_like<T>) {
    // (6) Tuple-like types
    Parent::append_tuple(dest, value, args...);
  } else if constexpr (partially_flattenable_class<T>) {
    // (9) Partially flattenable class types
    Parent::append_struct(dest, value, args...);
  } else {
    // Others
    dest.append_utf_string("(...)");
  }
}

struct indented_dumper : indented_serializer_base<indented_dumper> {
  static constexpr auto quotes_field_name = false;

  template <class CharT, class Allocator, class T>
  static constexpr bool operator()(basic_string_builder<CharT, Allocator>& dest,
                                   const T& value,
                                   int indent_level,
                                   int indent_size,
                                   CharT indent_char) {
    dumper_dispatch<indented_dumper>(dest, value, indent_level, indent_size, indent_char);
    return true;
  }
};

struct unindented_dumper : unindented_serializer_base<unindented_dumper> {
  static constexpr auto quotes_field_name = false;

  template <class CharT, class Allocator, class T>
  static constexpr bool operator()(basic_string_builder<CharT, Allocator>& dest, const T& value) {
    dumper_dispatch<unindented_dumper>(dest, value);
    return true;
  }
};
}  // namespace impl::json

template <class CharT, class Allocator, class T>
constexpr bool dump_to_json_like(basic_string_builder<CharT, Allocator>& dest, const T& value) {
  return impl::json::unindented_dumper::operator()(dest, value);
}

template <class CharT, class Allocator, class T>
constexpr bool dump_to_json_like(basic_string_builder<CharT, Allocator>& dest,
                                 const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' ')) {
  return impl::json::indented_dumper::operator()(dest, value, 0, indent_size, indent_char);
}

template <class CharT = char, class T>
constexpr auto dump_to_json_like(const T& value) -> std::basic_string<CharT> {
  auto builder = basic_string_builder<CharT>{};
  impl::json::unindented_dumper::operator()(builder, value);
  return builder.str();
}

template <class CharT = char, class T>
constexpr auto dump_to_json_like(const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' '))
    -> std::basic_string<CharT> {
  auto builder = basic_string_builder<CharT>{};
  impl::json::indented_dumper::operator()(builder, value, 0, indent_size, indent_char);
  return builder.str();
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_DUMP_TO_JSON_HPP
