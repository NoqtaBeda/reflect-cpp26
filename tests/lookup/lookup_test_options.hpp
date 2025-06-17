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

#pragma once

#include <reflect_cpp26/utils/config.hpp>

#include "tests/test_options.hpp"

#define CHECK_MEMBER_VARIABLE(expected, obj, mptr) \
  ASSERT_NE(nullptr, (mptr));                      \
  EXPECT_EQ((expected), (obj).*(mptr))

#ifdef ENABLE_STATIC_TEST
#define CHECK_MEMBER_VARIABLE_STATIC(expected, obj, mptr) \
  static_assert((mptr) != nullptr);                       \
  static_assert((expected) == (obj).*(mptr))
#else
#define CHECK_MEMBER_VARIABLE_STATIC(expected, obj, mptr) CHECK_MEMBER_VARIABLE(expected, obj, mptr)
#endif

#define CHECK_MEMBER_FUNCTION(expected, obj, mptr, ...) \
  ASSERT_NE(nullptr, (mptr));                           \
  EXPECT_EQ((expected), ((obj).*(mptr))(__VA_ARGS__));

#ifdef ENABLE_STATIC_TEST
#define CHECK_MEMBER_FUNCTION_STATIC(expected, obj, mptr, ...) \
  static_assert((mptr) != nullptr);                            \
  static_assert((expected) == ((obj).*(mptr))(__VA_ARGS__))
#else
#define CHECK_MEMBER_FUNCTION_STATIC(expected, obj, mptr, ...) \
  CHECK_MEMBER_FUNCTION(expected, obj, mptr, __VA_ARGS__)
#endif

#define CHECK_VARIABLE(expected, ptr) \
  ASSERT_NE(nullptr, (ptr));          \
  EXPECT_EQ((expected), *(ptr))

#ifdef ENABLE_STATIC_TEST
#define CHECK_VARIABLE_STATIC(expected, ptr) \
  static_assert((ptr) != nullptr);           \
  static_assert((expected) == *(ptr))
#else
#define CHECK_VARIABLE_STATIC(expected, ptr) CHECK_VARIABLE(expected, ptr);
#endif

#define CHECK_FUNCTION(expected, fptr, ...) \
  ASSERT_NE(nullptr, (fptr));               \
  EXPECT_EQ((expected), (fptr)(__VA_ARGS__))

#ifdef ENABLE_STATIC_TEST
#define CHECK_FUNCTION_STATIC(expected, fptr, ...) \
  static_assert((fptr) != nullptr);                \
  static_assert((expected) == (fptr)(__VA_ARGS__))
#else
#define CHECK_FUNCTION_STATIC(expected, fptr, ...) CHECK_FUNCTION(expected, fptr, __VA_ARGS__)
#endif
