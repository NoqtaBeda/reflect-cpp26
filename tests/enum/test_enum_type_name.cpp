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

#include <reflect_cpp26/enum/enum_type_name.hpp>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

using namespace reflect_cpp26;

static_assert("foo_signed" == enum_type_name_v<foo_signed>);
static_assert("foo_signed_reorder" == enum_type_name_v<const foo_signed_reorder>);
static_assert("foo_signed_rep" == enum_type_name_v<volatile foo_signed_rep>);
static_assert("bar_unsigned" == enum_type_name_v<const volatile bar_unsigned>);

template <size_t I>
struct indirect {};

template <>
struct indirect<0> {
  using type = baz_signed;
};

template <>
struct indirect<1> {
  using type = const qux_unsigned;
};

template <>
struct indirect<2> {
  using type = volatile empty;
};

template <>
struct indirect<3> {
  using type = const volatile single;
};

static_assert("baz_signed" == enum_type_name_v<indirect<0>::type>);
static_assert("baz_signed" == enum_type_name_v<const volatile indirect<0>::type>);
static_assert("qux_unsigned" == enum_type_name_v<indirect<1>::type>);
static_assert("qux_unsigned" == enum_type_name_v<const volatile indirect<1>::type>);
static_assert("empty" == enum_type_name_v<indirect<2>::type>);
static_assert("single" == enum_type_name_v<indirect<3>::type>);

TEST(EnumTypeName, StaticAll) {
  EXPECT_TRUE(true);  // All test cases done by static assertions above.
}
