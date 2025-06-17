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

#include <reflect_cpp26/lookup/lookup_table.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

namespace test_template {
int x = 10;
int y = 20;

template <class T>
int count_of = 0;

constexpr long get_sum(long x, long y) {
  return x + y;
}

template <class T>
constexpr T get_sum_generic(T x, T y) {
  return x + y;
}

constexpr long get_product(long x, long y) {
  return x * y;
}
}  // namespace test_template

TEST(NamespaceLookupTableByName, WithTemplates) {
  constexpr auto table_v = LOOKUP_TABLE(test_template,
                                        "*",
                                        {
                                            .category = rfl::namespace_member_category::variables,
                                        });
  static_assert(std::is_same_v<int*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 2);

  CHECK_VARIABLE(10, table_v["x"]);
  CHECK_VARIABLE(20, table_v["y"]);
  EXPECT_EQ_STATIC(nullptr, table_v["count_of"]);

  constexpr auto table_f = LOOKUP_TABLE(test_template, "get_*");
  static_assert(std::is_same_v<long (*)(long, long), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 2);

  CHECK_FUNCTION(5, table_f["sum"], 2, 3);
  CHECK_FUNCTION(6, table_f["product"], 2, 3);
  EXPECT_EQ_STATIC(nullptr, table_f["sum_generic"]);
}
