/**
 * Copyright (c) 2026 NoqtaBeda (noqtabeda@163.com)
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

#include <reflect_cpp26/utils/meta_tuple.hpp>

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#endif

namespace rfl = reflect_cpp26;

// ---- tuple_like ----

using rfl_meta_tuple = rfl::meta_tuple<char, char*, char**>;

// Tuple-like types (with cvref)
static_assert(rfl::tuple_like<volatile rfl_meta_tuple>);
static_assert(rfl::tuple_like<const volatile rfl_meta_tuple>);
static_assert(NOT rfl::tuple_like<volatile rfl_meta_tuple&>);
static_assert(NOT rfl::tuple_like<const volatile rfl_meta_tuple&&>);

// Nested tuple-like types
using nested_std_tuple =
    std::tuple<std::pair<float, std::pair<int, unsigned>>,
               std::tuple<char, std::tuple<short, int, std::tuple<float, double>>>,
               rfl::meta_tuple<char8_t, char16_t, char32_t>>;
static_assert(rfl::tuple_like<nested_std_tuple>);

using nested_meta_tuple =
    rfl::meta_tuple<rfl::meta_tuple<char, short, int, long>,
                    rfl::meta_tuple<float, double, rfl::meta_tuple<int, float, double>>>;
static_assert(rfl::tuple_like<const nested_meta_tuple>);

// ---- is_tuple_like_of_exactly_v ----

// With cvref
static_assert(rfl::tuple_like_of_exactly<const volatile rfl_meta_tuple, char, char*, char**>);
// Expects exact match
static_assert(NOT rfl::tuple_like_of_exactly<rfl_meta_tuple, char, char const*, char* const*>);

// ---- is_tuple_like_of_v ----

// Implicit conversion is OK
static_assert(rfl::tuple_like_of<const volatile rfl_meta_tuple, char, char const*, char* const*>);

// ---- are_tuple_like_of_same_size_v ----

static_assert(rfl::are_tuple_like_of_same_size_v<std::tuple<int, int*, int**>,
                                                 const rfl::meta_tuple<float, double, long>>);

static_assert(NOT rfl::are_tuple_like_of_same_size_v<std::tuple<int, int*, int**>,
                                                     const rfl::meta_tuple<float, double>>);

TEST(TypeTraits, TupleLikeTypes2) {
  EXPECT_TRUE(true);  // All test cases done with static-asserts above
}
