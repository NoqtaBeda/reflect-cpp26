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
#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/debug_helper.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <cctype>

namespace rfl = reflect_cpp26;

TEST(UtilsMisc, CType)
{
  for (unsigned char i = 0; i <= 127; i++) {
#define MAKE_EXPECT_EQ(fn)                            \
    EXPECT_EQ(!!std::fn(i), rfl::fn(i))               \
      << std::format("Wrong " #fn "('\\x{:x}')", i);

    REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(MAKE_EXPECT_EQ)
#undef MAKE_EXPECT_EQ
  }

#define MAKE_EXPECT_FALSE(fn)       \
  EXPECT_FALSE(rfl::fn(value))      \
    << std::format("Wrong " #fn "({}): Failed to detect overflow.", value);

#define TEST_OVERFLOW(type, expr)                             \
  do {                                                        \
    auto value = static_cast<type>(expr);                     \
    REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(MAKE_EXPECT_FALSE); \
  } while (false)

  TEST_OVERFLOW(int, -128);
  TEST_OVERFLOW(int8_t, -1);
  TEST_OVERFLOW(unsigned, 256 + 'A');
  TEST_OVERFLOW(int, -256 + 'A');

#undef TEST_OVERFLOW
#undef MAKE_EXPECT_FALSE
}

TEST(UtilsMisc, DebugHelper)
{
  EXPECT_THAT(rfl::type_name_of(0), testing::HasSubstr("int"));
  EXPECT_THAT(rfl::type_name_of<std::string>(), testing::HasSubstr("string"));
  auto sv = std::string_view{"abcde"};
  EXPECT_THAT(rfl::type_name_of(sv), testing::HasSubstr("string_view"));

  EXPECT_EQ("int", rfl::type_description_of<int>());
  EXPECT_THAT(rfl::type_description_of(sv),
    testing::ContainsRegex(R"(string_view.*\(defined in.*:[0-9]+\))"));
}

struct A {
  int a;
};

struct B : A {
  int b;
  unsigned x: 4;
  unsigned y: 4;
};

struct C : B {
  int c;

  static constexpr auto make(int a, int b, int c, unsigned x, unsigned y) -> C
  {
    auto res = C{};
    std::tie(res.a, res.b, res.c) = std::tuple(a, b, c);
    res.x = x;
    res.y = y;
    return res;
  }
};

TEST(UtilsMisc, MetaUtility)
{
  constexpr auto obj = C::make(1, 2, 3, 4, 5);
  EXPECT_EQ_STATIC(1, obj.[:rfl::reflect_pointer_to_member(&C::a):]);
  EXPECT_EQ_STATIC(2, obj.[:rfl::reflect_pointer_to_member(&C::b):]);
  EXPECT_EQ_STATIC(3, obj.[:rfl::reflect_pointer_to_member(&C::c):]);
}
