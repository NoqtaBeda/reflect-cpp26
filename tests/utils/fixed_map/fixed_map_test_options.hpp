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

#include "tests/test_options.hpp"

constexpr auto magic_value = 0x12345678;

template <class T>
struct wrapper_t {
  T value = magic_value;

  constexpr auto operator<=>(const wrapper_t&) const = default;
  constexpr bool operator==(const wrapper_t&) const = default;

  constexpr auto operator<=>(T rhs) const {
    return value <=> rhs;
  }
  constexpr bool operator==(T rhs) const {
    return value == rhs;
  }
};

#define EXPECT_FOUND(expected_res, flat_map, key) \
  do {                                            \
    auto p = flat_map.get(key);                   \
    EXPECT_TRUE(p.second);                        \
    EXPECT_EQ(expected_res, p.first);             \
  } while (false)

#define EXPECT_NOT_FOUND(expected_res, flat_map, key) \
  do {                                                \
    auto p = flat_map.get(key);                       \
    EXPECT_FALSE(p.second);                           \
    EXPECT_EQ(expected_res, p.first);                 \
  } while (false)

#define EXPECT_FOUND_STATIC(expected_res, flat_map, key)  \
  do {                                                    \
    constexpr auto p = flat_map.get(key);                 \
    EXPECT_TRUE_STATIC(p.second);                         \
    EXPECT_EQ_STATIC(expected_res, p.first);              \
  } while (false)

#define EXPECT_NOT_FOUND_STATIC(expected_res, flat_map, key)  \
  do {                                                        \
    constexpr auto p = flat_map.get(key);                     \
    EXPECT_FALSE_STATIC(p.second);                            \
    EXPECT_EQ_STATIC(expected_res, p.first);                  \
  } while (false)