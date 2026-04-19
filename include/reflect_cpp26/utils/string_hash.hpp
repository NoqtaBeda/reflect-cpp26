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

#ifndef REFLECT_CPP26_UTILS_STRING_HASH_HPP
#define REFLECT_CPP26_UTILS_STRING_HASH_HPP

#include <cwctype>
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <reflect_cpp26/type_traits/type_comparison.hpp>
#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/string_utility.hpp>
#include <string_view>

namespace reflect_cpp26 {
namespace impl {
constexpr auto bkdr_hash_p = 131zU;
}  // namespace impl

struct bkdr_hash_t {
  template <char_type CharT>
  static constexpr auto operator()(const CharT* begin, const CharT* end) -> size_t {
    auto res = size_t{0};
    for (; begin < end; ++begin) {
      res = res * impl::bkdr_hash_p + static_cast<size_t>(*begin);
    }
    return res;
  }

  template <string_like StringT>
  static constexpr auto operator()(const StringT& str) -> size_t {
    auto sv = make_string_view(str);
    return operator()(sv.data(), sv.data() + sv.size());
  }
};

struct ascii_ci_bkdr_hash_t {
  template <char_type CharT>
  static constexpr auto operator()(const CharT* begin, const CharT* end) -> size_t {
    auto res = size_t{0};
    for (; begin < end; ++begin) {
      auto cur = ascii_tolower(*begin);
      res = res * impl::bkdr_hash_p + static_cast<size_t>(cur);
    }
    return res;
  }

  template <string_like StringT>
  static constexpr auto operator()(const StringT& str) -> size_t {
    auto sv = make_string_view(str);
    return operator()(sv.data(), sv.data() + sv.size());
  }
};

constexpr auto bkdr_hash = bkdr_hash_t{};
constexpr auto ascii_ci_bkdr_hash = ascii_ci_bkdr_hash_t{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_STRING_HASH_HPP
