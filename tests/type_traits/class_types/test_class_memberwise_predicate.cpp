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
#include <reflect_cpp26/utils/functional.hpp>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/class_types/memberwise_predicate.hpp>
#endif

namespace rfl = reflect_cpp26;

struct foo_t {
  int a;
  long abc;
  double abcde;
private:
  const char* name;
};

// No inheritance, unary predicate
static_assert(rfl::all_of_direct_memberwise_v<
  std::is_arithmetic, foo_t>);
static_assert(NOT rfl::all_of_direct_memberwise_v<
  std::is_integral, foo_t>);
static_assert(rfl::all_of_flattened_memberwise_v<
  std::is_arithmetic, foo_t>);
static_assert(NOT rfl::all_of_flattened_memberwise_v<
  std::is_integral, foo_t>);

static_assert(rfl::any_of_direct_memberwise_v<
  std::is_floating_point, foo_t>);
static_assert(NOT rfl::any_of_direct_memberwise_v<
  std::is_reference, foo_t>);
static_assert(rfl::any_of_flattened_memberwise_v<
  std::is_floating_point, foo_t>);
static_assert(NOT rfl::any_of_flattened_memberwise_v<
  std::is_reference, foo_t>);

static_assert(rfl::none_of_direct_memberwise_v<
  std::is_pointer, foo_t>);
static_assert(NOT rfl::none_of_direct_memberwise_v<
  std::is_integral, foo_t>);
static_assert(rfl::none_of_flattened_memberwise_v<
  std::is_pointer, foo_t>);
static_assert(NOT rfl::none_of_flattened_memberwise_v<
  std::is_integral, foo_t>);

struct identifier_length_is_odd {
  static consteval auto operator()(std::meta::info member) {
    return identifier_of(member).size() % 2 != 0;
  }
};

struct identifier_length_is_even {
  static consteval auto operator()(std::meta::info member) {
    return identifier_of(member).size() % 2 == 0;
  }
};

template <class Comp, size_t N>
struct identifier_length_compare {
  static consteval auto operator()(std::meta::info member) {
    return Comp::operator()(identifier_of(member).size(), N);
  }
};

// No inheritance, unary predicate(std::meta::info)
static_assert(rfl::all_of_direct_memberwise_meta_v<
  identifier_length_is_odd, foo_t>);
static_assert(NOT rfl::all_of_direct_memberwise_meta_v<
  identifier_length_compare<rfl::less_equal_t, 4>, foo_t>);
static_assert(rfl::all_of_flattened_memberwise_meta_v<
  identifier_length_is_odd, foo_t>);
static_assert(NOT rfl::all_of_flattened_memberwise_meta_v<
  identifier_length_compare<rfl::less_equal_t, 4>, foo_t>);

static_assert(rfl::any_of_direct_memberwise_meta_v<
  identifier_length_compare<rfl::less_equal_t, 2>, foo_t>);
static_assert(NOT rfl::any_of_direct_memberwise_meta_v<
  identifier_length_is_even, foo_t>);
static_assert(rfl::any_of_flattened_memberwise_meta_v<
  identifier_length_compare<rfl::less_equal_t, 2>, foo_t>);
static_assert(NOT rfl::any_of_flattened_memberwise_meta_v<
  identifier_length_is_even, foo_t>);

static_assert(rfl::none_of_direct_memberwise_meta_v<
  identifier_length_is_even, foo_t>);
static_assert(NOT rfl::none_of_direct_memberwise_meta_v<
  identifier_length_compare<rfl::greater_t, 2>, foo_t>);
static_assert(rfl::none_of_flattened_memberwise_meta_v<
  identifier_length_is_even, foo_t>);
static_assert(NOT rfl::none_of_flattened_memberwise_meta_v<
  identifier_length_compare<rfl::greater_t, 2>, foo_t>);

struct bar_t {
  std::partial_ordering comp_res;
  std::errc ec;
  std::byte b;
  std::byte c;
};

struct type_identifier_length_is_odd {
  static consteval auto operator()(rfl::flattened_data_member_info spec) {
    return identifier_of(type_of(spec.member)).size() % 2 == 1;
  }
};

struct type_identifier_length_is_even {
  static consteval auto operator()(rfl::flattened_data_member_info spec) {
    return identifier_of(type_of(spec.member)).size() % 2 == 0;
  }
};

struct actual_offset_is_odd {
  static consteval auto operator()(rfl::flattened_data_member_info spec) {
    return spec.actual_offset.bytes % 2 == 1;
  }
};

// No inheritance, unary predicate(reflect_cpp26::flattened_data_member_info)
static_assert(rfl::all_of_flattened_memberwise_meta_v<
  type_identifier_length_is_even, bar_t>);
static_assert(NOT rfl::all_of_flattened_memberwise_meta_v<
  actual_offset_is_odd, bar_t>);

static_assert(rfl::any_of_flattened_memberwise_meta_v<
  actual_offset_is_odd, bar_t>);
static_assert(NOT rfl::any_of_flattened_memberwise_meta_v<
  type_identifier_length_is_odd, bar_t>);

static_assert(rfl::none_of_flattened_memberwise_meta_v<
  type_identifier_length_is_odd, bar_t>);
static_assert(NOT rfl::none_of_flattened_memberwise_meta_v<
  actual_offset_is_odd, bar_t>);

struct baz_t : foo_t, bar_t {
  size_t index;
  intptr_t offset;
};

// Inheritance, unary predicate
static_assert(rfl::all_of_direct_memberwise_v<
  std::is_integral, baz_t>);
static_assert(NOT rfl::all_of_flattened_memberwise_v<
  std::is_integral, baz_t>);

static_assert(NOT rfl::any_of_direct_memberwise_v<
  std::is_floating_point, baz_t>);
static_assert(rfl::any_of_flattened_memberwise_v<
  std::is_floating_point, baz_t>);

static_assert(rfl::none_of_direct_memberwise_v<
  std::is_pointer, baz_t>);
static_assert(rfl::none_of_flattened_memberwise_v<
  std::is_pointer, baz_t>);
static_assert(rfl::none_of_direct_memberwise_v<
  std::is_floating_point, baz_t>);
static_assert(NOT rfl::none_of_flattened_memberwise_v<
  std::is_floating_point, baz_t>);

static_assert(NOT rfl::any_of_direct_memberwise_meta_v<
  identifier_length_compare<rfl::less_t, 3>, baz_t>);
static_assert(rfl::any_of_flattened_memberwise_meta_v<
  identifier_length_compare<rfl::less_t, 3>, baz_t>);
static_assert(NOT rfl::none_of_flattened_memberwise_meta_v<
  actual_offset_is_odd, baz_t>);

struct qux_A_t {
  int32_t a;
  int64_t b;
};

struct qux_B_t {
  float c;
  double d;
  double e[4];
};

struct qux_C_t : qux_A_t {
  uint32_t f[8];
  uint16_t g[5];
};

template <class T, class... Args>
struct sizes_are_equal {
  static constexpr auto value = ((sizeof(T) == sizeof(Args)) && ...);
};

struct all_are_non_arrays {
  template <class... Args>
    requires (std::is_same_v<Args, std::meta::info> && ...)
  static constexpr auto operator()(Args... args) {
    return ((!is_array_type(type_of(args))) && ...);
  }
};

struct all_are_non_arrays_by_spec {
  template <class... Args>
    requires (std::is_same_v<Args, rfl::flattened_data_member_info> && ...)
  static constexpr auto operator()(Args... args) {
    return ((!is_array_type(type_of(args.member))) && ...);
  }
};

// Binary & ternary predicates
static_assert(rfl::all_of_direct_memberwise_v<
  sizes_are_equal, qux_A_t, qux_B_t>);
static_assert(rfl::none_of_direct_memberwise_v<
  sizes_are_equal, qux_B_t, qux_C_t>);
static_assert(rfl::all_of_flattened_memberwise_v<
  sizes_are_equal, qux_B_t, qux_C_t>);
static_assert(rfl::none_of_direct_memberwise_v<
  sizes_are_equal, qux_A_t, qux_B_t, qux_C_t>);
static_assert(rfl::any_of_flattened_memberwise_v<
  sizes_are_equal, qux_A_t, qux_B_t, qux_C_t>);

static_assert(rfl::all_of_direct_memberwise_meta_v<
  all_are_non_arrays, qux_A_t, qux_B_t>);
static_assert(rfl::none_of_direct_memberwise_meta_v<
  all_are_non_arrays, qux_B_t, qux_C_t>);
static_assert(rfl::any_of_flattened_memberwise_meta_v<
  all_are_non_arrays, qux_B_t, qux_C_t>);
static_assert(rfl::none_of_direct_memberwise_meta_v<
  all_are_non_arrays, qux_A_t, qux_B_t, qux_C_t>);
static_assert(rfl::all_of_flattened_memberwise_meta_v<
  all_are_non_arrays, qux_A_t, qux_B_t, qux_C_t>);

static_assert(rfl::any_of_flattened_memberwise_meta_v<
  all_are_non_arrays_by_spec, qux_B_t, qux_C_t>);
static_assert(NOT rfl::all_of_flattened_memberwise_meta_v<
  all_are_non_arrays_by_spec, qux_B_t, qux_C_t>);
static_assert(rfl::all_of_flattened_memberwise_meta_v<
  all_are_non_arrays_by_spec, qux_A_t, qux_B_t, qux_C_t>);

TEST(TypeTraitsClassTypes, MemberwisePredicate) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
