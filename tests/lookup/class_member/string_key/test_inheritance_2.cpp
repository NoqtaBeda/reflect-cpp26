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

#define LOOKUP_TABLE(...) REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

struct A : std::monostate {
  long a_count;
};
struct B {
  long b_count;
};
struct C {
  long c_count;
};
struct D : A, B, C {
  static constexpr auto make(long a, long b, long c) -> D {
    auto res = D{};
    res.a_count = a;
    res.b_count = b;
    res.c_count = c;
    return res;
  }
};

TEST(ClassLookupTableByName, Inheritance2) {
  constexpr auto table_value = LOOKUP_TABLE(D, "*_count");
  static_assert(std::is_same_v<long D::*, decltype(table_value)::value_type>);
  static_assert(table_value.size() == 3);

  constexpr auto d = D::make(1, 3, 5);
  CHECK_MEMBER_VARIABLE_STATIC(1, d, table_value["a"]);
  CHECK_MEMBER_VARIABLE_STATIC(3, d, table_value["b"]);
  CHECK_MEMBER_VARIABLE_STATIC(5, d, table_value["c"]);
  EXPECT_EQ_STATIC(nullptr, table_value["a_count"]);
  EXPECT_EQ_STATIC(nullptr, table_value["d"]);
  EXPECT_EQ_STATIC(nullptr, table_value[""]);
}
