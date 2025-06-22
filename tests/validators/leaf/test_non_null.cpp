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
#include <reflect_cpp26/validators/leaf/non_null_test.hpp>
#endif

struct foo_non_null_t {
  VALIDATOR(is_non_null)
  const char* str;

  VALIDATOR(is_non_null)
  volatile char* buffer;

  VALIDATOR(is_non_null)
  std::shared_ptr<int[]> sptr;

  VALIDATOR(is_non_null)
  std::unique_ptr<int[]> uptr;

  VALIDATOR(is_non_null)
  std::weak_ptr<int[]> wptr;

  VALIDATOR(is_non_null)
  std::any any;

  VALIDATOR(is_non_null)
  std::optional<int> opt;
};

TEST(Validators, LeafNonNull)
{
  char buffer[128];
  auto obj_ok_1 = foo_non_null_t{
    .str = "Hello",
    .buffer = buffer,
    .sptr = std::make_shared<int[]>(32),
    .uptr = std::make_unique<int[]>(64),
    .any = 42,
    .opt = 84
  };
  obj_ok_1.wptr = obj_ok_1.sptr;
  EXPECT_TRUE(rfl::validate_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(rfl::validate_members_with_error_info(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  auto obj_1 = foo_non_null_t{};
  EXPECT_FALSE(rfl::validate_members(obj_1));
  msg.clear();
  EXPECT_FALSE(rfl::validate_members_with_error_info(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'str': Expects pointer to be non-null.", msg);

  msg.clear();
  EXPECT_FALSE(rfl::validate_members_with_full_error_info(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'str':"
    "\n* Expects pointer to be non-null."
    "\nInvalid member 'buffer':"
    "\n* Expects pointer to be non-null."
    "\nInvalid member 'sptr':"
    "\n* Expects shared pointer to be non-null."
    "\nInvalid member 'uptr':"
    "\n* Expects unique pointer to be non-null."
    "\nInvalid member 'wptr':"
    "\n* Expects weak pointer to be non-null."
    "\nInvalid member 'any':"
    "\n* Expects std::any to be non-null."
    "\nInvalid member 'opt':"
    "\n* Expects std::optional to be non-null.", msg);
}
