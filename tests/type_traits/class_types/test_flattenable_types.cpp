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

#include <print>
#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#include <system_error>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

// Examples in docs/type_traits.md
namespace examples {
template <class T, class U>
struct my_pair_t {
  T first;
  U second;
};

struct std_pairs_t {
  std::pair<int, long> pi;
  std::pair<float, double> pf;
};

struct my_pairs_t {
  my_pair_t<int, long> pi;
  my_pair_t<float, double> pf;
};

// Memberwise flattenable aggregate
static_assert(rfl::flattenable_aggregate_class<my_pairs_t>);
// Memberwise flattenable aggregate, which can be flattened as: {
//   pi.first: int
//   pi.second: int
//   pf.first: int
//   pf.second: int
// }
static_assert(rfl::memberwise_flattenable_aggregate_class<my_pairs_t>);

static_assert(rfl::flattenable_aggregate_class<std_pairs_t>);
// Memberwise flattenable. Similar with above.
static_assert(rfl::memberwise_flattenable_class<std_pairs_t>);
// NOT memberwise flattenable aggregate (since std::pair is not aggregate).
static_assert(NOT rfl::memberwise_flattenable_aggregate_class<std_pairs_t>);

struct foo_t {
  std::string first_name;
  std::string last_name;
};
struct bar_t : foo_t {
  int age;
};

// Flattenable aggregate, which can be flattened as: {
//   first_name: std::string
//   last_name: std::string
//   age: int
// }
static_assert(rfl::flattenable_aggregate_class<bar_t>);
// Memberwise flattening blocked by std::string.
static_assert(NOT rfl::memberwise_flattenable_aggregate_class<bar_t>);

struct baz_t : foo_t {
  virtual void introduce_myself() {
    std::println("Hello. I am {} {}.", this->first_name, this->last_name);
  }
};

// Flattenable, which can be flattened as: {
//   first_name: std::string
//   last_name: std::string
// }
static_assert(rfl::flattenable_class<baz_t>);
// Memberwise flattening blocked by std::string.
static_assert(NOT rfl::memberwise_flattenable_class<baz_t>);
// Not aggregate (due to existence of implicit v-table pointer)
static_assert(NOT rfl::flattenable_aggregate_class<baz_t>);

class qux_t : foo_t {
private:
  uint64_t last_visit_timestamp_;
};

// Partially flattenable (we can still try to flatten its public members): {
//   first_name: std::string
//   last_name: std::string
//   /* non-public data members are filtered out */
// }
static_assert(rfl::partially_flattenable_class<qux_t>);
static_assert(rfl::memberwise_partially_flattenable_class<qux_t>);
// Not flattenable (due to existence of private non-static data members)
static_assert(NOT rfl::flattenable_class<qux_t>);
// Not flattenable aggregate (stronger constraints than flattenable concept)
static_assert(NOT rfl::flattenable_aggregate_class<qux_t>);

// Hint: partially_flattenable has loose constraints that most class types in practice can satisfy.
// A class type can be partially flattenable even if there's nothing public to be flattened.
static_assert(rfl::partially_flattenable_class<std::string>);
static_assert(rfl::partially_flattenable_class<std::vector<std::string>>);

static_assert(rfl::memberwise_partially_flattenable_class<std::string>);
static_assert(rfl::memberwise_partially_flattenable_class<std::vector<std::string>>);
}  // namespace examples

// Arithmetic
static_assert(rfl::partially_flattenable<int>);
static_assert(rfl::partially_flattenable<const long>);
static_assert(rfl::partially_flattenable<volatile float>);

static_assert(rfl::flattenable<int>);
static_assert(rfl::flattenable<const long>);
static_assert(rfl::flattenable<volatile float>);

static_assert(NOT rfl::flattenable_aggregate<int>);
static_assert(NOT rfl::flattenable_aggregate<const long>);
static_assert(NOT rfl::flattenable_aggregate<volatile float>);

static_assert(rfl::memberwise_partially_flattenable<int>);
static_assert(rfl::memberwise_partially_flattenable<const long>);
static_assert(rfl::memberwise_partially_flattenable<volatile float>);

static_assert(rfl::memberwise_flattenable<int>);
static_assert(rfl::memberwise_flattenable<const long>);
static_assert(rfl::memberwise_flattenable<volatile float>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<int>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<const long>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<volatile float>);

// Enum
static_assert(rfl::partially_flattenable<std::byte>);
static_assert(rfl::partially_flattenable<std::errc>);

static_assert(rfl::flattenable<std::byte>);
static_assert(rfl::flattenable<std::errc>);

static_assert(NOT rfl::flattenable_aggregate<std::byte>);
static_assert(NOT rfl::flattenable_aggregate<std::errc>);

static_assert(rfl::memberwise_partially_flattenable<std::byte>);
static_assert(rfl::memberwise_partially_flattenable<std::errc>);

static_assert(rfl::memberwise_flattenable<std::byte>);
static_assert(rfl::memberwise_flattenable<std::errc>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<std::byte>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<std::errc>);

// Pointers
using std_pair = std::pair<double, double>;
using std_vector = std::vector<double>;
static_assert(rfl::partially_flattenable<char*>);
static_assert(rfl::partially_flattenable<const long* volatile>);
static_assert(rfl::partially_flattenable<std::nullptr_t>);
static_assert(rfl::partially_flattenable<double std_pair::*>);
static_assert(rfl::partially_flattenable<void (std_vector::*)(size_t)>);

static_assert(rfl::flattenable<char*>);
static_assert(rfl::flattenable<const long* volatile>);
static_assert(rfl::flattenable<std::nullptr_t>);
static_assert(rfl::flattenable<double std_pair::*>);
static_assert(rfl::flattenable<void (std_vector::*)(size_t)>);

static_assert(NOT rfl::flattenable_aggregate<char*>);
static_assert(NOT rfl::flattenable_aggregate<const long* volatile>);
static_assert(NOT rfl::flattenable_aggregate<std::nullptr_t>);
static_assert(NOT rfl::flattenable_aggregate<double std_pair::*>);
static_assert(NOT rfl::flattenable_aggregate<void (std_vector::*)(int)>);

static_assert(rfl::memberwise_partially_flattenable<char*>);
static_assert(rfl::memberwise_partially_flattenable<const long* volatile>);
static_assert(rfl::memberwise_partially_flattenable<std::nullptr_t>);
static_assert(rfl::memberwise_partially_flattenable<double std_pair::*>);
static_assert(rfl::memberwise_partially_flattenable<void (std_vector::*)(size_t)>);

static_assert(rfl::memberwise_flattenable<char*>);
static_assert(rfl::memberwise_flattenable<const long* volatile>);
static_assert(rfl::memberwise_flattenable<std::nullptr_t>);
static_assert(rfl::memberwise_flattenable<double std_pair::*>);
static_assert(rfl::memberwise_flattenable<void (std_vector::*)(size_t)>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<char*>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<const long* volatile>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<std::nullptr_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<double std_pair::*>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<void (std_vector::*)(int)>);

// References
static_assert(NOT rfl::partially_flattenable<int&>);
static_assert(NOT rfl::partially_flattenable<const std_pair&>);
static_assert(NOT rfl::partially_flattenable<float&&>);
static_assert(NOT rfl::partially_flattenable<const std_pair&&>);

static_assert(NOT rfl::flattenable<int&>);
static_assert(NOT rfl::flattenable<const std_pair&>);
static_assert(NOT rfl::flattenable<float&&>);
static_assert(NOT rfl::flattenable<const std_pair&&>);

static_assert(NOT rfl::flattenable_aggregate<int&>);
static_assert(NOT rfl::flattenable_aggregate<const std_pair&>);
static_assert(NOT rfl::flattenable_aggregate<float&&>);
static_assert(NOT rfl::flattenable_aggregate<const std_pair&&>);

static_assert(NOT rfl::memberwise_partially_flattenable<int&>);
static_assert(NOT rfl::memberwise_partially_flattenable<const std_pair&>);
static_assert(NOT rfl::memberwise_partially_flattenable<float&&>);
static_assert(NOT rfl::memberwise_partially_flattenable<const std_pair&&>);

static_assert(NOT rfl::memberwise_flattenable<int&>);
static_assert(NOT rfl::memberwise_flattenable<const std_pair&>);
static_assert(NOT rfl::memberwise_flattenable<float&&>);
static_assert(NOT rfl::memberwise_flattenable<const std_pair&&>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<int&>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<const std_pair&>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<float&&>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<const std_pair&&>);

// C-style Arrays
using std_vector = std::vector<double>;
static_assert(rfl::partially_flattenable<long[]>);
static_assert(rfl::partially_flattenable<double[16]>);
static_assert(rfl::partially_flattenable<volatile std_vector[]>);
static_assert(rfl::partially_flattenable<std_vector[32]>);

static_assert(rfl::flattenable<long[]>);
static_assert(rfl::flattenable<double[16]>);
static_assert(rfl::flattenable<volatile std_vector[]>);
static_assert(rfl::flattenable<std_vector[32]>);

static_assert(rfl::flattenable_aggregate<long[]>);
static_assert(rfl::flattenable_aggregate<double[16]>);
static_assert(rfl::flattenable_aggregate<volatile std_vector[]>);
static_assert(rfl::flattenable_aggregate<std_vector[32]>);

static_assert(rfl::memberwise_partially_flattenable<long[]>);
static_assert(rfl::memberwise_partially_flattenable<double[16]>);
static_assert(rfl::memberwise_partially_flattenable<volatile std_vector[]>);
static_assert(rfl::memberwise_partially_flattenable<std_vector[32]>);

static_assert(rfl::memberwise_flattenable<long[]>);
static_assert(rfl::memberwise_flattenable<double[16]>);
// std::vector blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable<volatile std_vector[]>);
static_assert(NOT rfl::memberwise_flattenable<std_vector[32]>);

static_assert(rfl::memberwise_flattenable_aggregate<long[]>);
static_assert(rfl::memberwise_flattenable_aggregate<double[16]>);
// std::vector blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable_aggregate<volatile std_vector[]>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<std_vector[32]>);

union some_union_t {
  int as_int;
  float as_float;
  const char* as_char_literal;
};

// Unions
static_assert(NOT rfl::partially_flattenable<some_union_t>);
static_assert(NOT rfl::flattenable<some_union_t>);
static_assert(NOT rfl::flattenable_aggregate<some_union_t>);

// Memberwise flattenable union types (should all be false)
static_assert(NOT rfl::memberwise_partially_flattenable<some_union_t>);
static_assert(NOT rfl::memberwise_flattenable<some_union_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<some_union_t>);

struct my_pair {
  std::string first;
  std::string second;
};

struct my_pair_with_ctor {
  std::string first;
  std::string second;

  // Not aggregate anymore
  my_pair_with_ctor() = default;
};

// Class types: Basic
static_assert(rfl::partially_flattenable<std::monostate>);
static_assert(rfl::partially_flattenable<const std::to_chars_result>);
static_assert(rfl::partially_flattenable<volatile std_pair>);
static_assert(rfl::partially_flattenable<volatile my_pair>);
static_assert(rfl::partially_flattenable<my_pair_with_ctor>);
static_assert(rfl::partially_flattenable<const volatile std_vector>);

static_assert(rfl::flattenable<std::monostate>);
static_assert(rfl::flattenable<const std::to_chars_result>);
static_assert(rfl::flattenable<volatile std_pair>);
static_assert(rfl::flattenable<volatile my_pair>);
static_assert(rfl::flattenable<my_pair_with_ctor>);
static_assert(NOT rfl::flattenable<const volatile std_vector>);

static_assert(rfl::flattenable_aggregate<std::monostate>);
static_assert(rfl::flattenable_aggregate<const std::to_chars_result>);
static_assert(rfl::flattenable_aggregate<volatile my_pair>);
static_assert(NOT rfl::flattenable_aggregate<my_pair_with_ctor>);
static_assert(NOT rfl::flattenable_aggregate<const volatile std_vector>);

static_assert(rfl::memberwise_partially_flattenable<std::monostate>);
static_assert(rfl::memberwise_partially_flattenable<const std::to_chars_result>);
static_assert(rfl::memberwise_partially_flattenable<volatile my_pair>);
static_assert(rfl::memberwise_partially_flattenable<my_pair_with_ctor>);
static_assert(rfl::memberwise_partially_flattenable<const volatile std_vector>);

static_assert(rfl::memberwise_flattenable<std::monostate>);
static_assert(rfl::memberwise_flattenable<const std::to_chars_result>);
// std::string blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable<volatile my_pair>);
static_assert(NOT rfl::memberwise_flattenable<my_pair_with_ctor>);
static_assert(NOT rfl::memberwise_flattenable<const volatile std_vector>);

static_assert(rfl::memberwise_flattenable_aggregate<std::monostate>);
static_assert(rfl::memberwise_flattenable_aggregate<const std::to_chars_result>);
// std::string blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable_aggregate<volatile my_pair>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<my_pair_with_ctor>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<const volatile std_vector>);

// Class types: Non-aggregate base class
struct my_pair_with_ctor_extended : my_pair_with_ctor {
  std::string third;
  std::string fourth;
};
static_assert(std::is_aggregate_v<my_pair_with_ctor_extended>,
              "This derived class itself is aggregate. Its base class is not.");

static_assert(rfl::partially_flattenable<my_pair_with_ctor_extended>);
static_assert(rfl::flattenable<my_pair_with_ctor_extended>);
// flattenable_aggregate constraints impose to base classes **memberwise**.
static_assert(NOT rfl::flattenable_aggregate<my_pair_with_ctor_extended>);

static_assert(rfl::memberwise_partially_flattenable<my_pair_with_ctor_extended>);
// std::string blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable<my_pair_with_ctor_extended>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<my_pair_with_ctor_extended>);

// Class types: With reference members
struct references_t {
  int& i;
  const long& cl;
  volatile float& vf;
  const volatile double& cvd;
};
static_assert(std::is_aggregate_v<references_t>, "This class is aggregate.");

// Reference members are OK in flattenable classes.
static_assert(rfl::partially_flattenable<references_t>);
static_assert(rfl::flattenable<references_t>);
static_assert(rfl::flattenable_aggregate<references_t>);

// References are not memberwise flattenable
static_assert(NOT rfl::memberwise_partially_flattenable<references_t>);
static_assert(NOT rfl::memberwise_flattenable<references_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<references_t>);

// Class types: With bit-field members
struct bit_fields_t {
  int16_t flag : 1;
  int16_t x : 4;
  int16_t y : 8;
  int16_t z : 12;
};
static_assert(std::is_aggregate_v<bit_fields_t>, "This class is aggregate.");

// Bit-field members are OK in flattenable classes as well.
static_assert(rfl::partially_flattenable<bit_fields_t>);
static_assert(rfl::flattenable<bit_fields_t>);
static_assert(rfl::flattenable_aggregate<bit_fields_t>);

static_assert(rfl::memberwise_partially_flattenable<bit_fields_t>);
static_assert(rfl::memberwise_flattenable<bit_fields_t>);
static_assert(rfl::memberwise_flattenable_aggregate<bit_fields_t>);

// Class types: Not (trivially) destructible
struct struct_not_destructible_t {
  int x;
  int y;
  ~struct_not_destructible_t() = delete;
};
static_assert(std::is_aggregate_v<struct_not_destructible_t>, "This type is aggregate.");

struct struct_not_trivially_destructible_t {
  int x;
  int y;
  const int z;
  ~struct_not_trivially_destructible_t() {
    std::println("z = {}", z);
  }
};
static_assert(std::is_aggregate_v<struct_not_trivially_destructible_t>, "This type is aggregate.");

static_assert(rfl::partially_flattenable<struct_not_destructible_t>);
static_assert(rfl::partially_flattenable<struct_not_trivially_destructible_t>);

static_assert(rfl::flattenable<struct_not_destructible_t>);
static_assert(rfl::flattenable<struct_not_trivially_destructible_t>);

static_assert(rfl::flattenable_aggregate<struct_not_destructible_t>);
static_assert(rfl::flattenable_aggregate<struct_not_trivially_destructible_t>);

static_assert(rfl::memberwise_partially_flattenable<struct_not_destructible_t>);
static_assert(rfl::memberwise_partially_flattenable<struct_not_trivially_destructible_t>);

static_assert(rfl::memberwise_flattenable<struct_not_destructible_t>);
static_assert(rfl::memberwise_flattenable<struct_not_trivially_destructible_t>);

static_assert(rfl::memberwise_flattenable_aggregate<struct_not_destructible_t>);
static_assert(rfl::memberwise_flattenable_aggregate<struct_not_trivially_destructible_t>);

// Class types: Inheritance
struct my_pair_extended_A : my_pair {
  size_t index;
};

struct my_pair_extended_B : my_pair_extended_A {
  double values[16];
  size_t index;  // Shadows my_pair_extended_A::index
  size_t size;
};

struct my_pair_extended_C : my_pair_extended_B, std::to_chars_result {
  const double* ptr;  // Shadows std::to_chars_result::ptr
};

// Member name collision is OK.
static_assert(rfl::partially_flattenable<my_pair_extended_A>);
static_assert(rfl::partially_flattenable<my_pair_extended_B>);
static_assert(rfl::partially_flattenable<my_pair_extended_C>);

static_assert(rfl::flattenable<my_pair_extended_A>);
static_assert(rfl::flattenable<my_pair_extended_B>);
static_assert(rfl::flattenable<my_pair_extended_C>);

static_assert(rfl::flattenable_aggregate<my_pair_extended_A>);
static_assert(rfl::flattenable_aggregate<my_pair_extended_B>);
static_assert(rfl::flattenable_aggregate<my_pair_extended_C>);

static_assert(rfl::memberwise_partially_flattenable<my_pair_extended_A>);
static_assert(rfl::memberwise_partially_flattenable<my_pair_extended_B>);
static_assert(rfl::memberwise_partially_flattenable<my_pair_extended_C>);

// std::string members in the base class my_pair blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable<my_pair_extended_A>);
static_assert(NOT rfl::memberwise_flattenable<my_pair_extended_B>);
static_assert(NOT rfl::memberwise_flattenable<my_pair_extended_C>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<my_pair_extended_A>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<my_pair_extended_B>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<my_pair_extended_C>);

struct my_c_str_pair {
  const char* first;
  const char* first_end;
  const wchar_t* second;
  const wchar_t* second_end;
};

struct my_c_str_pair_extended_A : my_c_str_pair {
  size_t index;
};

struct my_c_str_pair_extended_B : my_c_str_pair_extended_A {
  double values[16];
  size_t index;  // Shadows my_c_str_pair_extended_A::index
  size_t size;
};

struct my_c_str_pair_extended_C : my_c_str_pair_extended_B, std::to_chars_result {
  const double* ptr;  // Shadows std::to_chars_result::ptr
};

static_assert(rfl::memberwise_partially_flattenable<my_c_str_pair_extended_A>);
static_assert(rfl::memberwise_partially_flattenable<my_c_str_pair_extended_B>);
static_assert(rfl::memberwise_partially_flattenable<my_c_str_pair_extended_C>);

static_assert(rfl::memberwise_flattenable<my_c_str_pair_extended_A>);
static_assert(rfl::memberwise_flattenable<my_c_str_pair_extended_B>);
static_assert(rfl::memberwise_flattenable<my_c_str_pair_extended_C>);

static_assert(rfl::memberwise_flattenable_aggregate<my_c_str_pair_extended_A>);
static_assert(rfl::memberwise_flattenable_aggregate<my_c_str_pair_extended_B>);
static_assert(rfl::memberwise_flattenable_aggregate<my_c_str_pair_extended_C>);

// Class types: With virtual members
struct my_pair_extended_D : my_pair {
  double d[12];
  virtual void dump() {
    std::println("{} {}\n", this->first, this->second);
  }
};

struct my_pair_extended_E : my_pair_extended_D, std::to_chars_result {
  double e[12];
  void dump() override {
    std::println("{} {} {}\n", this->first, this->second, static_cast<const void*>(this->ptr));
  }
};

static_assert(rfl::partially_flattenable<my_pair_extended_D>);
static_assert(rfl::partially_flattenable<my_pair_extended_E>);

// Still flattenable. We don't care virtual member functions and the implicit v-table pointer.
static_assert(rfl::flattenable<my_pair_extended_D>);
static_assert(rfl::flattenable<my_pair_extended_E>);

// Not aggregate
static_assert(NOT rfl::flattenable_aggregate<my_pair_extended_D>);
static_assert(NOT rfl::flattenable_aggregate<my_pair_extended_E>);

static_assert(rfl::memberwise_partially_flattenable<my_pair_extended_D>);
static_assert(rfl::memberwise_partially_flattenable<my_pair_extended_E>);

// std::string members in the base class my_pair blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable<my_pair_extended_D>);
static_assert(NOT rfl::memberwise_flattenable<my_pair_extended_E>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<my_pair_extended_D>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<my_pair_extended_E>);

// Class types: Virtual inheritance
struct foo_A_t : virtual my_pair {
  size_t a;
};

struct foo_B_t : virtual my_pair {
  size_t b;
};

struct foo_C_t : foo_A_t, foo_B_t {
  size_t c;
};

// Virtual inheritance is never accepted.
static_assert(NOT rfl::partially_flattenable<foo_A_t>);
static_assert(NOT rfl::partially_flattenable<foo_B_t>);
static_assert(NOT rfl::partially_flattenable<foo_C_t>);

static_assert(NOT rfl::flattenable<foo_A_t>);
static_assert(NOT rfl::flattenable<foo_B_t>);
static_assert(NOT rfl::flattenable<foo_C_t>);

static_assert(NOT rfl::flattenable_aggregate<foo_A_t>);
static_assert(NOT rfl::flattenable_aggregate<foo_B_t>);
static_assert(NOT rfl::flattenable_aggregate<foo_C_t>);

static_assert(NOT rfl::memberwise_partially_flattenable<foo_A_t>);
static_assert(NOT rfl::memberwise_partially_flattenable<foo_B_t>);
static_assert(NOT rfl::memberwise_partially_flattenable<foo_C_t>);

static_assert(NOT rfl::memberwise_flattenable<foo_A_t>);
static_assert(NOT rfl::memberwise_flattenable<foo_B_t>);
static_assert(NOT rfl::memberwise_flattenable<foo_C_t>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<foo_A_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<foo_B_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<foo_C_t>);

// Class types: Diamond inheritance
struct bar_A_t : my_pair {
  size_t a;
};

struct bar_B_t : my_pair {
  size_t b;
};

struct bar_C_t : bar_A_t, bar_B_t {
  size_t c;
};

static_assert(rfl::partially_flattenable<bar_A_t>);
static_assert(rfl::partially_flattenable<bar_B_t>);
// Ambiguous base class
static_assert(NOT rfl::partially_flattenable<bar_C_t>);

static_assert(rfl::flattenable<bar_A_t>);
static_assert(rfl::flattenable<bar_B_t>);
static_assert(NOT rfl::flattenable<bar_C_t>);

static_assert(rfl::flattenable_aggregate<bar_A_t>);
static_assert(rfl::flattenable_aggregate<bar_B_t>);
static_assert(NOT rfl::flattenable_aggregate<bar_C_t>);

static_assert(rfl::memberwise_partially_flattenable<bar_A_t>);
static_assert(rfl::memberwise_partially_flattenable<bar_B_t>);
static_assert(NOT rfl::memberwise_partially_flattenable<bar_C_t>);

// std::string in base class my_pair blocks memberwise flattening
static_assert(NOT rfl::memberwise_flattenable<bar_A_t>);
static_assert(NOT rfl::memberwise_flattenable<bar_B_t>);
static_assert(NOT rfl::memberwise_flattenable<bar_C_t>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<bar_A_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<bar_B_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<bar_C_t>);

struct bar_D_t : my_c_str_pair {
  size_t a;
};

struct bar_E_t : my_c_str_pair {
  size_t b;
};

struct bar_F_t : bar_D_t, bar_E_t {
  size_t c;
};

static_assert(rfl::memberwise_partially_flattenable<bar_D_t>);
static_assert(rfl::memberwise_partially_flattenable<bar_E_t>);
// Ambiguous base class my_c_str_pair
static_assert(NOT rfl::memberwise_partially_flattenable<bar_F_t>);

static_assert(rfl::memberwise_flattenable<bar_D_t>);
static_assert(rfl::memberwise_flattenable<bar_E_t>);
static_assert(NOT rfl::memberwise_flattenable<bar_F_t>);

static_assert(rfl::memberwise_flattenable_aggregate<bar_D_t>);
static_assert(rfl::memberwise_flattenable_aggregate<bar_E_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<bar_F_t>);

// Class types: With non-public inheritance
struct baz_A_t : protected my_pair {
  size_t a;
};

struct baz_B_t : private my_pair {
  size_t b;
};

struct baz_C_t : baz_A_t, baz_B_t {
  size_t c;
};

static_assert(rfl::partially_flattenable<baz_A_t>);
static_assert(rfl::partially_flattenable<baz_B_t>);
// Still, ambiguous base class
static_assert(NOT rfl::partially_flattenable<baz_C_t>);

static_assert(NOT rfl::flattenable<baz_A_t>);
static_assert(NOT rfl::flattenable<baz_B_t>);
static_assert(NOT rfl::flattenable<baz_C_t>);

static_assert(NOT rfl::flattenable_aggregate<baz_A_t>);
static_assert(NOT rfl::flattenable_aggregate<baz_B_t>);
static_assert(NOT rfl::flattenable_aggregate<baz_C_t>);

static_assert(rfl::memberwise_partially_flattenable<baz_A_t>);
static_assert(rfl::memberwise_partially_flattenable<baz_B_t>);
static_assert(NOT rfl::memberwise_partially_flattenable<baz_C_t>);

static_assert(NOT rfl::memberwise_flattenable<baz_A_t>);
static_assert(NOT rfl::memberwise_flattenable<baz_B_t>);
static_assert(NOT rfl::memberwise_flattenable<baz_C_t>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<baz_A_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<baz_B_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<baz_C_t>);

// Class types with non-public members
struct with_non_public_members_1_t {
  int a;

private:
  int b;
};

struct with_non_public_members_2_t : with_non_public_members_1_t {
  int c;
};

static_assert(rfl::partially_flattenable<with_non_public_members_1_t>);
static_assert(NOT rfl::flattenable<with_non_public_members_1_t>);
static_assert(NOT rfl::flattenable_aggregate<with_non_public_members_1_t>);

static_assert(rfl::partially_flattenable<with_non_public_members_2_t>);
static_assert(NOT rfl::flattenable<with_non_public_members_2_t>);
static_assert(NOT rfl::flattenable_aggregate<with_non_public_members_2_t>);

static_assert(rfl::memberwise_partially_flattenable<with_non_public_members_1_t>);
static_assert(rfl::memberwise_partially_flattenable<with_non_public_members_2_t>);

static_assert(NOT rfl::memberwise_flattenable<with_non_public_members_1_t>);
static_assert(NOT rfl::memberwise_flattenable<with_non_public_members_2_t>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<with_non_public_members_1_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<with_non_public_members_2_t>);

// Class types with union members
struct with_union_member_1_t {
  int a;
  union {
    int as_int;
    unsigned as_unsigned;
    float as_float;
  } u;
};

struct with_union_member_2_t {
  int a;
  union {
    int as_int;
    unsigned as_unsigned;
    float as_float;
  };  // Anonymous union member
};

static_assert(std::is_aggregate_v<with_union_member_1_t>, "This class is aggregate.");
static_assert(std::is_aggregate_v<with_union_member_2_t>, "This class is aggregate.");

// We don't care the type of members in these concepts.
static_assert(rfl::partially_flattenable<with_union_member_1_t>);
static_assert(rfl::flattenable<const with_union_member_1_t>);
static_assert(rfl::flattenable_aggregate<volatile with_union_member_1_t>);

static_assert(rfl::partially_flattenable<with_union_member_2_t>);
static_assert(rfl::flattenable<const with_union_member_2_t>);
static_assert(rfl::flattenable_aggregate<volatile with_union_member_2_t>);

// Union members block memberwise flattening
static_assert(NOT rfl::memberwise_partially_flattenable<with_union_member_1_t>);
static_assert(NOT rfl::memberwise_partially_flattenable<with_union_member_2_t>);

static_assert(NOT rfl::memberwise_flattenable<with_union_member_1_t>);
static_assert(NOT rfl::memberwise_flattenable<with_union_member_2_t>);

static_assert(NOT rfl::memberwise_flattenable_aggregate<with_union_member_1_t>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<with_union_member_2_t>);

// Memberwise flattenable types tests
namespace memberwise_tests {
// Simple memberwise flattenable types
struct simple_pair {
  int a;
  long b;
};

struct nested_simple {
  simple_pair sp;
  float f;
};

static_assert(rfl::memberwise_partially_flattenable<simple_pair>);
static_assert(rfl::memberwise_flattenable<simple_pair>);
static_assert(rfl::memberwise_flattenable_aggregate<simple_pair>);

static_assert(rfl::memberwise_partially_flattenable<nested_simple>);
static_assert(rfl::memberwise_flattenable<nested_simple>);
static_assert(rfl::memberwise_flattenable_aggregate<nested_simple>);

// Arrays of memberwise flattenable types
static_assert(rfl::memberwise_partially_flattenable<simple_pair[]>);
static_assert(rfl::memberwise_flattenable<simple_pair[5]>);
static_assert(rfl::memberwise_flattenable_aggregate<simple_pair[5]>);

// Non-memberwise flattenable due to std::string member
struct with_string {
  std::string s;
  int i;
};

struct nested_with_string {
  with_string ws;
  float f;
};

static_assert(rfl::memberwise_partially_flattenable<with_string>);
static_assert(NOT rfl::memberwise_flattenable<with_string>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<with_string>);

// Partially flattened memberwise as: {
//   ws: {
//     s: {}    (no public members can be flattened)
//     i: int
//   }
//   f: float
// }
static_assert(rfl::memberwise_partially_flattenable<nested_with_string>);
static_assert(NOT rfl::memberwise_flattenable<nested_with_string>);
static_assert(NOT rfl::memberwise_flattenable_aggregate<nested_with_string>);

// Partially flattenable but not memberwise due to private members
class with_private : public simple_pair {
private:
  int hidden;

public:
  double d;
};

struct nested_with_private {
  with_private wp;
  char c;
};

static_assert(rfl::partially_flattenable<with_private>);
static_assert(rfl::memberwise_partially_flattenable<with_private>);
// Partially flattened memberwise as: {
//   wp: {
//     d: double
//   }
//   c: char
// }
static_assert(rfl::memberwise_partially_flattenable<nested_with_private>);

// Memberwise partially flattenable with public members only
class with_private_public {
private:
  int hidden;

public:
  simple_pair sp;
  double d;
};

struct nested_with_private_public {
  with_private_public wpp;
  char c;
};

static_assert(rfl::memberwise_partially_flattenable<with_private_public>);
static_assert(rfl::memberwise_partially_flattenable<nested_with_private_public>);
static_assert(NOT rfl::memberwise_flattenable<with_private_public>);  // Due to private member
static_assert(NOT rfl::memberwise_flattenable<nested_with_private_public>);
}  // namespace memberwise_tests

TEST(TypeTraitsClassTypes, FlattenableTypes) {
  EXPECT_TRUE(true);  // All test cases done with static-asserts above
}
