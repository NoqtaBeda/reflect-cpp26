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

#include <deque>
#include <list>
#include <map>
#include <reflect_cpp26/type_traits/serializable_types.hpp>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

enum class color_t : uint8_t {
  red,
  green,
  blue
};

struct foo_t {};

// ---- Test structs for serializable ----

// Struct with all serializable members (flattenable)
struct serializable_struct_t {
  int x;
  double y;
  std::string name;
};

// Struct with nested serializable struct
struct nested_serializable_struct_t {
  int id;
  serializable_struct_t inner;
};

// Struct with a pointer member (not serializable)
struct struct_with_pointer_t {
  int x;
  int* ptr;
};

// Struct with a reference member (not serializable)
struct struct_with_reference_t {
  int x;
  int& ref;
};

// Struct with private member (not flattenable_class, but partially_flattenable_class)
struct struct_with_private_member_t {
  int public_x;

private:
  int private_y;
};

// Struct with inheritance (all members serializable)
struct base_serializable_t {
  int base_x;
};

struct derived_serializable_t : base_serializable_t {
  double derived_y;
};

// Struct with inheritance (pointer in base)
struct base_with_pointer_t {
  int* ptr;
};

struct derived_from_pointer_base_t : base_with_pointer_t {
  int x;
};

// Struct with multiple levels of inheritance
struct grandparent_t {
  int gp_val;
};

struct parent_t : grandparent_t {
  double p_val;
};

struct child_t : parent_t {
  std::string c_val;
};

TEST(TypeTraits, Serializable) {
  // ---- All serializable types are memberwise serializable: ✔️ ----
  // Arithmetic types
  static_assert(rfl::serializable<int>);
  static_assert(rfl::serializable<double>);
  static_assert(rfl::serializable<bool>);
  static_assert(rfl::serializable<char>);
  static_assert(rfl::serializable<wchar_t>);
  // Enum types
  static_assert(rfl::serializable<color_t>);
  // String-like types
  static_assert(rfl::serializable<std::string>);
  static_assert(rfl::serializable<std::wstring>);
  // Range types
  static_assert(rfl::serializable<std::vector<int>>);
  static_assert(rfl::serializable<std::array<double, 4>>);
  // Tuple-like types
  static_assert(rfl::serializable<std::pair<int, double>>);
  static_assert(rfl::serializable<std::tuple<int, std::string, bool>>);
  // std::optional
  static_assert(rfl::serializable<std::optional<int>>);
  // std::variant
  static_assert(rfl::serializable<std::variant<int, double>>);
  // std::monostate
  static_assert(rfl::serializable<std::monostate>);

  // ---- Flattenable class types with all serializable members: ✔️ ----
  // Empty class is trivially flattenable
  static_assert(rfl::serializable<foo_t>);
  // Simple struct with serializable members
  static_assert(rfl::serializable<serializable_struct_t>);
  // Nested struct with serializable members
  static_assert(rfl::serializable<nested_serializable_struct_t>);
  // Struct with inheritance (all members serializable)
  static_assert(rfl::serializable<base_serializable_t>);
  static_assert(rfl::serializable<derived_serializable_t>);
  // Multi-level inheritance with serializable members
  static_assert(rfl::serializable<grandparent_t>);
  static_assert(rfl::serializable<parent_t>);
  static_assert(rfl::serializable<child_t>);

  // ---- Class types with non-serializable members: ❌ ----
  // Struct with pointer member (pointer is not serializable)
  static_assert(NOT rfl::serializable<struct_with_pointer_t>);
  // Struct with reference member (reference is not serializable)
  static_assert(NOT rfl::serializable<struct_with_reference_t>);
  // Struct with private member (not flattenable_class)
  static_assert(NOT rfl::serializable<struct_with_private_member_t>);
  // Struct with inheritance where base has pointer
  static_assert(NOT rfl::serializable<base_with_pointer_t>);
  static_assert(NOT rfl::serializable<derived_from_pointer_base_t>);

  // ---- Other non-memberwise-serializable types: ❌ ----
  static_assert(NOT rfl::serializable<void>);
  static_assert(NOT rfl::serializable<int*>);
  static_assert(NOT rfl::serializable<int&>);
  static_assert(NOT rfl::serializable<int()>);
  static_assert(NOT rfl::serializable<int (*)()>);
}

TEST(TypeTraits, SerializableAdvancedNesting) {
  // ---- Range with struct elements ----
  static_assert(rfl::serializable<std::vector<serializable_struct_t>>);
  static_assert(rfl::serializable<std::vector<nested_serializable_struct_t>>);
  static_assert(rfl::serializable<std::array<serializable_struct_t, 4>>);

  // ---- Range with optional elements ----
  static_assert(rfl::serializable<std::vector<std::optional<int>>>);
  static_assert(rfl::serializable<std::vector<std::optional<serializable_struct_t>>>);

  // ---- Range with variant elements ----
  static_assert(rfl::serializable<std::vector<std::variant<int, std::string>>>);
  static_assert(rfl::serializable<std::vector<std::variant<serializable_struct_t, int>>>);

  // ---- Range with tuple elements ----
  static_assert(rfl::serializable<std::vector<std::tuple<int, double>>>);
  static_assert(rfl::serializable<std::vector<std::pair<int, std::string>>>);

  // ---- Range with range elements (nested ranges) ----
  static_assert(rfl::serializable<std::vector<std::vector<int>>>);
  static_assert(rfl::serializable<std::vector<std::array<int, 4>>>);

  // ---- Tuple with struct elements ----
  static_assert(rfl::serializable<std::tuple<serializable_struct_t, int>>);
  static_assert(rfl::serializable<std::pair<serializable_struct_t, serializable_struct_t>>);

  // ---- Tuple with optional elements ----
  static_assert(rfl::serializable<std::tuple<std::optional<int>, std::optional<double>>>);
  static_assert(rfl::serializable<std::pair<std::optional<serializable_struct_t>, int>>);

  // ---- Tuple with variant elements ----
  static_assert(rfl::serializable<std::tuple<std::variant<int, double>, std::string>>);
  static_assert(rfl::serializable<std::pair<std::variant<color_t, bool>, serializable_struct_t>>);

  // ---- Optional with struct ----
  static_assert(rfl::serializable<std::optional<serializable_struct_t>>);
  static_assert(rfl::serializable<std::optional<nested_serializable_struct_t>>);

  // ---- Optional with range ----
  static_assert(rfl::serializable<std::optional<std::vector<int>>>);
  static_assert(rfl::serializable<std::optional<std::array<double, 4>>>);

  // ---- Optional with tuple ----
  static_assert(rfl::serializable<std::optional<std::pair<int, double>>>);
  static_assert(rfl::serializable<std::optional<std::tuple<int, std::string, bool>>>);

  // ---- Optional with optional (nested optional) ----
  static_assert(rfl::serializable<std::optional<std::optional<int>>>);
  static_assert(rfl::serializable<std::optional<std::optional<serializable_struct_t>>>);

  // ---- Optional with variant ----
  static_assert(rfl::serializable<std::optional<std::variant<int, double>>>);
  static_assert(rfl::serializable<std::optional<std::variant<serializable_struct_t, int>>>);

  // ---- Variant with struct ----
  static_assert(rfl::serializable<std::variant<serializable_struct_t, int>>);
  static_assert(rfl::serializable<std::variant<int, serializable_struct_t, double>>);

  // ---- Variant with range ----
  static_assert(rfl::serializable<std::variant<std::vector<int>, double>>);
  static_assert(rfl::serializable<std::variant<int, std::array<std::string, 4>>>);

  // ---- Variant with tuple ----
  static_assert(rfl::serializable<std::variant<std::pair<int, double>, std::string>>);
  static_assert(rfl::serializable<std::variant<int, std::tuple<double, bool, std::string>>>);

  // ---- Variant with optional ----
  static_assert(rfl::serializable<std::variant<std::optional<int>, double>>);
  static_assert(rfl::serializable<std::variant<int, std::optional<serializable_struct_t>>>);

  // ---- Variant with variant (nested variant) ----
  static_assert(rfl::serializable<std::variant<std::variant<int, double>, std::string>>);
  static_assert(rfl::serializable<std::variant<int, std::variant<double, std::string, color_t>>>);

  // ---- Deep nesting ----
  static_assert(rfl::serializable<std::vector<std::pair<int, std::vector<serializable_struct_t>>>>);
  static_assert(rfl::serializable<
                std::tuple<int, std::optional<std::vector<std::pair<std::string, color_t>>>>>);
  static_assert(
      rfl::serializable<std::tuple<serializable_struct_t,
                                   std::optional<std::variant<std::vector<int>, std::string>>>>);
  static_assert(
      rfl::serializable<std::optional<std::vector<
          std::tuple<std::optional<int>, std::variant<serializable_struct_t, std::string>>>>>);

  // ---- Associative containers with complex value_types ----
  static_assert(rfl::serializable<std::map<int, std::vector<serializable_struct_t>>>);
  static_assert(
      rfl::serializable<std::unordered_map<std::string, std::optional<serializable_struct_t>>>);
  static_assert(rfl::serializable<std::map<std::string, std::variant<int, double, std::string>>>);
}

TEST(TypeTraits, SerializableMultiLevelNesting) {
  // ---- Level 1: struct -> range -> struct ----
  static_assert(rfl::serializable<std::vector<serializable_struct_t>>);
  static_assert(rfl::serializable<std::list<std::vector<serializable_struct_t>>>);
  static_assert(rfl::serializable<std::array<std::list<serializable_struct_t>, 3>>);

  // ---- Level 2: struct -> tuple -> struct ----
  static_assert(rfl::serializable<std::pair<serializable_struct_t, serializable_struct_t>>);
  static_assert(rfl::serializable<std::tuple<serializable_struct_t, int, serializable_struct_t>>);
  static_assert(rfl::serializable<std::tuple<std::string, serializable_struct_t, double>>);

  // ---- Level 2: struct -> optional -> struct ----
  static_assert(rfl::serializable<std::optional<serializable_struct_t>>);
  static_assert(rfl::serializable<std::optional<std::optional<serializable_struct_t>>>);

  // ---- Level 2: struct -> variant -> struct ----
  static_assert(rfl::serializable<std::variant<serializable_struct_t, serializable_struct_t>>);
  static_assert(rfl::serializable<std::variant<int, serializable_struct_t, double>>);

  // ---- Level 3: range -> struct -> range ----
  static_assert(rfl::serializable<std::vector<std::vector<serializable_struct_t>>>);
  static_assert(rfl::serializable<std::vector<std::array<serializable_struct_t, 4>>>);
  static_assert(rfl::serializable<std::list<std::vector<serializable_struct_t>>>);
  static_assert(rfl::serializable<std::vector<std::list<serializable_struct_t>>>);
  static_assert(rfl::serializable<std::vector<std::set<serializable_struct_t>>>);

  // ---- Level 3: tuple -> struct -> tuple ----
  static_assert(rfl::serializable<std::pair<serializable_struct_t, serializable_struct_t>>);
  static_assert(rfl::serializable<
                std::tuple<serializable_struct_t, serializable_struct_t, serializable_struct_t>>);
  static_assert(rfl::serializable<std::tuple<std::pair<int, serializable_struct_t>, std::string>>);
  static_assert(
      rfl::serializable<std::pair<std::string, std::tuple<int, serializable_struct_t, double>>>);

  // ---- Level 3: optional -> struct -> optional ----
  static_assert(rfl::serializable<std::optional<std::optional<serializable_struct_t>>>);
  static_assert(
      rfl::serializable<std::optional<std::optional<std::optional<serializable_struct_t>>>>);

  // ---- Level 3: variant -> struct -> variant ----
  static_assert(rfl::serializable<std::variant<serializable_struct_t, serializable_struct_t>>);
  static_assert(rfl::serializable<std::variant<std::variant<int, serializable_struct_t>, double>>);
  static_assert(rfl::serializable<
                std::variant<int, std::variant<double, serializable_struct_t, std::string>>>);

  // ---- Level 4: range -> tuple -> struct -> range ----
  static_assert(rfl::serializable<std::vector<std::tuple<int, serializable_struct_t, double>>>);
  static_assert(rfl::serializable<std::list<std::pair<std::string, serializable_struct_t>>>);

  // ---- Level 4: tuple -> optional -> struct -> tuple ----
  static_assert(rfl::serializable<std::tuple<int, std::optional<serializable_struct_t>, double>>);
  static_assert(rfl::serializable<std::pair<std::string, std::optional<serializable_struct_t>>>);

  // ---- Level 4: optional -> variant -> struct -> optional ----
  static_assert(rfl::serializable<std::optional<std::variant<int, serializable_struct_t>>>);
  static_assert(rfl::serializable<std::optional<std::variant<serializable_struct_t, double>>>);

  // ---- Level 4: variant -> range -> struct -> variant ----
  static_assert(rfl::serializable<std::variant<std::vector<serializable_struct_t>, int>>);
  static_assert(rfl::serializable<std::variant<int, std::vector<serializable_struct_t>>>);

  // ---- Level 5: deeply nested combinations ----
  static_assert(
      rfl::serializable<std::vector<std::tuple<std::optional<serializable_struct_t>, int>>>);
  static_assert(
      rfl::serializable<std::list<std::pair<std::string, std::optional<serializable_struct_t>>>>);
  static_assert(
      rfl::serializable<std::optional<std::vector<std::optional<serializable_struct_t>>>>);
  static_assert(rfl::serializable<std::variant<std::list<serializable_struct_t>, double>>);
  static_assert(
      rfl::serializable<std::tuple<std::variant<int, serializable_struct_t>, std::string, bool>>);

  // ---- Level 6+: extremely deep nesting ----
  static_assert(rfl::serializable<
                std::optional<std::vector<std::tuple<int, std::optional<serializable_struct_t>>>>>);
  static_assert(rfl::serializable<
                std::vector<std::optional<std::variant<serializable_struct_t, std::string>>>>);
  static_assert(
      rfl::serializable<std::list<
          std::pair<std::string, std::optional<std::variant<int, serializable_struct_t>>>>>);
  static_assert(rfl::serializable<
                std::optional<std::tuple<std::vector<serializable_struct_t>, int, double>>>);
  static_assert(rfl::serializable<
                std::variant<std::optional<std::list<serializable_struct_t>>, std::string>>);

  // ---- mixed nesting: struct containing range containing tuple containing struct ----
  static_assert(rfl::serializable<std::vector<std::pair<int, std::vector<serializable_struct_t>>>>);
  static_assert(
      rfl::serializable<std::tuple<serializable_struct_t,
                                   std::optional<std::variant<std::vector<int>, std::string>>>>);
  static_assert(
      rfl::serializable<std::optional<std::vector<
          std::tuple<std::optional<int>, std::variant<serializable_struct_t, std::string>>>>>);
  static_assert(rfl::serializable<std::map<std::string, std::vector<serializable_struct_t>>>);
  static_assert(
      rfl::serializable<std::unordered_map<std::string, std::optional<serializable_struct_t>>>);
  static_assert(rfl::serializable<std::multimap<int, std::variant<serializable_struct_t, double>>>);
  static_assert(rfl::serializable<std::map<serializable_struct_t, std::vector<int>>>);
  static_assert(rfl::serializable<std::map<serializable_struct_t, std::set<int>>>);
}

TEST(TypeTraits, SerializableNegativeTests) {
  // ---- Struct with pointer member: ❌ ----
  static_assert(NOT rfl::serializable<struct_with_pointer_t>);
  static_assert(NOT rfl::serializable<std::vector<struct_with_pointer_t>>);
  static_assert(NOT rfl::serializable<std::optional<struct_with_pointer_t>>);
  static_assert(NOT rfl::serializable<std::tuple<int, struct_with_pointer_t>>);
  static_assert(NOT rfl::serializable<std::variant<struct_with_pointer_t, int>>);
  static_assert(NOT rfl::serializable<std::pair<struct_with_pointer_t, double>>);
  static_assert(NOT rfl::serializable<std::map<int, struct_with_pointer_t>>);
  static_assert(NOT rfl::serializable<std::optional<std::vector<struct_with_pointer_t>>>);

  // ---- Struct with reference member: ❌ ----
  static_assert(NOT rfl::serializable<struct_with_reference_t>);
  static_assert(NOT rfl::serializable<std::vector<struct_with_reference_t>>);
  static_assert(NOT rfl::serializable<std::optional<struct_with_reference_t>>);
  static_assert(NOT rfl::serializable<std::tuple<struct_with_reference_t, int>>);
  static_assert(NOT rfl::serializable<std::variant<double, struct_with_reference_t>>);
  static_assert(NOT rfl::serializable<std::pair<std::string, struct_with_reference_t>>);
  static_assert(NOT rfl::serializable<std::unordered_map<int, struct_with_reference_t>>);

  // ---- Struct with private member: ❌ ----
  static_assert(NOT rfl::serializable<struct_with_private_member_t>);
  static_assert(NOT rfl::serializable<std::vector<struct_with_private_member_t>>);
  static_assert(NOT rfl::serializable<std::optional<struct_with_private_member_t>>);
  static_assert(NOT rfl::serializable<std::tuple<struct_with_private_member_t, double>>);
  static_assert(NOT rfl::serializable<std::variant<int, struct_with_private_member_t>>);
  static_assert(NOT rfl::serializable<std::pair<struct_with_private_member_t, bool>>);
  static_assert(NOT rfl::serializable<std::map<struct_with_private_member_t, std::string>>);

  // ---- Derived class with pointer in base: ❌ ----
  static_assert(NOT rfl::serializable<base_with_pointer_t>);
  static_assert(NOT rfl::serializable<derived_from_pointer_base_t>);
  static_assert(NOT rfl::serializable<std::vector<derived_from_pointer_base_t>>);
  static_assert(NOT rfl::serializable<std::optional<derived_from_pointer_base_t>>);
  static_assert(NOT rfl::serializable<std::tuple<derived_from_pointer_base_t, int>>);
  static_assert(NOT rfl::serializable<std::variant<derived_from_pointer_base_t, double>>);
  static_assert(NOT rfl::serializable<std::pair<std::string, derived_from_pointer_base_t>>);
  static_assert(NOT rfl::serializable<std::unordered_map<int, derived_from_pointer_base_t>>);

  // ---- Deep nesting with non-memberwise-serializable structs: ❌ ----
  static_assert(
      NOT rfl::serializable<std::vector<std::pair<int, std::vector<struct_with_pointer_t>>>>);
  static_assert(NOT rfl::serializable<std::optional<std::vector<struct_with_reference_t>>>);
  static_assert(
      NOT rfl::serializable<std::tuple<struct_with_private_member_t, std::optional<int>, double>>);
  static_assert(
      NOT rfl::serializable<std::variant<std::vector<derived_from_pointer_base_t>, std::string>>);
  static_assert(NOT rfl::serializable<std::map<std::string, std::optional<struct_with_pointer_t>>>);
  static_assert(NOT rfl::serializable<std::list<std::pair<std::string, struct_with_reference_t>>>);
}

namespace inheritance {
struct A {
  int a;
  int x;
};
struct B1 {
  int b;
  double y;
};
struct B2 {
  int b;
  double x;
};
struct C1 : A, B1 {
  int c;
};
// Name duplication: A::x and B2::x
struct C2 : A, B2 {
  int c;
};
// Diamond inheritance
struct D : C1, C2 {
  int d;
};
}  // namespace inheritance

TEST(TypeTraits, SerializableNameInheritance) {
  using namespace inheritance;
  static_assert(rfl::serializable<A>);
  static_assert(rfl::serializable<B1>);
  static_assert(rfl::serializable<B2>);
  static_assert(rfl::serializable<C1>);
  static_assert(NOT rfl::serializable<C2>);  // Name duplication
  static_assert(NOT rfl::serializable<D>);   // Inheritance
}