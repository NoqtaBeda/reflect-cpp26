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

#include <reflect_cpp26/utils/meta_utility.hpp>
#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_meta_entries.hpp>
#endif

using namespace reflect_cpp26;

template <auto B>
struct some_type_t {
  static constexpr auto value =
    std::to_underlying(B) * std::to_underlying(B) + 2000;
};

TEST(EnumMetaEntries, All)
{
  EXPECT_EQ_STATIC("yi", std::meta::identifier_of(
    enum_meta_entry<foo_signed_rep>(3)));
  EXPECT_EQ_STATIC("one", std::meta::identifier_of(
    enum_meta_entry<const volatile foo_signed_rep>(1)));
  EXPECT_EQ_STATIC("four", std::meta::identifier_of(
    enum_meta_entry<const foo_signed_rep, enum_entry_order::by_value>(7)));
  EXPECT_EQ_STATIC("yi", std::meta::identifier_of(
    enum_meta_entry<volatile foo_signed_rep, enum_entry_order::by_name>(9)));
  
  EXPECT_EQ_STATIC("three", std::meta::identifier_of(
    enum_meta_entry<bar_unsigned>(3)));
  EXPECT_EQ_STATIC("one", std::meta::identifier_of(
    enum_meta_entry<const volatile bar_unsigned>(1)));
  EXPECT_EQ_STATIC("thirteen", std::meta::identifier_of(
    enum_meta_entry<const bar_unsigned, enum_entry_order::by_value>(7)));
  EXPECT_EQ_STATIC("two", std::meta::identifier_of(
    enum_meta_entry<volatile bar_unsigned, enum_entry_order::by_name>(9)));

  constexpr auto some_type_twelve_param_v =
    extract<enum_meta_entry<bar_unsigned, enum_entry_order::by_name>(2)>();
  using some_type_twelve = some_type_t<some_type_twelve_param_v>;
  EXPECT_EQ_STATIC(2196, some_type_twelve::value);
}
