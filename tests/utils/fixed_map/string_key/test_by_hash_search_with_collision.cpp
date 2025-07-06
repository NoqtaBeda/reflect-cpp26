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

#include "tests/utils/fixed_map/string_key/string_key_test_options.hpp"
#include <reflect_cpp26/utils/fixed_map/string_key.hpp>

namespace rfl = reflect_cpp26;

constexpr auto strings_with_hash_collision = std::array{
  "0BCPElfPXEtMOUE",
  "2P2H907ksk6vQFW",
  "4KGSiDd_WBUdLEg",
  "6_Cm_AklFmYKc4S",
  "7SwO68tn5JnfpWe",
  "ALPOBd813kHWVxu",
  "AyshlQKfxmMdGE4",
  "Bvo3i5j5_ZFZsZc",
  "DTkO8hjT4NsMLxv",
  "EK0OTLFzgRM6lDN",
  "EQJUIE23PenIm2I",
  "EWNizOU0M0tF2Kj",
  "EbYwQ4i8J4uLwgA",
  "HsMqQzGTXfJGVZw",
  "HucDVkbRnhdAFDN",
  "ITD_KpPq2hDzrZT",
  "Kwg43nspyUx82Hg",
  "MV7jDdjEkL7C_mE",
  "MkFJCq2VQyMywJf",
  "Qyfre0rOlxRvRWf",
  "RhPAaEW2gV0iA2v",
  "SuHWKjS3W6SeuEt",
  "VZN4nduCikBzm_h",
  "XQUd5ANFbrdOirf",
  "_EnPg50lJUbrQcu",
  "aedZVfoeGRrSCuZ",
  "cpedvFH0008U_Sz",
  "cuFFJIHGp_jNJKS",
  "eEq531p7C604fY2",
  "fs5vbC2dMMgh7HS",
  "gDcf6PmY8L7E2Y3",
  "gJnJJ7jhR6pCClY",
  "iN42c6SyFQJqSC4",
  "jc9Nad159YjNyn6",
  "lUwq4JUvMNVZ2Kc",
  "ltDwvfM7t4q8Epd",
  "pBFHYjoPHfC7_SI",
  "pnyyUTzU793Dx3C",
  "ua233bXVs05ligl",
  "wSYZDRpiQJf8Rfv",
  "wlAsvdpajDERFnE",
  "wo6UUYzXIiZphUo",
  "ysndSaMOUqxeqEt",
  "zdYtgKeY1l7CFyd",
};

template <class CharT>
consteval auto make_kv_pairs()
{
  using KVPair = std::pair<std::basic_string<CharT>, wrapper_t<size_t>>;
  auto n = strings_with_hash_collision.size();
  auto res = std::vector<KVPair>();
  for (auto i = 0zU; i < n; i++) {
    res.emplace_back(to<CharT>(strings_with_hash_collision[i]), i);
  }
  return res;
}

template <class CharT>
void test_by_hash_search_with_collision_common()
{
  constexpr auto map = FIXED_MAP(make_kv_pairs<CharT>(), {
    .case_insensitive = false,
    .min_load_factor = 1e-10,
  });
  static_assert(std::is_same_v<
    typename decltype(map)::result_type, const wrapper_t<size_t>&>);
  EXPECT_THAT(display_string_of(^^decltype(map)),
    testing::HasSubstr("string_key_map_with_hash_collision"));
  EXPECT_EQ_STATIC(strings_with_hash_collision.size(), map.size());

  EXPECT_FOUND_STATIC(0, map, to<CharT>("0BCPElfPXEtMOUE"));
  for (auto i = 0zU, n = strings_with_hash_collision.size(); i < n; i++) {
    auto char_key = strings_with_hash_collision[i];
    auto key = to<CharT>(char_key);
    EXPECT_EQ(i, map[key]) << "Fails with key '" << char_key << "'";
  }
  EXPECT_EQ_STATIC(magic_value, map[to<CharT>("")]);
  EXPECT_NOT_FOUND_STATIC(magic_value, map, to<CharT>("_zdYtgKeY1l7CFyd_"));
}

#define MAKE_MAP_TESTS(char_type, CharTypeName)                           \
  TEST(UtilsFixedMap, StringKeyByHashSearchWithCollision##CharTypeName) { \
    test_by_hash_search_with_collision_common<char_type>();               \
  }

MAKE_MAP_TESTS(char, Char)
MAKE_MAP_TESTS(char8_t, Char8)
