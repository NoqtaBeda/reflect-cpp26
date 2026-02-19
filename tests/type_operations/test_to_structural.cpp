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

#include <cstdio>
#include <numeric>
#include <reflect_cpp26/type_operations/to_structural.hpp>
#include <reflect_cpp26/utils/to_string.hpp>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

// Examples in docs/type_operations.md
namespace examples {
// (1) Range (including initializer list) to meta_span
constexpr auto span_1 = rfl::to_structural(std::vector<int>{1, 2, 3, 4, 5});
static_assert(std::is_same_v<std::remove_const_t<decltype(span_1)>, rfl::meta_span<int>>);

constexpr auto span_2 = rfl::to_structural({"Cat", "Dog", "Rabbit"});
static_assert(
    std::is_same_v<std::remove_const_t<decltype(span_2)>, rfl::meta_span<rfl::meta_string_view>>);

// (2) Tuple-like to meta_tuple
constexpr auto meta_tup =
    rfl::to_structural(std::tuple<int, double, std::u16string>{42, 3.14, u"Hello"});
static_assert(std::is_same_v<std::remove_const_t<decltype(meta_tup)>,
                             rfl::meta_tuple<int, double, rfl::meta_u16string_view>>);

// (3) Const char pointer to meta_string_view
constexpr auto cstr = rfl::to_structural("world");
static_assert(std::is_same_v<std::remove_const_t<decltype(cstr)>, rfl::meta_string_view>);

// (4) Identity
struct point_t {
  int x;
  int y;
};
constexpr auto val = rfl::to_structural(point_t{.x = 12, .y = 34});
static_assert(std::is_same_v<decltype(val), const point_t>);
}  // namespace examples

// Arithmetic types
constexpr auto some_int = 42;
constexpr auto some_int_conv = rfl::to_structural(some_int);
static_assert(std::is_same_v<decltype(some_int_conv), const int>);
static_assert(some_int_conv == some_int);

constexpr auto some_double = 4.25;
constexpr auto some_double_conv = rfl::to_structural(some_double);
static_assert(std::is_same_v<decltype(some_double_conv), const double>);
static_assert(some_double_conv == some_double);

// Pointers
constexpr auto int_ptr_conv = rfl::to_structural(&some_int);
static_assert(std::is_same_v<decltype(int_ptr_conv), const int* const>);
static_assert(*int_ptr_conv == some_int);

constexpr const auto* u16_literal = u"你好，世界";
constexpr auto u16_literal_conv = rfl::to_structural(u16_literal);
static_assert(std::is_same_v<decltype(u16_literal_conv), const rfl::meta_u16string_view>);
static_assert(u16_literal_conv.size() == 5);
static_assert(u16_literal_conv[4] == u'界');
static_assert(*u16_literal_conv.tail == '\0');

constexpr auto printf_fptr = printf;
constexpr auto printf_fptr_conv = rfl::to_structural(printf_fptr);
static_assert(std::is_same_v<decltype(printf_fptr_conv), int (*const)(const char*, ...)>);
static_assert(printf_fptr_conv == printf);

// Implicit conversion required.
constexpr auto printf_conv = rfl::to_structural(printf);
static_assert(std::is_same_v<decltype(printf_conv), int (*const)(const char*, ...)>);
static_assert(printf_conv == printf);

constexpr auto nullptr_conv = rfl::to_structural(nullptr);
static_assert(std::is_same_v<decltype(nullptr_conv), const nullptr_t>);
static_assert(nullptr_conv == nullptr);

struct some_struct_t {
  const char* name;
  int x;
  int y;

  constexpr auto operator<=>(const some_struct_t& rhs) const -> std::weak_ordering = default;
};
constexpr auto some_struct_value = some_struct_t{.name = "hello", .x = 123, .y = 456};

// Pointer to Members
constexpr auto mem_obj_ptr_conv = rfl::to_structural(&some_struct_t::x);
static_assert(std::is_same_v<decltype(mem_obj_ptr_conv), int some_struct_t::* const>);
static_assert(some_struct_value.*mem_obj_ptr_conv == 123);

constexpr auto mem_fn_ptr_conv = rfl::to_structural(&some_struct_t::operator<=>);
static_assert(std::is_same_v<decltype(mem_fn_ptr_conv),
                             std::weak_ordering (some_struct_t::* const)(const some_struct_t&)
                                 const noexcept>);
static_assert((some_struct_value.*mem_fn_ptr_conv)(some_struct_value)
              == std::weak_ordering::equivalent);

// Enum Types
constexpr auto errc_conv = rfl::to_structural(std::errc::connection_reset);
static_assert(std::is_same_v<decltype(errc_conv), const std::errc>);
static_assert(errc_conv == std::errc::connection_reset);

// Lambdas without Capture
constexpr auto lambda = [](const char* str) { std::println("{}", str); };
constexpr auto lambda_conv = rfl::to_structural(lambda);
static_assert(std::is_same_v<decltype(lambda_conv), decltype(lambda)>);
static_assert(lambda_conv == lambda);

// C-style Array Types
constexpr long long_array[] = {1, 2, 3, 4, 6, 8};
constexpr auto long_array_conv = rfl::to_structural(long_array);
static_assert(std::is_same_v<decltype(long_array_conv), const rfl::meta_span<long>>);
static_assert(long_array_conv.size() == 6);
static_assert(long_array_conv[3] == 4);

constexpr char char_array[] = {'W', 'e', 'l', 'c', 'o', 'm', 'e'};
constexpr auto char_array_conv = rfl::to_structural(char_array);
static_assert(std::is_same_v<decltype(char_array_conv), const rfl::meta_string_view>);
static_assert(char_array_conv.size() == 7);
static_assert(char_array_conv[4] == 'o');
static_assert(*char_array_conv.tail == '\0');

// std::initializer_list
constexpr auto float_il_conv = rfl::to_structural({1.0f, 2.5f, 3.75f, 5.875f});
static_assert(std::is_same_v<decltype(float_il_conv), const rfl::meta_span<float>>);
static_assert(float_il_conv.size() == 4);
static_assert(float_il_conv[2] == 3.75f);

constexpr auto char_il_conv = rfl::to_structural({'x', 'y', 'z', 'a', 'b', 'c'});
static_assert(std::is_same_v<decltype(char_il_conv), const rfl::meta_string_view>);
static_assert(char_il_conv == "xyzabc");

// std::vector
constexpr auto fib_until(unsigned x) -> std::vector<unsigned> {
  auto res = std::vector<unsigned>{1u, 2u};
  while (true) {
    auto n = res.size();
    auto next = res[n - 1] + res[n - 2];
    if (next > x) {
      break;
    }
    res.push_back(next);
  }
  return res;
}
constexpr auto fib_100_conv = rfl::to_structural(fib_until(100));
static_assert(std::is_same_v<decltype(fib_100_conv), const rfl::meta_span<unsigned>>);
// 1, 2, 3, 5, 8, 13, 21, 34, 55, 89
static_assert(fib_100_conv.size() == 10);
static_assert(fib_100_conv.back() == 89);
static_assert(std::reduce(fib_100_conv.begin(), fib_100_conv.end()) == 231);

// std::string
constexpr auto fib_str_until(unsigned x) -> std::vector<std::string> {
  auto values = std::vector<unsigned>{1u, 2u};
  while (true) {
    auto n = values.size();
    auto next = values[n - 1] + values[n - 2];
    if (next > x) {
      break;
    }
    values.push_back(next);
  }
  auto res = std::vector<std::string>{};
  for (auto i = 0zU, n = values.size(); i < n; i++) {
    res.push_back("[" + rfl::to_string(i) + "]=" + rfl::to_string(values[i]));
  }
  return res;
}
constexpr auto fib_str_100_conv = rfl::to_structural(fib_str_until(100));
static_assert(
    std::is_same_v<decltype(fib_str_100_conv), const rfl::meta_span<rfl::meta_string_view>>);
static_assert(fib_str_100_conv.size() == 10);
static_assert(fib_str_100_conv.back() == "[9]=89");

// std::tuple<long, std::vector<long>,
//            std::wstring, std::vector<std::wstring>>
constexpr auto make_ds_tuple_1(unsigned n) {
  auto to_wstring = [](auto value) {
    auto str = rfl::to_string(value);
    auto wstr = std::wstring{};
    wstr.assign_range(str);
    return wstr;
  };
  auto long_values = std::vector<long>{};
  for (auto i = 0u; i < n; i++) {
    long_values.push_back(i * i);
  }
  auto sum = std::reduce(long_values.begin(), long_values.end());
  auto wstring_values = std::vector<std::wstring>{};
  for (auto i = 0u; i < n; i++) {
    wstring_values.push_back(to_wstring(i * i));
  }
  auto sum_wstr = to_wstring(sum);
  return std::tuple{sum, std::move(long_values), std::move(sum_wstr), std::move(wstring_values)};
}
constexpr auto ds_tuple_1_conv = rfl::to_structural(make_ds_tuple_1(10));
using ds_tuple_1_conv_expected_type = rfl::meta_tuple<long,
                                                      rfl::meta_span<long>,
                                                      rfl::meta_wstring_view,
                                                      rfl::meta_span<rfl::meta_wstring_view>>;
static_assert(std::is_same_v<decltype(ds_tuple_1_conv), const ds_tuple_1_conv_expected_type>);
static_assert(get<0>(ds_tuple_1_conv) == 285);
static_assert(get<1>(ds_tuple_1_conv).size() == 10);
static_assert(get<1>(ds_tuple_1_conv)[9] == 81);
static_assert(get<2>(ds_tuple_1_conv) == L"285");
static_assert(get<3>(ds_tuple_1_conv).size() == 10);
static_assert(get<3>(ds_tuple_1_conv)[8] == L"64");

// Nested tuple type:
// std::tuple<long, std::vector<long>,
//            std::wstring, std::vector<std::wstring>,
//            std::tuple<...>>
template <unsigned N>
constexpr auto make_ds_tuple_2() {
  if constexpr (N == 0) {
    return make_ds_tuple_1(10);
  } else {
    auto next = make_ds_tuple_2<N - 1>();
    return std::tuple_cat(make_ds_tuple_1(10 - N), std::tuple<decltype(next)>{std::move(next)});
  }
}
constexpr auto ds_tuple_2_conv = rfl::to_structural(make_ds_tuple_2<2>());
using ds_tuple_2_conv_expected_type =
    rfl::meta_tuple<long,
                    rfl::meta_span<long>,
                    rfl::meta_wstring_view,
                    rfl::meta_span<rfl::meta_wstring_view>,
                    rfl::meta_tuple<long,
                                    rfl::meta_span<long>,
                                    rfl::meta_wstring_view,
                                    rfl::meta_span<rfl::meta_wstring_view>,
                                    rfl::meta_tuple<long,
                                                    rfl::meta_span<long>,
                                                    rfl::meta_wstring_view,
                                                    rfl::meta_span<rfl::meta_wstring_view>>>>;
static_assert(std::is_same_v<decltype(ds_tuple_2_conv), const ds_tuple_2_conv_expected_type>);
static_assert(get<2>(ds_tuple_2_conv) == L"140");
static_assert(get<0>(get<4>(ds_tuple_2_conv)) == 204);
static_assert(get<2>(get<4>(get<4>(ds_tuple_2_conv))) == L"285");

// std::vector<std::tuple<...>>
constexpr auto make_ds_tuple_list(unsigned n) {
  return std::views::iota(1u, n) | std::views::transform(make_ds_tuple_1)
       | std::ranges::to<std::vector>();
}
constexpr auto ds_tuple_list_conv = rfl::to_structural(make_ds_tuple_list(10));
static_assert(std::is_same_v<decltype(ds_tuple_list_conv),
                             const rfl::meta_span<ds_tuple_1_conv_expected_type>>);
static_assert(ds_tuple_list_conv.size() == 9);
static_assert(get<2>(ds_tuple_list_conv[7]) == L"140");
static_assert(std::accumulate(get<3>(ds_tuple_list_conv[6]).begin(),
                              get<3>(ds_tuple_list_conv[6]).end(),
                              std::wstring{})
              == L"0149162536");

// to_structural_result_t
static_assert(std::is_same_v<rfl::to_structural_result_t<int>, int>);
static_assert(std::is_same_v<rfl::to_structural_result_t<std::vector<int>>, rfl::meta_span<int>>);
static_assert(
    std::is_same_v<rfl::to_structural_result_t<std::initializer_list<std::u32string_view>>,
                   rfl::meta_span<rfl::meta_u32string_view>>);
static_assert(std::is_same_v<rfl::to_structural_result_t<decltype(make_ds_tuple_1(10))>,
                             ds_tuple_1_conv_expected_type>);
static_assert(std::is_same_v<
              rfl::to_structural_result_t<std::initializer_list<decltype(make_ds_tuple_1(10))>>,
              rfl::meta_span<ds_tuple_1_conv_expected_type>>);

TEST(TypeOperationsTostructural, StaticAll) {
  EXPECT_TRUE(true);  // All tested with static assertions above.
}
