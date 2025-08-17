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
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
#define REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, member)  \
  if constexpr (is_bit_field(member)) {                     \
    return obj.[: member :];                                \
  } else {                                                  \
    return std::forward_like<T>(obj.[: member :]);          \
  }

template <size_t I, class_or_union_type_or_cvref T>
constexpr decltype(auto) get_ith_direct_nsdm(T&& obj)
{
  constexpr auto all_nsdms = all_direct_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < all_nsdms.size());
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, all_nsdms[I]);
}

template <size_t I, class_or_union_type_or_cvref T>
constexpr decltype(auto) get_ith_public_direct_nsdm(T&& obj)
{
  constexpr auto public_nsdms = public_direct_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < public_nsdms.size());
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, public_nsdms[I]);
}

template <size_t I, partially_flattenable_class_or_cvref T>
constexpr decltype(auto) get_ith_flattened_nsdm(T&& obj)
{
  constexpr auto all_nsdms = all_flattened_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < all_nsdms.size());
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, all_nsdms[I].member);
}

template <size_t I, partially_flattenable_class_or_cvref T>
constexpr decltype(auto) get_ith_public_flattened_nsdm(T&& obj)
{
  constexpr auto public_nsdms =
    public_flattened_nsdm_v<std::remove_cvref_t<T>>;
  static_assert(I < public_nsdms.size());
  REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, public_nsdms[I].member);
}

#define REFLECT_CPP26_FIRST_LAST_NSDM(fn, front_back, all_public)     \
  template <class_or_union_type_or_cvref T>                           \
  constexpr decltype(auto) fn(T&& obj)                                \
  {                                                                   \
    constexpr auto members =                                          \
      all_public##_direct_nsdm_v<std::remove_cvref_t<T>>;             \
    REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, members.front_back()); \
  }

REFLECT_CPP26_FIRST_LAST_NSDM(get_first_direct_nsdm, front, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_direct_nsdm, back, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_public_direct_nsdm, front, public)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_public_direct_nsdm, back, public)
#undef REFLECT_CPP26_FIRST_LAST_NSDM

#define REFLECT_CPP26_FIRST_LAST_NSDM(fn, front_back, all_public)             \
  template <partially_flattenable_class_or_cvref T>                           \
  constexpr decltype(auto) fn(T&& obj)                                        \
  {                                                                           \
    constexpr auto members =                                                  \
      all_public##_flattened_nsdm_v<std::remove_cvref_t<T>>;                  \
    REFLECT_CPP26_FORWARD_AND_RETURN_MEMBER(T, members.front_back().member);  \
  }

REFLECT_CPP26_FIRST_LAST_NSDM(get_first_flattened_nsdm, front, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_flattened_nsdm, back, all)
REFLECT_CPP26_FIRST_LAST_NSDM(get_first_public_flattened_nsdm, front, public)
REFLECT_CPP26_FIRST_LAST_NSDM(get_last_public_flattened_nsdm, back, public)
#undef REFLECT_CPP26_FIRST_LAST_NSDM

#define REFLECT_CPP26_DIRECT_NSDM_NAME_ARRAY(name, all_or_public) \
  namespace impl {                                                \
  template <class T>                                              \
  constexpr auto make_##name##_array()                            \
    /* -> std::array<std::string_view, N> */                      \
  {                                                               \
    constexpr auto members = all_or_public##_direct_nsdm_v<T>;    \
    constexpr auto N = members.size();                            \
    auto res = std::array<std::string_view, N>{};                 \
    for (auto i = 0zU; i < N; i++) {                              \
      res[i] = reflect_cpp26::identifier_of(members[i]);          \
    }                                                             \
    return res;                                                   \
  }                                                               \
  } /* namespace impl */                                          \
                                                                  \
  template <class_or_union_type T>                                \
  constexpr auto name##_v = impl::make_##name##_array<T>();

REFLECT_CPP26_DIRECT_NSDM_NAME_ARRAY(direct_nsdm_names, all)
REFLECT_CPP26_DIRECT_NSDM_NAME_ARRAY(public_direct_nsdm_names, public)
#undef REFLECT_CPP26_DIRECT_NSDM_NAME_ARRAY

#define REFLECT_CPP26_FLATTENED_NSDM_NAME_ARRAY(name, all_or_public)  \
  namespace impl {                                                    \
  template <class T>                                                  \
  constexpr auto make_##name##_array()                                \
    /* -> std::array<std::string_view, N> */                          \
  {                                                                   \
    constexpr auto members = all_or_public##_flattened_nsdm_v<T>;     \
    constexpr auto N = members.size();                                \
    auto res = std::array<std::string_view, N>{};                     \
    for (auto i = 0zU; i < N; i++) {                                  \
      res[i] = reflect_cpp26::identifier_of(members[i].member);       \
    }                                                                 \
    return res;                                                       \
  }                                                                   \
  } /* namespace impl */                                              \
                                                                      \
  template <class_or_union_type T>                                    \
  constexpr auto name##_v = impl::make_##name##_array<T>();

REFLECT_CPP26_FLATTENED_NSDM_NAME_ARRAY(flattened_nsdm_names, all)
REFLECT_CPP26_FLATTENED_NSDM_NAME_ARRAY(public_flattened_nsdm_names, public)
#undef REFLECT_CPP26_FLATTENED_NSDM_NAME_ARRAY

#define REFLECT_CPP26_NSDM_NAME(type_concept, name_group, all_or_public)  \
  template <type_concept T>                                               \
  constexpr auto ith_##name_group(size_t i, std::string_view alt = "")    \
    -> std::string_view                                                   \
  {                                                                       \
    constexpr auto names = name_group##s_v<T>;                            \
    if (i >= names.size()) {                                              \
      return "<index-out-of-range>";                                      \
    }                                                                     \
    auto res = names[i];                                                  \
    return res.empty() ? alt : res;                                       \
  }

REFLECT_CPP26_NSDM_NAME(class_or_union_type, direct_nsdm_name, all)
REFLECT_CPP26_NSDM_NAME(class_or_union_type, public_direct_nsdm_name, public)
REFLECT_CPP26_NSDM_NAME(partially_flattenable_class, flattened_nsdm_name, all)
REFLECT_CPP26_NSDM_NAME(
  partially_flattenable_class, public_flattened_nsdm_name, public)
#undef REFLECT_CPP26_NSDM_NAME
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_OPERATIONS_MEMBER_ACCESS_HPP
