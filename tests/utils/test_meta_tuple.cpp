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

#include <ranges>
#include <reflect_cpp26/utils/functional_tuple.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

TEST(UtilsMetaTuple, Small) {
  using tuple_type = rfl::meta_tuple<char, int, double>;
  static_assert(rfl::tuple_like<tuple_type>);
  static_assert(std::tuple_size_v<tuple_type> == 3);
  static_assert(std::is_same_v<std::tuple_element_t<0, tuple_type>, char>);
  static_assert(std::is_same_v<std::tuple_element_t<1, tuple_type>, int>);
  static_assert(std::is_same_v<std::tuple_element_t<2, tuple_type>, double>);

  // CTAD
  auto t = rfl::meta_tuple{'0', 123, 2.5};
  static_assert(std::is_same_v<decltype(t), tuple_type>);
  // Free get()
  EXPECT_EQ('0', get<0>(t));
  EXPECT_EQ(123, get<1>(t));
  EXPECT_EQ(2.5, get<2>(t));
  // Direct access via field name
  EXPECT_EQ('0', t.elements.first);
  EXPECT_EQ(123, t.elements.second);
  EXPECT_EQ(2.5, t.elements.third);

  get<0>(t) += 5;
  EXPECT_EQ('5', t.elements.first);

  // Structured binding
  auto [x, y, z] = t;
  EXPECT_EQ('5', x);
  EXPECT_EQ(123, y);
  EXPECT_EQ(2.5, z);
}

TEST(UtilsMetaTuple, Large) {
  using tuple_type = rfl::meta_tuple<bool, char, int, float, long, double>;
  static_assert(rfl::tuple_like<tuple_type>);
  static_assert(std::tuple_size_v<tuple_type> == 6);
  static_assert(std::is_same_v<std::tuple_element_t<0, tuple_type>, bool>);
  static_assert(std::is_same_v<std::tuple_element_t<1, tuple_type>, char>);
  static_assert(std::is_same_v<std::tuple_element_t<2, tuple_type>, int>);
  static_assert(std::is_same_v<std::tuple_element_t<3, tuple_type>, float>);
  static_assert(std::is_same_v<std::tuple_element_t<4, tuple_type>, long>);
  static_assert(std::is_same_v<std::tuple_element_t<5, tuple_type>, double>);

  // CTAD
  auto t = tuple_type{true, 'A', 4, 1.25f, 10L, 3.25};
  static_assert(std::is_same_v<decltype(t), tuple_type>);
  // Free get()
  EXPECT_TRUE(get<0>(t));
  EXPECT_EQ('A', get<1>(t));
  EXPECT_EQ(4, get<2>(t));
  EXPECT_EQ(1.25f, get<3>(t));
  EXPECT_EQ(10L, get<4>(t));
  EXPECT_EQ(3.25, get<5>(t));

  get<1>(t) += 7;
  EXPECT_EQ('H', t.elements.v2);

  // Structured binding
  auto [b, c, i, f, l, d] = t;
  EXPECT_TRUE(b);
  EXPECT_EQ('H', c);
  EXPECT_EQ(4, i);
  EXPECT_EQ(1.25f, f);
  EXPECT_EQ(10L, l);
  EXPECT_EQ(3.25, d);
}

TEST(UtilsMetaTuple, Comparison) {
  // Test equal tuples
  constexpr auto t1 = rfl::meta_tuple{1, 2, 3};
  constexpr auto t2 = rfl::meta_tuple{1, 2, 3};
  EXPECT_TRUE_STATIC(rfl::elementwise_equal(t1, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_not_equal(t1, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_less(t1, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_less_equal(t1, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_greater(t1, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_greater_equal(t1, t2));
  EXPECT_EQ_STATIC(rfl::elementwise_compare_three_way(t1, t2), std::partial_ordering::equivalent);

  // Test tuple with all elements smaller
  constexpr auto t3 = rfl::meta_tuple{0, 1, 2};
  EXPECT_FALSE_STATIC(rfl::elementwise_equal(t3, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_not_equal(t3, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_less(t3, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_less_equal(t3, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_greater(t3, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_greater_equal(t3, t2));

  // Test tuple with all elements larger
  constexpr auto t4 = rfl::meta_tuple{2, 3, 4};
  EXPECT_FALSE_STATIC(rfl::elementwise_equal(t4, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_not_equal(t4, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_less(t4, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_less_equal(t4, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_greater(t4, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_greater_equal(t4, t2));

  // Test tuple with mixed elements (not all less)
  constexpr auto t5 = rfl::meta_tuple{0, 1, 4};
  EXPECT_FALSE_STATIC(rfl::elementwise_equal(t5, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_not_equal(t5, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_less(t5, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_greater(t5, t2));

  // Test tuple with mixed elements (not all greater)
  constexpr auto t6 = rfl::meta_tuple{2, 3, 2};
  EXPECT_FALSE_STATIC(rfl::elementwise_equal(t6, t2));
  EXPECT_TRUE_STATIC(rfl::elementwise_not_equal(t6, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_less(t6, t2));
  EXPECT_FALSE_STATIC(rfl::elementwise_greater(t6, t2));

  // Test with different types (but comparable)
  constexpr auto t7 = rfl::meta_tuple{1, 2, 3.0};
  constexpr auto t8 = rfl::meta_tuple{1, 2, 3};
  EXPECT_TRUE_STATIC(rfl::elementwise_equal(t7, t8));
  EXPECT_FALSE_STATIC(rfl::elementwise_not_equal(t7, t8));
}

TEST(UtilsMetaTuple, CompareThreeWay) {
  // Test equivalent tuples
  constexpr auto t1 = rfl::meta_tuple{1, 2, 3};
  constexpr auto t2 = rfl::meta_tuple{1, 2, 3};
  EXPECT_EQ_STATIC(rfl::elementwise_compare_three_way(t1, t2), std::partial_ordering::equivalent);

  // Test less than
  constexpr auto t3 = rfl::meta_tuple{1, 2, 2};
  EXPECT_EQ_STATIC(rfl::elementwise_compare_three_way(t3, t2), std::partial_ordering::less);

  // Test greater than
  constexpr auto t4 = rfl::meta_tuple{1, 2, 4};
  EXPECT_EQ_STATIC(rfl::elementwise_compare_three_way(t4, t2), std::partial_ordering::greater);

  // Test unordered (mixed order)
  constexpr auto t5 = rfl::meta_tuple{1, 3, 2};
  EXPECT_EQ_STATIC(rfl::elementwise_compare_three_way(t5, t2), std::partial_ordering::unordered);
}

TEST(UtilsMetaTuple, ComparisonOperators) {
  // Test equality operator
  constexpr auto t1 = rfl::meta_tuple{1, 2, 3};
  constexpr auto t2 = rfl::meta_tuple{1, 2, 3};
  constexpr auto t3 = rfl::meta_tuple{1, 2, 4};
  EXPECT_TRUE_STATIC(t1 == t2);
  EXPECT_FALSE_STATIC(t1 == t3);
  EXPECT_FALSE_STATIC(t1 != t2);
  EXPECT_TRUE_STATIC(t1 != t3);

  // Test comparison operators
  constexpr auto t4 = rfl::meta_tuple{1, 1, 3};
  constexpr auto t5 = rfl::meta_tuple{1, 2, 2};
  constexpr auto t6 = rfl::meta_tuple{1, 2, 4};
  constexpr auto t7 = rfl::meta_tuple{1, 3, 3};

  // Less than
  EXPECT_TRUE_STATIC(t4 < t1);
  EXPECT_TRUE_STATIC(t5 < t1);
  EXPECT_FALSE_STATIC(t1 < t4);
  EXPECT_FALSE_STATIC(t1 < t1);

  // Less than or equal
  EXPECT_TRUE_STATIC(t4 <= t1);
  EXPECT_TRUE_STATIC(t5 <= t1);
  EXPECT_TRUE_STATIC(t1 <= t1);
  EXPECT_FALSE_STATIC(t1 <= t4);

  // Greater than
  EXPECT_TRUE_STATIC(t6 > t1);
  EXPECT_TRUE_STATIC(t7 > t1);
  EXPECT_FALSE_STATIC(t1 > t6);
  EXPECT_FALSE_STATIC(t1 > t1);

  // Greater than or equal
  EXPECT_TRUE_STATIC(t6 >= t1);
  EXPECT_TRUE_STATIC(t7 >= t1);
  EXPECT_TRUE_STATIC(t1 >= t1);
  EXPECT_FALSE_STATIC(t1 >= t6);

  // Test with different types (but comparable)
  constexpr auto t8 = rfl::meta_tuple{1, 2, 3.0};
  EXPECT_TRUE_STATIC(t1 == t8);
  EXPECT_FALSE_STATIC(t1 != t8);
  EXPECT_FALSE_STATIC(t1 < t8);
  EXPECT_TRUE_STATIC(t1 <= t8);
  EXPECT_FALSE_STATIC(t1 > t8);
  EXPECT_TRUE_STATIC(t1 >= t8);
}

TEST(UtilsMetaTuple, DifferentTypesComparison) {
  // Test int vs long
  constexpr auto t1 = rfl::meta_tuple{1, 2, 3};
  constexpr auto t2 = rfl::meta_tuple{1L, 2L, 3L};
  EXPECT_TRUE_STATIC(t1 == t2);
  EXPECT_FALSE_STATIC(t1 != t2);
  EXPECT_FALSE_STATIC(t1 < t2);
  EXPECT_TRUE_STATIC(t1 <= t2);
  EXPECT_FALSE_STATIC(t1 > t2);
  EXPECT_TRUE_STATIC(t1 >= t2);

  // Test float vs double
  constexpr auto t3 = rfl::meta_tuple{1.0f, 2.0f, 3.0f};
  constexpr auto t4 = rfl::meta_tuple{1.0, 2.0, 3.0};
  EXPECT_TRUE_STATIC(t3 == t4);
  EXPECT_FALSE_STATIC(t3 != t4);
  EXPECT_FALSE_STATIC(t3 < t4);
  EXPECT_TRUE_STATIC(t3 <= t4);
  EXPECT_FALSE_STATIC(t3 > t4);
  EXPECT_TRUE_STATIC(t3 >= t4);

  // Test mixed types
  constexpr auto t5 = rfl::meta_tuple{1, 2.0f, 3L};
  constexpr auto t6 = rfl::meta_tuple{1L, 2.0, 3};
  EXPECT_TRUE_STATIC(t5 == t6);
  EXPECT_FALSE_STATIC(t5 != t6);

  // Test different types with different values
  constexpr auto t7 = rfl::meta_tuple{1, 2, 3};
  constexpr auto t8 = rfl::meta_tuple{1L, 2L, 4L};
  EXPECT_FALSE_STATIC(t7 == t8);
  EXPECT_TRUE_STATIC(t7 != t8);
  EXPECT_TRUE_STATIC(t7 < t8);
  EXPECT_TRUE_STATIC(t7 <= t8);
  EXPECT_FALSE_STATIC(t7 > t8);
  EXPECT_FALSE_STATIC(t7 >= t8);
}

TEST(UtilsMetaTuple, CompareWithStdTuple) {
  // Test equality
  constexpr auto mt1 = rfl::meta_tuple{1, 2, 3};
  constexpr std::tuple<int, int, int> st1{1, 2, 3};
  EXPECT_TRUE_STATIC(mt1 == st1);
  EXPECT_FALSE_STATIC(mt1 != st1);

  // Test different values
  constexpr std::tuple<int, int, int> st2{1, 2, 4};
  EXPECT_FALSE_STATIC(mt1 == st2);
  EXPECT_TRUE_STATIC(mt1 != st2);
  EXPECT_TRUE_STATIC(mt1 < st2);
  EXPECT_TRUE_STATIC(mt1 <= st2);
  EXPECT_FALSE_STATIC(mt1 > st2);
  EXPECT_FALSE_STATIC(mt1 >= st2);

  // Test different types
  constexpr auto mt2 = rfl::meta_tuple{1, 2.0, 3L};
  constexpr std::tuple<long, double, int> st3{1L, 2.0, 3};
  EXPECT_TRUE_STATIC(mt2 == st3);
  EXPECT_FALSE_STATIC(mt2 != st3);

  // Test with different sizes (should not compile, but let's test with same size)
  constexpr auto mt3 = rfl::meta_tuple{1, 2, 3, 4};
  constexpr std::tuple<int, int, int, int> st4{1, 2, 3, 4};
  EXPECT_TRUE_STATIC(mt3 == st4);
  EXPECT_FALSE_STATIC(mt3 != st4);
}

TEST(UtilsMetaTuple, RangeAdaptors) {
  // Create a range of meta_tuple
  std::vector<rfl::meta_tuple<int, int>> tuples = {{1, 2}, {3, 4}, {4, 6}, {7, 8}, {8, 10}};

  // Test views::filter
  auto filtered = tuples | std::views::filter([](const auto& t) { return get<0>(t) % 2 != 0; });
  EXPECT_EQ(std::ranges::distance(filtered), 3);
  // Verify filtered elements
  std::vector<rfl::meta_tuple<int, int>> filtered_result;
  std::ranges::copy(filtered, std::back_inserter(filtered_result));
  EXPECT_EQ(filtered_result[0], tuples[0]);
  EXPECT_EQ(filtered_result[1], tuples[1]);
  EXPECT_EQ(filtered_result[2], tuples[3]);

  // Test views::transform
  auto transformed = tuples | std::views::transform([](const auto& t) {
                       return rfl::meta_tuple{get<0>(t) * 2, get<1>(t) * 2};
                     });
  std::vector<rfl::meta_tuple<int, int>> transformed_expected = {
      {2, 4}, {6, 8}, {10, 12}, {14, 16}, {18, 20}};
  EXPECT_EQ(std::ranges::distance(transformed), 5);
  // Verify transformed elements
  std::vector<rfl::meta_tuple<int, int>> transformed_result;
  std::ranges::copy(transformed, std::back_inserter(transformed_result));
  EXPECT_EQ(transformed_result, transformed_expected);

  // Test views::take
  auto taken = tuples | std::views::take(3);
  EXPECT_EQ(std::ranges::distance(taken), 3);
  // Verify taken elements
  std::vector<rfl::meta_tuple<int, int>> taken_result;
  std::ranges::copy(taken, std::back_inserter(taken_result));
  EXPECT_EQ(taken_result[0], tuples[0]);
  EXPECT_EQ(taken_result[1], tuples[1]);
  EXPECT_EQ(taken_result[2], tuples[2]);

  // Test views::drop
  auto dropped = tuples | std::views::drop(2);
  EXPECT_EQ(std::ranges::distance(dropped), 3);
  // Verify dropped elements
  std::vector<rfl::meta_tuple<int, int>> dropped_result;
  std::ranges::copy(dropped, std::back_inserter(dropped_result));
  EXPECT_EQ(dropped_result[0], tuples[2]);
  EXPECT_EQ(dropped_result[1], tuples[3]);
  EXPECT_EQ(dropped_result[2], tuples[4]);

  // Test views::reverse
  auto reversed = tuples | std::views::reverse;
  EXPECT_EQ(std::ranges::distance(reversed), 5);
  // Verify reversed elements
  std::vector<rfl::meta_tuple<int, int>> reversed_result;
  std::ranges::copy(reversed, std::back_inserter(reversed_result));
  EXPECT_EQ(reversed_result[0], tuples[4]);
  EXPECT_EQ(reversed_result[1], tuples[3]);
  EXPECT_EQ(reversed_result[2], tuples[2]);
  EXPECT_EQ(reversed_result[3], tuples[1]);
  EXPECT_EQ(reversed_result[4], tuples[0]);

  // Test sorting (using std::ranges::sort)
  auto sorted_tuples = tuples;
  std::ranges::sort(sorted_tuples,
                    [](const auto& a, const auto& b) { return get<0>(a) > get<0>(b); });
  EXPECT_EQ(sorted_tuples.size(), 5);
  // Verify sorted elements
  EXPECT_EQ(sorted_tuples[0], (rfl::meta_tuple{9, 10}));
  EXPECT_EQ(sorted_tuples[1], (rfl::meta_tuple{7, 8}));
  EXPECT_EQ(sorted_tuples[2], (rfl::meta_tuple{5, 6}));
  EXPECT_EQ(sorted_tuples[3], (rfl::meta_tuple{3, 4}));
  EXPECT_EQ(sorted_tuples[4], (rfl::meta_tuple{1, 2}));

  // Test views::zip with another range
  std::vector<int> values = {10, 20, 30, 40, 50};
  auto zipped = std::views::zip(tuples, values);
  EXPECT_EQ(std::ranges::distance(zipped), 5);
  // Verify zipped elements
  int i = 0;
  for (const auto& [t, v] : zipped) {
    EXPECT_EQ(t, tuples[i]);
    EXPECT_EQ(v, values[i]);
    ++i;
  }

  // Test views::enumerate
  auto enumerated = tuples | std::views::enumerate;
  EXPECT_EQ(std::ranges::distance(enumerated), 5);
  // Verify enumerated elements
  i = 0;
  for (const auto& [idx, t] : enumerated) {
    EXPECT_EQ(idx, i);
    EXPECT_EQ(t, tuples[i]);
    ++i;
  }
}

TEST(UtilsMetaTuple, RangeAdaptorsWithDifferentTypes) {
  // Create a range of meta_tuple with different types
  std::vector<rfl::meta_tuple<int, double, std::string>> tuples = {
      {1, 1.1, "one"}, {2, 2.2, "two"}, {3, 3.3, "three"}};

  // Test views::filter with different types
  auto filtered = tuples | std::views::filter([](const auto& t) { return get<0>(t) > 1; });
  EXPECT_EQ(std::ranges::distance(filtered), 2);
  // Verify filtered elements
  std::vector<rfl::meta_tuple<int, double, std::string>> filtered_result;
  std::ranges::copy(filtered, std::back_inserter(filtered_result));
  EXPECT_EQ(filtered_result[0], tuples[1]);
  EXPECT_EQ(filtered_result[1], tuples[2]);

  // Test views::transform with type conversion
  auto transformed =
      tuples | std::views::transform([](const auto& t) {
        return rfl::meta_tuple{get<0>(t), get<1>(t) + 1.0, get<2>(t) + "_transformed"};
      });
  EXPECT_EQ(std::ranges::distance(transformed), 3);
  // Verify transformed elements
  std::vector<rfl::meta_tuple<int, double, std::string>> transformed_result;
  std::ranges::copy(transformed, std::back_inserter(transformed_result));
  EXPECT_EQ(transformed_result[0], (rfl::meta_tuple{1, 2.1, "one_transformed"}));
  EXPECT_EQ(transformed_result[1], (rfl::meta_tuple{2, 3.2, "two_transformed"}));
  EXPECT_EQ(transformed_result[2], (rfl::meta_tuple{3, 4.3, "three_transformed"}));
}
