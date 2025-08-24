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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_NAME_ACCESS_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_NAME_ACCESS_HPP

#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
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

template <class_or_union_type T>
constexpr auto all_direct_nsdm_names_v =
  impl::make_direct_nsdm_name_array<
    std::remove_cv_t<T>, ^^all_direct_nsdms_v>();

template <class_or_union_type T>
constexpr auto public_direct_nsdm_names_v =
  impl::make_direct_nsdm_name_array<
    std::remove_cv_t<T>, ^^public_direct_nsdms_v>();

template <partially_flattenable_class T>
constexpr auto all_flattened_nsdm_names_v =
  impl::make_flattened_nsdm_name_array<
    std::remove_cv_t<T>, ^^all_flattened_nsdms_v>();

template <partially_flattenable_class T>
constexpr auto public_flattened_nsdm_names_v =
  impl::make_flattened_nsdm_name_array<
    std::remove_cv_t<T>, ^^public_flattened_nsdms_v>();
} // namespace impl

#define REFLECT_CPP26_NSDM_NAME(concept_type, fn_name, arr_v)     \
  template <concept_type T>                                       \
  constexpr auto fn_name(size_t i, std::string_view alt = "")     \
    -> std::string_view                                           \
  {                                                               \
    constexpr const auto& arr = impl::arr_v<std::remove_cv_t<T>>; \
    if (i >= arr.size()) {                                        \
      return "<out-of-range>";                                    \
    }                                                             \
    return arr[i].empty() ? alt : arr[i];                         \
  }

/**
 * ith_direct_nonstatic_data_member_name<T>(
 *   size_t i, std::string_view alt = "")
 * Gets the name of i-th direct non-static data member of class or union T,
 * or alt if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(class_or_union_type,
                        ith_direct_nonstatic_data_member_name,
                        all_direct_nsdm_names_v)
/**
 * ith_public_direct_nonstatic_data_member_name<T>(
 *   size_t i,
 *   std::string_view alt = "")
 * Gets the name of i-th direct non-static data member with public access of
 * class or union T, or alt if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(class_or_union_type,
                        ith_public_direct_nonstatic_data_member_name,
                        public_direct_nsdm_names_v)
/**
 * ith_flattened_nonstatic_data_member_name<T>(
 *   size_t i,
 *   std::string_view alt = ""):
 * Gets the name of i-th flattened non-static data member of class T, or alt
 * if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(partially_flattenable_class,
                        ith_flattened_nonstatic_data_member_name,
                        all_flattened_nsdm_names_v)
/**
 * ith_public_flattened_nonstatic_data_member_name<T>(
 *   size_t i,
 *   std::string_view alt = ""):
 * Gets the name of i-th flattened non-static data member with public access of
 * class T, or alt if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(partially_flattenable_class,
                        ith_public_flattened_nonstatic_data_member_name,
                        public_flattened_nsdm_names_v)

#undef REFLECT_CPP26_NSDM_NAME
} // namespace reflect_cpp26
