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
#include <reflect_cpp26/validators/leaf/size_test.hpp>
#endif

struct foo_non_empty_1_t {
  VALIDATOR(is_not_empty)
  VALIDATOR(size_is, 3)
  std::vector<int> v1;
};

TEST(Validators, LeafNonEmpty1)
{
  LAZY_OBJECT(obj_ok_1, foo_non_empty_1_t{.v1 = {1, 2, 3}});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_non_empty_1_t{});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects input range to be non-empty",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects input range to be non-empty"
    "\n* Expects size to be 3, while actual size is 0",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_non_empty_1_t{.v1 = {1, 2, 3, 4}});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects size to be 3, while actual size is 4",
    validate_public_nsdm_msg(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects size to be 3, while actual size is 4",
    validate_public_nsdm_msg_verbose(obj_2));
}

struct foo_non_empty_2_t {
  VALIDATOR(size_is, next_ith_public_nsdm<2>) // foo_non_empty_2_t::d
  std::vector<int> v1;

  constexpr foo_non_empty_2_t(
    std::initializer_list<int> v, std::array<size_t, 6> a)
    : v1(v), a(a[0]), b(a[1]), c(a[2]), d(a[3]), e(a[4]), f(a[5]) {}

private:
  size_t a;
public:
  size_t b;
protected:
  size_t c;
public:
  size_t d;
private:
  size_t e;
public:
  size_t f;
};

TEST(Validators, LeafNonEmpty2)
{
  LAZY_OBJECT(obj_ok_1, foo_non_empty_2_t{{1, 2, 3, 4}, {1, 2, 3, 4, 5, 6}});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_non_empty_2_t{{1, 2}, {1, 2, 3, 4, 5, 6}});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects size to be member 'd' which is 4, "
    "while actual size is 2",
    validate_public_nsdm_msg(obj_1));

  LAZY_OBJECT(obj_2, foo_non_empty_2_t{{}, {1, 2, 3, 4, 5, 6}});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects size to be member 'd' which is 4, "
    "while actual size is 0",
    validate_public_nsdm_msg(obj_2));
}
