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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_POLICY_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_POLICY_HPP

#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/fixed_map/impl/common.hpp>
#include <reflect_cpp26/utils/string_hash.hpp>

namespace reflect_cpp26::impl {
struct string_key_identity_policy_t {
  struct equal_length_tag_t {};
  static constexpr auto equal_length = equal_length_tag_t{};

  static constexpr uint64_t hash(const string_like auto& u) {
    return bkdr_hash64(u);
  }

  static constexpr auto convert_char(char_type auto c) {
    return c;
  }

  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool equals(
    meta_basic_string_view<CharT> t, const StringU& u)
  {
    return t == u;
  }

  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool equals(
    meta_basic_string_view<CharT> t, const StringU& u, equal_length_tag_t)
  {
    const auto* iu = std::ranges::data(u);
    for (const auto *it = t.head; it < t.tail; ++it, ++iu) {
      if (*it != *iu) { return false; }
    }
    return true;
  }
};

struct string_key_case_insensitive_policy_t {
  struct equal_length_tag_t {};
  static constexpr auto equal_length = equal_length_tag_t{};

  static constexpr uint64_t hash(const string_like auto& u) {
    return ascii_ci_bkdr_hash64(u);
  }

  static constexpr auto convert_char(char_type auto c) {
    return ascii_tolower(c);
  }

  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool equals(
    meta_basic_string_view<CharT> t, const StringU& u)
  {
    return t.length() == u.length() && equals(t, u, equal_length);
  }

  // t has already been converted to all-lower case.
  template <char_type CharT, string_like_of<CharT> StringU>
  static constexpr bool equals(
    meta_basic_string_view<CharT> t, const StringU& u, equal_length_tag_t)
  {
    const auto* iu = std::ranges::data(u);
    for (const auto *it = t.head; it < t.tail; ++it, ++iu) {
      if (*it != ascii_tolower(*iu)) { return false; }
    }
    return true;
  }
};

consteval auto string_key_policy_type(bool case_insensitive)
{
  return case_insensitive
    ? ^^string_key_case_insensitive_policy_t
    : ^^string_key_identity_policy_t;
}
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_POLICY_HPP
