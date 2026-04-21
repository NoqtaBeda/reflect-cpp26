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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_DUMP_TO_JSON_LIKE_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_DUMP_TO_JSON_LIKE_HPP

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
#include <reflect_cpp26/utils/string_utils.hpp>
#include <variant>

namespace reflect_cpp26 {
namespace impl::json {
struct field_name_item {
  std::string_view name;
  size_t index;
};

// Note: uint8_t is used to avoid std::vector<bool> specialization
consteval auto make_name_collision_table(std::span<const flattened_data_member_info> members)
    -> meta_span<uint8_t> {
  auto items = std::vector<field_name_item>{};
  auto n = members.size();

  items.reserve(n);
  for (auto i = 0zU; i < n; i++) {
    auto name = identifier_of(members[i].member);
    items.push_back({.name = name, .index = i});
  }
  std::ranges::sort(items, {}, &field_name_item::name);

  auto res = std::vector<uint8_t>(n);
  for (auto pos = 0zU; pos + 1 < n;) {
    if (items[pos + 1].name != items[pos].name) {
      pos += 1;
      continue;
    }
    res[items[pos].index] = true;
    res[items[pos + 1].index] = true;
    auto next = pos + 2;
    while (next < n && items[next].name == items[pos].name) {
      res[items[next].index] = true;
      next += 1;
    }
    pos = next;
  }
  return reflect_cpp26::define_static_array(res);
}

template <class T>
consteval auto make_name_collision_table() -> meta_span<uint8_t> {
  return make_name_collision_table(all_flattened_nonstatic_data_members_v<T>);
}

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

// precondition: value != nullptr
// pointers can not be dumped in constant evaluation
template <class CharT, class Allocator, class MemT, class ClassT>
void dump_pointer_to_member_function(basic_string_builder<CharT, Allocator>& dest,
                                     MemT ClassT::* value) {
  dest.append_utf_string("(...)");  // TODO: unimplemented yet
}

// precondition: value != nullptr
// pointers can not be dumped in constant evaluation
template <class CharT, class Allocator, class MemT, class ClassT>
void dump_pointer_to_data_member(basic_string_builder<CharT, Allocator>& dest,
                                 MemT ClassT::* value) {
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
  // Mismatch
  dest.append_utf_string("(malformed: 0x")
      .append_integer(*reinterpret_cast<uintptr_t*>(&value), 16)
      .append_char(')');
}

// pointers can not be dumped in constant evaluation
template <class CharT, class Allocator, class MemT, class ClassT>
void dump_pointer_to_member(basic_string_builder<CharT, Allocator>& dest, MemT ClassT::* value) {
  if (value == nullptr) {
    dest.append_utf_string("nullptr");
    return;
  }
  if constexpr (!partially_flattenable_class<ClassT>) {
    dest.append_utf_string("(...)");
  } else if constexpr (std::is_function_v<MemT>) {
    dump_pointer_to_member_function(dest, value);
  } else {
    dump_pointer_to_data_member(dest, value);
  }
}

template <class Parent, class CharT, class Allocator, class T, class... Args>
constexpr void dumper_dispatch(basic_string_builder<CharT, Allocator>& dest,
                               const T& value,
                               const Args&... args) {
  if constexpr (std::is_same_v<T, std::monostate>) {
    // (1) std::monostate
    dest.append_utf_string("monostate");
  } else if constexpr (std::is_arithmetic_v<T>) {
    // (2) arithmetic types
    dumper_dispatch_arithmetic(dest, value);
  } else if constexpr (std::is_enum_v<T>) {
    // (3) Enum types
    dump_enum(dest, value);
  } else if constexpr (string_like<T>) {
    // (4) String-like types
    auto str = make_string_view(value);
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
  template <class CharT, class Allocator, class T>
  static constexpr bool operator()(basic_string_builder<CharT, Allocator>& dest,
                                   const T& value,
                                   int indent_level,
                                   int indent_size,
                                   CharT indent_char) {
    dumper_dispatch<indented_dumper>(dest, value, indent_level, indent_size, indent_char);
    return true;
  }

  template <class CharT, class Allocator, class T>
  static constexpr void append_struct(basic_string_builder<CharT, Allocator>& dest,
                                      const T& value,
                                      int indent_level,
                                      int indent_size,
                                      CharT indent_char) {
    constexpr const auto& members = all_flattened_nonstatic_data_members_v<T>;
    constexpr auto N = std::size(members);
    constexpr auto has_name_collision = make_name_collision_table<T>();

    dest.append_char('{');
    indent_level += indent_size;

    template for (constexpr auto I : std::views::iota(0zU, N)) {
      constexpr auto M = members[I];
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      dest.append_char('\n').append_char(indent_char, indent_level);
      if constexpr (has_name_collision[I]) {
        dest.append_utf_string(display_string_of(parent_of(M.member)));
        dest.append_char(':', 2);
      }
      dest.append_utf_string(identifier_of(M.member));
      dest.append_utf_string(": ");

      const auto& elem = value.[:M.member:];
      if constexpr (is_reference_type(type_of(M.member))) {
        // References should be dumped as pointers in case of loop reference
        dump_pointer(dest, &elem);
      } else {
        operator()(dest, elem, indent_level, indent_size, indent_char);
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level).append_char('}');
  }
};

struct unindented_dumper : unindented_serializer_base<unindented_dumper> {
  template <class CharT, class Allocator, class T>
  static constexpr bool operator()(basic_string_builder<CharT, Allocator>& dest, const T& value) {
    dumper_dispatch<unindented_dumper>(dest, value);
    return true;
  }

  template <class CharT, class Allocator, class T>
  static constexpr void append_struct(basic_string_builder<CharT, Allocator>& dest,
                                      const T& value) {
    constexpr const auto& members = all_flattened_nonstatic_data_members_v<T>;
    constexpr auto N = std::size(members);
    constexpr auto has_name_collision = make_name_collision_table<T>();

    dest.append_char('{');
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      constexpr auto M = members[I];
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      if constexpr (has_name_collision[I]) {
        dest.append_utf_string(display_string_of(parent_of(M.member)));
        dest.append_char(':', 2);
      }
      dest.append_utf_string(identifier_of(M.member));
      dest.append_char(':');

      const auto& elem = value.[:M.member:];
      if constexpr (is_reference_type(type_of(M.member))) {
        // References should be dumped as pointers in case of loop reference
        dump_pointer(dest, &elem);
      } else {
        operator()(dest, elem);
      }
    }
    dest.append_char('}');
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

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_DUMP_TO_JSON_LIKE_HPP
