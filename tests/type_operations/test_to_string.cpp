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

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_operations.hpp>
#else
#include <reflect_cpp26/type_operations/to_string.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_1_t {
  std::vector<int> vec;
  std::array<int, 4> std_arr;
  int c_arr[4];
};

TEST(TypeOperationsToString, IntRange)
{
  auto foo = foo_1_t{
    .vec = {-1, 1, -2, 2, -3, 3},
    .std_arr = {1, -2, 3, -4},
    .c_arr = {-1, -2, -3, -4},
  };
  EXPECT_EQ("[-1, 1, -2, 2, -3, 3]", rfl::generic_to_string(foo.vec));
  EXPECT_EQ("[1, -2, 3, -4]", rfl::generic_to_string(foo.std_arr));
  EXPECT_EQ("[-1, -2, -3, -4]", rfl::generic_to_string(foo.c_arr));

  EXPECT_EQ("[-1, 1, -2, 2, -3, 3]",
    rfl::generic_to_display_string(foo.vec));
  EXPECT_EQ("[1, -2, 3, -4]",
    rfl::generic_to_display_string(foo.std_arr));
  EXPECT_EQ("[-1, -2, -3, -4]",
    rfl::generic_to_display_string(foo.c_arr));
}

struct foo_2_t {
  std::vector<std::string> vec;
  std::array<std::string_view, 4> std_arr;
  const char* c_arr[4];
};

TEST(TypeOperationsToString, StringRange)
{
  auto foo = foo_2_t{
    .vec = {"abc", "def", "ghi\tjkl"},
    .std_arr = {"Hello", "World", "and", "C++"},
    .c_arr = {"foo", "bar", "i love you", "n\nt\t"},
  };
  EXPECT_EQ("[abc, def, ghi\tjkl]", rfl::generic_to_string(foo.vec));
  EXPECT_EQ("[Hello, World, and, C++]", rfl::generic_to_string(foo.std_arr));
  EXPECT_EQ("[foo, bar, i love you, n\nt\t]",
    rfl::generic_to_string(foo.c_arr));

  EXPECT_EQ("[\"abc\", \"def\", \"ghi\\tjkl\"]",
    rfl::generic_to_display_string(foo.vec));
  EXPECT_EQ("[\"Hello\", \"World\", \"and\", \"C++\"]",
    rfl::generic_to_display_string(foo.std_arr));
  EXPECT_EQ("[\"foo\", \"bar\", \"i love you\", \"n\\nt\\t\"]",
    rfl::generic_to_display_string(foo.c_arr));
}
