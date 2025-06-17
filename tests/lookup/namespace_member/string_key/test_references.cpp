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

namespace test_refs {
int v1 = 10;
int v2 = 20;
int v3 = 30;
int& ref1 = v1;
int& ref2 = v2;
int& ref3 = v3;
}  // namespace test_refs

TEST(NamespaceLookupTableByName, WithReferences) {
  constexpr auto table = LOOKUP_TABLE(test_refs, "*");
  EXPECT_EQ_STATIC(6, table.size());
  CHECK_VARIABLE(10, table["v1"]);
  CHECK_VARIABLE(20, table["v2"]);
  CHECK_VARIABLE(30, table["v3"]);
  CHECK_VARIABLE(10, table["ref1"]);
  CHECK_VARIABLE(20, table["ref2"]);
  CHECK_VARIABLE(30, table["ref3"]);
}
