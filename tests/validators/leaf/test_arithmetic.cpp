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
#endif

struct foo_floating_t {
  VALIDATOR(is_positive)
  VALIDATOR(is_non_negative)
  VALIDATOR(is_finite)
  VALIDATOR(is_finite_positive)
  VALIDATOR(is_finite_non_negative)
  float f32;

  VALIDATOR(is_negative)
  VALIDATOR(is_non_positive)
  VALIDATOR(is_finite_negative)
  VALIDATOR(is_finite_non_positive)
  double f64;
};

#if __cpp_lib_constexpr_cmath >= 202202L
#define EXPECT_TRUE_STATIC_IF_CONSTEXPR_MATH(expr) EXPECT_TRUE_STATIC(expr)
#define EXPECT_FALSE_STATIC_IF_CONSTEXPR_MATH(expr) EXPECT_FALSE_STATIC(expr)
#else
#define EXPECT_TRUE_STATIC_IF_CONSTEXPR_MATH(expr) EXPECT_TRUE(expr)
#define EXPECT_FALSE_STATIC_IF_CONSTEXPR_MATH(expr) EXPECT_FALSE(expr)
#endif

TEST(Validators, LeafArithmetic)
{
  constexpr auto obj_ok_1 = foo_floating_t{.f32 = 1.5f, .f64 = -3.5};
  EXPECT_TRUE_STATIC_IF_CONSTEXPR_MATH(rfl::validate_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(rfl::validate_members_with_error_info(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  constexpr auto obj_2 = foo_floating_t{.f32 = -1.5f, .f64 = 1.5};
  EXPECT_FALSE_STATIC_IF_CONSTEXPR_MATH(rfl::validate_members(obj_2));
  msg.clear();
  EXPECT_FALSE(rfl::validate_members_with_error_info(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'f32': Expects value to meet the condition 'is_positive', "
    "but actual value = -1.5", msg);

  msg.clear();
  EXPECT_FALSE(rfl::validate_members_with_full_error_info(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'f32':"
    "\n* Expects value to meet the condition 'is_positive', "
    "but actual value = -1.5"
    "\n* Expects value to meet the condition 'is_non_negative', "
    "but actual value = -1.5"
    "\n* Expects value to meet the condition 'is_finite_positive', "
    "but actual value = -1.5"
    "\n* Expects value to meet the condition 'is_finite_non_negative', "
    "but actual value = -1.5"
    "\nInvalid member 'f64':"
    "\n* Expects value to meet the condition 'is_negative', "
    "but actual value = 1.5"
    "\n* Expects value to meet the condition 'is_non_positive', "
    "but actual value = 1.5"
    "\n* Expects value to meet the condition 'is_finite_negative', "
    "but actual value = 1.5"
    "\n* Expects value to meet the condition 'is_finite_non_positive', "
    "but actual value = 1.5", msg);

  constexpr auto obj_3 = foo_floating_t{.f32 = 0.0f, .f64 = 0.0};
  EXPECT_FALSE_STATIC_IF_CONSTEXPR_MATH(rfl::validate_members(obj_3));
  msg.clear();
  EXPECT_FALSE(rfl::validate_members_with_full_error_info(obj_3, &msg));
  EXPECT_EQ(
    "Invalid member 'f32':"
    "\n* Expects value to meet the condition 'is_positive', "
    "but actual value = 0"
    "\n* Expects value to meet the condition 'is_finite_positive', "
    "but actual value = 0"
    "\nInvalid member 'f64':"
    "\n* Expects value to meet the condition 'is_negative', "
    "but actual value = 0"
    "\n* Expects value to meet the condition 'is_finite_negative', "
    "but actual value = 0", msg);

  // todo: "nan" or "nan(char_sequence)" which is implementation defined.
  constexpr auto obj_4 = foo_floating_t{.f32 = NAN, .f64 = NAN};
  EXPECT_FALSE_STATIC_IF_CONSTEXPR_MATH(rfl::validate_members(obj_4));
  msg.clear();
  EXPECT_FALSE(rfl::validate_members_with_full_error_info(obj_4, &msg));
  EXPECT_EQ(
    "Invalid member 'f32':"
    "\n* Expects value to meet the condition 'is_positive', "
    "but actual value = nan"
    "\n* Expects value to meet the condition 'is_non_negative', "
    "but actual value = nan"
    "\n* Expects value to meet the condition 'is_finite', "
    "but actual value = nan"
    "\n* Expects value to meet the condition 'is_finite_positive', "
    "but actual value = nan"
    "\n* Expects value to meet the condition 'is_finite_non_negative', "
    "but actual value = nan"
    "\nInvalid member 'f64':"
    "\n* Expects value to meet the condition 'is_negative', "
    "but actual value = nan"
    "\n* Expects value to meet the condition 'is_non_positive', "
    "but actual value = nan"
    "\n* Expects value to meet the condition 'is_finite_negative', "
    "but actual value = nan"
    "\n* Expects value to meet the condition 'is_finite_non_positive', "
    "but actual value = nan", msg);

  // todo: "inf" or "infinity" which is implementation defined.
  constexpr auto obj_5 = foo_floating_t{.f32 = INFINITY, .f64 = -INFINITY};
  EXPECT_FALSE_STATIC_IF_CONSTEXPR_MATH(rfl::validate_members(obj_5));
  msg.clear();
  EXPECT_FALSE(rfl::validate_members_with_full_error_info(obj_5, &msg));
  EXPECT_EQ(
    "Invalid member 'f32':"
    "\n* Expects value to meet the condition 'is_finite', "
    "but actual value = inf"
    "\n* Expects value to meet the condition 'is_finite_positive', "
    "but actual value = inf"
    "\n* Expects value to meet the condition 'is_finite_non_negative', "
    "but actual value = inf"
    "\nInvalid member 'f64':"
    "\n* Expects value to meet the condition 'is_finite_negative', "
    "but actual value = -inf"
    "\n* Expects value to meet the condition 'is_finite_non_positive', "
    "but actual value = -inf"
    , msg);
}
