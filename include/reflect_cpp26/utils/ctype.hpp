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

#ifndef REFLECT_CPP26_UTILS_CTYPE_HPP
#define REFLECT_CPP26_UTILS_CTYPE_HPP

#include <algorithm>
#include <cstdint>
#include <functional>
#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <reflect_cpp26/utils/string_utility.hpp>

namespace reflect_cpp26 {
struct is_ascii_char_t {
  static constexpr bool operator()(non_bool_integral auto c) {
    return c >= 0 && c <= 127;
  }
};
constexpr auto is_ascii_char = is_ascii_char_t{};

struct is_ascii_string_t {
  static constexpr bool operator()(const string_like auto& str) {
    auto sv = std::basic_string_view{str};
    return std::ranges::all_of(sv, is_ascii_char);
  }
};
constexpr auto is_ascii_string = is_ascii_string_t{};

namespace impl {
constexpr uint8_t ctype_print_mask = 1u;
constexpr uint8_t ctype_space_mask = 2u;
constexpr uint8_t ctype_blank_mask = 4u;
constexpr uint8_t ctype_punct_mask = 8u;
constexpr uint8_t ctype_upper_mask = 16u;
constexpr uint8_t ctype_lower_mask = 32u;
constexpr uint8_t ctype_digit_mask = 64u;
constexpr uint8_t ctype_xdigit_mask = 128u;

constexpr uint8_t ctype_alpha_mask = ctype_upper_mask | ctype_lower_mask;
constexpr uint8_t ctype_alnum_mask = ctype_alpha_mask | ctype_digit_mask;
constexpr uint8_t ctype_graph_mask = ctype_alnum_mask | ctype_punct_mask;

constexpr uint8_t ctype_flag_table[128] = {
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x6,  0x2,  0x2,  0x2,  0x2,  0x0,  0x0,
    0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
    0x7,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,
    0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0x9,  0x9,  0x9,  0x9,  0x9,  0x9,
    0x9,  0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x9,  0x9,  0x9,  0x9,  0x9,
    0x9,  0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
    0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x9,  0x9,  0x9,  0x9,  0x0};

template <class Comp, uint8_t Mask>
struct ascii_ctype_predicate_t {
  static constexpr auto comp = Comp{};

  static constexpr bool operator()(non_bool_integral auto c) {
    return is_ascii_char(c) && comp(ctype_flag_table[c] & Mask, uint8_t(0));
  }
};
}  // namespace impl

#define REFLECT_CPP26_CTYPE_PREDICATE(func, Comp, Mask)                                       \
  struct ascii_##func##_t : impl::ascii_ctype_predicate_t<std::Comp<uint8_t>, impl::Mask> {}; \
  constexpr auto ascii_##func = ascii_##func##_t{};

REFLECT_CPP26_CTYPE_PREDICATE(isalnum, not_equal_to, ctype_alnum_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isalpha, not_equal_to, ctype_alpha_mask)
REFLECT_CPP26_CTYPE_PREDICATE(islower, not_equal_to, ctype_lower_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isupper, not_equal_to, ctype_upper_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isdigit, not_equal_to, ctype_digit_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isxdigit, not_equal_to, ctype_xdigit_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isblank, not_equal_to, ctype_blank_mask)
REFLECT_CPP26_CTYPE_PREDICATE(iscntrl, equal_to, ctype_print_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isgraph, not_equal_to, ctype_graph_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isspace, not_equal_to, ctype_space_mask)
REFLECT_CPP26_CTYPE_PREDICATE(isprint, not_equal_to, ctype_print_mask)
REFLECT_CPP26_CTYPE_PREDICATE(ispunct, not_equal_to, ctype_punct_mask)
#undef REFLECT_CPP26_CTYPE_PREDICATE

namespace impl {
template <class Derived>
struct ascii_ctype_conversion_interface_t {
  template <string_like StringT>
  static constexpr auto operator()(const StringT& str)
  /* -> std::basic_string<CharT> */
  {
    using CharT = char_type_t<StringT>;
    auto sv = make_string_view(str);
    auto res = std::basic_string<CharT>{};
    res.resize_and_overwrite(sv.length(), [sv](CharT* buffer, size_t n) {
      for (auto c : sv) {
        *buffer++ = Derived::convert_char(c);
      }
      return n;
    });
    return res;
  }

  static constexpr auto operator()(char_type auto c) {
    return Derived::convert_char(c);
  }
};
}  // namespace impl

struct ascii_tolower_t : impl::ascii_ctype_conversion_interface_t<ascii_tolower_t> {
  template <char_type CharT>
  static constexpr auto convert_char(CharT c) {
    return static_cast<CharT>(ascii_isupper(c) ? c + 'a' - 'A' : c);
  }
};
constexpr auto ascii_tolower = ascii_tolower_t{};

struct ascii_toupper_t : impl::ascii_ctype_conversion_interface_t<ascii_toupper_t> {
  template <char_type CharT>
  static constexpr auto convert_char(CharT c) {
    return static_cast<CharT>(ascii_islower(c) ? c + 'A' - 'a' : c);
  }
};
constexpr auto ascii_toupper = ascii_toupper_t{};

struct ascii_trim_t {
  template <string_like StringT>
  static constexpr auto operator()(const StringT& str)
  /* -> std::basic_string_view */
  {
    auto sv = make_string_view(str);
    return do_trim(sv);
  }

private:
  template <class CharT, class Traits>
  static constexpr auto do_trim(std::basic_string_view<CharT, Traits> str)
      -> std::basic_string_view<CharT, Traits> {
    if (str.empty()) {
      return str;  // Empty check is necessary for constant evaluation
    }
    auto head = str.begin();
    auto before_tail = str.end() - 1;
    for (; head <= before_tail && ascii_isspace(*head); ++head) {
    }
    for (; head <= before_tail && ascii_isspace(*before_tail); --before_tail) {
    }
    return {head, before_tail + 1};
  }
};
constexpr auto ascii_trim = ascii_trim_t{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_CTYPE_HPP
