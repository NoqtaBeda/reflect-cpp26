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

#include <string>

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/enum.hpp>
#else
#include <reflect_cpp26/enum/enum_switch.hpp>
#endif

using namespace reflect_cpp26;
using namespace std::literals;

template <class E, unsigned p>
struct underlying_pow_t {
  static constexpr auto operator()(E value) {
    auto x = std::to_underlying(value);
    auto res = static_cast<decltype(x)>(1);
    for (auto i = p; i > 0; i--) {
      res *= x;
    }
    return res;
  }
};

TEST(EnumSwitch, WithReturnValue) {
  constexpr auto pos_res_1 = enum_switch<int>(underlying_pow_t<foo_signed, 2>{}, foo_signed::four);
  ASSERT_TRUE_STATIC(pos_res_1.has_value());
  EXPECT_EQ_STATIC(16, *pos_res_1);

  constexpr auto pos_res_2 =
      enum_switch<uint32_t>(underlying_pow_t<foo_signed, 3>{}, foo_signed::error);
  ASSERT_TRUE_STATIC(pos_res_2.has_value());
  EXPECT_EQ_STATIC(4294967288, *pos_res_2);

  constexpr auto pos_res_3 =
      enum_switch<int>(underlying_pow_t<bar_unsigned, 5>{}, bar_unsigned::error);
  ASSERT_TRUE_STATIC(pos_res_3.has_value());
  EXPECT_EQ_STATIC(-32, *pos_res_3);

  constexpr auto pos_res_4 =
      enum_switch<unsigned>(underlying_pow_t<bar_unsigned, 5>{}, static_cast<bar_unsigned>(1234));
  EXPECT_EQ_STATIC(std::nullopt, pos_res_4);
}

struct underlying_to_string_t {
  std::string* dest;

  template <class E, auto V>
  void operator()(std::integral_constant<E, V>) {
    *dest = "<match> " + std::to_string(std::to_underlying(V));
  }
};

TEST(EnumSwitch, WithoutReturnValue) {
  auto res = std::string{};
  enum_switch(underlying_to_string_t{&res}, bar_unsigned::eleven);
  EXPECT_EQ("<match> 11", res);
  enum_switch(underlying_to_string_t{&res}, static_cast<bar_unsigned>(4));
  EXPECT_EQ("<match> 11", res);  // No change

  enum_switch(
      underlying_to_string_t{&res},
      [&res](bar_unsigned value) {
        res = "<invalid> " + std::to_string(std::to_underlying(value));
      },
      bar_unsigned::twelve);
  EXPECT_EQ("<match> 12", res);

  enum_switch(
      underlying_to_string_t{&res},
      [&res](bar_unsigned value) {
        res = "<invalid> " + std::to_string(std::to_underlying(value));
      },
      static_cast<bar_unsigned>(5));
  EXPECT_EQ("<invalid> 5", res);
}

constexpr auto constant_tuple = std::tuple{
    short{12},
    "const char*",
    int{34},
    "std::string_view"sv,
    long{56},
};

struct get_from_constant_tuple_t {
  static constexpr auto operator()(auto ec) {
    constexpr auto I = std::to_underlying(ec());
    return get<I>(constant_tuple);
  }
};

struct get_from_non_constant_tuple_t {
  static constexpr auto& operator()(auto* tuple, auto ec) {
    constexpr auto I = std::to_underlying(ec.value);
    return get<I>(*tuple);
  }
};

enum class array_index {
  zero = 0,
  one = 1,
  two = 2,
  three = 3,
};

TEST(EnumSwitch, Dereference) {
  auto array = std::array{"abc"s, "def"s, "ghi"s, "jkl"s};
  auto default_str = "<n/a>"s;

  get_from_non_constant_tuple_t{}(&array, constant_v<array_index::zero>) += "<test>";
  EXPECT_EQ("abc<test>", array[0]) << "Implementation error with test case.";

  auto&& str1 = enum_switch(
      std::bind_front(get_from_non_constant_tuple_t{}, &array), array_index::two, default_str);
  str1 += "<changed>";
  EXPECT_EQ("ghi<changed>", str1);
  EXPECT_EQ("ghi", array[2]);

  auto&& str2 = enum_switch(std::bind_front(get_from_non_constant_tuple_t{}, &array),
                            static_cast<array_index>(4),
                            default_str);
  str2 += "<changed>";
  EXPECT_EQ("<n/a><changed>", str2);
  EXPECT_EQ("<n/a>", default_str);
}
