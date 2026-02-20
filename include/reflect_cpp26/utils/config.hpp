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

#ifndef REFLECT_CPP26_UTILS_CONFIG_HPP
#define REFLECT_CPP26_UTILS_CONFIG_HPP

#if __cplusplus
#if __has_include(<meta>)
#include <meta>
#elif __has_include(<experimental/meta>)
#include <experimental/meta>
#else
#error "<meta> for C++26 Reflection is missing!"
#endif
#endif  // __cplusplus

#ifdef __clang__
#define REFLECT_CPP26_ALWAYS_INLINE __attribute__((always_inline))
#define REFLECT_CPP26_ALWAYS_INLINE_CALL [[clang::always_inline]]
#elifdef __GNUC__
#define REFLECT_CPP26_ALWAYS_INLINE __attribute__((always_inline))
#define REFLECT_CPP26_ALWAYS_INLINE_CALL
#else
#define REFLECT_CPP26_ALWAYS_INLINE
#define REFLECT_CPP26_ALWAYS_INLINE_CALL
#endif  // __cplusplus

#if __cplusplus
namespace reflect_cpp26 {
[[noreturn]] int compile_error(const char* msg) noexcept;
}  // namespace reflect_cpp26

#define REFLECT_CPP26_ERROR_IF_CONSTEVAL(msg) \
  do {                                        \
    if consteval {                            \
      ::reflect_cpp26::compile_error(msg);    \
    }                                         \
  } while (false)
#define REFLECT_CPP26_UNREACHABLE(msg)     \
  do {                                     \
    REFLECT_CPP26_ERROR_IF_CONSTEVAL(msg); \
    __builtin_unreachable();               \
  } while (false)
#else
#define REFLECT_CPP26_ERROR_IF_CONSTEVAL(msg)  // No-op
#define REFLECT_CPP26_UNREACHABLE(msg) __builtin_unreachable()
#endif  // __cplusplus

#endif  // REFLECT_CPP26_UTILS_CONFIG_HPP
