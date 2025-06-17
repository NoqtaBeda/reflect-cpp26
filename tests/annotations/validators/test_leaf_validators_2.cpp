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

#include "tests/annotations/validators/validator_test_options.hpp"

/**
 * The following validators are tested:
 * - size_is
 * - is_not_null
 * - is_not_empty
 * - is_sorted, is_sorted_unique, etc.
 */

struct foo_non_empty_t {
  VALIDATOR(is_not_empty)
  VALIDATOR(size_is, 3)
  std::vector<int> v1;
};

TEST(AnnotationValidators, LeafNonEmpty)
{
  LAZY_OBJECT(obj_ok_1, foo_non_empty_t{.v1 = {1, 2, 3}});
  EXPECT_TRUE_STATIC(do_validate_members(obj_ok_1));
  EXPECT_EQ_STATIC("", validation_error_message(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_non_empty_t{});
  EXPECT_FALSE_STATIC(do_validate_members(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects input range to be non-empty.",
    validation_error_message(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects input range to be non-empty."
    "\n* Expects size to be 3, while actual size is 0",
    validation_full_error_message(obj_1));

  LAZY_OBJECT(obj_2, foo_non_empty_t{.v1 = {1, 2, 3, 4}});
  EXPECT_FALSE_STATIC(do_validate_members(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects size to be 3, while actual size is 4",
    validation_error_message(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects size to be 3, while actual size is 4",
    validation_full_error_message(obj_2));
}

struct foo_non_null_t {
  VALIDATOR(is_not_null)
  const char* str;

  VALIDATOR(is_not_null)
  volatile char* buffer;

  VALIDATOR(is_not_null)
  std::shared_ptr<int[]> sptr;

  VALIDATOR(is_not_null)
  std::unique_ptr<int[]> uptr;

  VALIDATOR(is_not_null)
  std::weak_ptr<int[]> wptr;

  VALIDATOR(is_not_null)
  std::any any;

  VALIDATOR(is_not_null)
  std::optional<int> opt;
};

TEST(AnnotationValidators, LeafNonNull)
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
  EXPECT_TRUE(annots::validate_members(obj_ok_1));
  auto msg = std::string{};
  EXPECT_TRUE(annots::validate_members_with_error_info(obj_ok_1, &msg));
  EXPECT_EQ("", msg);

  auto obj_1 = foo_non_null_t{};
  EXPECT_FALSE(annots::validate_members(obj_1));
  msg.clear();
  EXPECT_FALSE(annots::validate_members_with_error_info(obj_1, &msg));
  EXPECT_EQ(
    "Invalid member 'str': Expects pointer to be non-null.", msg);

  msg.clear();
  EXPECT_FALSE(annots::validate_members_with_full_error_info(obj_1, &msg));
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

struct foo_sorted_t {
  VALIDATOR(is_sorted)
  VALIDATOR(is_sorted_unique)
  std::vector<int> v1;

  VALIDATOR(is_ascending) // Equivalent to is_sorted
  VALIDATOR(is_ascending_unique) // Equivalent to is_sorted_unique
  std::vector<std::string> v2;

  VALIDATOR(is_descending)
  VALIDATOR(is_descending_unique)
  std::array<std::vector<std::string>, 3> a3;
};

TEST(AnnotationValidators, LeafIsSorted)
{
  LAZY_OBJECT(obj_ok_1, foo_sorted_t{
    .v1 = {1, 2, 3, 4},
    .v2 = {"abc", "def", "ghi", "jkl"},
    .a3 = {
      std::vector{"ccc"s, "ccc"s},
      std::vector{"bbb"s, "bbb"s, "bbb"s},
      std::vector{"bbb"s, "bbb"s},
    },
  });
  EXPECT_TRUE_STATIC(do_validate_members(obj_ok_1));
  EXPECT_EQ_STATIC("", validation_error_message(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_sorted_t{
    .v1 = {1, 2, 2, 3},
    .v2 = {"abc", "bcd", "bcd", "cde"},
    .a3 = {
      std::vector{"ghi"s, "jkl"s},
      std::vector{"ghi"s, "jkl"s},
      std::vector{"abc"s, "def"s},
    },
  });
  EXPECT_FALSE_STATIC(do_validate_members(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects input range to be sorted in ascending order "
    "and unique, while actual value = [1, 2, 2, 3]",
    validation_error_message(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects input range to be sorted in ascending order and unique, "
    "while actual value = [1, 2, 2, 3]"
    "\nInvalid member 'v2':"
    "\n* Expects input range to be sorted in ascending order and unique, "
    "while actual value = [\"abc\", \"bcd\", \"bcd\", \"cde\"]"
    "\nInvalid member 'a3':"
    "\n* Expects input range to be sorted in descending order and unique, "
    "while actual value = "
    "[[\"ghi\", \"jkl\"], [\"ghi\", \"jkl\"], [\"abc\", \"def\"]]",
    validation_full_error_message(obj_1));

  LAZY_OBJECT(obj_2, foo_sorted_t{
    .v1 = {1, 2, 3, 2},
    .v2 = {"abc", "bcd", "def", "cde"},
    .a3 = {
      std::vector{"ghi"s, "jkl"s},
      std::vector{"ghi"s, "jkl"s, "mno"s},
      std::vector{"abc"s, "def"s},
    },
  });
  EXPECT_FALSE_STATIC(do_validate_members(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects input range to be sorted in "
    "ascending order, while actual value = [1, 2, 3, 2]",
    validation_error_message(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1':"
    "\n* Expects input range to be sorted in ascending order, "
    "while actual value = [1, 2, 3, 2]"
    "\n* Expects input range to be sorted in ascending order and unique, "
    "while actual value = [1, 2, 3, 2]"
    "\nInvalid member 'v2':"
    "\n* Expects input range to be sorted in ascending order, "
    "while actual value = [\"abc\", \"bcd\", \"def\", \"cde\"]"
    "\n* Expects input range to be sorted in ascending order and unique, "
    "while actual value = [\"abc\", \"bcd\", \"def\", \"cde\"]"
    "\nInvalid member 'a3':"
    "\n* Expects input range to be sorted in descending order, "
    "while actual value = "
    "[[\"ghi\", \"jkl\"], [\"ghi\", \"jkl\", \"mno\"], [\"abc\", \"def\"]]"
    "\n* Expects input range to be sorted in descending order and unique, "
    "while actual value = "
    "[[\"ghi\", \"jkl\"], [\"ghi\", \"jkl\", \"mno\"], [\"abc\", \"def\"]]",
    validation_full_error_message(obj_2));
}
