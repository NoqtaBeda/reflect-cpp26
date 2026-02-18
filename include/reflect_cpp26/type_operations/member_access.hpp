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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP

#include <reflect_cpp26/type_traits/class_types/flattened_nsdm.hpp>
#include <reflect_cpp26/type_traits/cvref.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
#define REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, m)               \
  if constexpr (is_bit_field(m) || is_reference_type(type_of(m))) { \
    return obj.[:m:];                                               \
  } else {                                                          \
    using M = [:type_of(m):];                                       \
    return static_cast<add_cvref_like_t<M, T>>(obj.[:m:]);          \
  }

#define REFLECT_CPP26_NSDM_COUNT(prefix, T) \
  prefix##_nonstatic_data_members_v<std::remove_cvref_t<T>>.size()

#define REFLECT_CPP26_ITH_NSDM(prefix, T, I) \
  prefix##_nonstatic_data_members_v<std::remove_cvref_t<T>>[I]

/**
 * Gets the i-th direct non-static data member of class or union type T.
 * (1) If the member is bit-field whose underlying type is M, then the result
 *     type is M;
 * (2) If the member is reference whose type is M& or M&& (where M may be cv-
 *     qualified), then the result type is M& or M&&;
 * (3) Otherwise, let M be the member type, then the result type is
 *     add_cvref_like_t<M, T>.
 */
template <size_t I, class T>
  requires(class_or_union_type<std::remove_cvref_t<T>>
           && I < REFLECT_CPP26_NSDM_COUNT(all_direct, T))
constexpr decltype(auto) get_ith_direct_nonstatic_data_member(T&& obj) {
  constexpr auto cur_nsdm = REFLECT_CPP26_ITH_NSDM(all_direct, T, I);
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

/**
 * Gets the i-th direct non-static data member of class or union type T with
 * public access (i.e. accessible outside T).
 * Details same as above.
 */
template <size_t I, class T>
  requires(class_or_union_type<std::remove_cvref_t<T>>
           && I < REFLECT_CPP26_NSDM_COUNT(public_direct, T))
constexpr decltype(auto) get_ith_public_direct_nonstatic_data_member(T&& obj) {
  constexpr auto cur_nsdm = REFLECT_CPP26_ITH_NSDM(public_direct, T, I);
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

/**
 * Gets the i-th flattened non-static data member of class T.
 * Details same as above.
 */
template <size_t I, class T>
  requires(partially_flattenable_class<std::remove_cvref_t<T>>
           && I < REFLECT_CPP26_NSDM_COUNT(all_flattened, T))
constexpr decltype(auto) get_ith_nonstatic_data_member(T&& obj) {
  constexpr auto cur_nsdm = REFLECT_CPP26_ITH_NSDM(all_flattened, T, I).member;
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

/**
 * Gets the i-th flattened non-static data member of class T with public access
 * (i.e. accessible outside T).
 * Details same as above.
 */
template <size_t I, class T>
  requires(partially_flattenable_class<std::remove_cvref_t<T>>
           && I < REFLECT_CPP26_NSDM_COUNT(public_flattened, T))
constexpr decltype(auto) get_ith_public_nonstatic_data_member(T&& obj) {
  constexpr auto cur_nsdm = REFLECT_CPP26_ITH_NSDM(public_flattened, T, I).member;
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, cur_nsdm);
}

#undef REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER
#undef REFLECT_CPP26_NSDM_COUNT
#undef REFLECT_CPP26_ITH_NSDM
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP
