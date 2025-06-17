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

struct test_deleted_t {
  int x;
  int y;

  constexpr int min() const {
    return std::min(x, y);
  }

  int min_with(int z) const = delete;

  constexpr int max() const {
    return std::max(x, y);
  }

  int max_with(int z) const = delete;
};

TEST(ClassLookupTableByName, WithDeleted) {
  constexpr auto table_fn =
      LOOKUP_TABLE(test_deleted_t,
                   "*",
                   {
                       .category = rfl::class_member_category::nonstatic_member_functions,
                   });
  static_assert(std::is_same_v<int (test_deleted_t::*)() const, decltype(table_fn)::value_type>);
  static_assert(table_fn.size() == 2);

  constexpr auto foo = test_deleted_t{.x = 12, .y = 34};
  CHECK_MEMBER_FUNCTION_STATIC(12, foo, table_fn["min"]);
  CHECK_MEMBER_FUNCTION_STATIC(34, foo, table_fn["max"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["min_with"]);
  EXPECT_EQ_STATIC(nullptr, table_fn["max_with"]);
}
