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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_STRING_POLICY_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_STRING_POLICY_HPP

#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <reflect_cpp26/utils/string_hash.hpp>

namespace reflect_cpp26::impl::map {
template <class CharT>
struct skey_identity_policy {
  static constexpr size_t hash(std::basic_string_view<CharT> u) {
    return bkdr_hash(u);
  }

  static constexpr bool equals(meta_basic_string_view<CharT> t, std::basic_string_view<CharT> u) {
    return t == u;
  }
};

// ASCII case-insensitive. Non-ASCII characters are NOT supported.
template <class CharT>
struct skey_case_insensitive_policy {
  static constexpr size_t hash(std::basic_string_view<CharT> u) {
    return ascii_ci_bkdr_hash(u);
  }

  static constexpr bool equals(meta_basic_string_view<CharT> t, std::basic_string_view<CharT> u) {
    if (u.length() != t.length()) {
      return false;
    }
    const auto* iu = u.data();
    for (const auto* it = t.head; it < t.head + t.n; ++it, ++iu) {
      if (*it != ascii_tolower(*iu)) return false;
    }
    return true;
  }
};

consteval auto get_skey_policy_template(bool case_insensitive) {
  return case_insensitive ? ^^skey_case_insensitive_policy : ^^skey_identity_policy;
}
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_STRING_POLICY_HPP
