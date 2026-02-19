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

#include <reflect_cpp26/enum/enum_meta_entries.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

using namespace reflect_cpp26;

template <auto B>
struct some_type_t {
  static constexpr auto value = std::to_underlying(B) * std::to_underlying(B) + 2000;
};

TEST(EnumMetaEntries, All) {
  using enum enum_entry_order;

  static_assert("yi" == std::meta::identifier_of(enum_meta_entries_v<foo_signed_rep>[3]));
  static_assert("one"
                == std::meta::identifier_of(enum_meta_entries_v<const volatile foo_signed_rep>[1]));
  static_assert(
      "four" == std::meta::identifier_of(enum_meta_entries_v<const foo_signed_rep, by_value>[7]));
  static_assert(
      "yi" == std::meta::identifier_of(enum_meta_entries_v<volatile foo_signed_rep, by_name>[9]));

  static_assert("three" == std::meta::identifier_of(enum_meta_entries_v<bar_unsigned>[3]));
  static_assert("one"
                == std::meta::identifier_of(enum_meta_entries_v<const volatile bar_unsigned>[1]));
  static_assert("thirteen"
                == std::meta::identifier_of(enum_meta_entries_v<const bar_unsigned, by_value>[7]));
  static_assert(
      "two" == std::meta::identifier_of(enum_meta_entries_v<volatile bar_unsigned, by_name>[9]));

  constexpr auto some_type_twelve_param_v =
      extract<enum_meta_entries_v<bar_unsigned, by_name>[2]>();
  using some_type_twelve = some_type_t<some_type_twelve_param_v>;
  static_assert(2196 == some_type_twelve::value);
}
