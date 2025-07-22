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
#include <reflect_cpp26/validators/leaf/sorted_test.hpp>
#endif

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

TEST(Validators, LeafIsSorted)
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
  EXPECT_TRUE_STATIC(validate_public_nsdm(obj_ok_1));
  EXPECT_EQ_STATIC("", validate_public_nsdm_msg(obj_ok_1));

  LAZY_OBJECT(obj_1, foo_sorted_t{
    .v1 = {1, 2, 2, 3},
    .v2 = {"abc", "bcd", "bcd", "cde"},
    .a3 = {
      std::vector{"ghi"s, "jkl"s},
      std::vector{"ghi"s, "jkl"s},
      std::vector{"abc"s, "def"s},
    },
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_1));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects input range to be sorted in ascending order "
    "and unique, while actual value = [1, 2, 2, 3]",
    validate_public_nsdm_msg(obj_1));
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
    validate_public_nsdm_msg_verbose(obj_1));

  LAZY_OBJECT(obj_2, foo_sorted_t{
    .v1 = {1, 2, 3, 2},
    .v2 = {"abc", "bcd", "def", "cde"},
    .a3 = {
      std::vector{"ghi"s, "jkl"s},
      std::vector{"ghi"s, "jkl"s, "mno"s},
      std::vector{"abc"s, "def"s},
    },
  });
  EXPECT_FALSE_STATIC(validate_public_nsdm(obj_2));
  EXPECT_EQ_STATIC(
    "Invalid member 'v1': Expects input range to be sorted in "
    "ascending order, while actual value = [1, 2, 3, 2]",
    validate_public_nsdm_msg(obj_2));
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
    validate_public_nsdm_msg_verbose(obj_2));
}
