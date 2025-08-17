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
#include <reflect_cpp26/validators/leaf/enum_test.hpp>
#endif

enum class values : int {
  minus_two = -2,
  minus_one = -1,
  zero = 0,
  one = 1,
  two = 2,
  three = 3,
  four = 4,
};

enum class flags : int {
  zero = 0,
  one = 1,
  two = 2,
  four = 4,
  eight = 8,
};

struct foo_t {
  VALIDATOR(is_valid_enum)
  values v;

  VALIDATOR(is_valid_enum_flags)
  flags f;
};

TEST(Validators, LeafEnumIsValid)
{
  LAZY_OBJECT(obj_ok_1, foo_t{.v = values::one, .f = static_cast<flags>(13)});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_t{
    .v = static_cast<values>(6),
    .f = static_cast<flags>(16),
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_THAT(validate_public_nsdm_msg_verbose(obj_1), testing::MatchesRegex(
    "Invalid member 'v':"
    "\n\\* Input enum value \\(.*values\\)6 "
    "is not an entry defined in enum type"
    "\nInvalid member 'f':"
    "\n\\* Input enum value \\(.*flags\\)16 "
    "is not disjunction of entries defined in enum type"));
}
