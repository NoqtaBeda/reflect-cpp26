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

namespace reflect_cpp26 {
template <size_t I, non_empty_class_type_or_with_cvref T>
constexpr decltype(auto) get_ith_direct_nsdm(T&& obj)
{
  constexpr auto all_nsdms = all_direct_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < all_nsdms.size());
  return std::forward_like<T>(obj.[: all_nsdms[I] :]);
}

template <size_t I, non_empty_class_type_or_with_cvref T>
constexpr decltype(auto) get_ith_public_direct_nsdm(T&& obj)
{
  constexpr auto public_nsdms = public_direct_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < public_nsdms.size());
  return std::forward_like<T>(obj.[: public_nsdms[I] :]);
}

template <size_t I, non_empty_class_type_or_with_cvref T>
constexpr decltype(auto) get_ith_flattened_nsdm(T&& obj)
{
  constexpr auto all_nsdms = all_flattened_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < all_nsdms.size());
  return std::forward_like<T>(obj.[: all_nsdms[I].member :]);
}

template <size_t I, non_empty_class_type_or_with_cvref T>
constexpr decltype(auto) get_ith_public_flattened_nsdm(T&& obj)
{
  constexpr auto public_nsdms =
    public_flattened_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < public_nsdms.size());
  return std::forward_like<T>(obj.[: public_nsdms[I].member :]);
}

#define REFLECT_CPP26_FIRST_LAST_NSDM(fn, front_back, all_public) \
  template <non_empty_class_type_or_with_cvref T>                 \
  constexpr decltype(auto) fn(T&& obj)                            \
  {                                                               \
    constexpr auto members =                                      \
      all_public##_direct_nsdm_v<std::remove_cvref_t<T>>;         \
    return std::forward_like<T>(obj.[: members.front_back() :]);  \
  }

REFLECT_CPP26_FIRST_LAST_NSDM(get_first_direct_nsdm, front, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_direct_nsdm, back, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_public_direct_nsdm, front, public)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_public_direct_nsdm, back, public)
#undef REFLECT_CPP26_FIRST_LAST_NSDM

#define REFLECT_CPP26_FIRST_LAST_NSDM(fn, front_back, all_public)       \
  template <non_empty_class_type_or_with_cvref T>                       \
  constexpr decltype(auto) fn(T&& obj)                                  \
  {                                                                     \
    constexpr auto members =                                            \
      all_public##_flattened_nsdm_v<std::remove_cvref_t<T>>;            \
    return std::forward_like<T>(obj.[: members.front_back().member :]); \
  }

REFLECT_CPP26_FIRST_LAST_NSDM(get_first_flattened_nsdm, front, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_flattened_nsdm, back, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_public_flattened_nsdm, front, public)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_public_flattened_nsdm, back, public)
#undef REFLECT_CPP26_FIRST_LAST_NSDM
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP
