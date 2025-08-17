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
#include <reflect_cpp26/validators/leaf/contains_test.hpp>
#endif

using namespace std::literals;

struct foo_integral_single_t {
  VALIDATOR(contains, 0)
  std::deque<int> deque; // Random-access

  VALIDATOR(contains, 1)
  std::list<int> list; // Bidirectional

  VALIDATOR(contains, 2)
  std::forward_list<int> forward_list; // Unidirectional
};

TEST(Validators, LeafContainsIntegralSingle)
{
  auto obj_ok_1 = foo_integral_single_t{
    .deque = {1, 0, -1},
    .list = {1, 2, 3},
    .forward_list = {1, 2, 3},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  auto obj_1 = foo_integral_single_t{
    .deque = {1, 2, 3},
    .list = {0},
    .forward_list = {-1, 0, 1},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_1));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range [1, 2, 3] does not contain 0"
    "\nInvalid member 'list':"
    "\n* Input range [0] does not contain 1"
    "\nInvalid member 'forward_list':"
    "\n* Input range [-1, 0, 1] does not contain 2", msg);

  auto obj_2 = foo_integral_single_t{}; // All empty
  EXPECT_FALSE(rfl::validate_public_members(obj_2));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range is empty"
    "\nInvalid member 'list':"
    "\n* Input range is empty"
    "\nInvalid member 'forward_list':"
    "\n* Input range is empty", msg);
}

struct foo_integral_range_t {
  VALIDATOR(contains, std::vector{1, 2, 3})
  std::deque<int> deque; // Random-access

  VALIDATOR(contains, {4, 5, 6})
  std::list<int> list; // Bidirectional

  VALIDATOR(contains, std::array{1, 1})
  std::forward_list<int> forward_list; // Unidirectional

  VALIDATOR(contains, std::vector<int>{})
  std::deque<std::deque<int>> nested_deque;

  VALIDATOR(contains, {4, 5, 6})
  std::list<std::list<int>> nested_list;

  VALIDATOR(contains, std::array{1, 1})
  std::forward_list<std::forward_list<int>> nested_forward_list;

  VALIDATOR(contains, std::vector<int>{})
  std::vector<int> always_passes_validation;
};

TEST(Validators, LeafContainsIntegralRange)
{
  auto obj_ok_1 = foo_integral_range_t{
    .deque = {1, 2, 3},
    .list = {1, 2, 3, 4, 5, 6},
    .forward_list = {0, 0, 1, 1, 2, 2},
    .nested_deque = {{}},
    .nested_list = {{}, {1, 2, 3}, {4, 5, 6}},
    .nested_forward_list = {{0, 0}, {1, 1}, {2, 2}},
    .always_passes_validation = {0},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  auto obj_1 = foo_integral_range_t{
    .deque = {1, -1, 2, -2, 3},
    .list = {4, 5, 5, 6},
    .forward_list = {0, 1, 2},
    .nested_deque = {{0}, {1}, {2}},
    .nested_list = {{4}, {5}, {6}, {4, 5}, {6}, {4}, {5, 6}},
    .nested_forward_list = {{1}, {1}, {1, 0, 1}, {1}},
    .always_passes_validation = {-1, 0, 1},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_1));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range [1, -1, 2, -2, 3] does not contain [1, 2, 3]"
    "\nInvalid member 'list':"
    "\n* Input range [4, 5, 5, 6] does not contain [4, 5, 6]"
    "\nInvalid member 'forward_list':"
    "\n* Input range [0, 1, 2] does not contain [1, 1]"
    "\nInvalid member 'nested_deque':"
    "\n* Input range [[0], [1], [2]] does not contain []"
    "\nInvalid member 'nested_list':"
    "\n* Input range [[4], [5], [6], [4, 5], [6], [4], [5, 6]] "
    "does not contain [4, 5, 6]"
    "\nInvalid member 'nested_forward_list':"
    "\n* Input range [[1], [1], [1, 0, 1], [1]] does not contain [1, 1]", msg);

  auto obj_2 = foo_integral_range_t{}; // All empty
  EXPECT_FALSE(rfl::validate_public_members(obj_2));
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'deque':"
    "\n* Input range is empty"
    "\nInvalid member 'list':"
    "\n* Input range is empty"
    "\nInvalid member 'forward_list':"
    "\n* Input range is empty"
    "\nInvalid member 'nested_deque':"
    "\n* Input range is empty"
    "\nInvalid member 'nested_list':"
    "\n* Input range is empty"
    "\nInvalid member 'nested_forward_list':"
    "\n* Input range is empty", msg);
}

struct foo_string_1_t {
  VALIDATOR(contains, '!')
  std::string s1;

  VALIDATOR(contains, "hello")
  std::string s2;

  VALIDATOR(contains, "cat")
  std::vector<std::string> slist;
};

TEST(Validators, LeafContainsString1)
{
  LAZY_OBJECT(obj_ok_1, foo_string_1_t{
    .s1 = "hello!",
    .s2 = "Say 'hello'!",
    .slist = {"dog", "cat", "fox"},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_string_1_t{
    .s1 = "hello",
    .s2 = "Say 'Hello'!",
    .slist = {"Dog", "Cat", "Fox"},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 's1':"
    "\n* Input string \"hello\" does not contain '!'"
    "\nInvalid member 's2':"
    "\n* Input string \"Say 'Hello'!\" does not contain \"hello\""
    "\nInvalid member 'slist':"
    "\n* Input range [\"Dog\", \"Cat\", \"Fox\"] does not contain \"cat\"",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_string_1_t{
    .s1 = "!",
    .s2 = "hell no!",
    .slist = {"", "", ""},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 's2':"
    "\n* Input string \"hell no!\" does not contain \"hello\""
    "\nInvalid member 'slist':"
    "\n* Input range [\"\", \"\", \"\"] does not contain \"cat\"",
    validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_string_1_t{}); // All empty
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 's1':"
    "\n* Input string is empty"
    "\nInvalid member 's2':"
    "\n* Input string is empty"
    "\nInvalid member 'slist':"
    "\n* Input range is empty",
    validate_public_nsdm_msg_verbose(obj_3));
}

struct foo_string_2_t {
  VALIDATOR(contains, "")
  const char* always_passes_validation;

  VALIDATOR(contains, '!')
  const char* may_fail_validation_1;

  VALIDATOR(contains, "")
  std::forward_list<std::string_view> may_fail_validation_2;

  VALIDATOR(contains, "end"s)
  std::vector<char> buffer;
};

TEST(Validators, LeafContainsString2)
{
  auto obj_ok_1 = foo_string_2_t{
    .always_passes_validation = "hello!",
    .may_fail_validation_1 = "hello!",
    .may_fail_validation_2 = {"hello", "world", ""},
    .buffer = {std::from_range, "begin do_something; end;"sv},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));

  auto obj_1 = foo_string_2_t{
    .always_passes_validation = "",
    .may_fail_validation_1 = "hello",
    .may_fail_validation_2 = {"hello", "world"},
    .buffer = {std::from_range, "begin do_something; END;"sv},
  };
  EXPECT_FALSE(rfl::validate_public_members(obj_1));
  auto msg = std::string{};
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_THAT(msg, testing::MatchesRegex(
    "Invalid member 'may_fail_validation_1':"
    "\n\\* Input string \"hello\" does not contain '!'"
    "\nInvalid member 'may_fail_validation_2':"
    "\n\\* Input range \\[\"hello\", \"world\"\\] does not contain \"\""
    "\nInvalid member 'buffer':"
    "\n\\* Input (string|range) \\['b', 'e', 'g', 'i', 'n', ' ', 'd', 'o', "
    "'_', 's', 'o', 'm', 'e', 't', 'h', 'i', 'n', 'g', ';', ' ', 'E', 'N', "
    "'D', ';'\\] does not contain \"end\""));

  auto obj_2 = foo_string_2_t{
    .always_passes_validation = nullptr,
    .may_fail_validation_1 = nullptr,
    .may_fail_validation_2 = {},
    .buffer = {},
  };
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'may_fail_validation_1':"
    "\n* Input string is empty"
    "\nInvalid member 'may_fail_validation_2':"
    "\n* Input range is empty"
    "\nInvalid member 'buffer':"
    "\n* Input string is empty", msg);
}

struct foo_with_relation_tag_1_t {
  int s1;

  VALIDATOR(contains, prev_nsdm)
  std::vector<unsigned> v1;

  VALIDATOR(contains, prev_nsdm)
  std::vector<int> v2;
};

TEST(Validators, LeafContainsWithNSDMRelation1)
{
  LAZY_OBJECT(obj_ok_1, foo_with_relation_tag_1_t{
    .s1 = 42,
    .v1 = {6, 7, 42},
    .v2 = {2, 3, 6, 7, 42, 43},
  });
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_with_relation_tag_1_t{
    .s1 = -1,
    .v1 = {1, -1u},
    .v2 = {1, -1, 2, -2},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Input range [1, 4294967295] does not contain member 's1' which is -1"
    "\nInvalid member 'v2':"
    "\n* Input range [1, -1, 2, -2] does not contain member 'v1' "
    "which is [1, 4294967295]",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_with_relation_tag_1_t{
    .s1 = 42,
    .v1 = {},
    .v2 = {},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':\n* Input range is empty",
    validate_public_nsdm_msg_verbose(obj_2));

  LAZY_OBJECT(obj_3, foo_with_relation_tag_1_t{
    .s1 = 42,
    .v1 = {4, 2},
    .v2 = {},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_3));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Input range [4, 2] does not contain member 's1' which is 42"
    "\nInvalid member 'v2':"
    "\n* Input range is empty",
    validate_public_nsdm_msg_verbose(obj_3));

  LAZY_OBJECT(obj_4, foo_with_relation_tag_1_t{
    .s1 = 42,
    .v1 = {},
    .v2 = {4, 2},
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_4));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':\n* Input range is empty",
    validate_public_nsdm_msg_verbose(obj_4));
}

struct foo_with_relation_tag_2_t {
  unsigned bf1: 4;
  unsigned bf2: 8;

  VALIDATOR(contains, prev_ith_nsdm<2>) // bf1
  VALIDATOR(contains, prev_ith_nsdm<1>) // bf2
  std::vector<int> values;

  VALIDATOR(contains, next_ith_nsdm<1>) // cs1
  VALIDATOR(contains, next_ith_nsdm<2>) // cs2
  std::deque<std::u8string>& vs;

  const char* & cs1;
  VALIDATOR(contains, prev_nsdm)
  const wchar_t* & cs2;

  VALIDATOR(contains, prev_ith_nsdm<2>) // cs1
  std::deque<char16_t> dc;
};

TEST(Validators, LeafContainsWithNSDMRelation2)
{
  auto vs1 = std::deque<std::u8string>{
    u8"pin", u8"pineapple", u8"app", u8"apple"};
  const auto* cs1 = "app";
  const auto* cs2 = L"pineapple";

  auto obj_ok_1 = foo_with_relation_tag_2_t{
    .bf1 = 3,
    .bf2 = 12,
    .values = {3, 6, 9, 12},
    .vs = vs1,
    .cs1 = cs1,
    .cs2 = cs2,
    .dc = {u'm', u'y', u' ', u'a', u'p', u'p', u'l', u'e'},
  };
  EXPECT_TRUE(rfl::validate_public_members(obj_ok_1));

  const auto* cs3 = "Apple";
  const auto* cs4 = L"Pineapple";
  auto obj_1 = foo_with_relation_tag_2_t{
    .bf1 = 3,
    .bf2 = 12,
    .values = {19, 28},
    .vs = vs1,
    .cs1 = cs3,
    .cs2 = cs4,
    .dc = {u'm', u'y', u' ', u'a', u'p', u'p', u'l', u'e'},
  };
  auto msg = std::string{};
  EXPECT_FALSE(rfl::validate_public_members(obj_1, &msg));
  EXPECT_EQ("Invalid member 'values': Input range [19, 28] does not contain "
            "member 'bf1' which is 3", msg);
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_1, &msg));
  EXPECT_THAT(msg, testing::MatchesRegex(
    "Invalid member 'values':"
    "\n\\* Input range \\[19, 28\\] does not contain member 'bf1' which is 3"
    "\n\\* Input range \\[19, 28\\] does not contain member 'bf2' which is 12"
    "\nInvalid member 'vs':"
    "\n\\* Input range \\[.*\\] does not contain member 'cs1' "
    "which is \"Apple\""
    "\n\\* Input range \\[.*\\] does not contain member 'cs2'.*"
    "\nInvalid member 'cs2':"
    "\n\\* Input string .*does not contain member 'cs1' which is \"Apple\""
    "\nInvalid member 'dc':"
    "\n\\* Input range \\['m', 'y', ' ', 'a', 'p', 'p', 'l', 'e'\\] does not "
    "contain member 'cs1' which is \"Apple\""));

  auto vs2 = std::deque<std::u8string>{};
  auto obj_2 = foo_with_relation_tag_2_t{
    .bf1 = 3,
    .bf2 = 12,
    .values = {},
    .vs = vs2,
    .cs1 = cs1,
    .cs2 = cs2,
    .dc = {},
  };
  msg.clear();
  EXPECT_FALSE(rfl::validate_public_members_verbose(obj_2, &msg));
  EXPECT_EQ(
    "Invalid member 'values':"
    "\n* Input range is empty"
    "\n* Input range is empty"
    "\nInvalid member 'vs':"
    "\n* Input range is empty"
    "\n* Input range is empty"
    "\nInvalid member 'dc':"
    "\n* Input range is empty", msg);
}

struct foo_with_relation_tag_3_t {
  const char* s1;

  VALIDATOR(contains, prev_nsdm)
  const char* s2;
};

TEST(Validators, LeafContainsWithNSDMRelation3)
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
    "\n* Input string \"hell no\" does not contain "
    "member 's1' which is \"hello\"",
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_with_relation_tag_3_t{
    .s1 = "hello",
    .s2 = nullptr,
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 's2':\n* Input string is empty",
    validate_public_nsdm_msg_verbose(obj_2));
}
