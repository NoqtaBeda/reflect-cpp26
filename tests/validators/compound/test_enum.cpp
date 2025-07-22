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
#include <reflect_cpp26/validators/compound/enum_test.hpp>
#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#endif

enum class values : int {
  minus_two = -2,
  minus_one = -1,
  zero = 0,
  one = 1,
  two = 2,
  three = 3,
};

struct foo_t {
  VALIDATOR(underlying >> greater_equal, 0)
  values v1;

  VALIDATOR(underlying >> in_closed_range, -1, 1)
  values v2;
};

TEST(Validators, CompoundEnumUnderlying)
{
  LAZY_OBJECT(obj_ok_1, foo_t{.v1 = values::zero, .v2 = values::one});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_t{.v1 = values::minus_one, .v2 = values::minus_two});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Invalid underlying value -> Expects value >= 0, "
    "while actual value = -1"
    "\nInvalid member 'v2':"
    "\n* Invalid underlying value -> Expects value to fall in [-1, 1], "
    "while actual value = -2",
    validate_public_nsdm_msg_verbose(obj_1));
}
