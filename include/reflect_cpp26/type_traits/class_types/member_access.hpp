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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_MEMBER_ACCESS_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_MEMBER_ACCESS_HPP

#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#include <reflect_cpp26/type_traits/cvref.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval auto convert_nsdm_type_like(
  std::meta::info member, std::meta::info T) -> std::meta::info
{
  auto M = type_of(member);
  if (is_reference_type(M)) {
    return M;
  }
  return add_cvref_like(M, T);
}
} // namespace impl

#define REFLECT_CPP26_ITH_DIRECT_NSDM_EXPR(arr) \
  [: impl::convert_nsdm_type_like(arr<std::remove_cvref_t<T>>[I], ^^T) :]

#define REFLECT_CPP26_ITH_FLATTENED_NSDM_EXPR(arr) \
  [: impl::convert_nsdm_type_like(arr<std::remove_cvref_t<T>>[I].member, ^^T) :]

/**
 * Gets the type of i-th direct non-static data member (including private and
 * protected ones) of type T by definition order.
 * (1) If the member type itself is a reference, gets the member type
 *     changed (i.e. cvref qualifiers of T have no effect);
 * (2) Otherwise, Let M be the member type (including the underlying type for
       bit-field members), gets add_cvref_like_t<M, T>.
 */
template <size_t I, class_or_union_type_or_cvref T>
using ith_direct_nsdm_type_t =
  REFLECT_CPP26_ITH_DIRECT_NSDM_EXPR(all_direct_nsdm_v);

/**
 * Gets the type of i-th direct public non-static data member of type T by
 * definition order. Details same as above.
 */
template <size_t I, class_or_union_type_or_cvref T>
using ith_public_direct_nsdm_type_t =
  REFLECT_CPP26_ITH_DIRECT_NSDM_EXPR(public_direct_nsdm_v);

/**
 * Gets the type of i-th flattened non-static data member (including private
 * and protected ones) of type T by definition order. Details see above and
 * flattenable.hpp.
 */
template <size_t I, partially_flattenable_or_cvref T>
using ith_flattened_nsdm_type_t =
  REFLECT_CPP26_ITH_FLATTENED_NSDM_EXPR(all_flattened_nsdm_v);

/**
 * Gets the type of i-th public flattened non-static data member of type T by
 * definition order. Details see above and flattenable.hpp.
 */
template <size_t I, partially_flattenable_or_cvref T>
using ith_public_flattened_nsdm_type_t =
  REFLECT_CPP26_ITH_FLATTENED_NSDM_EXPR(public_flattened_nsdm_v);

#undef REFLECT_CPP26_ITH_DIRECT_NSDM_EXPR
#undef REFLECT_CPP26_ITH_FLATTENED_NSDM_EXPR
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_MEMBER_ACCESS_HPP
