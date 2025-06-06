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
#include <reflect_cpp26/utils/constant.hpp>
#include <format>

namespace rfl = reflect_cpp26;

template <class T, size_t N>
constexpr auto sum(const std::array<T, N>& values) -> T {
  auto res = T{};
  for (auto x: values) {
    res += x;
  }
  return res;
}

TEST(UtilsConstant, Basic)
{
  auto c1 = rfl::constant<12>{};
  EXPECT_EQ_STATIC(12, c1.value); // rfl::constant<12>::value
  EXPECT_EQ_STATIC(12, sum(c1.values)); // rfl::constant<12>::values
  EXPECT_EQ_STATIC(12, c1); // Implicit conversion
}

TEST(UtilsConstant, Reduce)
{
  auto c2 = rfl::constant<1u, 2u, 3u, 5u, 8u>{};
  EXPECT_EQ_STATIC(19u, sum(c2.values));
  // (1) Implicit conversion
  constexpr auto c2_reducer_1 = [](unsigned dest, unsigned x) {
    return dest * 10 + x;
  };
  constexpr auto c2_reduced_1 = c2.reduce<c2_reducer_1, 0u>();
  static_assert(std::is_same_v<decltype(c2_reduced_1), const unsigned>);
  EXPECT_EQ_STATIC(12358u, c2_reduced_1);

  // (2) Static initial value
  constexpr auto c2_reducer_2 = [](auto dest, auto x) {
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(dest), rfl::constant>);
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(x), rfl::constant>);
    static_assert(std::is_same_v<decltype(dest.value), const unsigned>);
    static_assert(std::is_same_v<decltype(x.value), const unsigned>);
    return dest * 100 + x;
  };
  constexpr auto c2_reduced_2 = c2.reduce<c2_reducer_2, 0u>();
  static_assert(std::is_same_v<decltype(c2_reduced_2), const unsigned>);
  EXPECT_EQ_STATIC(102030508u, c2_reduced_2);

  // (3) Dynamic initial value
  constexpr auto c2_reducer_3 = [](auto dest, auto x) {
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(x), rfl::constant>);
    static_assert(std::is_same_v<decltype(x.value), const unsigned>);
    static_assert(std::is_same_v<decltype(dest), unsigned>);
    return dest * 10 + (10 - x);
  };
  EXPECT_EQ_STATIC(998752u, c2.reduce(c2_reducer_3, 9u));
}

TEST(UtilsConstant, ForEach)
{
  auto c2 = rfl::constant<1u, 2u, 3u, 5u, 8u>{};
  auto str = std::string{};
  // (1) func(value)
  c2.for_each([&str](unsigned x) {
    str += std::to_string(x);
    str += ",";
  });
  EXPECT_EQ("1,2,3,5,8,", str);
  str.clear();
  // (2) func(index, value)
  c2.for_each([&str](size_t i, unsigned x) {
    str += std::format("[{}]={},", i, x);
  });
  EXPECT_EQ("[0]=1,[1]=2,[2]=3,[3]=5,[4]=8,", str);
  str.clear();
  // (3) func(index, value) with stop token
  c2.for_each([&str](size_t i, unsigned x) {
    str += std::format("[{}]={},", i, x);
    return x <= 2;
  });
  EXPECT_EQ("[0]=1,[1]=2,[2]=3,", str);
}

TEST(UtilsConstant, Map)
{
  auto c2 = rfl::constant<1u, 2u, 3u, 5u, 8u>{};
  // (1) Implicit conversion
  constexpr auto c2_mapper_1 = [](unsigned x) {
    return x / 8.0;
  };
  auto c2_mapped_1 = c2.map<c2_mapper_1>();
  EXPECT_EQ_STATIC(2.375, sum(c2_mapped_1.values));
  EXPECT_EQ_STATIC(2.375, sum(c2.map(c2_mapper_1)));

  // (2) No implicit conversion
  constexpr auto c2_mapper_2 = [](auto x) {
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(x), rfl::constant>);
    static_assert(std::is_same_v<decltype(x.value), const unsigned>);
    return x / 4.0;
  };
  auto c2_mapped_2 = c2.map<c2_mapper_2>();
  EXPECT_EQ_STATIC(4.75, sum(c2_mapped_2.values));
  EXPECT_EQ_STATIC(4.75, sum(c2.map(c2_mapper_2)));
}

TEST(UtilsConstant, Concat)
{
  auto c2 = rfl::constant<1u, 2u, 3u, 5u, 8u>{};
  constexpr auto reducer = [](unsigned dest, unsigned x) {
    return dest * 10 + x;
  };
  auto c2_concat = c2.concat(c2);
  EXPECT_EQ_STATIC(10, c2_concat.size());
  EXPECT_EQ_STATIC(1235812358u, c2_concat.reduce(reducer, 0u));
}

TEST(UtilsConstant, Filter)
{
  auto c3 = rfl::constant<1u, 2u, 3u, 5u, 8u, 1u, 2u, 3u, 5u, 8u>{};
  constexpr auto reducer = [](unsigned dest, unsigned x) {
    return dest * 10 + x;
  };
  // (1) Static predicate with implicit conversion
  constexpr auto c3_filter_1 = [](unsigned x) {
    return x % 2 == 1;
  };
  constexpr auto c3_filtered_1 = c3.filter<c3_filter_1>();
  constexpr auto reduced_1 = c3_filtered_1.reduce<reducer, 0u>();
  EXPECT_EQ_STATIC(135135u, reduced_1);

  // (2) Static predicate without implicit conversion
  constexpr auto c3_filter_2 = [](auto x) {
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(x), rfl::constant>);
    static_assert(std::is_same_v<decltype(x.value), const unsigned>);
    return x % 2 == 0;
  };
  constexpr auto c3_filtered_2 = c3.filter<c3_filter_2>();
  constexpr auto reduced_2 = c3_filtered_2.reduce<reducer, 0u>();
  EXPECT_EQ_STATIC(2828u, reduced_2);

  // (3) Dynamic predicate without implicit conversion
  auto index = 0u;
  //    1, 2, 3, 5,  8, 1, 2,  3,  5,  8
  // => 1, 3, 5, 8, 12, 6, 8, 10, 13, 17
  EXPECT_THAT(c3.filter([&index](auto x) {
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(x), rfl::constant>);
    static_assert(std::is_same_v<decltype(x.value), const unsigned>);
    return ((index++) + x) % 3 != 0;
  }), testing::ElementsAre(1, 3, 5, 2, 3, 5, 8));
}

// index_of, last_index_of, includes
TEST(UtilsConstant, Lookup)
{
  auto c3 = rfl::constant<1u, 2u, 3u, 5u, 8u, 1u, 2u, 3u, 5u, 8u>{};
  EXPECT_EQ_STATIC(1, c3.index_of(2));
  EXPECT_EQ_STATIC(8, c3.last_index_of(5));
  EXPECT_TRUE_STATIC(c3.includes(8));
  EXPECT_EQ_STATIC(rfl::npos, c3.index_of(4));
  EXPECT_EQ_STATIC(rfl::npos, c3.last_index_of(6));
  EXPECT_FALSE_STATIC(c3.includes(9));
  // Non-comparable types
  EXPECT_EQ_STATIC(rfl::npos, c3.index_of(std::errc{}));
  EXPECT_EQ_STATIC(rfl::npos, c3.last_index_of(std::monostate{}));

  auto tags = rfl::tags_constant<
    std::monostate,
    std::remove_cvref<int&>,
    std::remove_cvref<const int&>>();
  // Tags matching
  EXPECT_EQ_STATIC(2, tags.index_of(std::remove_cvref<const int&>{}));
  EXPECT_EQ_STATIC(1, tags.last_index_of(std::remove_cvref<int&>{}));
  EXPECT_EQ_STATIC(rfl::npos, tags.index_of(std::remove_cvref<int>{}));
  EXPECT_EQ_STATIC(rfl::npos, tags.last_index_of(std::remove_cvref<int&&>{}));

  // Implicit conversion
  auto tags_misuse = rfl::tags_constant<
    std::is_integral<int>,    // -> true
    std::is_integral<float>,  // -> false
    std::is_integral<long>,   // -> true
    std::monostate>();        // Not convertible
  EXPECT_EQ_STATIC(0, tags_misuse.index_of(std::is_integral<long>{}));
  EXPECT_EQ_STATIC(1, tags_misuse.last_index_of(std::is_integral<double>{}));
}

TEST(UtilsConstant, Predicates)
{
  auto c1 = rfl::constant<2u, 3u, 7u>{};
  auto value = 42u;
  auto predicate_1 = [&value](auto x) {
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(x), rfl::constant>);
    return value % x == 0;
  };
  EXPECT_TRUE(c1.all_of(predicate_1));
  EXPECT_TRUE(c1.any_of(predicate_1));
  EXPECT_FALSE(c1.none_of(predicate_1));

  value = 26u;
  auto predicate_2 = [&value](auto index, auto x) {
    static_assert(rfl::is_nontype_template_instance_of_v<
      decltype(index), rfl::constant>);
    return (value + index) % x == 0;
  };
  EXPECT_TRUE(c1.all_of(predicate_2));
  EXPECT_TRUE(c1.any_of(predicate_2));
  EXPECT_FALSE(c1.none_of(predicate_2));

  value = 28u;
  EXPECT_FALSE(c1.all_of(predicate_2));
  EXPECT_TRUE(c1.any_of(predicate_2));
  EXPECT_FALSE(c1.none_of(predicate_2));

  value = 1u;
  EXPECT_FALSE(c1.all_of(predicate_2));
  EXPECT_FALSE(c1.any_of(predicate_2));
  EXPECT_TRUE(c1.none_of(predicate_2));
}
