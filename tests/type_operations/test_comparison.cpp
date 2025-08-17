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
#include <cstdio>
#include <deque>
#include <limits>
#include <list>
#include <set>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_operations.hpp>
#else
#include <reflect_cpp26/type_operations/comparison.hpp>
#endif

namespace rfl = reflect_cpp26;

template <class FloatingT>
constexpr auto nan_v = std::numeric_limits<FloatingT>::quiet_NaN();

template <class T, class U>
constexpr bool check_equality(const T& expected, const U& actual)
{
  static_assert(std::is_same_v<T, U>, "Type mismatch.");
  return expected == actual;
}

template <class T, class U>
constexpr bool check_inequality(const T& unexpected, const U& actual)
{
  static_assert(std::is_same_v<T, U>, "Type mismatch.");
  return unexpected != actual;
}

TEST(TypeOperationsComparison, Arithmetic)
{
  // Between integer & character types
  static_assert(rfl::generic_equal(48u, u8'0'));
  static_assert(! rfl::generic_not_equal(48u, u8'0'));
  static_assert(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(48u, u8'0')));

  static_assert(! rfl::generic_equal(40, u'0'));
  static_assert(rfl::generic_not_equal(40, u'0'));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(40, u'0')));

  // Between signed & unsigned
  static_assert(rfl::generic_equal(2, 2u));
  static_assert(! rfl::generic_not_equal(2, 2u));
  static_assert(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(2, 2u)));

  static_assert(! rfl::generic_equal(-1, 4294967295u));
  static_assert(rfl::generic_not_equal(-1, 4294967295u));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(-1, 4294967295u)));

  // Between integral & floating point
  // Note that comparison result with FP types is std::partial_ordering.
  static_assert(rfl::generic_equal(2.0, 2));
  static_assert(! rfl::generic_not_equal(2.0, 2));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way(2.0, 2)));

  static_assert(! rfl::generic_equal(3, 2.5));
  static_assert(rfl::generic_not_equal(3, 2.5));
  static_assert(check_equality(
    std::partial_ordering::greater,
    rfl::generic_compare_three_way(3, 2.5)));

  // Between floating points
  static_assert(rfl::generic_equal(1.5, 1.5f));
  static_assert(! rfl::generic_not_equal(1.5, 1.5f));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way(1.5, 1.5f)));

  static_assert(! rfl::generic_equal(nan_v<double>, nan_v<double>));
  static_assert(rfl::generic_not_equal(nan_v<double>, nan_v<double>));
  static_assert(check_equality(
    std::partial_ordering::unordered,
    rfl::generic_compare_three_way(nan_v<double>, nan_v<double>)));
}

TEST(TypeOperationsComparison, Pointers)
{
  // Compile-time comparison
  constexpr auto int_value_1 = 1234;
  constexpr auto int_value_2 = 5678;
  static_assert(! rfl::generic_equal(&int_value_1, &int_value_2));
  static_assert(rfl::generic_not_equal(&int_value_1, &int_value_2));

  static_assert(! rfl::generic_equal(scanf, printf));
  static_assert(rfl::generic_not_equal(scanf, printf));

  static_assert(rfl::generic_equal(scanf, std::scanf));
  static_assert(! rfl::generic_not_equal(scanf, std::scanf));

  // Runtime comparison:
  // int* vs. int*
  int arr[4] = {0, 1, 2, 3};
  EXPECT_TRUE(rfl::generic_equal(arr + 1, arr + 1));
  EXPECT_FALSE(rfl::generic_not_equal(arr + 1, arr + 1));
  EXPECT_FALSE(rfl::generic_equal(arr + 2, arr + 1));
  EXPECT_TRUE(rfl::generic_not_equal(arr + 2, arr + 1));
  EXPECT_EQ(std::strong_ordering::less,
            rfl::generic_compare_three_way(arr + 1, arr + 3));

  // const char* vs. const char*: Compared by address.
  // Same behavior as (str1 < str2).
  // Note: such behavior is allowed yet discouraged due to semantic confusion.
  const char* str1 = "Hello\0Hello";
  const char* str2 = str1 + 6;
  EXPECT_FALSE(rfl::generic_equal(str1, str2));
  EXPECT_TRUE(rfl::generic_not_equal(str1, str2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(str1, str2)));

  // std::string_view vs. const char*: Compared by contents.
  // Same behavior as std::string_view::operator<(const char*)
  EXPECT_TRUE(rfl::generic_equal(std::string_view{str1}, str2));
  EXPECT_FALSE(rfl::generic_not_equal(std::string_view{str1}, str2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(std::string_view{str2}, str1)));
}

// Pointer to members
struct foo_t {
  const char* name;
  int x;
  int y;
  void foo(int);
  void bar(int);
};

TEST(TypeOperationsComparison, PointerToMembers)
{
  static_assert(rfl::generic_equal(&foo_t::x, &foo_t::x));
  static_assert(! rfl::generic_not_equal(&foo_t::x, &foo_t::x));

  static_assert(! rfl::generic_equal(&foo_t::x, &foo_t::y));
  static_assert(rfl::generic_not_equal(&foo_t::x, &foo_t::y));

  static_assert(! rfl::generic_equal(&foo_t::foo, &foo_t::bar));
  static_assert(rfl::generic_not_equal(&foo_t::foo, &foo_t::bar));

  // Member pointers are not 3-way comparable.
  static_assert(! rfl::is_less_comparable_v<
    int foo_t::*, int foo_t::*>);
  static_assert(! rfl::is_generic_three_way_comparable_v<
    int foo_t::*, int foo_t::*>);
  static_assert(! rfl::less_comparable_with<
    void (foo_t::*)(int), void (foo_t::*)(int)>);
  static_assert(! rfl::generic_three_way_comparable_with<
    void (foo_t::*)(int), void (foo_t::*)(int)>);
}

TEST(TypeOperationsComparison, EnumTypes)
{
  static_assert(! rfl::generic_equal(
    std::errc{}, std::errc::connection_reset));
  static_assert(rfl::generic_not_equal(
    std::errc{}, std::errc::connection_reset));

  static_assert(! rfl::is_greater_comparable_v<std::errc, int>);
  static_assert(! rfl::is_generic_equal_comparable_v<std::errc, int>);
  static_assert(! rfl::greater_comparable_with<
    std::errc, std::chars_format>);
  static_assert(! rfl::generic_equal_comparable_with<
    std::errc, std::chars_format>);
}

TEST(TypeOperationsComparison, BothAreInitializerLists)
{
  // Between character types
  static_assert(rfl::generic_equal({'a', 'b', 'c'}, {L'a', L'b', L'c'}));
  static_assert(! rfl::generic_not_equal({'a', 'b', 'c'}, {L'a', L'b', L'c'}));
  static_assert(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way({'a', 'b', 'c'}, {L'a', L'b', L'c'})));

  // Between signed & unsigned integral
  static_assert(rfl::generic_equal({1, 2, 3}, {1u, 2u, 3u}));
  static_assert(! rfl::generic_not_equal({1, 2, 3}, {1u, 2u, 3u}));
  static_assert(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way({1, 2, 3}, {1u, 2u, 3u})));

  static_assert(! rfl::generic_equal(
    {-1, -2, -3}, {4294967295u, 4294967294u, 4294967293u}));
  static_assert(rfl::generic_not_equal(
    {-1, -2, -3}, {4294967295u, 4294967294u, 4294967293u}));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(
      {-1, -2, -3}, {4294967295u, 4294967294u, 4294967293u})));

  // Between integral & floating point
  // Note: comparison result is std::partial_ordering
  static_assert(rfl::generic_equal({1, 2, 3}, {1.0, 2.0, 3.0}));
  static_assert(! rfl::generic_not_equal({1, 2, 3}, {1.0, 2.0, 3.0}));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way({1, 2, 3}, {1.0, 2.0, 3.0})));

  // Between floating points
  static_assert(rfl::generic_equal({1.0f, 2.0f, 3.0f}, {1.0, 2.0, 3.0}));
  static_assert(! rfl::generic_not_equal({1.0f, 2.0f, 3.0f}, {1.0, 2.0, 3.0}));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way({1.0f, 2.0f, 3.0f}, {1.0, 2.0, 3.0})));

  // Range comparison
  // (1) Different value
  static_assert(! rfl::generic_equal({4, 5, 7}, {4, 5, 6}));
  static_assert(rfl::generic_not_equal({4, 5, 7}, {4, 5, 6}));
  static_assert(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way({4, 5, 7}, {4, 5, 6})));

  // (2) Different size
  static_assert(! rfl::generic_equal({4, 5, 6}, {4, 5, 6, 7}));
  static_assert(rfl::generic_not_equal({4, 5, 6}, {4, 5, 6, 7}));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way({4, 5, 6}, {4, 5, 6, 7})));

  // (3) Partial ordering
  static_assert(! rfl::generic_equal(
    {4.0, 5.0, nan_v<double>}, {4.0, 5.0, nan_v<double>}));
  static_assert(rfl::generic_not_equal(
    {4.0, 5.0, nan_v<double>}, {4.0, 5.0, nan_v<double>}));
  static_assert(check_equality(
    std::partial_ordering::unordered,
    rfl::generic_compare_three_way(
      {4.0, 5.0, nan_v<double>}, {4.0, 5.0, nan_v<double>})));
}

TEST(TypeOperationsComparison, OneIsInitializerList1)
{
  // Between character types
  static_assert(rfl::generic_equal(
    std::vector{'a', 'b', 'c'}, {L'a', L'b', L'c'}));
  static_assert(! rfl::generic_not_equal(
    std::vector{'a', 'b', 'c'}, {L'a', L'b', L'c'}));
  static_assert(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(
      std::vector{'a', 'b', 'c'}, {L'a', L'b', L'c'})));

  // Between signed & unsigned integral
  static_assert(rfl::generic_equal({1, 2, 3}, std::vector{1u, 2u, 3u}));
  static_assert(! rfl::generic_not_equal({1, 2, 3}, std::vector{1u, 2u, 3u}));
  static_assert(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way({1, 2, 3}, std::vector{1u, 2u, 3u})));

  static_assert(! rfl::generic_equal(
    std::vector{-1, -2, -3}, {4294967295u, 4294967294u, 4294967293u}));
  static_assert(rfl::generic_not_equal(
    std::vector{-1, -2, -3}, {4294967295u, 4294967294u, 4294967293u}));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(
      std::vector{-1, -2, -3}, {4294967295u, 4294967294u, 4294967293u})));

  // Between integral & floating point
  static_assert(rfl::generic_equal({1, 2, 3}, std::vector{1.0, 2.0, 3.0}));
  static_assert(! rfl::generic_not_equal({1, 2}, std::vector{1.0, 2.0}));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way({1, 2, 3}, std::vector{1.0, 2.0, 3.0})));

  // Between floating points
  static_assert(rfl::generic_equal(
    std::vector{1.0f, 2.0f, 3.0f}, {1.0, 2.0, 3.0}));
  static_assert(! rfl::generic_not_equal(
    std::vector{1.0f, 2.0f, 3.0f}, {1.0, 2.0, 3.0}));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way(
      std::vector{1.0f, 2.0f, 3.0f}, {1.0, 2.0, 3.0})));

  // Range comparison
  // (1) Different value
  static_assert(! rfl::generic_equal({4, 5, 7}, std::vector{4, 5, 6}));
  static_assert(rfl::generic_not_equal({4, 5, 7}, std::vector{4, 5, 6}));
  static_assert(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way({4, 5, 7}, std::vector{4, 5, 6})));

  // (2) Different size
  static_assert(! rfl::generic_equal(std::vector{4, 5, 6}, {4, 5, 6, 7}));
  static_assert(rfl::generic_not_equal(std::vector{4, 5, 6}, {4, 5, 6, 7}));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(std::vector{4, 5, 6}, {4, 5, 6, 7})));

  // (3) Partial ordering
  static_assert(! rfl::generic_equal(
    {4.0, 5.0, nan_v<double>}, std::vector{4.0, 5.0, nan_v<double>}));
  static_assert(rfl::generic_not_equal(
    {4.0, 5.0, nan_v<double>}, std::vector{4.0, 5.0, nan_v<double>}));
  static_assert(check_equality(
    std::partial_ordering::unordered,
    rfl::generic_compare_three_way(
      {4.0, 5.0, nan_v<double>}, std::vector{4.0, 5.0, nan_v<double>})));
}

// std::initializer_list participating in overload resolution
// of comparison operators.
struct bar_t {
  int x;
  int y;
  constexpr bool operator==(const bar_t&) const = default;
  constexpr bool operator==(std::initializer_list<int> il) const {
    return il.size() == 2 && data(il)[0] == x && data(il)[1] == y;
  }
  constexpr auto operator<=>(std::initializer_list<int> il) const
    -> std::weak_ordering
  {
    if (il.size() == 0) {
      return std::weak_ordering::greater;
    }
    return x <=> data(il)[0];
  }
};

TEST(TypeOperationsComparison, OneIsInitializerList2)
{
  static_assert(rfl::generic_equal(bar_t{1, 2}, {1, 2}));
  static_assert(! rfl::generic_not_equal(bar_t{1, 2}, {1, 2}));
  static_assert(check_equality(
    std::weak_ordering::equivalent,
    rfl::generic_compare_three_way(bar_t{1, 2}, {1, 2})));

  static_assert(! rfl::generic_equal({1, 2}, bar_t{1, -2}));
  static_assert(rfl::generic_not_equal({1, 2}, bar_t{1, -2}));
  static_assert(check_equality(
    std::weak_ordering::equivalent,
    rfl::generic_compare_three_way({1, -2}, bar_t{1, 2})));

  static_assert(rfl::generic_equal(
    {bar_t{1, 2}, bar_t{3, 4}}, {bar_t{1, 2}, bar_t{3, 4}}));
  static_assert(! rfl::generic_not_equal(
    {bar_t{1, 2}, bar_t{3, 4}}, {bar_t{1, 2}, bar_t{3, 4}}));

  static_assert(! rfl::is_generic_equal_comparable_v<
    std::initializer_list<bar_t>, std::initializer_list<int>>);
  static_assert(rfl::is_generic_not_equal_comparable_v<
    bar_t, std::initializer_list<int>>);
  static_assert(! rfl::is_generic_three_way_comparable_v<
    std::initializer_list<int>, std::initializer_list<bar_t>>);
  static_assert(rfl::is_generic_three_way_comparable_v<
    std::initializer_list<int>, bar_t>);
  static_assert(! rfl::is_generic_three_way_comparable_v<bar_t, bar_t>);
  static_assert(! rfl::is_generic_three_way_comparable_v<
    std::initializer_list<bar_t>, std::initializer_list<bar_t>>);
}

TEST(TypeOperationsComparison, Arrays1)
{
  // C-style arrays
  constexpr char char_array_1[] = "HelloWorld";
  constexpr char char_array_2[] = "Hello";
  constexpr char char_array_3[] = "World";
  constexpr char char_array_4[] = "HelloWorld";
  constexpr const char* c_str_ptr = "HelloWorld";

  // Array with same contents
  static_assert(rfl::generic_equal(char_array_1, char_array_4));
  static_assert(! rfl::generic_not_equal(char_array_1, char_array_4));
  static_assert(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(char_array_1, char_array_4)));

  // Array with different contents
  static_assert(! rfl::generic_equal(char_array_1, char_array_2));
  static_assert(rfl::generic_not_equal(char_array_1, char_array_2));
  static_assert(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way(char_array_1, char_array_2)));

  static_assert(! rfl::generic_equal(char_array_2, char_array_3));
  static_assert(rfl::generic_not_equal(char_array_2, char_array_3));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(char_array_2, char_array_3)));
}

struct convertible_to_ptr_t {
  int data[4];
  constexpr operator int*() {
    return data;
  }
  constexpr operator const int*() const {
    return data;
  }
};

TEST(TypeOperationsComparison, Arrays2)
{
  // Both are arrays: range comparison
  static_assert(rfl::is_generic_equal_comparable_v<int[6], unsigned[6]>);
  static_assert(rfl::is_generic_not_equal_comparable_v<int*[6], const int*[7]>);
  // Integer vs character memberwise comparison is OK.
  static_assert(rfl::is_generic_three_way_comparable_v<int[6], wchar_t[8]>);
  // Not comparable since the elements can not be compared
  static_assert(! rfl::is_generic_not_equal_comparable_v<
    int*[6], const int64_t*[7]>);

  // Array vs. pointer (including C-style strings):
  // comparison is disabled to prevent ambiguity
  static_assert(! rfl::is_generic_equal_comparable_v<
    int8_t[6], const int8_t*>);
  static_assert(! rfl::is_generic_not_equal_comparable_v<
    const int8_t*, int8_t[6]>);
  static_assert(! rfl::is_generic_three_way_comparable_v<
    int8_t[6], const int8_t*>);

  static_assert(! rfl::is_generic_equal_comparable_v<
    char[6], const char*>);
  static_assert(! rfl::is_generic_not_equal_comparable_v<
    const char*, char[6]>);
  static_assert(! rfl::is_generic_three_way_comparable_v<
    char[6], const char*>);

  // Array vs. implicitly converted pointer: disabled as well.
  static_assert(! rfl::is_generic_equal_comparable_v<
    char[6], convertible_to_ptr_t>);
  static_assert(! rfl::generic_not_equal_comparable_with<
    convertible_to_ptr_t, char[6]>);
  static_assert(! rfl::is_generic_three_way_comparable_v<
    char[6], convertible_to_ptr_t>);
}

TEST(TypeOperationsComparison, Arrays3)
{
  unsigned arr_u[4] = {0, 1, 2, 4294967295u};
  int arr_i[4] = {0, 1, 2, -1};
  EXPECT_FALSE(rfl::generic_equal(arr_u, arr_i));
  EXPECT_TRUE(rfl::generic_not_equal(arr_u, arr_i));
  EXPECT_EQ(std::strong_ordering::greater,
            rfl::generic_compare_three_way(arr_u, arr_i));

  char str1[] = "Hello";
  char str2[] = "Hello";
  EXPECT_TRUE(rfl::generic_equal(str1, str2));
  EXPECT_FALSE(rfl::generic_not_equal(str1, str2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(str1, str2)));

  // Comparison between C-style array and std::initializer_list
  // str1 has an additional '\0'
  EXPECT_FALSE(rfl::generic_equal(str1, {'H', 'e', 'l', 'l', 'o'}));
  EXPECT_TRUE(rfl::generic_not_equal(str1, {'H', 'e', 'l', 'l', 'o'}));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way(str1, {'H', 'e', 'l', 'l', 'o'})));

  // Hint: Be careful with decaying cases in practice.
  auto with_decay = [](auto comp, auto lhs, auto rhs) {
    return comp(lhs, rhs);
  };
  EXPECT_FALSE(with_decay(rfl::generic_equal, str1, str2));
  EXPECT_TRUE(with_decay(rfl::generic_not_equal, str1, str2));
  EXPECT_TRUE(check_inequality(
    std::strong_ordering::equal,
    with_decay(rfl::generic_compare_three_way, str1, str2)));
}

TEST(TypeOperationsComparison, Arrays4)
{
  // Nested C-style arrays
  int arr_1[2][3][4];
  int arr_2[2][3][4];
  std::fill_n(reinterpret_cast<int*>(arr_1), 24, 123);
  std::fill_n(reinterpret_cast<int*>(arr_2), 24, 123);

  EXPECT_TRUE(rfl::generic_equal(arr_1, arr_2));
  EXPECT_FALSE(rfl::generic_not_equal(arr_1, arr_2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(arr_1, arr_2)));

  arr_2[1][1][1] += 100;
  EXPECT_FALSE(rfl::generic_equal(arr_1, arr_2));
  EXPECT_TRUE(rfl::generic_not_equal(arr_1, arr_2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(arr_1, arr_2)));

  // Mixture of C-style array and C++ stardard library containers
  std::array<double, 4> arr_3[2][3];
  std::vector<double> arr_4[3][2];
  std::fill_n(reinterpret_cast<double*>(arr_3), 24, 1.25);
  std::fill_n(reinterpret_cast<std::vector<double>*>(arr_4),
              4, std::vector(4zU, 1.25));
  // Note: arr_3 and arr_4 are always unequal due to different shape.
  EXPECT_FALSE(rfl::generic_equal(arr_3, arr_4));
  EXPECT_TRUE(rfl::generic_not_equal(arr_3, arr_4));
  // Let x = The first element of arr_3 which is std::array<double, 4>[3]
  //     y = The first element of arr_4 which is std::vector<double>[2]
  //     x[0] is equal to y[0], x1 is equal to y[1]. Then x is greater than y
  //     due to different size.
  EXPECT_TRUE(check_equality(
    std::partial_ordering::greater,
    rfl::generic_compare_three_way(arr_3, arr_4)));

  arr_4[1][1][1] += 1.25;
  EXPECT_FALSE(rfl::generic_equal(arr_3, arr_4));
  EXPECT_TRUE(rfl::generic_not_equal(arr_3, arr_4));
  // Change in arr_4[1] makes no effect since arr_3[0] > arr_4[0] (see above)
  EXPECT_TRUE(check_equality(
    std::partial_ordering::greater,
    rfl::generic_compare_three_way(arr_3, arr_4)));

  arr_4[0][1].push_back(2.5);
  EXPECT_FALSE(rfl::generic_equal(arr_3, arr_4));
  EXPECT_TRUE(rfl::generic_not_equal(arr_3, arr_4));
  // Now x[1] < y[1] due to different size
  EXPECT_TRUE(check_equality(
    std::partial_ordering::less,
    rfl::generic_compare_three_way(arr_3, arr_4)));

  arr_4[0][1][2] = nan_v<double>;
  EXPECT_FALSE(rfl::generic_equal(arr_3, arr_4));
  EXPECT_TRUE(rfl::generic_not_equal(arr_3, arr_4));
  // Now x[1] <=> y[1] are unordered due to NaN
  EXPECT_TRUE(check_equality(
    std::partial_ordering::unordered,
    rfl::generic_compare_three_way(arr_3, arr_4)));
}

TEST(TypeOperationsComparison, Strings1)
{
  auto s1 = std::string{"Hello World"};
  auto s2 = u"Peace Be Upon Him";
  // Comparison between std::basic_string<CharT> and const CharU*
  // as if const CharU* is converted to std::basic_string_view<CharU>.
  EXPECT_FALSE(rfl::generic_equal(s1, s2));
  EXPECT_TRUE(rfl::generic_not_equal(s1, s2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(s1, s2)));

  auto s3 = std::wstring_view{L"Hello World"};
  // Between std::basic_string<CharT> and std::basic_string_view<CharU>
  // as range memberwise comparison
  EXPECT_TRUE(rfl::generic_equal(s1, s3));
  EXPECT_FALSE(rfl::generic_not_equal(s1, s3));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(s1, s3)));

  auto s4 = std::u16string{u"Peace Be Upon Him"};
  auto s5 = s4.data();
  // Comparison between const CharT* and CharT*
  // Note: comparison by address which is semantically error-prone.
  EXPECT_FALSE(rfl::generic_equal(s2, s5));
  EXPECT_TRUE(rfl::generic_not_equal(s2, s5));

  // Comparison between CharT* and CharU* is disabled,
  // analogous to C and C++ syntax rule.
  static_assert(! rfl::generic_equal_comparable_with<char*, wchar_t*>);
  static_assert(! rfl::generic_three_way_comparable_with<
    const char8_t*, const char16_t*>);

  // Comparison between CharT* and CharT[] is disabled
  // due to semantic ambiguity.
  static_assert(! rfl::generic_not_equal_comparable_with<
    const char*, char[16]>);

  // volatile CharT* is not considered C-style string.
  static_assert(rfl::generic_equal_comparable_with<
    const char16_t*, std::u16string_view>);
  static_assert(! rfl::generic_equal_comparable_with<
    volatile char16_t*, std::u16string_view>);
}

TEST(TypeOperationsComparison, Strings2)
{
  auto s1 = std::u16string{u"你好，世界"};
  auto s2 = static_cast<const char16_t*>(nullptr);
  // (const CharT*) nullptr is condidered as empty string.
  EXPECT_FALSE(rfl::generic_equal(s1, s2));
  EXPECT_TRUE(rfl::generic_not_equal(s1, s2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way(s1, s2)));

  auto s3 = std::u16string_view{}; // Empty as well
  EXPECT_TRUE(rfl::generic_equal(s2, s3));
  EXPECT_FALSE(rfl::generic_not_equal(s2, s3));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(s2, s3)));

  // Comparison by pointer: nullptr == nullptr
  auto s4 = static_cast<const char16_t*>(nullptr);
  EXPECT_TRUE(rfl::generic_equal(s2, s4));
  EXPECT_FALSE(rfl::generic_not_equal(s2, s4));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(s2, s4)));

  // Nested comparison in ranges
  auto a1 = std::array<std::u8string, 3>{u8"ni ai wo", u8"", u8"wo ai ni"};
  const char* a2[] = {"ni ai wo", nullptr, "wo ai ni"};
  EXPECT_TRUE(rfl::generic_equal(a1, a2));
  EXPECT_FALSE(rfl::generic_not_equal(a1, a2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(a1, a2)));

  a2[2] = nullptr;
  EXPECT_FALSE(rfl::generic_equal(a1, a2));
  EXPECT_TRUE(rfl::generic_not_equal(a1, a2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way(a1, a2)));

  a1[2] = u8"";
  EXPECT_TRUE(rfl::generic_equal(a1, a2));
  EXPECT_FALSE(rfl::generic_not_equal(a1, a2));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(a1, a2)));
}

TEST(TypeOperationsComparison, TupleLike)
{
  constexpr auto constant_1 = rfl::constant<1, 2, 3>{};
  constexpr auto constant_2 = rfl::constant<1, 2.0, 3u>{};
  constexpr auto constant_3 = rfl::constant<1, 2, 4294967299>{}; // 2 ** 32 + 3
  constexpr auto constant_4 = rfl::constant<1, 2, 3, 4>{};

  static_assert(rfl::generic_equal(constant_1, constant_2));
  static_assert(! rfl::generic_not_equal(constant_1, constant_2));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way(constant_1, constant_2)));

  static_assert(! rfl::generic_equal(constant_1, constant_3));
  static_assert(rfl::generic_not_equal(constant_1, constant_3));
  static_assert(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(constant_1, constant_3)));

  // Tuple-like with different sizes are not comparable
  static_assert(rfl::generic_equal_comparable_with<
    decltype(constant_1), decltype(constant_2)>);
  static_assert(! rfl::is_generic_equal_comparable_v<
    decltype(constant_1), decltype(constant_4)>);
  static_assert(! rfl::generic_not_equal_comparable_with<
    decltype(constant_1), decltype(constant_4)>);
  static_assert(! rfl::is_generic_three_way_comparable<
    decltype(constant_1), decltype(constant_4)>::value);

  constexpr auto std_tuple_1 = std::tuple{1.0, 2, 3u};
  static_assert(rfl::generic_equal(constant_1, std_tuple_1));
  static_assert(! rfl::generic_not_equal(constant_1, std_tuple_1));
  static_assert(check_equality(
    std::partial_ordering::equivalent,
    rfl::generic_compare_three_way(constant_1, std_tuple_1)));
}

TEST(TypeOperationsComparison, NestedRanges)
{
  auto vec_list = std::vector{
    std::list{3, 2, 1},
    std::list{6, 5, 4},
    std::list{9, 8, 7}
  };
  auto deq_set = std::deque{
    std::set{3, 2, 1},
    std::set{6, 5, 4},
    std::set{9, 8, 7}
  };
  EXPECT_FALSE(rfl::generic_equal(vec_list, deq_set));
  EXPECT_TRUE(rfl::generic_not_equal(vec_list, deq_set));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way(vec_list, deq_set)));

  std::deque<unsigned> carr_deq[] = {
    {3, 2, 1},
    {6, 5, 4},
    {9, 8, 7}
  };
  EXPECT_TRUE(rfl::generic_equal(vec_list, carr_deq));
  EXPECT_FALSE(rfl::generic_not_equal(vec_list, carr_deq));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(vec_list, carr_deq)));

  // With std::initializer_list
#define IL_VEC { std::vector{3.0, 2.0, 1.0}, std::vector{6.0, 5.0, 4.0} }
  EXPECT_FALSE(rfl::generic_equal(vec_list, IL_VEC));
  EXPECT_TRUE(rfl::generic_not_equal(vec_list, IL_VEC));
  EXPECT_TRUE(check_equality(
    std::partial_ordering::greater,
    rfl::generic_compare_three_way(vec_list, IL_VEC)));
#undef IL_VEC
}

TEST(TypeOperationsComparison, NestedRangesWithStr)
{
  using namespace std::literals;
  auto vec_list = std::vector{
    std::list{"aaa", "bbb", "ccc"},
    std::list{"xxx", "yyy", "zzz"},
  };
  auto deq_set = std::deque{
    std::set{"aaa"s, "bbb"s, "ccc"s},
    std::set{"xxx"s, "yyy"s, "zz"s},
  };
  EXPECT_FALSE(rfl::generic_equal(vec_list, deq_set));
  EXPECT_TRUE(rfl::generic_not_equal(vec_list, deq_set));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way(vec_list, deq_set)));

  std::deque<std::string_view> carr_deq[] = {
    {"aaa", "bbb", "ccc"},
    {"xxx", "yyy", "zzz"},
  };
  EXPECT_TRUE(rfl::generic_equal(vec_list, carr_deq));
  EXPECT_FALSE(rfl::generic_not_equal(vec_list, carr_deq));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(vec_list, carr_deq)));

  // With std::initializer_list
#define IL_VEC {                          \
    std::vector{"aaa"s, "bbb"s, "ccc"s},  \
    std::vector{"xxx"s, "yyy"s, "zzz"s},  \
    std::vector{"abc"s}                   \
  }
  EXPECT_FALSE(rfl::generic_equal(vec_list, IL_VEC));
  EXPECT_TRUE(rfl::generic_not_equal(vec_list, IL_VEC));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(vec_list, IL_VEC)));
#undef IL_VEC
}

TEST(TypeOperationsComparison, NestedTupleLike1)
{
  using std_tuple_ids = std::tuple<int, double, std::wstring>;
  using meta_tuple_ufc = rfl::meta_tuple<unsigned, float, const wchar_t*>;
  auto vec_set_1 = std::vector{
    std::set{std_tuple_ids{1, 2.0, L"3"},
             std_tuple_ids{4, 5.5, L"6"}},
    std::set{std_tuple_ids{-1, -2.0, L"-3"},
             std_tuple_ids{-4, nan_v<double>, L"-6"}},
  };
  auto deq_list_1 = std::deque{
    std::list{meta_tuple_ufc{1u, 2.0f, L"3"},
              meta_tuple_ufc{4u, 5.5f, L"6"}},
    std::list{meta_tuple_ufc{-4u, nan_v<double>, L"-6"},
              meta_tuple_ufc{-1u, -2.0f, L"-3"}},
  };
  EXPECT_FALSE(rfl::generic_equal(vec_set_1, deq_list_1));
  EXPECT_TRUE(rfl::generic_not_equal(vec_set_1, deq_list_1));
  EXPECT_TRUE(check_equality(
    std::partial_ordering::less, // cmp_less(-4, -4u)
    rfl::generic_compare_three_way(vec_set_1, deq_list_1)));

  auto vec_set_2 = std::vector{
    std::set{std_tuple_ids{1, 2.0, L"3"},
             std_tuple_ids{4, nan_v<double>, L"6"}},
  };
  std::list<meta_tuple_ufc> carr_list_2[] = {
    std::list{meta_tuple_ufc{1u, 2.0f, L"3"},
              meta_tuple_ufc{4u, nan_v<double>, L"6"}},
  };
  EXPECT_FALSE(rfl::generic_equal(vec_set_2, carr_list_2));
  EXPECT_TRUE(rfl::generic_not_equal(vec_set_2, carr_list_2));
  EXPECT_TRUE(check_equality(
    std::partial_ordering::unordered,
    rfl::generic_compare_three_way(vec_set_2, carr_list_2)));
}

TEST(TypeOperationsComparison, NestedTupleLike2)
{
  using namespace std::literals;
  using std_tuple = std::tuple<
    int, std::string, std::vector<std::string>>;
  using meta_tuple = rfl::meta_tuple<
    uint64_t, const char*, std::deque<std::string_view>>;

  auto list_std_tuple = std::list{
    std_tuple{1, "hello", {"aa", "bb", "cc"}},
    std_tuple{2, "world", {"xx", "yy", "zz"}}
  };
  auto vector_meta_tuple = std::vector{
    meta_tuple{1u, "hello", {"aa"sv, "bb"sv, "cc"sv}},
    meta_tuple{2u, "world", {"xx"sv, "yy"sv, "zz"sv}}
  };
  EXPECT_TRUE(rfl::generic_equal(list_std_tuple, vector_meta_tuple));
  EXPECT_FALSE(rfl::generic_not_equal(list_std_tuple, vector_meta_tuple));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::equal,
    rfl::generic_compare_three_way(list_std_tuple, vector_meta_tuple)));

  get<0>(vector_meta_tuple[1]) += 1uLL << 32;
  EXPECT_FALSE(rfl::generic_equal(list_std_tuple, vector_meta_tuple));
  EXPECT_TRUE(rfl::generic_not_equal(list_std_tuple, vector_meta_tuple));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::less,
    rfl::generic_compare_three_way(list_std_tuple, vector_meta_tuple)));

  meta_tuple carr_meta_tuple[] = {
    meta_tuple{1u, "hello", {"aa"sv, "bb"sv, "cc"sv}},
    meta_tuple{2u, "world", {"xx"sv, "yy"sv, "zz"sv, "xyz"sv}}
  };
  EXPECT_FALSE(rfl::generic_equal(carr_meta_tuple, list_std_tuple));
  EXPECT_TRUE(rfl::generic_not_equal(carr_meta_tuple, list_std_tuple));
  EXPECT_TRUE(check_equality(
    std::strong_ordering::greater,
    rfl::generic_compare_three_way(carr_meta_tuple, list_std_tuple)));
}
