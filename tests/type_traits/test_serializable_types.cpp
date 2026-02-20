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

// ---- Test structs for memberwise_serializable ----

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

// Struct with a pointer member (not memberwise_serializable)
struct struct_with_pointer_t {
  int x;
  int* ptr;
};

// Struct with a reference member (not memberwise_serializable)
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
  // ---- std::monostate: ✔️ ----
  static_assert(rfl::serializable<std::monostate>);

  // ---- Arithmetic types (including bool and all character types): ✔️ ----
  // Signed integers: ✔️
  static_assert(rfl::serializable<int>);
  static_assert(rfl::serializable<int8_t>);
  static_assert(rfl::serializable<int64_t>);
  // Unsigned integers: ✔️
  static_assert(rfl::serializable<unsigned>);
  static_assert(rfl::serializable<uint8_t>);
  static_assert(rfl::serializable<uint64_t>);
  // Floating-point types: ✔️
  static_assert(rfl::serializable<float>);
  static_assert(rfl::serializable<double>);
  // Boolean type: ✔️
  static_assert(rfl::serializable<bool>);
  // All character types: ✔️
  static_assert(rfl::serializable<char>);
  static_assert(rfl::serializable<wchar_t>);
  static_assert(rfl::serializable<char16_t>);
  static_assert(rfl::serializable<char32_t>);
  static_assert(rfl::serializable<char8_t>);

  // ---- Enum types: ✔️ ----
  static_assert(rfl::serializable<color_t>);

  // ---- String-like types: ✔️ (all character types supported) ----
  // char-based: ✔️
  static_assert(rfl::serializable<std::string>);
  static_assert(rfl::serializable<std::string_view>);
  static_assert(rfl::serializable<const char*>);
  static_assert(rfl::serializable<char[16]>);
  // wchar_t-based: ✔️
  static_assert(rfl::serializable<std::wstring>);
  static_assert(rfl::serializable<std::wstring_view>);
  static_assert(rfl::serializable<const wchar_t*>);
  static_assert(rfl::serializable<wchar_t[16]>);
  // char16_t-based: ✔️
  static_assert(rfl::serializable<std::u16string>);
  static_assert(rfl::serializable<std::u16string_view>);
  static_assert(rfl::serializable<const char16_t*>);
  static_assert(rfl::serializable<char16_t[16]>);
  // char32_t-based: ✔️
  static_assert(rfl::serializable<std::u32string>);
  static_assert(rfl::serializable<std::u32string_view>);
  static_assert(rfl::serializable<const char32_t*>);
  static_assert(rfl::serializable<char32_t[16]>);
  // char8_t-based: ✔️
  static_assert(rfl::serializable<std::u8string>);
  static_assert(rfl::serializable<std::u8string_view>);
  static_assert(rfl::serializable<const char8_t*>);
  static_assert(rfl::serializable<char8_t[16]>);

  // ---- Array types: ✔️ (element type must be serializable) ----
  static_assert(rfl::serializable<int[4]>);
  static_assert(rfl::serializable<double[2][3]>);  // Multi-dimensional
  static_assert(rfl::serializable<color_t[8]>);
  static_assert(rfl::serializable<std::string[4]>);
  static_assert(rfl::serializable<wchar_t[4]>);       // wchar_t is serializable
  static_assert(rfl::serializable<std::wstring[2]>);  // wstring elements are serializable
  // Array of non-serializable element: ❌
  static_assert(NOT rfl::serializable<int* [4]>);  // int* is not serializable

  // ---- Range types: ✔️ (element type must be serializable) ----
  static_assert(rfl::serializable<std::vector<int>>);
  static_assert(rfl::serializable<std::vector<double>>);
  static_assert(rfl::serializable<std::vector<color_t>>);
  static_assert(rfl::serializable<std::vector<std::string>>);
  static_assert(rfl::serializable<std::vector<std::wstring>>);  // wstring is serializable
  static_assert(rfl::serializable<std::vector<wchar_t>>);       // wchar_t is serializable
  static_assert(rfl::serializable<std::deque<int>>);
  static_assert(rfl::serializable<std::list<int>>);
  static_assert(rfl::serializable<std::array<int, 4>>);
  static_assert(rfl::serializable<std::array<std::wstring, 2>>);
  // Range of non-serializable element: ❌
  static_assert(NOT rfl::serializable<std::vector<int*>>);     // int* is not serializable
  static_assert(NOT rfl::serializable<std::array<void*, 2>>);  // void* is not serializable

  // ---- Associative containers: ✔️ (range with serializable value_type) ----
  // std::map: value_type is std::pair<const Key, T>
  static_assert(rfl::serializable<std::map<int, double>>);
  static_assert(rfl::serializable<std::map<std::string, int>>);
  static_assert(rfl::serializable<std::map<int, std::string>>);
  static_assert(rfl::serializable<std::map<std::string, std::vector<int>>>);
  // std::unordered_map: value_type is std::pair<const Key, T>
  static_assert(rfl::serializable<std::unordered_map<int, double>>);
  static_assert(rfl::serializable<std::unordered_map<std::string, color_t>>);
  // std::set: value_type is Key
  static_assert(rfl::serializable<std::set<int>>);
  static_assert(rfl::serializable<std::set<std::string>>);
  // std::unordered_set: value_type is Key
  static_assert(rfl::serializable<std::unordered_set<int>>);
  static_assert(rfl::serializable<std::unordered_set<double>>);
  // std::multimap and std::multiset
  static_assert(rfl::serializable<std::multimap<int, double>>);
  static_assert(rfl::serializable<std::multiset<int>>);
  // Associative container with non-serializable value_type: ❌
  static_assert(NOT rfl::serializable<std::map<int, int*>>);
  static_assert(NOT rfl::serializable<std::set<int*>>);
  static_assert(NOT rfl::serializable<std::unordered_map<std::string, void*>>);

  // ---- Tuple-like types: ✔️ (all elements must be serializable) ----
  static_assert(rfl::serializable<std::pair<int, double>>);
  static_assert(rfl::serializable<std::pair<std::string, color_t>>);
  static_assert(rfl::serializable<std::pair<wchar_t, char16_t>>);  // All char types serializable
  static_assert(rfl::serializable<std::tuple<int, double, std::string>>);
  static_assert(rfl::serializable<std::tuple<int, std::vector<int>>>);
  static_assert(rfl::serializable<std::tuple<std::wstring, std::u16string>>);
  // Tuple with non-serializable element: ❌
  static_assert(NOT rfl::serializable<std::pair<int, int*>>);
  static_assert(NOT rfl::serializable<std::tuple<int, void*>>);
  static_assert(NOT rfl::serializable<std::pair<int, std::vector<int*>>>);

  // ---- Nested tuple-like types: ✔️ ----
  // pair of pairs
  static_assert(rfl::serializable<std::pair<std::pair<int, double>, std::pair<std::string, bool>>>);
  // tuple containing pair
  static_assert(rfl::serializable<std::tuple<int, std::pair<double, std::string>, bool>>);
  // pair containing tuple
  static_assert(rfl::serializable<std::pair<std::tuple<int, double>, std::string>>);
  // deeply nested
  static_assert(
      rfl::serializable<std::tuple<std::pair<std::tuple<int, double>, bool>, std::string>>);
  // Nested with non-serializable element: ❌
  static_assert(NOT rfl::serializable<std::pair<std::pair<int, int*>, double>>);
  static_assert(NOT rfl::serializable<std::tuple<std::pair<int, void*>, bool>>);

  // ---- std::optional: ✔️ (contained type must be serializable) ----
  static_assert(rfl::serializable<std::optional<int>>);
  static_assert(rfl::serializable<std::optional<double>>);
  static_assert(rfl::serializable<std::optional<color_t>>);
  static_assert(rfl::serializable<std::optional<std::string>>);
  static_assert(rfl::serializable<std::optional<std::wstring>>);
  static_assert(rfl::serializable<std::optional<std::vector<int>>>);
  static_assert(rfl::serializable<std::optional<std::pair<int, double>>>);
  // Optional of non-serializable type: ❌
  static_assert(NOT rfl::serializable<std::optional<int*>>);
  static_assert(NOT rfl::serializable<std::optional<void*>>);

  // ---- std::variant: ✔️ (all alternative types must be serializable) ----
  static_assert(rfl::serializable<std::variant<int, double>>);
  static_assert(rfl::serializable<std::variant<int, std::string>>);
  static_assert(rfl::serializable<std::variant<int, std::wstring>>);
  static_assert(rfl::serializable<std::variant<int, double, std::string>>);
  static_assert(rfl::serializable<std::variant<std::monostate, int, std::string>>);
  static_assert(rfl::serializable<std::variant<color_t, std::string, bool>>);
  // Variant with non-serializable alternative: ❌
  static_assert(NOT rfl::serializable<std::variant<int, int*>>);
  static_assert(NOT rfl::serializable<std::variant<int, void*>>);
  static_assert(NOT rfl::serializable<std::variant<int*, double*>>);

  // ---- Other non-serializable types: ❌ ----
  static_assert(NOT rfl::serializable<void>);
  static_assert(NOT rfl::serializable<int*>);
  static_assert(NOT rfl::serializable<int&>);
  static_assert(NOT rfl::serializable<const int&>);
  static_assert(NOT rfl::serializable<int()>);         // Function type
  static_assert(NOT rfl::serializable<int (*)()>);     // Function pointer
  static_assert(NOT rfl::serializable<int foo_t::*>);  // Member pointer
}

TEST(TypeTraits, MemberwiseSerializable) {
  // ---- All serializable types are memberwise serializable: ✔️ ----
  // Arithmetic types
  static_assert(rfl::memberwise_serializable<int>);
  static_assert(rfl::memberwise_serializable<double>);
  static_assert(rfl::memberwise_serializable<bool>);
  static_assert(rfl::memberwise_serializable<char>);
  static_assert(rfl::memberwise_serializable<wchar_t>);
  // Enum types
  static_assert(rfl::memberwise_serializable<color_t>);
  // String-like types
  static_assert(rfl::memberwise_serializable<std::string>);
  static_assert(rfl::memberwise_serializable<std::wstring>);
  // Range types
  static_assert(rfl::memberwise_serializable<std::vector<int>>);
  static_assert(rfl::memberwise_serializable<std::array<double, 4>>);
  // Tuple-like types
  static_assert(rfl::memberwise_serializable<std::pair<int, double>>);
  static_assert(rfl::memberwise_serializable<std::tuple<int, std::string, bool>>);
  // std::optional
  static_assert(rfl::memberwise_serializable<std::optional<int>>);
  // std::variant
  static_assert(rfl::memberwise_serializable<std::variant<int, double>>);
  // std::monostate
  static_assert(rfl::memberwise_serializable<std::monostate>);

  // ---- Flattenable class types with all serializable members: ✔️ ----
  // Empty class is trivially flattenable
  static_assert(rfl::memberwise_serializable<foo_t>);
  // Simple struct with serializable members
  static_assert(rfl::memberwise_serializable<serializable_struct_t>);
  // Nested struct with serializable members
  static_assert(rfl::memberwise_serializable<nested_serializable_struct_t>);
  // Struct with inheritance (all members serializable)
  static_assert(rfl::memberwise_serializable<base_serializable_t>);
  static_assert(rfl::memberwise_serializable<derived_serializable_t>);
  // Multi-level inheritance with serializable members
  static_assert(rfl::memberwise_serializable<grandparent_t>);
  static_assert(rfl::memberwise_serializable<parent_t>);
  static_assert(rfl::memberwise_serializable<child_t>);

  // ---- Class types with non-serializable members: ❌ ----
  // Struct with pointer member (pointer is not memberwise_serializable)
  static_assert(NOT rfl::memberwise_serializable<struct_with_pointer_t>);
  // Struct with reference member (reference is not memberwise_serializable)
  static_assert(NOT rfl::memberwise_serializable<struct_with_reference_t>);
  // Struct with private member (not flattenable_class)
  static_assert(NOT rfl::memberwise_serializable<struct_with_private_member_t>);
  // Struct with inheritance where base has pointer
  static_assert(NOT rfl::memberwise_serializable<base_with_pointer_t>);
  static_assert(NOT rfl::memberwise_serializable<derived_from_pointer_base_t>);

  // ---- Other non-memberwise-serializable types: ❌ ----
  static_assert(NOT rfl::memberwise_serializable<void>);
  static_assert(NOT rfl::memberwise_serializable<int*>);
  static_assert(NOT rfl::memberwise_serializable<int&>);
  static_assert(NOT rfl::memberwise_serializable<int()>);
  static_assert(NOT rfl::memberwise_serializable<int (*)()>);
}
