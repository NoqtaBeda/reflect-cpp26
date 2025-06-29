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

#ifndef REFLECT_CPP26_UTILS_STRING_HASH_HPP
#define REFLECT_CPP26_UTILS_STRING_HASH_HPP

#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <reflect_cpp26/type_traits/type_comparison.hpp>
#include <reflect_cpp26/utils/ctype.hpp>
#include <cwctype>
#include <string_view>

namespace reflect_cpp26 {
namespace impl {
constexpr unsigned bkdr_hash_default_p = 131u;

struct char_identity_t {
  static constexpr auto operator()(auto c) {
    return c;
  }
};

struct std_ctype_tolower_t {
  static char operator()(char c) {
    return std::tolower(c); // Locale dependent
  }

  static wchar_t operator()(wchar_t c) {
    return std::towlower(c); // Locale dependent
  }
};

template <class Transform, class ResultT, class CharT>
constexpr auto bkdr_hash_generic_it(
  const CharT* begin, const CharT* end, ResultT p) -> ResultT
{
  auto res = ResultT{0};
  for (; begin < end; ++begin) {
    res = res * p + static_cast<ResultT>(Transform::operator()(*begin));
  }
  return res;
}

template <class Transform, class ResultT, class StringT>
constexpr auto bkdr_hash_generic_s(const StringT& str, ResultT p) -> ResultT
{
  auto begin = std::ranges::data(str);
  auto end = begin + std::ranges::size(str);
  return bkdr_hash_generic_it<Transform>(begin, end, p);
}
} // namespace impl

#define REFLECT_CPP26_BKDR_HASH(fn_name, ResultT, Transform)                  \
  template <char_type CharT>                                                  \
  constexpr auto fn_name(                                                     \
    const CharT* begin, const CharT* end,                                     \
    ResultT p = impl::bkdr_hash_default_p) -> ResultT                         \
  {                                                                           \
    return impl::bkdr_hash_generic_it<Transform>(begin, end, p);              \
  }                                                                           \
                                                                              \
  template <char_type CharT>                                                  \
  constexpr auto fn_name(                                                     \
    const CharT* str, ResultT p = impl::bkdr_hash_default_p) -> ResultT       \
  {                                                                           \
    auto null_pos = std::ranges::find(str, std::unreachable_sentinel, '\0');  \
    return impl::bkdr_hash_generic_it<Transform>(str, null_pos, p);           \
  }                                                                           \
                                                                              \
  template <string_like StringT>                                              \
  constexpr auto fn_name(                                                     \
    const StringT& str, uint32_t p = impl::bkdr_hash_default_p) -> ResultT    \
  {                                                                           \
    return impl::bkdr_hash_generic_s<Transform>(str, p);                      \
  }

REFLECT_CPP26_BKDR_HASH(bkdr_hash32, uint32_t, impl::char_identity_t)
REFLECT_CPP26_BKDR_HASH(bkdr_hash64, uint64_t, impl::char_identity_t)
REFLECT_CPP26_BKDR_HASH(ascii_ci_bkdr_hash32, uint32_t, ascii_tolower_t)
REFLECT_CPP26_BKDR_HASH(ascii_ci_bkdr_hash64, uint64_t, ascii_tolower_t)
#undef REFLECT_CPP26_BKDR_HASH


#define REFLECT_CPP26_CI_BKDR_HASH(fn_name, ResultT)                          \
  template <same_as_one_of<char, wchar_t> CharT>                              \
  ResultT fn_name(const CharT* begin, const CharT* end,                       \
                  ResultT p = impl::bkdr_hash_default_p)                      \
  {                                                                           \
    return impl::bkdr_hash_generic_it<impl::std_ctype_tolower_t>(             \
      begin, end, p);                                                         \
  }                                                                           \
                                                                              \
  template <same_as_one_of<char, wchar_t> CharT>                              \
  ResultT fn_name(const CharT* str, ResultT p = impl::bkdr_hash_default_p)    \
  {                                                                           \
    auto null_pos = std::ranges::find(str, std::unreachable_sentinel, '\0');  \
    return impl::bkdr_hash_generic_it<impl::std_ctype_tolower_t>(             \
      str, null_pos, p);                                                      \
  }                                                                           \
                                                                              \
  template <string_like StringT>                                              \
    requires (same_as_one_of<char_type_t<StringT>, char, wchar_t>)            \
  ResultT fn_name(const StringT& str, uint32_t p = impl::bkdr_hash_default_p) \
  {                                                                           \
    return impl::bkdr_hash_generic_s<impl::std_ctype_tolower_t>(str, p);      \
  }

REFLECT_CPP26_CI_BKDR_HASH(ci_bkdr_hash32, uint32_t)
REFLECT_CPP26_CI_BKDR_HASH(ci_bkdr_hash64, uint64_t)
#undef REFLECT_CPP26_CI_BKDR_HASH
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_STRING_HASH_HPP
