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

struct references_t {
  int& i;
  const long long& cll;
  volatile float& vf;
  const volatile double& cvd;
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

template <class T>
void force_write_ref_by_offset(void* base, uintptr_t offset, T value)
{
  auto dest = reinterpret_cast<T**>(
    reinterpret_cast<uintptr_t>(base) + offset);
  **dest = value;
}

// Note: this test case assumes that references are implemented as pointers
// to target value by the C++ compiler.
TEST(TypeTraitsClassTypes, NSDMListGroup2References)
{
  constexpr auto ref_members =
    rfl::public_flattened_nsdm_v<references_t>;
  static_assert(ref_members.size() == 4);
  auto expected_offsets = std::array<ptrdiff_t, 4>{
    0, sizeof(void*), sizeof(void*) * 2, sizeof(void*) * 3};
  EXPECT_THAT(ref_members.to_actual_offset_bytes().values,
    testing::ContainerEq(expected_offsets));

  auto [i, ll, f, d] = std::tuple{1, 2LL, 3.5f, 4.75};
  auto foo = references_t{i, ll, f, d};
  foo.[:get<0>(ref_members).member:] += 100;
  foo.[:get<2>(ref_members).member:] += 200;
  EXPECT_EQ("101, 2, 203.5, 4.75",
    std::format("{}, {}, {}, {}", foo.i, foo.cll,
      static_cast<float>(foo.vf), static_cast<double>(foo.cvd)));
  EXPECT_EQ("101, 2, 203.5, 4.75",
    std::format("{}, {}, {}, {}", i, ll, f, d));

  // Ignores cv qualifiers by reinterpret_cast
  force_write_ref_by_offset(&foo, get_offset<0>(ref_members), 100);
  force_write_ref_by_offset(&foo, get_offset<1>(ref_members), 200LL);
  force_write_ref_by_offset(&foo, get_offset<2>(ref_members), 300.0f);
  force_write_ref_by_offset(&foo, get_offset<3>(ref_members), 400.0);
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

template <size_t I, auto... Values>
constexpr auto get_bf_offset(rfl::constant<Values...> members)
{
  auto offset = get<I>(members).actual_offset;
  return offset.bytes * CHAR_BIT + offset.bits;
}

template <size_t I, auto... Values>
constexpr auto get_bf_size(rfl::constant<Values...> members) {
  return bit_size_of(get<I>(members).member);
}

void force_write_bf_by_offset(
  void* base, uintptr_t offset_bits, uintptr_t size_bits, uint64_t value)
{
  auto* u64_arr = reinterpret_cast<uint64_t*>(base);
  size_t u64_index = offset_bits / 64;
  size_t u64_offset_bits = offset_bits % 64;

  if (u64_offset_bits + size_bits <= 64) {
    auto mask = ((uint64_t{1} << size_bits) - 1) << u64_offset_bits;
    u64_arr[u64_index] &= ~mask;
    u64_arr[u64_index] |= (value << u64_offset_bits) & mask;
    return;
  }
  auto size_bits_first_part = 64 - u64_offset_bits;
  force_write_bf_by_offset(base, offset_bits, size_bits_first_part, value);
  auto size_bits_second_part = size_bits - size_bits_first_part;
  force_write_bf_by_offset(
    base, offset_bits + size_bits_first_part, size_bits_second_part,
    value >> size_bits_first_part);
}

TEST(TypeTraitsClassTypes, NSDMListGroup2BitFields)
{
  constexpr auto members =
    rfl::public_flattened_nsdm_v<bit_fields_B_t>;
  static_assert(members.size() == 8);

  auto bf = bit_fields_B_t{};
  bf.[:get<0>(members).member:] = true;
  bf.[:get<1>(members).member:] = 10;
  bf.[:get<2>(members).member:] = 12345;
  bf.[:get<3>(members).member:] = false;
  bf.[:get<4>(members).member:] = 6;
  bf.[:get<5>(members).member:] = true;
  bf.[:get<6>(members).member:] = 3;
  bf.[:get<7>(members).member:] = 123456789012345uLL;
  EXPECT_EQ("1, 10, 12345", bf.dump_A());
  EXPECT_EQ("0, 6, 1, 3, 123456789012345", bf.dump_B());

  uint64_t arr[2] = {0, 0};
  force_write_bf_by_offset(arr, 40, 42, 0x355'5555'5555uLL);
  EXPECT_EQ(0x5555'5500'0000'0000uLL, arr[0])
    << "Implementation error of testing tools.";
  EXPECT_EQ(0x0000'0000'0003'5555uLL, arr[1])
    << "Implementation error of testing tools.";

  bf = bit_fields_B_t{};
  force_write_bf_by_offset(
    &bf, get_bf_offset<0>(members), get_bf_size<0>(members), 1);
  force_write_bf_by_offset(
    &bf, get_bf_offset<1>(members), get_bf_size<1>(members), 10);
  force_write_bf_by_offset(
    &bf, get_bf_offset<2>(members), get_bf_size<2>(members), 12345);
  force_write_bf_by_offset(
    &bf, get_bf_offset<3>(members), get_bf_size<3>(members), 0);
  force_write_bf_by_offset(
    &bf, get_bf_offset<4>(members), get_bf_size<4>(members), 6);
  force_write_bf_by_offset(
    &bf, get_bf_offset<5>(members), get_bf_size<5>(members), 1);
  force_write_bf_by_offset(
    &bf, get_bf_offset<6>(members), get_bf_size<6>(members), 3);
  force_write_bf_by_offset(
    &bf, get_bf_offset<7>(members), get_bf_size<7>(members), 1234567890123uLL);
  EXPECT_EQ("1, 10, 12345", bf.dump_A());
  EXPECT_EQ("0, 6, 1, 3, 1234567890123", bf.dump_B());
}
