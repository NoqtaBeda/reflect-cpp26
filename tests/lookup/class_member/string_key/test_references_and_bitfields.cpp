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

#include <reflect_cpp26/lookup/lookup_table.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

struct with_ref_and_bf_t {
  const unsigned value1;
  const unsigned value2;

  // References are filtered out since they are not addressable.
  const unsigned& ref1;
  const unsigned& ref2;

  // Bit-fields are filtered out since they are not addressable.
  const unsigned bf1 : 5;
  const unsigned bf2 : 10;
  const unsigned bf3 : 15;
};

TEST(ClassLookupTableByName, WithReferencesAndBitFields) {
  constexpr auto table = LOOKUP_TABLE(with_ref_and_bf_t, "*");
  static_assert(std::is_same_v<const unsigned with_ref_and_bf_t::*, decltype(table)::value_type>);
  static_assert(table.size() == 2);

  static constexpr auto v1 = 12u;
  static constexpr auto v2 = 24u;
  constexpr auto foo = with_ref_and_bf_t{
      .value1 = 2, .value2 = 4, .ref1 = v1, .ref2 = v2, .bf1 = 5, .bf2 = 7, .bf3 = 11};

  CHECK_MEMBER_VARIABLE_STATIC(2, foo, table["value1"]);
  CHECK_MEMBER_VARIABLE_STATIC(4, foo, table["value2"]);
  EXPECT_EQ_STATIC(nullptr, table["ref1"]);
  EXPECT_EQ_STATIC(nullptr, table["ref2"]);
  EXPECT_EQ_STATIC(nullptr, table["bf1"]);
  EXPECT_EQ_STATIC(nullptr, table["bf2"]);
  EXPECT_EQ_STATIC(nullptr, table["bf3"]);
}
