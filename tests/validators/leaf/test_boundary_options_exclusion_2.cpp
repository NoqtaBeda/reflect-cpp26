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
 * The following validators are tested with member relations:
 * - Bound checking: min, max, min_exclusive, max_exclusive
 * - Options checking: options, excludes
 */

struct foo_1_t {
  int x;
  VALIDATOR(greater_than, prev_nsdm)
  VALIDATOR(less_than, next_nsdm)
  unsigned y;
  int z;
};

TEST(Validators, LeafBoundaryWithNSDMRelation1)
{
  LAZY_OBJECT(obj_ok_1, foo_1_t{.x = 1, .y = 2, .z = 3});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, foo_1_t{.x = -1, .y = 2, .z = 3});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_1, foo_1_t{.x = 10, .y = 9, .z = 8});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'y':"
    "\n* Expects value > member 'x' which is 10, while actual value = 9"
    "\n* Expects value < member 'z' which is 8, while actual value = 9",
    validate_public_nsdm_msg_verbose(obj_1));
}

struct foo_2_t {
  VALIDATOR(not_equal_to, next_ith_nsdm<2>)
  std::string a;

  VALIDATOR(less_equal, first_nsdm)
  VALIDATOR(greater_equal, last_nsdm)
  std::string_view b;

  const char* c;

  VALIDATOR(greater_than, prev_ith_nsdm<3>)
  std::string d;
};

TEST(Validators, LeafBoundaryWithNSDMRelation2)
{
  LAZY_OBJECT(obj_1, foo_2_t{.a = "abc", .b = "ABC", .c = "cba", .d = "xyz"});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'b':"
    "\n* Expects value >= member 'd' which is \"xyz\", "
    "while actual value = \"ABC\"",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_2_t{.a = "xyz", .b = "zyx", .c = "xyz", .d = "xyz"});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'a':"
    "\n* Expects value != member 'c' which is \"xyz\", "
    "while actual value = \"xyz\""
    "\nInvalid member 'b':"
    "\n* Expects value <= member 'a' which is \"xyz\", "
    "while actual value = \"zyx\""
    "\nInvalid member 'd':"
    "\n* Expects value > member 'a' which is \"xyz\", "
    "while actual value = \"xyz\"",
    validate_public_nsdm_msg_verbose(obj_2));
}

struct foo_3_t {
  VALIDATOR(not_equal_to, ith_nsdm<2>) // v0 != v2
  std::vector<int> v0;

  VALIDATOR(not_equal_to, last_ith_nsdm<0>) // v1 != a3
  std::vector<unsigned> v1;

  VALIDATOR(greater_equal, prev_nsdm) // a2 >= v1
  std::array<int, 4> a2;

  VALIDATOR(greater_equal, last_ith_nsdm<3>) // v1 >= a3 >= v0
  VALIDATOR(less_equal, prev_ith_nsdm<2>)
  unsigned a3[3];
};

TEST(Validators, LeafBoundaryWithNSDMRelation3)
{
  LAZY_OBJECT(obj_ok_1, foo_3_t{
    .v0 = {0, 1, 2},
    .v1 = {1, 2, 3, 4},
    .a2 = {1, 2, 4, 8},
    .a3 = {0, 1, 2},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg_verbose(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_3_t{
    .v0 = {1, 2, 3, 4},
    .v1 = {5, 6, 7, 8},
    .a2 = {1, 2, 3, 4},
    .a3 = {2, 3, 4},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v0':"
    "\n* Expects value != member 'a2' which is [1, 2, 3, 4], "
    "while actual value = [1, 2, 3, 4]"
    "\nInvalid member 'a2':"
    "\n* Expects value >= member 'v1' which is [5, 6, 7, 8], "
    "while actual value = [1, 2, 3, 4]",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_3_t{
    .v0 = {-1, 2, 4, 8},
    .v1 = {3, 6, 9},
    .a2 = {-1, -2, -4, -8},
    .a3 = {3, 6, 9},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects value != member 'a3' which is [3, 6, 9], "
    "while actual value = [3, 6, 9]"
    "\nInvalid member 'a2':"
    "\n* Expects value >= member 'v1' which is [3, 6, 9], "
    "while actual value = [-1, -2, -4, -8]",
    validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_3_t{
    .v0 = {1, 2, 3, 4},
    .v1 = {1, 2},
    .a2 = {-1, -2, -3, -4},
    .a3 = {1, 2, 3},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 'a2':"
    "\n* Expects value >= member 'v1' which is [1, 2], "
    "while actual value = [-1, -2, -3, -4]"
    "\nInvalid member 'a3':"
    "\n* Expects value >= member 'v0' which is [1, 2, 3, 4], "
    "while actual value = [1, 2, 3]"
    "\n* Expects value <= member 'v1' which is [1, 2], "
    "while actual value = [1, 2, 3]",
    validate_public_nsdm_msg_verbose(obj_3));
}

struct foo_4_t {
private:
  VALIDATOR(options, {1, 3, 6, 10, 15})
  int left;
public:
  VALIDATOR(excludes, {1, 3, 6, 10, 15})
  unsigned right;

  VALIDATOR(in_open_range, first_nsdm, first_public_nsdm)
  int cur_value;

  constexpr foo_4_t(int left, unsigned right, int cur_value)
    : left(left), right(right), cur_value(cur_value) {}
};

TEST(Validators, LeafBoundaryWithNSDMRelation4)
{
  LAZY_OBJECT(obj_ok_1, foo_4_t{1, -1u, 10});
  EXPECT_TRUE_STATIC(validate_all_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_all_nsdm_msg_verbose(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_4_t{2, 4, 3});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'left':"
    "\n* Expects value to be any of [1, 3, 6, 10, 15], while actual value = 2",
    validate_all_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_4_t{4, 16, -1});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'left':"
    "\n* Expects value to be any of [1, 3, 6, 10, 15], while actual value = 4"
    "\nInvalid member 'cur_value':"
    "\n* Expects value to fall in (member 'left' which is 4, "
    "member 'right' which is 16), while actual value = -1",
    validate_all_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_4_t{4, 15, 60});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 'left':"
    "\n* Expects value to be any of [1, 3, 6, 10, 15], while actual value = 4"
    "\nInvalid member 'right':"
    "\n* Expects value to be none of [1, 3, 6, 10, 15], while actual value = 15"
    "\nInvalid member 'cur_value':"
    "\n* Expects value to fall in (member 'left' which is 4, "
    "member 'right' which is 15), while actual value = 60",
    validate_all_nsdm_msg_verbose(obj_3));
}

struct foo_5_t : public foo_4_t {
private:
  VALIDATOR(in_half_closed_range, last_public_nsdm, last_nsdm)
  VALIDATOR(less_than, ith_public_nsdm<1>) // foo_4_t::cur_value
  int next_value;

public:
  VALIDATOR(greater_than, prev_ith_nsdm<4>) // foo_4_t::left
  int next_left;

  constexpr foo_5_t(
    int left, unsigned right, int next_left, unsigned next_right,
    int cur_value, int next_value)
    : foo_4_t(left, right, cur_value), next_value(next_value),
      next_left(next_left), next_right(next_right) {}

private:
  VALIDATOR(greater_than, prev_ith_public_nsdm<3>) // foo_4_t::right
  unsigned next_right;
};

TEST(Validators, LeafBoundaryWithNSDMRelation5)
{
  LAZY_OBJECT(obj_ok_1, foo_5_t{1, -10u, 10, -1u, 200, 100});
  EXPECT_TRUE_STATIC(validate_all_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_all_nsdm_msg_verbose(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_5_t{2, 20u, 3, 20u, 4, 5});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'left':"
    "\n* Expects value to be any of [1, 3, 6, 10, 15], while actual value = 2"
    "\nInvalid member 'next_value':"
    "\n* Expects value < member 'cur_value' which is 4, while actual value = 5"
    "\nInvalid member 'next_right':"
    "\n* Expects value > member 'right' which is 20, while actual value = 20",
    validate_all_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_5_t{7, 6, 5, 4, 1, 2});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'right': "
    "Expects value to be none of [1, 3, 6, 10, 15], while actual value = 6",
    validate_public_nsdm_msg(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'left':"
    "\n* Expects value to be any of [1, 3, 6, 10, 15], while actual value = 7"
    "\nInvalid member 'right':"
    "\n* Expects value to be none of [1, 3, 6, 10, 15], while actual value = 6"
    "\nInvalid member 'cur_value':"
    "\n* Expects value to fall in (member 'left' which is 7, "
    "member 'right' which is 6), while actual value = 1"
    "\nInvalid member 'next_value':"
    "\n* Expects value to fall in [member 'next_left' which is 5, "
    "member 'next_right' which is 4), while actual value = 2"
    "\n* Expects value < member 'cur_value' which is 1, while actual value = 2"
    "\nInvalid member 'next_left':"
    "\n* Expects value > member 'left' which is 7, while actual value = 5"
    "\nInvalid member 'next_right':"
    "\n* Expects value > member 'right' which is 6, while actual value = 4",
    validate_all_nsdm_msg_verbose(obj_2));
}

struct foo_6_t {
  VALIDATOR(in_half_closed_range, std::array{0, 0, 0, 0}, next_public_nsdm)
  VALIDATOR(in_closed_range, next_nsdm, std::vector{9, 9, 9, 9})
  std::vector<int> cur_values;

  constexpr foo_6_t(
    std::initializer_list<int> cur_values, std::array<unsigned, 4> min_values,
    std::array<unsigned, 4> max_values)
    : cur_values(cur_values), min_values(min_values), max_values(max_values) {}

private:
  VALIDATOR(less_than, next_nsdm)
  std::array<unsigned, 4> min_values;

public:
  std::array<unsigned, 4> max_values;
};

TEST(Validators, LeafBoundaryWithNSDMRelation6)
{
  LAZY_OBJECT(obj_ok_1, foo_6_t{{1, 2, 3, 4}, {0, 1, 2, 3}, {4, 5, 6, 7}});
  EXPECT_TRUE_STATIC(validate_all_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_6_t{{}, {1, 2, 3, 4}, {5, 6, 7, 8}});
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'cur_values':"
    "\n* Expects value to fall in [[0, 0, 0, 0], "
    "member 'max_values' which is [5, 6, 7, 8]), while actual value = []"
    "\n* Expects value to fall in [member 'min_values' which is [1, 2, 3, 4], "
    "[9, 9, 9, 9]], while actual value = []",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_6_t{{-1, 2, 3, 4}, {7, 6, 5, 4}, {3, 2, 1, 0}});
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'cur_values':"
    "\n* Expects value to fall in [[0, 0, 0, 0], "
    "member 'max_values' which is [3, 2, 1, 0]), "
    "while actual value = [-1, 2, 3, 4]"
    "\n* Expects value to fall in [member 'min_values' which is [7, 6, 5, 4], "
    "[9, 9, 9, 9]], while actual value = [-1, 2, 3, 4]",
    validate_public_nsdm_msg_verbose(obj_2));
}

struct bar_1_t {
  VALIDATOR(options, last_ith_public_nsdm<1>) // bar_1_t::s1
  wchar_t c;

  constexpr bar_1_t(wchar_t c, wchar_t d, std::wstring_view s1,
                    std::wstring_view s2, std::wstring_view s3)
    : c(c), d(d), s1(s1), s2(s2), s3(s3) {}
private:
  VALIDATOR(not_equal_to, last_ith_public_nsdm<2>) // bar_1_t::c
  wchar_t d;
public:
  std::wstring s1;
private:
  std::wstring s2;
public:
  VALIDATOR(excludes, prev_public_nsdm) // bar_1_t::s1
  std::wstring s3;
};

TEST(Validators, LeafOptionsExclusionWithNSDMRelation1)
{
  LAZY_OBJECT(obj_ok_1, bar_1_t{L'A', L'B', L"APPLE", L"BANANA", L"BANANA"});
  EXPECT_TRUE_STATIC(validate_all_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_1, bar_1_t{L'A', L'A', L"Apple", L"Banana", L"apple"});
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'd':"
    "\n* Expects value != member 'c' which is 'A', while actual value = 'A'",
    validate_all_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, bar_1_t{L'A', L'B', L"Cat", L"Dog", L"Cat"});
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_2));
  EXPECT_THAT(validate_all_nsdm_msg_verbose(obj_2), testing::MatchesRegex(
    "Invalid member 'c':"
    "\n\\* Expects value to be any of member 's1' which is "
    "\\['C', 'a', 't'\\], while actual value = 'A'"
    "\nInvalid member 's3':"
    "\n\\* Expects value to be none of member 's1' which is .*C.*a.*t.*, "
    "while actual value = .*C.*a.*t.*"));
}

struct bar_2_t {
  std::vector<unsigned> v1;
  std::vector<unsigned> v2;

  VALIDATOR(options, prev_ith_nsdm<2>) // bar_2_t::v1
  VALIDATOR(excludes, prev_ith_nsdm<1>) // bar_2_t::v2
  int i;

  VALIDATOR(excludes, ith_nsdm<1>) // bar_2_t::v2
  std::deque<int> d;
};

TEST(Validators, LeafOptionsExclusionWithNSDMRelation2)
{
  auto obj_ok_1 = bar_2_t{
    .v1 = {1, 2, 3, 4, 5},
    .v2 = {1, 2, 4},
    .i = 3,
    .d = {1, 2, 4, 8},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));

  auto obj_1 = bar_2_t{
    .v1 = {1, 2, 3, 4, 5, UINT_MAX},
    .v2 = {1, 2, 4, UINT_MAX},
    .i = -1,
    .d = {1, 2, 4},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_1));
  auto msg = std::string{};
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'i':"
    "\n* Expects value to be any of member 'v1' which is "
    "[1, 2, 3, 4, 5, 4294967295], while actual value = -1", msg);

  auto obj_2 = bar_2_t{
    .v1 = {1, 2, 3, 5, UINT_MAX},
    .v2 = {1, 2, 4},
    .i = 4,
    .d = {1, 2, 4},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_2));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'i':"
    "\n* Expects value to be any of member 'v1' which is "
    "[1, 2, 3, 5, 4294967295], while actual value = 4"
    "\n* Expects value to be none of member 'v2' which is [1, 2, 4], "
    "while actual value = 4"
    "\nInvalid member 'd':"
    "\n* Expects value to be none of member 'v2' which is [1, 2, 4], "
    "while actual value = [1, 2, 4]", msg);
}
