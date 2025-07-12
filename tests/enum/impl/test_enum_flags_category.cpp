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

#include "tests/enum/flags_test_cases.hpp"
#include "tests/test_options.hpp"
#include <reflect_cpp26/enum/impl/enum_flags.hpp>

namespace impl = reflect_cpp26::impl;

TEST(EnumFlagsCategory, Empty)
{
  static_assert(NOT impl::enum_flags_is_empty_v<D1>);
  static_assert(NOT impl::enum_flags_is_empty_v<D2>);
  static_assert(NOT impl::enum_flags_is_empty_v<D3>);
  static_assert(NOT impl::enum_flags_is_empty_v<D4>);
  static_assert(NOT impl::enum_flags_is_empty_v<D5>);
  static_assert(NOT impl::enum_flags_is_empty_v<E1>);
  static_assert(NOT impl::enum_flags_is_empty_v<E2>);
  static_assert(impl::enum_flags_is_empty_v<empty>);
  static_assert(NOT impl::enum_flags_is_empty_v<single_one>);
  static_assert(NOT impl::enum_flags_is_empty_v<single_one_rep>);
  static_assert(impl::enum_flags_is_empty_v<single_zero>);
  static_assert(impl::enum_flags_is_empty_v<single_zero_rep>);
}

TEST(EnumFlagsCategory, Regular)
{
  static_assert(impl::enum_flags_is_regular_v<D1>);
  static_assert(impl::enum_flags_is_regular_v<D2>);
  static_assert(impl::enum_flags_is_regular_v<D3>);
  static_assert(impl::enum_flags_is_regular_v<D4>);
  static_assert(impl::enum_flags_is_regular_v<D5>);
  static_assert(NOT impl::enum_flags_is_regular_v<E1>);
  static_assert(NOT impl::enum_flags_is_regular_v<E2>);
  static_assert(NOT impl::enum_flags_is_regular_v<empty>);
  static_assert(impl::enum_flags_is_regular_v<single_one>);
  static_assert(impl::enum_flags_is_regular_v<single_one_rep>);
  static_assert(NOT impl::enum_flags_is_regular_v<single_zero>);
  static_assert(NOT impl::enum_flags_is_regular_v<single_zero_rep>);
}

TEST(EnumFlagsCategory, Irregular)
{
  static_assert(NOT impl::enum_flags_is_irregular_v<D1>);
  static_assert(NOT impl::enum_flags_is_irregular_v<D2>);
  static_assert(NOT impl::enum_flags_is_irregular_v<D3>);
  static_assert(NOT impl::enum_flags_is_irregular_v<D4>);
  static_assert(NOT impl::enum_flags_is_irregular_v<D5>);
  static_assert(impl::enum_flags_is_irregular_v<E1>);
  static_assert(impl::enum_flags_is_irregular_v<E2>);
  static_assert(NOT impl::enum_flags_is_irregular_v<empty>);
  static_assert(NOT impl::enum_flags_is_irregular_v<single_one>);
  static_assert(NOT impl::enum_flags_is_irregular_v<single_one_rep>);
  static_assert(NOT impl::enum_flags_is_irregular_v<single_zero>);
  static_assert(NOT impl::enum_flags_is_irregular_v<single_zero_rep>);
}
