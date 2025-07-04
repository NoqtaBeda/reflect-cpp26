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

#include "tests/utils/fixed_map/fixed_map_test_options.hpp"
#include <string>
#include <string_view>
#include <ranges>

#define FIXED_MAP(...) REFLECT_CPP26_STRING_KEY_FIXED_MAP(__VA_ARGS__)

// #define FOR_EACH_CHARACTER_TYPE(F)  \
//   F(char, Char)                     \
//   F(wchar_t, WChar)                 \
//   F(char8_t, Char8)                 \
//   F(char16_t, Char16)               \
//   F(char32_t, Char32)

template <class ToCharT, class FromCharT>
constexpr auto to(const FromCharT* str) -> std::basic_string<ToCharT>
{
  auto from_sv = std::basic_string_view<FromCharT>(str);
  auto transform_fn = [](FromCharT c) {
    return static_cast<ToCharT>(c);
  };
  return {std::from_range, from_sv | std::views::transform(transform_fn)};
}