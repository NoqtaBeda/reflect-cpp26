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
#include <system_error>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/class_types/member_access.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_t {
  int x;
private:
  int y;
protected:
  const foo_t* prev;
public:
  const foo_t** next;
};

TEST(TypeTraitsClassTypes, MemberAccessFooPublic)
{
  static_assert(std::is_same_v<
    rfl::ith_public_direct_nsdm_type_t<0, foo_t>, int>);
  static_assert(std::is_same_v<
    rfl::ith_public_direct_nsdm_type_t<1, const foo_t&>, const foo_t** const&>);
}

TEST(TypeTraitsClassTypes, MemberAccessFooAll)
{
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<0, foo_t>, int>);
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<1, foo_t&>, int&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<2, const foo_t&>, const foo_t* const &>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<3, volatile foo_t&&>,
    const foo_t** volatile&&>);
}

struct bar_t : foo_t {
  volatile double value;

  explicit bar_t(unsigned x, const foo_t& a1, volatile foo_t&& a2)
    : value(x), ok_flag(x & 1), done_flag((x >> 1) & 1), label(x >> 2),
      associated_1(a1), associated_2(std::move(a2)) {}

private:
  unsigned ok_flag: 1;
protected:
  volatile unsigned done_flag: 1;
public:
  const unsigned label: 30;
  const foo_t& associated_1;
private:
  volatile foo_t&& associated_2;
};

TEST(TypeTraitsClassTypes, MemberAccessBarPublic)
{
  static_assert(std::is_same_v<
    rfl::ith_public_direct_nsdm_type_t<0, bar_t>, volatile double>);
  static_assert(std::is_same_v<
    rfl::ith_public_direct_nsdm_type_t<1, volatile bar_t>,
    const volatile unsigned>);
  static_assert(std::is_same_v<
    rfl::ith_public_direct_nsdm_type_t<2, bar_t&>, const foo_t&>);

  static_assert(std::is_same_v<
    rfl::ith_public_flattened_nsdm_type_t<0, const bar_t>, const int>);
  static_assert(std::is_same_v<
    rfl::ith_public_flattened_nsdm_type_t<1, volatile bar_t>,
    const foo_t** volatile>);
  static_assert(std::is_same_v<
    rfl::ith_public_flattened_nsdm_type_t<2, const bar_t&>,
    const volatile double&>);
  static_assert(std::is_same_v<
    rfl::ith_public_flattened_nsdm_type_t<3, const bar_t&>,
    const unsigned&>);
  static_assert(std::is_same_v<
    rfl::ith_public_flattened_nsdm_type_t<4, volatile bar_t&&>, const foo_t&>);
};

TEST(TypeTraitsClassTypes, MemberAccessBarAll)
{
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<0, bar_t>, volatile double>);
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<1, bar_t>, unsigned>);
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<2, bar_t>, volatile unsigned>);
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<3, bar_t>, const unsigned>);
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<4, bar_t>, const foo_t&>);
  static_assert(std::is_same_v<
    rfl::ith_direct_nsdm_type_t<5, bar_t>, volatile foo_t&&>);

  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<0, const bar_t&>, const int&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<1, const bar_t&&>, const int&&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<2, volatile bar_t&>,
    const foo_t* volatile&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<3, volatile bar_t&&>,
    const foo_t** volatile&&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<4, const bar_t&>, const volatile double&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<5, bar_t&>, unsigned&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<6, const bar_t&>,
    const volatile unsigned&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<7, volatile bar_t&&>,
    const volatile unsigned&&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<8, volatile bar_t&&>, const foo_t&>);
  static_assert(std::is_same_v<
    rfl::ith_flattened_nsdm_type_t<9, const bar_t&>, volatile foo_t&&>);
}
