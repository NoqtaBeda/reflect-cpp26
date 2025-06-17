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
#include <reflect_cpp26/utils/fixed_map/string_key.hpp>
#include <reflect_cpp26/utils/to_string.hpp>

namespace rfl = reflect_cpp26;
using namespace std::literals;

#define STATIC_STR(s) reflect_cpp26::define_static_string(s)
#define FIXED_MAP(...) REFLECT_CPP26_STRING_KEY_FIXED_MAP(__VA_ARGS__)

TEST(UtilsFixedMap, StringKeyEmpty)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<int>>;
  constexpr auto map_empty = FIXED_MAP(std::vector<KVPair>{});
  EXPECT_THAT(display_string_of(^^decltype(map_empty)),
              testing::HasSubstr("empty_string_key_map"));
  EXPECT_EQ_STATIC(0, map_empty.size());
  EXPECT_EQ_STATIC(magic_value, map_empty[""]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map_empty, "hello");
}

TEST(UtilsFixedMap, StringKeyNaive)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<double>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {STATIC_STR("Apple"), {.value = 12.5}},
      {STATIC_STR("Pineapple"), {.value = 25.0}},
      {STATIC_STR("Pen"), {.value = 37.5}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .optimization_threshold = 4,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<double>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("naive_string_key_map"));
  EXPECT_EQ_STATIC(3, map.size());
  EXPECT_EQ_STATIC(12.5, map["Apple"]);
  EXPECT_FOUND_STATIC(25.0, map, "Pineapple");
  EXPECT_FOUND_STATIC(37.5, map, "Pen");
  EXPECT_EQ_STATIC(magic_value, map[""]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "apple");
}

TEST(UtilsFixedMap, StringKeyByLength)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<double>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {STATIC_STR("Apple"), {.value = 12.5}},
      {STATIC_STR("Pineapple"), {.value = 25.0}},
      {STATIC_STR("Pen"), {.value = 37.5}},
      {STATIC_STR("Banana"), {.value = 50.0}},
      {STATIC_STR("Strawberry"), {.value = 62.5}},
      {STATIC_STR(""), {.value = 75.0}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .optimization_threshold = 4,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<double>&>);
  static_assert(std::is_same_v<decltype(map)::kv_pair_type, KVPair>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_length"));
  EXPECT_EQ_STATIC(6, map.size());
  EXPECT_EQ_STATIC(12.5, map["Apple"]);
  EXPECT_FOUND_STATIC(25.0, map, "Pineapple");
  EXPECT_FOUND_STATIC(37.5, map, "Pen");
  EXPECT_FOUND_STATIC(50.0, map, "Banana");
  EXPECT_FOUND_STATIC(62.5, map, "Strawberry");
  EXPECT_FOUND_STATIC(75.0, map, "");
  EXPECT_EQ_STATIC(magic_value, map["apple"]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "Cat");
}

TEST(UtilsFixedMap, StringKeyByCharacter1)
{
    using KVPair = std::pair<rfl::meta_string_view, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    // Differs at I == 1: 'p', 'i', 'e', 'a', 't', 'o', 'h', 'w'
    return std::vector<KVPair>{
      {STATIC_STR("Apple"), {.value = 10}},
      {STATIC_STR("Pineapple"), {.value = 20}},
      {STATIC_STR("Pen"), {.value = 30}},
      {STATIC_STR("Banana"), {.value = 40}},
      {STATIC_STR("Strawberry"), {.value = 50}},
      {STATIC_STR("Dog"), {.value = 60}},
      {STATIC_STR("Sheep"), {.value = 70}},
      {STATIC_STR("Owl"), {.value = 80}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .optimization_threshold = 4,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<int>&>);
  static_assert(std::is_same_v<decltype(map)::kv_pair_type, KVPair>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_character"));
  EXPECT_EQ_STATIC(1, map.differed_index);
  EXPECT_EQ_STATIC(8, map.size());
  EXPECT_EQ_STATIC(10, map["Apple"]);
  EXPECT_FOUND_STATIC(20, map, "Pineapple");
  EXPECT_FOUND_STATIC(30, map, "Pen");
  EXPECT_FOUND_STATIC(40, map, "Banana");
  EXPECT_FOUND_STATIC(50, map, "Strawberry");
  EXPECT_FOUND_STATIC(60, map, "Dog");
  EXPECT_FOUND_STATIC(70, map, "Sheep");
  EXPECT_FOUND_STATIC(80, map, "Owl");
  EXPECT_EQ_STATIC(magic_value, map[""]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "Dig");
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "Dug");
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "Watermelon");
}

TEST(UtilsFixedMap, StringKeyByCharacter2)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    // Differs at I == 3
    return std::vector<KVPair>{
      {STATIC_STR("op_0"), {.value = 0}},
      {STATIC_STR("op_1"), {.value = 10}},
      {STATIC_STR("op_2"), {.value = 20}},
      {STATIC_STR("op_3"), {.value = 30}},
      {STATIC_STR("op_4"), {.value = 40}},
      {STATIC_STR("op_5"), {.value = 50}},
      {STATIC_STR("op_6"), {.value = 60}},
      {STATIC_STR("op_7"), {.value = 70}},
      {STATIC_STR("op_8"), {.value = 80}},
      {STATIC_STR("op_9"), {.value = 90}},
      {STATIC_STR("op_+"), {.value = 100}},
      {STATIC_STR("op_-"), {.value = 110}},
      {STATIC_STR("op_*"), {.value = 120}},
      {STATIC_STR("op_/"), {.value = 130}},
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .min_load_factor = 1.0,
    .optimization_threshold = 4,
    .dense_part_threshold = 4,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<int>&>);
  static_assert(std::is_same_v<decltype(map)::kv_pair_type, KVPair>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_character"));
  EXPECT_EQ_STATIC(3, map.differed_index);
  EXPECT_EQ_STATIC(14, map.size());
  for (auto i = 0; i <= 9; i++) {
    EXPECT_EQ(i * 10, map["op_" + rfl::to_string(i)])
      << "Fails with key 'op_" << i << "'.";
  }
  EXPECT_EQ_STATIC(100, map["op_+"]);
  EXPECT_FOUND_STATIC(110, map, "op_-");
  EXPECT_FOUND_STATIC(120, map, "op_*");
  EXPECT_FOUND_STATIC(130, map, "op_/");
  EXPECT_EQ_STATIC(magic_value, map[""]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "op_%");
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "OP_+");
}

TEST(UtilsFixedMap, StringKeyByHashTableFast1)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {STATIC_STR("Apple"), {.value = 0}},  //   0x4'841a'c9fa
      {STATIC_STR("Banana"), {.value = 1}}, // 0x257'8da4'8005
      {STATIC_STR("Cat"), {.value = 2}},    //       0x11'bd72
      {STATIC_STR("Dog"), {.value = 3}},    //       0x12'0798
      {STATIC_STR("Horse"), {.value = 4}},  //   0x4'fed9'023d
      {STATIC_STR("Rabbit"), {.value = 5}}, // 0x2e7'444d'd47e
      {STATIC_STR(""), {.value = 6}},       //             0x0
      {STATIC_STR("Sheep"), {.value = 7}},  //   0x5'befc'49e7
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .min_load_factor = 0.5,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_fast"));
  EXPECT_EQ_STATIC(8, map.size());
  EXPECT_EQ_STATIC(15, map._bucket_size_mask);
  EXPECT_EQ_STATIC(0, map["Apple"]);
  EXPECT_FOUND_STATIC(1, map, "Banana");
  EXPECT_FOUND_STATIC(2, map, "Cat");
  EXPECT_FOUND_STATIC(3, map, "Dog");
  EXPECT_FOUND_STATIC(4, map, "Horse");
  EXPECT_FOUND_STATIC(5, map, "Rabbit");
  EXPECT_FOUND_STATIC(6, map, "");
  EXPECT_FOUND_STATIC(7, map, "Sheep");
  EXPECT_EQ_STATIC(magic_value, map["Donkey"]); // 0x26a'7a6f'a930
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "apple");
  // 0x6143'2afc'63e3'd68a
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "Pineapple");
}

TEST(UtilsFixedMap, StringKeyByHashTableFast2)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() constexpr {
    return std::vector<KVPair>{
      {STATIC_STR("Apple"), {.value = 0}},       // 0x0000'0004'841a'c9fa
      {STATIC_STR("Banana"), {.value = 1}},      // 0x0000'0257'8da4'8005
      {STATIC_STR("Cat"), {.value = 2}},         // 0x0000'0000'0011'bd72
      {STATIC_STR("Dog"), {.value = 3}},         // 0x0000'0000'0012'0798
      {STATIC_STR("Horse"), {.value = 4}},       // 0x0000'0004'fed9'023d
      {STATIC_STR("Rabbit"), {.value = 5}},      // 0x0000'02e7'444d'd47e
      {STATIC_STR("Donkey"), {.value = 6}},      // 0x0000'026a'7a6f'a930
      {STATIC_STR("Sheep"), {.value = 7}},       // 0x0000'0005'befc'49e7
      {STATIC_STR("Pineapple"), {.value = 8}},   // 0x6143'2afc'63e3'd68a
      {STATIC_STR("Watermelon"), {.value = 9}},  // 0x0b85'1508'69f4'1520
      {STATIC_STR("Capybara"), {.value = 10}},   // 0x009f'595e'1a65'd529
      {STATIC_STR("ABCDEF"), {.value = 11}},     // 0x0000'024c'6c2c'6a9f
      {STATIC_STR("ABCDEV"), {.value = 12}},     // 0x0000'024c'6c2c'6aaf
    };
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .min_load_factor = 0.5,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_fast"));
  EXPECT_EQ_STATIC(13, map.size());
  EXPECT_EQ_STATIC(15, map._bucket_size_mask);
  EXPECT_EQ_STATIC(0, map["Apple"]);
  EXPECT_FOUND_STATIC(1, map, "Banana");
  EXPECT_FOUND_STATIC(2, map, "Cat");
  EXPECT_FOUND_STATIC(3, map, "Dog");
  EXPECT_FOUND_STATIC(4, map, "Horse");
  EXPECT_FOUND_STATIC(5, map, "Rabbit");
  EXPECT_FOUND_STATIC(6, map, "Donkey");
  EXPECT_FOUND_STATIC(7, map, "Sheep");
  EXPECT_FOUND_STATIC(8, map, "Pineapple");
  EXPECT_FOUND_STATIC(9, map, "Watermelon");
  EXPECT_FOUND_STATIC(10, map, "Capybara");
  EXPECT_FOUND_STATIC(11, map, "ABCDEF");
  EXPECT_FOUND_STATIC(12, map, "ABCDEV");
  EXPECT_EQ_STATIC(magic_value, map[""]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "banana");
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "Mouse"); // 0x5'569e'5aed
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "ABCDEG");
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "ABCDEf");
}

TEST(UtilsFixedMap, StringKeyByHashTableSlow)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<int>>;
  constexpr auto make_kv_pairs = []() consteval {
    auto res = std::vector<KVPair>{
      {rfl::define_static_string("BA"), {.value = 1}}, // 0x2207
      {rfl::define_static_string("Ba"), {.value = 2}}, // 0x2227
    };
    for (auto i = 'A'; i <= 'Z'; i++) {
      res.emplace_back(
        rfl::define_static_string("A" + std::string(1, i)),
        wrapper_t<int>{.value = i}); // 0x2184 ~ 0x219d
    }
    return res;
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .min_load_factor = 0.5
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<int>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr("string_key_map_by_hash_slow"));
  EXPECT_EQ_STATIC(28, map.size());
  EXPECT_EQ_STATIC(33, map._bucket_size); // Modulo: 0 ~ 25, 31, 32
  EXPECT_FOUND_STATIC('G', map, "AG");
  EXPECT_FOUND_STATIC('T', map, "AT");
  for (auto i = 'B'; i <= 'Z'; i++) {
    auto key = "A" + std::string(1, i);
    EXPECT_EQ(i, map[key]) << "Failed with key '" << key << "'";
  }
  EXPECT_EQ_STATIC(magic_value, map[""]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "B@");
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "A{");
}

consteval auto make_kv_pairs_for_string_key_by_hash_search()
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<int>>;
  return std::vector<KVPair>{
    {STATIC_STR("Apple"), {.value = 0}},
    {STATIC_STR("Banana"), {.value = 1}},
    {STATIC_STR("Cat"), {.value = 2}},
    {STATIC_STR("Dog"), {.value = 3}},
    {STATIC_STR("Horse"), {.value = 4}},
    {STATIC_STR("Rabbit"), {.value = 5}},
    {STATIC_STR("Donkey"), {.value = 6}},
    {STATIC_STR("Sheep"), {.value = 7}},
  };
}

template <class FixedMap>
void test_string_key_by_hash_search_common(const FixedMap& map)
{
  static_assert(std::is_same_v<
    typename FixedMap::result_type, const wrapper_t<int>&>);
  EXPECT_EQ(8, map.size());
  EXPECT_EQ(0, map["Apple"]);
  EXPECT_FOUND(1, map, "Banana");
  EXPECT_FOUND(2, map, "Cat");
  EXPECT_FOUND(3, map, "Dog");
  EXPECT_FOUND(4, map, "Horse");
  EXPECT_FOUND(5, map, "Rabbit");
  EXPECT_FOUND(6, map, "Donkey");
  EXPECT_FOUND(7, map, "Sheep");
  EXPECT_EQ(magic_value, map[""]);
  EXPECT_NOT_FOUND(magic_value, map, "apple");
  EXPECT_NOT_FOUND(magic_value, map, "Pineapple");
}

TEST(UtilsFixedMap, StringKeyByHashLinearSearch)
{
  constexpr auto options = rfl::string_key_map_options{
    .min_load_factor = 1.0,
    .binary_search_threshold = 9,
  };
  constexpr auto map = FIXED_MAP(
    make_kv_pairs_for_string_key_by_hash_search(), options);
  EXPECT_THAT(display_string_of(^^decltype(map)),
    testing::HasSubstr("string_key_map_by_hash_linear_search"));
  test_string_key_by_hash_search_common(map);
}

TEST(UtilsFixedMap, StringKeyByHashBinarySearchFast)
{

  constexpr auto options = rfl::string_key_map_options{
    .min_load_factor = 1.0,
    .binary_search_threshold = 8,
  };
  constexpr auto map = FIXED_MAP(
    make_kv_pairs_for_string_key_by_hash_search(), options);
  EXPECT_THAT(display_string_of(^^decltype(map)),
    testing::HasSubstr("string_key_map_by_hash_binary_search_fast"));
  test_string_key_by_hash_search_common(map);
}

constexpr auto strings_with_hash_collision = std::array{
  STATIC_STR("0BCPElfPXEtMOUE"),
  STATIC_STR("2P2H907ksk6vQFW"),
  STATIC_STR("4KGSiDd_WBUdLEg"),
  STATIC_STR("6_Cm_AklFmYKc4S"),
  STATIC_STR("7SwO68tn5JnfpWe"),
  STATIC_STR("ALPOBd813kHWVxu"),
  STATIC_STR("AyshlQKfxmMdGE4"),
  STATIC_STR("Bvo3i5j5_ZFZsZc"),
  STATIC_STR("DTkO8hjT4NsMLxv"),
  STATIC_STR("EK0OTLFzgRM6lDN"),
  STATIC_STR("EQJUIE23PenIm2I"),
  STATIC_STR("EWNizOU0M0tF2Kj"),
  STATIC_STR("EbYwQ4i8J4uLwgA"),
  STATIC_STR("HsMqQzGTXfJGVZw"),
  STATIC_STR("HucDVkbRnhdAFDN"),
  STATIC_STR("ITD_KpPq2hDzrZT"),
  STATIC_STR("Kwg43nspyUx82Hg"),
  STATIC_STR("MV7jDdjEkL7C_mE"),
  STATIC_STR("MkFJCq2VQyMywJf"),
  STATIC_STR("Qyfre0rOlxRvRWf"),
  STATIC_STR("RhPAaEW2gV0iA2v"),
  STATIC_STR("SuHWKjS3W6SeuEt"),
  STATIC_STR("VZN4nduCikBzm_h"),
  STATIC_STR("XQUd5ANFbrdOirf"),
  STATIC_STR("_EnPg50lJUbrQcu"),
  STATIC_STR("aedZVfoeGRrSCuZ"),
  STATIC_STR("cpedvFH0008U_Sz"),
  STATIC_STR("cuFFJIHGp_jNJKS"),
  STATIC_STR("eEq531p7C604fY2"),
  STATIC_STR("fs5vbC2dMMgh7HS"),
  STATIC_STR("gDcf6PmY8L7E2Y3"),
  STATIC_STR("gJnJJ7jhR6pCClY"),
  STATIC_STR("iN42c6SyFQJqSC4"),
  STATIC_STR("jc9Nad159YjNyn6"),
  STATIC_STR("lUwq4JUvMNVZ2Kc"),
  STATIC_STR("ltDwvfM7t4q8Epd"),
  STATIC_STR("pBFHYjoPHfC7_SI"),
  STATIC_STR("pnyyUTzU793Dx3C"),
  STATIC_STR("ua233bXVs05ligl"),
  STATIC_STR("wSYZDRpiQJf8Rfv"),
  STATIC_STR("wlAsvdpajDERFnE"),
  STATIC_STR("wo6UUYzXIiZphUo"),
  STATIC_STR("ysndSaMOUqxeqEt"),
  STATIC_STR("zdYtgKeY1l7CFyd"),
};

TEST(UtilsFixedMap, StringKeyByHashBinarySearchSlow)
{
  using KVPair = std::pair<rfl::meta_string_view, wrapper_t<size_t>>;
  constexpr auto make_kv_pairs = []() constexpr {
    auto res = std::vector<KVPair>{};
    for (auto i = 0zU, n = strings_with_hash_collision.size(); i < n; i++) {
      res.emplace_back(strings_with_hash_collision[i], i);
    }
    return res;
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), {
    .min_load_factor = 1e-10,
  });
  static_assert(std::is_same_v<
    decltype(map)::result_type, const wrapper_t<size_t>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
    testing::HasSubstr("string_key_map_by_hash_binary_search_slow"));
  EXPECT_EQ_STATIC(strings_with_hash_collision.size(), map.size());

  EXPECT_FOUND_STATIC(0, map, "0BCPElfPXEtMOUE");
  for (auto i = 0zU, n = strings_with_hash_collision.size(); i < n; i++) {
    auto key = std::string_view{strings_with_hash_collision[i]};
    EXPECT_EQ(i, map[key]) << "Fails with key '" << key << "'";
  }
  EXPECT_EQ_STATIC(magic_value, map[""]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, "_zdYtgKeY1l7CFyd_");
}
