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
#include <reflect_cpp26/type_operations/serialize_to_json.hpp>
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

TEST(TypeOperationsSerializeToJson, BasicIntegers) {
  EXPECT_EQ("42", rfl::serialize_to_json(42));
  EXPECT_EQ("-123", rfl::serialize_to_json(-123));
  EXPECT_EQ("0", rfl::serialize_to_json(0));
  EXPECT_EQ("18446744073709551615", rfl::serialize_to_json(UINT64_MAX));
}

TEST(TypeOperationsSerializeToJson, BasicFloats) {
  EXPECT_EQ("3.14", rfl::serialize_to_json(3.14));
  EXPECT_EQ("-0.5", rfl::serialize_to_json(-0.5f));
  EXPECT_EQ("0", rfl::serialize_to_json(0.0));
}

TEST(TypeOperationsSerializeToJson, BasicBool) {
  EXPECT_EQ("true", rfl::serialize_to_json(true));
  EXPECT_EQ("false", rfl::serialize_to_json(false));
}

TEST(TypeOperationsSerializeToJson, BasicString) {
  EXPECT_EQ("\"hello\"", rfl::serialize_to_json(std::string{"hello"}));
  EXPECT_EQ("\"world\"", rfl::serialize_to_json(std::string_view{"world"}));
}

TEST(TypeOperationsSerializeToJson, CharToString) {
  EXPECT_EQ(R"("a")",
            (rfl::serialize_to_json<char, rfl::serialize_options{.char_to_string = true}>('a')));
  EXPECT_EQ("97",
            (rfl::serialize_to_json<char, rfl::serialize_options{.char_to_string = false}>('a')));
}

TEST(TypeOperationsSerializeToJson, OptionalWithValue) {
  std::optional<int> opt{42};
  EXPECT_EQ("42", rfl::serialize_to_json(opt));
}

TEST(TypeOperationsSerializeToJson, OptionalEmpty) {
  std::optional<int> opt{std::nullopt};
  EXPECT_EQ("null", rfl::serialize_to_json(opt));
}

TEST(TypeOperationsSerializeToJson, VectorOfInts) {
  std::vector<int> vec{1, 2, 3};
  EXPECT_EQ("[1,2,3]", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, VectorOfBools) {
  std::vector<bool> vec{true, false, true};
  EXPECT_EQ("[true,false,true]", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsEmpty) {
  std::vector<bool> vec{};
  EXPECT_EQ("[]", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsAllTrue) {
  std::vector<bool> vec{true, true, true};
  EXPECT_EQ("[true,true,true]", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsAllFalse) {
  std::vector<bool> vec{false, false, false};
  EXPECT_EQ("[false,false,false]", rfl::serialize_to_json(vec));
}

// Note: std::vector<char> is handled as a single string since it satisfies string_like concept
TEST(TypeOperationsSerializeToJson, VectorOfChars) {
  std::vector<char> vec{'a', 'b', 'c'};
  EXPECT_EQ(uR"("abc")", rfl::serialize_to_json<char16_t>(vec));
}

TEST(TypeOperationsSerializeToJson, EmptyVector) {
  std::vector<int> vec{};
  EXPECT_EQ("[]", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, VectorOfStrings) {
  std::vector<std::string> vec{"apple", "banana"};
  EXPECT_EQ(R"(["apple","banana"])", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, Pair) {
  std::pair<int, std::string> p{42, "hello"};
  EXPECT_EQ(R"([42,"hello"])", rfl::serialize_to_json(p));
}

TEST(TypeOperationsSerializeToJson, Tuple) {
  auto t = std::make_tuple(1, "hello", 3.14);
  EXPECT_EQ(R"([1,"hello",3.14])", rfl::serialize_to_json(t));
}

TEST(TypeOperationsSerializeToJson, EmptyTuple) {
  std::tuple<> t{};
  EXPECT_EQ("[]", rfl::serialize_to_json(t));
}

TEST(TypeOperationsSerializeToJson, Struct) {
  person_t p{"Alice", 30};
  EXPECT_EQ(R"({"name":"Alice","age":30})", rfl::serialize_to_json(p));
}

TEST(TypeOperationsSerializeToJson, NestedStruct) {
  nested_t n{{"Bob", 25}, {90, 85, 92}};
  EXPECT_EQ(R"({"person":{"name":"Bob","age":25},"scores":[90,85,92]})", rfl::serialize_to_json(n));
}

TEST(TypeOperationsSerializeToJson, EnumToInt) {
  EXPECT_EQ("0", rfl::serialize_to_json(color_t::red));
  EXPECT_EQ("1", rfl::serialize_to_json(color_t::green));
  EXPECT_EQ("2", rfl::serialize_to_json(color_t::blue));
}

TEST(TypeOperationsSerializeToJson, EnumToString) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  EXPECT_EQ(R"("red")", (rfl::serialize_to_json<char, opts>(color_t::red)));
}

TEST(TypeOperationsSerializeToJson, EnumFlagAsInteger) {
  EXPECT_EQ("1", rfl::serialize_to_json(permissions_t::read));
  EXPECT_EQ("3", rfl::serialize_to_json(permissions_t::read | permissions_t::write));
  EXPECT_EQ(
      "7",
      rfl::serialize_to_json(permissions_t::read | permissions_t::write | permissions_t::execute));
}

TEST(TypeOperationsSerializeToJson, EnumFlagToString) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  EXPECT_EQ(R"("read")", (rfl::serialize_to_json<char, opts>(permissions_t::read)));
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(permissions_t::read | permissions_t::write)),
              testing::AnyOf(R"("read|write")", R"("write|read")"));
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(permissions_t::read | permissions_t::write
                                                  | permissions_t::execute)),
              testing::AnyOf(R"("read|write|execute")",
                             R"("read|execute|write")",
                             R"("write|read|execute")",
                             R"("write|execute|read")",
                             R"("execute|read|write")",
                             R"("execute|write|read")"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagToStringInvalidHalts) {
  constexpr rfl::serialize_options opts{.enum_to_string = true, .halts_on_invalid_enum = true};
  auto result = rfl::serialize_to_json<char, opts>(static_cast<permissions_t>(8));
  EXPECT_EQ(std::nullopt, result);
}

TEST(TypeOperationsSerializeToJson, EnumFlagToStringInvalidNull) {
  constexpr rfl::serialize_options opts{.enum_to_string = true, .halts_on_invalid_enum = false};
  EXPECT_EQ("null", (rfl::serialize_to_json<char, opts>(static_cast<permissions_t>(8))));
}

TEST(TypeOperationsSerializeToJson, EnumFlagInStruct) {
  struct file_t {
    std::string name;
    permissions_t mode;
  };
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  file_t f{"script.sh", permissions_t::read | permissions_t::execute};
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(f)),
              testing::AnyOf(R"({"name":"script.sh","mode":"read|execute"})",
                             R"({"name":"script.sh","mode":"execute|read"})"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagInVector) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  std::vector<permissions_t> vec{
      permissions_t::read, permissions_t::write, permissions_t::read | permissions_t::write};
  EXPECT_THAT(
      (rfl::serialize_to_json<char, opts>(vec)),
      testing::AnyOf(R"(["read","write","read|write"])", R"(["read","write","write|read"])"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagEmptySet) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  auto result = rfl::serialize_to_json<char, opts>(static_cast<permissions_t>(0));
  EXPECT_EQ(R"("")", result);
}

TEST(TypeOperationsSerializeToJson, EnumFlagAllFlags) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  auto all = permissions_t::read | permissions_t::write | permissions_t::execute;
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(all)),
              testing::AnyOf(R"("read|write|execute")",
                             R"("read|execute|write")",
                             R"("write|read|execute")",
                             R"("write|execute|read")",
                             R"("execute|read|write")",
                             R"("execute|write|read")"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagMixedWithRegularEnum) {
  struct config_t {
    color_t color;
    permissions_t perms;
  };
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  config_t c{color_t::blue, permissions_t::read | permissions_t::write};
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(c)),
              testing::AnyOf(R"({"color":"blue","perms":"read|write"})",
                             R"({"color":"blue","perms":"write|read"})"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagInOptional) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  std::optional<permissions_t> present{permissions_t::read | permissions_t::execute};
  std::optional<permissions_t> empty{std::nullopt};
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(present)),
              testing::AnyOf(R"("read|execute")", R"("execute|read")"));
  EXPECT_EQ("null", (rfl::serialize_to_json<char, opts>(empty)));
}

TEST(TypeOperationsSerializeToJson, EnumFlagInVariant) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  std::variant<permissions_t, int> v1{permissions_t::write};
  std::variant<permissions_t, int> v2{42};
  EXPECT_EQ(R"("write")", (rfl::serialize_to_json<char, opts>(v1)));
  EXPECT_EQ("42", (rfl::serialize_to_json<char, opts>(v2)));
}

TEST(TypeOperationsSerializeToJson, EnumFlagWithChar16T) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  EXPECT_THAT((rfl::serialize_to_json<char16_t, opts>(permissions_t::read | permissions_t::write)),
              testing::AnyOf(u"\"read|write\"", u"\"write|read\""));
}

TEST(TypeOperationsSerializeToJson, EnumFlagWithChar32T) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  EXPECT_THAT((rfl::serialize_to_json<char32_t, opts>(permissions_t::read | permissions_t::write)),
              testing::AnyOf(U"\"read|write\"", U"\"write|read\""));
}

TEST(TypeOperationsSerializeToJson, Variant1) {
  std::variant<int, std::string> v1{42};
  std::variant<int, std::string> v2{"hello"};
  EXPECT_EQ("42", rfl::serialize_to_json(v1));
  EXPECT_EQ("\"hello\"", rfl::serialize_to_json(v2));
}

TEST(TypeOperationsSerializeToJson, Variant2) {
  struct S {
    int x;
  };
  std::variant<S, int> v{S{}};
  EXPECT_EQ(R"({"x":0})", rfl::serialize_to_json(v));
}

TEST(TypeOperationsSerializeToJson, NestedOptional) {
  std::optional<std::optional<int>> nested{std::nullopt};
  EXPECT_EQ("null", rfl::serialize_to_json(nested));
}

TEST(TypeOperationsSerializeToJson, OptionalInStruct) {
  struct with_optional_t {
    std::string name;
    std::optional<int> age;
  };
  with_optional_t w1{"Carol", 25};
  with_optional_t w2{"Dave", std::nullopt};
  EXPECT_EQ(R"({"name":"Carol","age":25})", rfl::serialize_to_json(w1));
  EXPECT_EQ(R"({"name":"Dave","age":null})", rfl::serialize_to_json(w2));
}

TEST(TypeOperationsSerializeToJson, Array) {
  std::array<int, 3> arr{1, 2, 3};
  EXPECT_EQ("[1,2,3]", rfl::serialize_to_json(arr));
}

TEST(TypeOperationsSerializeToJson, WithIndent) {
  person_t p{"Eve", 28};
  EXPECT_EQ(R"({
  "name": "Eve",
  "age": 28
})",
            rfl::serialize_to_json(p, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, NestedWithIndent) {
  nested_t n{{"Frank", 35}, {100, 95}};
  EXPECT_EQ(R"({
  "person": {
    "name": "Frank",
    "age": 35
  },
  "scores": [
    100,
    95
  ]
})",
            rfl::serialize_to_json(n, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, VectorWithIndent) {
  std::vector<int> vec{10, 20, 30};
  EXPECT_EQ(R"([
  10,
  20,
  30
])",
            rfl::serialize_to_json(vec, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsWithIndent) {
  std::vector<bool> vec{true, false, true};
  EXPECT_EQ(R"([
  true,
  false,
  true
])",
            rfl::serialize_to_json(vec, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, Monostate) {
  std::monostate m{};
  EXPECT_EQ("null", rfl::serialize_to_json(m));
}

TEST(TypeOperationsSerializeToJson, OptionalInVariant) {
  std::variant<std::optional<int>, std::string> v{std::nullopt};
  EXPECT_EQ("null", rfl::serialize_to_json(v));
}

TEST(TypeOperationsSerializeToJson, VectorOfOptionals) {
  std::vector<std::optional<int>> vec{1, std::nullopt, 3};
  EXPECT_EQ(R"([1,null,3])", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, StringWithSpecialChars) {
  std::string s{"hello\nworld\ttab"};
  EXPECT_EQ(R"("hello\nworld\ttab")", rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, StringWithMixedContent) {
  std::string s = "Hello \"World\"\nLine2\tTabbed";
  EXPECT_EQ(R"("Hello \"World\"\nLine2\tTabbed")", rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, VariantValuelessByException) {
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
    EXPECT_EQ("null", rfl::serialize_to_json(v));
  } else {
    EXPECT_EQ(R"({"value":99})", rfl::serialize_to_json(v));
  }
}

TEST(TypeOperationsSerializeToJson, VariantIndex) {
  std::variant<int, std::string, double> v1{42};
  std::variant<int, std::string, double> v2{"hello"};
  std::variant<int, std::string, double> v3{3.14};
  EXPECT_EQ("42", rfl::serialize_to_json(v1));
  EXPECT_EQ("\"hello\"", rfl::serialize_to_json(v2));
  EXPECT_EQ("3.14", rfl::serialize_to_json(v3));
}

TEST(TypeOperationsSerializeToJson, NestedStructInVector) {
  std::vector<person_t> people{{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};
  EXPECT_EQ(R"([{"name":"Alice","age":30},{"name":"Bob","age":25},{"name":"Charlie","age":35}])",
            rfl::serialize_to_json(people));
}

TEST(TypeOperationsSerializeToJson, NestedStructInStruct) {
  struct company_t {
    std::string name;
    person_t ceo;
    int founded_year;
  };
  company_t company{"Acme Corp", {"John", 50}, 2020};
  EXPECT_EQ(R"({"name":"Acme Corp","ceo":{"name":"John","age":50},"founded_year":2020})",
            rfl::serialize_to_json(company));
}

TEST(TypeOperationsSerializeToJson, TripleNestedStruct) {
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
  EXPECT_EQ(
      R"({"name":"TechCorp","ceo":{"name":"Alice","address":{"street":"123 Main St","city":"New York"}}})",
      rfl::serialize_to_json(company));
}

TEST(TypeOperationsSerializeToJson, NestedOptionalInVector) {
  std::vector<std::optional<person_t>> people{
      person_t{"Alice", 30}, std::nullopt, person_t{"Bob", 25}};
  EXPECT_EQ(R"([{"name":"Alice","age":30},null,{"name":"Bob","age":25}])",
            rfl::serialize_to_json(people));
}

TEST(TypeOperationsSerializeToJson, OptionalStructWithValue) {
  struct config_t {
    std::string name;
    std::optional<int> port;
  };
  config_t c1{"server", 8080};
  config_t c2{"client", std::nullopt};
  EXPECT_EQ(R"({"name":"server","port":8080})", rfl::serialize_to_json(c1));
  EXPECT_EQ(R"({"name":"client","port":null})", rfl::serialize_to_json(c2));
}

TEST(TypeOperationsSerializeToJson, VectorOfVariants) {
  std::vector<std::variant<int, std::string>> vec{42, "hello", 100, "world"};
  EXPECT_EQ(R"([42,"hello",100,"world"])", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsInStruct) {
  struct settings_t {
    std::string name;
    std::vector<bool> flags;
  };
  settings_t s{"debug", {true, false, true, false}};
  EXPECT_EQ(R"({"name":"debug","flags":[true,false,true,false]})", rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, MapLikeWithVector) {
  std::vector<std::pair<std::string, int>> pairs{{"a", 1}, {"b", 2}, {"c", 3}};
  EXPECT_EQ(R"([["a",1],["b",2],["c",3]])", rfl::serialize_to_json(pairs));
}

TEST(TypeOperationsSerializeToJson, TupleWithNestedTypes) {
  auto t =
      std::make_tuple(42, std::string{"hello"}, std::vector<int>{1, 2, 3}, person_t{"Alice", 30});
  EXPECT_EQ(R"([42,"hello",[1,2,3],{"name":"Alice","age":30}])", rfl::serialize_to_json(t));
}

TEST(TypeOperationsSerializeToJson, DeepNestedTuples) {
  auto inner = std::make_tuple(1, 2);
  auto outer = std::make_tuple(inner, 3);
  EXPECT_EQ(R"([[1,2],3])", rfl::serialize_to_json(outer));
}

TEST(TypeOperationsSerializeToJson, StructWithAllTypes) {
  struct mixed_t {
    int i;
    double d;
    bool b;
    std::string s;
    std::vector<int> arr;
    person_t p;
  };
  mixed_t m{42, 3.14, true, "test", {1, 2, 3}, {"Alice", 30}};
  EXPECT_EQ(R"({"i":42,"d":3.14,"b":true,"s":"test","arr":[1,2,3],)"
            R"("p":{"name":"Alice","age":30}})",
            rfl::serialize_to_json(m));
}

// The expected result shall be INDENTED by 2 spaces. Also, make the expected as raw string.
TEST(TypeOperationsSerializeToJson, NestedWithIndentComplex) {
  std::vector<person_t> people{{"Alice", 30}, {"Bob", 25}};
  EXPECT_EQ(R"([
  {
    "name": "Alice",
    "age": 30
  },
  {
    "name": "Bob",
    "age": 25
  }
])",
            rfl::serialize_to_json(people, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, EmptyOptional) {
  std::optional<std::string> opt{std::nullopt};
  EXPECT_EQ("null", rfl::serialize_to_json(opt));
}

TEST(TypeOperationsSerializeToJson, OptionalWithString) {
  std::optional<std::string> opt{"hello"};
  EXPECT_EQ("\"hello\"", rfl::serialize_to_json(opt));
}

TEST(TypeOperationsSerializeToJson, DoubleNestedOptional) {
  std::optional<std::optional<int>> opt1{42};
  std::optional<std::optional<int>> opt2{std::nullopt};
  std::optional<std::optional<int>> opt3{std::optional<int>{std::nullopt}};
  EXPECT_EQ("42", rfl::serialize_to_json(opt1));
  EXPECT_EQ("null", rfl::serialize_to_json(opt2));
  EXPECT_EQ("null", rfl::serialize_to_json(opt3));
}

TEST(TypeOperationsSerializeToJson, QuadrupleNestedStruct) {
  struct level1_t {
    int a;
  };
  struct level2_t {
    level1_t l1;
    std::string s;
  };
  struct level3_t {
    level2_t l2;
    std::vector<int> v;
  };
  struct level4_t {
    level3_t l3;
    bool b;
  };
  level4_t obj{{{{{1}}, "test"}, {1, 2, 3}}, true};
  EXPECT_EQ(R"({"l3":{"l2":{"l1":{"a":1},"s":"test"},"v":[1,2,3]},"b":true})",
            rfl::serialize_to_json(obj));
}

TEST(TypeOperationsSerializeToJson, VectorOfPairs) {
  std::vector<std::pair<std::string, int>> vec{{"key1", 100}, {"key2", 200}};
  EXPECT_EQ(R"([["key1",100],["key2",200]])", rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, TupleOfStructs) {
  auto t = std::make_tuple(person_t{"Alice", 30}, person_t{"Bob", 25});
  EXPECT_EQ(R"([{"name":"Alice","age":30},{"name":"Bob","age":25}])", rfl::serialize_to_json(t));
}

TEST(TypeOperationsSerializeToJson, StructWithVariant) {
  struct container_t {
    std::variant<int, std::string> v1;
    std::variant<double, bool> v2;
  };
  container_t c{std::variant<int, std::string>{42}, std::variant<double, bool>{true}};
  EXPECT_EQ(R"({"v1":42,"v2":true})", rfl::serialize_to_json(c));
}

TEST(TypeOperationsSerializeToJson, StructWithVariantIndent) {
  struct container_t {
    std::variant<int, std::string> v1;
    std::variant<double, bool> v2;
  };
  container_t c{std::variant<int, std::string>{"hello"}, std::variant<double, bool>{3.14}};
  EXPECT_EQ(R"({
  "v1": "hello",
  "v2": 3.14
})",
            rfl::serialize_to_json(c, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, ComplexNestedWithIndent) {
  struct project_t {
    std::string name;
    std::vector<person_t> team;
    std::vector<std::pair<std::string, int>> scores;
  };
  project_t p{"Alpha", {{"Alice", 30}, {"Bob", 25}}, {{"a", 1}, {"b", 2}}};
  EXPECT_EQ(R"({
  "name": "Alpha",
  "team": [
    {
      "name": "Alice",
      "age": 30
    },
    {
      "name": "Bob",
      "age": 25
    }
  ],
  "scores": [
    [
      "a",
      1
    ],
    [
      "b",
      2
    ]
  ]
})",
            rfl::serialize_to_json(p, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, VectorOfNestedStructsIndent) {
  struct item_t {
    std::string name;
    int value;
  };
  std::vector<item_t> items{{"item1", 100}, {"item2", 200}};
  EXPECT_EQ(R"([
  {
    "name": "item1",
    "value": 100
  },
  {
    "name": "item2",
    "value": 200
  }
])",
            rfl::serialize_to_json(items, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, DeepNestingWithIndent) {
  auto t = std::make_tuple(std::make_tuple(std::make_tuple(1, 2), std::vector<int>{3, 4}),
                           std::make_tuple(std::make_tuple(5, 6), std::vector<int>{7, 8}));
  EXPECT_EQ(R"([
  [
    [
      1,
      2
    ],
    [
      3,
      4
    ]
  ],
  [
    [
      5,
      6
    ],
    [
      7,
      8
    ]
  ]
])",
            rfl::serialize_to_json(t, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, UTF8String) {
  std::u8string s = u8"你好";
  constexpr auto expected = std::string_view{R"("你好")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF16String) {
  std::u16string s = u"你好";
  constexpr auto expected = std::string_view{R"("你好")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF32String) {
  std::u32string s = U"你好";
  constexpr auto expected = std::string_view{R"("你好")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8StringToUtf16) {
  std::u8string s = u8"你好";
  constexpr auto expected = std::u16string_view{uR"("你好")"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char16_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF8StringToUtf32) {
  std::u8string s = u8"你好";
  constexpr auto expected = std::u32string_view{U"\"你好\""};
  EXPECT_EQ(expected, rfl::serialize_to_json<char32_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF16StringToUtf8) {
  std::u16string s = u"你好";
  constexpr auto expected = std::u8string_view{u8R"("你好")"};
  EXPECT_TRUE(expected == rfl::serialize_to_json<char8_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF16StringToUtf16) {
  std::u16string s = u"你好";
  constexpr auto expected = std::u16string_view{uR"("你好")"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char16_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF16StringToUtf32) {
  std::u16string s = u"你好";
  constexpr auto expected = std::u32string_view{U"\"你好\""};
  EXPECT_EQ(expected, rfl::serialize_to_json<char32_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF32StringToUtf8) {
  std::u32string s = U"你好";
  constexpr auto expected = std::u8string_view{u8R"("你好")"};
  EXPECT_TRUE(expected == rfl::serialize_to_json<char8_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF32StringToUtf16) {
  std::u32string s = U"你好";
  constexpr auto expected = std::u16string_view{uR"("你好")"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char16_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF32StringToUtf32) {
  std::u32string s = U"你好";
  constexpr auto expected = std::u32string_view{U"\"你好\""};
  EXPECT_EQ(expected, rfl::serialize_to_json<char32_t>(s));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorOfStrings) {
  std::vector<std::u8string> vec{u8"你好", u8"世界"};
  constexpr auto expected = std::string_view{R"(["你好","世界"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStrings) {
  std::vector<std::u16string> vec{u"你好", u"世界"};
  constexpr auto expected = std::string_view{R"(["你好","世界"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStrings) {
  std::vector<std::u32string> vec{U"你好", U"世界"};
  constexpr auto expected = std::string_view{R"(["你好","世界"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorOfStringsToUtf16) {
  std::vector<std::u8string> vec{u8"你好", u8"世界"};
  constexpr auto expected = std::u16string_view{uR"(["你好","世界"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char16_t>(vec));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorOfStringsToUtf32) {
  std::vector<std::u8string> vec{u8"你好", u8"世界"};
  constexpr auto expected = std::u32string_view{U"[\"你好\",\"世界\"]"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char32_t>(vec));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStringsToUtf8) {
  std::vector<std::u16string> vec{u"你好", u"世界"};
  constexpr auto expected = std::u8string_view{u8R"(["你好","世界"])"};
  EXPECT_TRUE(expected == rfl::serialize_to_json<char8_t>(vec));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStringsToUtf16) {
  std::vector<std::u16string> vec{u"你好", u"世界"};
  constexpr auto expected = std::u16string_view{uR"(["你好","世界"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char16_t>(vec));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStringsToUtf32) {
  std::vector<std::u16string> vec{u"你好", u"世界"};
  constexpr auto expected = std::u32string_view{U"[\"你好\",\"世界\"]"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char32_t>(vec));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStringsToUtf8) {
  std::vector<std::u32string> vec{U"你好", U"世界"};
  constexpr auto expected = std::u8string_view{u8R"(["你好","世界"])"};
  EXPECT_TRUE(expected == rfl::serialize_to_json<char8_t>(vec));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStringsToUtf16) {
  std::vector<std::u32string> vec{U"你好", U"世界"};
  constexpr auto expected = std::u16string_view{uR"(["你好","世界"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char16_t>(vec));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStringsToUtf32) {
  std::vector<std::u32string> vec{U"你好", U"世界"};
  constexpr auto expected = std::u32string_view{U"[\"你好\",\"世界\"]"};
  EXPECT_EQ(expected, rfl::serialize_to_json<char32_t>(vec));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF8String) {
  struct localized_t {
    std::u8string chinese;
    std::u8string english;
  };
  localized_t loc{u8"中文", u8"English"};
  constexpr auto expected = std::string_view{R"({"chinese":"中文","english":"English"})"};
  EXPECT_EQ(expected, rfl::serialize_to_json(loc));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF16String) {
  struct localized_t {
    std::u16string chinese;
    std::u16string english;
  };
  localized_t loc{u"中文", u"English"};
  constexpr auto expected = std::string_view{R"({"chinese":"中文","english":"English"})"};
  EXPECT_EQ(expected, rfl::serialize_to_json(loc));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF32String) {
  struct localized_t {
    std::u32string chinese;
    std::u32string english;
  };
  localized_t loc{U"中文", U"English"};
  constexpr auto expected = std::string_view{R"({"chinese":"中文","english":"English"})"};
  EXPECT_EQ(expected, rfl::serialize_to_json(loc));
}

TEST(TypeOperationsSerializeToJson, NestedStructWithUTF8Indent) {
  struct item_t {
    std::u8string name;
    std::u8string description;
  };
  struct container_t {
    std::u8string title;
    std::vector<item_t> items;
  };
  container_t c{u8"标题", {{u8"项目1", u8"描述1"}, {u8"项目2", u8"描述2"}}};
  constexpr auto expected = std::string_view{
      R"({
  "title": "标题",
  "items": [
    {
      "name": "项目1",
      "description": "描述1"
    },
    {
      "name": "项目2",
      "description": "描述2"
    }
  ]
})"};
  EXPECT_EQ(expected, rfl::serialize_to_json(c, 2, ' '));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithNewline) {
  std::u8string s = u8"hello\nworld";
  constexpr auto expected = std::string_view{R"("hello\nworld")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithTab) {
  std::u8string s = u8"hello\tworld";
  constexpr auto expected = std::string_view{R"("hello\tworld")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithQuote) {
  std::u8string s = u8"他说:\"你好\"";
  constexpr auto expected = std::string_view{R"("他说:\"你好\"")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithBackslash) {
  std::u8string s = u8"路径:C:\\Users\\name";
  constexpr auto expected = std::string_view{R"("路径:C:\\Users\\name")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithNewline) {
  std::u16string s = u"hello\nworld";
  constexpr auto expected = std::string_view{R"("hello\nworld")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithQuote) {
  std::u16string s = u"他说:\"你好\"";
  constexpr auto expected = std::string_view{R"("他说:\"你好\"")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithNewline) {
  std::u32string s = U"hello\nworld";
  constexpr auto expected = std::string_view{R"("hello\nworld")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithQuote) {
  std::u32string s = U"他说:\"你好\"";
  constexpr auto expected = std::string_view{R"("他说:\"你好\"")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorWithEscapes) {
  std::vector<std::u8string> vec{u8"line1\nline2", u8"tab\there"};
  constexpr auto expected = std::string_view{R"(["line1\nline2","tab\there"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorWithEscapes) {
  std::vector<std::u16string> vec{u"换行\n", u"制表\t"};
  constexpr auto expected = std::string_view{R"(["换行\n","制表\t"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorWithEscapes) {
  std::vector<std::u32string> vec{U"引号\"", U"反斜\\"};
  constexpr auto expected = std::string_view{R"(["引号\"","反斜\\"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF8Escapes) {
  struct msg_t {
    std::u8string content;
    std::u8string path;
  };
  msg_t m{u8"内容:\n换行", u8"C:\\path"};
  constexpr auto expected = std::string_view{R"({"content":"内容:\n换行","path":"C:\\path"})"};
  EXPECT_EQ(expected, rfl::serialize_to_json(m));
}

// Escaping slashes: Not required by RFC 8259 standard, yet commonly applied in practice
// for historical and security reason.

TEST(TypeOperationsSerializeToJson, UTF8StringWithSlash) {
  std::u8string s = u8"https://example.com/path";
  constexpr auto expected = std::string_view{R"("https:\/\/example.com\/path")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithSlashAndChinese) {
  std::u8string s = u8"网址:https://中文.com";
  constexpr auto expected = std::string_view{R"("网址:https:\/\/中文.com")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithAllRFC8259Escapes) {
  std::u8string s = u8"引\"反\\斜/杠\n换\r行\t制表\f换页\b";
  constexpr auto expected = std::string_view{R"("引\"反\\斜\/杠\n换\r行\t制表\f换页\b")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithSlash) {
  std::u16string s = u"https://example.com/path";
  constexpr auto expected = std::string_view{R"("https:\/\/example.com\/path")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithSlashAndChinese) {
  std::u16string s = u"网址:https://中文.com";
  constexpr auto expected = std::string_view{R"("网址:https:\/\/中文.com")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithSlash) {
  std::u32string s = U"https://example.com/path";
  constexpr auto expected = std::string_view{R"("https:\/\/example.com\/path")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithSlashAndChinese) {
  std::u32string s = U"网址:https://中文.com";
  constexpr auto expected = std::string_view{R"("网址:https:\/\/中文.com")"};
  EXPECT_EQ(expected, rfl::serialize_to_json(s));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorWithSlash) {
  std::vector<std::u8string> vec{u8"http:/a.com", u8"https:/b.com"};
  constexpr auto expected = std::string_view{R"(["http:\/a.com","https:\/b.com"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorWithSlash) {
  std::vector<std::u16string> vec{u"网址/http", u"网址/https"};
  constexpr auto expected = std::string_view{R"(["网址\/http","网址\/https"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorWithSlash) {
  std::vector<std::u32string> vec{U"网址/http", U"网址/https"};
  constexpr auto expected = std::string_view{R"(["网址\/http","网址\/https"])"};
  EXPECT_EQ(expected, rfl::serialize_to_json(vec));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF8Slash) {
  struct url_t {
    std::u8string http;
    std::u8string https;
  };
  url_t u{u8"http://example.com", u8"https://example.com"};
  constexpr auto expected =
      std::string_view{R"({"http":"http:\/\/example.com","https":"https:\/\/example.com"})"};
  EXPECT_EQ(expected, rfl::serialize_to_json(u));
}

// Tests for serialize_to_json<char8_t> - returns std::u8string
// Note: Using EXPECT_TRUE(expected == actual) for u8string due to libgtest compatibility

TEST(TypeOperationsSerializeToJson, Char8TBasicString) {
  std::u8string result = rfl::serialize_to_json<char8_t>(std::u8string{u8"你好"});
  std::u8string expected = u8R"("你好")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithEscapes) {
  std::u8string result = rfl::serialize_to_json<char8_t>(std::u8string{u8"换行:\n制表:\t"});
  std::u8string expected = u8R"("换行:\n制表:\t")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithQuotes) {
  std::u8string result = rfl::serialize_to_json<char8_t>(std::u8string{u8"他说:\"你好\""});
  std::u8string expected = u8R"("他说:\"你好\"")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithBackslash) {
  std::u8string result = rfl::serialize_to_json<char8_t>(std::u8string{u8"路径:C:\\Users"});
  std::u8string expected = u8R"("路径:C:\\Users")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithAllEscapes) {
  std::u8string result =
      rfl::serialize_to_json<char8_t>(std::u8string{u8"换行\n制表\t双引\"反斜\\"});
  std::u8string expected = u8R"("换行\n制表\t双引\"反斜\\")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TVector) {
  std::vector<std::u16string> vec{u"你好", u"世界"};
  std::u8string result = rfl::serialize_to_json<char8_t>(vec);
  std::u8string expected = u8R"(["你好","世界"])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TInt) {
  std::u8string result = rfl::serialize_to_json<char8_t>(42);
  std::u8string expected = u8R"(42)";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TFloat) {
  std::u8string result = rfl::serialize_to_json<char8_t>(3.14);
  std::u8string expected = u8R"(3.14)";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TBool) {
  std::u8string result_true = rfl::serialize_to_json<char8_t>(true);
  std::u8string expected_true = u8R"(true)";
  EXPECT_TRUE(expected_true == result_true);

  std::u8string result_false = rfl::serialize_to_json<char8_t>(false);
  std::u8string expected_false = u8R"(false)";
  EXPECT_TRUE(expected_false == result_false);
}

TEST(TypeOperationsSerializeToJson, Char8TWithIndent) {
  struct item_t {
    std::u8string name;
    std::u8string desc;
  };
  std::vector<item_t> vec{{u8"项1", u8"描1"}, {u8"项2", u8"描2"}};
  std::u8string result = rfl::serialize_to_json<char8_t>(vec, 2, char8_t{' '});
  std::u8string expected =
      u8R"([
  {
    "name": "项1",
    "desc": "描1"
  },
  {
    "name": "项2",
    "desc": "描2"
  }
])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TStruct) {
  struct person_t {
    std::u32string name;
    std::u32string city;
  };
  person_t p{U"张三", U"北京"};
  std::u8string result = rfl::serialize_to_json<char8_t>(p);
  std::u8string expected = u8R"({"name":"张三","city":"北京"})";
  EXPECT_TRUE(expected == result);
}

// Tests for serialize_to_json<char16_t> - returns std::u16string

TEST(TypeOperationsSerializeToJson, Char16TBasicString) {
  std::u16string result = rfl::serialize_to_json<char16_t>(std::u16string{u"你好"});
  std::u16string expected = u"\"你好\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TStringWithEscapes) {
  std::u16string result = rfl::serialize_to_json<char16_t>(std::u16string{u"换行:\n制表:\t"});
  std::u16string expected = u"\"换行:\\n制表:\\t\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TStringWithQuotes) {
  std::u16string result = rfl::serialize_to_json<char16_t>(std::u16string{u"他说:\"你好\""});
  std::u16string expected = u"\"他说:\\\"你好\\\"\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TStringWithBackslash) {
  std::u16string result = rfl::serialize_to_json<char16_t>(std::u16string{u"路径:C:\\Users"});
  std::u16string expected = u"\"路径:C:\\\\Users\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TVector) {
  std::vector<std::u32string> vec{U"中文", U"测试"};
  std::u16string result = rfl::serialize_to_json<char16_t>(vec);
  std::u16string expected = u"[\"中文\",\"测试\"]";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TInt) {
  std::u16string result = rfl::serialize_to_json<char16_t>(123);
  std::u16string expected = u"123";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TStruct) {
  struct config_t {
    std::u16string title;
    std::u16string value;
  };
  config_t c{u"标题", u"值"};
  std::u16string result = rfl::serialize_to_json<char16_t>(c);
  std::u16string expected = u"{\"title\":\"标题\",\"value\":\"值\"}";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TWithIndent) {
  struct item_t {
    std::u16string name;
    std::u16string desc;
  };
  std::vector<item_t> vec{{u"项1", u"描1"}, {u"项2", u"描2"}};
  std::u16string result = rfl::serialize_to_json<char16_t>(vec, 2, char16_t{u' '});
  std::u16string expected =
      uR"([
  {
    "name": "项1",
    "desc": "描1"
  },
  {
    "name": "项2",
    "desc": "描2"
  }
])";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TMap) {
  std::map<std::u16string, std::u16string> m{{u"key1", u"值1"}, {u"key2", u"值2"}};
  std::u16string result = rfl::serialize_to_json<char16_t>(m);
  std::u16string expected = u"{\"key1\":\"值1\",\"key2\":\"值2\"}";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TMapIntKey) {
  std::map<int, std::u16string> m{{1, u"一"}, {2, u"二"}};
  std::u16string result = rfl::serialize_to_json<char16_t>(m);
  std::u16string expected = u"[[1,\"一\"],[2,\"二\"]]";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char16TMapNested) {
  std::map<std::u16string, std::map<std::u16string, int>> m{{u"a", {{u"x", 1}, {u"y", 2}}},
                                                            {u"b", {{u"z", 3}}}};
  std::u16string result = rfl::serialize_to_json<char16_t>(m);
  std::u16string expected = u"{\"a\":{\"x\":1,\"y\":2},\"b\":{\"z\":3}}";
  EXPECT_EQ(expected, result);
}

// Tests for serialize_to_json<char32_t> - returns std::u32string

TEST(TypeOperationsSerializeToJson, Char32TBasicString) {
  std::u32string result = rfl::serialize_to_json<char32_t>(std::u32string{U"你好"});
  std::u32string expected = U"\"你好\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TStringWithEscapes) {
  std::u32string result = rfl::serialize_to_json<char32_t>(std::u32string{U"换行:\n制表:\t"});
  std::u32string expected = U"\"换行:\\n制表:\\t\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TStringWithQuotes) {
  std::u32string result = rfl::serialize_to_json<char32_t>(std::u32string{U"他说:\"你好\""});
  std::u32string expected = U"\"他说:\\\"你好\\\"\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TStringWithBackslash) {
  std::u32string result = rfl::serialize_to_json<char32_t>(std::u32string{U"路径:C:\\Users"});
  std::u32string expected = U"\"路径:C:\\\\Users\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TVector) {
  std::vector<std::u8string> vec{u8"你好", u8"世界"};
  std::u32string result = rfl::serialize_to_json<char32_t>(vec);
  std::u32string expected = U"[\"你好\",\"世界\"]";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TInt) {
  std::u32string result = rfl::serialize_to_json<char32_t>(-999);
  std::u32string expected = U"-999";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TStruct) {
  struct record_t {
    std::u32string label;
    std::u32string content;
  };
  record_t r{U"标签", U"内容"};
  std::u32string result = rfl::serialize_to_json<char32_t>(r);
  std::u32string expected = U"{\"label\":\"标签\",\"content\":\"内容\"}";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TWithIndent) {
  struct item_t {
    std::u32string name;
    std::u32string desc;
  };
  std::vector<item_t> vec{{U"项1", U"描1"}, {U"项2", U"描2"}};
  std::u32string result = rfl::serialize_to_json<char32_t>(vec, 2, char32_t{U' '});
  std::u32string expected =
      UR"([
  {
    "name": "项1",
    "desc": "描1"
  },
  {
    "name": "项2",
    "desc": "描2"
  }
])";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TMap) {
  std::map<std::u8string, std::u16string> m{{u8"key1", u"值1"}, {u8"key2", u"值2"}};
  std::u32string result = rfl::serialize_to_json<char32_t>(m);
  std::u32string expected = U"{\"key1\":\"值1\",\"key2\":\"值2\"}";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TMapIntKey) {
  std::map<int, std::u16string> m{{1, u"一"}, {2, u"二"}};
  std::u32string result = rfl::serialize_to_json<char32_t>(m);
  std::u32string expected = U"[[1,\"一\"],[2,\"二\"]]";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TMapNested) {
  std::map<std::u32string, std::map<std::u32string, int>> m{{U"a", {{U"x", 1}, {U"y", 2}}},
                                                            {U"b", {{U"z", 3}}}};
  std::u32string result = rfl::serialize_to_json<char32_t>(m);
  std::u32string expected = U"{\"a\":{\"x\":1,\"y\":2},\"b\":{\"z\":3}}";
  EXPECT_EQ(expected, result);
}

// Tests for std::set - serialized as array (because key is not string-like)
TEST(TypeOperationsSerializeToJson, Char8TSetInt) {
  std::set<int> s{3, 1, 4, 1, 5};
  std::u8string result = rfl::serialize_to_json<char8_t>(s);
  std::u8string expected = u8R"([1,3,4,5])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TSetString) {
  std::set<std::u8string> s{u8"苹果", u8"香蕉", u8"樱桃"};
  std::string result = rfl::serialize_to_json(s);
  std::string expected = R"(["樱桃","苹果","香蕉"])";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char8TSetEmpty) {
  std::set<int> s{};
  std::u8string result = rfl::serialize_to_json<char8_t>(s);
  std::u8string expected = u8R"([])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char16TSetInt) {
  std::set<int> s{1, 2, 3};
  std::u16string result = rfl::serialize_to_json<char16_t>(s);
  std::u16string expected = u"[1,2,3]";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TSetInt) {
  std::set<int> s{1, 2, 3};
  std::u32string result = rfl::serialize_to_json<char32_t>(s);
  std::u32string expected = U"[1,2,3]";
  EXPECT_EQ(expected, result);
}

// Tests for std::multimap - serialized as array
TEST(TypeOperationsSerializeToJson, Char8TMultiMap) {
  std::multimap<std::u8string, int> mm{{u8"a", 1}, {u8"b", 2}, {u8"a", 3}};
  std::u8string result = rfl::serialize_to_json<char8_t>(mm);
  std::u8string expected = u8R"([["a",1],["a",3],["b",2]])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char16TMultiMap) {
  std::multimap<std::u16string, int> mm{{u"x", 10}, {u"y", 20}};
  std::u16string result = rfl::serialize_to_json<char16_t>(mm);
  std::u16string expected = u"[[\"x\",10],[\"y\",20]]";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, Char32TMultiMap) {
  std::multimap<std::u32string, int> mm{{U"键", 100}, {U"值", 200}};
  std::u32string result = rfl::serialize_to_json<char32_t>(mm);
  std::u32string expected = U"[[\"值\",200],[\"键\",100]]";
  EXPECT_EQ(expected, result);
}

// Mixed types with char8_t template parameter

TEST(TypeOperationsSerializeToJson, Char8TMixedStruct) {
  struct mixed_t {
    std::u8string name;
    int age;
    std::u8string city;
  };
  mixed_t m{u8"李四", 25, u8"上海"};
  std::u8string result = rfl::serialize_to_json<char8_t>(m);
  std::u8string expected = u8R"({"name":"李四","age":25,"city":"上海"})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TMap) {
  std::map<std::u8string, std::u8string> m{{u8"key1", u8"值1"}, {u8"key2", u8"值2"}};
  std::u8string result = rfl::serialize_to_json<char8_t>(m);
  std::u8string expected = u8R"({"key1":"值1","key2":"值2"})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TMapIntKey) {
  std::map<int, std::u8string> m{{1, u8"一"}, {2, u8"二"}, {3, u8"三"}};
  std::u8string result = rfl::serialize_to_json<char8_t>(m);
  std::u8string expected = u8R"([[1,"一"],[2,"二"],[3,"三"]])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TMapMixedValue) {
  std::map<std::u8string, std::variant<std::u8string, int>> m{{u8"name", u8"张三"}, {u8"age", 25}};
  std::u8string result = rfl::serialize_to_json<char8_t>(m);
  std::u8string expected = u8R"({"age":25,"name":"张三"})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TMapNested) {
  std::map<std::u8string, std::map<std::u8string, int>> m{{u8"a", {{u8"x", 1}, {u8"y", 2}}},
                                                          {u8"b", {{u8"z", 3}}}};
  std::u8string result = rfl::serialize_to_json<char8_t>(m);
  std::u8string expected = u8R"({"a":{"x":1,"y":2},"b":{"z":3}})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TMapVectorValue) {
  std::map<std::u8string, std::vector<int>> m{{u8"奇数", {1, 3, 5}}, {u8"偶数", {2, 4, 6}}};
  std::u8string result = rfl::serialize_to_json<char8_t>(m);
  std::u8string expected = u8R"({"偶数":[2,4,6],"奇数":[1,3,5]})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TMapEmpty) {
  std::map<std::u8string, std::u8string> m{};
  std::u8string result = rfl::serialize_to_json<char8_t>(m);
  std::u8string expected = u8R"({})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TMapWithIndent) {
  std::map<std::u8string, std::u8string> m{{u8"中文键", u8"中文值"}, {u8"key2", u8"value2"}};
  std::u8string result = rfl::serialize_to_json<char8_t>(m, 2, char8_t{' '});
  std::u8string expected =
      u8R"({
  "key2": "value2",
  "中文键": "中文值"
})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TTuple) {
  std::tuple<std::u8string, int, std::u8string> t{u8"产品", 100, u8"名称"};
  std::u8string result = rfl::serialize_to_json<char8_t>(t);
  std::u8string expected = u8R"(["产品",100,"名称"])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TOptional) {
  std::optional<std::u8string> has_value{u8"有值"};
  std::optional<std::u8string> no_value{std::nullopt};

  std::u8string result1 = rfl::serialize_to_json<char8_t>(has_value);
  std::u8string expected1 = u8R"("有值")";
  EXPECT_TRUE(expected1 == result1);

  std::u8string result2 = rfl::serialize_to_json<char8_t>(no_value);
  std::u8string expected2 = u8R"(null)";
  EXPECT_TRUE(expected2 == result2);
}

TEST(TypeOperationsSerializeToJson, Char8TVariant) {
  std::variant<std::u8string, int> v1{u8"字符串"};
  std::variant<std::u8string, int> v2{42};

  std::u8string result1 = rfl::serialize_to_json<char8_t>(v1);
  std::u8string expected1 = u8R"("字符串")";
  EXPECT_TRUE(expected1 == result1);

  std::u8string result2 = rfl::serialize_to_json<char8_t>(v2);
  std::u8string expected2 = u8R"(42)";
  EXPECT_TRUE(expected2 == result2);
}

TEST(TypeOperationsSerializeToJson, Char8TDeepNested) {
  struct inner_t {
    std::u8string key;
    std::u8string value;
  };
  struct middle_t {
    std::vector<inner_t> items;
    std::u8string label;
  };
  struct outer_t {
    middle_t data;
    std::u8string title;
  };

  outer_t o{{{{u8"k1", u8"v1"}, {u8"k2", u8"v2"}}, u8"中间层"}, u8"外层标题"};
  std::u8string result = rfl::serialize_to_json<char8_t>(o);
  std::u8string expected =
      u8R"({"data":{"items":[{"key":"k1","value":"v1"},{"key":"k2","value":"v2"}],"label":"中间层"},"title":"外层标题"})";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TEmptyStrings) {
  std::vector<std::u8string> vec{u8"", u8"内容", u8""};
  std::u8string result = rfl::serialize_to_json<char8_t>(vec);
  std::u8string expected = u8R"(["","内容",""])";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, Char8TControlCharacters) {
  std::u8string s;
  s.push_back(u8'\x00');
  s.push_back(u8'\x01');
  s.push_back(u8'\x1F');
  std::u8string result = rfl::serialize_to_json<char8_t>(s);
  std::u8string expected = u8R"("\u0000\u0001\u001F")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar8) {
  std::string s = "换行\n制表\t双引\"反斜\\斜杠/";
  std::string result = rfl::serialize_to_json<char>(s);
  std::string expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar8T) {
  std::string s = "换行\n制表\t双引\"反斜\\斜杠/";
  std::u8string result = rfl::serialize_to_json<char8_t>(s);
  std::u8string expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar16T) {
  std::string s = "换行\n制表\t双引\"反斜\\斜杠/";
  std::u16string result = rfl::serialize_to_json<char16_t>(s);
  std::u16string expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar32T) {
  std::string s = "换行\n制表\t双引\"反斜\\斜杠/";
  std::u32string result = rfl::serialize_to_json<char32_t>(s);
  std::u32string expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar8) {
  std::u8string s = u8"换行\n制表\t双引\"反斜\\斜杠/";
  std::string result = rfl::serialize_to_json<char>(s);
  std::string expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar8T) {
  std::u8string s = u8"换行\n制表\t双引\"反斜\\斜杠/";
  std::u8string result = rfl::serialize_to_json<char8_t>(s);
  std::u8string expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar16T) {
  std::u8string s = u8"换行\n制表\t双引\"反斜\\斜杠/";
  std::u16string result = rfl::serialize_to_json<char16_t>(s);
  std::u16string expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar32T) {
  std::u8string s = u8"换行\n制表\t双引\"反斜\\斜杠/";
  std::u32string result = rfl::serialize_to_json<char32_t>(s);
  std::u32string expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar8) {
  std::u16string s = u"换行\n制表\t双引\"反斜\\斜杠/";
  std::string result = rfl::serialize_to_json<char>(s);
  std::string expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar8T) {
  std::u16string s = u"换行\n制表\t双引\"反斜\\斜杠/";
  std::u8string result = rfl::serialize_to_json<char8_t>(s);
  std::u8string expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar16T) {
  std::u16string s = u"换行\n制表\t双引\"反斜\\斜杠/";
  std::u16string result = rfl::serialize_to_json<char16_t>(s);
  std::u16string expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar32T) {
  std::u16string s = u"换行\n制表\t双引\"反斜\\斜杠/";
  std::u32string result = rfl::serialize_to_json<char32_t>(s);
  std::u32string expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar8) {
  std::u32string s = U"换行\n制表\t双引\"反斜\\斜杠/";
  std::string result = rfl::serialize_to_json<char>(s);
  std::string expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar8T) {
  std::u32string s = U"换行\n制表\t双引\"反斜\\斜杠/";
  std::u8string result = rfl::serialize_to_json<char8_t>(s);
  std::u8string expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE(expected == result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar16T) {
  std::u32string s = U"换行\n制表\t双引\"反斜\\斜杠/";
  std::u16string result = rfl::serialize_to_json<char16_t>(s);
  std::u16string expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar32T) {
  std::u32string s = U"换行\n制表\t双引\"反斜\\斜杠/";
  std::u32string result = rfl::serialize_to_json<char32_t>(s);
  std::u32string expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ(expected, result);
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::string_view{R"("A")"};
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::string_view{R"("\n")"};
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char, options>(c2)));

  // char8_t -> char
  constexpr auto c3 = u8'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char, options>(c4)));

  // char16_t -> char
  constexpr auto c5 = u'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char, options>(c6)));

  // char32_t -> char
  constexpr auto c7 = U'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char, options>(c8)));
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar8T) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char8_t
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::u8string_view{u8R"("A")"};
  EXPECT_TRUE(expected_1 == (rfl::serialize_to_json<char8_t, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::u8string_view{u8R"("\n")"};
  EXPECT_TRUE(expected_2 == (rfl::serialize_to_json<char8_t, options>(c2)));

  // char8_t -> char8_t
  constexpr auto c3 = u8'A';
  EXPECT_TRUE(expected_1 == (rfl::serialize_to_json<char8_t, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_TRUE(expected_2 == (rfl::serialize_to_json<char8_t, options>(c4)));

  // char16_t -> char8_t
  constexpr auto c5 = u'A';
  EXPECT_TRUE(expected_1 == (rfl::serialize_to_json<char8_t, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_TRUE(expected_2 == (rfl::serialize_to_json<char8_t, options>(c6)));

  // char32_t -> char8_t
  constexpr auto c7 = U'A';
  EXPECT_TRUE(expected_1 == (rfl::serialize_to_json<char8_t, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_TRUE(expected_2 == (rfl::serialize_to_json<char8_t, options>(c8)));
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar16T) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char16_t
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::u16string_view{u"\"A\""};
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char16_t, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::u16string_view{u"\"\\n\""};
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char16_t, options>(c2)));

  // char8_t -> char16_t
  constexpr auto c3 = u8'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char16_t, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char16_t, options>(c4)));

  // char16_t -> char16_t
  constexpr auto c5 = u'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char16_t, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char16_t, options>(c6)));

  // char32_t -> char16_t
  constexpr auto c7 = U'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char16_t, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char16_t, options>(c8)));
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar32T) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char32_t
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::u32string_view{U"\"A\""};
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char32_t, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::u32string_view{U"\"\\n\""};
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char32_t, options>(c2)));

  // char8_t -> char32_t
  constexpr auto c3 = u8'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char32_t, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char32_t, options>(c4)));

  // char16_t -> char32_t
  constexpr auto c5 = u'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char32_t, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char32_t, options>(c6)));

  // char32_t -> char32_t
  constexpr auto c7 = U'A';
  EXPECT_EQ(expected_1, (rfl::serialize_to_json<char32_t, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_EQ(expected_2, (rfl::serialize_to_json<char32_t, options>(c8)));
}

struct base_person_t {
  std::string name;
  int age;
};

struct derived_employee_t : base_person_t {
  double salary;
  std::string department;
};

TEST(TypeOperationsSerializeToJson, StructWithInheritance) {
  derived_employee_t emp{{"Alice", 30}, 75000.0, "Engineering"};
  EXPECT_EQ(R"({"name":"Alice","age":30,"salary":75000,"department":"Engineering"})",
            rfl::serialize_to_json(emp));
}

TEST(TypeOperationsSerializeToJson, StructWithInheritanceIndent) {
  derived_employee_t emp{{"Bob", 25}, 60000.5, "Sales"};
  EXPECT_EQ(R"({
  "name": "Bob",
  "age": 25,
  "salary": 60000.5,
  "department": "Sales"
})",
            rfl::serialize_to_json(emp, 2, ' '));
}

struct grand_base_t {
  int id;
};

struct middle_base_t : grand_base_t {
  std::string tag;
};

struct top_derived_t : middle_base_t {
  double value;
};

TEST(TypeOperationsSerializeToJson, StructWithMultiLevelInheritance) {
  top_derived_t obj;
  obj.id = 1;
  obj.tag = "test";
  obj.value = 3.14;
  EXPECT_EQ(R"({"id":1,"tag":"test","value":3.14})", rfl::serialize_to_json(obj));
}

struct multiple_base_a_t {
  int a;
};

struct multiple_base_b_t {
  int b;
};

struct multiple_derived_t : multiple_base_a_t, multiple_base_b_t {
  int c;
};

TEST(TypeOperationsSerializeToJson, StructWithMultipleInheritance) {
  multiple_derived_t obj{{1}, {2}, 3};
  EXPECT_EQ(R"({"a":1,"b":2,"c":3})", rfl::serialize_to_json(obj));
}

struct base_with_virtual_t {
  int x;
  virtual void greet() {
    std::println("x = {}", x);
  }
};

struct derived_with_virtual_t : base_with_virtual_t {
  int y;
  void greet() override {
    std::println("y = {}", y);
  }
};

TEST(TypeOperationsSerializeToJson, StructWithInheritanceWithVirtual) {
  derived_with_virtual_t obj;
  obj.x = 1;
  obj.y = 2;
  EXPECT_EQ(R"({"x":1,"y":2})", rfl::serialize_to_json(obj));
}

struct bitfield_t {
  unsigned int flags : 4;
  unsigned int mode : 4;
  unsigned int value : 8;
  int priority : 4;
};

TEST(TypeOperationsSerializeToJson, StructWithBitFields) {
  bitfield_t bf{};
  bf.flags = 0b1010;
  bf.mode = 0b0011;
  bf.value = 255;
  bf.priority = -8;
  EXPECT_EQ(R"({"flags":10,"mode":3,"value":255,"priority":-8})", rfl::serialize_to_json(bf));
}

struct bitfield_with_other_members_t {
  std::string name;
  unsigned int enabled : 1;
  unsigned int readonly : 1;
  unsigned int hidden : 1;
  int data;
};

TEST(TypeOperationsSerializeToJson, StructWithBitFieldsAndOtherMembers) {
  bitfield_with_other_members_t obj{"config", 1, 0, 1, 42};
  EXPECT_EQ(R"({"name":"config","enabled":1,"readonly":0,"hidden":1,"data":42})",
            rfl::serialize_to_json(obj));
}

struct bitfield_mixed_t {
  int a;
  unsigned int b : 3;
  int c;
  unsigned int d : 5;
  double e;
};

TEST(TypeOperationsSerializeToJson, StructWithBitFieldsMixed) {
  bitfield_mixed_t obj{1, 7, 2, 31, 3.14};
  EXPECT_EQ(R"({"a":1,"b":7,"c":2,"d":31,"e":3.14})", rfl::serialize_to_json(obj));
}
