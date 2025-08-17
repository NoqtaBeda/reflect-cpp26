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

#ifndef REFLECT_CPP26_VALIDATORS_MEMBER_RELATION_TAGS_HPP
#define REFLECT_CPP26_VALIDATORS_MEMBER_RELATION_TAGS_HPP

#include <reflect_cpp26/type_operations/member_access.hpp>
#include <reflect_cpp26/type_traits/class_types/member_access.hpp>

namespace reflect_cpp26::validators {
namespace impl {
template <class T>
constexpr auto flattened_nsdm_is_public_array_v = std::define_static_array(
  all_flattened_nsdm_v<T>
    | std::views::transform(&flattened_data_member_info::is_public));

template <class T>
constexpr size_t prev_ith_public_nsdm_index(size_t i, size_t cur_index)
{
  constexpr auto is_public = flattened_nsdm_is_public_array_v<T>;
  for (--cur_index; cur_index != npos; --cur_index) {
    if (!is_public[cur_index]) {
      continue;
    }
    if (--i == 0) { break; }
  }
  return cur_index;
}

template <class T>
constexpr size_t next_ith_public_nsdm_index(size_t i, size_t cur_index)
{
  constexpr auto is_public = flattened_nsdm_is_public_array_v<T>;
  auto n = is_public.size();
  for (++cur_index; cur_index != n; ++cur_index) {
    if (!is_public[cur_index]) {
      continue;
    }
    if (--i == 0) { return cur_index; }
  }
  return npos;
}

struct nsdm_relation_tag_t {};

template <class T>
concept nsdm_relation_tag_type = std::derived_from<T, nsdm_relation_tag_t>;

template <class Derived>
struct nsdm_relation_tag_interfaces : nsdm_relation_tag_t {
  template <size_t CurIndex, partially_flattenable_class T>
  static constexpr decltype(auto) get(const T& obj)
  {
    constexpr auto J = Derived::template get_target_nsdm_index<T>(CurIndex);
    return get_ith_flattened_nsdm<J>(obj);
  }

  template <partially_flattenable_class T>
  static constexpr auto get_name(size_t cur_index) -> std::string_view
  {
    auto j = Derived::template get_target_nsdm_index<T>(cur_index);
    return ith_flattened_nsdm_name<T>(j);
  }
};

template <class Tag, size_t CurIndex, class T>
using target_type_t = ith_flattened_nsdm_type_t<
  Tag::template get_target_nsdm_index<T>(CurIndex), T>;
} // namespace impl

template <size_t I>
  requires (I > 0)
struct prev_ith_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<prev_ith_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t cur_index)
  {
    if (cur_index < I) {
      REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid index");
      return npos;
    }
    return cur_index - I;
  }
};

template <size_t I>
  requires (I > 0)
struct next_ith_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<next_ith_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t cur_index)
  {
    constexpr auto N = all_flattened_nsdm_v<T>.size();
    if (cur_index + I >= N) {
      REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid index");
      return npos;
    }
    return cur_index + I;
  }
};

template <size_t I>
  requires (I > 0)
struct prev_ith_public_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<prev_ith_public_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t cur_index)
  {
    auto res = impl::prev_ith_public_nsdm_index<T>(I, cur_index);
    if (res == npos) {
      REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid index");
    }
    return res;
  }
};

template <size_t I>
  requires (I > 0)
struct next_ith_public_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<next_ith_public_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t cur_index)
  {
    auto res = impl::next_ith_public_nsdm_index<T>(I, cur_index);
    if (res == npos) {
      REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid index");
    }
    return res;
  }
};

template <size_t I>
struct ith_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<ith_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t) {
    return I;
  }
};

template <size_t I>
struct last_ith_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<last_ith_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t)
  {
    static constexpr auto N = all_flattened_nsdm_v<T>.size();
    return N - I - 1;
  }
};

template <size_t I>
struct ith_public_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<ith_public_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t)
  {
    constexpr auto res = public_flattened_nsdm_v<T>[I].index;
    return res;
  }
};

template <size_t I>
struct last_ith_public_nsdm_tag_t
  : impl::nsdm_relation_tag_interfaces<last_ith_public_nsdm_tag_t<I>>
{
  template <partially_flattenable_class T>
  static constexpr size_t get_target_nsdm_index(size_t)
  {
    constexpr auto N = public_flattened_nsdm_v<T>.size();
    constexpr auto res = public_flattened_nsdm_v<T>[N - I - 1].index;
    return res;
  }
};

template <size_t I>
constexpr auto prev_ith_nsdm = prev_ith_nsdm_tag_t<I>{};
template <size_t I>
constexpr auto next_ith_nsdm = next_ith_nsdm_tag_t<I>{};
template <size_t I>
constexpr auto prev_ith_public_nsdm = prev_ith_public_nsdm_tag_t<I>{};
template <size_t I>
constexpr auto next_ith_public_nsdm = next_ith_public_nsdm_tag_t<I>{};
template <size_t I>
constexpr auto ith_nsdm = ith_nsdm_tag_t<I>{};
template <size_t I>
constexpr auto last_ith_nsdm = last_ith_nsdm_tag_t<I>{};
template <size_t I>
constexpr auto ith_public_nsdm = ith_public_nsdm_tag_t<I>{};
template <size_t I>
constexpr auto last_ith_public_nsdm = last_ith_public_nsdm_tag_t<I>{};

constexpr auto prev_nsdm = prev_ith_nsdm<1>;
constexpr auto next_nsdm = next_ith_nsdm<1>;
constexpr auto prev_public_nsdm = prev_ith_public_nsdm<1>;
constexpr auto next_public_nsdm = next_ith_public_nsdm<1>;

constexpr auto first_nsdm = ith_nsdm<0>;
constexpr auto last_nsdm = last_ith_nsdm<0>;
constexpr auto first_public_nsdm = ith_public_nsdm<0>;
constexpr auto last_public_nsdm = last_ith_public_nsdm<0>;
} // reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_MEMBER_RELATION_TAGS_HPP
