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
#include <forward_list>
#include <list>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/validators.hpp>
#else
#include <reflect_cpp26/validators/leaf/prefix_suffix_test.hpp>
#endif

struct foo_integral_single_t {
  VALIDATOR(starts_with, 0)
  VALIDATOR(ends_with, 1)
  std::deque<int> deque; // Random-access

  VALIDATOR(starts_with, 2)
  VALIDATOR(ends_with, 3)
  std::list<int> list; // Bidirectional

  VALIDATOR(starts_with, 4)
  VALIDATOR(ends_with, 5)
  std::forward_list<int> forward_list; // Unidirectional
};

TEST(Validators, LeafPrefixSuffixIntegralSingle)
{
  auto obj_ok_1 = foo_integral_single_t{
    .deque = {0, -1, 1},
    .list = {2, -1, 0, 1, 2, 3},
    .forward_list = {4, 3, 2, 1, 0, 5},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  auto obj_1 = foo_integral_single_t{
    .deque = {1, 0},
    .list = {3, 2},
    .forward_list = {},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_1));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range [1, 0] does not start with 0"
    "\n* Input range [1, 0] does not end with 1"
    "\nInvalid member 'list':"
    "\n* Input range [3, 2] does not start with 2"
    "\n* Input range [3, 2] does not end with 3"
    "\nInvalid member 'forward_list':"
    "\n* Input range is empty"
    "\n* Input range is empty", msg);

  auto obj_2 = foo_integral_single_t{
    .forward_list = {5, 4},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_2));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range is empty"
    "\n* Input range is empty"
    "\nInvalid member 'list':"
    "\n* Input range is empty"
    "\n* Input range is empty"
    "\nInvalid member 'forward_list':"
    "\n* Input range [5, 4] does not start with 4"
    "\n* Input range [5, 4] does not end with 5", msg);
}

struct foo_integral_range_1_t {
  VALIDATOR(starts_with, std::vector{1, 2, 3})
  VALIDATOR(ends_with, std::vector{3, 2, 1})
  std::deque<int> deque; // Random-access

  VALIDATOR(starts_with, {4, 5, 6})
  VALIDATOR(ends_with, {6, 5})
  std::list<int> list; // Bidirectional

  VALIDATOR(starts_with, std::array{1, 1})
  VALIDATOR(ends_with, std::array{1, 1, 0})
  std::forward_list<int> forward_list; // Unidirectional

  VALIDATOR(starts_with, std::vector<int>{})
  VALIDATOR(ends_with, std::vector<int>{})
  std::deque<std::deque<int>> nested_deque;

  VALIDATOR(starts_with, {4, 5, 6})
  VALIDATOR(ends_with, {6, 5})
  std::list<std::list<int>> nested_list;

  VALIDATOR(starts_with, std::array{1, 1})
  VALIDATOR(ends_with, std::array{1, 1})
  std::forward_list<std::forward_list<int>> nested_forward_list;
};

TEST(Validators, LeafPrefixSuffixIntegralRange1)
{
  auto obj_ok_1 = foo_integral_range_1_t{
    .deque = {1, 2, 3, 4, 3, 2, 1},
    .list = {4, 5, 6, 5},
    .forward_list = {1, 1, 1, 0},
    .nested_deque = {{}, {4, 5, 6}, {}},
    .nested_list = {{4, 5, 6}, {6, 5}},
    .nested_forward_list = {{1, 1}},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  auto obj_1 = foo_integral_range_1_t{
    .deque = {1, 2, 3},
    .list = {3, 4, 5, 6, 5},
    .forward_list = {1, 1},
    .nested_deque = {{1}, {2}, {1, 2}},
    .nested_list = {{4}, {5}, {6}, {5}},
    .nested_forward_list = {},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_1));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range [1, 2, 3] does not end with [3, 2, 1]"
    "\nInvalid member 'list':"
    "\n* Input range [3, 4, 5, 6, 5] does not start with [4, 5, 6]"
    "\nInvalid member 'forward_list':"
    "\n* Input range [1, 1] does not end with [1, 1, 0]"
    "\nInvalid member 'nested_deque':"
    "\n* Input range [[1], [2], [1, 2]] does not start with []"
    "\n* Input range [[1], [2], [1, 2]] does not end with []"
    "\nInvalid member 'nested_list':"
    "\n* Input range [[4], [5], [6], [5]] does not start with [4, 5, 6]"
    "\n* Input range [[4], [5], [6], [5]] does not end with [6, 5]"
    "\nInvalid member 'nested_forward_list':"
    "\n* Input range is empty"
    "\n* Input range is empty", msg);

  auto obj_2 = foo_integral_range_1_t{
    .deque = {4, 3, 2, 1},
    .list = {4, 5, 6, 5, 4},
    .forward_list = {2, 2, 1, 1, 0, 0},
    .nested_deque = {},
    .nested_list = {},
    .nested_forward_list = {{1}, {1}, {1}},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_2));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range [4, 3, 2, 1] does not start with [1, 2, 3]"
    "\nInvalid member 'list':"
    "\n* Input range [4, 5, 6, 5, 4] does not end with [6, 5]"
    "\nInvalid member 'forward_list':"
    "\n* Input range [2, 2, 1, 1, 0, 0] does not start with [1, 1]"
    "\n* Input range [2, 2, 1, 1, 0, 0] does not end with [1, 1, 0]"
    "\nInvalid member 'nested_deque':"
    "\n* Input range is empty"
    "\n* Input range is empty"
    "\nInvalid member 'nested_list':"
    "\n* Input range is empty"
    "\n* Input range is empty"
    "\nInvalid member 'nested_forward_list':"
    "\n* Input range [[1], [1], [1]] does not start with [1, 1]"
    "\n* Input range [[1], [1], [1]] does not end with [1, 1]", msg);
}

struct foo_integral_range_2_t {
  VALIDATOR(starts_with, std::vector<int>{})
  VALIDATOR(ends_with, std::vector<int>{})
  std::vector<int> always_passes_validation;

  VALIDATOR(starts_with, std::vector<int>{})
  VALIDATOR(ends_with, std::vector<int>{})
  std::vector<std::vector<int>> may_fail_validation;
};

TEST(Validators, LeafPrefixSuffixIntegralRange2)
{
  LAZY_OBJECT(obj_ok_1, foo_integral_range_2_t{
    .always_passes_validation = {},
    .may_fail_validation = {{}},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, foo_integral_range_2_t{
    .always_passes_validation = {1, 2, 3},
    .may_fail_validation = {{}, {1, 2, 3}, {}},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_2));

  LAZY_OBJECT(obj_1, foo_integral_range_2_t{
    .always_passes_validation = {},
    .may_fail_validation = {{0}},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'may_fail_validation':"
    "\n* Input range [[0]] does not start with []"
    "\n* Input range [[0]] does not end with []",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_integral_range_2_t{
    .always_passes_validation = {},
    .may_fail_validation = {},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'may_fail_validation':"
    "\n* Input range is empty"
    "\n* Input range is empty",
    validate_public_nsdm_msg_verbose(obj_2));
}

struct foo_string_1_t {
  VALIDATOR(starts_with, 'a')
  VALIDATOR(ends_with, 'z')
  std::string s1;

  VALIDATOR(starts_with, "Hello")
  VALIDATOR(ends_with, "!")
  std::string s2;

  VALIDATOR(starts_with, "Cat")
  VALIDATOR(ends_with, "Dog")
  std::vector<std::string> slist;
};

TEST(Validators, LeafPrefixSuffixString1)
{
  LAZY_OBJECT(obj_ok_1, foo_string_1_t{
    .s1 = "abcxyz",
    .s2 = "Hello world!",
    .slist = {"Cat", "Wolf", "Fox", "Dog"},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_string_1_t{
    .s1 = "abc",
    .s2 = "Hello world",
    .slist = {"Dog", "Cat"},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 's1':"
    "\n* Input string \"abc\" does not end with 'z'"
    "\nInvalid member 's2':"
    "\n* Input string \"Hello world\" does not end with \"!\""
    "\nInvalid member 'slist':"
    "\n* Input range [\"Dog\", \"Cat\"] does not start with \"Cat\""
    "\n* Input range [\"Dog\", \"Cat\"] does not end with \"Dog\"",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_string_1_t{
    .s1 = "xyz",
    .s2 = "Holy shit!",
    .slist = {},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 's1':"
    "\n* Input string \"xyz\" does not start with 'a'"
    "\nInvalid member 's2':"
    "\n* Input string \"Holy shit!\" does not start with \"Hello\""
    "\nInvalid member 'slist':"
    "\n* Input range is empty"
    "\n* Input range is empty", validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_string_1_t{
    .s1 = "",
    .s2 = "",
    .slist = {"D", "o", "g", "C", "a", "t"},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 's1':"
    "\n* Input string is empty"
    "\n* Input string is empty"
    "\nInvalid member 's2':"
    "\n* Input string is empty"
    "\n* Input string is empty"
    "\nInvalid member 'slist':"
    "\n* Input range [\"D\", \"o\", \"g\", \"C\", \"a\", \"t\"] "
    "does not start with \"Cat\""
    "\n* Input range [\"D\", \"o\", \"g\", \"C\", \"a\", \"t\"] "
    "does not end with \"Dog\"",
    validate_public_nsdm_msg_verbose(obj_3));
}

struct foo_string_2_t {
  VALIDATOR(starts_with, "")
  VALIDATOR(ends_with, "")
  const char* always_passes_validation;

  VALIDATOR(starts_with, 'H')
  VALIDATOR(ends_with, 'd')
  const char* may_fail_validation_1;

  VALIDATOR(ends_with, "")
  std::forward_list<std::string_view> may_fail_validation_2;

  VALIDATOR(starts_with, "Hello")
  VALIDATOR(ends_with, '!')
  std::vector<char> vector_char; // String-like type
};

TEST(Validators, LeafPrefixSuffixString2)
{
  LAZY_OBJECT(obj_ok_1, foo_string_2_t{
    .always_passes_validation = "hello",
    .may_fail_validation_1 = "Helloworld",
    .may_fail_validation_2 = {"hello", "world", ""},
    .vector_char = {'H', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'},
  });
  EXPECT_TRUE(validate_public_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_string_2_t{
    .always_passes_validation = nullptr, // nullptr is also empty string
    .may_fail_validation_1 = "What the Hell is this world",
    .may_fail_validation_2 = {"", "hello", "world"},
    .vector_char = {'H', 'e', 'l', 'l'},
  });
  EXPECT_FALSE(validate_public_nsdm(obj_1));
  EXPECT_THAT(validate_public_nsdm_msg_verbose(obj_1), testing::MatchesRegex(
    "Invalid member 'may_fail_validation_1':"
    "\n\\* Input string \"What the Hell is this world\" does not start with 'H'"
    "\nInvalid member 'may_fail_validation_2':"
    "\n\\* Input range \\[\"\", \"hello\", \"world\"\\] does not end with \"\""
    "\nInvalid member 'vector_char':"
    "\n\\* Input (string|range) \\['H', 'e', 'l', 'l'\\] "
    "does not start with \"Hello\""
    "\n\\* Input (string|range) \\['H', 'e', 'l', 'l'\\] "
    "does not end with '!'"));

  LAZY_OBJECT(obj_2, foo_string_2_t{
    .always_passes_validation = "",
    .may_fail_validation_1 = nullptr,
    .may_fail_validation_2 = {},
    .vector_char = {},
  });
  EXPECT_FALSE(validate_public_nsdm(obj_2));
  EXPECT_THAT(validate_public_nsdm_msg_verbose(obj_2), testing::MatchesRegex(
    "Invalid member 'may_fail_validation_1':"
    "\n\\* Input string is empty"
    "\n\\* Input string is empty"
    "\nInvalid member 'may_fail_validation_2':"
    "\n\\* Input range is empty"
    "\nInvalid member 'vector_char':"
    "\n\\* Input (string|range) is empty"
    "\n\\* Input (string|range) is empty"));
}

struct foo_with_relation_tag_1_t {
  int s1;
  VALIDATOR(starts_with, prev_nsdm)
  std::vector<unsigned> v1;

  int s2;
  VALIDATOR(ends_with, prev_nsdm)
  std::vector<unsigned> v2;

  VALIDATOR(starts_with, ith_nsdm<1>) // v1
  VALIDATOR(ends_with, ith_nsdm<3>) // v2
  std::vector<int> v_full;
};

TEST(Validators, LeafPrefixSuffixWithNSDMRelation1)
{
  LAZY_OBJECT(obj_ok_1, foo_with_relation_tag_1_t{
    .s1 = 10,
    .v1 = {10, 20, 30},
    .s2 = 60,
    .v2 = {40, 50, 60},
    .v_full = {10, 20, 30, 40, 50, 60},
  });
  EXPECT_TRUE_STATIC(validate_all_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_with_relation_tag_1_t{
    .s1 = -1,
    .v1 = {-1u, 2, 3},
    .s2 = -6,
    .v2 = {4, 5, -6u},
    .v_full = {-1, 2, 3, 4, 5, -6},
  });
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Input range [4294967295, 2, 3] "
    "does not start with member 's1' which is -1"
    "\nInvalid member 'v2':"
    "\n* Input range [4, 5, 4294967290] "
    "does not end with member 's2' which is -6"
    "\nInvalid member 'v_full':"
    "\n* Input range [-1, 2, 3, 4, 5, -6] "
    "does not start with member 'v1' which is [4294967295, 2, 3]"
    "\n* Input range [-1, 2, 3, 4, 5, -6] "
    "does not end with member 'v2' which is [4, 5, 4294967290]",
    validate_all_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_with_relation_tag_1_t{
    .s1 = -1,
    .v1 = {},
    .s2 = -6,
    .v2 = {},
    .v_full = {},
  });
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Input range is empty"
    "\nInvalid member 'v2':"
    "\n* Input range is empty",
    validate_all_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_with_relation_tag_1_t{
    .s1 = 1,
    .v1 = {1},
    .s2 = 6,
    .v2 = {6},
    .v_full = {},
  });
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 'v_full':\n* Input range is empty\n* Input range is empty",
    validate_all_nsdm_msg_verbose(obj_3));

  LAZY_OBJECT(obj_4, foo_with_relation_tag_1_t{
    .s1 = 1,
    .v1 = {},
    .s2 = 6,
    .v2 = {},
    .v_full = {1, 6},
  });
  EXPECT_FALSE_STATIC(validate_all_nsdm(obj_4));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Input range is empty"
    "\nInvalid member 'v2':"
    "\n* Input range is empty",
    validate_all_nsdm_msg_verbose(obj_4));
}

struct foo_with_relation_tag_2_t {
  unsigned bf1: 4;
  unsigned bf2: 8;

  VALIDATOR(starts_with, prev_ith_nsdm<2>)
  VALIDATOR(ends_with, prev_ith_nsdm<1>)
  std::vector<int> values;

  const char* & cs1;
  VALIDATOR(starts_with, prev_nsdm)
  const wchar_t* & cs2;

  VALIDATOR(starts_with, prev_ith_nsdm<2>)
  VALIDATOR(ends_with, prev_ith_nsdm<1>)
  std::vector<std::u8string> vs;
};

TEST(Validators, LeafPrefixSuffixWithNSDMRelation2)
{
  const char* s1 = "Hello";
  const wchar_t* s2 = L"HelloWorld";
  auto obj_ok_1 = foo_with_relation_tag_2_t{
    .bf1 = 3,
    .bf2 = 12,
    .values = {3, 6, 9, 12},
    .cs1 = s1,
    .cs2 = s2,
    .vs = {u8"Hello", u8"World", u8"HelloWorld"},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));

  const wchar_t* s3 = L"helloworld";
  auto obj_1 = foo_with_relation_tag_2_t{
    .bf1 = 3,
    .bf2 = 12,
    .values = {19, 28},
    .cs1 = s1,
    .cs2 = s3,
    .vs = {u8"你好", u8"世界", u8"hello", u8"world"},
  };
  auto msg = std::string{};
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_THAT(msg, testing::MatchesRegex(
    "Invalid member 'values':"
    "\n\\* Input range \\[19, 28\\] "
    "does not start with member 'bf1' which is 3"
    "\n\\* Input range \\[19, 28\\] "
    "does not end with member 'bf2' which is 12"
    "\nInvalid member 'cs2':"
    "\n\\* Input string .*does not start with member 'cs1' which is \"Hello\".*"
    "\nInvalid member 'vs':"
    "\n\\* Input range .*does not start with member 'cs1' which is \"Hello\".*"
    "\n\\* Input range .*does not end with member 'cs2'.*"));

  auto obj_2 = foo_with_relation_tag_2_t{
    .bf1 = 3,
    .bf2 = 12,
    .values = {},
    .cs1 = s1,
    .cs2 = s3,
    .vs = {},
  };
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_THAT(msg, testing::MatchesRegex(
    "Invalid member 'values':"
    "\n\\* Input range is empty"
    "\n\\* Input range is empty"
    "\nInvalid member 'cs2':"
    "\n\\* Input string .*does not start with member 'cs1' which is \"Hello\""
    "\nInvalid member 'vs':"
    "\n\\* Input range is empty"
    "\n\\* Input range is empty"));
}

struct foo_with_relation_tag_3_t {
  const char* s1;

  VALIDATOR(starts_with, prev_nsdm)
  VALIDATOR(ends_with, prev_nsdm)
  const char* s2;
};

TEST(Validators, LeafPrefixSuffixWithNSDMRelation3)
{
  LAZY_OBJECT(obj_ok_1, foo_with_relation_tag_3_t{
    .s1 = "hello",
    .s2 = "hello",
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_ok_2, foo_with_relation_tag_3_t{
    .s1 = "tacotaco",
    .s2 = "tacotacotaco",
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_2));

  LAZY_OBJECT(obj_ok_3, foo_with_relation_tag_3_t{
    .s1 = "",
    .s2 = "",
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_3));

  LAZY_OBJECT(obj_ok_4, foo_with_relation_tag_3_t{
    .s1 = nullptr,
    .s2 = "tacotacotaco",
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_4));

  LAZY_OBJECT(obj_ok_5, foo_with_relation_tag_3_t{
    .s1 = nullptr,
    .s2 = "",
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_5));

  LAZY_OBJECT(obj_ok_6, foo_with_relation_tag_3_t{
    .s1 = "",
    .s2 = nullptr,
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_6));

  LAZY_OBJECT(obj_1, foo_with_relation_tag_3_t{
    .s1 = "hello",
    .s2 = "hell no",
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 's2':"
    "\n* Input string \"hell no\" does not start with "
    "member 's1' which is \"hello\""
    "\n* Input string \"hell no\" does not end with "
    "member 's1' which is \"hello\"",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_with_relation_tag_3_t{
    .s1 = "hello",
    .s2 = nullptr,
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 's2':\n* Input string is empty\n* Input string is empty",
    validate_public_nsdm_msg_verbose(obj_2));
}

struct foo_with_relation_tag_4_t {
  const char* prefix;
  const char* suffix;

  VALIDATOR(starts_with, ith_nsdm<0>)
  VALIDATOR(ends_with, ith_nsdm<1>)
  std::deque<char16_t> deque_chars;

  VALIDATOR(starts_with, ith_nsdm<0>)
  VALIDATOR(ends_with, ith_nsdm<1>)
  std::deque<std::u16string> deque_strings;
};

TEST(Validators, LeafPrefixSuffixWithNSDMRelation4)
{
  auto obj_ok_1 = foo_with_relation_tag_4_t{
    .prefix = "Hello",
    .suffix = "world",
    .deque_chars = std::u16string(u"Hello this fxxking world")
      | std::ranges::to<std::deque>(),
    .deque_strings = {u"Hello", u"this", u"fxxking", u"world"},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));

  auto obj_1 = foo_with_relation_tag_4_t{
    .prefix = "Hello",
    .suffix = "world",
    .deque_chars = std::u16string(u"H w") | std::ranges::to<std::deque>(),
    .deque_strings = {u"hello", u"this", u"fxxking", u"WORLD"},
  };
  auto msg = std::string{};
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_THAT(msg, testing::MatchesRegex(
    "Invalid member 'deque_chars':"
    "\n\\* Input range \\['H', ' ', 'w'\\] does not start with member "
    "'prefix' which is \"Hello\""
    "\n\\* Input range \\['H', ' ', 'w'\\] does not end with member "
    "'suffix' which is \"world\""
    "\nInvalid member 'deque_strings':"
    "\n\\* Input range \\[.*\\] does not start with member "
    "'prefix' which is \"Hello\""
    "\n\\* Input range \\[.*\\] does not end with member "
    "'suffix' which is \"world\""));

  auto obj_2 = foo_with_relation_tag_4_t{
    .prefix = nullptr,
    .suffix = nullptr,
    .deque_chars = {},
    .deque_strings = {},
  };
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'deque_strings':"
    "\n* Input range is empty"
    "\n* Input range is empty", msg);
}
