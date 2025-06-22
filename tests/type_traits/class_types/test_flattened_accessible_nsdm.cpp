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
#include <cassert>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#endif

namespace rfl = reflect_cpp26;

template <class T>
void force_write_by_offset(void* base, uintptr_t offset, T value)
{
  auto dest = reinterpret_cast<T*>(
    reinterpret_cast<uintptr_t>(base) + offset);
  *dest = value;
}

template <class T>
void force_write_ref_by_offset(void* base, uintptr_t offset, T value)
{
  auto dest = reinterpret_cast<T**>(
    reinterpret_cast<uintptr_t>(base) + offset);
  **dest = value;
}

void force_write_bf_by_offset(
  void* base, uintptr_t offset_bits, uintptr_t size_bits, uint64_t value)
{
  auto* u64_arr = reinterpret_cast<uint64_t*>(base);
  size_t u64_index = offset_bits / 64;
  size_t u64_offset_bits = offset_bits % 64;

  assert(u64_offset_bits + size_bits <= 64);
  auto mask = ((uint64_t{1} << size_bits) - 1) << u64_offset_bits;
  u64_arr[u64_index] &= ~mask;
  u64_arr[u64_index] |= (value << u64_offset_bits) & mask;
}

consteval ptrdiff_t get_offset(
  std::span<const rfl::flattened_data_member_info> members, size_t index)
{
  return members[index].actual_offset_bytes();
}

consteval ptrdiff_t get_bf_offset(
  std::span<const rfl::flattened_data_member_info> members, size_t index)
{
  return members[index].actual_offset_bits();
}

consteval ptrdiff_t get_bf_size(
  std::span<const rfl::flattened_data_member_info> members, size_t index)
{
  return bit_size_of(members[index].member);
}

struct foo_t {
  int32_t x;
  int32_t y;

  void set_xy(int x, int y);
private:
  double _timestamp;
};

TEST(TypeTraitsClassTypes, NSDMListMonostate)
{
  constexpr auto std_monostate_members =
    rfl::public_flattened_nsdm_v<std::monostate>;
  static_assert(std_monostate_members.size() == 0);
}

TEST(TypeTraitsClassTypes, NSDMListFoo)
{
  constexpr auto foo_members = rfl::public_flattened_nsdm_v<foo_t>;
  static_assert(foo_members.size() == 2);
  static_assert(foo_members[0].actual_offset_bytes() == 0z);
  static_assert(foo_members[1].actual_offset_bytes() == 4z);

  auto foo = foo_t{};
  foo.[:foo_members[0].member:] = 21;
  foo.[:foo_members[1].member:] = 42;
  EXPECT_EQ("21,42", std::format("{},{}", foo.x, foo.y));
}

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

TEST(TypeTraitsClassTypes, NSDMListBar1)
{
  constexpr auto bar_1_members = rfl::public_flattened_nsdm_v<bar_1_t>;
  static_assert(bar_1_members.size() == 4);
  static_assert(bar_1_members[0].actual_offset_bytes() == 0z);
  static_assert(bar_1_members[1].actual_offset_bytes() == 4z);
  static_assert(bar_1_members[2].actual_offset_bytes() == 16z);
  static_assert(bar_1_members[3].actual_offset_bytes() == 20z);

  auto bar_1 = bar_1_t{};
  bar_1.[:bar_1_members[0].member:] = 63;
  bar_1.[:bar_1_members[1].member:] = 84;
  bar_1.[:bar_1_members[2].member:] = 4.5;
  bar_1.[:bar_1_members[3].member:] = 123;
  EXPECT_EQ("63,84", std::format("{},{}", bar_1.x, bar_1.y));
  EXPECT_EQ("average_rating = 4.5, rating_count = 123", bar_1.dump());
}

struct bar_2_t : std::pair<int32_t, int32_t>, protected bar_1_t {
  float first; // Shadows member 'first' of base
protected:
  char d2;
};

TEST(TypeTraitsClassTypes, NSDMListBar2)
{
  constexpr auto bar_2_members = rfl::public_flattened_nsdm_v<bar_2_t>;
  static_assert(bar_2_members.size() == 3);

  auto bar_2 = bar_2_t{};
  bar_2.[:bar_2_members[0].member:] = 100;
  bar_2.[:bar_2_members[1].member:] = 200;
  bar_2.[:bar_2_members[2].member:] = 3.125;
  auto first_from_base = &std::pair<int32_t, int32_t>::first;
  EXPECT_EQ("100,200,3.125", std::format(
    "{},{},{}", bar_2.*first_from_base, bar_2.second, bar_2.first));

  bar_2 = bar_2_t{};
  force_write_by_offset(&bar_2, get_offset(bar_2_members, 0), 400);
  force_write_by_offset(&bar_2, get_offset(bar_2_members, 1), 800);
  force_write_by_offset(&bar_2, get_offset(bar_2_members, 2), -6.25f);
  EXPECT_EQ("400,800,-6.25", std::format(
    "{},{},{}", bar_2.*first_from_base, bar_2.second, bar_2.first));
}

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

TEST(TypeTraitsClassTypes, NSDMListBar3)
{
  constexpr auto bar_3_members = rfl::public_flattened_nsdm_v<bar_3_t>;
  static_assert(bar_3_members.size() == 3);

  auto bar_3 = bar_3_t{};
  bar_3.[:bar_3_members[0].member:] = 1000;
  bar_3.[:bar_3_members[1].member:] = 2000;
  bar_3.[:bar_3_members[2].member:] = -1234.5;
  auto first_from_base = &std::pair<int32_t, int32_t>::first;
  EXPECT_EQ("1000,2000,-1234.5", std::format(
    "{},{},{}", bar_3.*first_from_base, bar_3.second, bar_3.first));

  bar_3 = bar_3_t{};
  force_write_by_offset(&bar_3, get_offset(bar_3_members, 0), 400);
  force_write_by_offset(&bar_3, get_offset(bar_3_members, 1), 800);
  force_write_by_offset(&bar_3, get_offset(bar_3_members, 2), -6.25f);
  EXPECT_EQ("400,800,-6.25", std::format(
    "{},{},{}", bar_3.*first_from_base, bar_3.second, bar_3.first));
  EXPECT_EQ("[0, 0, 0, 0, 0, 0]", bar_3.dump_private_base());
}

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

TEST(TypeTraitsClassTypes, NSDMListBaz1)
{
  constexpr auto baz_1_members = rfl::public_flattened_nsdm_v<baz_1_t>;
  static_assert(baz_1_members.size() == 9);

  auto baz_1 = baz_1_t{};
  // inherited from bar_1
  baz_1.[:baz_1_members[0].member:] = 123;
  baz_1.[:baz_1_members[1].member:] = 456;
  baz_1.[:baz_1_members[2].member:] = -1.25;
  baz_1.[:baz_1_members[3].member:] = 789;
  // inherited from bar_3
  baz_1.[:baz_1_members[4].member:] = 111;
  baz_1.[:baz_1_members[5].member:] = 222;
  baz_1.[:baz_1_members[6].member:] = -3.875;
  // direct members of baz_1
  baz_1.[:baz_1_members[7].member:] = {10, 20, 30, 50};
  baz_1.[:baz_1_members[8].member:] = 110;
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
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 0), 11);
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 1), 22);
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 2), 3.375f);
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 3), 44);
  // inherited from bar_3
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 4), 55);
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 5), 66);
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 6), -7.75f);
  // direct members of baz_1
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 7),
    std::array<int32_t, 4>{8, 9, 10, 11});
  force_write_by_offset(&baz_1, get_offset(baz_1_members, 8), 38);
  EXPECT_EQ("11,22", std::format("{},{}", baz_1.x, baz_1.y));
  EXPECT_EQ("average_rating = 3.375, rating_count = 44, "
            "e = [8, 9, 10, 11], e_sum = 38, f = ['a', 'b', 'c']",
            baz_1.dump());
  EXPECT_EQ("55,66,-7.75", std::format(
    "{},{},{}", baz_1.*first_from_base, baz_1.second, baz_1.first));
  EXPECT_EQ("[0, 0, 0, 0, 0, 0]", baz_1.dump_private_base());
}

struct references_t {
  int& i;
  const long long& cll;
  volatile float& vf;
  const volatile double& cvd;
};

// Note: this test case assumes that references are implemented as pointers
// to target value by the C++ compiler.
TEST(TypeTraitsClassTypes, NSDMListReferences)
{
  constexpr auto ref_members = rfl::public_flattened_nsdm_v<references_t>;
  static_assert(ref_members.size() == 4);
  static_assert(ref_members[0].actual_offset_bytes() == 0 * sizeof(void*));
  static_assert(ref_members[1].actual_offset_bytes() == 1 * sizeof(void*));
  static_assert(ref_members[2].actual_offset_bytes() == 2 * sizeof(void*));
  static_assert(ref_members[3].actual_offset_bytes() == 3 * sizeof(void*));

  auto [i, ll, f, d] = std::tuple{1, 2LL, 3.5f, 4.75};
  auto foo = references_t{i, ll, f, d};
  foo.[:ref_members[0].member:] += 100;
  foo.[:ref_members[2].member:] += 200;
  EXPECT_EQ("101, 2, 203.5, 4.75",
    std::format("{}, {}, {}, {}", foo.i, foo.cll,
      static_cast<float>(foo.vf), static_cast<double>(foo.cvd)));
  EXPECT_EQ("101, 2, 203.5, 4.75",
    std::format("{}, {}, {}, {}", i, ll, f, d));

  // Ignores cv qualifiers by reinterpret_cast
  force_write_ref_by_offset(&foo, get_offset(ref_members, 0), 100);
  force_write_ref_by_offset(&foo, get_offset(ref_members, 1), 200LL);
  force_write_ref_by_offset(&foo, get_offset(ref_members, 2), 300.0f);
  force_write_ref_by_offset(&foo, get_offset(ref_members, 3), 400.0);
  EXPECT_EQ("100, 200, 300, 400",
    std::format("{}, {}, {}, {}", foo.i, foo.cll,
      static_cast<float>(foo.vf), static_cast<double>(foo.cvd)));
  EXPECT_EQ("100, 200, 300, 400",
    std::format("{}, {}, {}, {}", i, ll, f, d));
}

struct bit_fields_A_t {
  uint32_t flag: 1;
  uint32_t tag: 5;
  uint32_t value: 17;

  auto dump_A() const -> std::string {
    return std::format("{}, {}, {}", flag, tag, value);
  }
};
static_assert(sizeof(bit_fields_A_t) == 4);

struct bit_fields_B_t : bit_fields_A_t {
  uint32_t flag_extra: 1;
  uint32_t mark: 4;
  uint64_t hash_valid_flag: 1;
  uint64_t hash_header: 3;
  uint64_t hash_value: 60;

  auto dump_B() const -> std::string {
    return std::format("{}, {}, {}, {}, {}",
      flag_extra, mark, hash_valid_flag, hash_header, hash_value);
  }
};

TEST(TypeTraitsClassTypes, NSDMListBitFields)
{
  constexpr auto members = rfl::public_flattened_nsdm_v<bit_fields_B_t>;
  static_assert(members.size() == 8);

  auto bf = bit_fields_B_t{};
  bf.[:members[0].member:] = true;
  bf.[:members[1].member:] = 10;
  bf.[:members[2].member:] = 12345;
  bf.[:members[3].member:] = false;
  bf.[:members[4].member:] = 6;
  bf.[:members[5].member:] = true;
  bf.[:members[6].member:] = 3;
  bf.[:members[7].member:] = 123456789012345uLL;
  EXPECT_EQ("1, 10, 12345", bf.dump_A());
  EXPECT_EQ("0, 6, 1, 3, 123456789012345", bf.dump_B());

  bf = bit_fields_B_t{};
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 0), get_bf_size(members, 0), 1);
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 1), get_bf_size(members, 1), 10);
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 2), get_bf_size(members, 2), 12345);
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 3), get_bf_size(members, 3), 0);
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 4), get_bf_size(members, 4), 6);
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 5), get_bf_size(members, 5), 1);
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 6), get_bf_size(members, 6), 3);
  force_write_bf_by_offset(
    &bf, get_bf_offset(members, 7), get_bf_size(members, 7), 1234567890123uLL);
  EXPECT_EQ("1, 10, 12345", bf.dump_A());
  EXPECT_EQ("0, 6, 1, 3, 1234567890123", bf.dump_B());
}

struct A {
  uint32_t a1;
  uint8_t a2;

  explicit constexpr A(uint8_t base): a1(base), a2(base + 1) {}

  constexpr virtual uint32_t get_from_a() const {
    return a1 + a2;
  }
};

struct B : A {
  uint8_t b1;
  uint64_t b2;

  explicit constexpr B(uint8_t base): A(base), b1(base + 10), b2(base + 11) {}

  constexpr uint32_t get_from_a() const override {
    return static_cast<uint32_t>(b1 + b2);
  }
  virtual constexpr uint32_t get_from_b() const {
    return b1 * b2;
  }
};

struct C {
  uint32_t c1;
  uint32_t c2;
  uint8_t c3;

  explicit constexpr C(uint8_t base)
    : c1(base + 20), c2(base + 21), c3(base + 22) {}

  virtual constexpr uint32_t get_from_c() const {
    return c1 * c2 + c3;
  }
};

struct D : B, C {
  uint8_t d1;
  uint16_t d2;
  uint32_t d3;
  uint64_t d4;

  explicit constexpr D(uint8_t base)
    : B(base), C(base),
      d1(base + 30), d2(base + 31), d3(base + 32), d4(base + 33) {}

  constexpr uint32_t get_from_a() const override {
    return d1 + d2 + d3 + d4;
  }
  constexpr uint32_t get_from_b() const override {
    return d1 * d2 + d3 * d4;
  }
  constexpr uint32_t get_from_c() const override {
    return d3 * d4 - d1 * d2;
  }
  virtual constexpr uint32_t get_from_d() const {
    return -d1 + d2 - d3 + d4;
  }
};

TEST(TypeTraitsClassTypes, NSDMListPolymorphic)
{
  constexpr auto members = rfl::public_flattened_nsdm_v<D>;
  static_assert(members.size() == 11);

  constexpr auto obj1 = D{10};
  EXPECT_EQ_STATIC(10, obj1.[:members[0].member:]);
  EXPECT_EQ_STATIC(11, obj1.[:members[1].member:]);
  EXPECT_EQ_STATIC(20, obj1.[:members[2].member:]);
  EXPECT_EQ_STATIC(21, obj1.[:members[3].member:]);
  EXPECT_EQ_STATIC(30, obj1.[:members[4].member:]);
  EXPECT_EQ_STATIC(31, obj1.[:members[5].member:]);
  EXPECT_EQ_STATIC(32, obj1.[:members[6].member:]);
  EXPECT_EQ_STATIC(40, obj1.[:members[7].member:]);
  EXPECT_EQ_STATIC(41, obj1.[:members[8].member:]);
  EXPECT_EQ_STATIC(42, obj1.[:members[9].member:]);
  EXPECT_EQ_STATIC(43, obj1.[:members[10].member:]);

  auto obj2 = obj1;
  force_write_by_offset(&obj2, get_offset(members, 0), uint32_t{1});
  force_write_by_offset(&obj2, get_offset(members, 1), uint8_t{2});
  force_write_by_offset(&obj2, get_offset(members, 2), uint8_t{3});
  force_write_by_offset(&obj2, get_offset(members, 3), uint64_t{4});
  force_write_by_offset(&obj2, get_offset(members, 4), uint32_t{5});
  force_write_by_offset(&obj2, get_offset(members, 5), uint32_t{6});
  force_write_by_offset(&obj2, get_offset(members, 6), uint8_t{7});
  force_write_by_offset(&obj2, get_offset(members, 7), uint8_t{8});
  force_write_by_offset(&obj2, get_offset(members, 8), uint16_t{9});
  force_write_by_offset(&obj2, get_offset(members, 9), uint16_t{10});
  force_write_by_offset(&obj2, get_offset(members, 10), uint16_t{11});
  EXPECT_EQ(1, obj2.a1);
  EXPECT_EQ(2, obj2.a2);
  EXPECT_EQ(3, obj2.b1);
  EXPECT_EQ(4, obj2.b2);
  EXPECT_EQ(5, obj2.c1);
  EXPECT_EQ(6, obj2.c2);
  EXPECT_EQ(7, obj2.c3);
  EXPECT_EQ(8, obj2.d1);
  EXPECT_EQ(9, obj2.d2);
  EXPECT_EQ(10, obj2.d3);
  EXPECT_EQ(11, obj2.d4);
}
