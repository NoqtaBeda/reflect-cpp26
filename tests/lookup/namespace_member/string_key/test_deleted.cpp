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

namespace test_deleted {
bool write_sum(int x, int y, int* dest) {
  if (dest != nullptr) {
    *dest = x + y;
    return true;
  }
  return false;
}

bool write_difference(int x, int y, int* dest) {
  if (dest != nullptr) {
    *dest = x - y;
    return true;
  }
  return false;
}

bool write_product(int x, int y, int* dest) = delete ("Expected to be excluded");
}  // namespace test_deleted

TEST(NamespaceLookupTableByName, WithDeleted) {
  constexpr auto table_f = LOOKUP_TABLE(test_deleted, "write_*");
  static_assert(std::is_same_v<bool (*)(int, int, int*), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 2);

  int dest = 0;
  ASSERT_NE_STATIC(nullptr, table_f["sum"]);
  CHECK_FUNCTION(true, table_f["sum"], 2, 3, &dest);
  EXPECT_EQ(5, dest);
  dest = 0;
  ASSERT_NE_STATIC(nullptr, table_f["difference"]);
  CHECK_FUNCTION(true, table_f["difference"], 2, 3, &dest);
  EXPECT_EQ(-1, dest);

  EXPECT_EQ_STATIC(nullptr, table_f[""]);
  EXPECT_EQ_STATIC(nullptr, table_f["product"]);
  EXPECT_EQ_STATIC(nullptr, table_f["write_sum"]);
  EXPECT_EQ_STATIC(nullptr, table_f["write_product"]);
}
