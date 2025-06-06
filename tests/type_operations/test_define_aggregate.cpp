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

#include "tests/test_options.hpp"
#include <reflect_cpp26/type_traits/class_types/member_traits.hpp>
#include <deque>
#include <limits>
#include <list>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_operations.hpp>
#else
#include <reflect_cpp26/type_operations/define_aggregate.hpp>
#endif

namespace rfl = reflect_cpp26;

template <class T, size_t N>
using c_style_array_t = T[N];

template <size_t N>
struct to_c_style_array {
  static constexpr auto operator()(std::meta::info member)
  {
    auto dest_type = rfl::substitute(
      ^^c_style_array_t, type_of(member), std::meta::reflect_constant(N));
    return data_member_spec(dest_type, {
      .name = identifier_of(member),
    });
  }
};

struct foo_t {
  int x;
  const float y;
  volatile double z;
  const char* str;
};

// No inheritance. Unary transform function.
using foo_soa_t = rfl::aggregate_by_direct_memberwise_t<
  to_c_style_array<4>, foo_t>;
static_assert(rfl::all_direct_nsdm_of(^^foo_soa_t).size() == 4);

using foo_soa_x_traits = rfl::member_pointer_traits<
  decltype(&foo_soa_t::x)>;
static_assert(std::is_same_v<
  foo_soa_x_traits::target_type, int[4]>);

using foo_soa_y_traits = rfl::member_pointer_traits<
  decltype(&foo_soa_t::y)>;
static_assert(std::is_same_v<
  foo_soa_y_traits::target_type, const float[4]>);

using foo_soa_z_traits = rfl::member_pointer_traits<
  decltype(&foo_soa_t::z)>;
static_assert(std::is_same_v<
  foo_soa_z_traits::target_type, volatile double[4]>);

using foo_soa_str_traits = rfl::member_pointer_traits<
  decltype(&foo_soa_t::str)>;
static_assert(std::is_same_v<
  foo_soa_str_traits::target_type, const char*[4]>);

static_assert(foo_soa_t{
  .x = {123, 234, 345, 456},
  .y = {1.1, 2.2, 3.3, 4.4},
  .z = {5.5, 6.6, 7.7, 8.8},
  .str = {"hello", "world", "from", "C++"}
}.x[0] == 123);

struct bar_A_t {
  char a1;
private:
  char __not_me__;
public:
  float a2;
  double a3;
};

struct bar_B_t {
  short b1;
  char b2[2];
protected:
  char __not_me_either__;
public:
  long long b3;
  char b4;
};

struct bar_C_t {
  int c1;
  double c2;
  short c3[3];
};

struct take_smallest {
  template <class... Args>
  static constexpr auto operator()(Args... members)
  {
    auto res = std::meta::info{};
    auto res_size = std::numeric_limits<size_t>::max();
    for (auto m: {members...}) {
      auto cur_size = size_of(type_of(m));
      if (cur_size < res_size) {
        res_size = cur_size;
        res = m;
      }
    }
    return data_member_spec(type_of(res), {
      .name = identifier_of(res) + std::string{"_x"},
    });
  }
};

// No inheritance. Ternary transform function.
using bar_smallest_t = rfl::aggregate_by_direct_memberwise_t<
  take_smallest, bar_A_t, bar_B_t, bar_C_t>;
static_assert(rfl::all_direct_nsdm_of(^^bar_smallest_t).size() == 3);

static_assert(bar_smallest_t{
  .a1_x = 'a',
  .b2_x = {'b', '2'},
  .c3_x = {123, 456, 789}
}.c3_x[2] == 789);

struct baz_A_t {
  std::list<int> a1;
  std::deque<int> a2;
  std::vector<int> a3;
  std::set<int> a4;
};

struct baz_B_t {
  double b1;
};

struct baz_C_t : baz_B_t {
  const char* c1;
};

struct baz_D_t : baz_C_t, protected foo_t {
  std::array<unsigned, 4> d1;
};

struct replace_value_type {
  static consteval auto operator()(std::meta::info cont, std::meta::info x)
  {
    auto tpl = template_of(type_of(cont));
    auto type = substitute(tpl, {type_of(x)});
    return data_member_spec(type, {
      .name = identifier_of(cont) + std::string{"_"} + identifier_of(x)
    });
  }
};

using baz_replaced_t = rfl::aggregate_by_flattened_memberwise_t<
  replace_value_type, baz_A_t, baz_D_t>;
static_assert(rfl::all_direct_nsdm_of(^^baz_replaced_t).size() == 3);

using baz_replaced_b1_traits =
  rfl::member_pointer_traits<decltype(&baz_replaced_t::a1_b1)>;
static_assert(std::is_same_v<
  std::list<double>, baz_replaced_b1_traits::target_type>);

using baz_replaced_c1_traits =
  rfl::member_pointer_traits<decltype(&baz_replaced_t::a2_c1)>;
static_assert(std::is_same_v<
  std::deque<const char*>, baz_replaced_c1_traits::target_type>);

using baz_replaced_d1_traits =
  rfl::member_pointer_traits<decltype(&baz_replaced_t::a3_d1)>;
static_assert(std::is_same_v<
  std::vector<std::array<unsigned, 4>>, baz_replaced_d1_traits::target_type>);

TEST(TypeOperationsDefineAggregate, All)
{
  // All other test cases done with static-asserts above
  auto baz_replaced = baz_replaced_t{
    .a1_b1 = {1.1, 2.2, 3.3, 4.4, 5.5},
    .a2_c1 = {"hello", "world", "from", "C++"},
    .a3_d1 = {{1u, 2u, 3u, 4u}, {5u, 6u, 7u, 8u}, {9u, 10u, 11u}}
  };
  // Tests whether baz_replaced is constructed successfully.
  EXPECT_EQ(0, baz_replaced.a3_d1[2][3]);
}
