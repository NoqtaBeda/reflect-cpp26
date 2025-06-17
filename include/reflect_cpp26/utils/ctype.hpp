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


#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <cstdint>

/**
 * Constexpr, locale-independent alternative to std::isalnum etc. in <cctype>.
 */
#define REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(F) \
  F(isalnum)  \
  F(isalpha)  \
  F(islower)  \
  F(isupper)  \
  F(isdigit)  \
  F(isxdigit) \
  F(isblank)  \
  F(iscntrl)  \
  F(isgraph)  \
  F(isspace)  \
  F(isprint)  \
  F(ispunct)

namespace reflect_cpp26 {
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
} // namespace impl

constexpr bool isalnum(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_alnum_mask) != 0;
}

constexpr bool isalpha(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_alpha_mask) != 0;
}

constexpr bool islower(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_lower_mask) != 0;
}

constexpr bool isupper(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_upper_mask) != 0;
}

constexpr bool isdigit(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_digit_mask) != 0;
}

constexpr bool isxdigit(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_xdigit_mask) != 0;
}

constexpr bool isblank(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_blank_mask) != 0;
}

constexpr bool iscntrl(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_print_mask) == 0;
}

constexpr bool isgraph(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_graph_mask) != 0;
}

constexpr bool isspace(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_space_mask) != 0;
}

constexpr bool isprint(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_print_mask) != 0;
}

constexpr bool ispunct(uint8_t c) {
  return (c <= 127u)
    && (impl::ctype_flag_table[c] & impl::ctype_punct_mask) != 0;
}

constexpr uint8_t tolower(uint8_t c) {
  return isupper(c) ? c + 'a' - 'A' : c;
}

constexpr uint8_t toupper(uint8_t c) {
  return islower(c) ? c + 'A' - 'a' : c;
}

#define REFLECT_CPP26_DEFINE_CTYPE_PREDICATE_IN_NAMESPACE(func) \
  template <non_bool_integral T>                                \
  constexpr bool func(T c) {                                    \
    return c >= 0 && c <= 127 && func(static_cast<uint8_t>(c)); \
  }

REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(
  REFLECT_CPP26_DEFINE_CTYPE_PREDICATE_IN_NAMESPACE)

#undef REFLECT_CPP26_DEFINE_CTYPE_PREDICATE_IN_NAMESPACE

template <non_bool_integral T>
constexpr auto tolower(T c) -> T {
  return c >= 0 && c <= 127 ? tolower(static_cast<uint8_t>(c)) : c;
}

template <non_bool_integral T>
constexpr auto toupper(T c) -> T {
  return c >= 0 && c <= 127 ? toupper(static_cast<uint8_t>(c)) : c;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_CTYPE_HPP
