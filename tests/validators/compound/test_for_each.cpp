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
#include <reflect_cpp26/validators/compound/for_each_test.hpp>
#include <reflect_cpp26/validators/leaf/arithmetic_test.hpp>
#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#endif

struct foo_for_each_t {
  VALIDATOR(all_of >> is_non_negative)
  VALIDATOR(any_of >> equal_to, 1)
  VALIDATOR(none_of >> equal_to, 2)
  std::array<double, 4> v1;
};

TEST(Validators, CompoundForEachOf)
{
  LAZY_OBJECT(obj_ok_1, foo_for_each_t{.v1 = {1, 3, 5, INFINITY}});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_for_each_t{.v1 = {-1, 2, -4, 8}});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_THAT(validate_public_nsdm_msg(obj_1), testing::StartsWith(
    "Invalid member 'v1': Expects all of values meets given condition, "
    "but 2 value(s) dissatisfy actually"));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects all of values meets given condition, "
    "but 2 value(s) dissatisfy actually."
    "\n* Expects any of values meets given condition, "
    "but 4 value(s) dissatisfy actually."
    "\n* Expects none of values meets given condition, "
    "but 1 value(s) satisfy actually.",
    validate_public_nsdm_msg_verbose(obj_1));
}
