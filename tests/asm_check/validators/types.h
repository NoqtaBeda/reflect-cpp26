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

#pragma once

#include <reflect_cpp26/validators/macros.h>
#include <reflect_cpp26/utils/config.h>
#include <stddef.h>

#ifdef __cplusplus
#include <numbers>
#endif

REFLECT_CPP26_EXTERN_C_BEGIN

typedef struct arithmetic_values_t {
  REFLECT_CPP26_VALIDATOR(min, 1)
  size_t size;
  REFLECT_CPP26_VALIDATOR(min_exclusive, 0.0)
  REFLECT_CPP26_VALIDATOR(max_exclusive, 1.0)
  double v1;
  REFLECT_CPP26_VALIDATOR(min, 1.0)
  REFLECT_CPP26_VALIDATOR(max, std::numbers::e)
  double v2;
} arithmetic_values_t;

int validate_arithmetic_values(const arithmetic_values_t* input);

int validate_arithmetic_values_with_error(
  const arithmetic_values_t* input,
  char* error_msg_buffer_begin, char* error_msg_buffer_end);

REFLECT_CPP26_EXTERN_C_END
