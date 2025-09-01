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

#ifndef REFLECT_CPP26_UTILS_TAGS_HPP
#define REFLECT_CPP26_UTILS_TAGS_HPP

#include <reflect_cpp26/type_traits/type_comparison.hpp>

namespace reflect_cpp26 {
struct case_insensitive_tag_t {};
struct case_insensitive_by_ascii_tag_t {};

[[deprecated]]
constexpr auto case_insensitive = case_insensitive_tag_t{};
constexpr auto case_insensitive_by_ascii = case_insensitive_by_ascii_tag_t{};

struct nonstatic_data_members_only_tag_t {};
struct nonstatic_member_functions_only_tag_t {};
struct static_data_members_only_tag_t {};
struct static_member_functions_only_tag_t {};

struct functions_only_tag_t {};
struct variables_only_tag_t {};

template <class T>
concept class_member_filter_tag = same_as_one_of<T,
  nonstatic_data_members_only_tag_t,
  nonstatic_member_functions_only_tag_t,
  static_data_members_only_tag_t,
  static_member_functions_only_tag_t>;

template <class T>
concept non_class_member_filter_tag = same_as_one_of<T,
  functions_only_tag_t,
  variables_only_tag_t>;

constexpr auto nonstatic_data_members_only =
  nonstatic_data_members_only_tag_t{};
constexpr auto nonstatic_member_functions_only =
  nonstatic_member_functions_only_tag_t{};
constexpr auto static_data_members_only =
  static_data_members_only_tag_t{};
constexpr auto static_member_functions_only =
  static_member_functions_only_tag_t{};

constexpr auto functions_only = functions_only_tag_t{};
constexpr auto variables_only = variables_only_tag_t{};

template <class_member_filter_tag FilterTag>
constexpr auto is_static_members_only(FilterTag)
{
  return same_as_one_of<
    FilterTag,
    static_data_members_only_tag_t,
    static_member_functions_only_tag_t>;
}

template <class_member_filter_tag FilterTag>
constexpr auto is_functions_only(FilterTag)
{
  return same_as_one_of<
    FilterTag,
    nonstatic_member_functions_only_tag_t,
    static_member_functions_only_tag_t>;
}

template <non_class_member_filter_tag FilterTag>
constexpr auto is_functions_only(FilterTag)
{
  return same_as<FilterTag, functions_only_tag_t>;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_TAGS_HPP
