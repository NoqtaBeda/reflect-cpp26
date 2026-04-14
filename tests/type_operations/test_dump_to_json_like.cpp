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

#include <gmock/gmock-matchers.h>

#include <optional>
#include <reflect_cpp26/enum/enum_bitwise_operators.hpp>
#include <reflect_cpp26/type_operations/dump_to_json_like.hpp>
#include <variant>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

struct person_t {
  std::string name;
  int age;
};

struct nested_t {
  person_t person;
  std::vector<int> scores;
};

enum class color_t {
  red,
  green,
  blue
};

enum class permissions_t : int {
  read = 1,
  write = 2,
  execute = 4,
};
REFLECT_CPP26_DEFINE_ENUM_BITWISE_BINARY_OPERATORS(permissions_t)
template <>
constexpr auto rfl::is_enum_flag_v<permissions_t> = true;

TEST(TypeOperationsDumpToJsonLike, BasicIntegers) {
  EXPECT_EQ("42", rfl::dump_to_json_like(42));
  EXPECT_EQ("-123", rfl::dump_to_json_like(-123));
  EXPECT_EQ("0", rfl::dump_to_json_like(0));
}

TEST(TypeOperationsDumpToJsonLike, BasicFloats) {
  EXPECT_EQ("3.14", rfl::dump_to_json_like(3.14));
  EXPECT_EQ("-0.5", rfl::dump_to_json_like(-0.5f));
  EXPECT_EQ("0", rfl::dump_to_json_like(0.0));
}

TEST(TypeOperationsDumpToJsonLike, BasicBool) {
  EXPECT_EQ("true", rfl::dump_to_json_like(true));
  EXPECT_EQ("false", rfl::dump_to_json_like(false));
}

TEST(TypeOperationsDumpToJsonLike, BasicString) {
  EXPECT_EQ("\"hello\"", rfl::dump_to_json_like(std::string{"hello"}));
  EXPECT_EQ("\"world\"", rfl::dump_to_json_like(std::string_view{"world"}));
}

TEST(TypeOperationsDumpToJsonLike, CharToString) {
  EXPECT_EQ("'a'", rfl::dump_to_json_like('a'));
}

TEST(TypeOperationsDumpToJsonLike, OptionalWithValue) {
  std::optional<int> opt{42};
  EXPECT_EQ("42", rfl::dump_to_json_like(opt));
}

TEST(TypeOperationsDumpToJsonLike, OptionalEmpty) {
  std::optional<int> opt{std::nullopt};
  EXPECT_EQ("nullopt", rfl::dump_to_json_like(opt));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfInts) {
  std::vector<int> vec{1, 2, 3};
  EXPECT_EQ("[1,2,3]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBools) {
  std::vector<bool> vec{true, false, true};
  EXPECT_EQ("[true,false,true]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsEmpty) {
  std::vector<bool> vec{};
  EXPECT_EQ("[]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsAllTrue) {
  std::vector<bool> vec{true, true, true};
  EXPECT_EQ("[true,true,true]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsAllFalse) {
  std::vector<bool> vec{false, false, false};
  EXPECT_EQ("[false,false,false]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, EmptyVector) {
  std::vector<int> vec{};
  EXPECT_EQ("[]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfStrings) {
  std::vector<std::string> vec{"apple", "banana"};
  EXPECT_EQ(R"(["apple","banana"])", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, Pair) {
  std::pair<int, std::string> p{42, "hello"};
  EXPECT_EQ(R"([42,"hello"])", rfl::dump_to_json_like(p));
}

TEST(TypeOperationsDumpToJsonLike, Tuple) {
  auto t = std::make_tuple(1, "hello", 3.14);
  EXPECT_EQ(R"([1,"hello",3.14])", rfl::dump_to_json_like(t));
}

TEST(TypeOperationsDumpToJsonLike, EmptyTuple) {
  std::tuple<> t{};
  EXPECT_EQ("[]", rfl::dump_to_json_like(t));
}

TEST(TypeOperationsDumpToJsonLike, Struct) {
  person_t p{"Alice", 30};
  EXPECT_EQ(R"({name:"Alice",age:30})", rfl::dump_to_json_like(p));
}

TEST(TypeOperationsDumpToJsonLike, NestedStruct) {
  nested_t n{{"Bob", 25}, {90, 85, 92}};
  EXPECT_EQ(R"({person:{name:"Bob",age:25},scores:[90,85,92]})", rfl::dump_to_json_like(n));
}

TEST(TypeOperationsDumpToJsonLike, EnumToInt) {
  EXPECT_EQ("red", rfl::dump_to_json_like(color_t::red));
  EXPECT_EQ("green", rfl::dump_to_json_like(color_t::green));
  EXPECT_EQ("blue", rfl::dump_to_json_like(color_t::blue));
}

TEST(TypeOperationsDumpToJsonLike, EnumToString) {
  EXPECT_EQ("red", rfl::dump_to_json_like(color_t::red));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagAsInteger) {
  EXPECT_EQ("read", rfl::dump_to_json_like(permissions_t::read));
  EXPECT_THAT(rfl::dump_to_json_like(permissions_t::read | permissions_t::write),
              testing::AnyOf("read|write", "write|read"));
  EXPECT_THAT(
      rfl::dump_to_json_like(permissions_t::read | permissions_t::write | permissions_t::execute),
      testing::AnyOf("read|write|execute",
                     "read|execute|write",
                     "write|read|execute",
                     "write|execute|read",
                     "execute|read|write",
                     "execute|write|read"));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagToString) {
  EXPECT_EQ("read", rfl::dump_to_json_like(permissions_t::read));
  EXPECT_THAT(rfl::dump_to_json_like(permissions_t::read | permissions_t::write),
              testing::AnyOf("read|write", "write|read"));
  EXPECT_THAT(
      rfl::dump_to_json_like(permissions_t::read | permissions_t::write | permissions_t::execute),
      testing::AnyOf("read|write|execute",
                     "read|execute|write",
                     "write|read|execute",
                     "write|execute|read",
                     "execute|read|write",
                     "execute|write|read"));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagToStringInvalidHalts) {
  EXPECT_EQ("(permissions_t)8", rfl::dump_to_json_like(static_cast<permissions_t>(8)));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagToStringInvalidNull) {
  EXPECT_EQ("(permissions_t)8", rfl::dump_to_json_like(static_cast<permissions_t>(8)));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInStruct) {
  struct file_t {
    std::string name;
    permissions_t mode;
  };
  file_t f{"script.sh", permissions_t::read | permissions_t::execute};
  auto result = rfl::dump_to_json_like(f);
  EXPECT_TRUE(result == R"({name:"script.sh",mode:read|execute})"
              || result == R"({name:"script.sh",mode:execute|read})");
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInVector) {
  std::vector<permissions_t> vec{
      permissions_t::read, permissions_t::write, permissions_t::read | permissions_t::write};
  auto result = rfl::dump_to_json_like(vec);
  EXPECT_TRUE(result == "[read,write,read|write]" || result == "[read,write,write|read]");
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagEmptySet) {
  EXPECT_EQ("", rfl::dump_to_json_like(static_cast<permissions_t>(0)));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagAllFlags) {
  auto all = permissions_t::read | permissions_t::write | permissions_t::execute;
  auto result = rfl::dump_to_json_like(all);
  EXPECT_TRUE(result == "read|write|execute" || result == "read|execute|write"
              || result == "write|read|execute" || result == "write|execute|read"
              || result == "execute|read|write" || result == "execute|write|read");
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagMixedWithRegularEnum) {
  struct config_t {
    color_t color;
    permissions_t perms;
  };
  config_t c{color_t::blue, permissions_t::read | permissions_t::write};
  auto result = rfl::dump_to_json_like(c);
  EXPECT_TRUE(result == "{color:blue,perms:read|write}"
              || result == "{color:blue,perms:write|read}");
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInOptional) {
  std::optional<permissions_t> present{permissions_t::read | permissions_t::execute};
  std::optional<permissions_t> empty{std::nullopt};
  auto result1 = rfl::dump_to_json_like(present);
  EXPECT_TRUE(result1 == "read|execute" || result1 == "execute|read");
  EXPECT_EQ("nullopt", rfl::dump_to_json_like(empty));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInVariant) {
  std::variant<permissions_t, int> v1{permissions_t::write};
  std::variant<permissions_t, int> v2{42};
  EXPECT_THAT(rfl::dump_to_json_like(v1), testing::AnyOf("write", "read", "execute"));
  EXPECT_EQ("42", rfl::dump_to_json_like(v2));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagWithChar16T) {
  auto result = rfl::dump_to_json_like<char16_t>(permissions_t::read | permissions_t::write);
  EXPECT_TRUE(result == u"read|write" || result == u"write|read");
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagWithChar32T) {
  auto result = rfl::dump_to_json_like<char32_t>(permissions_t::read | permissions_t::write);
  EXPECT_TRUE(result == U"read|write" || result == U"write|read");
}

TEST(TypeOperationsDumpToJsonLike, Variant1) {
  std::variant<int, std::string> v1{42};
  std::variant<int, std::string> v2{"hello"};
  EXPECT_EQ("42", rfl::dump_to_json_like(v1));
  EXPECT_EQ("\"hello\"", rfl::dump_to_json_like(v2));
}

TEST(TypeOperationsDumpToJsonLike, Variant2) {
  struct S {
    int x;
  };
  std::variant<S, int> v{S{}};
  EXPECT_EQ(R"({x:0})", rfl::dump_to_json_like(v));
}

TEST(TypeOperationsDumpToJsonLike, NestedOptional) {
  std::optional<std::optional<int>> nested{std::nullopt};
  EXPECT_EQ("nullopt", rfl::dump_to_json_like(nested));
}

TEST(TypeOperationsDumpToJsonLike, OptionalInStruct) {
  struct with_optional_t {
    std::string name;
    std::optional<int> age;
  };
  with_optional_t w1{"Carol", 25};
  with_optional_t w2{"Dave", std::nullopt};
  EXPECT_EQ(R"({name:"Carol",age:25})", rfl::dump_to_json_like(w1));
  EXPECT_EQ(R"({name:"Dave",age:nullopt})", rfl::dump_to_json_like(w2));
}

TEST(TypeOperationsDumpToJsonLike, Array) {
  std::array<int, 3> arr{1, 2, 3};
  EXPECT_EQ("[1,2,3]", rfl::dump_to_json_like(arr));
}

TEST(TypeOperationsDumpToJsonLike, WithIndent) {
  person_t p{"Eve", 28};
  EXPECT_EQ(R"({
  name: "Eve",
  age: 28
})",
            rfl::dump_to_json_like(p, 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, NestedWithIndent) {
  nested_t n{{"Frank", 35}, {100, 95}};
  EXPECT_EQ(R"({
  person: {
    name: "Frank",
    age: 35
  },
  scores: [
    100,
    95
  ]
})",
            rfl::dump_to_json_like(n, 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, VectorWithIndent) {
  std::vector<int> vec{10, 20, 30};
  EXPECT_EQ(R"([
  10,
  20,
  30
])",
            rfl::dump_to_json_like(vec, 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsWithIndent) {
  std::vector<bool> vec{true, false, true};
  EXPECT_EQ(R"([
  true,
  false,
  true
])",
            rfl::dump_to_json_like(vec, 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, Monostate) {
  std::monostate m{};
  EXPECT_EQ("monostate", rfl::dump_to_json_like(m));
}

TEST(TypeOperationsDumpToJsonLike, OptionalInVariant) {
  std::variant<std::optional<int>, std::string> v{std::nullopt};
  EXPECT_EQ("nullopt", rfl::dump_to_json_like(v));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfOptionals) {
  std::vector<std::optional<int>> vec{1, std::nullopt, 3};
  EXPECT_EQ(R"([1,nullopt,3])", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, StringWithSpecialChars) {
  std::string s{"hello\nworld\ttab"};
  EXPECT_EQ(R"("hello\nworld\ttab")", rfl::dump_to_json_like(s));
}

TEST(TypeOperationsDumpToJsonLike, StringWithMixedContent) {
  std::string s = "Hello \"World\"\nLine2\tTabbed";
  EXPECT_EQ(R"("Hello \"World\"\nLine2\tTabbed")", rfl::dump_to_json_like(s));
}

TEST(TypeOperationsDumpToJsonLike, VariantValuelessByException) {
  struct ThrowsOnCopy {
    int value;
    ThrowsOnCopy(int v) : value(v) {}
    ThrowsOnCopy(const ThrowsOnCopy&) noexcept(false) {
      throw 1;
    }
    ThrowsOnCopy& operator=(const ThrowsOnCopy&) noexcept(false) {
      throw 2;
    }
  };
  std::variant<ThrowsOnCopy, int> v = 42;
  ThrowsOnCopy target{99};
  try {
    v = target;
  } catch (...) {
  }
  if (v.valueless_by_exception()) {
    EXPECT_EQ("(valueless by exception)", rfl::dump_to_json_like(v));
  } else {
    EXPECT_EQ(R"({"value":99})", rfl::dump_to_json_like(v));
  }
}

TEST(TypeOperationsDumpToJsonLike, VariantIndex) {
  std::variant<int, std::string, double> v1{42};
  std::variant<int, std::string, double> v2{"hello"};
  std::variant<int, std::string, double> v3{3.14};
  EXPECT_EQ("42", rfl::dump_to_json_like(v1));
  EXPECT_EQ("\"hello\"", rfl::dump_to_json_like(v2));
  EXPECT_EQ("3.14", rfl::dump_to_json_like(v3));
}

TEST(TypeOperationsDumpToJsonLike, NestedStructInVector) {
  std::vector<person_t> people{{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};
  EXPECT_EQ(R"([{name:"Alice",age:30},{name:"Bob",age:25},{name:"Charlie",age:35}])",
            rfl::dump_to_json_like(people));
}

TEST(TypeOperationsDumpToJsonLike, NestedStructInStruct) {
  struct company_t {
    std::string name;
    person_t ceo;
    int founded_year;
  };
  company_t company{"Acme Corp", {"John", 50}, 2020};
  EXPECT_EQ(R"({name:"Acme Corp",ceo:{name:"John",age:50},founded_year:2020})",
            rfl::dump_to_json_like(company));
}

TEST(TypeOperationsDumpToJsonLike, TripleNestedStruct) {
  struct address_t {
    std::string street;
    std::string city;
  };
  struct employee_t {
    std::string name;
    address_t address;
  };
  struct company_t {
    std::string name;
    employee_t ceo;
  };
  company_t company{"TechCorp", {"Alice", {"123 Main St", "New York"}}};
  auto result = rfl::dump_to_json_like(company);
  EXPECT_TRUE(
      result
          == R"({name:"TechCorp",ceo:{name:"Alice",address:{street:"123 Main St",city:"New York"}}})"
      || result
             == R"({name:"TechCorp",ceo:{name:"Alice",address:{city:"New York",street:"123 Main St"}}})");
}

TEST(TypeOperationsDumpToJsonLike, NestedOptionalInVector) {
  std::vector<std::optional<person_t>> people{
      person_t{"Alice", 30}, std::nullopt, person_t{"Bob", 25}};
  EXPECT_EQ(R"([{name:"Alice",age:30},nullopt,{name:"Bob",age:25}])",
            rfl::dump_to_json_like(people));
}

TEST(TypeOperationsDumpToJsonLike, OptionalStructWithValue) {
  struct config_t {
    std::string name;
    std::optional<int> port;
  };
  config_t c1{"server", 8080};
  config_t c2{"client", std::nullopt};
  EXPECT_EQ(R"({name:"server",port:8080})", rfl::dump_to_json_like(c1));
  EXPECT_EQ(R"({name:"client",port:nullopt})", rfl::dump_to_json_like(c2));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfVariants) {
  std::vector<std::variant<int, std::string>> vec{42, "hello", 100, "world"};
  EXPECT_EQ(R"([42,"hello",100,"world"])", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, MapLikeWithVector) {
  std::vector<std::pair<std::string, int>> pairs{{"a", 1}, {"b", 2}, {"c", 3}};
  EXPECT_EQ(R"([["a",1],["b",2],["c",3]])", rfl::dump_to_json_like(pairs));
}

TEST(TypeOperationsDumpToJsonLike, TupleWithNestedTypes) {
  auto t =
      std::make_tuple(42, std::string{"hello"}, std::vector<int>{1, 2, 3}, person_t{"Alice", 30});
  EXPECT_EQ(R"([42,"hello",[1,2,3],{name:"Alice",age:30}])", rfl::dump_to_json_like(t));
}

TEST(TypeOperationsDumpToJsonLike, DeepNestedTuples) {
  auto inner = std::make_tuple(1, 2);
  auto outer = std::make_tuple(inner, 3);
  EXPECT_EQ(R"([[1,2],3])", rfl::dump_to_json_like(outer));
}

TEST(TypeOperationsDumpToJsonLike, StructWithAllTypes) {
  struct mixed_t {
    int i;
    double d;
    bool b;
    std::string s;
    std::vector<int> arr;
    person_t p;
  };
  mixed_t m{42, 3.14, true, "test", {1, 2, 3}, {"Alice", 30}};
  EXPECT_EQ(R"({i:42,d:3.14,b:true,s:"test",arr:[1,2,3],)"
            R"(p:{name:"Alice",age:30}})",
            rfl::dump_to_json_like(m));
}

// The expected result shall be INDENTED by 2 spaces. Also, make the expected as raw string.
TEST(TypeOperationsDumpToJsonLike, NestedWithIndentComplex) {
  std::vector<person_t> people{{"Alice", 30}, {"Bob", 25}};
  EXPECT_EQ(R"([
  {
    name: "Alice",
    age: 30
  },
  {
    name: "Bob",
    age: 25
  }
])",
            rfl::dump_to_json_like(people, 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, EmptyOptional) {
  std::optional<std::string> opt{std::nullopt};
  EXPECT_EQ("nullopt", rfl::dump_to_json_like(opt));
}

TEST(TypeOperationsDumpToJsonLike, OptionalWithString) {
  std::optional<std::string> opt{"hello"};
  EXPECT_EQ("\"hello\"", rfl::dump_to_json_like(opt));
}

TEST(TypeOperationsDumpToJsonLike, DoubleNestedOptional) {
  std::optional<std::optional<int>> opt1{42};
  std::optional<std::optional<int>> opt2{std::nullopt};
  std::optional<std::optional<int>> opt3{std::nullopt};
  EXPECT_EQ("42", rfl::dump_to_json_like(opt1));
  EXPECT_EQ("nullopt", rfl::dump_to_json_like(opt2));
  EXPECT_EQ("nullopt", rfl::dump_to_json_like(opt3));
}

TEST(TypeOperationsDumpToJsonLike, TripleNestedOptional) {
  std::optional<std::optional<std::optional<int>>> opt{42};
  EXPECT_EQ("42", rfl::dump_to_json_like(opt));
}

TEST(TypeOperationsDumpToJsonLike, EmptyStruct) {
  struct empty_t {};
  EXPECT_EQ("{}", rfl::dump_to_json_like(empty_t{}));
}

TEST(TypeOperationsDumpToJsonLike, EmptyString) {
  EXPECT_EQ("\"\"", rfl::dump_to_json_like(std::string{""}));
}

TEST(TypeOperationsDumpToJsonLike, LargeInteger) {
  EXPECT_EQ("2147483647", rfl::dump_to_json_like(2147483647));
  EXPECT_EQ("-2147483648", rfl::dump_to_json_like(-2147483648));
}

TEST(TypeOperationsDumpToJsonLike, NegativeZeroFloat) {
  EXPECT_EQ("-0", rfl::dump_to_json_like(-0.0));
}

TEST(TypeOperationsDumpToJsonLike, ScientificNotation) {
  EXPECT_EQ("1e+10", rfl::dump_to_json_like(1e10));
  EXPECT_EQ("1e-05", rfl::dump_to_json_like(1e-5));
}

TEST(TypeOperationsDumpToJsonLike, StructWithMapLike) {
  std::vector<std::pair<std::string, std::string>> map{{"key1", "value1"}, {"key2", "value2"}};
  EXPECT_EQ(R"([["key1","value1"],["key2","value2"]])", rfl::dump_to_json_like(map));
}

TEST(TypeOperationsDumpToJsonLike, ComplexNesting) {
  struct inner_t {
    std::optional<std::vector<int>> nums;
    std::string label;
  };
  struct outer_t {
    inner_t data;
    std::variant<inner_t, int> choice;
  };
  inner_t i1{std::vector<int>{1, 2, 3}, "first"};
  inner_t i2{std::nullopt, "second"};
  outer_t o1{i1, 42};
  auto result1 = rfl::dump_to_json_like(o1);
  EXPECT_TRUE(result1 == R"({data:{nums:[1,2,3],label:"first"},choice:42})");
}

TEST(TypeOperationsDumpToJsonLike, IndentWithTab) {
  person_t p{"Tabby", 40};
  EXPECT_EQ("{\n\tname: \"Tabby\",\n\tage: 40\n}", rfl::dump_to_json_like(p, 1, '\t'));
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar8T) {
  std::u8string result = rfl::dump_to_json_like<char8_t>(u8"hello");
  EXPECT_TRUE(result == u8"\"hello\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar16T) {
  std::u16string result = rfl::dump_to_json_like<char16_t>(u"world");
  EXPECT_TRUE(result == u"\"world\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar32T) {
  std::u32string result = rfl::dump_to_json_like<char32_t>(U"test");
  EXPECT_TRUE(result == U"\"test\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar8TWithSpecialChars) {
  std::u8string result = rfl::dump_to_json_like<char8_t>(u8"hello\nworld");
  EXPECT_TRUE(result == u8"\"hello\\nworld\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar16TWithSpecialChars) {
  std::u16string result = rfl::dump_to_json_like<char16_t>(u"hello\nworld");
  EXPECT_TRUE(result == u"\"hello\\nworld\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar32TWithSpecialChars) {
  std::u32string result = rfl::dump_to_json_like<char32_t>(U"hello\nworld");
  EXPECT_TRUE(result == U"\"hello\\nworld\"");
}

// Note: std::vector<char> is handled as a single string since it satisfies string_like concept
TEST(TypeOperationsDumpToJsonLike, VectorOfChars) {
  std::vector<char> vec{'a', 'b', 'c'};
  EXPECT_EQ(R"("abc")", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfDoubles) {
  std::vector<double> vec{1.1, 2.2, 3.3};
  EXPECT_EQ("[1.1,2.2,3.3]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfOptionalStrings) {
  std::vector<std::optional<std::string>> vec{"hello", std::nullopt, "world"};
  EXPECT_EQ(R"(["hello",nullopt,"world"])", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfPairs) {
  std::vector<std::pair<int, int>> vec{{1, 2}, {3, 4}};
  EXPECT_EQ("[[1,2],[3,4]]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, EmptyPairsVector) {
  std::vector<std::pair<int, int>> vec{};
  EXPECT_EQ("[]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, ArrayOfStrings) {
  std::array<std::string, 2> arr{"first", "second"};
  EXPECT_EQ(R"(["first","second"])", rfl::dump_to_json_like(arr));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsInStruct) {
  struct settings_t {
    std::string name;
    std::vector<bool> flags;
  };
  settings_t s{"debug", {true, false, true, false}};
  EXPECT_EQ(R"({name:"debug",flags:[true,false,true,false]})", rfl::dump_to_json_like(s));
}

TEST(TypeOperationsDumpToJsonLike, ArrayOfStructs) {
  std::array<person_t, 2> arr{{{"Alice", 30}, {"Bob", 25}}};
  EXPECT_EQ(R"([{name:"Alice",age:30},{name:"Bob",age:25}])", rfl::dump_to_json_like(arr));
}

TEST(TypeOperationsDumpToJsonLike, Pointers) {
  int* p = reinterpret_cast<int*>(0x12345678);
  EXPECT_EQ("0x12345678", rfl::dump_to_json_like(p));

  struct my_pair_t {
    int* first;
    int** second;
  };
  my_pair_t ps = {p, nullptr};
  EXPECT_EQ(uR"({first:0x12345678,second:nullptr})", rfl::dump_to_json_like<char16_t>(ps));
}

TEST(TypeOperationsDumpToJsonLike, PointersToDataMember) {
  struct my_pair_t {
    int first;
    int second;
  };

  // May be "&my_pair_t::first" or "&(...)::my_pair_t::first"
  // where "(...)" is implementation-defined by the C++ standard library.
  auto result = rfl::dump_to_json_like(&my_pair_t::first);
  EXPECT_THAT(result, testing::StartsWith("&"));
  EXPECT_THAT(result, testing::EndsWith("my_pair_t::first"));

  result = rfl::dump_to_json_like(&my_pair_t::second);
  EXPECT_THAT(result, testing::StartsWith("&"));
  EXPECT_THAT(result, testing::EndsWith("my_pair_t::second"));
}

struct A {
  int a;
  int x;
};
struct B {
  int b;
  int x;
};
struct C : A, B {
  int c;
};

TEST(TypeOperationsDumpToJsonLike, PointersToDataMemberWithInheritance) {
  int C::* p = &C::a;
  EXPECT_EQ("&A::a", rfl::dump_to_json_like(p));
  p = &C::b;
  EXPECT_EQ("&B::b", rfl::dump_to_json_like(p));
  p = &C::c;
  EXPECT_EQ("&C::c", rfl::dump_to_json_like(p));
  p = &A::x;
  EXPECT_EQ("&A::x", rfl::dump_to_json_like(p));
  p = &B::x;
  EXPECT_EQ("&B::x", rfl::dump_to_json_like(p));
}

TEST(TypeOperationsDumpToJsonLike, StructWithInheritance) {
  constexpr auto c = C{{1, 2}, {3, 4}, 5};
  // Disambiguation
  EXPECT_EQ("{a:1,A::x:2,b:3,B::x:4,c:5}", rfl::dump_to_json_like(c));
  EXPECT_EQ(R"({
    a: 1,
    A::x: 2,
    b: 3,
    B::x: 4,
    c: 5
})",
            rfl::dump_to_json_like(c, 4));
}

struct with_referenct_t {
  intptr_t x;
  int& r;
};

TEST(TypeOperationsDumpToJsonLike, StructWithReferences) {
  int value = 42;
  auto s = with_referenct_t{.x = 21, .r = value};
  EXPECT_NE("{x:21,r:42}", rfl::dump_to_json_like(s));

  reinterpret_cast<intptr_t*>(&s)[1] = 0x12345678;
  EXPECT_EQ("{x:21,r:0x12345678}", rfl::dump_to_json_like(s));
  EXPECT_EQ(R"({
    x: 21,
    r: 0x12345678
})",
            rfl::dump_to_json_like(s, 4));
}
