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
#include <reflect_cpp26/enum/enum_switch.hpp>
#endif

using namespace reflect_cpp26;
using namespace std::literals;

template <class E, unsigned p>
struct underlying_pow_t {
  static constexpr auto operator()(E value)
  {
    auto x = std::to_underlying(value);
    auto res = static_cast<decltype(x)>(1);
    for (auto i = p; i > 0; i--) {
      res *= x;
    }
    return res;
  }
};

TEST(EnumSwitch, Basic)
{
  constexpr auto pos_res_1 = enum_switch<int>(
    underlying_pow_t<foo_signed, 2>{}, foo_signed::four);
  ASSERT_TRUE_STATIC(pos_res_1.has_value());
  EXPECT_EQ_STATIC(16, *pos_res_1);

  constexpr auto pos_res_2 = enum_switch<uint32_t>(
    underlying_pow_t<foo_signed, 3>{}, foo_signed::error);
  ASSERT_TRUE_STATIC(pos_res_2.has_value());
  EXPECT_EQ_STATIC(4294967288, *pos_res_2);

  constexpr auto pos_res_3 = enum_switch<int>(
    underlying_pow_t<bar_unsigned, 5>{}, bar_unsigned::error);
  ASSERT_TRUE_STATIC(pos_res_3.has_value());
  EXPECT_EQ_STATIC(-32, *pos_res_3);

  constexpr auto pos_res_4 = enum_switch<unsigned>(
    underlying_pow_t<bar_unsigned, 5>{}, static_cast<bar_unsigned>(1234));
  EXPECT_EQ_STATIC(std::nullopt, pos_res_4);
}

constexpr auto constant_tuple = std::tuple{
  short{12},
  "const char*",
  int{34},
  "std::string_view"sv,
  long{56},
};

struct get_from_constant_tuple_t {
  static constexpr auto operator()(auto ec)
  {
    constexpr auto I = std::to_underlying(ec.value);
    return get<I>(constant_tuple);
  }
};

enum class string_ref_indices {
  const_char_ptr = 1,
  std_string_view = 3,
};

enum class integer_indices {
  index_short = 0,
  index_int = 2,
  index_long = 4,
};

TEST(EnumSwitch, ToCommon)
{
  constexpr auto get_res_1 = enum_switch_to_common(
    get_from_constant_tuple_t{},
    string_ref_indices::const_char_ptr,
    "abc");
  // common_type_t<const char*, std::string_view, const char[4]>
  EXPECT_TRUE_STATIC(
    std::is_same_v<decltype(get_res_1), const std::string_view>);
  EXPECT_EQ_STATIC("const char*", get_res_1);

  constexpr auto get_res_2 = enum_switch_to_common(
    get_from_constant_tuple_t{},
    string_ref_indices::std_string_view,
    "def"s);
  // common_type_t<const char*, std::string_view, std::string>
  EXPECT_TRUE_STATIC(
    std::is_same_v<decltype(get_res_2), const std::string_view>);
  EXPECT_EQ_STATIC("std::string_view", get_res_2);

  constexpr auto get_res_3 = enum_switch_to_common(
    get_from_constant_tuple_t{},
    static_cast<string_ref_indices>(1234),
    "ghi");
  // common_type_t<const char*, std::string_view, const char[4]>
  EXPECT_TRUE_STATIC(
    std::is_same_v<decltype(get_res_3), const std::string_view>);
  EXPECT_EQ_STATIC("ghi", get_res_3);

  constexpr auto get_res_4 = enum_switch_to_common(
    get_from_constant_tuple_t{},
    integer_indices::index_short,
    1234u);
  // common_type_t<short, int, long, unsigned>
  EXPECT_TRUE_STATIC(std::is_same_v<decltype(get_res_4), const long>);
  EXPECT_EQ_STATIC(12, get_res_4);

  constexpr auto get_res_5 = enum_switch_to_common(
    get_from_constant_tuple_t{},
    integer_indices::index_int,
    9.75f);
  // common_type_t<short, int, long, float>
  EXPECT_TRUE_STATIC(std::is_same_v<decltype(get_res_5), const float>);
  EXPECT_EQ_STATIC(34, get_res_5);

  constexpr auto get_res_6 = enum_switch_to_common(
    get_from_constant_tuple_t{},
    static_cast<integer_indices>(-1),
    constant<1234>{});
  // common_type_t<short, int, long, constant<int>>
  EXPECT_TRUE_STATIC(std::is_same_v<decltype(get_res_6), const long>);
  EXPECT_EQ_STATIC(1234, get_res_6);

  constexpr auto get_res_7 = enum_switch(
    get_from_constant_tuple_t{},
    integer_indices::index_long,
    '\0');
    EXPECT_TRUE_STATIC(std::is_same_v<decltype(get_res_7), const char>);
  EXPECT_EQ_STATIC('\x38', get_res_7);

  constexpr auto get_res_8 = enum_switch(
    get_from_constant_tuple_t{},
    static_cast<integer_indices>(-1),
    L'\0');
    EXPECT_TRUE_STATIC(std::is_same_v<decltype(get_res_8), const wchar_t>);
  EXPECT_EQ_STATIC('\0', get_res_8);
}

struct get_from_non_constant_tuple_t {
  static constexpr auto& operator()(auto* tuple, auto ec)
  {
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

TEST(EnumSwitch, Dereference)
{
  auto array = std::array{"abc"s, "def"s, "ghi"s, "jkl"s};
  auto default_str = "<n/a>"s;

  get_from_non_constant_tuple_t{}(
    &array, constant<array_index::zero>{}) += "<test>";
  EXPECT_EQ("abc<test>", array[0]) << "Implementation error with test case.";

  auto&& str1 = enum_switch(
    std::bind_front(get_from_non_constant_tuple_t{}, &array),
    array_index::two, default_str);
  str1 += "<changed>";
  EXPECT_EQ("ghi<changed>", str1);
  EXPECT_EQ("ghi", array[2]);

  auto&& str2 = enum_switch(
    std::bind_front(get_from_non_constant_tuple_t{}, &array),
    static_cast<array_index>(4), default_str);
  str2 += "<changed>";
  EXPECT_EQ("<n/a><changed>", str2);
  EXPECT_EQ("<n/a>", default_str);
}
