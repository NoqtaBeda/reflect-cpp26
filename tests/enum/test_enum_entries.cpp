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

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_entries.hpp>
#endif

using namespace reflect_cpp26;

template <class E>
constexpr auto enum_entry_by_value(size_t index) {
  return enum_entries_v<E, enum_entry_order::by_value>[index];
}

template <class E>
constexpr auto enum_entry_by_name(size_t index) {
  return enum_entries_v<E, enum_entry_order::by_name>[index];
}

TEST(EnumEntries, All)
{
  EXPECT_EQ_STATIC(
    std::pair(foo_signed_rep::yi, "yi"),
    enum_entries_v<foo_signed_rep>[3]);
  EXPECT_EQ_STATIC(
    std::pair(foo_signed_rep::one, "one"),
    enum_entries_v<const volatile foo_signed_rep>[1]);
  EXPECT_EQ_STATIC(
    std::pair(foo_signed_rep::four, "four"),
    enum_entry_by_value<const foo_signed_rep>(7));
  EXPECT_EQ_STATIC(
    std::pair(foo_signed_rep::yi, "yi"),
    enum_entry_by_name<volatile foo_signed_rep>(9));
}
