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

#include "tests/validators/validator_test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/validators.hpp>
#else
#include <reflect_cpp26/validators/leaf/arithmetic_test.hpp>
#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#include <reflect_cpp26/validators/leaf/size_test.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_t {
  VALIDATOR(is_positive)
  int x;

  VALIDATOR(is_not_empty)
  std::vector<int> y;

  constexpr foo_t(int x, std::initializer_list<int> y, int z)
    : x(x), y(y), z(z) {}

private:
  VALIDATOR(is_negative)
  int z;
};

TEST(Validators, PublicAccess1)
{
  LAZY_OBJECT(obj_ok_1, foo_t{1, {2, 3, 4}, -5});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, foo_t{1, {2, 3, 4}, 5});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_1, foo_t{0, {}, 5});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'x':"
    "\n* Expects value to meet the condition 'is_positive', "
    "but actual value = 0"
    "\nInvalid member 'y':"
    "\n* Expects input range to be non-empty",
    validate_public_nsdm_msg_verbose(obj_1));
}

TEST(Validators, AllAccess1)
{
  LAZY_OBJECT(obj_ok_1, foo_t{1, {2, 3, 4}, -5});
  EXPECT_TRUE_STATIC(validate_all_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_all_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_t{1, {2, 3, 4}, 5});
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'z': Expects value to meet the condition 'is_negative', "
    "but actual value = 5",
    validate_all_nsdm_msg(obj_1));

  LAZY_OBJECT(obj_2, foo_t{0, {}, 5});
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'x':"
    "\n* Expects value to meet the condition 'is_positive', "
    "but actual value = 0"
    "\nInvalid member 'y':"
    "\n* Expects input range to be non-empty"
    "\nInvalid member 'z':"
    "\n* Expects value to meet the condition 'is_negative', "
    "but actual value = 5", validate_all_nsdm_msg_verbose(obj_2));
}

struct bar_t : public foo_t {
  VALIDATOR(in_closed_range, 0, 42)
  int a;

  constexpr bar_t(int x, std::initializer_list<int> y, int z, int a, int b)
    : foo_t(x, y, z), a(a), b(b) {}

private:
  VALIDATOR(in_half_closed_range, 0, 10)
  int b;
};

TEST(Validators, PublicAccess2)
{
  LAZY_OBJECT(obj_ok_1, bar_t{1, {2, 3, 4}, -5, 6, 7});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, bar_t{1, {2, 3, 4}, 5, 6, -7});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_1, bar_t{-1, {}, 2, -3, -4});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'x':"
    "\n* Expects value to meet the condition 'is_positive', "
    "but actual value = -1"
    "\nInvalid member 'y':"
    "\n* Expects input range to be non-empty"
    "\nInvalid member 'a':"
    "\n* Expects value to fall in [0, 42], while actual value = -3",
    validate_public_nsdm_msg_verbose(obj_1));
}

template <class BarOrBaz>
void test_all_access_bar_baz_common()
{
  LAZY_OBJECT(obj_ok_1, BarOrBaz{1, {2, 3, 4}, -5, 6, 7});
  EXPECT_TRUE_STATIC(validate_all_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_all_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, BarOrBaz{1, {2, 3, 4}, 5, 6, -7});
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'z': Expects value to meet the condition 'is_negative', "
    "but actual value = 5",
    validate_all_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'z':"
    "\n* Expects value to meet the condition 'is_negative', "
    "but actual value = 5"
    "\nInvalid member 'b':\n* Expects value to fall in [0, 10), "
    "while actual value = -7", validate_all_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, BarOrBaz{-1, {}, 2, -3, -4});
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'x':"
    "\n* Expects value to meet the condition 'is_positive', "
    "but actual value = -1"
    "\nInvalid member 'y':"
    "\n* Expects input range to be non-empty"
    "\nInvalid member 'z':"
    "\n* Expects value to meet the condition 'is_negative', "
    "but actual value = 2"
    "\nInvalid member 'a':"
    "\n* Expects value to fall in [0, 42], while actual value = -3"
    "\nInvalid member 'b':"
    "\n* Expects value to fall in [0, 10), while actual value = -4",
    validate_all_nsdm_msg_verbose(obj_2));
}

TEST(Validators, AllAccess2) {
  test_all_access_bar_baz_common<bar_t>();
}

struct baz_t : protected foo_t {
  VALIDATOR(in_closed_range, 0, 42)
  int a;

  constexpr baz_t(int x, std::initializer_list<int> y, int z, int a, int b)
    : foo_t(x, y, z), a(a), b(b) {}

private:
  VALIDATOR(in_half_closed_range, 0, 10)
  int b;
};

TEST(Validators, PublicAccess3)
{
  LAZY_OBJECT(obj_ok_1, baz_t{1, {2, 3, 4}, -5, 6, 7});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, baz_t{1, {2, 3, 4}, 5, 6, -7});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_ok_3, baz_t{-1, {}, 2, 3, -4});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_3));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_3));

  LAZY_OBJECT(obj_1, baz_t{-1, {}, 2, -3, -4});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'a':"
    "\n* Expects value to fall in [0, 42], while actual value = -3",
    validate_public_nsdm_msg_verbose(obj_1));
}

TEST(Validators, AllAccess3) {
  test_all_access_bar_baz_common<baz_t>();
}
