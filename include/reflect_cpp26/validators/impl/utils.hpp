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

#ifndef REFLECT_CPP26_VALIDATORS_IMPL_UTILS_HPP
#define REFLECT_CPP26_VALIDATORS_IMPL_UTILS_HPP

#include <reflect_cpp26/type_operations/comparison.hpp>
#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_traits/string_like_types.hpp>

namespace reflect_cpp26::validators::impl {
enum class is_empty_result {
  non_empty,
  empty_string,
  empty_range,
};

template <class R, class T>
concept forward_range_comparable_with = std::ranges::forward_range<R> &&
  generic_equal_comparable_with<std::ranges::range_value_t<R>, T>;

template <class T>
constexpr decltype(auto) to_string_view_or_identity(T&& value)
{
  using TNoCVRef = std::remove_cvref_t<T>;
  if constexpr (is_c_style_string_v<TNoCVRef>) {
    auto sv = std::basic_string_view<char_type_t<TNoCVRef>>{};
    if (value != nullptr) {
      sv = value;
    }
    return sv;
  } else if constexpr (is_string_like_v<TNoCVRef>) {
    return std::basic_string_view<char_type_t<TNoCVRef>>{value};
  } else {
    return std::forward<T>(value);
  }
}

template <class T>
constexpr auto is_empty(const T& value)
{
  if constexpr (is_string_like_v<T>) {
    if constexpr (std::is_pointer_v<T>) {
      auto e = (value == nullptr || *value == '\0');
      return e ? is_empty_result::empty_string : is_empty_result::non_empty;
    } else {
      auto e = std::ranges::empty(value);
      return e ? is_empty_result::empty_string : is_empty_result::non_empty;
    }
  } else {
    auto e = std::ranges::empty(value);
    return e ? is_empty_result::empty_range : is_empty_result::non_empty;
  }
}

template <class T>
constexpr void dump_nsdm(
  std::string* dest, std::string_view member_name, const T& value)
{
  *dest += "member '";
  *dest += member_name;
  if constexpr (is_generic_to_string_invocable_v<T>) {
    *dest += "' which is ";
    *dest += generic_to_display_string(value);
  } else {
    *dest += '\'';
  }
}

template <size_t CurIndex, class RelationTag, class T>
constexpr void dump_nsdm_by_relation_tag(std::string* dest, const T& obj)
{
  auto name = RelationTag::template get_name<T>(CurIndex);
  dump_nsdm(dest, name, RelationTag::template get<CurIndex>(obj));
}
} // namespace reflect_cpp26::validators::impl

#endif // REFLECT_CPP26_VALIDATORS_IMPL_UTILS_HPP
