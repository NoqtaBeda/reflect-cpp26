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

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_t {
  int32_t x;
  int32_t y;

  void set_xy(int x, int y);
private:
  double _timestamp;
};

struct bar_1_t : foo_t {
  float average_rating;
  int32_t rating_count;

  auto dump() const -> std::string {
    return std::format("average_rating = {}, rating_count = {}",
      average_rating, rating_count);
  }
protected:
  int32_t tag;
  void call_bar();
};

struct bar_2_t : std::pair<int32_t, int32_t>, protected bar_1_t {
  float first; // Shadows member 'first' of base
protected:
  char d2;
};

struct bar_3_t
  : private std::array<int16_t, 6>
  , std::pair<int32_t, int32_t>
{
private:
  char d3;
public:
  float first; // Shadows member 'first' of base

  bar_3_t(): std::array<int16_t, 6>{}, std::pair<int32_t, int32_t>{} {}

  auto dump_private_base() -> std::string {
    return std::format("{}", *static_cast<std::array<int16_t, 6>*>(this));
  }
protected:
  char d4;
};

class baz_1_t : public bar_1_t, public bar_3_t {
private:
  std::array<char, 3> f;
public:
  std::array<int32_t, 4> e;
  int32_t e_sum;

  baz_1_t(): f({'a', 'b', 'c'}) {}

  auto dump() const -> std::string {
    auto res = bar_1_t::dump();
    res += std::format(", e = {}, e_sum = {}, f = {}", e, e_sum, f);
    return res;
  }
};

template <size_t I, auto... Values>
constexpr auto get_offset(rfl::constant<Values...> members) {
  return get<I>(members).actual_offset.bytes;
}

template <class T>
void force_write_by_offset(void* base, uintptr_t offset, T value)
{
  auto dest = reinterpret_cast<T*>(
    reinterpret_cast<uintptr_t>(base) + offset);
  *dest = value;
}

TEST(TypeTraitsClassTypes, NSDMListGroup1Monostate)
{
  constexpr auto std_monostate_members =
    rfl::public_flattened_nsdm_v<std::monostate>;
  static_assert(std_monostate_members.size() == 0);
}

TEST(TypeTraitsClassTypes, NSDMListGroup1Foo)
{
  constexpr auto foo_members =
    rfl::public_flattened_nsdm_v<foo_t>;
  static_assert(foo_members.size() == 2);
  auto expected_offsets = std::array{0z, 4z};
  EXPECT_THAT(foo_members.to_actual_offset_bytes().values,
    testing::ContainerEq(expected_offsets));

  auto foo = foo_t{};
  foo.[:get<0>(foo_members).member:] = 21;
  foo.[:get<1>(foo_members).member:] = 42;
  EXPECT_EQ("21,42", std::format("{},{}", foo.x, foo.y));
}

TEST(TypeTraitsClassTypes, NSDMListGroup1Bar1)
{
  constexpr auto bar_1_members =
    rfl::public_flattened_nsdm_v<bar_1_t>;
  static_assert(bar_1_members.size() == 4);
  auto expected_offsets = std::array{0z, 4z, 16z, 20z};
  EXPECT_THAT(bar_1_members.to_actual_offset_bytes().values,
    testing::ContainerEq(expected_offsets));

  auto bar_1 = bar_1_t{};
  bar_1.[:get<0>(bar_1_members).member:] = 63;
  bar_1.[:get<1>(bar_1_members).member:] = 84;
  bar_1.[:get<2>(bar_1_members).member:] = 4.5;
  bar_1.[:get<3>(bar_1_members).member:] = 123;
  EXPECT_EQ("63,84", std::format("{},{}", bar_1.x, bar_1.y));
  EXPECT_EQ("average_rating = 4.5, rating_count = 123", bar_1.dump());
}

TEST(TypeTraitsClassTypes, NSDMListGroup1Bar2)
{
  constexpr auto bar_2_members =
    rfl::public_flattened_nsdm_v<bar_2_t>;
  static_assert(bar_2_members.size() == 3);

  auto bar_2 = bar_2_t{};
  bar_2.[:get<0>(bar_2_members).member:] = 100;
  bar_2.[:get<1>(bar_2_members).member:] = 200;
  bar_2.[:get<2>(bar_2_members).member:] = 3.125;
  auto first_from_base = &std::pair<int32_t, int32_t>::first;
  EXPECT_EQ("100,200,3.125", std::format(
    "{},{},{}", bar_2.*first_from_base, bar_2.second, bar_2.first));

  bar_2 = bar_2_t{};
  force_write_by_offset(&bar_2, get_offset<0>(bar_2_members), 400);
  force_write_by_offset(&bar_2, get_offset<1>(bar_2_members), 800);
  force_write_by_offset(&bar_2, get_offset<2>(bar_2_members), -6.25f);
  EXPECT_EQ("400,800,-6.25", std::format(
    "{},{},{}", bar_2.*first_from_base, bar_2.second, bar_2.first));
}

TEST(TypeTraitsClassTypes, NSDMListGroup1Bar3)
{
  constexpr auto bar_3_members =
    rfl::public_flattened_nsdm_v<bar_3_t>;
  static_assert(bar_3_members.size() == 3);

  auto bar_3 = bar_3_t{};
  bar_3.[:get<0>(bar_3_members).member:] = 1000;
  bar_3.[:get<1>(bar_3_members).member:] = 2000;
  bar_3.[:get<2>(bar_3_members).member:] = -1234.5;
  auto first_from_base = &std::pair<int32_t, int32_t>::first;
  EXPECT_EQ("1000,2000,-1234.5", std::format(
    "{},{},{}", bar_3.*first_from_base, bar_3.second, bar_3.first));

  bar_3 = bar_3_t{};
  force_write_by_offset(&bar_3, get_offset<0>(bar_3_members), 400);
  force_write_by_offset(&bar_3, get_offset<1>(bar_3_members), 800);
  force_write_by_offset(&bar_3, get_offset<2>(bar_3_members), -6.25f);
  EXPECT_EQ("400,800,-6.25", std::format(
    "{},{},{}", bar_3.*first_from_base, bar_3.second, bar_3.first));
  EXPECT_EQ("[0, 0, 0, 0, 0, 0]", bar_3.dump_private_base());
}

TEST(TypeTraitsClassTypes, NSDMListGroup1Baz1)
{
  constexpr auto baz_1_members =
    rfl::public_flattened_nsdm_v<baz_1_t>;
  static_assert(baz_1_members.size() == 9);

  auto baz_1 = baz_1_t{};
  // inherited from bar_1
  baz_1.[:get<0>(baz_1_members).member:] = 123;
  baz_1.[:get<1>(baz_1_members).member:] = 456;
  baz_1.[:get<2>(baz_1_members).member:] = -1.25;
  baz_1.[:get<3>(baz_1_members).member:] = 789;
  // inherited from bar_3
  baz_1.[:get<4>(baz_1_members).member:] = 111;
  baz_1.[:get<5>(baz_1_members).member:] = 222;
  baz_1.[:get<6>(baz_1_members).member:] = -3.875;
  // direct members of baz_1
  baz_1.[:get<7>(baz_1_members).member:] = {10, 20, 30, 50};
  baz_1.[:get<8>(baz_1_members).member:] = 110;
  EXPECT_EQ("123,456", std::format("{},{}", baz_1.x, baz_1.y));
  EXPECT_EQ("average_rating = -1.25, rating_count = 789, "
            "e = [10, 20, 30, 50], e_sum = 110, f = ['a', 'b', 'c']",
            baz_1.dump());
  auto first_from_base = &std::pair<int32_t, int32_t>::first;
  EXPECT_EQ("111,222,-3.875", std::format(
    "{},{},{}", baz_1.*first_from_base, baz_1.second, baz_1.first));
  EXPECT_EQ("[0, 0, 0, 0, 0, 0]", baz_1.dump_private_base());

  baz_1 = baz_1_t{};
  // inherited from bar_1
  force_write_by_offset(&baz_1, get_offset<0>(baz_1_members), 11);
  force_write_by_offset(&baz_1, get_offset<1>(baz_1_members), 22);
  force_write_by_offset(&baz_1, get_offset<2>(baz_1_members), 3.375f);
  force_write_by_offset(&baz_1, get_offset<3>(baz_1_members), 44);
  // inherited from bar_3
  force_write_by_offset(&baz_1, get_offset<4>(baz_1_members), 55);
  force_write_by_offset(&baz_1, get_offset<5>(baz_1_members), 66);
  force_write_by_offset(&baz_1, get_offset<6>(baz_1_members), -7.75f);
  // direct members of baz_1
  force_write_by_offset(&baz_1, get_offset<7>(baz_1_members),
    std::array<int32_t, 4>{8, 9, 10, 11});
  force_write_by_offset(&baz_1, get_offset<8>(baz_1_members), 38);
  EXPECT_EQ("11,22", std::format("{},{}", baz_1.x, baz_1.y));
  EXPECT_EQ("average_rating = 3.375, rating_count = 44, "
            "e = [8, 9, 10, 11], e_sum = 38, f = ['a', 'b', 'c']",
            baz_1.dump());
  EXPECT_EQ("55,66,-7.75", std::format(
    "{},{},{}", baz_1.*first_from_base, baz_1.second, baz_1.first));
  EXPECT_EQ("[0, 0, 0, 0, 0, 0]", baz_1.dump_private_base());
}
