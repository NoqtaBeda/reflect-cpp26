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

struct qux_t {
  unsigned bf1 : 5;
  const unsigned bf2 : 6;
  volatile unsigned bf3 : 7;
  const volatile unsigned bf4 : 8;

  int& r1;
  const int& r2;
  volatile int& r3;
  const volatile int& r4;
};

TEST(TypeOperationsMemberAccess, WithBitFieldsAndReferences)
{
  auto ints = std::array{21, 42, 63, 84};
  auto qux = qux_t{
    .bf1 = 5, .bf2 = 6, .bf3 = 7, .bf4 = 8,
    .r1 = ints[0], .r2 = ints[1], .r3 = ints[2], .r4 = ints[3],
  };
  // Bit-field access: cv-qualifiers of bit-field members are discarded
  auto&& v1 = rfl::get_ith_direct_nsdm<0>(qux);
  static_assert(std::is_same_v<decltype(v1), unsigned &&>);
  EXPECT_EQ(5, v1);
  auto&& v2 = rfl::get_ith_direct_nsdm<1>(static_cast<qux_t &&>(qux));
  static_assert(std::is_same_v<decltype(v2), unsigned &&>);
  EXPECT_EQ(6, v2);
  auto&& v3 = rfl::get_ith_direct_nsdm<2>(qux);
  static_assert(std::is_same_v<decltype(v3), unsigned &&>);
  EXPECT_EQ(7, v3);
  auto&& v4 = rfl::get_ith_direct_nsdm<3>(static_cast<qux_t &&>(qux));
  static_assert(std::is_same_v<decltype(v4), unsigned &&>);
  EXPECT_EQ(8, v4);

  // Const reference access:
  rfl::get_ith_direct_nsdm<4>(qux) += 100;
  EXPECT_EQ(121, ints[0]);
  rfl::get_ith_direct_nsdm<6>(static_cast<const qux_t &&>(qux)) += 100;
  EXPECT_EQ(163, ints[2]);

  // Non-const reference access
  auto&& v5 = rfl::get_ith_direct_nsdm<5>(static_cast<volatile qux_t &&>(qux));
  static_assert(std::is_same_v<decltype(v5), const int &>);
  EXPECT_EQ(42, v5);
  auto&& v6 = rfl::get_ith_direct_nsdm<7>(qux);
  static_assert(std::is_same_v<decltype(v6), const volatile int &>);
  EXPECT_EQ(84, v6);
}

union union_t {
private:
  const char as_char;
public:
  short as_short;
protected:
  int as_int;
public:
  explicit union_t(long x) : as_long(x) {}

  long as_long;
  int as_bf_1 : 4;
  int as_bf_2 : 12;
};

TEST(TypeOperationsMemberAccess, WithUnion)
{
  auto u = union_t{0x40'41'42'43'44'45'46'47};
  if (std::endian::native == std::endian::little) {
    auto&& v1 = rfl::get_ith_direct_nsdm<0>(u);
    static_assert(std::is_same_v<decltype(v1), const char &>);
    EXPECT_EQ(0x47, v1);

    rfl::get_ith_direct_nsdm<1>(u) += 0x10'10;
    EXPECT_EQ(0x56'57, rfl::get_ith_direct_nsdm<1>(u));

    rfl::get_ith_direct_nsdm<2>(u) += 0x30'20'00'00;
    EXPECT_EQ(0x74'65'56'57, rfl::get_ith_direct_nsdm<2>(u));
    EXPECT_EQ(0x40'41'42'43'74'65'56'57, rfl::get_ith_direct_nsdm<3>(u));

    // For bit-fields: assumes that every bit-field member starts from the
    // least significant bit.
    EXPECT_EQ(0x57 & 0xF, rfl::get_ith_direct_nsdm<4>(u));
    EXPECT_EQ(0x56'57 & 0xFFF, rfl::get_ith_direct_nsdm<5>(u));

    EXPECT_EQ(0x56'57, rfl::get_ith_public_direct_nsdm<0>(u));
    EXPECT_EQ(0x40'41'42'43'74'65'56'57, rfl::get_ith_public_direct_nsdm<1>(u));
    EXPECT_EQ(0x57 & 0xF, rfl::get_ith_public_direct_nsdm<2>(u));
    EXPECT_EQ(0x56'57 & 0xFFF, rfl::get_ith_public_direct_nsdm<3>(u));
  }
}
