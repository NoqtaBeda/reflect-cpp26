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

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_index.hpp>
#endif

using namespace reflect_cpp26;
using enum enum_entry_order;

template <class E>
constexpr void test_foo_signed_common() {
  ASSERT_EQ_STATIC(2, enum_index_by<by_value>(E::zero));
  ASSERT_EQ_STATIC(3, enum_index_by<by_value>(E::one));
  ASSERT_EQ_STATIC(4, enum_index_by<by_value>(E::two));
  ASSERT_EQ_STATIC(5, enum_index_by<by_value>(E::four));
  ASSERT_EQ_STATIC(6, enum_index_by<by_value>(E::five));
  ASSERT_EQ_STATIC(7, enum_index_by<by_value>(E::six));
  ASSERT_EQ_STATIC(8, enum_index_by<by_value>(E::seven));
  ASSERT_EQ_STATIC(1, enum_index_by<by_value>(E::invalid));
  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(E::error));

  ASSERT_EQ_STATIC(0, enum_index_by<by_name>(E::error));
  ASSERT_EQ_STATIC(1, enum_index_by<by_name>(E::five));
  ASSERT_EQ_STATIC(2, enum_index_by<by_name>(E::four));
  ASSERT_EQ_STATIC(3, enum_index_by<by_name>(E::invalid));
  ASSERT_EQ_STATIC(4, enum_index_by<by_name>(E::one));
  ASSERT_EQ_STATIC(5, enum_index_by<by_name>(E::seven));
  ASSERT_EQ_STATIC(6, enum_index_by<by_name>(E::six));
  ASSERT_EQ_STATIC(7, enum_index_by<by_name>(E::two));
  ASSERT_EQ_STATIC(8, enum_index_by<by_name>(E::zero));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<E>(-3)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<E>(3)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<E>(1 << 31)));
}

TEST(EnumIndex, FooSigned) {
  test_foo_signed_common<foo_signed>();
  ASSERT_EQ_STATIC(0, enum_index(foo_signed::zero));
  ASSERT_EQ_STATIC(1, enum_index(foo_signed::one));
  ASSERT_EQ_STATIC(2, enum_index(foo_signed::two));
  ASSERT_EQ_STATIC(3, enum_index(foo_signed::four));
  ASSERT_EQ_STATIC(4, enum_index(foo_signed::five));
  ASSERT_EQ_STATIC(5, enum_index(foo_signed::six));
  ASSERT_EQ_STATIC(6, enum_index(foo_signed::seven));
  ASSERT_EQ_STATIC(7, enum_index(foo_signed::invalid));
  ASSERT_EQ_STATIC(8, enum_index(foo_signed::error));
}

TEST(EnumIndex, FooSignedReorder) {
  test_foo_signed_common<foo_signed_reorder>();
  ASSERT_EQ_STATIC(0, enum_index(foo_signed_reorder::two));
  ASSERT_EQ_STATIC(1, enum_index(foo_signed_reorder::five));
  ASSERT_EQ_STATIC(2, enum_index(foo_signed_reorder::zero));
  ASSERT_EQ_STATIC(3, enum_index(foo_signed_reorder::seven));
  ASSERT_EQ_STATIC(4, enum_index(foo_signed_reorder::one));
  ASSERT_EQ_STATIC(5, enum_index(foo_signed_reorder::six));
  ASSERT_EQ_STATIC(6, enum_index(foo_signed_reorder::invalid));
  ASSERT_EQ_STATIC(7, enum_index(foo_signed_reorder::four));
  ASSERT_EQ_STATIC(8, enum_index(foo_signed_reorder::error));
}

TEST(EnumIndex, FooSignedRep) {
  ASSERT_EQ_STATIC(0, enum_index(foo_signed_rep::zero));
  ASSERT_THAT(enum_index(foo_signed_rep::one), testing::AnyOf(1, 3));
  ASSERT_THAT(enum_index(foo_signed_rep::two), testing::AnyOf(2, 4));
  ASSERT_EQ_STATIC(5, enum_index(foo_signed_rep::four));
  ASSERT_EQ_STATIC(6, enum_index(foo_signed_rep::five));
  ASSERT_EQ_STATIC(7, enum_index(foo_signed_rep::six));
  ASSERT_EQ_STATIC(8, enum_index(foo_signed_rep::seven));
  ASSERT_EQ_STATIC(9, enum_index(foo_signed_rep::invalid));
  ASSERT_EQ_STATIC(10, enum_index(foo_signed_rep::error));

  ASSERT_EQ_STATIC(2, enum_index_by<by_value>(foo_signed_rep::zero));
  ASSERT_EQ_STATIC(3, enum_index_by<by_value>(foo_signed_rep::one));
  ASSERT_EQ_STATIC(5, enum_index_by<by_value>(foo_signed_rep::two));
  ASSERT_EQ_STATIC(7, enum_index_by<by_value>(foo_signed_rep::four));
  ASSERT_EQ_STATIC(8, enum_index_by<by_value>(foo_signed_rep::five));
  ASSERT_EQ_STATIC(9, enum_index_by<by_value>(foo_signed_rep::six));
  ASSERT_EQ_STATIC(10, enum_index_by<by_value>(foo_signed_rep::seven));
  ASSERT_EQ_STATIC(1, enum_index_by<by_value>(foo_signed_rep::invalid));
  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(foo_signed_rep::error));

  ASSERT_THAT(enum_index_by<by_name>(foo_signed_rep::er), testing::AnyOf(0, 8));
  ASSERT_EQ_STATIC(1, enum_index_by<by_name>(foo_signed_rep::error));
  ASSERT_EQ_STATIC(2, enum_index_by<by_name>(foo_signed_rep::five));
  ASSERT_EQ_STATIC(3, enum_index_by<by_name>(foo_signed_rep::four));
  ASSERT_EQ_STATIC(4, enum_index_by<by_name>(foo_signed_rep::invalid));
  ASSERT_THAT(enum_index_by<by_name>(foo_signed_rep::one), testing::AnyOf(5, 9));
  ASSERT_EQ_STATIC(6, enum_index_by<by_name>(foo_signed_rep::seven));
  ASSERT_EQ_STATIC(7, enum_index_by<by_name>(foo_signed_rep::six));
  ASSERT_EQ_STATIC(10, enum_index_by<by_name>(foo_signed_rep::zero));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<foo_signed_rep>(-3)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<foo_signed_rep>(3)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<foo_signed_rep>(123)));
}

TEST(EnumIndex, BarUnsigned) {
  ASSERT_EQ_STATIC(0, enum_index(bar_unsigned::zero));
  ASSERT_EQ_STATIC(1, enum_index(bar_unsigned::one));
  ASSERT_EQ_STATIC(2, enum_index(bar_unsigned::two));
  ASSERT_EQ_STATIC(3, enum_index(bar_unsigned::three));
  ASSERT_EQ_STATIC(4, enum_index(bar_unsigned::ten));
  ASSERT_EQ_STATIC(5, enum_index(bar_unsigned::eleven));
  ASSERT_EQ_STATIC(6, enum_index(bar_unsigned::twelve));
  ASSERT_EQ_STATIC(7, enum_index(bar_unsigned::thirteen));
  ASSERT_EQ_STATIC(8, enum_index(bar_unsigned::fourteen));
  ASSERT_EQ_STATIC(9, enum_index(bar_unsigned::invalid));
  ASSERT_EQ_STATIC(10, enum_index(bar_unsigned::error));

  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(bar_unsigned::zero));
  ASSERT_EQ_STATIC(1, enum_index_by<by_value>(bar_unsigned::one));
  ASSERT_EQ_STATIC(2, enum_index_by<by_value>(bar_unsigned::two));
  ASSERT_EQ_STATIC(3, enum_index_by<by_value>(bar_unsigned::three));
  ASSERT_EQ_STATIC(4, enum_index_by<by_value>(bar_unsigned::ten));
  ASSERT_EQ_STATIC(5, enum_index_by<by_value>(bar_unsigned::eleven));
  ASSERT_EQ_STATIC(6, enum_index_by<by_value>(bar_unsigned::twelve));
  ASSERT_EQ_STATIC(7, enum_index_by<by_value>(bar_unsigned::thirteen));
  ASSERT_EQ_STATIC(8, enum_index_by<by_value>(bar_unsigned::fourteen));
  ASSERT_EQ_STATIC(10, enum_index_by<by_value>(bar_unsigned::invalid));
  ASSERT_EQ_STATIC(9, enum_index_by<by_value>(bar_unsigned::error));

  ASSERT_EQ_STATIC(0, enum_index_by<by_name>(bar_unsigned::eleven));
  ASSERT_EQ_STATIC(1, enum_index_by<by_name>(bar_unsigned::error));
  ASSERT_EQ_STATIC(2, enum_index_by<by_name>(bar_unsigned::fourteen));
  ASSERT_EQ_STATIC(3, enum_index_by<by_name>(bar_unsigned::invalid));
  ASSERT_EQ_STATIC(4, enum_index_by<by_name>(bar_unsigned::one));
  ASSERT_EQ_STATIC(5, enum_index_by<by_name>(bar_unsigned::ten));
  ASSERT_EQ_STATIC(6, enum_index_by<by_name>(bar_unsigned::thirteen));
  ASSERT_EQ_STATIC(7, enum_index_by<by_name>(bar_unsigned::three));
  ASSERT_EQ_STATIC(8, enum_index_by<by_name>(bar_unsigned::twelve));
  ASSERT_EQ_STATIC(9, enum_index_by<by_name>(bar_unsigned::two));
  ASSERT_EQ_STATIC(10, enum_index_by<by_name>(bar_unsigned::zero));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<bar_unsigned>(-3)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<bar_unsigned>(9)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<bar_unsigned>(1234)));
}

TEST(EnumIndex, BazSigned) {
  ASSERT_EQ_STATIC(0, enum_index(baz_signed::ling));
  ASSERT_EQ_STATIC(1, enum_index(baz_signed::yi));
  ASSERT_EQ_STATIC(2, enum_index(baz_signed::er));
  ASSERT_EQ_STATIC(3, enum_index(baz_signed::san));
  ASSERT_EQ_STATIC(4, enum_index(baz_signed::fuyi));

  ASSERT_EQ_STATIC(1, enum_index_by<by_value>(baz_signed::ling));
  ASSERT_EQ_STATIC(2, enum_index_by<by_value>(baz_signed::yi));
  ASSERT_EQ_STATIC(3, enum_index_by<by_value>(baz_signed::er));
  ASSERT_EQ_STATIC(4, enum_index_by<by_value>(baz_signed::san));
  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(baz_signed::fuyi));

  ASSERT_EQ_STATIC(0, enum_index_by<by_name>(baz_signed::er));
  ASSERT_EQ_STATIC(1, enum_index_by<by_name>(baz_signed::fuyi));
  ASSERT_EQ_STATIC(2, enum_index_by<by_name>(baz_signed::ling));
  ASSERT_EQ_STATIC(3, enum_index_by<by_name>(baz_signed::san));
  ASSERT_EQ_STATIC(4, enum_index_by<by_name>(baz_signed::yi));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<baz_signed>(-2)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<baz_signed>(4)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<baz_signed>(1 << 15)));
}

TEST(EnumIndex, QuxUnsigned) {
  ASSERT_EQ_STATIC(0, enum_index(qux_unsigned::ling));
  ASSERT_EQ_STATIC(1, enum_index(qux_unsigned::yi));
  ASSERT_EQ_STATIC(2, enum_index(qux_unsigned::er));
  ASSERT_EQ_STATIC(3, enum_index(qux_unsigned::san));
  ASSERT_EQ_STATIC(4, enum_index(qux_unsigned::si));
  ASSERT_EQ_STATIC(5, enum_index(qux_unsigned::fuyi));

  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(qux_unsigned::ling));
  ASSERT_EQ_STATIC(1, enum_index_by<by_value>(qux_unsigned::yi));
  ASSERT_EQ_STATIC(2, enum_index_by<by_value>(qux_unsigned::er));
  ASSERT_EQ_STATIC(3, enum_index_by<by_value>(qux_unsigned::san));
  ASSERT_EQ_STATIC(4, enum_index_by<by_value>(qux_unsigned::si));
  ASSERT_EQ_STATIC(5, enum_index_by<by_value>(qux_unsigned::fuyi));

  ASSERT_EQ_STATIC(0, enum_index_by<by_name>(qux_unsigned::er));
  ASSERT_EQ_STATIC(1, enum_index_by<by_name>(qux_unsigned::fuyi));
  ASSERT_EQ_STATIC(2, enum_index_by<by_name>(qux_unsigned::ling));
  ASSERT_EQ_STATIC(3, enum_index_by<by_name>(qux_unsigned::san));
  ASSERT_EQ_STATIC(4, enum_index_by<by_name>(qux_unsigned::si));
  ASSERT_EQ_STATIC(5, enum_index_by<by_name>(qux_unsigned::yi));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<qux_unsigned>(-2)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<qux_unsigned>(5)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<qux_unsigned>(1 << 15)));
}

// Compilation error during static assertion since Clang 21:
// integer value -1 is outside the valid range of values [0, 1] for the
// enumeration type 'empty'
TEST(EnumIndex, Empty) {
  ASSERT_EQ_STATIC(npos, enum_index(static_cast<empty>(0)));
  ASSERT_EQ_STATIC(npos, enum_index(static_cast<empty>(1)));
  ASSERT_EQ(npos, enum_index(static_cast<empty>(-1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<empty>(0)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<empty>(1)));
  ASSERT_EQ(npos, enum_index_by<by_value>(static_cast<empty>(2)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<empty>(0)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<empty>(1)));
  ASSERT_EQ(npos, enum_index_by<by_name>(static_cast<empty>(2)));
}

TEST(EnumIndex, Single) {
  ASSERT_EQ_STATIC(0, enum_index(static_cast<single>(233)));
  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(static_cast<single>(233)));
  ASSERT_EQ_STATIC(0, enum_index_by<by_name>(static_cast<single>(233)));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<single>(0)));
  ASSERT_EQ_STATIC(npos, enum_index(static_cast<single>(1)));
  ASSERT_EQ(npos, enum_index(static_cast<single>(-1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<single>(0)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<single>(1)));
  ASSERT_EQ(npos, enum_index_by<by_value>(static_cast<single>(-2)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<single>(0)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<single>(1)));
  ASSERT_EQ(npos, enum_index_by<by_name>(static_cast<single>(-3)));
}

TEST(EnumIndex, SingleRep) {
  ASSERT_THAT(enum_index(static_cast<single_rep>(233)),
              testing::AllOf(testing::Ge(0), testing::Le(5)));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<single_rep>(0)));
  ASSERT_EQ_STATIC(npos, enum_index(static_cast<single_rep>(1)));
  ASSERT_EQ_STATIC(npos, enum_index(static_cast<single_rep>(-1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<single_rep>(0)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<single_rep>(1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<single_rep>(-1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<single_rep>(0)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<single_rep>(1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<single_rep>(-1)));
}

TEST(EnumIndex, Color) {
  ASSERT_EQ_STATIC(100, enum_index(color::orange_red));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<color>(0x12345678)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<color>(0x98765432)));
}

TEST(EnumIndex, TerminalColor) {
  ASSERT_EQ_STATIC(0, enum_index(terminal_color::black));
  ASSERT_EQ_STATIC(1, enum_index(terminal_color::red));
  ASSERT_EQ_STATIC(2, enum_index(terminal_color::green));
  ASSERT_EQ_STATIC(3, enum_index(terminal_color::yellow));
  ASSERT_EQ_STATIC(4, enum_index(terminal_color::blue));
  ASSERT_EQ_STATIC(5, enum_index(terminal_color::magenta));
  ASSERT_EQ_STATIC(6, enum_index(terminal_color::cyan));
  ASSERT_EQ_STATIC(7, enum_index(terminal_color::white));
  ASSERT_EQ_STATIC(8, enum_index(terminal_color::bright_black));
  ASSERT_EQ_STATIC(9, enum_index(terminal_color::bright_red));
  ASSERT_EQ_STATIC(10, enum_index(terminal_color::bright_green));
  ASSERT_EQ_STATIC(11, enum_index(terminal_color::bright_yellow));
  ASSERT_EQ_STATIC(12, enum_index(terminal_color::bright_blue));
  ASSERT_EQ_STATIC(13, enum_index(terminal_color::bright_magenta));
  ASSERT_EQ_STATIC(14, enum_index(terminal_color::bright_cyan));
  ASSERT_EQ_STATIC(15, enum_index(terminal_color::bright_white));

  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(terminal_color::black));
  ASSERT_EQ_STATIC(1, enum_index_by<by_value>(terminal_color::red));
  ASSERT_EQ_STATIC(2, enum_index_by<by_value>(terminal_color::green));
  ASSERT_EQ_STATIC(3, enum_index_by<by_value>(terminal_color::yellow));
  ASSERT_EQ_STATIC(4, enum_index_by<by_value>(terminal_color::blue));
  ASSERT_EQ_STATIC(5, enum_index_by<by_value>(terminal_color::magenta));
  ASSERT_EQ_STATIC(6, enum_index_by<by_value>(terminal_color::cyan));
  ASSERT_EQ_STATIC(7, enum_index_by<by_value>(terminal_color::white));
  ASSERT_EQ_STATIC(8, enum_index_by<by_value>(terminal_color::bright_black));
  ASSERT_EQ_STATIC(9, enum_index_by<by_value>(terminal_color::bright_red));
  ASSERT_EQ_STATIC(10, enum_index_by<by_value>(terminal_color::bright_green));
  ASSERT_EQ_STATIC(11, enum_index_by<by_value>(terminal_color::bright_yellow));
  ASSERT_EQ_STATIC(12, enum_index_by<by_value>(terminal_color::bright_blue));
  ASSERT_EQ_STATIC(13, enum_index_by<by_value>(terminal_color::bright_magenta));
  ASSERT_EQ_STATIC(14, enum_index_by<by_value>(terminal_color::bright_cyan));
  ASSERT_EQ_STATIC(15, enum_index_by<by_value>(terminal_color::bright_white));

  ASSERT_EQ_STATIC(0, enum_index_by<by_name>(terminal_color::black));
  ASSERT_EQ_STATIC(1, enum_index_by<by_name>(terminal_color::blue));
  ASSERT_EQ_STATIC(2, enum_index_by<by_name>(terminal_color::bright_black));
  ASSERT_EQ_STATIC(3, enum_index_by<by_name>(terminal_color::bright_blue));
  ASSERT_EQ_STATIC(4, enum_index_by<by_name>(terminal_color::bright_cyan));
  ASSERT_EQ_STATIC(5, enum_index_by<by_name>(terminal_color::bright_green));
  ASSERT_EQ_STATIC(6, enum_index_by<by_name>(terminal_color::bright_magenta));
  ASSERT_EQ_STATIC(7, enum_index_by<by_name>(terminal_color::bright_red));
  ASSERT_EQ_STATIC(8, enum_index_by<by_name>(terminal_color::bright_white));
  ASSERT_EQ_STATIC(9, enum_index_by<by_name>(terminal_color::bright_yellow));
  ASSERT_EQ_STATIC(10, enum_index_by<by_name>(terminal_color::cyan));
  ASSERT_EQ_STATIC(11, enum_index_by<by_name>(terminal_color::green));
  ASSERT_EQ_STATIC(12, enum_index_by<by_name>(terminal_color::magenta));
  ASSERT_EQ_STATIC(13, enum_index_by<by_name>(terminal_color::red));
  ASSERT_EQ_STATIC(14, enum_index_by<by_name>(terminal_color::white));
  ASSERT_EQ_STATIC(15, enum_index_by<by_name>(terminal_color::yellow));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<terminal_color>(0)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<terminal_color>(1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<terminal_color>(40)));
}

TEST(EnumIndex, HashCollision) {
  ASSERT_EQ_STATIC(0, enum_index(hash_collision::_wSYZDRpiQJf8Rfv));
  ASSERT_EQ_STATIC(1, enum_index(hash_collision::_cuFFJIHGp_jNJKS));

  ASSERT_EQ_STATIC(0, enum_index_by<by_value>(hash_collision::_wSYZDRpiQJf8Rfv));
  ASSERT_EQ_STATIC(1, enum_index_by<by_value>(hash_collision::_cuFFJIHGp_jNJKS));

  ASSERT_EQ_STATIC(0, enum_index_by<by_name>(hash_collision::_cuFFJIHGp_jNJKS));
  ASSERT_EQ_STATIC(1, enum_index_by<by_name>(hash_collision::_wSYZDRpiQJf8Rfv));

  ASSERT_EQ_STATIC(npos, enum_index(static_cast<hash_collision>(-1)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_value>(static_cast<hash_collision>(2)));
  ASSERT_EQ_STATIC(npos, enum_index_by<by_name>(static_cast<hash_collision>(123)));
}
