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

#include <reflect_cpp26/fixed_map/integral_key.hpp>

#include "tests/fixed_map/integral_key/integral_key_test_options.hpp"

namespace rfl = reflect_cpp26;
using namespace std::literals;

struct string_wrapper {
  std::string_view string;
};

template <>
struct std::tuple_size<string_wrapper> : std::integral_constant<size_t, 2> {};

template <>
struct std::tuple_element<0, string_wrapper> {
  using type = char;
};
template <>
struct std::tuple_element<1, string_wrapper> {
  using type = std::string_view;
};

template <size_t I>
constexpr auto get(string_wrapper s) {
  if constexpr (I == 0) {
    return s.string[0];
  } else if constexpr (I == 1) {
    return s.string;
  } else {
    static_assert(false, "Invalid index.");
  }
}

TEST(FixedMap, IntegralKeyCustomKVPair1) {
  constexpr auto make_kv_pairs = []() consteval {
    return std::vector<string_wrapper>({
        {"Apple"},
        {"Banana"},
        {"Cat"},
        {"Dog"},
        {"Foo"},
        {"Horse"},
        {"Island"},
        {"Rabbit"},
        {"Snake"},
        {"Zebra"},
    });
  };
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .min_load_factor = 0.5,
      .binary_search_threshold = 8,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  auto expected_regex = "general_with_ikey"s               //
                      + ".*" + "non_null_dense_with_ikey"  // left_sparse_part
                      + ".*" + "empty_with_ikey"           // left_sparse_part
                      + ".*" + "linear_search_with_ikey";  // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::ContainsRegex(expected_regex));
  EXPECT_EQ_STATIC(10, map.size());
  EXPECT_EQ_STATIC(7, map.dense_part.size());
  EXPECT_EQ_STATIC(0, map.left_sparse_part.size());
  EXPECT_EQ_STATIC(3, map.right_sparse_part.size());

  EXPECT_EQ_STATIC("Apple", map['A']);
  EXPECT_FOUND_STATIC("Banana", map, 'B');
  EXPECT_FOUND_STATIC("Cat", map, 'C');
  EXPECT_FOUND_STATIC("Dog", map, 'D');
  EXPECT_FOUND_STATIC("Foo", map, 'F');
  EXPECT_FOUND_STATIC("Horse", map, 'H');
  EXPECT_FOUND_STATIC("Island", map, 'I');
  EXPECT_FOUND_STATIC("Rabbit", map, 'R');
  EXPECT_FOUND_STATIC("Snake", map, 'S');
  EXPECT_FOUND_STATIC("Zebra", map, 'Z');
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(nullptr, map['a']);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 'E');
}
