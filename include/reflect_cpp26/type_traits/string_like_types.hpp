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

#ifndef REFLECT_CPP26_TYPE_TRAITS_STRING_LIKE_TYPES_HPP
#define REFLECT_CPP26_TYPE_TRAITS_STRING_LIKE_TYPES_HPP

// Root header: Include only:
// (1) C++ stdlib; (2) utils/config.h; (3) Other root headers
#include <ranges>
#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/type_traits/type_comparison.hpp>
#include <string_view>

namespace reflect_cpp26 {
namespace impl {
template <class T>
consteval bool is_string_like_impl() {
  using D = std::decay_t<T>;
  if constexpr (std::is_reference_v<T>) {
    return false;
  } else if constexpr (std::ranges::contiguous_range<T>) {
    using V = std::ranges::range_value_t<T>;
    if constexpr (char_type<V>) {
      return requires(D range) { std::basic_string_view<V>(range); };
    } else {
      return false;
    }
  } else if constexpr (std::is_pointer_v<D>) {
    using V = std::remove_pointer_t<D>;
    return char_type<V> && !std::is_volatile_v<V>;
  } else {
    return false;
  }
}

consteval auto get_char_type(std::meta::info T) {
  auto is_range = extract<bool>(substitute(^^std::ranges::contiguous_range,
                                           {
                                               T}));
  if (is_range) {
    return substitute(^^std::ranges::range_value_t,
                      {
                          T});
  }
  auto D = decay(T);
  if (!is_pointer_type(D)) {
    compile_error("Implementation error: Expects D to be [const] CharT*");
  }
  return remove_cv(remove_pointer(D));
}
}  // namespace impl

/**
 * Whether T (possibly with cv qualifiers) is a string-like type:
 * T can be converted to std::basic_string_view<CharT> where CharT is a
 * character type, including (but not limited to):
 * (1) C-style non-volatile character arrays: CharT[], const CharT[N], etc.
 * (2) C-style string pointer: CharT*, const CharT*
 * (3) std::basic_string
 * (4) std::basic_string_view or meta_basic_string_view
 * (5) Contiguous range of characters: std::vector<CharT>, std::array<CharT, N>
 */
template <class T>
concept string_like = impl::is_string_like_impl<T>();

/**
 * Extracts the character type of a string-like type.
 */
template <string_like T>
using char_type_t = [:impl::get_char_type(^^T):];

/**
 * Whether T (possibly with cv qualifiers) is a string-like type whose
 * value type is exactly CharT.
 */
template <class T, class CharT>
concept string_like_of =
    string_like<T> && char_type<CharT> && std::is_same_v<char_type_t<T>, CharT>;

/**
 * Whether T and Args... (possibly with cv qualifiers) are
 * string-like types of exactly the same character type.
 */
template <class T, class... Args>
constexpr auto are_string_like_of_same_char_type_v = false;

template <string_like T, string_like... Args>
constexpr auto are_string_like_of_same_char_type_v<T, Args...> =
    are_all_same_v<char_type_t<T>, char_type_t<Args>...>;

/**
 * Whether T is a C-style string, i.e. CharT* or const CharT*.
 * Note that volatile CharT* is disallowed.
 */
template <class T>
concept c_style_string = std::is_pointer_v<T> && char_type<std::remove_pointer_t<T>>
                      && !std::is_volatile_v<std::remove_pointer_t<T>>;
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_STRING_LIKE_TYPES_HPP
