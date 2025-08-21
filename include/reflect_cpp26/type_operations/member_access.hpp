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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP

#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#include <reflect_cpp26/type_traits/cvref.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
#define REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, m)               \
  if constexpr (is_bit_field(m) || is_reference_type(type_of(m))) { \
    return obj.[: m :];                                             \
  } else {                                                          \
    using M = [: type_of(m) :];                                     \
    return static_cast<add_cvref_like_t<M, T>>(obj.[: m :]);        \
  }

/**
 * Gets the i-th direct non-static data member of class or union type T.
 * (1) If the member is bit-field whose underlying type is M, then the result
 *     type is M;
 * (2) If the member is reference whose type is M& or M&& (where M may be cv-
 *     qualified), then the result type is M& or M&&;
 * (3) Otherwise, let M be the member type, then the result type is
 *     add_cvref_like_t<M, T>.
 */
template <size_t I, class_or_union_type_or_cvref T>
  requires (I < all_direct_nsdm_v<std::remove_cvref_t<T>>.size())
constexpr decltype(auto) get_ith_direct_nsdm(T&& obj)
{
  constexpr auto cur_nsdm = all_direct_nsdm_v<std::remove_cvref_t<T>>[I];
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

/**
 * Gets the i-th direct non-static data member of class or union type T with
 * public access (i.e. accessible outside T).
 * Details same as above.
 */
template <size_t I, class_or_union_type_or_cvref T>
  requires (I < public_direct_nsdm_v<std::remove_cvref_t<T>>.size())
constexpr decltype(auto) get_ith_public_direct_nsdm(T&& obj)
{
  constexpr auto cur_nsdm = public_direct_nsdm_v<std::remove_cvref_t<T>>[I];
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

/**
 * Gets the i-th flattened non-static data member of class T.
 * Details same as above.
 */
template <size_t I, partially_flattenable_class_or_cvref T>
  requires (I < all_flattened_nsdm_v<std::remove_cvref_t<T>>.size())
constexpr decltype(auto) get_ith_flattened_nsdm(T&& obj)
{
  constexpr auto cur_nsdm =
    all_flattened_nsdm_v<std::remove_cvref_t<T>>[I].member;
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

/**
 * Gets the i-th flattened non-static data member of class T with public access
 * (i.e. accessible outside T).
 * Details same as above.
 */
template <size_t I, partially_flattenable_class_or_cvref T>
  requires (I < public_flattened_nsdm_v<std::remove_cvref_t<T>>.size())
constexpr decltype(auto) get_ith_public_flattened_nsdm(T&& obj)
{
  constexpr auto cur_nsdm =
    public_flattened_nsdm_v<std::remove_cvref_t<T>>[I].member;
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

#define REFLECT_CPP26_FIRST_LAST_NSDM(fn, front_back, all_public)       \
  template <class_or_union_type_or_cvref T>                             \
    requires (!std::is_empty_v<std::remove_cvref_t<T>>)                 \
  constexpr decltype(auto) fn(T&& obj)                                  \
  {                                                                     \
    constexpr auto cur_nsdm =                                           \
      all_public##_direct_nsdm_v<std::remove_cvref_t<T>>.front_back();  \
    REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);               \
  }

/**
 * get_first_direct_nsdm<T>(T&& obj):
 * Equivalent to get_ith_direct_nsdm<0>(obj).
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_direct_nsdm, front, all)
/**
 * get_last_direct_nsdm<T>(T&& obj):
 * Equivalent to get_ith_direct_nsdm<N-1>(obj) where N is the number of direct
 * non-static data members defined in std::remove_cvref_t<T>.
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_direct_nsdm, back, all)
/*
 * get_first_public_direct_nsdm<T>(T&& obj)
 * Equivalent to get_ith_public_direct_nsdm<0>(obj).
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_public_direct_nsdm, front, public)
/**
 * get_last_public_direct_nsdm<T>(T&& obj)
 * Equivalent to get_ith_public_direct_nsdm<Np-1>(obj) where Np is the number of
 * direct non-static data members with public access in std::remove_cvref_t<T>.
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_public_direct_nsdm, back, public)
#undef REFLECT_CPP26_FIRST_LAST_NSDM

#define REFLECT_CPP26_FIRST_LAST_NSDM(fn, front_back, all_public) \
  template <partially_flattenable_class_or_cvref T>               \
    requires (!std::is_empty_v<std::remove_cvref_t<T>>)           \
  constexpr decltype(auto) fn(T&& obj)                            \
  {                                                               \
    constexpr auto cur_nsdm =                                     \
      all_public##_flattened_nsdm_v<std::remove_cvref_t<T>>       \
        .front_back().member;                                     \
    REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);         \
  }

/**
 * get_first_flattened_nsdm<T>(T&& obj):
 * Equivalent to get_ith_flattened_nsdm<0>(obj).
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_flattened_nsdm, front, all)
/**
 * get_last_flattened_nsdm<T>(T&& obj):
 * Equivalent to get_ith_flattened_nsdm<N-1>(obj) where N is the number of
 * flattened non-static data members defined in or inherited by
 * std::remove_cvref_t<T>.
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_flattened_nsdm, back, all)
/**
 * get_first_public_flattened_nsdm<T>(T&& obj):
 * Equivalent to get_ith_public_flattened_nsdm<0>(obj).
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_public_flattened_nsdm, front, public)
/**
 * get_last_public_flattened_nsdm<T>(T&& obj):
 * Equivalent to get_ith_public_flattened_nsdm<Np-1>(obj) where Np is the number
 * of flattened non-static data members with public access defined in or
 * inherited by std::remove_cvref_t<T>.
 */
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_public_flattened_nsdm, back, public)
#undef REFLECT_CPP26_FIRST_LAST_NSDM
#undef REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER

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
    std::remove_cv_t<T>, ^^all_direct_nsdm_v>();

template <class_or_union_type T>
constexpr auto public_direct_nsdm_names_v =
  impl::make_direct_nsdm_name_array<
    std::remove_cv_t<T>, ^^public_direct_nsdm_v>();

template <partially_flattenable_class T>
constexpr auto all_flattened_nsdm_names_v =
  impl::make_flattened_nsdm_name_array<
    std::remove_cv_t<T>, ^^all_flattened_nsdm_v>();

template <partially_flattenable_class T>
constexpr auto public_flattened_nsdm_names_v =
  impl::make_flattened_nsdm_name_array<
    std::remove_cv_t<T>, ^^public_flattened_nsdm_v>();
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
 * ith_direct_nsdm_name<T>(size_t i, std::string_view alt = ""):
 * Gets the name of i-th direct non-static data member of class or union T,
 * or alt if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(class_or_union_type,
                        ith_direct_nsdm_name,
                        all_direct_nsdm_names_v)
/**
 * ith_public_direct_nsdm_name<T>(size_t i, std::string_view alt = ""):
 * Gets the name of i-th direct non-static data member with public access of
 * class or union T, or alt if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(class_or_union_type,
                        ith_public_direct_nsdm_name,
                        public_direct_nsdm_names_v)
/**
 * ith_flattened_nsdm_name<T>(size_t i, std::string_view alt = ""):
 * Gets the name of i-th flattened non-static data member of class T, or alt
 * if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(partially_flattenable_class,
                        ith_flattened_nsdm_name,
                        all_flattened_nsdm_names_v)
/**
 * ith_public_flattened_nsdm_name<T>(size_t i, std::string_view alt = ""):
 * Gets the name of i-th flattened non-static data member with public access of
 * class T, or alt if the member is anonymous.
 */
REFLECT_CPP26_NSDM_NAME(partially_flattenable_class,
                        ith_public_flattened_nsdm_name,
                        public_flattened_nsdm_names_v)

#undef REFLECT_CPP26_NSDM_NAME
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP
