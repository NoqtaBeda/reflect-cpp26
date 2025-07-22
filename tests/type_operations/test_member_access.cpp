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
#include <reflect_cpp26/utils/to_string.hpp>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_operations.hpp>
#else
#include <reflect_cpp26/type_operations/member_access.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_t {
private:
  int a;
public:
  int x;
  int y;
  int z;

  explicit constexpr foo_t(int base)
    : a(base), x(base + 1), y(base + 2), z(base + 3), b(base + 4) {}

protected:
  int b;
};

TEST(TypeOperationsMemberAccess, NoInheritance)
{
  constexpr auto foo = foo_t{10};
  EXPECT_EQ_STATIC(10, rfl::get_ith_direct_nsdm<0>(foo));
  EXPECT_EQ_STATIC(11, rfl::get_ith_direct_nsdm<1>(foo));
  EXPECT_EQ_STATIC(12, rfl::get_ith_direct_nsdm<2>(foo));
  EXPECT_EQ_STATIC(13, rfl::get_ith_direct_nsdm<3>(foo));
  EXPECT_EQ_STATIC(14, rfl::get_ith_direct_nsdm<4>(foo));

  EXPECT_EQ_STATIC(11, rfl::get_ith_public_direct_nsdm<0>(foo));
  EXPECT_EQ_STATIC(12, rfl::get_ith_public_direct_nsdm<1>(foo));
  EXPECT_EQ_STATIC(13, rfl::get_ith_public_direct_nsdm<2>(foo));

  EXPECT_EQ_STATIC(10, rfl::get_first_direct_nsdm(foo));
  EXPECT_EQ_STATIC(14, rfl::get_last_direct_nsdm(foo));

  EXPECT_EQ_STATIC(11, rfl::get_first_public_direct_nsdm(foo));
  EXPECT_EQ_STATIC(13, rfl::get_last_public_direct_nsdm(foo));
}

struct bar_t : public foo_t {
private:
  std::string s1;
public:
  std::string s2;
  std::string s3;

  explicit bar_t(int base)
    : foo_t(base), s1(rfl::to_string(base + 10)), s2(rfl::to_string(base + 11))
    , s3(rfl::to_string(base + 12)), s4(rfl::to_string(base + 13)) {}

private:
  std::string s4;
};

TEST(TypeOperationsMemberAccess, WithInheritance1)
{
  auto bar = bar_t{10};
  EXPECT_EQ("20", rfl::get_ith_direct_nsdm<0>(bar));
  EXPECT_EQ("21", rfl::get_ith_direct_nsdm<1>(bar));
  EXPECT_EQ("22", rfl::get_ith_direct_nsdm<2>(bar));
  EXPECT_EQ("23", rfl::get_ith_direct_nsdm<3>(bar));

  EXPECT_EQ("21", rfl::get_ith_public_direct_nsdm<0>(bar));
  EXPECT_EQ("22", rfl::get_ith_public_direct_nsdm<1>(bar));

  EXPECT_EQ("20", rfl::get_first_direct_nsdm(bar));
  EXPECT_EQ("21", rfl::get_first_public_direct_nsdm(bar));

  EXPECT_EQ("23", rfl::get_last_direct_nsdm(bar));
  EXPECT_EQ("22", rfl::get_last_public_direct_nsdm(bar));

  EXPECT_EQ(10, rfl::get_ith_flattened_nsdm<0>(bar));
  EXPECT_EQ(11, rfl::get_ith_flattened_nsdm<1>(bar));
  EXPECT_EQ(12, rfl::get_ith_flattened_nsdm<2>(bar));
  EXPECT_EQ(13, rfl::get_ith_flattened_nsdm<3>(bar));
  EXPECT_EQ(14, rfl::get_ith_flattened_nsdm<4>(bar));
  EXPECT_EQ("20", rfl::get_ith_flattened_nsdm<5>(bar));
  EXPECT_EQ("21", rfl::get_ith_flattened_nsdm<6>(bar));
  EXPECT_EQ("22", rfl::get_ith_flattened_nsdm<7>(bar));
  EXPECT_EQ("23", rfl::get_ith_flattened_nsdm<8>(bar));

  EXPECT_EQ(11, rfl::get_ith_public_flattened_nsdm<0>(bar));
  EXPECT_EQ(12, rfl::get_ith_public_flattened_nsdm<1>(bar));
  EXPECT_EQ(13, rfl::get_ith_public_flattened_nsdm<2>(bar));
  EXPECT_EQ("21", rfl::get_ith_public_flattened_nsdm<3>(bar));
  EXPECT_EQ("22", rfl::get_ith_public_flattened_nsdm<4>(bar));

  EXPECT_EQ(10, rfl::get_first_flattened_nsdm(bar));
  EXPECT_EQ(11, rfl::get_first_public_flattened_nsdm(bar));

  EXPECT_EQ("23", rfl::get_last_flattened_nsdm(bar));
  EXPECT_EQ("22", rfl::get_last_public_flattened_nsdm(bar));
}

struct baz_t : protected std::pair<double, double>, public bar_t {
  double alpha;
  double beta;

  explicit baz_t(int base)
    : std::pair<double, double>{1.0 * base, 1.0 / base}, bar_t{base + 10}
    , alpha(base + 30.25), beta(base + 31.5), gamma(base + 32.75) {}

protected:
  double gamma;
};

TEST(TypeOperationsMemberAccess, WithInheritance2)
{
  auto baz = baz_t{4};
  EXPECT_EQ(34.25, rfl::get_ith_direct_nsdm<0>(baz));
  EXPECT_EQ(35.50, rfl::get_ith_direct_nsdm<1>(baz));
  EXPECT_EQ(36.75, rfl::get_ith_direct_nsdm<2>(baz));

  EXPECT_EQ(34.25, rfl::get_ith_public_direct_nsdm<0>(baz));
  EXPECT_EQ(35.50, rfl::get_ith_public_direct_nsdm<1>(baz));

  EXPECT_EQ(34.25, rfl::get_first_direct_nsdm(baz));
  EXPECT_EQ(34.25, rfl::get_first_public_direct_nsdm(baz));

  EXPECT_EQ(36.75, rfl::get_last_direct_nsdm(baz));
  EXPECT_EQ(35.50, rfl::get_last_public_direct_nsdm(baz));

  EXPECT_EQ(4.0, rfl::get_ith_flattened_nsdm<0>(baz));
  EXPECT_EQ(0.25, rfl::get_ith_flattened_nsdm<1>(baz));
  EXPECT_EQ(14, rfl::get_ith_flattened_nsdm<2>(baz));
  EXPECT_EQ(15, rfl::get_ith_flattened_nsdm<3>(baz));
  EXPECT_EQ(16, rfl::get_ith_flattened_nsdm<4>(baz));
  EXPECT_EQ(17, rfl::get_ith_flattened_nsdm<5>(baz));
  EXPECT_EQ(18, rfl::get_ith_flattened_nsdm<6>(baz));
  EXPECT_EQ("24", rfl::get_ith_flattened_nsdm<7>(baz));
  EXPECT_EQ("25", rfl::get_ith_flattened_nsdm<8>(baz));
  EXPECT_EQ("26", rfl::get_ith_flattened_nsdm<9>(baz));
  EXPECT_EQ("27", rfl::get_ith_flattened_nsdm<10>(baz));
  EXPECT_EQ(34.25, rfl::get_ith_flattened_nsdm<11>(baz));
  EXPECT_EQ(35.50, rfl::get_ith_flattened_nsdm<12>(baz));
  EXPECT_EQ(36.75, rfl::get_ith_flattened_nsdm<13>(baz));

  EXPECT_EQ(15, rfl::get_ith_public_flattened_nsdm<0>(baz));
  EXPECT_EQ(16, rfl::get_ith_public_flattened_nsdm<1>(baz));
  EXPECT_EQ(17, rfl::get_ith_public_flattened_nsdm<2>(baz));
  EXPECT_EQ("25", rfl::get_ith_public_flattened_nsdm<3>(baz));
  EXPECT_EQ("26", rfl::get_ith_public_flattened_nsdm<4>(baz));
  EXPECT_EQ(34.25, rfl::get_ith_public_flattened_nsdm<5>(baz));
  EXPECT_EQ(35.50, rfl::get_ith_public_flattened_nsdm<6>(baz));

  EXPECT_EQ(4.0, rfl::get_first_flattened_nsdm(baz));
  EXPECT_EQ(15, rfl::get_first_public_flattened_nsdm(baz));

  EXPECT_EQ(36.75, rfl::get_last_flattened_nsdm(baz));
  EXPECT_EQ(35.50, rfl::get_last_public_flattened_nsdm(baz));
}
