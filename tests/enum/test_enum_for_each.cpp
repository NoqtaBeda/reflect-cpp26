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

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_for_each.hpp>
#endif

using namespace reflect_cpp26;

template <auto B>
struct some_type_t {
  static constexpr auto value =
    std::to_underlying(B) * std::to_underlying(B) + 2000;
};

TEST(EnumForEach, All)
{
  constexpr auto some_type_dec_baz_signed_1 = []() {
    auto dec = 0;
    enum_for_each<baz_signed>([&dec](auto v) {
      dec = dec * 10 + std::to_underlying(v.value);
    });
    return dec;
  };
  EXPECT_EQ(1229, some_type_dec_baz_signed_1());

  constexpr auto some_type_dec_baz_signed_2 = []() {
    auto dec = 0;
    enum_for_each<baz_signed, enum_entry_order::by_name>([&dec](auto v) {
      dec = dec * 10 + std::to_underlying(v.value);
    });
    return dec;
  };
  EXPECT_EQ(19031, some_type_dec_baz_signed_2());

  constexpr auto some_type_sum_dec_baz_signed = []() {
    auto sum = 0;
    auto dec = 0;
    enum_for_each<baz_signed, enum_entry_order::by_value>([&sum, &dec](auto v) {
      sum += some_type_t<v.value>::value;
      dec = dec * 10 + std::to_underlying(v.value);
    });
    return std::pair{sum, dec};
  }();
  EXPECT_EQ(10015, some_type_sum_dec_baz_signed.first);
  EXPECT_EQ(-9877, some_type_sum_dec_baz_signed.second);
}
