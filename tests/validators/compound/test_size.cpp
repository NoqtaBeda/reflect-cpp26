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
#include <reflect_cpp26/validators/compound/size_test.hpp>
#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#include <reflect_cpp26/validators/leaf/options_exclusion_test.hpp>
#endif

struct foo_size_t {
  VALIDATOR(size >> equals_to, 4)
  std::vector<int> v1;

  VALIDATOR(size >> options, {1, 2, 4, 8})
  VALIDATOR(size >> not_equal_to, 4)
  std::vector<int> v2;
};

TEST(Validators, CompoundSize)
{
  LAZY_OBJECT(obj_ok_1, foo_size_t{.v1 = {1, 2, 3, 4}, .v2 = {1, 2}});
  EXPECT_TRUE_STATIC(do_validate_members(obj_ok_1));
  EXPECT_EQ_STATIC("", validation_error_message(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_size_t{.v1 = {1, 2, 3}, .v2 = {1, 2, 3, 4}});
  EXPECT_FALSE_STATIC(do_validate_members(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Invalid size -> Expects value == 4, "
    "while actual value = 3",
    validation_error_message(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Invalid size -> Expects value == 4, while actual value = 3"
    "\nInvalid member 'v2':"
    "\n* Invalid size -> Expects value != 4, while actual value = 4",
    validation_full_error_message(obj_1));

  LAZY_OBJECT(obj_2, foo_size_t{.v1 = {1, 2, 3, 4}, .v2 = {1, 2, 3, 4, 5}});
  EXPECT_FALSE_STATIC(do_validate_members(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v2': Invalid size -> "
    "Expects value to be any of [1, 2, 4, 8], while actual value = 5",
    validation_error_message(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v2':"
    "\n* Invalid size -> Expects value to be any of [1, 2, 4, 8], "
    "while actual value = 5",
    validation_full_error_message(obj_2));
}

