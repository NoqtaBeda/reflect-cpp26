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

#ifndef REFLECT_CPP26_UTILS_CONFIG_H
#define REFLECT_CPP26_UTILS_CONFIG_H

#if __cplusplus
#if __has_include(<meta>)
#include <meta>
#elif __has_include(<experimental/meta>)
#include <experimental/meta>
#else
#error "<meta> for C++26 Reflection is missing!"
#endif
#endif // __cplusplus

#if __cplusplus
#define REFLECT_CPP26_BOOL bool
#define REFLECT_CPP26_EXTERN_C_BEGIN extern "C" {
#define REFLECT_CPP26_EXTERN_C_END }
#else
#define REFLECT_CPP26_BOOL _Bool
#define REFLECT_CPP26_EXTERN_C_BEGIN
#define REFLECT_CPP26_EXTERN_C_END
#endif

#if __cplusplus
namespace reflect_cpp26 {
[[noreturn]] int compile_error(const char* msg) noexcept;

template <class... Args>
[[noreturn]] int debug_compile_error_impl(Args... args) noexcept;

/**
 * Compile-time debug helper to dump args... in error stacktrace.
 */
template <class... Args>
consteval int debug_compile_error(Args... args) noexcept {
  return debug_compile_error_impl(args...);
}
} // namespace reflect_cpp26
#define REFLECT_CPP26_ERROR_IF_CONSTEVAL(msg) \
  do {                                        \
    if consteval {                            \
      ::reflect_cpp26::compile_error(msg);    \
    }                                         \
  } while (false)
#define REFLECT_CPP26_UNREACHABLE(msg)      \
  do {                                      \
    REFLECT_CPP26_ERROR_IF_CONSTEVAL(msg);  \
    __builtin_unreachable();                \
  } while (false)
#else
#define REFLECT_CPP26_ERROR_IF_CONSTEVAL(msg) // No-op
#define REFLECT_CPP26_UNREACHABLE(msg) __builtin_unreachable()
#endif

#endif // REFLECT_CPP26_UTILS_CONFIG_H
