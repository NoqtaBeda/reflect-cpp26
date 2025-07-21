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

#ifndef REFLECT_CPP26_ENUM_IMPL_ENUM_ALT_NAME_HPP
#define REFLECT_CPP26_ENUM_IMPL_ENUM_ALT_NAME_HPP

#include <reflect_cpp26/enum/impl/tags.hpp>
#include <reflect_cpp26/enum/enum_type_name.hpp>
#include <reflect_cpp26/utils/to_string_utils.hpp>
#include <charconv>
#include <utility>

namespace reflect_cpp26::impl {
template <class Iter, class E>
constexpr auto enum_alt_name_to(Iter iter, E value, with_allocated_buffer_tag_t)
  -> Iter
{
  *iter++ = '(';
  iter = std::ranges::copy(enum_type_name<E>(), iter).out;
  *iter++ = ')';

  std::array<char, max_decimal_digits_int64> buffer;
  auto buffer_digits_end =
    std::to_chars(buffer.begin(), buffer.end(), std::to_underlying(value)).ptr;
  return std::ranges::copy(buffer.begin(), buffer_digits_end, iter).out;
}
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_ENUM_IMPL_ENUM_ALT_NAME_HPP
