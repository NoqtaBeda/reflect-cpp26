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

#include <reflect_cpp26/utils/debug_helper.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

TEST(UtilsDebugHelper, All) {
  EXPECT_THAT(rfl::type_name_of(0), testing::HasSubstr("int"));
  EXPECT_THAT(rfl::type_name_of<std::string>(), testing::HasSubstr("string"));
  auto sv = std::string_view{"abcde"};
  EXPECT_THAT(rfl::type_name_of(sv), testing::HasSubstr("string_view"));

  EXPECT_EQ("int", rfl::type_description_of<int>());
  EXPECT_THAT(rfl::type_description_of(sv),
              testing::ContainsRegex(R"(string_view.*\(defined in.*:[0-9]+\))"));
}
