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

#include "tests/utils/fixed_map/fixed_map_test_options.hpp"
#include <reflect_cpp26/utils/fixed_map/integral_key.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <climits>

namespace rfl = reflect_cpp26;
using namespace std::literals;

#define FIXED_MAP(...) \
  REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(__VA_ARGS__)

TEST(UtilsFixedMap, IntegralKeyEmpty)
{
  using KVPair = std::pair<int, const void*>;
  constexpr auto map_empty = FIXED_MAP(std::vector<KVPair>());
  EXPECT_THAT(display_string_of(^^decltype(map_empty)),
              testing::HasSubstr("empty_integral_key_map"));
  EXPECT_EQ_STATIC(0, map_empty.size());
  EXPECT_EQ_STATIC(nullptr, map_empty[0]);
  EXPECT_NOT_FOUND_STATIC(nullptr, map_empty, -1);
}

TEST(UtilsFixedMap, SignedIntegralKeyFullyContinuous)
{
  using KVPair = std::pair<int, int>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {-1, 123}, {1, 456}, {3, 789}, {-2, 12}, {0, 34}, {2, 56}
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs());
  static_assert(std::is_same_v<decltype(map)::result_type, const int&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("dense_integral_key_map"));
  EXPECT_EQ_STATIC(6, map.size());
  EXPECT_EQ_STATIC(-2, map.min_key());
  EXPECT_EQ_STATIC(3, map.max_key());

  EXPECT_EQ_STATIC(12, map[-2]);
  EXPECT_FOUND_STATIC(123, map, -1);
  EXPECT_FOUND_STATIC(34, map, 0);
  EXPECT_FOUND_STATIC(456, map, 1u);
  EXPECT_FOUND_STATIC(56, map, 2);
  EXPECT_FOUND_STATIC(789, map, 3u);
  // Value-initialized
  EXPECT_EQ_STATIC(0, map[-3]);
  EXPECT_NOT_FOUND_STATIC(0, map, std::numeric_limits<int>::max());
  // Safe integral comparison is used
  EXPECT_EQ_STATIC(0, map[static_cast<unsigned>(-1)]);
  EXPECT_NOT_FOUND_STATIC(0, map, 0x1'0000'0000LL);
}

TEST(UtilsFixedMap, UnsignedIntegralKeyFullyContinuous)
{
  using KVPair = std::pair<unsigned, double>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {10, 1.25}, {12, 2.5}, {11, 3.75}, {14, 5.0}, {13, 6.25}
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs());
  static_assert(std::is_same_v<decltype(map)::result_type, const double&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("dense_integral_key_map"));
  EXPECT_EQ_STATIC(5, map.size());
  EXPECT_EQ_STATIC(10, map.min_key());
  EXPECT_EQ_STATIC(14, map.max_key());

  EXPECT_EQ_STATIC(1.25, map[10]);
  EXPECT_FOUND_STATIC(3.75, map, 11);
  EXPECT_FOUND_STATIC(2.5, map, 12);
  EXPECT_FOUND_STATIC(6.25, map, 13);
  EXPECT_FOUND_STATIC(5.0, map, 14);
  // Value-initialized
  EXPECT_EQ_STATIC(0.0, map[0]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, -1);
  // Safe integral comparison is used
  EXPECT_NOT_FOUND_STATIC(0.0, map, 0x1'0000'000aULL);
}

TEST(UtilsFixedMap, SignedIntegralKeyContinuousWithHoles)
{
  using KVPair = std::pair<signed char, rfl::meta_string_view>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {'a', rfl::define_static_string("Apple")},
      {'b', rfl::define_static_string("Banana")},
      {'c', rfl::define_static_string("Cat")},
      {'d', rfl::define_static_string("Dog")},
      {'h', rfl::define_static_string("Hedgedog")},
      {'i', rfl::define_static_string("Ice")},
      {'j', rfl::define_static_string("John")},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .already_sorted = true,
    .already_unique = true,
    .min_load_factor = 0.5,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const rfl::meta_string_view&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("dense_integral_key_map"));
  EXPECT_EQ_STATIC(7, map.size());
  EXPECT_EQ_STATIC('a', map.min_key());
  EXPECT_EQ_STATIC('j', map.max_key());

  EXPECT_EQ_STATIC("Apple", map['a']);
  EXPECT_FOUND_STATIC("Banana", map, int8_t{'b'});
  EXPECT_FOUND_STATIC("Cat", map, uint8_t{'c'});
  EXPECT_FOUND_STATIC("Dog", map, int16_t{'d'});
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(nullptr, map['e']);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 'f');

  EXPECT_EQ_STATIC("Hedgedog", map[uint16_t{'h'}]);
  EXPECT_FOUND_STATIC("Ice", map, int32_t{'i'});
  EXPECT_FOUND_STATIC("John", map, uint32_t{'j'});
  // Safe integral comparison is used
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 0x100 + 'a');
}

constexpr auto make_kv_pairs_for_unsigned_integral_key_continuous()
{
  using KVPair = std::pair<unsigned char, rfl::meta_string_view>;
  return std::vector<KVPair>{
    {126, rfl::define_static_string("Apple")},
    {128, rfl::define_static_string("Banana")},
    {130, rfl::define_static_string("Cat")},
    {132, rfl::define_static_string("Dog")},
    {134, rfl::define_static_string("Hedgedog")},
    {136, rfl::define_static_string("Ice")},
  };
}

template <class FixedMap>
void test_unsigned_integral_key_continuous_common(const FixedMap& map)
{
  static_assert(std::is_same_v<
    typename FixedMap::result_type, const rfl::meta_string_view&>);
  EXPECT_EQ(6, map.size());
  EXPECT_EQ(126, map.min_key());
  EXPECT_EQ(136, map.max_key());

  EXPECT_EQ("Apple", map[int8_t{126}]);
  EXPECT_FOUND("Banana", map, uint8_t{128});
  EXPECT_FOUND("Cat", map, int16_t{130});
  EXPECT_FOUND("Dog", map, uint16_t{132});
  EXPECT_FOUND("Hedgedog", map, int32_t{134});
  EXPECT_FOUND("Ice", map, uint32_t{136});
  // Holes: Value-initialized
  EXPECT_EQ(nullptr, map[int8_t{127}]);
  EXPECT_NOT_FOUND(nullptr, map, uint8_t{129});
  EXPECT_NOT_FOUND(nullptr, map, int16_t{131});
  EXPECT_NOT_FOUND(nullptr, map, uint16_t{133});
  EXPECT_NOT_FOUND(nullptr, map, int32_t{135});
  EXPECT_NOT_FOUND(nullptr, map, uint32_t{137});
  // Safe integral comparison is used
  EXPECT_EQ(nullptr, map[int8_t{-128}]);
  EXPECT_NOT_FOUND(nullptr, map, int16_t{256 + 128});
}

TEST(UtilsFixedMap, UnsignedIntegralKeyContinuousWithHoles1)
{
  constexpr auto options = rfl::integral_key_map_options{
    .min_load_factor = 0.5,
  };
  constexpr auto map = FIXED_MAP(
    make_kv_pairs_for_unsigned_integral_key_continuous(), options);
  // true: Uses value wrapper with flag 'is_valid'
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("dense_integral_key_map_common<true"));
  test_unsigned_integral_key_continuous_common(map);
}

TEST(UtilsFixedMap, UnsignedIntegralKeyContinuousWithHoles2)
{
  constexpr auto options = rfl::integral_key_map_options{
    .default_value_is_always_invalid = true,
    .min_load_factor = 0.5,
  };
  constexpr auto map = FIXED_MAP(
    make_kv_pairs_for_unsigned_integral_key_continuous(), options);
  // false: Does not use value wrapper with flag 'is_valid' which is not needed.
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("dense_integral_key_map_common<false"));
  test_unsigned_integral_key_continuous_common(map);
}

TEST(UtilsFixedMap, SignedIntegralKeySparseLinearSearch)
{
  using KVPair = std::pair<int64_t, double>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {-100, 1.25}, {200, 3.5}, {-300, 5.75}, {400, 7.0}, {-500, 8.25}
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .binary_search_threshold = 100,
  });
  static_assert(std::is_same_v<decltype(map)::result_type, const double&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("sparse_linear_search_integral_key_map"));
  EXPECT_EQ_STATIC(5, map.size());
  EXPECT_EQ_STATIC(-500, map.min_key());
  EXPECT_EQ_STATIC(400, map.max_key());

  EXPECT_EQ_STATIC(1.25, map[-100]);
  EXPECT_FOUND_STATIC(5.75, map, -300);
  EXPECT_FOUND_STATIC(8.25, map, -500);
  EXPECT_FOUND_STATIC(3.5, map, 200);
  EXPECT_FOUND_STATIC(7.0, map, 400);
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(0.0, map[0]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, 10);
  EXPECT_NOT_FOUND_STATIC(0.0, map, 1000);
  // Safe integral comparison is used
  EXPECT_NOT_FOUND_STATIC(0.0, map, static_cast<unsigned>(-100));
}

TEST(UtilsFixedMap, UnsignedIntegralKeySparseBinarySearch)
{
  using KVPair = std::pair<uint64_t, uint64_t>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {static_cast<uint64_t>(-200), 12},
      {static_cast<uint64_t>(-100), 24},
      {100, 36},
      {300, 48},
      {500, 60},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .binary_search_threshold = 1,
  });
  static_assert(std::is_same_v<decltype(map)::result_type, const uint64_t&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("sparse_binary_search_integral_key_map"));
  EXPECT_EQ_STATIC(5, map.size());
  EXPECT_EQ_STATIC(100, map.min_key());
  EXPECT_EQ_STATIC(static_cast<uint64_t>(-100), map.max_key());

  EXPECT_EQ_STATIC(36, map[100]);
  EXPECT_FOUND_STATIC(48, map, 300);
  EXPECT_FOUND_STATIC(60, map, 500);
  EXPECT_FOUND_STATIC(24, map, static_cast<uint64_t>(-100));
  EXPECT_FOUND_STATIC(12, map, static_cast<uint64_t>(-200));
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(0, map[0]);
  // Safe integral comparison is used
  EXPECT_EQ_STATIC(0, map[-100]);
  EXPECT_NOT_FOUND_STATIC(0, map, -200);
}

template <size_t I>
size_t fn() { return I; }

TEST(UtilsFixedMap, SignedIntegralKeyGeneral)
{
  using KVPair = std::pair<int, size_t (*)()>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {INT_MIN, fn<0>}, {-3, fn<1>},
      {-1, fn<2>}, {0, fn<3>}, {1, fn<4>}, {2, fn<5>}, {3, fn<6>}, {4, fn<7>},
      {8, fn<8>}, {16, fn<9>}, {32, fn<10>}, {INT_MAX, fn<11>},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .min_load_factor = 1.0,
    .dense_part_threshold = 6,
    .binary_search_threshold = 4,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, size_t (* const &)()>);
  auto expected_regex = "general_integral_key_map"s
    + ".*" + "sparse_linear_search_integral_key_map" // left_sparse_part
    + ".*" + "sparse_binary_search_integral_key_map"; // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::ContainsRegex(expected_regex));
  EXPECT_EQ_STATIC(12, map.size());
  EXPECT_EQ_STATIC(6, map._dense_part.size());
  EXPECT_EQ_STATIC(2, map._left_sparse_part.size());
  EXPECT_EQ_STATIC(4, map._right_sparse_part.size());
  EXPECT_EQ_STATIC(INT_MIN, map.min_key());
  EXPECT_EQ_STATIC(INT_MAX, map.max_key());

  auto check_non_null = [&map](size_t expected_value, int key) {
    EXPECT_NE(nullptr, map[key]) << "failed with key = " << key;
    EXPECT_EQ(expected_value, map[key]()) << "failed with key = " << key;
  };
  check_non_null(0, INT_MIN);
  check_non_null(1, -3);
  check_non_null(2, -1);
  check_non_null(3, 0);
  check_non_null(4, 1);
  check_non_null(5, 2);
  check_non_null(6, 3);
  check_non_null(7, 4);
  check_non_null(8, 8);
  check_non_null(9, 16);
  check_non_null(10, 32);
  check_non_null(11, INT_MAX);
  // Holes: Value-initialized
  EXPECT_EQ_STATIC(nullptr, map[-7]);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, -5);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 9);
  EXPECT_NOT_FOUND_STATIC(nullptr, map, 65);
  // Safe integral comparison is used
  EXPECT_EQ_STATIC(nullptr, map[static_cast<unsigned>(-1)]);
}

constexpr auto make_kv_pairs_for_unsigned_integral_key_general()
{
  using KVPair = std::pair<unsigned, size_t (*)()>;
  return std::vector<KVPair>{
    {UINT_MAX, fn<0>},
    {0, fn<1>}, {2, fn<2>}, {4, fn<3>}, {6, fn<4>}, {9, fn<5>},
    {12, fn<6>}, {15, fn<7>}, {18, fn<8>},
  };
}

template <class FixedMap>
constexpr auto test_unsigned_integral_key_general_common(const FixedMap& map)
{
  static_assert(std::is_same_v<
    typename FixedMap::result_type, size_t (* const&)()>);
  EXPECT_EQ(9, map.size());
  EXPECT_EQ(5, map._dense_part.size());
  EXPECT_EQ(0, map._left_sparse_part.size());
  EXPECT_EQ(4, map._right_sparse_part.size());
  EXPECT_EQ(0, map.min_key());
  EXPECT_EQ(UINT_MAX, map.max_key());

  auto check_non_null = [&map](size_t expected_value, unsigned key) {
    EXPECT_NE(nullptr, map[key]) << "failed with key = " << key;
    EXPECT_EQ(expected_value, map[key]()) << "failed with key = " << key;
  };
  check_non_null(1, 0);
  check_non_null(2, 2);
  check_non_null(3, 4);
  check_non_null(4, 6);
  check_non_null(5, 9);
  check_non_null(6, 12);
  check_non_null(7, 15);
  check_non_null(8, 18);
  check_non_null(0, UINT_MAX);
  // Holes: Value-initialized
  EXPECT_EQ(nullptr, map[1]);
  EXPECT_NOT_FOUND(nullptr, map, 10);
  EXPECT_NOT_FOUND(nullptr, map, 16);
  EXPECT_NOT_FOUND(nullptr, map, UINT_MAX - 1);
  // Safe integral comparison is used
  EXPECT_EQ(nullptr, map[-1]);
}

TEST(UtilsFixedMap, UnsignedIntegralKeyGeneral1)
{
  constexpr auto options = rfl::integral_key_map_options{
    .min_load_factor = 0.5,
    .dense_part_threshold = 5,
    .binary_search_threshold = 4,
  };
  constexpr auto map = FIXED_MAP(
    make_kv_pairs_for_unsigned_integral_key_general(), options);
  // true: Uses value wrapper with flag 'is_valid'
  auto expected_regex = "general_integral_key_map<true"s
    + ".*" + "empty_integral_key_map" // left_sparse_part
    + ".*" + "sparse_binary_search_integral_key_map"; // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::ContainsRegex(expected_regex));
  test_unsigned_integral_key_general_common(map);
}

TEST(UtilsFixedMap, UnsignedIntegralKeyGeneral2)
{
  constexpr auto options = rfl::integral_key_map_options{
    .default_value_is_always_invalid = true,
    .min_load_factor = 0.5,
    .dense_part_threshold = 5,
    .binary_search_threshold = 4,
  };
  constexpr auto map = FIXED_MAP(
    make_kv_pairs_for_unsigned_integral_key_general(), options);
  // true: Noes not value wrapper with flag 'is_valid' as it's not needed.
  auto expected_regex = "general_integral_key_map<false"s
    + ".*" + "empty_integral_key_map" // left_sparse_part
    + ".*" + "sparse_binary_search_integral_key_map"; // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::ContainsRegex(expected_regex));
  test_unsigned_integral_key_general_common(map);
}

struct static_string_wrapper {
  rfl::meta_string_view string;
};

namespace std {
template <>
struct tuple_size<static_string_wrapper> {
  static constexpr size_t value = 2;
};

template <size_t I>
  requires (I == 0 || I == 1)
struct tuple_element<I, static_string_wrapper> {
  using type = std::conditional_t<(I == 0), char, rfl::meta_string_view>;
};
} // namespace std

template <size_t I>
constexpr auto get(static_string_wrapper s)
{
  if constexpr (I == 0) {
    return s.string[0];
  } else if constexpr (I == 1) {
    return s.string;
  } else {
    static_assert(false, "Invalid index.");
  }
}

TEST(UtilsFixedMap, IntegralKeyCustomKVPair)
{
  constexpr auto map = FIXED_MAP(std::vector<static_string_wrapper>({
    { rfl::define_static_string("Apple") },
    { rfl::define_static_string("Banana") },
    { rfl::define_static_string("Cat") },
    { rfl::define_static_string("Dog") },
    { rfl::define_static_string("Foo") },
    { rfl::define_static_string("Horse") },
    { rfl::define_static_string("Island") },
    { rfl::define_static_string("Rabbit") },
    { rfl::define_static_string("Snake") },
    { rfl::define_static_string("Zebra") },
  }), {
    .already_sorted = true,
    .already_unique = true,
    .min_load_factor = 0.5,
    .dense_part_threshold = 4,
    .binary_search_threshold = 4,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, rfl::meta_string_view>);
  // true: Uses value wrapper with flag 'is_valid'
  auto expected_regex = "general_integral_key_map<true"s
    + ".*" + "empty_integral_key_map" // left_sparse_part
    + ".*" + "sparse_linear_search_integral_key_map"; // right_sparse_part
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::ContainsRegex(expected_regex));
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

TEST(UtilsFixedMap, IntegralKeyInt8Full)
{
  constexpr auto delta = 1000.0;
  using KVPair = std::pair<int8_t, double>;
  constexpr auto make_kv_pairs = [delta]() constexpr {
    auto res = std::vector<KVPair>(1 << CHAR_BIT);
    for (auto i = SCHAR_MIN; ; i++) {
      res[i - SCHAR_MIN] = {i, i + delta};
      if (i == SCHAR_MAX) { break; }
    }
    return res;
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs());
  static_assert(std::is_same_v<decltype(map)::result_type, const double&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("dense_integral_key_map"));
  EXPECT_EQ_STATIC(1 << CHAR_BIT, map.size());
  EXPECT_EQ_STATIC(SCHAR_MIN, map.min_key());
  EXPECT_EQ_STATIC(SCHAR_MAX, map.max_key());

  for (auto i = SCHAR_MIN; ; i++) {
    auto [v, found] = map.get(i);
    EXPECT_TRUE(found) << "Failed with i = " << i;
    EXPECT_EQ(i + delta, v) << "Failed with i = " << i;
    if (i == SCHAR_MAX) {
      break;
    }
  }
  EXPECT_EQ_STATIC(0.0, map[SCHAR_MIN - 1]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, SCHAR_MAX + 1);
}

TEST(UtilsFixedMap, IntegralKeyUInt8Full)
{
  constexpr auto delta = 1000.0;
  using KVPair = std::pair<uint8_t, double>;
  constexpr auto make_kv_pairs = [delta]() constexpr {
    auto res = std::vector<KVPair>(1 << CHAR_BIT);
    for (auto i = 0; i <= UCHAR_MAX; i++) {
      res[i] = {i, i + delta};
    }
    return res;
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs());
  static_assert(std::is_same_v<decltype(map)::result_type, const double&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("dense_integral_key_map"));
  EXPECT_EQ_STATIC(1 << CHAR_BIT, map.size());
  EXPECT_EQ_STATIC(0, map.min_key());
  EXPECT_EQ_STATIC(UCHAR_MAX, map.max_key());

  for (auto i = 0; i <= UCHAR_MAX; i++) {
    auto [v, found] = map.get(i);
    EXPECT_TRUE(found) << "Failed with i = " << i;
    EXPECT_EQ(i + delta, v) << "Failed with i = " << i;
  }
  EXPECT_EQ_STATIC(0.0, map[-1]);
  EXPECT_NOT_FOUND_STATIC(0.0, map, UCHAR_MAX + 1);
}
