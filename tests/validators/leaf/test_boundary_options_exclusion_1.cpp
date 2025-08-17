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
#include <deque>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/validators.hpp>
#else
#include <reflect_cpp26/validators/leaf/boundary_test.hpp>
#include <reflect_cpp26/validators/leaf/options_exclusion_test.hpp>
#endif

/**
 * The following validators are tested:
 * - Bound checking: min, max, min_exclusive, max_exclusive
 * - Options checking: options, excludes
 */

struct foo_integral_1_t {
  VALIDATOR(min, -10)
  VALIDATOR(max, 10)
  // Tests safe integer comparison:
  // 4294967295 shall not be equal to int32_t{-1}
  VALIDATOR(excludes, std::vector<unsigned>{1, 2, 4, 8, 16, 4294967295u})
  int32_t s;

  VALIDATOR(min_exclusive, 10)
  VALIDATOR(max_exclusive, 100)
  // Tests safe integer comparison:
  // -1 shall not be equal to uint32_t{4294967295}
  VALIDATOR(options, {-1, 1, 10, 11, 99}) // std::initializer_list
  uint32_t u;
};

TEST(Validators, LeafBoundaryOptionsExclusionIntegral1)
{
  LAZY_OBJECT(obj_ok_1, foo_integral_1_t{.s = -10, .u = 11});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, foo_integral_1_t{.s = 10, .u = 99});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_ok_3, foo_integral_1_t{.s = -1, .u = 99});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_3));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_3));

  LAZY_OBJECT(obj_1, foo_integral_1_t{.s = 2, .u = -1u});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 's': "
    "Expects value to be none of [1, 2, 4, 8, 16, 4294967295], "
    "while actual value = 2",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 's':"
    "\n* Expects value to be none of [1, 2, 4, 8, 16, 4294967295], "
    "while actual value = 2"
    "\nInvalid member 'u':"
    "\n* Expects value < 100, while actual value = 4294967295"
    "\n* Expects value to be any of [-1, 1, 10, 11, 99], "
    "while actual value = 4294967295",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_integral_1_t{.s = 16, .u = 10});
  EXPECT_EQ_STATIC(
    "Invalid member 's': "
    "Expects value <= 10, while actual value = 16",
    validate_public_nsdm_msg(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 's':"
    "\n* Expects value <= 10, while actual value = 16"
    "\n* Expects value to be none of [1, 2, 4, 8, 16, 4294967295], "
    "while actual value = 16"
    "\nInvalid member 'u':"
    "\n* Expects value > 10, while actual value = 10",
    validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_integral_1_t{.s = -11, .u = 100});
  EXPECT_EQ_STATIC(
    "Invalid member 's': "
    "Expects value >= -10, while actual value = -11",
    validate_public_nsdm_msg(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 's':"
    "\n* Expects value >= -10, while actual value = -11"
    "\nInvalid member 'u':"
    "\n* Expects value < 100, while actual value = 100"
    "\n* Expects value to be any of [-1, 1, 10, 11, 99], "
    "while actual value = 100",
    validate_public_nsdm_msg_verbose(obj_3));

  LAZY_OBJECT(obj_4, foo_integral_1_t{.s = 5, .u = 5});
  EXPECT_EQ_STATIC(
    "Invalid member 'u': "
    "Expects value > 10, while actual value = 5",
    validate_public_nsdm_msg(obj_4));
  EXPECT_EQ_STATIC(
    "Invalid member 'u':"
    "\n* Expects value > 10, while actual value = 5"
    "\n* Expects value to be any of [-1, 1, 10, 11, 99], "
    "while actual value = 5",
    validate_public_nsdm_msg_verbose(obj_4));
}

struct foo_integral_2_t {
  VALIDATOR(in_closed_range, 0, 10)
  int32_t s;

  VALIDATOR(in_half_closed_range, -10, 10)
  uint32_t u;

  VALIDATOR(in_open_range, 0, 10)
  uint32_t v;
};

TEST(Validators, LeafBoundaryOptionsExclusionIntegral2)
{
  LAZY_OBJECT(obj_ok_1, foo_integral_2_t{.s = 0, .u = 9, .v = 8});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, foo_integral_2_t{.s = 10, .u = 0, .v = 1});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_1, foo_integral_2_t{.s = -1, .u = 10, .v = 0});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 's':"
    "\n* Expects value to fall in [0, 10], while actual value = -1"
    "\nInvalid member 'u':"
    "\n* Expects value to fall in [-10, 10), while actual value = 10"
    "\nInvalid member 'v':"
    "\n* Expects value to fall in (0, 10), while actual value = 0",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2,
    foo_integral_2_t{.s = 12, .u = static_cast<uint32_t>(-1), .v = 10});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 's':"
    "\n* Expects value to fall in [0, 10], while actual value = 12"
    "\nInvalid member 'u':"
    "\n* Expects value to fall in [-10, 10), while actual value = 4294967295"
    "\nInvalid member 'v':"
    "\n* Expects value to fall in (0, 10), while actual value = 10",
    validate_public_nsdm_msg_verbose(obj_2));
}

struct foo_fp_1_t {
  VALIDATOR(min, -1.0)
  VALIDATOR(max, 1.0)
  VALIDATOR(excludes, {-0.75, -0.5, -0.25, 0.25, 0.5, 0.75})
  float a;

  VALIDATOR(min_exclusive, -10.0)
  VALIDATOR(max_exclusive, 10.0)
  VALIDATOR(options, std::vector<double>{-8, -6, -4, -2, 2, 4, 6, 8, NAN})
  float b;

  VALIDATOR(in_closed_range, 0.0, 1.0)
  double x;

  VALIDATOR(in_half_closed_range, 0.0, 1.0)
  double y;

  VALIDATOR(in_open_range, 0.0, std::numeric_limits<float>::infinity())
  double z;
};

TEST(Validators, LeafBoundaryOptionsExclusionFP1)
{
  LAZY_OBJECT(obj_ok_1,
    foo_fp_1_t{.a = -1.0, .b = -8.0, .x = 1.0, .y = 0.0, .z = 0.5});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2,
    foo_fp_1_t{.a = 1.0, .b = 6.0, .x = 0.0, .y = 0.5, .z = 1e300});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_1,
    foo_fp_1_t{.a = 1.5, .b = -10.0, .x = 1.5, .y = 1.0, .z = 0.0});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ(
    "Invalid member 'a':"
    "\n* Expects value <= 1, while actual value = 1.5"
    "\nInvalid member 'b':"
    "\n* Expects value > -10, while actual value = -10"
    "\n* Expects value to be any of [-8, -6, -4, -2, 2, 4, 6, 8, nan], "
    "while actual value = -10"
    "\nInvalid member 'x':"
    "\n* Expects value to fall in [0, 1], while actual value = 1.5"
    "\nInvalid member 'y':"
    "\n* Expects value to fall in [0, 1), while actual value = 1"
    "\nInvalid member 'z':"
    "\n* Expects value to fall in (0, inf), while actual value = 0",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2,
    foo_fp_1_t{.a = -0.75, .b = 1.5, .x = -1.5, .y = -0.5, .z = -2.5});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ(
    "Invalid member 'a':"
    "\n* Expects value to be none of [-0.75, -0.5, -0.25, 0.25, 0.5, 0.75], "
    "while actual value = -0.75"
    "\nInvalid member 'b':"
    "\n* Expects value to be any of [-8, -6, -4, -2, 2, 4, 6, 8, nan], "
    "while actual value = 1.5"
    "\nInvalid member 'x':"
    "\n* Expects value to fall in [0, 1], while actual value = -1.5"
    "\nInvalid member 'y':"
    "\n* Expects value to fall in [0, 1), while actual value = -0.5"
    "\nInvalid member 'z':"
    "\n* Expects value to fall in (0, inf), while actual value = -2.5",
    validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3,
    foo_fp_1_t{.a = NAN, .b = NAN, .x = NAN, .y = NAN, .z = NAN});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ(
    "Invalid member 'a':"
    "\n* Expects value >= -1, while actual value = nan"
    "\n* Expects value <= 1, while actual value = nan"
    "\nInvalid member 'b':"
    "\n* Expects value > -10, while actual value = nan"
    "\n* Expects value < 10, while actual value = nan"
    "\n* Expects value to be any of [-8, -6, -4, -2, 2, 4, 6, 8, nan], "
    "while actual value = nan"
    "\nInvalid member 'x':"
    "\n* Expects value to fall in [0, 1], while actual value = nan"
    "\nInvalid member 'y':"
    "\n* Expects value to fall in [0, 1), while actual value = nan"
    "\nInvalid member 'z':"
    "\n* Expects value to fall in (0, inf), while actual value = nan",
    validate_public_nsdm_msg_verbose(obj_3));
}

struct foo_string_t {
  // Input type can be string literal (const char*)
  VALIDATOR(min, "pineapple")
  VALIDATOR(options, {"apple", "pineapple", "zebra"})
  std::string std_string;

  // Input type can be std::string_view
  VALIDATOR(min_exclusive, "000"sv)
  VALIDATOR(excludes, {"apple"sv, "zebra"sv})
  std::string_view std_string_view;

  // Input type can be std::string
  VALIDATOR(min_exclusive, "aaa"s)
  VALIDATOR(max_exclusive, std::string(3zU, 'z'))
  const char* c_style_str;
};

TEST(Validators, LeafBoundaryOptionsExclusionString)
{
  LAZY_OBJECT(obj_ok_1, foo_string_t{
    .std_string = "pineapple",
    .std_string_view = "pear",
    .c_style_str = "abc"});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_string_t{
    .std_string = "apple",
    .std_string_view = "apple",
    .c_style_str = "aaa"});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'std_string': Expects value >= \"pineapple\", "
    "while actual value = \"apple\"",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'std_string':"
    "\n* Expects value >= \"pineapple\", while actual value = \"apple\""
    "\nInvalid member 'std_string_view':"
    "\n* Expects value to be none of [\"apple\", \"zebra\"]"
    ", while actual value = \"apple\""
    "\nInvalid member 'c_style_str':"
    "\n* Expects value > \"aaa\", while actual value = \"aaa\"",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_string_t{
    .std_string = "yellow",
    .std_string_view = "000",
    .c_style_str = "zzzz"});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'std_string': Expects value to be any of "
    "[\"apple\", \"pineapple\", \"zebra\"], "
    "while actual value = \"yellow\"",
    validate_public_nsdm_msg(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'std_string':"
    "\n* Expects value to be any of [\"apple\", \"pineapple\", \"zebra\"], "
    "while actual value = \"yellow\""
    "\nInvalid member 'std_string_view':"
    "\n* Expects value > \"000\", while actual value = \"000\""
    "\nInvalid member 'c_style_str':"
    "\n* Expects value < \"zzz\", while actual value = \"zzzz\"",
    validate_public_nsdm_msg_verbose(obj_2));

  // c_style_str is nullptr
  LAZY_OBJECT(obj_3, foo_string_t{});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 'std_string': Expects value >= \"pineapple\", "
    "while actual value = \"\"",
    validate_public_nsdm_msg(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 'std_string':"
    "\n* Expects value >= \"pineapple\", while actual value = \"\""
    "\n* Expects value to be any of [\"apple\", \"pineapple\", \"zebra\"], "
    "while actual value = \"\""
    "\nInvalid member 'std_string_view':"
    "\n* Expects value > \"000\", while actual value = \"\""
    "\nInvalid member 'c_style_str':"
    "\n* Expects value > \"aaa\", while actual value = \"\"",
    validate_public_nsdm_msg_verbose(obj_3));
}

struct foo_char_t {
  VALIDATOR(min, 'a')
  VALIDATOR(max, 'z')
  VALIDATOR(excludes, {'a', 'e', 'i', 'o', 'u'}) // std::initializer_list
  char8_t c8;

  VALIDATOR(min_exclusive, 'A')
  VALIDATOR(max_exclusive, 'Z')
  VALIDATOR(options, u"AEIOU") // const char16_t*
  char16_t c16;

  VALIDATOR(options, "0123456789"s) // std::string
  char32_t c32;
};

TEST(Validators, LeafBoundaryOptionsExclusionChars)
{
  LAZY_OBJECT(obj_ok_1, foo_char_t{.c8 = 'z', .c16 = 'U', .c32 = '1'});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_char_t{.c8 = 'a', .c16 = 'A', .c32 = u'好'});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'c8': Expects value to be none of "
    "['a', 'e', 'i', 'o', 'u'], while actual value = 'a'",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'c8':"
    "\n* Expects value to be none of ['a', 'e', 'i', 'o', 'u'], "
    "while actual value = 'a'"
    "\nInvalid member 'c16':"
    "\n* Expects value > 'A', while actual value = 'A'"
    "\nInvalid member 'c32':"
    "\n* Expects value to be any of ['0', '1', '2', '3', '4', '5', '6', "
    "'7', '8', '9'], while actual value = '}'",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_char_t{.c8 = 'z', .c16 = 'z', .c32 = '5'});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'c16': Expects value < 'Z', while actual value = 'z'",
    validate_public_nsdm_msg(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'c16':"
    "\n* Expects value < 'Z', while actual value = 'z'"
    "\n* Expects value to be any of ['A', 'E', 'I', 'O', 'U'], "
    "while actual value = 'z'",
    validate_public_nsdm_msg_verbose(obj_2));
}

struct foo_range_1_t {
  // std::initializer_list<int>
  VALIDATOR(min, {1, 1, 1, 1})
  std::array<unsigned, 4> array_unsigned;

  // std::array<const char*, 3>
  VALIDATOR(min_exclusive, std::array{"AA", "BB", "CC"})
  // std::initializer_list<const char*> as single excluded option
  VALIDATOR(excludes, {"AAA", "BBB", "CCC"})
  std::vector<std::string> vector_string;

  // std::initializer_list<std::vector<int>>
  VALIDATOR(excludes, {
    std::vector{1, 2},
    std::vector{3, 4, 5}
  })
  std::deque<double> deque_double;
};

TEST(Validators, LeafBoundaryOptionsExclusionRange1)
{
  auto obj_ok_1 = foo_range_1_t{
    .array_unsigned = {1, 1, 1, 1},
    .vector_string = {"AA", "BB", "CC", "DD"},
    .deque_double = {1.25, 2.5, 3.75}
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  auto obj_ok_2 = foo_range_1_t{
    .array_unsigned = {-1u, 0, 0, 0},
    .vector_string = {"AA", "CC", "BB"},
    .deque_double = {1, 2, 3}
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_2));
  msg.clear();
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_2, &msg));
  EXPECT_EQ("", msg);

  auto obj_1 = foo_range_1_t{
    .array_unsigned = {0, 1, 2, 3},
    .vector_string = {"AA", "BB", "CC"},
    .deque_double = {1, 2}
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_1));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'array_unsigned': Expects value >= [1, 1, 1, 1], "
    "while actual value = [0, 1, 2, 3]", msg);

  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'array_unsigned':"
    "\n* Expects value >= [1, 1, 1, 1], while actual value = [0, 1, 2, 3]"
    "\nInvalid member 'vector_string':"
    "\n* Expects value > [\"AA\", \"BB\", \"CC\"], "
    "while actual value = [\"AA\", \"BB\", \"CC\"]"
    "\nInvalid member 'deque_double':"
    "\n* Expects value to be none of [[1, 2], [3, 4, 5]], "
    "while actual value = [1, 2]", msg);

  auto obj_2 = foo_range_1_t{
    .array_unsigned = {1, 2, 3, 4},
    .vector_string = {"AAA", "BBB", "CCC"},
    .deque_double = {1, 2, NAN}
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_2));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'vector_string': Expects value to be none of "
    "[\"AAA\", \"BBB\", \"CCC\"], while actual value = "
    "[\"AAA\", \"BBB\", \"CCC\"]", msg);

  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'vector_string':"
    "\n* Expects value to be none of [\"AAA\", \"BBB\", \"CCC\"], "
    "while actual value = [\"AAA\", \"BBB\", \"CCC\"]", msg);
}

struct foo_range_2_t {
  VALIDATOR(in_closed_range, std::array{0, 0, 0}, std::array{9, 9, 9})
  std::array<int, 3> arr;

  // std::initializer_list
  VALIDATOR(in_half_closed_range, {'a', 'b'}, {'x', 'y', 'z'})
  std::string str;

  VALIDATOR(in_open_range,
    std::vector<int32_t>{-3, -2, -1},
    std::vector<uint32_t>{7, 8, 9})
  std::vector<uint64_t> vec;
};

TEST(Validators, LeafBoundaryOptionsExclusionRange2)
{
  LAZY_OBJECT(obj_ok_1, foo_range_2_t{
    .arr = {1, 0, -1},
    .str = "abc",
    .vec = {3, 2, 1, 0},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, foo_range_2_t{
    .arr = {9, 9, 9},
    .str = "xy",
    .vec = {6, 7, 8, 9, 10},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));

  LAZY_OBJECT(obj_1, foo_range_2_t{
    .arr = {0, -1, 1},
    .str = "xyz",
    .vec = {3, 2, 1},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'arr':"
    "\n* Expects value to fall in [[0, 0, 0], [9, 9, 9]], "
    "while actual value = [0, -1, 1]"
    "\nInvalid member 'str':"
    "\n* Expects value to fall in [\"ab\", \"xyz\"), "
    "while actual value = \"xyz\"",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_range_2_t{
    .arr = {10},
    .str = "AB",
    .vec = {static_cast<uint64_t>(-2), static_cast<uint64_t>(-1)},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'arr':"
    "\n* Expects value to fall in [[0, 0, 0], [9, 9, 9]], "
    "while actual value = [10, 0, 0]"
    "\nInvalid member 'str':"
    "\n* Expects value to fall in [\"ab\", \"xyz\"), "
    "while actual value = \"AB\""
    "\nInvalid member 'vec':"
    "\n* Expects value to fall in ([-3, -2, -1], [7, 8, 9]), "
    "while actual value = [18446744073709551614, 18446744073709551615]",
    validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_range_2_t{
    .arr = {0, 0, 0},
    .str = "xylophone",
    .vec = {std::numeric_limits<uint32_t>::max() + uint64_t{7}},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 'vec':"
    "\n* Expects value to fall in ([-3, -2, -1], [7, 8, 9]), "
    "while actual value = [4294967302]",
    validate_public_nsdm_msg_verbose(obj_3));
}

struct foo_tuple_t {
  using tuple_type = std::tuple<
    int, std::string, std::vector<std::string>>;

  VALIDATOR(min, tuple_type{0, "AAA", {"AA", "BB"}})
  VALIDATOR(options, {
    tuple_type{1, "ABC", {"AB", "CD", "EF"}},
    tuple_type{2, "DEF", {"ABC", "DEF"}},
  })
  tuple_type tuple;
};

TEST(Validators, LeafBoundaryOptionsExclusionTuple)
{
  LAZY_OBJECT(obj_ok_1, foo_tuple_t{
    .tuple = {1, "ABC", {"AB", "CD", "EF"}}
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_tuple_t{
    .tuple = {-1, "XYZ", {"aa", "bb", "cc"}}
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'tuple': Expects value >= "
    "{0, \"AAA\", [\"AA\", \"BB\"]}, while actual value = "
    "{-1, \"XYZ\", [\"aa\", \"bb\", \"cc\"]}",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'tuple':"
    "\n* Expects value >= {0, \"AAA\", [\"AA\", \"BB\"]}, "
    "while actual value = {-1, \"XYZ\", [\"aa\", \"bb\", \"cc\"]}"
    "\n* Expects value to be any of "
    "[{1, \"ABC\", [\"AB\", \"CD\", \"EF\"]}, "
    "{2, \"DEF\", [\"ABC\", \"DEF\"]}], "
    "while actual value = {-1, \"XYZ\", [\"aa\", \"bb\", \"cc\"]}",
    validate_public_nsdm_msg_verbose(obj_1));
}

struct bar_A_t {
  VALIDATOR(min, 0)
  int a;
};

struct bar_B_t {
  VALIDATOR(max, 10)
  int b;
};

struct bar_C_t {
  VALIDATOR(options, {1, 2, 4, 8})
  int c;
};

struct bar_tree_t : bar_A_t, bar_B_t, bar_C_t {
  VALIDATOR(excludes, {1, 2, 4})
  int d;

  static constexpr auto create(int a, int b, int c, int d)
  {
    auto res = bar_tree_t{};
    res.a = a;
    res.b = b;
    res.c = c;
    res.d = d;
    return res;
  }
};

TEST(Validators, LeafBoundaryOptionsExclusionInheritance1)
{
  LAZY_OBJECT(obj_ok_1, bar_tree_t::create(6, 5, 4, 3));
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, bar_tree_t::create(-3, 22, -1, 1));
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'a': Expects value >= 0, while actual value = -3",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'a':"
    "\n* Expects value >= 0, while actual value = -3"
    "\nInvalid member 'b':"
    "\n* Expects value <= 10, while actual value = 22"
    "\nInvalid member 'c':"
    "\n* Expects value to be any of [1, 2, 4, 8], while actual value = -1"
    "\nInvalid member 'd':"
    "\n* Expects value to be none of [1, 2, 4], while actual value = 1",
    validate_public_nsdm_msg_verbose(obj_1));
}

struct baz_A_t {
  VALIDATOR(min, 0)
  int a;
};

struct baz_B_t : baz_A_t {
  VALIDATOR(max, 10)
  int b;
};

struct baz_C_t {
  VALIDATOR(options, {1, 2, 4, 8})
  int c;
};

struct baz_tree_t : baz_B_t, baz_C_t {
  VALIDATOR(excludes, {1, 2, 4})
  int d;

  static constexpr auto create(int a, int b, int c, int d)
  {
    auto res = baz_tree_t{};
    res.a = a;
    res.b = b;
    res.c = c;
    res.d = d;
    return res;
  }
};

TEST(Validators, LeafBoundaryOptionsExclusionInheritance2)
{
  LAZY_OBJECT(obj_ok_1, baz_tree_t::create(6, 5, 4, 3));
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, baz_tree_t::create(-3, 22, -1, 1));
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'a': Expects value >= 0, while actual value = -3",
    validate_public_nsdm_msg(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'a':"
    "\n* Expects value >= 0, while actual value = -3"
    "\nInvalid member 'b':"
    "\n* Expects value <= 10, while actual value = 22"
    "\nInvalid member 'c':"
    "\n* Expects value to be any of [1, 2, 4, 8], while actual value = -1"
    "\nInvalid member 'd':"
    "\n* Expects value to be none of [1, 2, 4], while actual value = 1",
    validate_public_nsdm_msg_verbose(obj_1));
}

struct foo_aliases_t {
  VALIDATOR(ge, 0)
  VALIDATOR(le, 10)
  int x;

  VALIDATOR(gt, 0)
  VALIDATOR(lt, 10)
  int y;

  VALIDATOR(eq, 0)
  VALIDATOR(ne, 1)
  int z;

  VALIDATOR(greater_equal, -1.0)
  VALIDATOR(less_equal, 1.0)
  double a;

  VALIDATOR(greater_than, -1.0)
  VALIDATOR(less_than, 1.0)
  double b;

  VALIDATOR(equal_to, 0.5)
  VALIDATOR(not_equal_to, 1.5)
  double c;
};

TEST(Validators, LeafBoundaryAliases)
{
  LAZY_OBJECT(obj_1, foo_aliases_t{
    .x = 0, .y = 0, .z = 0, .a = -1.0, .b = -1.0, .c = 0.5,
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ(
    "Invalid member 'y':"
    "\n* Expects value > 0, while actual value = 0"
    "\nInvalid member 'b':"
    "\n* Expects value > -1, while actual value = -1",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_aliases_t{
    .x = 10, .y = 10, .z = 1, .a = 1.0, .b = 1.0, .c = 1.5,
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ(
    "Invalid member 'y':"
    "\n* Expects value < 10, while actual value = 10"
    "\nInvalid member 'z':"
    "\n* Expects value == 0, while actual value = 1"
    "\n* Expects value != 1, while actual value = 1"
    "\nInvalid member 'b':"
    "\n* Expects value < 1, while actual value = 1"
    "\nInvalid member 'c':"
    "\n* Expects value == 0.5, while actual value = 1.5"
    "\n* Expects value != 1.5, while actual value = 1.5",
    validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_aliases_t{
    .x = -1, .y = 1, .z = -1, .a = NAN, .b = NAN, .c = NAN,
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ(
    "Invalid member 'x':"
    "\n* Expects value >= 0, while actual value = -1"
    "\nInvalid member 'z':"
    "\n* Expects value == 0, while actual value = -1"
    "\nInvalid member 'a':"
    "\n* Expects value >= -1, while actual value = nan"
    "\n* Expects value <= 1, while actual value = nan"
    "\nInvalid member 'b':"
    "\n* Expects value > -1, while actual value = nan"
    "\n* Expects value < 1, while actual value = nan"
    "\nInvalid member 'c':"
    "\n* Expects value == 0.5, while actual value = nan",
    validate_public_nsdm_msg_verbose(obj_3));
}
