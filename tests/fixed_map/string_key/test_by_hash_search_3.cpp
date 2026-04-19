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

#include <reflect_cpp26/fixed_map/string_key.hpp>

#include "tests/fixed_map/string_key/string_key_test_options.hpp"

namespace rfl = reflect_cpp26;
using namespace std::string_literals;

constexpr std::pair<const char*, const char*> hash_collision_pairs[] = {
    {"_wSYZDRpiQJf8Rfv", "_cuFFJIHGp_jNJKS"}, {"_lUwq4JUvMNVZ2Kc", "_SuHWKjS3W6SeuEt"},
    {"_EQJUIE23PenIm2I", "_wlAsvdpajDERFnE"}, {"_Kwg43nspyUx82Hg", "_fs5vbC2dMMgh7HS"},
    {"_zdYtgKeY1l7CFyd", "_pnyyUTzU793Dx3C"}, {"_2P2H907ksk6vQFW", "_MkFJCq2VQyMywJf"},
    {"_iN42c6SyFQJqSC4", "__EnPg50lJUbrQcu"}, {"_gDcf6PmY8L7E2Y3", "_DTkO8hjT4NsMLxv"},
    {"_EWNizOU0M0tF2Kj", "_gJnJJ7jhR6pCClY"}, {"_AyshlQKfxmMdGE4", "_0BCPElfPXEtMOUE"},
    {"_7SwO68tn5JnfpWe", "_ltDwvfM7t4q8Epd"}, {"_HucDVkbRnhdAFDN", "_VZN4nduCikBzm_h"},
    {"_4KGSiDd_WBUdLEg", "_eEq531p7C604fY2"}, {"_ysndSaMOUqxeqEt", "_wo6UUYzXIiZphUo"},
    {"_MV7jDdjEkL7C_mE", "_HsMqQzGTXfJGVZw"}, {"_Qyfre0rOlxRvRWf", "_EK0OTLFzgRM6lDN"},
    {"_XQUd5ANFbrdOirf", "_Bvo3i5j5_ZFZsZc"}, {"_RhPAaEW2gV0iA2v", "_EbYwQ4i8J4uLwgA"},
    {"_jc9Nad159YjNyn6", "_ua233bXVs05ligl"}, {"_aedZVfoeGRrSCuZ", "_ITD_KpPq2hDzrZT"},
    {"_cpedvFH0008U_Sz", "_pBFHYjoPHfC7_SI"}, {"_ALPOBd813kHWVxu", "_6_Cm_AklFmYKc4S"},
};

template <bool A, size_t BT, class CharT>
void test_by_hash_search_common_3() {
  using Value = std::pair<size_t, size_t>;
  using KVPair = std::pair<std::basic_string<CharT>, Value>;
  constexpr auto make_kv_pairs = []() consteval {
    auto res = std::vector<KVPair>{};
    for (auto i = 0zU, n = std::size(hash_collision_pairs); i < n; i++) {
      auto key = to<CharT>(hash_collision_pairs[i].first);
      res.emplace_back(key, Value{i, i + 1});
    }
    return res;
  };
  constexpr auto options = rfl::string_key_fixed_map_options{
      .ascii_case_insensitive = false,
      .adjusts_alignment = A,
      .max_n_iterations = 0,  // Disables hash table
      .binary_search_threshold = BT,
  };
  constexpr auto map = FIXED_MAP(make_kv_pairs(), options);

  constexpr auto expected_element_size = (sizeof(void*) == 8) ? (A ? 64 : 40) : (A ? 32 : 24);
  constexpr auto actual_element_size = sizeof(map.entries[0]);
  EXPECT_EQ(expected_element_size, actual_element_size)
      << "Unexpected element size with fixed map type " << display_string_of(^^decltype(map));

  constexpr auto n = std::size(hash_collision_pairs);
  EXPECT_THAT(display_string_of(^^decltype(map)),
              testing::HasSubstr(n >= BT ? "binary_hash_search_with_skey"
                                         : "linear_hash_search_with_skey"));
  EXPECT_EQ_STATIC(std::size(hash_collision_pairs), map.size());

  constexpr auto DEFAULT = Value{0, 0};
  for (auto i = 0zU; i < n; i++) {
    EXPECT_FOUND(Value(i, i + 1), map, to<CharT>(hash_collision_pairs[i].first));
    EXPECT_NOT_FOUND(DEFAULT, map, to<CharT>(hash_collision_pairs[i].second));
  }

  EXPECT_EQ_STATIC(DEFAULT, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("B@"));
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("A{"));
  EXPECT_NOT_FOUND_STATIC(DEFAULT, map, to<CharT>("ba"));
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)                 \
  TEST(FixedMap, StringKeyByHashSearch3Linear##CharTypeName) {  \
    test_by_hash_search_common_3<false, 1000, char_type>();     \
  }                                                             \
  TEST(FixedMap, StringKeyByHashSearch3Binary##CharTypeName) {  \
    test_by_hash_search_common_3<false, 4, char_type>();        \
  }                                                             \
  TEST(FixedMap, StringKeyByHashSearch3BinaryA##CharTypeName) { \
    test_by_hash_search_common_3<true, 4, char_type>();         \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(wchar_t, WChar)
MAKE_MAP_TESTS(char8_t, Char8)
MAKE_MAP_TESTS(char16_t, Char16)
MAKE_MAP_TESTS(char32_t, Char32)
