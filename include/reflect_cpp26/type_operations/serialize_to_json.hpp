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
#include <ranges>
#include <reflect_cpp26/enum/enum_name.hpp>
#include <reflect_cpp26/type_traits/class_types/flattened_nsdm.hpp>
#include <reflect_cpp26/type_traits/serializable_types.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/string_builder.hpp>

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

namespace impl {
template <class T>
struct is_std_map_as_json_object : std::false_type {};

template <class K, class V, class Comp, class Alloc>
struct is_std_map_as_json_object<std::map<K, V, Comp, Alloc>> : std::bool_constant<string_like<K>> {
};

template <bool Indents, serialize_options Options, class CharT, class Allocator, class T>
constexpr bool serialize_to_json_impl(basic_string_builder<CharT, Allocator>& dest,
                                      const T& value,
                                      int indent_level,
                                      int indent_size,
                                      CharT indent_char);

template <bool ToString, class CharT, class Allocator, class T>
constexpr void serialize_char_to_json_impl(basic_string_builder<CharT, Allocator>& dest, T value) {
  if constexpr (ToString) {
    dest.append_char('"');
    if constexpr (sizeof(T) <= sizeof(CharT)) {
      dest.append_char(value);
    } else {
      dest.append_utf_code_point(value);
    }
    dest.append_char('"');
  } else {
    dest.append_integer(value);
  }
}

template <class CharT, class Allocator, class T>
constexpr void serialize_non_char_integral_to_json_impl(
    basic_string_builder<CharT, Allocator>& dest, T value) {
  if constexpr (std::is_same_v<T, bool>) {
    dest.append_bool(value);
  } else if constexpr (std::is_integral_v<T>) {
    dest.append_integer(value);
  } else {
    static_assert(false, "Invalid or usupported value type");
  }
}

template <bool HaltsOnInfOrNaN, class CharT, class Allocator, class T>
constexpr bool serialize_floating_point_to_json_impl(basic_string_builder<CharT, Allocator>& dest,
                                                     T value) {
  if (std::isnan(value)) [[unlikely]] {
    if constexpr (HaltsOnInfOrNaN) {
      return false;
    } else {
      dest.append_utf_string("\"NaN\"");
    }
  } else if (std::isinf(value)) [[unlikely]] {
    if constexpr (HaltsOnInfOrNaN) {
      return false;
    } else if (value > 0) {
      dest.append_utf_string("\"Infinity\"");
    } else {
      dest.append_utf_string("\"-Infinity\"");
    }
  } else {
    dest.append_floating_point(value);
  }
  return true;
}

template <bool ToString, bool HaltsOnInvalid, class CharT, class T>
constexpr bool serialize_enum_to_json_impl(basic_string_builder<CharT>& dest, T value) {
  if constexpr (ToString) {
    auto str = enum_name(value);
    if (!str.empty()) [[likely]] {
      dest.append_char('"').append_utf_string(str).append_char('"');
    } else if constexpr (HaltsOnInvalid) {
      return false;
    } else {
      dest.append_utf_string("null");
    }
  } else {
    dest.append_integer(std::to_underlying(value));
  }
  return true;
}

template <bool Indents, serialize_options Options, class CharT, class Allocator, class T>
constexpr bool serialize_std_map_to_json_impl(basic_string_builder<CharT, Allocator>& dest,
                                              const T& value,
                                              int indent_level,
                                              int indent_size,
                                              CharT indent_char) {
  dest.append_char('{');
  if constexpr (Indents) {
    indent_level += indent_size;
    auto is_first = true;
    for (const auto& [k, v] : value) {
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      dest.append_char('\n').append_char(indent_char, indent_level);
      dest.append_char('"').append_utf_string_json_escaped(k).append_utf_string("\": ");
      if (!serialize_to_json_impl<true, Options>(dest, v, indent_level, indent_size, indent_char))
          [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level);
  } else {
    auto is_first = true;
    for (const auto& [k, v] : value) {
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      dest.append_char('"').append_utf_string_json_escaped(k).append_utf_string("\":");
      if (!serialize_to_json_impl<false, Options>(dest, v, 0, 0, static_cast<CharT>(0)))
          [[unlikely]] {
        return false;
      }
    }
  }
  dest.append_char('}');
  return true;
}

template <bool Indents, serialize_options Options, class CharT, class Allocator, class T>
constexpr bool serialize_range_to_json_impl(basic_string_builder<CharT, Allocator>& dest,
                                            const T& value,
                                            int indent_level,
                                            int indent_size,
                                            CharT indent_char) {
  dest.append_char('[');
  if constexpr (Indents) {
    indent_level += indent_size;
    auto is_first = true;
    for (const auto& elem : value) {
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      dest.append_char('\n').append_char(indent_char, indent_level);
      if (!serialize_to_json_impl<true, Options>(
              dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level);
  } else {
    auto is_first = true;
    for (const auto& elem : value) {
      is_first ? (void)(is_first = false) : (void)(dest.append_char(','));
      if (!serialize_to_json_impl<false, Options>(dest, elem, 0, 0, static_cast<CharT>(0)))
          [[unlikely]] {
        return false;
      }
    }
  }
  dest.append_char(']');
  return true;
}

template <bool Indents, serialize_options Options, class CharT, class Allocator, class T>
constexpr bool serialize_tuple_to_json_impl(basic_string_builder<CharT, Allocator>& dest,
                                            const T& value,
                                            int indent_level,
                                            int indent_size,
                                            CharT indent_char) {
  constexpr auto N = std::tuple_size_v<T>;
  dest.append_char('[');
  if constexpr (Indents) {
    indent_level += indent_size;
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      dest.append_char('\n').append_char(indent_char, indent_level);
      const auto& elem = get_ith_element<I>(value);
      if (!serialize_to_json_impl<true, Options>(
              dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    }
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level).append_char(']');
  } else {
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      if constexpr (I > 0) {
        dest.append_char(',');
      }
      const auto& elem = get_ith_element<I>(value);
      if (!serialize_to_json_impl<false, Options>(dest, elem, 0, 0, static_cast<CharT>(0)))
          [[unlikely]] {
        return false;
      }
    }
    dest.append_char(']');
  }
  return true;
}

template <bool Indents, serialize_options Options, class CharT, class Allocator, class T>
constexpr bool serialize_struct_to_json_impl(basic_string_builder<CharT, Allocator>& dest,
                                             const T& value,
                                             int indent_level,
                                             int indent_size,
                                             CharT indent_char) {
  constexpr auto members = all_flattened_nonstatic_data_members_v<T>;
  constexpr auto N = members.size();

  indent_level += indent_size;
  dest.append_char('{');
  template for (constexpr auto I : std::views::iota(0zU, N)) {
    if constexpr (I > 0) {
      dest.append_char(',');
    }
    if constexpr (Indents) {
      dest.append_char('\n').append_char(indent_char, indent_level);
    }
    constexpr auto cur_member = members[I];
    if constexpr (has_identifier(cur_member.member)) {
      dest.append_char('"').append_utf_string(identifier_of(cur_member.member)).append_char('"');
    } else {
      dest.append_integer(I);
    }
    const auto& elem = value.[:cur_member.member:];
    if constexpr (Indents) {
      dest.append_utf_string(": ");
      if (!serialize_to_json_impl<true, Options>(
              dest, elem, indent_level, indent_size, indent_char)) [[unlikely]] {
        return false;
      }
    } else {
      dest.append_char(':');
      if (!serialize_to_json_impl<false, Options>(dest, elem, 0, 0, static_cast<CharT>(0)))
          [[unlikely]] {
        return false;
      }
    }
  }
  if constexpr (Indents) {
    indent_level -= indent_size;
    dest.append_char('\n').append_char(indent_char, indent_level);
  }
  dest.append_char('}');
  return true;
}

template <bool Indents, serialize_options Options, class CharT, class Allocator, class T>
constexpr bool serialize_to_json_impl(basic_string_builder<CharT, Allocator>& dest,
                                      const T& value,
                                      int indent_level,
                                      int indent_size,
                                      CharT indent_char) {
  if constexpr (std::is_same_v<T, std::monostate>) {
    // (1) std::monostate
    dest.append_utf_string("null");
    return true;
  } else if constexpr (char_type<T>) {
    // (2.1) Character types
    constexpr auto to_string = Options.char_to_string;
    serialize_char_to_json_impl<to_string>(dest, value);
    return true;
  } else if constexpr (std::is_integral_v<T>) {
    // (2.2) Integral types (except chars)
    serialize_non_char_integral_to_json_impl(dest, value);
    return true;
  } else if constexpr (std::is_floating_point_v<T>) {
    // (2.3) Floating-point types
    constexpr auto halts_on_inf_or_nan = Options.halts_on_non_finite_floating_point;
    return serialize_floating_point_to_json_impl<halts_on_inf_or_nan>(dest, value);
  } else if constexpr (std::is_enum_v<T>) {
    // (3) Enum types
    constexpr auto to_string = Options.enum_to_string;
    constexpr auto halts_on_invalid = to_string && Options.halts_on_invalid_enum;
    return serialize_enum_to_json_impl<to_string, halts_on_invalid>(dest, value);
  } else if constexpr (string_like<T>) {
    // (4) String-like types
    dest.append_char('"').append_utf_string_json_escaped(value).append_char('"');
    return true;
  } else if constexpr (template_instance_of<T, std::optional>) {
    // (7) std::optional
    //     Note: we check std::optional before range types
    //           since std::optional is also a range (whose size is 0 or 1)
    if (value.has_value()) {
      return serialize_to_json_impl<Indents, Options>(
          dest, *value, indent_level, indent_size, indent_char);
    } else {
      dest.append_utf_string("null");
      return true;
    }
  } else if constexpr (template_instance_of<T, std::variant>) {
    // (8) std::variant
    auto visit_fn = [&](const auto& v) -> bool {
      return serialize_to_json_impl<Indents, Options>(
          dest, v, indent_level, indent_size, indent_char);
    };
    if (value.valueless_by_exception()) [[unlikely]] {
      dest.append_utf_string("null");
      return true;
    } else {
      return std::visit(visit_fn, value);
    }
  } else if constexpr (impl::is_std_map_as_json_object<T>::value) {
    // (5.1) std::map<K, V> where K is string-like type:
    //       Serialized to JSON object
    return serialize_std_map_to_json_impl<Indents, Options>(
        dest, value, indent_level, indent_size, indent_char);
  } else if constexpr (std::ranges::range<T>) {
    // (5.2) Other range types (including C-style arrays)
    return serialize_range_to_json_impl<Indents, Options>(
        dest, value, indent_level, indent_size, indent_char);
  } else if constexpr (tuple_like<T>) {
    // (6) Tuple-like types
    return serialize_tuple_to_json_impl<Indents, Options>(
        dest, value, indent_level, indent_size, indent_char);
  } else if constexpr (flattenable_class<T>) {
    // (9) Flattenable class types (memberwise serializable)
    return serialize_struct_to_json_impl<Indents, Options>(
        dest, value, indent_level, indent_size, indent_char);
  } else {
    static_assert(false, "Invalid or usupported value type");
  }
}
}  // namespace impl

template <serialize_options Options = {}, class CharT, class Allocator, memberwise_serializable T>
constexpr bool serialize_to_json(basic_string_builder<CharT, Allocator>& dest, const T& value) {
  return impl::serialize_to_json_impl<false, Options>(dest, value, 0, 0, static_cast<CharT>(0));
}

template <serialize_options Options = {}, class CharT, class Allocator, memberwise_serializable T>
constexpr bool serialize_to_json(basic_string_builder<CharT, Allocator>& dest,
                                 const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' ')) {
  return impl::serialize_to_json_impl<true, Options>(dest, value, 0, indent_size, indent_char);
}

template <class CharT = char, serialize_options Options = {}, memberwise_serializable T>
constexpr auto serialize_to_json(const T& value) /* -> (see below) */ {
  auto builder = basic_string_builder<CharT>{};
  if constexpr (Options.never_halts()) {
    // -> std::basic_string<CharT>
    impl::serialize_to_json_impl<false, Options>(builder, value, 0, 0, static_cast<CharT>(0));
    return builder.str();
  } else {
    // -> std::optional<std::basic_string<CharT>>
    using Ret = std::optional<std::basic_string<CharT>>;
    if (impl::serialize_to_json_impl<false, Options>(builder, value, 0, 0, static_cast<CharT>(0))) {
      return Ret{builder.str()};
    } else [[unlikely]] {
      return Ret{std::nullopt};
    }
  }
}

template <class CharT = char, serialize_options Options = {}, memberwise_serializable T>
constexpr auto serialize_to_json(const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' ')) /* -> (see below) */ {
  auto builder = basic_string_builder<CharT>{};
  if constexpr (Options.never_halts()) {
    // -> std::basic_string<CharT>
    impl::serialize_to_json_impl<true, Options>(builder, value, 0, indent_size, indent_char);
    return builder.str();
  } else {
    // -> std::optional<std::basic_string<CharT>>
    using Ret = std::optional<std::basic_string<CharT>>;
    if (impl::serialize_to_json_impl<true, Options>(builder, value, 0, indent_size, indent_char)) {
      return Ret{builder.str()};
    } else [[unlikely]] {
      return Ret{std::nullopt};
    }
  }
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_SERIALIZE_TO_JSON_HPP
