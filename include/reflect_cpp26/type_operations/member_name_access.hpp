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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_NAME_ACCESS_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_NAME_ACCESS_HPP

#include <reflect_cpp26/type_traits/class_types/flattened_nsdm.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
namespace impl {
template <class T, std::meta::info ArrV>
constexpr auto make_direct_nsdm_name_array()
/* std::array<std::string_view, N> */
{
  constexpr auto members = extract<substitute(ArrV, ^^T)>();
  constexpr auto N = members.size();
  auto res = std::array<std::string_view, N>{};
  for (auto i = 0zU; i < N; i++) {
    res[i] = reflect_cpp26::identifier_of(members[i]);
  }
  return res;
}

template <class T, std::meta::info ArrV>
constexpr auto make_flattened_nsdm_name_array()
/* std::array<std::string_view, N> */
{
  constexpr auto members = extract<substitute(ArrV, ^^T)>();
  constexpr auto N = members.size();
  auto res = std::array<std::string_view, N>{};
  for (auto i = 0zU; i < N; i++) {
    res[i] = reflect_cpp26::identifier_of(members[i].member);
  }
  return res;
}

template <class T, std::meta::info ArrV>
constexpr auto direct_nsdm_names_v = make_direct_nsdm_name_array<T, ArrV>();

template <class T, std::meta::info ArrV>
constexpr auto flattened_nsdm_names_v = make_flattened_nsdm_name_array<T, ArrV>();
}  // namespace impl

/**
 * Gets the name list of all the non-static data members defined directly in
 * class or union T, consistent to member definition order.
 * Anonymous members are named as empty string "" in the list.
 */
template <class_or_union_type T>
constexpr auto direct_nonstatic_data_member_names_v = std::span{
    impl::direct_nsdm_names_v<std::remove_cv_t<T>, ^^all_direct_nonstatic_data_members_v>};

/**
 * Gets the name list of non-static data members defined directly in
 * class or union T with public access, consistent to member definition order.
 * Anonymous members are named as empty string "" in the list.
 */
template <class_or_union_type T>
constexpr auto public_direct_nonstatic_data_member_names_v = std::span{
    impl::direct_nsdm_names_v<std::remove_cv_t<T>, ^^public_direct_nonstatic_data_members_v>};

/**
 * Gets the name list of all the non-static data members flattened from class T,
 * consistent to member definition order.
 * Anonymous members are named as empty string "" in the list.
 */
template <partially_flattenable_class T>
constexpr auto nonstatic_data_member_names_v = std::span{
    impl::flattened_nsdm_names_v<std::remove_cv_t<T>, ^^all_flattened_nonstatic_data_members_v>};

/**
 * Gets the name list of non-static data members flattened from class T with
 * public access, consistent to member definition order.
 * Anonymous members are named as empty string "" in the list.
 */
template <partially_flattenable_class T>
constexpr auto public_nonstatic_data_member_names_v = std::span{
    impl::flattened_nsdm_names_v<std::remove_cv_t<T>, ^^public_flattened_nonstatic_data_members_v>};
#undef REFLECT_CPP26_NSDM_NAME
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_NAME_ACCESS_HPP
