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
#include <reflect_cpp26/utils/type_tuple.hpp>

#include "tests/fixed_map/integral_key/integral_key_test_options.hpp"

namespace rfl = reflect_cpp26;
using namespace std::literals;

struct string_wrapper {
  using tuple_elements = rfl::type_tuple<char, std::string_view>;
  std::string_view string;
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

template <rfl::integral_key_fixed_map_options Options>
constexpr void test_custom_kv_pair_common() {
  constexpr auto map = FIXED_MAP(std::vector<string_wrapper>({
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
                                 }),
                                 Options);

  static_assert(rfl::same_as_one_of<typename decltype(map)::kv_pair_type,
                                    std::pair<char, rfl::meta_string_view>,
                                    rfl::meta_tuple<char, rfl::meta_string_view>>);
  // to_structural_result_t<std::string_view> -> rfl::meta_string_view
  static_assert(std::is_same_v<typename decltype(map)::result_type, const rfl::meta_string_view&>);

  auto expected_regex = "general_integral_key_map"s + ".*"
                      + "dense_integral_key_map"                  // left_sparse_part
                      + ".*" + "empty_integral_key_map"           // left_sparse_part
                      + ".*" + "linear_search_integral_key_map";  // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)), testing::ContainsRegex(expected_regex));
  EXPECT_EQ_STATIC(10, map.size());
  EXPECT_EQ_STATIC(7, map._dense_part.size());
  EXPECT_EQ_STATIC(0, map._left_sparse_part.size());
  EXPECT_EQ_STATIC(3, map._right_sparse_part.size());
  EXPECT_EQ_STATIC('A', map.min_key());
  EXPECT_EQ_STATIC('Z', map.max_key());

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

TEST(FixedMap, IntegralKeyCustomKVPair1) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .already_sorted = true,
      .already_unique = true,
      .adjusts_alignment = false,
      .min_load_factor = 0.5,
      .dense_lookup_threshold = 4,
      .binary_search_threshold = 4,
  };
  test_custom_kv_pair_common<options>();
}

TEST(FixedMap, IntegralKeyCustomKVPair2) {
  constexpr auto options = rfl::integral_key_fixed_map_options{
      .already_sorted = true,
      .already_unique = true,
      .adjusts_alignment = true,
      .min_load_factor = 0.5,
      .dense_lookup_threshold = 4,
      .binary_search_threshold = 4,
  };
  test_custom_kv_pair_common<options>();
}
