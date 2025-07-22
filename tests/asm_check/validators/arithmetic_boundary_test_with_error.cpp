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

#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#include <reflect_cpp26/validators/validate.hpp>
#include "tests/asm_check/validators/types.h"

namespace rfl = reflect_cpp26;

__attribute__((visibility("default")))
extern "C" int validate_arithmetic_values_with_error(
  const arithmetic_values_t* input,
  char* error_msg_buffer_begin, char* error_msg_buffer_end)
{
  if (error_msg_buffer_begin == nullptr || error_msg_buffer_end == nullptr) {
    return rfl::validate_public_members(*input);
  }
  if (error_msg_buffer_end - error_msg_buffer_begin <= 1) {
    return rfl::validate_public_members(*input);
  }
  auto error_msg = std::string{};
  auto res = rfl::validate_public_members(*input, &error_msg);

  auto error_msg_length = std::min<size_t>(
    error_msg.length(), error_msg_buffer_end - error_msg_buffer_begin - 1);
  auto [_, end_pos] = std::ranges::copy_n(
    error_msg.begin(), error_msg_length, error_msg_buffer_begin);
  *end_pos = '\0';
  return res;
}
