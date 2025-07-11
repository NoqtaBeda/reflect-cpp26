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

#ifndef REFLECT_CPP26_UTILS_TO_STRING_UTILS_HPP
#define REFLECT_CPP26_UTILS_TO_STRING_UTILS_HPP

#include <reflect_cpp26/utils/config.h>
#include <cstddef>

namespace reflect_cpp26 {
// Including minus sign '-'
constexpr size_t max_decimal_digits_int8  = 4;  // -128
constexpr size_t max_decimal_digits_int16 = 6;  // -32768
constexpr size_t max_decimal_digits_int32 = 11; // -2147483648
constexpr size_t max_decimal_digits_int64 = 20; // -9223372036854775808

consteval size_t max_decimal_digits(size_t bytes)
{
  switch (bytes) {
    case 1:
      return max_decimal_digits_int8;
    case 2:
      return max_decimal_digits_int16;
    case 4:
      return max_decimal_digits_int32;
    case 8:
      return max_decimal_digits_int64;
    default:
      compile_error("Invalid size bytes.");
      return 0; // dummy
  }
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_TO_STRING_UTILS_HPP
