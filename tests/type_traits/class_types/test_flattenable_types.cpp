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
#include <system_error>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#endif

namespace rfl = reflect_cpp26;

// Arithmetic
static_assert(rfl::is_partially_flattenable_v<int>);
static_assert(rfl::is_partially_flattenable_v<const long>);
static_assert(rfl::is_partially_flattenable_v<volatile float>);

static_assert(rfl::is_flattenable_v<int>);
static_assert(rfl::is_flattenable_v<const long>);
static_assert(rfl::is_flattenable_v<volatile float>);

static_assert(NOT rfl::is_flattenable_aggregate_v<int>);
static_assert(NOT rfl::is_flattenable_aggregate_v<const long>);
static_assert(NOT rfl::is_flattenable_aggregate_v<volatile float>);

// Enum
static_assert(rfl::is_partially_flattenable_v<std::byte>);
static_assert(rfl::is_partially_flattenable_v<std::errc>);

static_assert(rfl::is_flattenable_v<std::byte>);
static_assert(rfl::is_flattenable_v<std::errc>);

static_assert(NOT rfl::is_flattenable_aggregate_v<std::byte>);
static_assert(NOT rfl::is_flattenable_aggregate_v<std::errc>);

// Pointers
using std_pair = std::pair<double, double>;
using std_vector = std::vector<double>;
static_assert(rfl::is_partially_flattenable_v<char*>);
static_assert(rfl::is_partially_flattenable_v<const long* volatile>);
static_assert(rfl::is_partially_flattenable_v<std::nullptr_t>);
static_assert(rfl::is_partially_flattenable_v<double std_pair::*>);
static_assert(rfl::is_partially_flattenable_v<void (std_vector::*)(size_t)>);

static_assert(rfl::is_flattenable_v<char*>);
static_assert(rfl::is_flattenable_v<const long* volatile>);
static_assert(rfl::is_flattenable_v<std::nullptr_t>);
static_assert(rfl::is_flattenable_v<double std_pair::*>);
static_assert(rfl::is_flattenable_v<void (std_vector::*)(size_t)>);

static_assert(NOT rfl::is_flattenable_aggregate_v<char*>);
static_assert(NOT rfl::is_flattenable_aggregate_v<const long* volatile>);
static_assert(NOT rfl::is_flattenable_aggregate_v<std::nullptr_t>);
static_assert(NOT rfl::is_flattenable_aggregate_v<double std_pair::*>);
static_assert(NOT rfl::is_flattenable_aggregate_v<void (std_vector::*)(int)>);

// References
static_assert(NOT rfl::is_partially_flattenable_v<int&>);
static_assert(NOT rfl::is_partially_flattenable_v<const std_pair&>);
static_assert(NOT rfl::is_partially_flattenable_v<float&&>);
static_assert(NOT rfl::is_partially_flattenable_v<const std_pair&&>);

static_assert(NOT rfl::is_flattenable_v<int&>);
static_assert(NOT rfl::is_flattenable_v<const std_pair&>);
static_assert(NOT rfl::is_flattenable_v<float&&>);
static_assert(NOT rfl::is_flattenable_v<const std_pair&&>);

static_assert(NOT rfl::is_flattenable_aggregate_v<int&>);
static_assert(NOT rfl::is_flattenable_aggregate_v<const std_pair&>);
static_assert(NOT rfl::is_flattenable_aggregate_v<float&&>);
static_assert(NOT rfl::is_flattenable_aggregate_v<const std_pair&&>);

// Arrays
using std_vector = std::vector<double>;
static_assert(rfl::is_partially_flattenable_v<long[]>);
static_assert(rfl::is_partially_flattenable_v<double[16]>);
static_assert(rfl::is_partially_flattenable_v<volatile std_vector[]>);
static_assert(rfl::is_partially_flattenable_v<std_vector[32]>);

static_assert(rfl::is_flattenable_v<long[]>);
static_assert(rfl::is_flattenable_v<double[16]>);
static_assert(rfl::is_flattenable_v<volatile std_vector[]>);
static_assert(rfl::is_flattenable_v<std_vector[32]>);

static_assert(rfl::is_flattenable_aggregate_v<long[]>);
static_assert(rfl::is_flattenable_aggregate_v<double[16]>);
static_assert(rfl::is_flattenable_aggregate_v<volatile std_vector[]>);
static_assert(rfl::is_flattenable_aggregate_v<std_vector[32]>);

union some_union_t {
  int as_int;
  float as_float;
  const char* as_char_literal;
};

// Unions
static_assert(NOT rfl::is_partially_flattenable_v<some_union_t>);
static_assert(NOT rfl::is_flattenable_v<some_union_t>);
static_assert(NOT rfl::is_flattenable_aggregate_v<some_union_t>);

struct my_pair {
  std::string first;
  std::string second;
};

struct my_pair_with_ctor {
  std::string first;
  std::string second;
  my_pair_with_ctor() = default;
};

// Class types: Basic
static_assert(rfl::is_partially_flattenable_v<std::monostate>);
static_assert(rfl::is_partially_flattenable_v<const std::to_chars_result>);
static_assert(rfl::is_partially_flattenable_v<volatile std_pair>);
static_assert(rfl::is_partially_flattenable_v<volatile my_pair>);
static_assert(rfl::is_partially_flattenable_v<my_pair_with_ctor>);
static_assert(rfl::is_partially_flattenable_v<const volatile std_vector>);

static_assert(rfl::is_flattenable_v<std::monostate>);
static_assert(rfl::is_flattenable_v<const std::to_chars_result>);
static_assert(rfl::is_flattenable_v<volatile std_pair>);
static_assert(rfl::is_flattenable_v<volatile my_pair>);
static_assert(rfl::is_flattenable_v<my_pair_with_ctor>);
static_assert(NOT rfl::is_flattenable_v<const volatile std_vector>);

static_assert(rfl::is_flattenable_aggregate_v<std::monostate>);
static_assert(rfl::is_flattenable_aggregate_v<const std::to_chars_result>);
static_assert(rfl::is_flattenable_aggregate_v<volatile my_pair>);
static_assert(NOT rfl::is_flattenable_aggregate_v<my_pair_with_ctor>);
static_assert(NOT rfl::is_flattenable_aggregate_v<const volatile std_vector>);

struct my_pair_with_ctor_extended : my_pair_with_ctor {
  std::string third;
  std::string fourth;
};
static_assert(std::is_aggregate_v<my_pair_with_ctor_extended>,
  "Incorrect test case.");

// Class types: Non-aggregate base class
static_assert(rfl::is_partially_flattenable_v<my_pair_with_ctor_extended>);
static_assert(rfl::is_flattenable_v<my_pair_with_ctor_extended>);
static_assert(NOT rfl::is_flattenable_aggregate_v<my_pair_with_ctor_extended>);

struct references_t {
  int& i;
  const long& cl;
  volatile float& vf;
  const volatile double& cvd;
};
static_assert(std::is_aggregate_v<references_t>, "Incorrect test case.");

// Class types: With reference members
static_assert(rfl::is_partially_flattenable_v<references_t>);
static_assert(rfl::is_flattenable_v<references_t>);
static_assert(rfl::is_flattenable_aggregate_v<references_t>);

struct bit_fields_t {
  int16_t flag: 1;
  int16_t x: 4;
  int16_t y: 8;
  int16_t z: 12;
};
static_assert(std::is_aggregate_v<bit_fields_t>, "Incorrect test case.");

// Class types: With bit-field members
static_assert(rfl::is_partially_flattenable_v<bit_fields_t>);
static_assert(rfl::is_flattenable_v<bit_fields_t>);
static_assert(rfl::is_flattenable_aggregate_v<bit_fields_t>);

struct struct_not_destructible_t {
  int x;
  int y;
  ~struct_not_destructible_t() = delete;
};

struct struct_not_trivially_destructible_t {
  int x;
  int y;
  const int z;
  ~struct_not_trivially_destructible_t() {
    std::println("z = {}", z);
  }
};

// Class types: Not (trivially) destructible
static_assert(rfl::is_partially_flattenable_v<
  struct_not_destructible_t>);
static_assert(rfl::is_partially_flattenable_v<
  struct_not_trivially_destructible_t>);

static_assert(rfl::is_flattenable_v<struct_not_destructible_t>);
static_assert(rfl::is_flattenable_v<struct_not_trivially_destructible_t>);

static_assert(rfl::is_flattenable_aggregate_v<
  struct_not_destructible_t>);
static_assert(rfl::is_flattenable_aggregate_v<
  struct_not_trivially_destructible_t>);

struct my_pair_extended_A : my_pair {
  size_t index;
};

struct my_pair_extended_B : my_pair_extended_A {
  double values[16];
  size_t index; // Shadows my_pair_extended_A::index
  size_t size;
};

struct my_pair_extended_C : my_pair_extended_B, std::to_chars_result {
  const double* ptr; // Shadows std::to_chars_result::ptr
};

// Class types: Inheritance
static_assert(rfl::is_partially_flattenable_v<my_pair_extended_A>);
static_assert(rfl::is_partially_flattenable_v<my_pair_extended_B>);
static_assert(rfl::is_partially_flattenable_v<my_pair_extended_C>);

static_assert(rfl::is_flattenable_v<my_pair_extended_A>);
static_assert(rfl::is_flattenable_v<my_pair_extended_B>);
static_assert(rfl::is_flattenable_v<my_pair_extended_C>);

static_assert(rfl::is_flattenable_aggregate_v<my_pair_extended_A>);
static_assert(rfl::is_flattenable_aggregate_v<my_pair_extended_B>);
static_assert(rfl::is_flattenable_aggregate_v<my_pair_extended_C>);

struct my_pair_extended_D : my_pair {
  double d[12];
  virtual void dump() {
    std::println("{} {}\n", this->first, this->second);
  }
};

struct my_pair_extended_E : my_pair_extended_D, std::to_chars_result {
  double e[12];
  void dump() override {
    std::println("{} {} {}\n",
      this->first, this->second, static_cast<const void*>(this->ptr));
  }
};

// Class types: With virtual members
static_assert(rfl::is_partially_flattenable_v<my_pair_extended_D>);
static_assert(rfl::is_partially_flattenable_v<my_pair_extended_E>);

static_assert(rfl::is_flattenable_v<my_pair_extended_D>);
static_assert(rfl::is_flattenable_v<my_pair_extended_E>);

static_assert(NOT rfl::is_flattenable_aggregate_v<my_pair_extended_D>);
static_assert(NOT rfl::is_flattenable_aggregate_v<my_pair_extended_E>);

struct foo_A_t : virtual my_pair {
  size_t a;
};

struct foo_B_t : virtual my_pair {
  size_t b;
};

struct foo_C_t : foo_A_t, foo_B_t {
  size_t c;
};

// Class types: Virtual inheritance
static_assert(NOT rfl::is_partially_flattenable_v<foo_A_t>);
static_assert(NOT rfl::is_partially_flattenable_v<foo_B_t>);
static_assert(NOT rfl::is_partially_flattenable_v<foo_C_t>);

static_assert(NOT rfl::is_flattenable_v<foo_A_t>);
static_assert(NOT rfl::is_flattenable_v<foo_B_t>);
static_assert(NOT rfl::is_flattenable_v<foo_C_t>);

static_assert(NOT rfl::is_flattenable_aggregate_v<foo_A_t>);
static_assert(NOT rfl::is_flattenable_aggregate_v<foo_B_t>);
static_assert(NOT rfl::is_flattenable_aggregate_v<foo_C_t>);

struct bar_A_t : my_pair {
  size_t a;
};

struct bar_B_t : my_pair {
  size_t b;
};

struct bar_C_t : bar_A_t, bar_B_t {
  size_t c;
};

// Class types: Inheritance
static_assert(rfl::is_partially_flattenable_v<bar_A_t>);
static_assert(rfl::is_partially_flattenable_v<bar_B_t>);
static_assert(NOT rfl::is_partially_flattenable_v<bar_C_t>);

static_assert(rfl::is_flattenable_v<bar_A_t>);
static_assert(rfl::is_flattenable_v<bar_B_t>);
static_assert(NOT rfl::is_flattenable_v<bar_C_t>);

static_assert(rfl::is_flattenable_aggregate_v<bar_A_t>);
static_assert(rfl::is_flattenable_aggregate_v<bar_B_t>);
static_assert(NOT rfl::is_flattenable_aggregate_v<bar_C_t>);

struct baz_A_t : protected my_pair {
  size_t a;
};

struct baz_B_t : private my_pair {
  size_t b;
};

struct baz_C_t : baz_A_t, baz_B_t {
  size_t c;
};

// Class types: With non-public inheritance
static_assert(rfl::is_partially_flattenable_v<baz_A_t>);
static_assert(rfl::is_partially_flattenable_v<baz_B_t>);
// Not partially flattenable due to ambiguous base class my_pair
static_assert(NOT rfl::is_partially_flattenable_v<baz_C_t>);

static_assert(NOT rfl::is_flattenable_v<baz_A_t>);
static_assert(NOT rfl::is_flattenable_v<baz_B_t>);
static_assert(NOT rfl::is_flattenable_v<baz_C_t>);

static_assert(NOT rfl::is_flattenable_aggregate_v<baz_A_t>);
static_assert(NOT rfl::is_flattenable_aggregate_v<baz_B_t>);
static_assert(NOT rfl::is_flattenable_aggregate_v<baz_C_t>);

TEST(TypeTraitsClassTypes, FlattenableTypes) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
