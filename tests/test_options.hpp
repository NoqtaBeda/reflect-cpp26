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

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

// Note: Keyword 'not' may be inavailable in MSVC.
#define NOT !

#ifdef ENABLE_STATIC_TEST
#define CONSTEVAL_ON_STATIC_TEST consteval

#define ASSERT_EQ_STATIC(expected, ...) static_assert(expected == (__VA_ARGS__))
#define EXPECT_EQ_STATIC(expected, ...) static_assert(expected == (__VA_ARGS__))
#define ASSERT_NE_STATIC(expected, ...) static_assert(expected != (__VA_ARGS__))
#define EXPECT_NE_STATIC(expected, ...) static_assert(expected != (__VA_ARGS__))
#define ASSERT_FALSE_STATIC(...) static_assert(!(__VA_ARGS__))
#define EXPECT_FALSE_STATIC(...) static_assert(!(__VA_ARGS__))
#define ASSERT_TRUE_STATIC(...) static_assert(__VA_ARGS__)
#define EXPECT_TRUE_STATIC(...) static_assert(__VA_ARGS__)
#else
#define CONSTEVAL_ON_STATIC_TEST

#define ASSERT_EQ_STATIC(expected, ...) ASSERT_EQ(expected, (__VA_ARGS__))
#define EXPECT_EQ_STATIC(expected, ...) EXPECT_EQ(expected, (__VA_ARGS__))
#define ASSERT_NE_STATIC(expected, ...) ASSERT_NE(expected, (__VA_ARGS__))
#define EXPECT_NE_STATIC(expected, ...) EXPECT_NE(expected, (__VA_ARGS__))
#define ASSERT_FALSE_STATIC(...) ASSERT_FALSE((__VA_ARGS__))
#define EXPECT_FALSE_STATIC(...) EXPECT_FALSE((__VA_ARGS__))
#define ASSERT_TRUE_STATIC(...) ASSERT_TRUE((__VA_ARGS__))
#define EXPECT_TRUE_STATIC(...) EXPECT_TRUE((__VA_ARGS__))
#endif
