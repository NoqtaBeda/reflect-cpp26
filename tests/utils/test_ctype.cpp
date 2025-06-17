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

#include <cctype>
#include <reflect_cpp26/utils/ctype.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

#define REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(F) \
  F(isalnum)                                      \
  F(isalpha)                                      \
  F(islower)                                      \
  F(isupper)                                      \
  F(isdigit)                                      \
  F(isxdigit)                                     \
  F(isblank)                                      \
  F(iscntrl)                                      \
  F(isgraph)                                      \
  F(isspace)                                      \
  F(isprint)                                      \
  F(ispunct)

TEST(UtilsCtype, CharacterPredicate) {
  for (unsigned char i = 0; i <= 127; i++) {
#define MAKE_EXPECT_EQ(fn) \
  EXPECT_EQ(!!std::fn(i), rfl::ascii_##fn(i)) << std::format("Wrong " #fn "('\\x{:x}')", i);

    REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(MAKE_EXPECT_EQ)
#undef MAKE_EXPECT_EQ
  }

#define MAKE_EXPECT_FALSE(fn)          \
  EXPECT_FALSE(rfl::ascii_##fn(value)) \
      << std::format("Wrong " #fn "({}): Failed to detect overflow.", value);

#define TEST_OVERFLOW(type, expr)                              \
  do {                                                         \
    auto value = static_cast<type>(expr);                      \
    REFLECT_CPP26_CTYPE_PREDICATE_FOR_EACH(MAKE_EXPECT_FALSE); \
  } while (false)

  TEST_OVERFLOW(int, -128);
  TEST_OVERFLOW(int8_t, -1);
  TEST_OVERFLOW(unsigned, 256 + 'A');
  TEST_OVERFLOW(int, -256 + 'A');

#undef TEST_OVERFLOW
#undef MAKE_EXPECT_FALSE
}

TEST(UtilsCtype, ToLowerOrUpper) {
  EXPECT_EQ_STATIC("hello world!", rfl::ascii_tolower("Hello World!"));
  EXPECT_EQ_STATIC(u"HELLO WORLD!", rfl::ascii_toupper(u"Hello World!"));

  auto vc = std::vector<char>{'I', ' ', 'a', 'm', ' ', 'B', 'o', 'b'};
  EXPECT_EQ("i am bob", rfl::ascii_tolower(vc));
  EXPECT_EQ("I AM BOB", rfl::ascii_toupper(vc));

  // Tests nullptr safety
  constexpr const wchar_t* null = nullptr;
  EXPECT_EQ_STATIC(L"", rfl::ascii_tolower(null));
  EXPECT_EQ_STATIC(L"", rfl::ascii_toupper(null));
}

TEST(UtilsCtype, Trim) {
  EXPECT_EQ_STATIC("hello world", rfl::ascii_trim(" \n\thello world\f\r\v "));
  EXPECT_EQ_STATIC(U"Hello World", rfl::ascii_trim(U"  Hello World    \n  "));
  EXPECT_EQ_STATIC(u"", rfl::ascii_trim(u" \n \t \r \f \v "));

  // Tests nullptr safety
  constexpr const wchar_t* null = nullptr;
  EXPECT_EQ_STATIC(L"", rfl::ascii_trim(null));
}
