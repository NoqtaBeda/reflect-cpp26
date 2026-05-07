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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_SERIALIZE_TO_JSON_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_SERIALIZE_TO_JSON_HPP

#include <map>
#include <optional>
#include <reflect_cpp26/enum/enum_contains.hpp>
#include <reflect_cpp26/enum/enum_flags_name.hpp>
#include <reflect_cpp26/enum/enum_name.hpp>
#include <reflect_cpp26/type_operations/impl/serialize_to_json_common.hpp>
#include <reflect_cpp26/type_traits/enum_types.hpp>
#include <reflect_cpp26/type_traits/serializable_types.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/string_utility.hpp>
#include <variant>

namespace reflect_cpp26 {
struct serialize_options {
  bool char_to_string = false;
  bool enum_to_string = false;
  bool halts_on_invalid_enum = false;
  bool halts_on_non_finite_floating_point = false;

  constexpr bool never_halts() const {
    return !halts_on_invalid_enum && !halts_on_non_finite_floating_point;
  }
};

namespace impl::json {
template <bool ToString, class CharT, class Allocator, class T>
static constexpr void serialize_char(basic_string_builder<CharT, Allocator>& dest, T value) {
  if constexpr (ToString) {
    dest.reserve_at_least(8)
        .append_char_unsafe('"')
        .append_utf_code_point_json_escaped_unsafe(static_cast<char32_t>(value))
        .append_char_unsafe('"');
  } else {
    dest.append_integer(value);
  }
}

template <bool HaltsOnInfOrNaN, class CharT, class Allocator, class T>
static constexpr bool serialize_floating_point(basic_string_builder<CharT, Allocator>& dest,
                                               T value) {
  if (std::isnan(value)) [[unlikely]] {
    if constexpr (HaltsOnInfOrNaN) {
      return false;
    } else {
      dest.append_c_string("\"NaN\"");
    }
  } else if (std::isinf(value)) [[unlikely]] {
    if constexpr (HaltsOnInfOrNaN) {
      return false;
    } else if (value > 0) {
      dest.append_c_string("\"Infinity\"");
    } else {
      dest.append_c_string("\"-Infinity\"");
    }
  } else {
    dest.append_floating_point(value);
  }
  return true;
}

template <bool HaltsOnInvalid, class CharT, class Allocator, class T>
constexpr bool stringify_enum_flag(basic_string_builder<CharT, Allocator>& dest, T value) {
  if constexpr (std::is_same_v<CharT, char>) {
    dest.append_char('"');
    if (enum_flags_name_to(dest, value) == to_string_status::done) [[likely]] {
      dest.append_char('"');
    } else {
      dest.unwind_unsafe();
      if constexpr (HaltsOnInvalid) {
        return false;
      } else {
        dest.append_c_string("null");
      }
    }
  } else {
    auto str = enum_flags_name(value);

    if (str.has_value()) [[likely]] {
      if constexpr (enum_names_are_ascii_only_v<T>) {
        dest.reserve_at_least(str->length() + 2)
            .append_char_unsafe('"')
            .append_c_string_unsafe(*str)
            .append_char_unsafe('"');
      } else {
        dest.reserve_at_least(str->length() * 4 + 2)
            .append_char_unsafe('"')
            .append_utf_string_unsafe(*str)
            .append_char_unsafe('"');
      }
    } else if constexpr (HaltsOnInvalid) {
      return false;
    } else {
      dest.append_c_string("null");
    }
  }
  return true;
}

template <bool HaltsOnInvalid, class CharT, class Allocator, class T>
constexpr bool stringify_enum(basic_string_builder<CharT, Allocator>& dest, T value) {
  auto str = enum_name(value);

  if (!str.empty()) [[likely]] {
    if constexpr (enum_names_are_ascii_only_v<T>) {
      dest.reserve_at_least(str.length() + 2)
          .append_char_unsafe('"')
          .append_c_string_unsafe(str)
          .append_char_unsafe('"');
    } else {
      dest.reserve_at_least(4 * str.length() + 2)
          .append_char_unsafe('"')
          .append_utf_string_unsafe(str)
          .append_char_unsafe('"');
    }
    return true;
  } else if constexpr (HaltsOnInvalid) {
    return false;
  } else {
    dest.append_c_string("null");
    return true;
  }
}

template <bool ToString, bool HaltsOnInvalid, class CharT, class Allocator, class T>
constexpr bool serialize_enum(basic_string_builder<CharT, Allocator>& dest, T value) {
  if constexpr (ToString) {
    if constexpr (enum_flag_type<T>) {
      return stringify_enum_flag<HaltsOnInvalid>(dest, value);
    } else {
      return stringify_enum<HaltsOnInvalid>(dest, value);
    }
  } else {
    if constexpr (HaltsOnInvalid) {
      if (!enum_contains<T>(value)) return false;
    }
    dest.append_integer(std::to_underlying(value));
    return true;
  }
}

template <class Parent, class CharT, class Allocator, class T, class... Args>
constexpr bool serializer_dispatch_arithmetic(basic_string_builder<CharT, Allocator>& dest,
                                              const T& value,
                                              const Args&... args) {
  constexpr auto Options = Parent::options;

  if constexpr (std::is_same_v<T, bool>) {
    // (2.1) bool
    dest.append_c_string(value ? "true" : "false");
    return true;
  } else if constexpr (char_type<T>) {
    // (2.2) Character types
    constexpr auto to_string = Options.char_to_string;
    serialize_char<to_string>(dest, value);
    return true;
  } else if constexpr (std::is_integral_v<T>) {
    // (2.3) Integer types
    dest.append_integer(value);
    return true;
  } else if constexpr (std::is_floating_point_v<T>) {
    // (2.4) Floating-point types
    constexpr auto halts_on_inf_or_nan = Options.halts_on_non_finite_floating_point;
    auto res = serialize_floating_point<halts_on_inf_or_nan>(dest, value);
    return res;
  } else {
    static_assert(false, "Invalid or unsupported value type.");
  }
}

template <class Parent, class CharT, class Allocator, class T, class... Args>
constexpr bool serializer_dispatch(basic_string_builder<CharT, Allocator>& dest,
                                   const T& value,
                                   const Args&... args) {
  constexpr auto Options = Parent::options;

  if constexpr (std::is_same_v<T, std::monostate>) {
    // (1) std::monostate (typically used as nullish alternative in std::variant)
    dest.append_c_string("null");
    return true;
  } else if constexpr (std::is_arithmetic_v<T>) {
    // (2) Arithmetic types (see above)
    auto res = serializer_dispatch_arithmetic<Parent>(dest, value, args...);
    return res;
  } else if constexpr (std::is_enum_v<T>) {
    // (3) Enum types
    constexpr auto to_string = Options.enum_to_string;
    constexpr auto halts_on_invalid = Options.halts_on_invalid_enum;
    auto res = serialize_enum<to_string, halts_on_invalid>(dest, value);
    return res;
  } else if constexpr (string_like<T>) {
    // (4) String-like types
    auto str = make_string_view(value);
    dest.reserve_at_least(6 * str.length() + 2)
        .append_char_unsafe('"')
        .append_utf_string_json_escaped_unsafe(str)
        .append_char_unsafe('"');
    return true;
  } else if constexpr (template_instance_of<T, std::optional>) {
    // (7) std::optional
    if (value.has_value()) {
      return Parent::operator()(dest, *value, args...);
    } else {
      dest.append_c_string("null");
      return true;
    }
  } else if constexpr (template_instance_of<T, std::variant>) {
    // (8) std::variant
    auto visit_fn = [&](const auto& v) -> bool { return Parent::operator()(dest, v, args...); };
    if (value.valueless_by_exception()) [[unlikely]] {
      dest.append_c_string("null");
      return true;
    } else {
      return std::visit(visit_fn, value);
    }
  } else if constexpr (template_instance_of<T, std::map>) {
    if constexpr (string_like<typename T::key_type>) {
      // (5.1) std::map<K, V> where K is string-like type: Serialized to JSON object
      return Parent::append_map(dest, value, args...);
    } else {
      // (5.2.1) std::map<K, V> where K is not string-like type: Serialized to JSON nested array
      return Parent::append_range(dest, value, args...);
    }
  } else if constexpr (std::ranges::range<T>) {
    // (5.2.2) Other range types (including C-style arrays)
    return Parent::append_range(dest, value, args...);
  } else if constexpr (tuple_like<T>) {
    // (6) Tuple-like types
    return Parent::append_tuple(dest, value, args...);
  } else if constexpr (flattenable_class<T>) {
    // (9) Flattenable class types (memberwise serializable)
    return Parent::append_struct(dest, value, args...);
  } else {
    static_assert(false, "Invalid or usupported value type");
  }
}

template <serialize_options Options>
struct indented_serializer : indented_serializer_base<indented_serializer<Options>> {
  using self_type = indented_serializer<Options>;

  static constexpr auto options = Options;

  template <class CharT, class Allocator, class T>
  static constexpr bool operator()(basic_string_builder<CharT, Allocator>& dest,
                                   const T& value,
                                   int indent_level,
                                   int indent_size,
                                   CharT indent_char) {
    return serializer_dispatch<self_type>(dest, value, indent_level, indent_size, indent_char);
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
      constexpr auto cur_member = members[I];
      constexpr auto cur_member_name = std::meta::identifier_of(cur_member.member);
      constexpr auto is_ascii_only = is_ascii_string(cur_member_name);
      if constexpr (I > 0) {
        dest.reserve_at_least(indent_level + (is_ascii_only ? 1 : 4) * cur_member_name.length() + 6)
            .append_char_unsafe(',')   // +1
            .append_char_unsafe('\n')  // +1
            .append_char_unsafe(indent_char, indent_level);
      } else {
        dest.reserve_at_least(indent_level + (is_ascii_only ? 1 : 4) * cur_member_name.length() + 5)
            .append_char_unsafe('\n')
            .append_char_unsafe(indent_char, indent_level);
      }
      dest.append_char_unsafe('"');  // +1
      if constexpr (is_ascii_only) {
        dest.append_c_string_unsafe(cur_member_name);
      } else {
        dest.append_utf_string_unsafe(cur_member_name);
      }
      dest.append_c_string_unsafe("\": ");  // +3: '"', ':' and ' '

      const auto& elem = value.[:cur_member.member:];
      if (!operator()(dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.reserve_at_least(indent_level + 2)
        .append_char_unsafe('\n')
        .append_char_unsafe(indent_char, indent_level)
        .append_char_unsafe('}');
    return true;
  }
};

template <serialize_options Options>
struct unindented_serializer : unindented_serializer_base<unindented_serializer<Options>> {
  using self_type = unindented_serializer<Options>;

  static constexpr auto options = Options;

  template <class CharT, class Allocator, class T>
  static constexpr bool operator()(basic_string_builder<CharT, Allocator>& dest, const T& value) {
    return serializer_dispatch<self_type>(dest, value);
  }

  template <class CharT, class Allocator, class T>
  static constexpr bool append_struct(basic_string_builder<CharT, Allocator>& dest,
                                      const T& value) {
    constexpr const auto& members = all_flattened_nonstatic_data_members_v<T>;
    constexpr auto N = std::size(members);
    dest.append_char('{');

    template for (constexpr auto I : std::views::iota(0zU, N)) {
      constexpr auto cur_member = members[I];
      constexpr auto cur_member_name = std::meta::identifier_of(cur_member.member);
      constexpr auto is_ascii_only = is_ascii_string(cur_member_name);
      if constexpr (I > 0) {
        dest.reserve_at_least((is_ascii_only ? 1 : 4) * cur_member_name.length() + 4);
        dest.append_char_unsafe(',');  // +1
      } else {
        dest.reserve_at_least((is_ascii_only ? 1 : 4) * cur_member_name.length() + 3);
      }
      dest.append_char_unsafe('"');  // +1
      if constexpr (is_ascii_only) {
        dest.append_c_string_unsafe(cur_member_name);
      } else {
        dest.append_utf_string_unsafe(cur_member_name);
      }
      dest.append_c_string_unsafe("\":");  // +2: '"' and ':'

      const auto& elem = value.[:cur_member.member:];
      if (!operator()(dest, elem)) [[unlikely]] {
        return false;
      }
    }
    dest.append_char('}');
    return true;
  }
};
}  // namespace impl::json

template <serialize_options Options = {}, class CharT, class Allocator, serializable T>
constexpr bool serialize_to_json(basic_string_builder<CharT, Allocator>& dest, const T& value) {
  return impl::json::unindented_serializer<Options>::operator()(dest, value);
}

template <serialize_options Options = {}, class CharT, class Allocator, serializable T>
constexpr bool serialize_to_json(basic_string_builder<CharT, Allocator>& dest,
                                 const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' ')) {
  return impl::json::indented_serializer<Options>::operator()(
      dest, value, 0, indent_size, indent_char);
}

template <class CharT = char, serialize_options Options = {}, serializable T>
constexpr auto serialize_to_json(const T& value) /* -> (see below) */ {
  auto builder = basic_string_builder<CharT>{};
  if constexpr (Options.never_halts()) {
    // -> std::basic_string<CharT>
    impl::json::unindented_serializer<Options>::operator()(builder, value);
    return builder.str();
  } else {
    // -> std::optional<std::basic_string<CharT>>
    using Ret = std::optional<std::basic_string<CharT>>;
    if (impl::json::unindented_serializer<Options>::operator()(builder, value)) [[likely]] {
      return Ret{builder.str()};
    } else {
      return Ret{std::nullopt};
    }
  }
}

template <class CharT = char, serialize_options Options = {}, serializable T>
constexpr auto serialize_to_json(const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' ')) /* -> (see below) */ {
  auto builder = basic_string_builder<CharT>{};
  if constexpr (Options.never_halts()) {
    // -> std::basic_string<CharT>
    impl::json::indented_serializer<Options>::operator()(
        builder, value, 0, indent_size, indent_char);
    return builder.str();
  } else {
    // -> std::optional<std::basic_string<CharT>>
    using Ret = std::optional<std::basic_string<CharT>>;
    auto ok = impl::json::indented_serializer<Options>::operator()(
        builder, value, 0, indent_size, indent_char);
    if (ok) [[likely]] {
      return Ret{builder.str()};
    } else {
      return Ret{std::nullopt};
    }
  }
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_SERIALIZE_TO_JSON_HPP
