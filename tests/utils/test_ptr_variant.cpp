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

#include <cstdint>
#include <cstring>
#include <reflect_cpp26/utils/ptr_variant.hpp>
#include <string>
#include <type_traits>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

// For both 32-bit and 64-bit targets
static_assert(sizeof(rfl::ptr_variant<int, double>) == 8);

struct Base {
  int value = 0;
  virtual ~Base() = default;
};

struct Derived : Base {
  double extra = 0.0;
};

struct Other {
  std::string name;
};

class UtilsPtrVariant : public ::testing::Test {
protected:
  int int_value = 42;
  double double_value = 3.14;
  std::string string_value = "test";

  Derived derived_value;
  Other other_value;
};

TEST_F(UtilsPtrVariant, DefaultConstruction) {
  rfl::ptr_variant<int, double, std::string> v;
  EXPECT_EQ(rfl::npos, v.index());
}

TEST_F(UtilsPtrVariant, NullptrConstruction) {
  rfl::ptr_variant<int, double, std::string> v(nullptr);
  EXPECT_EQ(rfl::npos, v.index());
}

TEST_F(UtilsPtrVariant, ConstructionFromIntPointer) {
  rfl::ptr_variant<int, double, std::string> v(&int_value);
  EXPECT_EQ(0u, v.index());
  EXPECT_TRUE(rfl::holds_alternative<int*>(v));
  EXPECT_FALSE(rfl::holds_alternative<double*>(v));
  EXPECT_FALSE(rfl::holds_alternative<std::string*>(v));
  EXPECT_EQ(&int_value, rfl::get<int*>(v));
  EXPECT_EQ(nullptr, rfl::get<double*>(v));
  EXPECT_EQ(nullptr, rfl::get<std::string*>(v));
}

TEST_F(UtilsPtrVariant, ConstructionFromDoublePointer) {
  rfl::ptr_variant<int, double, std::string> v(&double_value);
  EXPECT_EQ(1u, v.index());
  EXPECT_FALSE(rfl::holds_alternative<int*>(v));
  EXPECT_TRUE(rfl::holds_alternative<double*>(v));
  EXPECT_FALSE(rfl::holds_alternative<std::string*>(v));
  EXPECT_EQ(&double_value, rfl::get<double*>(v));
}

TEST_F(UtilsPtrVariant, ConstructionFromStringPointer) {
  rfl::ptr_variant<int, double, std::string> v(&string_value);
  EXPECT_EQ(2u, v.index());
  EXPECT_TRUE(rfl::holds_alternative<std::string*>(v));
  EXPECT_EQ(&string_value, rfl::get<std::string*>(v));
}

TEST_F(UtilsPtrVariant, ConstructionFromNullPointer) {
  int* null_int = nullptr;
  rfl::ptr_variant<int, double, std::string> v(null_int);
  EXPECT_EQ(0u, v.index());
  EXPECT_TRUE(rfl::holds_alternative<int*>(v));
  EXPECT_EQ(nullptr, rfl::get<int*>(v));
}

TEST_F(UtilsPtrVariant, CopyConstruction) {
  rfl::ptr_variant<int, double, std::string> v1(&int_value);
  rfl::ptr_variant<int, double, std::string> v2(v1);

  EXPECT_EQ(v1.index(), v2.index());
  EXPECT_EQ(&int_value, rfl::get<int*>(v2));
}

TEST_F(UtilsPtrVariant, MoveConstruction) {
  rfl::ptr_variant<int, double, std::string> v1(&int_value);
  rfl::ptr_variant<int, double, std::string> v2(std::move(v1));

  EXPECT_EQ(0u, v2.index());
  EXPECT_EQ(&int_value, rfl::get<int*>(v2));
}

TEST_F(UtilsPtrVariant, CopyAssignment) {
  rfl::ptr_variant<int, double, std::string> v1(&int_value);
  rfl::ptr_variant<int, double, std::string> v2(&double_value);

  v2 = v1;
  EXPECT_EQ(0u, v2.index());
  EXPECT_EQ(&int_value, rfl::get<int*>(v2));
}

TEST_F(UtilsPtrVariant, MoveAssignment) {
  rfl::ptr_variant<int, double, std::string> v1(&int_value);
  rfl::ptr_variant<int, double, std::string> v2(&double_value);

  v2 = std::move(v1);
  EXPECT_EQ(0u, v2.index());
  EXPECT_EQ(&int_value, rfl::get<int*>(v2));
}

TEST_F(UtilsPtrVariant, NullptrAssignment) {
  rfl::ptr_variant<int, double, std::string> v(&int_value);
  EXPECT_NE(rfl::npos, v.index());

  v = nullptr;
  EXPECT_EQ(rfl::npos, v.index());
}

TEST_F(UtilsPtrVariant, GetByIndex) {
  rfl::ptr_variant<int, double, std::string> v(&int_value);

  EXPECT_EQ(&int_value, rfl::get<0>(v));
  EXPECT_EQ(nullptr, rfl::get<1>(v));
  EXPECT_EQ(nullptr, rfl::get<2>(v));
}

TEST_F(UtilsPtrVariant, GetByDifferentIndex) {
  rfl::ptr_variant<int, double, std::string> v(&double_value);

  EXPECT_EQ(nullptr, rfl::get<0>(v));
  EXPECT_EQ(&double_value, rfl::get<1>(v));
  EXPECT_EQ(nullptr, rfl::get<2>(v));
}

TEST_F(UtilsPtrVariant, HoldsAlternativeByIndex) {
  rfl::ptr_variant<int, double, std::string> v(&string_value);

  EXPECT_FALSE(rfl::holds_alternative<0>(v));
  EXPECT_FALSE(rfl::holds_alternative<1>(v));
  EXPECT_TRUE(rfl::holds_alternative<2>(v));
}

TEST_F(UtilsPtrVariant, ConstPtrVariant) {
  rfl::ptr_variant<int, double, std::string> v(&int_value);
  // const-ness of cv itself does not propagate to the pointer inside
  const rfl::ptr_variant<int, double, std::string>& cv = v;

  EXPECT_TRUE(rfl::holds_alternative<int*>(cv));
  EXPECT_EQ(&int_value, rfl::get<int*>(cv));
}

TEST_F(UtilsPtrVariant, InheritanceHierarchy) {
  rfl::ptr_variant<Base, Other> v(&derived_value);
  EXPECT_EQ(0u, v.index());
  EXPECT_TRUE(rfl::holds_alternative<Base*>(v));
  EXPECT_EQ(&derived_value, rfl::get<Base*>(v));
}

TEST_F(UtilsPtrVariant, NullptrAfterAssignment) {
  rfl::ptr_variant<int, double, std::string> v1(nullptr);
  rfl::ptr_variant<int, double, std::string> v2(&int_value);

  v2 = v1;
  EXPECT_EQ(rfl::npos, v2.index());
}

TEST_F(UtilsPtrVariant, MultipleTypeChanges) {
  rfl::ptr_variant<int, double, std::string> v(&int_value);
  EXPECT_EQ(0u, v.index());

  v = &double_value;
  EXPECT_EQ(1u, v.index());
  EXPECT_TRUE(rfl::holds_alternative<double*>(v));

  v = &string_value;
  EXPECT_EQ(2u, v.index());
  EXPECT_TRUE(rfl::holds_alternative<std::string*>(v));

  v = nullptr;
  EXPECT_EQ(rfl::npos, v.index());
}

TEST_F(UtilsPtrVariant, TypeAliasConstPtrVariant) {
  static_assert(std::is_same_v<rfl::const_ptr_variant<int, double>,
                               rfl::ptr_variant<const int, const double>>);
}

TEST_F(UtilsPtrVariant, SingleTypeVariant) {
  rfl::ptr_variant<int> v(&int_value);
  EXPECT_EQ(0u, v.index());
  EXPECT_TRUE(rfl::holds_alternative<int*>(v));
  EXPECT_EQ(&int_value, rfl::get<int*>(v));
  EXPECT_EQ(&int_value, rfl::get<0>(v));
  EXPECT_TRUE(rfl::holds_alternative<0>(v));
}

TEST_F(UtilsPtrVariant, TwoTypeVariant) {
  rfl::ptr_variant<int, double> v_int(&int_value);
  rfl::ptr_variant<int, double> v_double(&double_value);

  EXPECT_EQ(0u, v_int.index());
  EXPECT_EQ(1u, v_double.index());

  EXPECT_TRUE(rfl::holds_alternative<int*>(v_int));
  EXPECT_TRUE(rfl::holds_alternative<double*>(v_double));
}

TEST_F(UtilsPtrVariant, VisitAllTypes) {
  rfl::ptr_variant<int, double, std::string> v(&int_value);

  auto visitor = [](auto ptr) -> std::string {
    using T = std::remove_pointer_t<decltype(ptr)>;
    if constexpr (std::is_same_v<T, int>) {
      return "int: " + std::to_string(*ptr);
    } else if constexpr (std::is_same_v<T, double>) {
      return "double: " + std::to_string(*ptr);
    } else if constexpr (std::is_same_v<T, std::string>) {
      return "string: " + *ptr;
    } else {
      return "nullptr";
    }
  };
  EXPECT_EQ("int: 42", v.visit(visitor));
  v = &double_value;
  EXPECT_EQ("double: 3.14", v.visit(visitor));
  v = &string_value;
  EXPECT_EQ("string: test", v.visit(visitor));
}
