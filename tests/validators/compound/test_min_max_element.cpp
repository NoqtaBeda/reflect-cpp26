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
#include <reflect_cpp26/validators/compound/min_max_element_test.hpp>
#include <reflect_cpp26/validators/compound/size_test.hpp>
#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#include <reflect_cpp26/validators/leaf/options_exclusion_test.hpp>
#endif

struct foo_min_max_t {
  VALIDATOR(min_element >> max, 0)
  VALIDATOR(max_element >> min_exclusive, 0)
  std::vector<int> v1;

  VALIDATOR(min_element >> size >> equal_to, 1)
  VALIDATOR(max_element >> max_element >> min_exclusive, 0)
  std::vector<std::vector<int>> v2;
};

TEST(Validators, CompoundMinMaxElement)
{
  LAZY_OBJECT(obj_ok_1, foo_min_max_t{
    .v1 = {-1, 0, 1},
    .v2 = {{0}, {1, 2}, {3, 4, 5}}
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_min_max_t{
    .v1 = {1, 2, 3},
    .v2 = {{-4, -3}, {-2, -1}}
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Invalid minimum value -> Expects value <= 0, "
    "while actual value = 1",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Invalid minimum value -> Expects value <= 0, while actual value = 1"
    "\nInvalid member 'v2':"
    "\n* Invalid minimum value -> Invalid size -> Expects value == 1, "
    "while actual value = 2"
    "\n* Invalid maximum value -> Invalid maximum value -> Expects value > 0, "
    "while actual value = -1",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_min_max_t{.v1 = {}, .v2 = {{}, {}}});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Can not validate the minimum value since "
    "input range is empty.",
    validate_public_nsdm_msg(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Can not validate the minimum value since input range is empty."
    "\n* Can not validate the maximum value since input range is empty."
    "\nInvalid member 'v2':"
    "\n* Invalid minimum value -> Invalid size -> Expects value == 1, "
    "while actual value = 0"
    "\n* Invalid maximum value -> Can not validate the maximum value since "
    "input range is empty.",
    validate_public_nsdm_msg_verbose(obj_2));
}
