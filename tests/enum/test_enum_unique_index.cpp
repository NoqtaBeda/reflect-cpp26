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

#include <reflect_cpp26/enum/enum_index.hpp>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

using namespace reflect_cpp26;
using enum enum_entry_order;

template <class E>
constexpr void test_foo_signed_common() {
  ASSERT_EQ_STATIC(2, enum_unique_index(E::zero));
  ASSERT_EQ_STATIC(3, enum_unique_index(E::one));
  ASSERT_EQ_STATIC(4, enum_unique_index(E::two));
  ASSERT_EQ_STATIC(5, enum_unique_index(E::four));
  ASSERT_EQ_STATIC(6, enum_unique_index(E::five));
  ASSERT_EQ_STATIC(7, enum_unique_index(E::six));
  ASSERT_EQ_STATIC(8, enum_unique_index(E::seven));
  ASSERT_EQ_STATIC(1, enum_unique_index(E::invalid));
  ASSERT_EQ_STATIC(0, enum_unique_index(E::error));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<E>(-3)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<E>(3)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<E>(1 << 31)));
}

TEST(EnumUniqueIndex, FooSigned) {
  test_foo_signed_common<foo_signed>();
}

TEST(EnumUniqueIndex, FooSignedReorder) {
  test_foo_signed_common<foo_signed_reorder>();
}

TEST(EnumUniqueIndex, FooSignedRep) {
  ASSERT_EQ_STATIC(2, enum_unique_index(foo_signed_rep::zero));
  ASSERT_EQ_STATIC(3, enum_unique_index(foo_signed_rep::one));
  ASSERT_EQ_STATIC(4, enum_unique_index(foo_signed_rep::two));
  ASSERT_EQ_STATIC(5, enum_unique_index(foo_signed_rep::four));
  ASSERT_EQ_STATIC(6, enum_unique_index(foo_signed_rep::five));
  ASSERT_EQ_STATIC(7, enum_unique_index(foo_signed_rep::six));
  ASSERT_EQ_STATIC(8, enum_unique_index(foo_signed_rep::seven));
  ASSERT_EQ_STATIC(1, enum_unique_index(foo_signed_rep::invalid));
  ASSERT_EQ_STATIC(0, enum_unique_index(foo_signed_rep::error));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<foo_signed_rep>(-3)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<foo_signed_rep>(3)));
}

TEST(EnumUniqueIndex, BarUnsigned) {
  ASSERT_EQ_STATIC(0, enum_unique_index(bar_unsigned::zero));
  ASSERT_EQ_STATIC(1, enum_unique_index(bar_unsigned::one));
  ASSERT_EQ_STATIC(2, enum_unique_index(bar_unsigned::two));
  ASSERT_EQ_STATIC(3, enum_unique_index(bar_unsigned::three));
  ASSERT_EQ_STATIC(4, enum_unique_index(bar_unsigned::ten));
  ASSERT_EQ_STATIC(5, enum_unique_index(bar_unsigned::eleven));
  ASSERT_EQ_STATIC(6, enum_unique_index(bar_unsigned::twelve));
  ASSERT_EQ_STATIC(7, enum_unique_index(bar_unsigned::thirteen));
  ASSERT_EQ_STATIC(8, enum_unique_index(bar_unsigned::fourteen));
  ASSERT_EQ_STATIC(10, enum_unique_index(bar_unsigned::invalid));
  ASSERT_EQ_STATIC(9, enum_unique_index(bar_unsigned::error));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<bar_unsigned>(-3)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<bar_unsigned>(9)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<bar_unsigned>(1234)));
}

TEST(EnumUniqueIndex, BazSigned) {
  ASSERT_EQ_STATIC(1, enum_unique_index(baz_signed::ling));
  ASSERT_EQ_STATIC(2, enum_unique_index(baz_signed::yi));
  ASSERT_EQ_STATIC(3, enum_unique_index(baz_signed::er));
  ASSERT_EQ_STATIC(4, enum_unique_index(baz_signed::san));
  ASSERT_EQ_STATIC(0, enum_unique_index(baz_signed::fuyi));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<baz_signed>(-2)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<baz_signed>(4)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<baz_signed>(1 << 15)));
}

TEST(EnumUniqueIndex, QuxUnsigned) {
  ASSERT_EQ_STATIC(0, enum_unique_index(qux_unsigned::ling));
  ASSERT_EQ_STATIC(1, enum_unique_index(qux_unsigned::yi));
  ASSERT_EQ_STATIC(2, enum_unique_index(qux_unsigned::er));
  ASSERT_EQ_STATIC(3, enum_unique_index(qux_unsigned::san));
  ASSERT_EQ_STATIC(4, enum_unique_index(qux_unsigned::si));
  ASSERT_EQ_STATIC(5, enum_unique_index(qux_unsigned::fuyi));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<qux_unsigned>(-2)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<qux_unsigned>(5)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<qux_unsigned>(1 << 15)));
}

// Compilation error during static assertion since Clang 21:
// integer value -1 is outside the valid range of values [0, 1] for the
// enumeration type 'empty'
TEST(EnumUniqueIndex, Empty) {
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<empty>(0)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<empty>(1)));
  ASSERT_EQ(npos, enum_unique_index(static_cast<empty>(-1)));
}

TEST(EnumUniqueIndex, Single) {
  ASSERT_EQ_STATIC(0, enum_unique_index(static_cast<single>(233)));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<single>(0)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<single>(1)));
  ASSERT_EQ(npos, enum_unique_index(static_cast<single>(-1)));
}

TEST(EnumUniqueIndex, SingleRep) {
  ASSERT_EQ_STATIC(0, enum_unique_index(static_cast<single_rep>(233)));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<single_rep>(0)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<single_rep>(1)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<single_rep>(-1)));
}

TEST(EnumUniqueIndex, Color) {
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<color>(0x12345678)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<color>(0x98765432)));
}

TEST(EnumUniqueIndex, TerminalColor) {
  ASSERT_EQ_STATIC(0, enum_unique_index(terminal_color::black));
  ASSERT_EQ_STATIC(1, enum_unique_index(terminal_color::red));
  ASSERT_EQ_STATIC(2, enum_unique_index(terminal_color::green));
  ASSERT_EQ_STATIC(3, enum_unique_index(terminal_color::yellow));
  ASSERT_EQ_STATIC(4, enum_unique_index(terminal_color::blue));
  ASSERT_EQ_STATIC(5, enum_unique_index(terminal_color::magenta));
  ASSERT_EQ_STATIC(6, enum_unique_index(terminal_color::cyan));
  ASSERT_EQ_STATIC(7, enum_unique_index(terminal_color::white));
  ASSERT_EQ_STATIC(8, enum_unique_index(terminal_color::bright_black));
  ASSERT_EQ_STATIC(9, enum_unique_index(terminal_color::bright_red));
  ASSERT_EQ_STATIC(10, enum_unique_index(terminal_color::bright_green));
  ASSERT_EQ_STATIC(11, enum_unique_index(terminal_color::bright_yellow));
  ASSERT_EQ_STATIC(12, enum_unique_index(terminal_color::bright_blue));
  ASSERT_EQ_STATIC(13, enum_unique_index(terminal_color::bright_magenta));
  ASSERT_EQ_STATIC(14, enum_unique_index(terminal_color::bright_cyan));
  ASSERT_EQ_STATIC(15, enum_unique_index(terminal_color::bright_white));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<terminal_color>(0)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<terminal_color>(1)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<terminal_color>(40)));
}

TEST(EnumUniqueIndex, HashCollision) {
  ASSERT_EQ_STATIC(0, enum_unique_index(hash_collision::_wSYZDRpiQJf8Rfv));
  ASSERT_EQ_STATIC(1, enum_unique_index(hash_collision::_cuFFJIHGp_jNJKS));

  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<hash_collision>(-1)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<hash_collision>(2)));
  ASSERT_EQ_STATIC(npos, enum_unique_index(static_cast<hash_collision>(123)));
}
