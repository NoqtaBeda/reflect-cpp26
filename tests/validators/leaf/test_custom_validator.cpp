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
#include <list>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/validators.hpp>
#else
#include <reflect_cpp26/validators/leaf/custom_validator.hpp>
#endif

using double_vector_4 = std::array<double, 4>;

struct foo_custom_validator_t {
  VALIDATOR(custom_validator, [](int count) {
    return count > 0 && count % 2 == 0;
  })
  int count;

  VALIDATOR(custom_validator, [](std::string_view name) {
    return name.length() >= 4;
  })
  std::string name;

  VALIDATOR(custom_validator, [](const auto& vectors) {
    if (vectors.empty()) {
      return false;
    }
    for (const auto& vec: vectors) {
      if (*std::ranges::min_element(vec) <= 0) {
        return false;
      }
    }
    return true;
  })
  std::list<double_vector_4> vectors;
};

TEST(Validators, LeafCustomValidator)
{
  LAZY_OBJECT(obj_ok_1, foo_custom_validator_t{
    .count = 42,
    .name = "Hello",
    .vectors = {{1, 2, 3, 4}, {5, 6, 7, 8}}
  });
  EXPECT_TRUE(do_validate_members(obj_ok_1));
  EXPECT_EQ("", validation_error_message(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_custom_validator_t{
    .count = -1,
    .name = "Ben",
    .vectors = {}
  });
  EXPECT_FALSE(do_validate_members(obj_1));
  EXPECT_EQ("Invalid member 'count': Custom validator fails with value -1",
    validation_error_message(obj_1));
  EXPECT_EQ(
    "Invalid member 'count':"
    "\n* Custom validator fails with value -1"
    "\nInvalid member 'name':"
    "\n* Custom validator fails with value \"Ben\""
    "\nInvalid member 'vectors':"
    "\n* Custom validator fails with value []",
    validation_full_error_message(obj_1));

  LAZY_OBJECT(obj_2, foo_custom_validator_t{
    .count = 2,
    .name = "Hello",
    .vectors = {{-1, 2, 3, 4}, {5, 6, 7, 8}}
  });
  EXPECT_FALSE(do_validate_members(obj_2));
  EXPECT_EQ(
    "Invalid member 'vectors': Custom validator fails with value "
    "[[-1, 2, 3, 4], [5, 6, 7, 8]]", validation_error_message(obj_2));
  EXPECT_EQ(
    "Invalid member 'vectors':\n* Custom validator fails with value "
    "[[-1, 2, 3, 4], [5, 6, 7, 8]]", validation_full_error_message(obj_2));
}
