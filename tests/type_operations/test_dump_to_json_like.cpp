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
  EXPECT_EQ_STATIC("42", rfl::dump_to_json_like(42));
  EXPECT_EQ_STATIC("-123", rfl::dump_to_json_like(-123));
  EXPECT_EQ_STATIC("0", rfl::dump_to_json_like(0));
}

TEST(TypeOperationsDumpToJsonLike, BasicFloats) {
  EXPECT_EQ("3.14", rfl::dump_to_json_like(3.14));
  EXPECT_EQ("-0.5", rfl::dump_to_json_like(-0.5f));
  EXPECT_EQ("0", rfl::dump_to_json_like(0.0));
}

TEST(TypeOperationsDumpToJsonLike, BasicBool) {
  EXPECT_EQ_STATIC("true", rfl::dump_to_json_like(true));
  EXPECT_EQ_STATIC("false", rfl::dump_to_json_like(false));
}

TEST(TypeOperationsDumpToJsonLike, BasicString) {
  EXPECT_EQ_STATIC("\"hello\"", rfl::dump_to_json_like(std::string{"hello"}));
  EXPECT_EQ_STATIC("\"world\"", rfl::dump_to_json_like(std::string_view{"world"}));
}

TEST(TypeOperationsDumpToJsonLike, CharToString) {
  EXPECT_EQ_STATIC("'a'", rfl::dump_to_json_like('a'));
}

TEST(TypeOperationsDumpToJsonLike, OptionalWithValue) {
  constexpr std::optional<int> opt{42};
  EXPECT_EQ_STATIC("42", rfl::dump_to_json_like(opt));
}

TEST(TypeOperationsDumpToJsonLike, OptionalEmpty) {
  constexpr std::optional<int> opt{std::nullopt};
  EXPECT_EQ_STATIC("nullopt", rfl::dump_to_json_like(opt));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfInts) {
  constexpr auto make_vec = [] constexpr { return std::vector<int>{1, 2, 3}; };
  EXPECT_EQ_STATIC("[1,2,3]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBools) {
  constexpr auto make_vec = [] constexpr { return std::vector<bool>{true, false, true}; };
  EXPECT_EQ_STATIC("[true,false,true]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsEmpty) {
  constexpr auto make_vec = [] constexpr { return std::vector<bool>{}; };
  EXPECT_EQ_STATIC("[]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsAllTrue) {
  constexpr auto make_vec = [] constexpr { return std::vector<bool>{true, true, true}; };
  EXPECT_EQ_STATIC("[true,true,true]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsAllFalse) {
  constexpr auto make_vec = [] constexpr { return std::vector<bool>{false, false, false}; };
  EXPECT_EQ_STATIC("[false,false,false]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, EmptyVector) {
  constexpr auto make_vec = [] constexpr { return std::vector<int>{}; };
  EXPECT_EQ_STATIC("[]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfStrings) {
  constexpr auto make_vec = [] constexpr { return std::vector<std::string>{"apple", "banana"}; };
  EXPECT_EQ_STATIC(R"(["apple","banana"])", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, Pair) {
  constexpr auto make_pair = [] constexpr { return std::pair<int, std::string>{42, "hello"}; };
  EXPECT_EQ_STATIC(R"([42,"hello"])", rfl::dump_to_json_like(make_pair()));
}

TEST(TypeOperationsDumpToJsonLike, Tuple) {
  constexpr auto make_tuple = [] constexpr {
    return std::make_tuple(1, std::string{"hello"}, 'a');
  };
  EXPECT_EQ_STATIC(R"([1,"hello",'a'])", rfl::dump_to_json_like(make_tuple()));
}

TEST(TypeOperationsDumpToJsonLike, EmptyTuple) {
  constexpr std::tuple<> t{};
  EXPECT_EQ_STATIC("[]", rfl::dump_to_json_like(t));
}

TEST(TypeOperationsDumpToJsonLike, Struct) {
  constexpr auto make_person = [] constexpr { return person_t{"Alice", 30}; };
  EXPECT_EQ_STATIC(R"({name:"Alice",age:30})", rfl::dump_to_json_like(make_person()));
}

TEST(TypeOperationsDumpToJsonLike, NestedStruct) {
  constexpr auto make_nested = [] constexpr { return nested_t{{"Bob", 25}, {90, 85, 92}}; };
  EXPECT_EQ_STATIC(R"({person:{name:"Bob",age:25},scores:[90,85,92]})",
                   rfl::dump_to_json_like(make_nested()));
}

TEST(TypeOperationsDumpToJsonLike, Enum) {
  EXPECT_EQ_STATIC("red", rfl::dump_to_json_like(color_t::red));
  EXPECT_EQ_STATIC("green", rfl::dump_to_json_like(color_t::green));
  EXPECT_EQ_STATIC("blue", rfl::dump_to_json_like(color_t::blue));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlag) {
  EXPECT_EQ_STATIC("read", rfl::dump_to_json_like(permissions_t::read));
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

TEST(TypeOperationsDumpToJsonLike, EnumFlagToStringInvalid) {
  EXPECT_EQ_STATIC("(permissions_t)8", rfl::dump_to_json_like(static_cast<permissions_t>(8)));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInStruct) {
  struct file_t {
    std::string name;
    permissions_t mode;
  };
  file_t f{"script.sh", permissions_t::read | permissions_t::execute};
  EXPECT_THAT(rfl::dump_to_json_like(f),
              testing::AnyOf(R"({name:"script.sh",mode:read|execute})",
                             R"({name:"script.sh",mode:execute|read})"));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInVector) {
  std::vector<permissions_t> vec{
      permissions_t::read, permissions_t::write, permissions_t::read | permissions_t::write};
  EXPECT_THAT(rfl::dump_to_json_like(vec),
              testing::AnyOf("[read,write,read|write]", "[read,write,write|read]"));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagEmptySet) {
  EXPECT_EQ_STATIC("", rfl::dump_to_json_like(static_cast<permissions_t>(0)));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagAllFlags) {
  auto all = permissions_t::read | permissions_t::write | permissions_t::execute;
  EXPECT_THAT(rfl::dump_to_json_like(all),
              testing::AnyOf("read|write|execute",
                             "read|execute|write",
                             "write|read|execute",
                             "write|execute|read",
                             "execute|read|write",
                             "execute|write|read"));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagMixedWithRegularEnum) {
  struct config_t {
    color_t color;
    permissions_t perms;
  };
  config_t c{color_t::blue, permissions_t::read | permissions_t::write};
  EXPECT_THAT(
      rfl::dump_to_json_like(c),
      testing::AnyOf(R"({color:blue,perms:read|write})", R"({color:blue,perms:write|read})"));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInOptional) {
  constexpr std::optional<permissions_t> present{permissions_t::read | permissions_t::execute};
  constexpr std::optional<permissions_t> empty{std::nullopt};
  EXPECT_THAT(rfl::dump_to_json_like(present), testing::AnyOf("read|execute", "execute|read"));
  EXPECT_EQ_STATIC("nullopt", rfl::dump_to_json_like(empty));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagInVariant) {
  constexpr std::variant<permissions_t, int> v1{permissions_t::write};
  constexpr std::variant<permissions_t, int> v2{42};
  EXPECT_EQ_STATIC("write", rfl::dump_to_json_like(v1));
  EXPECT_EQ_STATIC("42", rfl::dump_to_json_like(v2));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagWithChar16T) {
  auto result = rfl::dump_to_json_like<char16_t>(permissions_t::read | permissions_t::write);
  EXPECT_THAT(result, testing::AnyOf(u"read|write", u"write|read"));
}

TEST(TypeOperationsDumpToJsonLike, EnumFlagWithChar32T) {
  auto result = rfl::dump_to_json_like<char32_t>(permissions_t::read | permissions_t::write);
  EXPECT_THAT(result, testing::AnyOf(U"read|write", U"write|read"));
}

TEST(TypeOperationsDumpToJsonLike, Variant1) {
  constexpr auto make_v1 = [] constexpr { return std::variant<int, std::string>{42}; };
  constexpr auto make_v2 = [] constexpr { return std::variant<int, std::string>{"hello"}; };
  EXPECT_EQ_STATIC("42", rfl::dump_to_json_like(make_v1()));
  EXPECT_EQ_STATIC("\"hello\"", rfl::dump_to_json_like(make_v2()));
}

TEST(TypeOperationsDumpToJsonLike, Variant2) {
  struct S {
    int x;
  };
  constexpr std::variant<S, int> v{S{}};
  EXPECT_EQ_STATIC(R"({x:0})", rfl::dump_to_json_like(v));
}

TEST(TypeOperationsDumpToJsonLike, NestedOptional) {
  constexpr std::optional<std::optional<int>> nested{std::nullopt};
  EXPECT_EQ_STATIC("nullopt", rfl::dump_to_json_like(nested));
}

TEST(TypeOperationsDumpToJsonLike, OptionalInStruct) {
  struct with_optional_t {
    std::string name;
    std::optional<int> age;
  };
  constexpr auto make_w1 = [] constexpr { return with_optional_t{"Carol", 25}; };
  constexpr auto make_w2 = [] constexpr { return with_optional_t{"Dave", std::nullopt}; };
  EXPECT_EQ_STATIC(R"({name:"Carol",age:25})", rfl::dump_to_json_like(make_w1()));
  EXPECT_EQ_STATIC(R"({name:"Dave",age:nullopt})", rfl::dump_to_json_like(make_w2()));
}

TEST(TypeOperationsDumpToJsonLike, Array) {
  constexpr std::array<int, 3> arr{1, 2, 3};
  EXPECT_EQ_STATIC("[1,2,3]", rfl::dump_to_json_like(arr));
}

TEST(TypeOperationsDumpToJsonLike, WithIndent) {
  constexpr auto make_person = [] constexpr { return person_t{"Eve", 28}; };
  EXPECT_EQ_STATIC(R"({
  name: "Eve",
  age: 28
})",
                   rfl::dump_to_json_like(make_person(), 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, NestedWithIndent) {
  constexpr auto make_nested = [] constexpr { return nested_t{{"Frank", 35}, {100, 95}}; };
  EXPECT_EQ_STATIC(R"({
  person: {
    name: "Frank",
    age: 35
  },
  scores: [
    100,
    95
  ]
})",
                   rfl::dump_to_json_like(make_nested(), 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, VectorWithIndent) {
  constexpr auto make_vec = [] constexpr { return std::vector<int>{10, 20, 30}; };
  EXPECT_EQ_STATIC(R"([
  10,
  20,
  30
])",
                   rfl::dump_to_json_like(make_vec(), 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsWithIndent) {
  constexpr auto make_vec = [] constexpr { return std::vector<bool>{true, false, true}; };
  EXPECT_EQ_STATIC(R"([
  true,
  false,
  true
])",
                   rfl::dump_to_json_like(make_vec(), 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, Monostate) {
  constexpr std::monostate m{};
  EXPECT_EQ_STATIC("monostate", rfl::dump_to_json_like(m));
}

TEST(TypeOperationsDumpToJsonLike, OptionalInVariant) {
  constexpr auto make_v = [] constexpr {
    return std::variant<std::optional<int>, std::string>{std::nullopt};
  };
  EXPECT_EQ_STATIC("nullopt", rfl::dump_to_json_like(make_v()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfOptionals) {
  constexpr auto make_vec = [] constexpr {
    return std::vector<std::optional<int>>{1, std::nullopt, 3};
  };
  EXPECT_EQ_STATIC(R"([1,nullopt,3])", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, StringWithSpecialChars) {
  constexpr auto make_s = [] constexpr { return std::string{"hello\nworld\ttab"}; };
  EXPECT_EQ_STATIC(R"("hello\nworld\ttab")", rfl::dump_to_json_like(make_s()));
}

TEST(TypeOperationsDumpToJsonLike, StringWithMixedContent) {
  constexpr auto make_s = [] constexpr { return std::string{"Hello \"World\"\nLine2\tTabbed"}; };
  EXPECT_EQ_STATIC(R"("Hello \"World\"\nLine2\tTabbed")", rfl::dump_to_json_like(make_s()));
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
  constexpr auto make_v1 = [] constexpr { return std::variant<int, std::string, double>{42}; };
  constexpr auto make_v2 = [] constexpr { return std::variant<int, std::string, double>{"hello"}; };
  constexpr auto make_v3 = [] constexpr { return std::variant<int, std::string, double>{3.14}; };
  EXPECT_EQ_STATIC("42", rfl::dump_to_json_like(make_v1()));
  EXPECT_EQ_STATIC("\"hello\"", rfl::dump_to_json_like(make_v2()));
  EXPECT_EQ("3.14", rfl::dump_to_json_like(make_v3()));
}

TEST(TypeOperationsDumpToJsonLike, NestedStructInVector) {
  constexpr auto make_vec = [] constexpr {
    return std::vector<person_t>{{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};
  };
  EXPECT_EQ_STATIC(R"([{name:"Alice",age:30},{name:"Bob",age:25},{name:"Charlie",age:35}])",
                   rfl::dump_to_json_like(make_vec()));
}

namespace test_nested_struct_in_struct {
struct company_t {
  std::string name;
  person_t ceo;
  int founded_year;
};
}  // namespace test_nested_struct_in_struct

TEST(TypeOperationsDumpToJsonLike, NestedStructInStruct) {
  constexpr auto make_company = [] constexpr {
    return test_nested_struct_in_struct::company_t{"Acme Corp", {"John", 50}, 2020};
  };
  EXPECT_EQ_STATIC(R"({name:"Acme Corp",ceo:{name:"John",age:50},founded_year:2020})",
                   rfl::dump_to_json_like(make_company()));
}

namespace test_triple_nested_struct {
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
}  // namespace test_triple_nested_struct

TEST(TypeOperationsDumpToJsonLike, TripleNestedStruct) {
  constexpr auto make_company = [] constexpr {
    return test_triple_nested_struct::company_t{"TechCorp", {"Alice", {"123 Main St", "New York"}}};
  };
  EXPECT_EQ_STATIC(
      R"({name:"TechCorp",ceo:{name:"Alice",address:{street:"123 Main St",city:"New York"}}})",
      rfl::dump_to_json_like(make_company()));
}

TEST(TypeOperationsDumpToJsonLike, NestedOptionalInVector) {
  constexpr auto make_people = [] constexpr {
    return std::vector<std::optional<person_t>>{
        person_t{"Alice", 30}, std::nullopt, person_t{"Bob", 25}};
  };
  EXPECT_EQ_STATIC(R"([{name:"Alice",age:30},nullopt,{name:"Bob",age:25}])",
                   rfl::dump_to_json_like(make_people()));
}

namespace test_optional_struct_with_value {
struct config_t {
  std::string name;
  std::optional<int> port;
};
}  // namespace test_optional_struct_with_value

TEST(TypeOperationsDumpToJsonLike, OptionalStructWithValue) {
  constexpr auto make_c1 = [] constexpr {
    return test_optional_struct_with_value::config_t{"server", 8080};
  };
  constexpr auto make_c2 = [] constexpr {
    return test_optional_struct_with_value::config_t{"client", std::nullopt};
  };
  EXPECT_EQ_STATIC(R"({name:"server",port:8080})", rfl::dump_to_json_like(make_c1()));
  EXPECT_EQ_STATIC(R"({name:"client",port:nullopt})", rfl::dump_to_json_like(make_c2()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfVariants) {
  constexpr auto make_vec = [] constexpr {
    return std::vector<std::variant<int, std::string>>{42, "hello", 100, "world"};
  };
  EXPECT_EQ_STATIC(R"([42,"hello",100,"world"])", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, MapLikeWithVector) {
  constexpr auto make_pairs = [] constexpr {
    return std::vector<std::pair<std::string, int>>{{"a", 1}, {"b", 2}, {"c", 3}};
  };
  EXPECT_EQ_STATIC(R"([["a",1],["b",2],["c",3]])", rfl::dump_to_json_like(make_pairs()));
}

TEST(TypeOperationsDumpToJsonLike, TupleWithNestedTypes) {
  constexpr auto make_tuple = [] constexpr {
    return std::make_tuple(
        42, std::string{"hello"}, std::vector<int>{1, 2, 3}, person_t{"Alice", 30});
  };
  EXPECT_EQ_STATIC(R"([42,"hello",[1,2,3],{name:"Alice",age:30}])",
                   rfl::dump_to_json_like(make_tuple()));
}

TEST(TypeOperationsDumpToJsonLike, DeepNestedTuples) {
  constexpr auto inner = std::make_tuple(1, 2);
  constexpr auto outer = std::make_tuple(inner, 3);
  EXPECT_EQ_STATIC(R"([[1,2],3])", rfl::dump_to_json_like(outer));
}

namespace test_struct_with_all_types {
struct mixed_t {
  int i;
  double d;
  bool b;
  std::string s;
  std::vector<int> arr;
  person_t p;
};
}  // namespace test_struct_with_all_types

TEST(TypeOperationsDumpToJsonLike, StructWithAllTypes) {
  constexpr auto make_mixed = [] constexpr {
    return test_struct_with_all_types::mixed_t{42, 3.14, true, "test", {1, 2, 3}, {"Alice", 30}};
  };
  EXPECT_EQ(R"({i:42,d:3.14,b:true,s:"test",arr:[1,2,3],)"
            R"(p:{name:"Alice",age:30}})",
            rfl::dump_to_json_like(make_mixed()));
}

TEST(TypeOperationsDumpToJsonLike, NestedWithIndentComplex) {
  constexpr auto make_people = [] constexpr {
    return std::vector<person_t>{{"Alice", 30}, {"Bob", 25}};
  };
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
            rfl::dump_to_json_like(make_people(), 2, ' '));
}

TEST(TypeOperationsDumpToJsonLike, EmptyStruct) {
  struct empty_t {};
  EXPECT_EQ_STATIC("{}", rfl::dump_to_json_like(empty_t{}));
}

TEST(TypeOperationsDumpToJsonLike, EmptyString) {
  EXPECT_EQ_STATIC("\"\"", rfl::dump_to_json_like(std::string{""}));
}

TEST(TypeOperationsDumpToJsonLike, LargeInteger) {
  EXPECT_EQ_STATIC("2147483647", rfl::dump_to_json_like(2147483647));
  EXPECT_EQ_STATIC("-2147483648", rfl::dump_to_json_like(-2147483648));
}

TEST(TypeOperationsDumpToJsonLike, NegativeZeroFloat) {
  EXPECT_EQ("-0", rfl::dump_to_json_like(-0.0));
}

TEST(TypeOperationsDumpToJsonLike, StructWithMapLike) {
  constexpr auto make_map = [] constexpr {
    return std::vector<std::pair<std::string, std::string>>{{"key1", "value1"}, {"key2", "value2"}};
  };
  EXPECT_EQ_STATIC(R"([["key1","value1"],["key2","value2"]])", rfl::dump_to_json_like(make_map()));
}

namespace test_complex_nesting {
struct inner_t {
  std::optional<std::vector<int>> nums;
  std::string label;
};
struct outer_t {
  inner_t data;
  std::variant<inner_t, int> choice;
};
}  // namespace test_complex_nesting

TEST(TypeOperationsDumpToJsonLike, ComplexNesting) {
  constexpr auto make_outer = [] constexpr {
    return test_complex_nesting::outer_t{
        test_complex_nesting::inner_t{std::vector<int>{1, 2, 3}, "first"}, 42};
  };
  EXPECT_TRUE(rfl::dump_to_json_like<char8_t>(make_outer())
              == u8R"({data:{nums:[1,2,3],label:"first"},choice:42})");
}

TEST(TypeOperationsDumpToJsonLike, IndentWithTab) {
  constexpr auto make_person = [] constexpr { return person_t{"Tabby", 40}; };
  EXPECT_EQ_STATIC("{\n\tname: \"Tabby\",\n\tage: 40\n}",
                   rfl::dump_to_json_like(make_person(), 1, '\t'));
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar8T) {
  constexpr auto make_hello = [] constexpr { return u8"hello"; };
  EXPECT_TRUE_STATIC(rfl::dump_to_json_like<char8_t>(make_hello()) == u8"\"hello\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar16T) {
  constexpr auto make_world = [] constexpr { return u"world"; };
  EXPECT_TRUE_STATIC(rfl::dump_to_json_like<char16_t>(make_world()) == u"\"world\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar32T) {
  constexpr auto make_test = [] constexpr { return U"test"; };
  EXPECT_TRUE_STATIC(rfl::dump_to_json_like<char32_t>(make_test()) == U"\"test\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar8TWithSpecialChars) {
  constexpr auto make_hello_world = [] constexpr { return u8"hello\nworld"; };
  EXPECT_TRUE_STATIC(rfl::dump_to_json_like<char8_t>(make_hello_world()) == u8"\"hello\\nworld\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar16TWithSpecialChars) {
  constexpr auto make_hello_world = [] constexpr { return u"hello\nworld"; };
  EXPECT_TRUE_STATIC(rfl::dump_to_json_like<char16_t>(make_hello_world()) == u"\"hello\\nworld\"");
}

TEST(TypeOperationsDumpToJsonLike, CharTypesChar32TWithSpecialChars) {
  constexpr auto make_hello_world = [] constexpr { return U"hello\nworld"; };
  EXPECT_TRUE_STATIC(rfl::dump_to_json_like<char32_t>(make_hello_world()) == U"\"hello\\nworld\"");
}

// Note: std::vector<char> is handled as a single string since it satisfies string_like concept
TEST(TypeOperationsDumpToJsonLike, VectorOfChars) {
  constexpr auto make_vec = [] constexpr { return std::vector<char>{'a', 'b', 'c'}; };
  EXPECT_EQ_STATIC(R"("abc")", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfDoubles) {
  std::vector<double> vec{1.1, 2.2, 3.3};
  EXPECT_EQ("[1.1,2.2,3.3]", rfl::dump_to_json_like(vec));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfOptionalStrings) {
  constexpr auto make_vec = [] constexpr {
    return std::vector<std::optional<std::string>>{"hello", std::nullopt, "world"};
  };
  EXPECT_EQ_STATIC(R"(["hello",nullopt,"world"])", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, VectorOfPairs) {
  constexpr auto make_vec = [] constexpr {
    return std::vector<std::pair<int, int>>{{1, 2}, {3, 4}};
  };
  EXPECT_EQ_STATIC("[[1,2],[3,4]]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, EmptyPairsVector) {
  constexpr auto make_vec = [] constexpr { return std::vector<std::pair<int, int>>{}; };
  EXPECT_EQ_STATIC("[]", rfl::dump_to_json_like(make_vec()));
}

TEST(TypeOperationsDumpToJsonLike, ArrayOfStrings) {
  constexpr auto make_arr = [] constexpr { return std::array<std::string, 2>{"first", "second"}; };
  EXPECT_EQ_STATIC(R"(["first","second"])", rfl::dump_to_json_like(make_arr()));
}

namespace test_vector_of_bools_in_struct {
struct settings_t {
  std::string name;
  std::vector<bool> flags;
};
}  // namespace test_vector_of_bools_in_struct

TEST(TypeOperationsDumpToJsonLike, VectorOfBoolsInStruct) {
  constexpr auto make_settings = [] constexpr {
    return test_vector_of_bools_in_struct::settings_t{"debug", {true, false, true, false}};
  };
  EXPECT_EQ_STATIC(R"({name:"debug",flags:[true,false,true,false]})",
                   rfl::dump_to_json_like(make_settings()));
}

TEST(TypeOperationsDumpToJsonLike, ArrayOfStructs) {
  constexpr auto make_arr = [] constexpr {
    return std::array<person_t, 2>{{{"Alice", 30}, {"Bob", 25}}};
  };
  EXPECT_EQ_STATIC(R"([{name:"Alice",age:30},{name:"Bob",age:25}])",
                   rfl::dump_to_json_like(make_arr()));
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

namespace test_pointers_to_data_member {
struct my_pair_t {
  int first;
  int second;
};
}  // namespace test_pointers_to_data_member

TEST(TypeOperationsDumpToJsonLike, PointersToDataMember) {
  using test_pointers_to_data_member::my_pair_t;

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
