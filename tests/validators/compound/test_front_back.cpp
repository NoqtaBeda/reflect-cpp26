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
#include <reflect_cpp26/validators/compound/front_back_test.hpp>
#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#include <reflect_cpp26/validators/leaf/options_exclusion_test.hpp>
#endif

struct foo_front_back_t {
  VALIDATOR(front >> max_exclusive, 0)
  VALIDATOR(back >> min_exclusive, 0)
  std::vector<int> v1;

  VALIDATOR(front >> excludes, {
    std::array{1, 2, 3},
    std::array{4, 5, 6}
  })
  VALIDATOR(back >> options, {
    std::array{1, 2, 3},
    std::array{4, 5, 6}
  })
  int v2[2][3];

  VALIDATOR(front >> front >> not_equal_to, -1)
  VALIDATOR(front >> back >> not_equal_to, -2)
  VALIDATOR(back >> front >> not_equal_to, -3)
  VALIDATOR(back >> back >> not_equal_to, -4)
  int v3[2][2];
};

TEST(Validators, CompoundFrontBack)
{
  LAZY_OBJECT(obj_ok_1, foo_front_back_t{
    .v1 = {-1, 0, 1},
    .v2 = {{-1, -2, -3}, {4, 5, 6}},
    .v3 = {{1, 2}, {4, 5}}
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_front_back_t{
    .v1 = {1, 0, -1},
    .v2 = {{4, 5, 6}, {1, 2, 3}},
    .v3 = {{-1, -2}, {-3, -4}}
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Invalid first value -> Expects value < 0, "
    "while actual value = 1",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Invalid first value -> Expects value < 0, while actual value = 1"
    "\n* Invalid last value -> Expects value > 0, while actual value = -1"
    "\nInvalid member 'v2':"
    "\n* Invalid first value -> Expects value to be none of "
    "[[1, 2, 3], [4, 5, 6]], while actual value = true"
    "\nInvalid member 'v3':"
    "\n* Invalid first value -> Invalid first value -> Expects value != -1, "
    "while actual value = -1"
    "\n* Invalid first value -> Invalid last value -> Expects value != -2, "
    "while actual value = -2"
    "\n* Invalid last value -> Invalid first value -> Expects value != -3, "
    "while actual value = -3"
    "\n* Invalid last value -> Invalid last value -> Expects value != -4, "
    "while actual value = -4",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_front_back_t{
    .v1 = {}, // empty
    .v2 = {{-1, -2, -3}, {4, 5, 6}}, // OK
    .v3 = {{1, 2}, {4, 5}} // OK
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Can not validate first value "
    "since input range is empty.",
    validate_public_nsdm_msg(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Can not validate first value since input range is empty."
    "\n* Can not validate last value since input range is empty.",
    validate_public_nsdm_msg_verbose(obj_2));
}
