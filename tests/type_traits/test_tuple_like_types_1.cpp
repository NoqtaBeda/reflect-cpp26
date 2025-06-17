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

#include "tests/test_options.hpp"
#include <utility>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#endif

namespace rfl = reflect_cpp26;

// ---- is_tuple_like_v ----

// Non-tuple types
static_assert(NOT rfl::is_tuple_like_v<void>);
static_assert(NOT rfl::is_tuple_like_v<int>);
static_assert(NOT rfl::is_tuple_like_v<std::vector<int>>);
static_assert(NOT rfl::is_tuple_like_v<std::string>);

using std_tuple = std::tuple<int, int&, int**>;
using std_pair = std::pair<std::string&&, size_t&>;

// Tuple-like types (with cvref)
static_assert(rfl::is_tuple_like_v<std_tuple>);
static_assert(rfl::is_tuple_like_v<const std_pair>);

static_assert(NOT rfl::is_pair_like_v<std_tuple>);
static_assert(rfl::is_pair_like_v<volatile std_pair&&>);

// ---- is_tuple_like_of_exactly_v ----

// With cvref
static_assert(rfl::is_tuple_like_of_exactly_v<
  std_tuple, int, int&, int**>);
static_assert(rfl::is_tuple_like_of_exactly_v<
  const std_pair, std::string&&, size_t&>);
// Expects exact match
static_assert(NOT rfl::is_tuple_like_of_exactly_v<
  std_pair, std::string, size_t>);

// ---- is_tuple_like_of_v ----

// Implicit conversion is OK
static_assert(rfl::is_tuple_like_of_v<
  std_tuple, int, int&, int**>);
static_assert(rfl::is_tuple_like_of_v<
  const std_pair, std::string, size_t>);
static_assert(rfl::is_tuple_like_of_v<
  std_pair, std::string_view, unsigned>);
// std::string -> std::string_view is OK, but inverse is not.
static_assert(rfl::is_tuple_like_of_v<
  std::tuple<std::string>, std::string_view>);
static_assert(NOT rfl::is_tuple_like_of_v<
  std::tuple<std::string_view>, std::string>);

TEST(TypeTraits, TupleLikeTypes1) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
