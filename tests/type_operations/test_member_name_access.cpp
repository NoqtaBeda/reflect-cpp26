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

#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_operations.hpp>
#else
#include <reflect_cpp26/type_operations/member_name_access.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_t {
private:
  int a_count;

public:
  int x1;
  int y2;
  int z3;

protected:
  int b_size;
};

TEST(TypeOperationsMemberNameAccess, NoInheritance) {
  EXPECT_EQ_STATIC("a_count", rfl::direct_nonstatic_data_member_names_v<foo_t>[0]);
  EXPECT_EQ_STATIC("b_size", rfl::direct_nonstatic_data_member_names_v<foo_t>[4]);

  EXPECT_EQ_STATIC("x1", rfl::public_direct_nonstatic_data_member_names_v<foo_t>[0]);
  EXPECT_EQ_STATIC("y2", rfl::public_direct_nonstatic_data_member_names_v<foo_t>[1]);
  EXPECT_EQ_STATIC("z3", rfl::public_direct_nonstatic_data_member_names_v<foo_t>[2]);

  EXPECT_EQ_STATIC("a_count", rfl::nonstatic_data_member_names_v<foo_t>[0]);
  EXPECT_EQ_STATIC("b_size", rfl::nonstatic_data_member_names_v<foo_t>[4]);

  EXPECT_EQ_STATIC("x1", rfl::public_nonstatic_data_member_names_v<foo_t>[0]);
  EXPECT_EQ_STATIC("y2", rfl::public_nonstatic_data_member_names_v<foo_t>[1]);
  EXPECT_EQ_STATIC("z3", rfl::public_nonstatic_data_member_names_v<foo_t>[2]);
}

struct bar_t : public foo_t {
private:
  std::string s1;

public:
  std::string s2;
  std::string s3;

private:
  std::string s4;
};

struct baz_t : protected std::pair<double, double>, public bar_t {
  double alpha;
  double beta;
  union {
    int as_int;
    float as_float;
  };  // Anonymous data member
protected:
  double gamma;
};

TEST(TypeOperationsMemberNameAccess, WithInheritance) {
  EXPECT_EQ_STATIC("alpha", rfl::direct_nonstatic_data_member_names_v<baz_t>[0]);
  EXPECT_EQ_STATIC("beta", rfl::direct_nonstatic_data_member_names_v<baz_t>[1]);
  EXPECT_EQ_STATIC("", rfl::direct_nonstatic_data_member_names_v<baz_t>[2]);
  EXPECT_EQ_STATIC("gamma", rfl::direct_nonstatic_data_member_names_v<baz_t>[3]);
  EXPECT_EQ_STATIC(4, rfl::direct_nonstatic_data_member_names_v<baz_t>.size());

  EXPECT_EQ_STATIC("alpha", rfl::public_direct_nonstatic_data_member_names_v<baz_t>[0]);
  EXPECT_EQ_STATIC("beta", rfl::public_direct_nonstatic_data_member_names_v<baz_t>[1]);
  EXPECT_EQ_STATIC("", rfl::public_direct_nonstatic_data_member_names_v<baz_t>[2]);
  EXPECT_EQ_STATIC(3, rfl::public_direct_nonstatic_data_member_names_v<baz_t>.size());

  // Data members 'first' and 'second' are from std::pair
  EXPECT_EQ_STATIC("first", rfl::nonstatic_data_member_names_v<baz_t>[0]);
  EXPECT_EQ_STATIC("second", rfl::nonstatic_data_member_names_v<baz_t>[1]);
  EXPECT_EQ_STATIC("a_count", rfl::nonstatic_data_member_names_v<baz_t>[2]);
  EXPECT_EQ_STATIC("b_size", rfl::nonstatic_data_member_names_v<baz_t>[6]);
  EXPECT_EQ_STATIC("s1", rfl::nonstatic_data_member_names_v<baz_t>[7]);
  EXPECT_EQ_STATIC("s4", rfl::nonstatic_data_member_names_v<baz_t>[10]);
  EXPECT_EQ_STATIC("alpha", rfl::nonstatic_data_member_names_v<baz_t>[11]);
  EXPECT_EQ_STATIC("beta", rfl::nonstatic_data_member_names_v<baz_t>[12]);
  EXPECT_EQ_STATIC("", rfl::nonstatic_data_member_names_v<baz_t>[13]);
  EXPECT_EQ_STATIC("gamma", rfl::nonstatic_data_member_names_v<baz_t>[14]);
  EXPECT_EQ_STATIC(15, rfl::nonstatic_data_member_names_v<baz_t>.size());

  EXPECT_EQ_STATIC("x1", rfl::public_nonstatic_data_member_names_v<baz_t>[0]);
  EXPECT_EQ_STATIC("y2", rfl::public_nonstatic_data_member_names_v<baz_t>[1]);
  EXPECT_EQ_STATIC("z3", rfl::public_nonstatic_data_member_names_v<baz_t>[2]);
  EXPECT_EQ_STATIC("s2", rfl::public_nonstatic_data_member_names_v<baz_t>[3]);
  EXPECT_EQ_STATIC("s3", rfl::public_nonstatic_data_member_names_v<baz_t>[4]);
  EXPECT_EQ_STATIC("alpha", rfl::public_nonstatic_data_member_names_v<baz_t>[5]);
  EXPECT_EQ_STATIC("beta", rfl::public_nonstatic_data_member_names_v<baz_t>[6]);
  EXPECT_EQ_STATIC("", rfl::public_nonstatic_data_member_names_v<baz_t>[7]);
  EXPECT_EQ_STATIC(8, rfl::public_nonstatic_data_member_names_v<baz_t>.size());
}
