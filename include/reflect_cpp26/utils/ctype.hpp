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

#ifndef REFLECT_CPP26_UTILS_CTYPE_HPP
#define REFLECT_CPP26_UTILS_CTYPE_HPP

// Root header: Include only:
// (1) C++ stdlib; (2) utils/config.h; (3) Other root headers
#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <cstdint>
#include <functional>

namespace reflect_cpp26 {
struct is_ascii_char_t {
  static constexpr bool operator()(non_bool_integral auto c) {
    return c >= 0 && c <= 127;
  }
};
constexpr auto is_ascii_char = is_ascii_char_t{};

struct is_ascii_string_t {
  static constexpr bool operator()(const string_like auto& str)
  {
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
  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
  0x0,  0x6,  0x2,  0x2,  0x2,  0x2,  0x0,  0x0,
  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,
  0x7,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,
  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,  0x9,
  0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1, 0xc1,
  0xc1, 0xc1, 0x9,  0x9,  0x9,  0x9,  0x9,  0x9,
  0x9,  0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x11,
  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x9,  0x9,  0x9,  0x9,  0x9,
  0x9,  0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0x21,
  0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
  0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,
  0x21, 0x21, 0x21, 0x9,  0x9,  0x9,  0x9,  0x0
};

template <class Comp, uint8_t Mask>
struct ascii_ctype_predicate_common_t {
  static constexpr auto comp = Comp{};

  static constexpr bool operator()(non_bool_integral auto c) {
    return is_ascii_char(c) && comp(ctype_flag_table[c] & Mask, uint8_t(0));
  }
};
} // namespace impl

/**
 * Constexpr, locale-independent alternative to std::isalnum etc. in <cctype>.
 */
#define REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(F)         \
  F(isalnum,  std::not_equal_to, impl::ctype_alnum_mask)  \
  F(isalpha,  std::not_equal_to, impl::ctype_alpha_mask)  \
  F(islower,  std::not_equal_to, impl::ctype_lower_mask)  \
  F(isupper,  std::not_equal_to, impl::ctype_upper_mask)  \
  F(isdigit,  std::not_equal_to, impl::ctype_digit_mask)  \
  F(isxdigit, std::not_equal_to, impl::ctype_xdigit_mask) \
  F(isblank,  std::not_equal_to, impl::ctype_blank_mask)  \
  F(iscntrl,  std::equal_to,     impl::ctype_print_mask)  \
  F(isgraph,  std::not_equal_to, impl::ctype_graph_mask)  \
  F(isspace,  std::not_equal_to, impl::ctype_space_mask)  \
  F(isprint,  std::not_equal_to, impl::ctype_print_mask)  \
  F(ispunct,  std::not_equal_to, impl::ctype_punct_mask)

#define REFLECT_CPP26_DEFINE_CTYPE_PREDICATE(func, Comp, Mask)      \
  struct ascii_##func##_t                                           \
    : impl::ascii_ctype_predicate_common_t<Comp<uint8_t>, Mask> {}; \
  constexpr auto ascii_##func = ascii_##func##_t{};

REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(REFLECT_CPP26_DEFINE_CTYPE_PREDICATE)
#undef REFLECT_CPP26_DEFINE_CTYPE_PREDICATE
#undef REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH

namespace impl {
template <class Derived>
struct ascii_ctype_conversion_common_t {
  template <string_like StringT>
  static constexpr auto operator()(const StringT& str)
    /* -> std::basic_string<CharT> */
  {
    using CharT = char_type_t<StringT>;
    auto res = std::basic_string<CharT>{};
    res.resize_and_overwrite(str.length(), [&str](CharT* buffer, size_t n) {
      for (auto c: str) { *buffer++ = Derived::convert_char(c); }
      return n;
    });
    return res;
  }

  static constexpr auto operator()(char_type auto c) {
    return Derived::convert_char(c);
  }
};
} // namespace impl

struct ascii_tolower_t
  : impl::ascii_ctype_conversion_common_t<ascii_tolower_t>
{
  static constexpr auto convert_char(non_bool_integral auto c) {
    return ascii_isupper(c) ? c + 'a' - 'A' : c;
  }
};

struct ascii_toupper_t
  : impl::ascii_ctype_conversion_common_t<ascii_toupper_t>
{
  static constexpr auto convert_char(non_bool_integral auto c) {
    return ascii_islower(c) ? c + 'A' - 'a' : c;
  }
};

constexpr auto ascii_tolower = ascii_tolower_t{};
constexpr auto ascii_toupper = ascii_toupper_t{};
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_CTYPE_HPP
