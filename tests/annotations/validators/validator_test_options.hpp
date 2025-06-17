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

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/annotations.hpp>
#else
#include <reflect_cpp26/annotations/macros.h>
#include <reflect_cpp26/annotations/validators.hpp>
#endif

#define VALIDATOR(...) REFLECT_CPP26_VALIDATOR(__VA_ARGS__)

namespace rfl = reflect_cpp26;
namespace annots = reflect_cpp26::annotations;
using namespace std::literals;

#define LAZY_OBJECT(name, ...)                          \
  constexpr auto name = []() { return (__VA_ARGS__); }

template <class LazyFn>
constexpr bool do_validate_members(LazyFn fn) {
  return annots::validate_members(fn());
}

template <class LazyFn>
constexpr auto validation_error_message(LazyFn fn) -> std::string
{
  auto msg = std::string{};
  auto obj = fn();
  annots::validate_members_with_error_info(obj, &msg);
  return msg;
}

template <class LazyFn>
constexpr auto validation_full_error_message(LazyFn fn) -> std::string
{
  auto msg = std::string{};
  auto obj = fn();
  annots::validate_members_with_full_error_info(obj, &msg);
  return msg;
}
