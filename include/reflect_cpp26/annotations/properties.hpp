/**
 * Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
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

#ifndef REFLECT_CPP26_ANNOTATIONS_PROPERTIES_HPP
#define REFLECT_CPP26_ANNOTATIONS_PROPERTIES_HPP

#include <reflect_cpp26/annotations/impl/lookup.hpp>
#include <reflect_cpp26/annotations/tags.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <concepts>
#include <ranges>

namespace reflect_cpp26::annotations {
template <class Annotation>
using property_value_type_t = decltype(std::declval<Annotation>().value);

struct aliases_t : property_tag_t {
  meta_span<meta_string_view> value;
};
struct arg_notation_t : property_tag_t {
  char value;
};
struct author_t : property_tag_t {
  meta_string_view value;
};
struct description_t : property_tag_t {
  meta_string_view value;
};
struct rename_t : property_tag_t {
  meta_string_view value;
};
struct version_t : property_tag_t {
  meta_string_view value;
};
struct version_since_t : property_tag_t {
  meta_string_view value;
};

template <class Prop>
struct make_property_t {
  using value_type = property_value_type_t<Prop>;

  static consteval auto operator()(value_type value) -> Prop {
    return Prop{.value = value};
  }

  template <class T>
    requires (std::is_same_v<to_structured_t<T>, value_type>)
  static consteval auto operator()(const T& value) -> Prop {
    return Prop{.value = to_structured(value)};
  }
};

template <class Prop>
struct make_range_property_t : make_property_t<Prop> {
  using base_type = make_property_t<Prop>;
  using base_type::operator();

  using value_type = base_type::value_type;
  using range_value_type = std::ranges::range_value_t<value_type>;

  template <class T>
    requires (std::is_same_v<to_structured_t<T>, range_value_type>)
  static consteval auto operator()(std::initializer_list<T> values) -> Prop
  {
    auto span = to_structured(values);
    return Prop{.value = span};
  }
};

struct make_aliases_t : make_range_property_t<aliases_t> {
  using base_type = make_range_property_t<aliases_t>;
  using base_type::operator();

  template <class T>
    requires (std::is_same_v<to_structured_t<T>, meta_string_view>)
  static consteval auto operator()(const T& value) -> aliases_t
  {
    auto span = to_structured(std::views::single(value));
    return aliases_t{.value = span};
  }
};

constexpr auto make_alias = make_aliases_t{};
constexpr auto make_aliases = make_aliases_t{};
constexpr auto make_arg_notation = make_property_t<arg_notation_t>{};
constexpr auto make_author = make_property_t<author_t>{};
constexpr auto make_description = make_property_t<description_t>{};
constexpr auto make_rename = make_property_t<rename_t>{};
constexpr auto make_version = make_property_t<version_t>{};
constexpr auto make_version_since = make_property_t<version_since_t>{};

template <class Prop, auto MemPtr>
consteval auto property_of_alt(property_value_type_t<Prop> alt = {})
  -> property_value_type_t<Prop>
{
  constexpr auto meta = impl::find_annotation_of_type(
    ^^Prop, reflect_pointer_to_member(MemPtr));
  if constexpr (std::meta::info{} == meta) {
    return alt;
  }
  return extract<Prop>(meta).value;
}

template <class Prop, auto MemPtr>
constexpr auto property_of_opt() -> std::optional<property_value_type_t<Prop>>
{
  constexpr auto meta = impl::find_annotation_of_type(
    ^^Prop, reflect_pointer_to_member(MemPtr));
  if constexpr (std::meta::info{} == meta) {
    return std::nullopt;
  }
  return extract<Prop>(meta).value;
}

template <auto MemPtr>
constexpr auto aliases_of() -> meta_span<meta_string_view> {
  return property_of_alt<aliases_t, MemPtr>();
}

template <auto MemPtr>
constexpr auto arg_notation_of() -> char {
  return property_of_alt<arg_notation_t, MemPtr>();
}

template <auto MemPtr>
constexpr auto author_of() -> meta_string_view {
  return property_of_alt<author_t, MemPtr>();
}

template <auto MemPtr>
constexpr auto description_of() -> meta_string_view {
  return property_of_alt<description_t, MemPtr>();
}

template <auto MemPtr>
constexpr auto rename_of() -> meta_string_view {
  return property_of_alt<rename_t, MemPtr>();
}

template <auto MemPtr>
constexpr auto version_of() -> meta_string_view {
  return property_of_alt<version_t, MemPtr>();
}

template <auto MemPtr>
constexpr auto version_since_of() -> meta_string_view {
  return property_of_alt<version_since_t, MemPtr>();
}
} // namespace reflect_cpp26::annotations

#endif // REFLECT_CPP26_ANNOTATIONS_PROPERTIES_HPP
