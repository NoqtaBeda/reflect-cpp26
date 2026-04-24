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
#include <print>
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
  EXPECT_EQ_STATIC("42", rfl::serialize_to_json(42));
  EXPECT_EQ_STATIC("-123", rfl::serialize_to_json(-123));
  EXPECT_EQ_STATIC("0", rfl::serialize_to_json(0));
  EXPECT_EQ_STATIC("18446744073709551615", rfl::serialize_to_json(UINT64_MAX));
}

TEST(TypeOperationsSerializeToJson, BasicFloats) {
  EXPECT_EQ("3.14", rfl::serialize_to_json(3.14));
  EXPECT_EQ("-0.5", rfl::serialize_to_json(-0.5f));
  EXPECT_EQ("0", rfl::serialize_to_json(0.0));
}

TEST(TypeOperationsSerializeToJson, BasicBool) {
  EXPECT_EQ_STATIC("true", rfl::serialize_to_json(true));
  EXPECT_EQ_STATIC("false", rfl::serialize_to_json(false));
}

TEST(TypeOperationsSerializeToJson, BasicString) {
  EXPECT_EQ_STATIC("\"hello\"", rfl::serialize_to_json(std::string{"hello"}));
  EXPECT_EQ_STATIC("\"world\"", rfl::serialize_to_json(std::string_view{"world"}));
}

TEST(TypeOperationsSerializeToJson, CharToString) {
  EXPECT_EQ_STATIC(
      R"("a")",
      (rfl::serialize_to_json<char, rfl::serialize_options{.char_to_string = true}>('a')));
  EXPECT_EQ_STATIC(
      "97", (rfl::serialize_to_json<char, rfl::serialize_options{.char_to_string = false}>('a')));
}

TEST(TypeOperationsSerializeToJson, OptionalWithValue) {
  constexpr std::optional<int> opt{42};
  EXPECT_EQ_STATIC("42", rfl::serialize_to_json(opt));
}

TEST(TypeOperationsSerializeToJson, OptionalEmpty) {
  constexpr std::optional<int> opt{std::nullopt};
  EXPECT_EQ_STATIC("null", rfl::serialize_to_json(opt));
}

TEST(TypeOperationsSerializeToJson, VectorOfInts) {
  constexpr auto make_vec = []() constexpr -> std::vector<int> { return {1, 2, 3}; };
  EXPECT_EQ_STATIC("[1,2,3]", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, VectorOfBools) {
  constexpr auto make_vec = []() constexpr -> std::vector<bool> { return {true, false, true}; };
  EXPECT_EQ_STATIC("[true,false,true]", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsEmpty) {
  constexpr auto make_vec = []() constexpr -> std::vector<bool> { return {}; };
  EXPECT_EQ_STATIC("[]", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsAllTrue) {
  constexpr auto make_vec = []() constexpr -> std::vector<bool> { return {true, true, true}; };
  EXPECT_EQ_STATIC("[true,true,true]", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsAllFalse) {
  constexpr auto make_vec = []() constexpr -> std::vector<bool> { return {false, false, false}; };
  EXPECT_EQ_STATIC("[false,false,false]", rfl::serialize_to_json(make_vec()));
}

// Note: std::vector<char> is handled as a single string since it satisfies string_like concept
TEST(TypeOperationsSerializeToJson, VectorOfChars) {
  constexpr auto make_vec = []() constexpr -> std::vector<char> { return {'a', 'b', 'c'}; };
  EXPECT_EQ_STATIC(uR"("abc")", rfl::serialize_to_json<char16_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, EmptyVector) {
  constexpr auto make_vec = []() constexpr -> std::vector<int> { return {}; };
  EXPECT_EQ_STATIC("[]", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, VectorOfStrings) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::string> {
    return {"apple", "banana"};
  };
  EXPECT_EQ_STATIC(R"(["apple","banana"])", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, Pair) {
  constexpr auto make_pair = []() constexpr -> std::pair<int, std::string> {
    return {42, "hello"};
  };
  EXPECT_EQ_STATIC(R"([42,"hello"])", rfl::serialize_to_json(make_pair()));
}

TEST(TypeOperationsSerializeToJson, Tuple) {
  auto t = std::make_tuple(1, "hello", 3.14);
  EXPECT_EQ(R"([1,"hello",3.14])", rfl::serialize_to_json(t));
}

TEST(TypeOperationsSerializeToJson, EmptyTuple) {
  constexpr std::tuple<> t{};
  EXPECT_EQ_STATIC("[]", rfl::serialize_to_json(t));
}

TEST(TypeOperationsSerializeToJson, Struct) {
  constexpr auto make_person = []() constexpr -> person_t { return {"Alice", 30}; };
  EXPECT_EQ_STATIC(R"({"name":"Alice","age":30})", rfl::serialize_to_json(make_person()));
}

TEST(TypeOperationsSerializeToJson, NestedStruct) {
  constexpr auto make_nested = []() constexpr -> nested_t { return {{"Bob", 25}, {90, 85, 92}}; };
  EXPECT_EQ_STATIC(R"({"person":{"name":"Bob","age":25},"scores":[90,85,92]})",
                   rfl::serialize_to_json(make_nested()));
}

TEST(TypeOperationsSerializeToJson, EnumToInt) {
  EXPECT_EQ_STATIC("0", rfl::serialize_to_json(color_t::red));
  EXPECT_EQ_STATIC("1", rfl::serialize_to_json(color_t::green));
  EXPECT_EQ_STATIC("2", rfl::serialize_to_json(color_t::blue));
}

TEST(TypeOperationsSerializeToJson, EnumToString) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  EXPECT_EQ_STATIC(R"("red")", (rfl::serialize_to_json<char, opts>(color_t::red)));
}

TEST(TypeOperationsSerializeToJson, EnumFlagAsInteger) {
  constexpr auto r = permissions_t::read;
  EXPECT_EQ_STATIC("1", rfl::serialize_to_json(r));
  constexpr auto rw = r | permissions_t::write;
  EXPECT_EQ_STATIC("3", rfl::serialize_to_json(rw));
  constexpr auto rwx = rw | permissions_t::execute;
  EXPECT_EQ_STATIC("7", rfl::serialize_to_json(rwx));
}

TEST(TypeOperationsSerializeToJson, EnumFlagToString) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};

  constexpr auto r = permissions_t::read;
  EXPECT_EQ_STATIC(R"("read")", (rfl::serialize_to_json<char, opts>(r)));

  constexpr auto rw = r | permissions_t::write;
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(rw)),
              testing::AnyOf(R"("read|write")", R"("write|read")"));

  constexpr auto rwx = rw | permissions_t::execute;
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(rwx)),
              testing::AnyOf(R"("read|write|execute")",
                             R"("read|execute|write")",
                             R"("write|read|execute")",
                             R"("write|execute|read")",
                             R"("execute|read|write")",
                             R"("execute|write|read")"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagToStringInvalidHalts) {
  constexpr rfl::serialize_options opts{.enum_to_string = true, .halts_on_invalid_enum = true};
  constexpr auto invalid = static_cast<permissions_t>(8);
  EXPECT_EQ_STATIC(std::nullopt, (rfl::serialize_to_json<char, opts>(invalid)));
}

TEST(TypeOperationsSerializeToJson, EnumFlagToStringInvalidNull) {
  constexpr rfl::serialize_options opts{.enum_to_string = true, .halts_on_invalid_enum = false};
  constexpr auto invalid = static_cast<permissions_t>(8);
  EXPECT_EQ_STATIC("null", (rfl::serialize_to_json<char, opts>(invalid)));
}

namespace test_enum_flag_in_struct {
struct file_t {
  std::string name;
  permissions_t mode;
};
}  // namespace test_enum_flag_in_struct

TEST(TypeOperationsSerializeToJson, EnumFlagInStruct) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  test_enum_flag_in_struct::file_t f{"script.sh", permissions_t::read | permissions_t::execute};
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(f)),
              testing::AnyOf(R"({"name":"script.sh","mode":"read|execute"})",
                             R"({"name":"script.sh","mode":"execute|read"})"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagInVector) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  std::vector<permissions_t> vec{
      permissions_t::read, permissions_t::write, permissions_t::read | permissions_t::write};
  EXPECT_THAT(
      (rfl::serialize_to_json<char8_t, opts>(vec)),
      testing::AnyOf(u8R"(["read","write","read|write"])", u8R"(["read","write","write|read"])"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagEmptySet) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  auto result = rfl::serialize_to_json<char, opts>(static_cast<permissions_t>(0));
  EXPECT_EQ(R"("")", result);
}

TEST(TypeOperationsSerializeToJson, EnumFlagAllFlags) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  auto all = permissions_t::read | permissions_t::write | permissions_t::execute;
  EXPECT_THAT((rfl::serialize_to_json<char16_t, opts>(all)),
              testing::AnyOf(uR"("read|write|execute")",
                             uR"("read|execute|write")",
                             uR"("write|read|execute")",
                             uR"("write|execute|read")",
                             uR"("execute|read|write")",
                             uR"("execute|write|read")"));
}

namespace test_enum_flag_mixed_with_regular_enum {
struct config_t {
  color_t color;
  permissions_t perms;
};
}  // namespace test_enum_flag_mixed_with_regular_enum

TEST(TypeOperationsSerializeToJson, EnumFlagMixedWithRegularEnum) {
  using config_t = test_enum_flag_mixed_with_regular_enum::config_t;

  constexpr rfl::serialize_options opts{.enum_to_string = true};
  config_t c{color_t::blue, permissions_t::read | permissions_t::write};
  EXPECT_THAT((rfl::serialize_to_json<char32_t, opts>(c)),
              testing::AnyOf(UR"({"color":"blue","perms":"read|write"})",
                             UR"({"color":"blue","perms":"write|read"})"));
}

TEST(TypeOperationsSerializeToJson, EnumFlagInOptional) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  constexpr std::optional<permissions_t> present{permissions_t::read | permissions_t::execute};
  constexpr std::optional<permissions_t> empty{std::nullopt};
  EXPECT_THAT((rfl::serialize_to_json<char, opts>(present)),
              testing::AnyOf(R"("read|execute")", R"("execute|read")"));
  EXPECT_EQ_STATIC("null", (rfl::serialize_to_json<char, opts>(empty)));
}

TEST(TypeOperationsSerializeToJson, EnumFlagInVariant) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  constexpr std::variant<permissions_t, int> v1{permissions_t::write};
  constexpr std::variant<permissions_t, int> v2{42};
  EXPECT_EQ_STATIC(R"("write")", (rfl::serialize_to_json<char, opts>(v1)));
  EXPECT_EQ_STATIC("42", (rfl::serialize_to_json<char, opts>(v2)));
}

TEST(TypeOperationsSerializeToJson, EnumFlagWithChar16T) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  constexpr auto rw = permissions_t::read | permissions_t::write;
  EXPECT_THAT((rfl::serialize_to_json<char16_t, opts>(rw)),
              testing::AnyOf(u"\"read|write\"", u"\"write|read\""));
}

TEST(TypeOperationsSerializeToJson, EnumFlagWithChar32T) {
  constexpr rfl::serialize_options opts{.enum_to_string = true};
  constexpr auto rw = permissions_t::read | permissions_t::write;
  EXPECT_THAT((rfl::serialize_to_json<char32_t, opts>(rw)),
              testing::AnyOf(U"\"read|write\"", U"\"write|read\""));
}

TEST(TypeOperationsSerializeToJson, Variant1) {
  constexpr auto make_v1 = []() constexpr -> std::variant<int, std::string> { return 42; };
  constexpr auto make_v2 = []() constexpr -> std::variant<int, std::string> { return "hello"; };
  EXPECT_EQ_STATIC("42", rfl::serialize_to_json(make_v1()));
  EXPECT_EQ_STATIC("\"hello\"", rfl::serialize_to_json(make_v2()));
}

TEST(TypeOperationsSerializeToJson, Variant2) {
  struct S {
    int x;
  };
  constexpr std::variant<S, int> v{S{}};
  EXPECT_EQ_STATIC(R"({"x":0})", rfl::serialize_to_json(v));
}

TEST(TypeOperationsSerializeToJson, NestedOptional) {
  constexpr std::optional<std::optional<int>> nested{std::nullopt};
  EXPECT_EQ_STATIC("null", rfl::serialize_to_json(nested));
}

namespace test_optional_in_struct {
struct with_optional_t {
  std::string name;
  std::optional<int> age;
};
}  // namespace test_optional_in_struct

TEST(TypeOperationsSerializeToJson, OptionalInStruct) {
  constexpr auto make_w1 = []() constexpr -> test_optional_in_struct::with_optional_t {
    return {"Carol", 25};
  };
  constexpr auto make_w2 = []() constexpr -> test_optional_in_struct::with_optional_t {
    return {"Dave", std::nullopt};
  };
  EXPECT_EQ_STATIC(R"({"name":"Carol","age":25})", rfl::serialize_to_json(make_w1()));
  EXPECT_EQ_STATIC(R"({"name":"Dave","age":null})", rfl::serialize_to_json(make_w2()));
}

TEST(TypeOperationsSerializeToJson, Array) {
  constexpr std::array<int, 3> arr{1, 2, 3};
  EXPECT_EQ_STATIC("[1,2,3]", rfl::serialize_to_json(arr));
}

TEST(TypeOperationsSerializeToJson, WithIndent) {
  constexpr auto make_p = []() constexpr -> person_t { return {"Eve", 28}; };
  EXPECT_EQ_STATIC(R"({
  "name": "Eve",
  "age": 28
})",
                   rfl::serialize_to_json(make_p(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, NestedWithIndent) {
  constexpr auto make_n = []() constexpr -> nested_t { return {{"Frank", 35}, {100, 95}}; };
  EXPECT_EQ_STATIC(R"({
  "person": {
    "name": "Frank",
    "age": 35
  },
  "scores": [
    100,
    95
  ]
})",
                   rfl::serialize_to_json(make_n(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, VectorWithIndent) {
  constexpr auto make_vec = []() constexpr -> std::vector<int> { return {10, 20, 30}; };
  EXPECT_EQ_STATIC(R"([
  10,
  20,
  30
])",
                   rfl::serialize_to_json(make_vec(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, VectorOfBoolsWithIndent) {
  constexpr auto make_vec = []() constexpr -> std::vector<bool> { return {true, false, true}; };
  EXPECT_EQ_STATIC(R"([
  true,
  false,
  true
])",
                   rfl::serialize_to_json(make_vec(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, Monostate) {
  constexpr std::monostate m{};
  EXPECT_EQ_STATIC("null", rfl::serialize_to_json(m));
}

TEST(TypeOperationsSerializeToJson, OptionalInVariant) {
  constexpr auto make_v = []() constexpr -> std::variant<std::optional<int>, std::string> {
    return std::nullopt;
  };
  EXPECT_EQ_STATIC("null", rfl::serialize_to_json(make_v()));
}

TEST(TypeOperationsSerializeToJson, VectorOfOptionals) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::optional<int>> {
    return {1, std::nullopt, 3};
  };
  EXPECT_EQ_STATIC(R"([1,null,3])", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, StringWithSpecialChars) {
  constexpr auto make_s = []() constexpr -> std::string { return "hello\nworld\ttab"; };
  EXPECT_EQ_STATIC(R"("hello\nworld\ttab")", rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, StringWithMixedContent) {
  constexpr auto make_s = []() constexpr -> std::string {
    return "Hello \"World\"\nLine2\tTabbed";
  };
  EXPECT_EQ_STATIC(R"("Hello \"World\"\nLine2\tTabbed")", rfl::serialize_to_json(make_s()));
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
  constexpr auto make_v1 = []() constexpr -> std::variant<int, std::string, double> { return 42; };
  constexpr auto make_v2 = []() constexpr -> std::variant<int, std::string, double> {
    return "hello";
  };
  constexpr auto make_v3 = []() constexpr -> std::variant<int, std::string, double> {
    return 3.14;
  };
  EXPECT_EQ_STATIC("42", rfl::serialize_to_json(make_v1()));
  EXPECT_EQ_STATIC("\"hello\"", rfl::serialize_to_json(make_v2()));
  EXPECT_EQ("3.14", rfl::serialize_to_json(make_v3()));
}

TEST(TypeOperationsSerializeToJson, NestedStructInVector) {
  constexpr auto make_people = []() constexpr -> std::vector<person_t> {
    return {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};
  };
  EXPECT_EQ_STATIC(
      R"([{"name":"Alice","age":30},{"name":"Bob","age":25},{"name":"Charlie","age":35}])",
      rfl::serialize_to_json(make_people()));
}

namespace test_nested_struct_in_struct {
struct company_t {
  std::string name;
  person_t ceo;
  int founded_year;
};
}  // namespace test_nested_struct_in_struct

TEST(TypeOperationsSerializeToJson, NestedStructInStruct) {
  constexpr auto make_company = []() constexpr -> test_nested_struct_in_struct::company_t {
    return {"Acme Corp", {"John", 50}, 2020};
  };
  EXPECT_EQ_STATIC(R"({"name":"Acme Corp","ceo":{"name":"John","age":50},"founded_year":2020})",
                   rfl::serialize_to_json(make_company()));
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

TEST(TypeOperationsSerializeToJson, TripleNestedStruct) {
  constexpr auto make_company = []() constexpr -> test_triple_nested_struct::company_t {
    return {"TechCorp", {"Alice", {"123 Main St", "New York"}}};
  };
  EXPECT_EQ_STATIC(
      R"({"name":"TechCorp","ceo":{"name":"Alice","address":{"street":"123 Main St","city":"New York"}}})",
      rfl::serialize_to_json(make_company()));
}

TEST(TypeOperationsSerializeToJson, NestedOptionalInVector) {
  constexpr auto make_people = []() constexpr -> std::vector<std::optional<person_t>> {
    return {person_t{"Alice", 30}, std::nullopt, person_t{"Bob", 25}};
  };
  EXPECT_EQ_STATIC(R"([{"name":"Alice","age":30},null,{"name":"Bob","age":25}])",
                   rfl::serialize_to_json(make_people()));
}

namespace test_optional_in_struct_with_value {
struct config_t {
  std::string name;
  std::optional<int> port;
};
}  // namespace test_optional_in_struct_with_value

TEST(TypeOperationsSerializeToJson, OptionalStructWithValue) {
  constexpr auto make_c1 = []() constexpr -> test_optional_in_struct_with_value::config_t {
    return {"server", 8080};
  };
  constexpr auto make_c2 = []() constexpr -> test_optional_in_struct_with_value::config_t {
    return {"client", std::nullopt};
  };
  EXPECT_EQ_STATIC(R"({"name":"server","port":8080})", rfl::serialize_to_json(make_c1()));
  EXPECT_EQ_STATIC(R"({"name":"client","port":null})", rfl::serialize_to_json(make_c2()));
}

TEST(TypeOperationsSerializeToJson, VectorOfVariants) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::variant<int, std::string>> {
    return {42, "hello", 100, "world"};
  };
  EXPECT_EQ_STATIC(R"([42,"hello",100,"world"])", rfl::serialize_to_json(make_vec()));
}

namespace test_vector_of_bools_in_struct {
struct settings_t {
  std::string name;
  std::vector<bool> flags;
};
}  // namespace test_vector_of_bools_in_struct

TEST(TypeOperationsSerializeToJson, VectorOfBoolsInStruct) {
  constexpr auto make_settings = []() constexpr -> test_vector_of_bools_in_struct::settings_t {
    return {"debug", {true, false, true, false}};
  };
  EXPECT_EQ_STATIC(R"({"name":"debug","flags":[true,false,true,false]})",
                   rfl::serialize_to_json(make_settings()));
}

// Note: To-object serialization applies to std::map specializations only.
TEST(TypeOperationsSerializeToJson, MapLikeWithVector) {
  constexpr auto make_pairs = []() constexpr -> std::vector<std::pair<std::string, int>> {
    return {{"a", 1}, {"b", 2}, {"c", 3}};
  };
  EXPECT_EQ_STATIC(R"([["a",1],["b",2],["c",3]])", rfl::serialize_to_json(make_pairs()));
}

TEST(TypeOperationsSerializeToJson, TupleWithNestedTypes) {
  constexpr auto make_tuple =
      []() constexpr -> std::tuple<int, std::string, std::vector<int>, person_t> {
    return std::make_tuple(
        42, std::string{"hello"}, std::vector<int>{1, 2, 3}, person_t{"Alice", 30});
  };
  EXPECT_EQ_STATIC(R"([42,"hello",[1,2,3],{"name":"Alice","age":30}])",
                   rfl::serialize_to_json(make_tuple()));
}

TEST(TypeOperationsSerializeToJson, DeepNestedTuples) {
  constexpr auto inner = std::make_tuple(1, 2);
  constexpr auto outer = std::make_tuple(inner, 3);
  EXPECT_EQ_STATIC(R"([[1,2],3])", rfl::serialize_to_json(outer));
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

TEST(TypeOperationsSerializeToJson, StructWithAllTypes) {
  using mixed_t = test_struct_with_all_types::mixed_t;

  mixed_t m{42, 3.14, true, "test", {1, 2, 3}, {"Alice", 30}};
  EXPECT_EQ(R"({"i":42,"d":3.14,"b":true,"s":"test","arr":[1,2,3],)"
            R"("p":{"name":"Alice","age":30}})",
            rfl::serialize_to_json(m));
}

// The expected result shall be INDENTED by 2 spaces. Also, make the expected as raw string.
TEST(TypeOperationsSerializeToJson, NestedWithIndentComplex) {
  constexpr auto make_people = []() constexpr -> std::vector<person_t> {
    return {{"Alice", 30}, {"Bob", 25}};
  };
  EXPECT_EQ_STATIC(R"([
  {
    "name": "Alice",
    "age": 30
  },
  {
    "name": "Bob",
    "age": 25
  }
])",
                   rfl::serialize_to_json(make_people(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, EmptyOptional) {
  constexpr auto make_opt = []() constexpr -> std::optional<std::string> { return std::nullopt; };
  EXPECT_EQ_STATIC("null", rfl::serialize_to_json(make_opt()));
}

TEST(TypeOperationsSerializeToJson, OptionalWithString) {
  constexpr auto make_opt = []() constexpr -> std::optional<std::string> { return "hello"; };
  EXPECT_EQ_STATIC("\"hello\"", rfl::serialize_to_json(make_opt()));
}

TEST(TypeOperationsSerializeToJson, DoubleNestedOptional) {
  constexpr auto make_opt1 = []() constexpr -> std::optional<std::optional<int>> { return 42; };
  constexpr auto make_opt2 = []() constexpr -> std::optional<std::optional<int>> {
    return std::nullopt;
  };
  constexpr auto make_opt3 = []() constexpr -> std::optional<std::optional<int>> {
    return std::optional<int>{std::nullopt};
  };
  EXPECT_EQ_STATIC("42", rfl::serialize_to_json(make_opt1()));
  EXPECT_EQ_STATIC("null", rfl::serialize_to_json(make_opt2()));
  EXPECT_EQ_STATIC("null", rfl::serialize_to_json(make_opt3()));
}

namespace test_quadruple_nested_struct {
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
}  // namespace test_quadruple_nested_struct

TEST(TypeOperationsSerializeToJson, QuadrupleNestedStruct) {
  constexpr auto make_obj = []() constexpr -> test_quadruple_nested_struct::level4_t {
    return {{{{{1}}, "test"}, {1, 2, 3}}, true};
  };
  EXPECT_EQ_STATIC(R"({"l3":{"l2":{"l1":{"a":1},"s":"test"},"v":[1,2,3]},"b":true})",
                   rfl::serialize_to_json(make_obj()));
}

TEST(TypeOperationsSerializeToJson, QuadrupleNestedStructIndented) {
  constexpr auto make_obj = []() constexpr -> test_quadruple_nested_struct::level4_t {
    return {{{{{1}}, "test"}, {1, 2, 3}}, true};
  };
  EXPECT_EQ_STATIC(R"({
  "l3": {
    "l2": {
      "l1": {
        "a": 1
      },
      "s": "test"
    },
    "v": [
      1,
      2,
      3
    ]
  },
  "b": true
})",
                   rfl::serialize_to_json(make_obj(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, VectorOfPairs) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::pair<std::string, int>> {
    return {{"key1", 100}, {"key2", 200}};
  };
  EXPECT_EQ_STATIC(R"([["key1",100],["key2",200]])", rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, TupleOfStructs) {
  constexpr auto make_tuple = []() constexpr -> std::tuple<person_t, person_t> {
    return std::make_tuple(person_t{"Alice", 30}, person_t{"Bob", 25});
  };
  EXPECT_EQ_STATIC(R"([{"name":"Alice","age":30},{"name":"Bob","age":25}])",
                   rfl::serialize_to_json(make_tuple()));
}

namespace test_struct_with_variant {
struct container_t {
  std::variant<int, std::string> v1;
  std::variant<double, bool> v2;
};
}  // namespace test_struct_with_variant

TEST(TypeOperationsSerializeToJson, StructWithVariant) {
  constexpr auto make_obj = []() constexpr -> test_struct_with_variant::container_t {
    return {std::variant<int, std::string>{42}, std::variant<double, bool>{true}};
  };
  EXPECT_EQ_STATIC(R"({"v1":42,"v2":true})", rfl::serialize_to_json(make_obj()));
}

TEST(TypeOperationsSerializeToJson, StructWithVariantIndent) {
  constexpr auto make_obj = []() constexpr -> test_struct_with_variant::container_t {
    return {std::variant<int, std::string>{"hello"}, std::variant<double, bool>{3.14}};
  };
  EXPECT_EQ(R"({
  "v1": "hello",
  "v2": 3.14
})",
            rfl::serialize_to_json(make_obj(), 2, ' '));
}

namespace test_complex_nested_with_indent {
struct project_t {
  std::string name;
  std::vector<person_t> team;
  std::vector<std::pair<std::string, int>> scores;
};
}  // namespace test_complex_nested_with_indent

TEST(TypeOperationsSerializeToJson, ComplexNestedWithIndent) {
  constexpr auto make_project = []() constexpr -> test_complex_nested_with_indent::project_t {
    return {"Alpha", {{"Alice", 30}, {"Bob", 25}}, {{"a", 1}, {"b", 2}}};
  };
  EXPECT_EQ_STATIC(R"({
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
                   rfl::serialize_to_json(make_project(), 2, ' '));
}

namespace test_vector_of_nested_structs_indent {
struct item_t {
  std::string name;
  int value;
};
}  // namespace test_vector_of_nested_structs_indent

TEST(TypeOperationsSerializeToJson, VectorOfNestedStructsIndent) {
  using item_t = test_vector_of_nested_structs_indent::item_t;

  constexpr auto make_items = []() constexpr -> std::vector<item_t> {
    return {{"item1", 100}, {"item2", 200}};
  };
  EXPECT_EQ_STATIC(R"([
  {
    "name": "item1",
    "value": 100
  },
  {
    "name": "item2",
    "value": 200
  }
])",
                   rfl::serialize_to_json(make_items(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, DeepNestingWithIndent) {
  constexpr auto make_t = []() constexpr {
    return std::make_tuple(std::make_tuple(std::make_tuple(1, 2), std::vector<int>{3, 4}),
                           std::make_tuple(std::make_tuple(5, 6), std::vector<int>{7, 8}));
  };
  EXPECT_EQ_STATIC(R"([
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
                   rfl::serialize_to_json(make_t(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, UTF8String) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"你好"; };
  constexpr auto expected = std::string_view{R"("你好")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16String) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"你好"; };
  constexpr auto expected = std::string_view{R"("你好")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32String) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"你好"; };
  constexpr auto expected = std::string_view{R"("你好")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8StringToUtf16) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"你好"; };
  constexpr auto expected = std::u16string_view{uR"("你好")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8StringToUtf32) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"你好"; };
  constexpr auto expected = std::u32string_view{U"\"你好\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16StringToUtf8) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"你好"; };
  constexpr auto expected = std::u8string_view{u8R"("你好")"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16StringToUtf16) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"你好"; };
  constexpr auto expected = std::u16string_view{uR"("你好")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16StringToUtf32) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"你好"; };
  constexpr auto expected = std::u32string_view{U"\"你好\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32StringToUtf8) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"你好"; };
  constexpr auto expected = std::u8string_view{u8R"("你好")"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32StringToUtf16) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"你好"; };
  constexpr auto expected = std::u16string_view{uR"("你好")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32StringToUtf32) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"你好"; };
  constexpr auto expected = std::u32string_view{U"\"你好\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorOfStrings) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u8string> {
    return {u8"你好", u8"世界"};
  };
  constexpr auto expected = std::string_view{R"(["你好","世界"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStrings) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u16string> {
    return {u"你好", u"世界"};
  };
  constexpr auto expected = std::string_view{R"(["你好","世界"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStrings) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"你好", U"世界"};
  };
  constexpr auto expected = std::string_view{R"(["你好","世界"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorOfStringsToUtf16) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u8string> {
    return {u8"你好", u8"世界"};
  };
  constexpr auto expected = std::u16string_view{uR"(["你好","世界"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorOfStringsToUtf32) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u8string> {
    return {u8"你好", u8"世界"};
  };
  constexpr auto expected = std::u32string_view{U"[\"你好\",\"世界\"]"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStringsToUtf8) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u16string> {
    return {u"你好", u"世界"};
  };
  constexpr auto expected = std::u8string_view{u8R"(["你好","世界"])"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStringsToUtf16) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u16string> {
    return {u"你好", u"世界"};
  };
  constexpr auto expected = std::u16string_view{uR"(["你好","世界"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorOfStringsToUtf32) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u16string> {
    return {u"你好", u"世界"};
  };
  constexpr auto expected = std::u32string_view{U"[\"你好\",\"世界\"]"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStringsToUtf8) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"你好", U"世界"};
  };
  constexpr auto expected = std::u8string_view{u8R"(["你好","世界"])"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStringsToUtf16) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"你好", U"世界"};
  };
  constexpr auto expected = std::u16string_view{uR"(["你好","世界"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorOfStringsToUtf32) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"你好", U"世界"};
  };
  constexpr auto expected = std::u32string_view{U"[\"你好\",\"世界\"]"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_vec()));
}

namespace test_struct_with_utf_string {
template <typename CharT>
struct localized_t {
  std::basic_string<CharT> chinese;
  std::basic_string<CharT> english;
};
}  // namespace test_struct_with_utf_string

TEST(TypeOperationsSerializeToJson, StructWithUTF8String) {
  constexpr auto make_loc = []() constexpr -> test_struct_with_utf_string::localized_t<char8_t> {
    return {u8"中文", u8"English"};
  };
  constexpr auto expected = std::string_view{R"({"chinese":"中文","english":"English"})"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_loc()));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF16String) {
  constexpr auto make_loc = []() constexpr -> test_struct_with_utf_string::localized_t<char16_t> {
    return {u"中文", u"English"};
  };
  constexpr auto expected = std::string_view{R"({"chinese":"中文","english":"English"})"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_loc()));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF32String) {
  constexpr auto make_loc = []() constexpr -> test_struct_with_utf_string::localized_t<char32_t> {
    return {U"中文", U"English"};
  };
  constexpr auto expected = std::string_view{R"({"chinese":"中文","english":"English"})"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_loc()));
}

namespace test_nested_struct_with_utf8_indent {
struct item_t {
  std::u8string name;
  std::u8string description;
};
struct container_t {
  std::u8string title;
  std::vector<item_t> items;
};
}  // namespace test_nested_struct_with_utf8_indent

TEST(TypeOperationsSerializeToJson, NestedStructWithUTF8Indent) {
  constexpr auto make_container =
      []() constexpr -> test_nested_struct_with_utf8_indent::container_t {
    return {u8"标题", {{u8"项目1", u8"描述1"}, {u8"项目2", u8"描述2"}}};
  };
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
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_container(), 2, ' '));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithNewline) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"hello\nworld"; };
  constexpr auto expected = std::string_view{R"("hello\nworld")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithTab) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"hello\tworld"; };
  constexpr auto expected = std::string_view{R"("hello\tworld")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithQuote) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"他说:\"你好\""; };
  constexpr auto expected = std::string_view{R"("他说:\"你好\"")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithBackslash) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"路径:C:\\Users\\name"; };
  constexpr auto expected = std::string_view{R"("路径:C:\\Users\\name")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithNewline) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"hello\nworld"; };
  constexpr auto expected = std::string_view{R"("hello\nworld")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithQuote) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"他说:\"你好\""; };
  constexpr auto expected = std::string_view{R"("他说:\"你好\"")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithNewline) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"hello\nworld"; };
  constexpr auto expected = std::string_view{R"("hello\nworld")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithQuote) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"他说:\"你好\""; };
  constexpr auto expected = std::string_view{R"("他说:\"你好\"")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorWithEscapes) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u8string> {
    return {u8"line1\nline2", u8"tab\there"};
  };
  constexpr auto expected = std::string_view{R"(["line1\nline2","tab\there"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorWithEscapes) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u16string> {
    return {u"换行\n", u"制表\t"};
  };
  constexpr auto expected = std::string_view{R"(["换行\n","制表\t"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorWithEscapes) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"引号\"", U"反斜\\"};
  };
  constexpr auto expected = std::string_view{R"(["引号\"","反斜\\"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

namespace test_struct_with_utf8_escapes {
struct msg_t {
  std::u8string content;
  std::u8string path;
};
}  // namespace test_struct_with_utf8_escapes

TEST(TypeOperationsSerializeToJson, StructWithUTF8Escapes) {
  constexpr auto make_msg = []() constexpr -> test_struct_with_utf8_escapes::msg_t {
    return {u8"内容:\n换行", u8"C:\\path"};
  };
  constexpr auto expected = std::string_view{R"({"content":"内容:\n换行","path":"C:\\path"})"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_msg()));
}

// Escaping slashes: Not required by RFC 8259 standard, yet commonly applied in practice
// for historical and security reason.

TEST(TypeOperationsSerializeToJson, UTF8StringWithSlash) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"https://example.com/path"; };
  constexpr auto expected = std::string_view{R"("https:\/\/example.com\/path")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithSlashAndChinese) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"网址:https://中文.com"; };
  constexpr auto expected = std::string_view{R"("网址:https:\/\/中文.com")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8StringWithAllRFC8259Escapes) {
  constexpr auto make_s = []() constexpr -> std::u8string {
    return u8"引\"反\\斜/杠\n换\r行\t制表\f换页\b";
  };
  constexpr auto expected = std::string_view{R"("引\"反\\斜\/杠\n换\r行\t制表\f换页\b")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithSlash) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"https://example.com/path"; };
  constexpr auto expected = std::string_view{R"("https:\/\/example.com\/path")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF16StringWithSlashAndChinese) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"网址:https://中文.com"; };
  constexpr auto expected = std::string_view{R"("网址:https:\/\/中文.com")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithSlash) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"https://example.com/path"; };
  constexpr auto expected = std::string_view{R"("https:\/\/example.com\/path")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF32StringWithSlashAndChinese) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"网址:https://中文.com"; };
  constexpr auto expected = std::string_view{R"("网址:https:\/\/中文.com")"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_s()));
}

TEST(TypeOperationsSerializeToJson, UTF8VectorWithSlash) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u8string> {
    return {u8"http:/a.com", u8"https:/b.com"};
  };
  constexpr auto expected = std::string_view{R"(["http:\/a.com","https:\/b.com"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF16VectorWithSlash) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u16string> {
    return {u"网址/http", u"网址/https"};
  };
  constexpr auto expected = std::string_view{R"(["网址\/http","网址\/https"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, UTF32VectorWithSlash) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"网址/http", U"网址/https"};
  };
  constexpr auto expected = std::string_view{R"(["网址\/http","网址\/https"])"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_vec()));
}

TEST(TypeOperationsSerializeToJson, StructWithUTF8Slash) {
  struct url_t {
    std::u8string http;
    std::u8string https;
  };
  constexpr auto make_u = []() constexpr -> url_t {
    return {u8"http://example.com", u8"https://example.com"};
  };
  constexpr auto expected =
      std::string_view{R"({"http":"http:\/\/example.com","https":"https:\/\/example.com"})"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json(make_u()));
}

// Tests for serialize_to_json<char8_t> - returns std::u8string
// Note: Using EXPECT_TRUE(expected == actual) for u8string due to libgtest compatibility

TEST(TypeOperationsSerializeToJson, Char8TBasicString) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"你好"; };
  constexpr auto expected = std::u8string_view{u8R"("你好")"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithEscapes) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"换行:\n制表:\t"; };
  constexpr auto expected = std::u8string_view{u8R"("换行:\n制表:\t")"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithQuotes) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"他说:\"你好\""; };
  constexpr auto expected = std::u8string_view{u8R"("他说:\"你好\"")"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithBackslash) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"路径:C:\\Users"; };
  constexpr auto expected = std::u8string_view{u8R"("路径:C:\\Users")"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char8TStringWithAllEscapes) {
  constexpr auto make_s = []() constexpr -> std::u8string { return u8"换行\n制表\t双引\"反斜\\"; };
  constexpr auto expected = std::u8string_view{u8R"("换行\n制表\t双引\"反斜\\")"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char8TVector) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u16string> {
    return {u"你好", u"世界"};
  };
  constexpr auto expected = std::u8string_view{u8R"(["你好","世界"])"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, Char8TInt) {
  constexpr auto make_i = []() constexpr -> int { return 42; };
  constexpr auto expected = std::u8string_view{u8R"(42)"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_i()));
}

TEST(TypeOperationsSerializeToJson, Char8TFloat) {
  constexpr auto make_f = []() constexpr -> float { return 3.14f; };
  constexpr auto expected = std::u8string_view{u8R"(3.14)"};
  EXPECT_TRUE(expected == rfl::serialize_to_json<char8_t>(make_f()));
}

TEST(TypeOperationsSerializeToJson, Char8TBool) {
  constexpr auto make_b = []() constexpr -> bool { return true; };
  constexpr auto expected_true = std::u8string_view{u8R"(true)"};
  EXPECT_TRUE_STATIC(expected_true == rfl::serialize_to_json<char8_t>(make_b()));

  constexpr auto make_b_false = []() constexpr -> bool { return false; };
  constexpr auto expected_false = std::u8string_view{u8R"(false)"};
  EXPECT_TRUE_STATIC(expected_false == rfl::serialize_to_json<char8_t>(make_b_false()));
}

namespace test_char8_t_with_indent {
struct item_t {
  std::u8string name;
  std::u8string desc;
};
}  // namespace test_char8_t_with_indent

TEST(TypeOperationsSerializeToJson, Char8TWithIndent) {
  using item_t = test_char8_t_with_indent::item_t;

  constexpr auto make_vec = []() constexpr -> std::vector<item_t> {
    return {{u8"项1", u8"描1"}, {u8"项2", u8"描2"}};
  };
  constexpr std::u8string_view expected =
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
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_vec(), 2, char8_t{' '}));
}

namespace test_char8_t_struct {
struct person_t {
  std::u32string name;
  std::u32string city;
};
}  // namespace test_char8_t_struct

TEST(TypeOperationsSerializeToJson, Char8TStruct) {
  constexpr auto make_person = []() constexpr -> test_char8_t_struct::person_t {
    return {U"张三", U"北京"};
  };
  constexpr std::u8string_view expected = u8R"({"name":"张三","city":"北京"})";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_person()));
}

// Tests for serialize_to_json<char16_t> - returns std::u16string

TEST(TypeOperationsSerializeToJson, Char16TBasicString) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"你好"; };
  constexpr auto expected = std::u16string_view{u"\"你好\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char16TStringWithEscapes) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"换行:\n制表:\t"; };
  constexpr auto expected = std::u16string_view{u"\"换行:\\n制表:\\t\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char16TStringWithQuotes) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"他说:\"你好\""; };
  constexpr auto expected = std::u16string_view{u"\"他说:\\\"你好\\\"\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char16TStringWithBackslash) {
  constexpr auto make_s = []() constexpr -> std::u16string { return u"路径:C:\\Users"; };
  constexpr auto expected = std::u16string_view{u"\"路径:C:\\\\Users\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char16TVector) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"中文", U"测试"};
  };
  constexpr auto expected = std::u16string_view{u"[\"中文\",\"测试\"]"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, Char16TInt) {
  constexpr auto make_i = []() constexpr -> int { return 42; };
  constexpr auto expected = std::u16string_view{u"42"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_i()));
}

namespace test_char16_t_struct {
struct config_t {
  std::u16string title;
  std::u16string value;
};
}  // namespace test_char16_t_struct

TEST(TypeOperationsSerializeToJson, Char16TStruct) {
  constexpr auto make_config = []() constexpr -> test_char16_t_struct::config_t {
    return {u"标题", u"值"};
  };
  constexpr std::u16string_view expected = u"{\"title\":\"标题\",\"value\":\"值\"}";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_config()));
}

namespace test_char16_t_with_indent {
struct item_t {
  std::u16string name;
  std::u16string desc;
};
}  // namespace test_char16_t_with_indent

TEST(TypeOperationsSerializeToJson, Char16TWithIndent) {
  using item_t = test_char16_t_with_indent::item_t;

  constexpr auto make_vec = []() constexpr -> std::vector<item_t> {
    return {{u"项1", u"描1"}, {u"项2", u"描2"}};
  };
  constexpr std::u16string_view expected =
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
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_vec(), 2, char16_t{u' '}));
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
  constexpr auto make_s = []() constexpr -> std::u32string { return U"你好"; };
  constexpr auto expected = std::u32string_view{U"\"你好\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char32TStringWithEscapes) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"换行:\n制表:\t"; };
  constexpr auto expected = std::u32string_view{U"\"换行:\\n制表:\\t\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char32TStringWithQuotes) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"他说:\"你好\""; };
  constexpr auto expected = std::u32string_view{U"\"他说:\\\"你好\\\"\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char32TStringWithBackslash) {
  constexpr auto make_s = []() constexpr -> std::u32string { return U"路径:C:\\Users"; };
  constexpr auto expected = std::u32string_view{U"\"路径:C:\\\\Users\""};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, Char32TVector) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u32string> {
    return {U"你好", U"世界"};
  };
  constexpr auto expected = std::u32string_view{U"[\"你好\",\"世界\"]"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, Char32TInt) {
  constexpr auto make_i = []() constexpr -> int { return -999; };
  constexpr auto expected = std::u32string_view{U"-999"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_i()));
}

namespace test_char32_t_struct {
struct record_t {
  std::u32string label;
  std::u32string content;
};

struct item_t {
  std::u32string name;
  std::u32string desc;
};
}  // namespace test_char32_t_struct

TEST(TypeOperationsSerializeToJson, Char32TStruct) {
  using record_t = test_char32_t_struct::record_t;

  constexpr auto make_record = []() constexpr -> record_t { return {U"标签", U"内容"}; };
  constexpr auto expected = U"{\"label\":\"标签\",\"content\":\"内容\"}";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_record()));
}

TEST(TypeOperationsSerializeToJson, Char32TWithIndent) {
  using item_t = test_char32_t_struct::item_t;

  constexpr auto make_vec = []() constexpr -> std::vector<item_t> {
    return {{U"项1", U"描1"}, {U"项2", U"描2"}};
  };
  constexpr std::u32string_view expected =
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
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_vec(), 2));
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

// // Mixed types with char8_t template parameter

namespace test_char8_t_mixed_struct {
struct mixed_t {
  std::u8string name;
  int age;
  std::u8string city;
};
}  // namespace test_char8_t_mixed_struct

TEST(TypeOperationsSerializeToJson, Char8TMixedStruct) {
  using mixed_t = test_char8_t_mixed_struct::mixed_t;

  constexpr auto make_mixed = []() constexpr -> mixed_t { return {u8"李四", 25, u8"上海"}; };
  constexpr std::u8string_view expected = u8R"({"name":"李四","age":25,"city":"上海"})";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_mixed()));
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
  constexpr auto make_tuple = []() constexpr -> std::tuple<std::u8string, int, std::u8string> {
    return {u8"产品", 100, u8"名称"};
  };
  constexpr std::u8string_view expected = u8R"(["产品",100,"名称"])";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_tuple()));
}

TEST(TypeOperationsSerializeToJson, Char8TOptional) {
  constexpr auto make_optional = []() constexpr -> std::optional<std::u8string> {
    return u8"有值";
  };
  constexpr std::u8string_view expected_has_value = u8R"("有值")";
  EXPECT_TRUE_STATIC(expected_has_value == rfl::serialize_to_json<char8_t>(make_optional()));

  constexpr auto make_no_value = []() constexpr -> std::optional<std::u8string> {
    return std::nullopt;
  };
  constexpr std::u8string_view expected_no_value = u8R"(null)";
  EXPECT_TRUE_STATIC(expected_no_value == rfl::serialize_to_json<char8_t>(make_no_value()));
}

TEST(TypeOperationsSerializeToJson, Char8TVariant) {
  constexpr auto make_v1 = []() constexpr -> std::variant<std::u8string, int> {
    return u8"字符串";
  };
  constexpr std::u8string_view expected_v1 = u8R"("字符串")";
  EXPECT_TRUE_STATIC(expected_v1 == rfl::serialize_to_json<char8_t>(make_v1()));

  constexpr auto make_v2 = []() constexpr -> std::variant<std::u8string, int> { return 42; };
  constexpr std::u8string_view expected_v2 = u8R"(42)";
  EXPECT_TRUE_STATIC(expected_v2 == rfl::serialize_to_json<char8_t>(make_v2()));
}

namespace test_char8_t_deep_nested {
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
}  // namespace test_char8_t_deep_nested

TEST(TypeOperationsSerializeToJson, Char8TDeepNested) {
  using outer_t = test_char8_t_deep_nested::outer_t;

  constexpr auto make_outer = []() constexpr -> outer_t {
    return {{{{u8"k1", u8"v1"}, {u8"k2", u8"v2"}}, u8"中间层"}, u8"外层标题"};
  };
  constexpr std::u8string_view expected =
      u8R"({"data":{"items":[{"key":"k1","value":"v1"},{"key":"k2","value":"v2"}],"label":"中间层"},"title":"外层标题"})";
  EXPECT_TRUE(expected == rfl::serialize_to_json<char8_t>(make_outer()));
}

TEST(TypeOperationsSerializeToJson, Char8TEmptyStrings) {
  constexpr auto make_vec = []() constexpr -> std::vector<std::u8string> {
    return {u8"", u8"内容", u8""};
  };
  constexpr std::u8string_view expected = u8R"(["","内容",""])";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_vec()));
}

TEST(TypeOperationsSerializeToJson, Char8TControlCharacters) {
  constexpr auto make_s = []() constexpr -> std::u8string {
    std::u8string s;
    s.push_back(u8'\x00');
    s.push_back(u8'\x01');
    s.push_back(u8'\x1F');
    return s;
  };
  constexpr std::u8string_view expected = u8R"("\u0000\u0001\u001F")";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar8) {
  constexpr auto make_s = []() constexpr -> std::string { return "换行\n制表\t双引\"反斜\\斜杠/"; };
  constexpr std::string_view expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ(expected, rfl::serialize_to_json<char>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar8T) {
  constexpr auto make_s = []() constexpr -> std::string { return "换行\n制表\t双引\"反斜\\斜杠/"; };
  constexpr std::u8string_view expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar16T) {
  constexpr auto make_s = []() constexpr -> std::string { return "换行\n制表\t双引\"反斜\\斜杠/"; };
  constexpr std::u16string_view expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8ToChar32T) {
  constexpr auto make_s = []() constexpr -> std::string { return "换行\n制表\t双引\"反斜\\斜杠/"; };
  constexpr std::u32string_view expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar8) {
  constexpr auto make_s = []() constexpr -> std::u8string {
    return u8"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::string_view expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar8T) {
  constexpr auto make_s = []() constexpr -> std::u8string {
    return u8"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u8string_view expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar16T) {
  constexpr auto make_s = []() constexpr -> std::u8string {
    return u8"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u16string_view expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar8TToChar32T) {
  constexpr auto make_s = []() constexpr -> std::u8string {
    return u8"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u32string_view expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar8) {
  constexpr auto make_s = []() constexpr -> std::u16string {
    return u"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::string_view expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar8T) {
  constexpr auto make_s = []() constexpr -> std::u16string {
    return u"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u8string_view expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar16T) {
  constexpr auto make_s = []() constexpr -> std::u16string {
    return u"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u16string_view expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar16TToChar32T) {
  constexpr auto make_s = []() constexpr -> std::u16string {
    return u"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u32string_view expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar8) {
  constexpr auto make_s = []() constexpr -> std::u32string {
    return U"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::string_view expected = R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar8T) {
  constexpr auto make_s = []() constexpr -> std::u32string {
    return U"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u8string_view expected = u8R"("换行\n制表\t双引\"反斜\\斜杠\/")";
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar16T) {
  constexpr auto make_s = []() constexpr -> std::u32string {
    return U"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u16string_view expected = u"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, EscapeCharSerializeChar32TToChar32T) {
  constexpr auto make_s = []() constexpr -> std::u32string {
    return U"换行\n制表\t双引\"反斜\\斜杠/";
  };
  constexpr std::u32string_view expected = U"\"换行\\n制表\\t双引\\\"反斜\\\\斜杠\\/\"";
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::string_view{R"("A")"};
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::string_view{R"("\n")"};
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char, options>(c2)));

  // char8_t -> char
  constexpr auto c3 = u8'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char, options>(c4)));

  // char16_t -> char
  constexpr auto c5 = u'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char, options>(c6)));

  // char32_t -> char
  constexpr auto c7 = U'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char, options>(c8)));
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar8T) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char8_t
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::u8string_view{u8R"("A")"};
  EXPECT_TRUE_STATIC(expected_1 == (rfl::serialize_to_json<char8_t, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::u8string_view{u8R"("\n")"};
  EXPECT_TRUE(expected_2 == (rfl::serialize_to_json<char8_t, options>(c2)));

  // char8_t -> char8_t
  constexpr auto c3 = u8'A';
  EXPECT_TRUE_STATIC(expected_1 == (rfl::serialize_to_json<char8_t, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_TRUE_STATIC(expected_2 == (rfl::serialize_to_json<char8_t, options>(c4)));

  // char16_t -> char8_t
  constexpr auto c5 = u'A';
  EXPECT_TRUE_STATIC(expected_1 == (rfl::serialize_to_json<char8_t, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_TRUE_STATIC(expected_2 == (rfl::serialize_to_json<char8_t, options>(c6)));

  // char32_t -> char8_t
  constexpr auto c7 = U'A';
  EXPECT_TRUE_STATIC(expected_1 == (rfl::serialize_to_json<char8_t, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_TRUE_STATIC(expected_2 == (rfl::serialize_to_json<char8_t, options>(c8)));
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar16T) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char16_t
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::u16string_view{u"\"A\""};
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char16_t, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::u16string_view{u"\"\\n\""};
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char16_t, options>(c2)));

  // char8_t -> char16_t
  constexpr auto c3 = u8'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char16_t, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char16_t, options>(c4)));

  // char16_t -> char16_t
  constexpr auto c5 = u'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char16_t, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char16_t, options>(c6)));

  // char32_t -> char16_t
  constexpr auto c7 = U'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char16_t, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char16_t, options>(c8)));
}

TEST(TypeOperationsSerializeToJson, SingleCharToChar32T) {
  constexpr auto options = rfl::serialize_options{
      .char_to_string = true,
  };
  // char -> char32_t
  constexpr auto c1 = 'A';
  constexpr auto expected_1 = std::u32string_view{U"\"A\""};
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char32_t, options>(c1)));
  constexpr auto c2 = '\n';
  constexpr auto expected_2 = std::u32string_view{U"\"\\n\""};
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char32_t, options>(c2)));

  // char8_t -> char32_t
  constexpr auto c3 = u8'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char32_t, options>(c3)));
  constexpr auto c4 = u8'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char32_t, options>(c4)));

  // char16_t -> char32_t
  constexpr auto c5 = u'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char32_t, options>(c5)));
  constexpr auto c6 = u'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char32_t, options>(c6)));

  // char32_t -> char32_t
  constexpr auto c7 = U'A';
  EXPECT_EQ_STATIC(expected_1, (rfl::serialize_to_json<char32_t, options>(c7)));
  constexpr auto c8 = U'\n';
  EXPECT_EQ_STATIC(expected_2, (rfl::serialize_to_json<char32_t, options>(c8)));
}

struct base_person_t {
  std::string name;
  int age;
};

struct derived_employee_t : base_person_t {
  long salary;
  std::string department;
};

TEST(TypeOperationsSerializeToJson, StructWithInheritance) {
  constexpr auto make_employee = []() constexpr -> derived_employee_t {
    return {{"Alice", 30}, 75000, "Engineering"};
  };
  EXPECT_EQ_STATIC(R"({"name":"Alice","age":30,"salary":75000,"department":"Engineering"})",
                   rfl::serialize_to_json(make_employee()));
}

TEST(TypeOperationsSerializeToJson, StructWithInheritanceIndent) {
  constexpr auto make_employee = []() constexpr -> derived_employee_t {
    return {{"Bob", 25}, 60000, "Sales"};
  };
  EXPECT_EQ_STATIC(R"({
  "name": "Bob",
  "age": 25,
  "salary": 60000,
  "department": "Sales"
})",
                   rfl::serialize_to_json(make_employee(), 2, ' '));
}

struct grand_base_t {
  int id;
};

struct middle_base_t : grand_base_t {
  std::string tag;
};

struct top_derived_t : middle_base_t {
  long value;
};

TEST(TypeOperationsSerializeToJson, StructWithMultiLevelInheritance) {
  constexpr auto make_employee = []() constexpr -> top_derived_t { return {{{1}, "test"}, 333}; };
  EXPECT_EQ(R"({"id":1,"tag":"test","value":333})", rfl::serialize_to_json(make_employee()));
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
  constexpr auto make_employee = []() constexpr -> multiple_derived_t { return {{1}, {2}, 3}; };
  EXPECT_EQ_STATIC(R"({"a":1,"b":2,"c":3})", rfl::serialize_to_json(make_employee()));
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
  constexpr auto make_employee = []() constexpr -> derived_with_virtual_t {
    auto res = derived_with_virtual_t{};
    res.x = 1;
    res.y = 2;
    return res;
  };
  EXPECT_EQ_STATIC(R"({"x":1,"y":2})", rfl::serialize_to_json(make_employee()));
}

struct bitfield_t {
  unsigned int flags : 4;
  unsigned int mode : 4;
  unsigned int value : 8;
  int priority : 4;
};

TEST(TypeOperationsSerializeToJson, StructWithBitFields) {
  constexpr auto make_bf = []() constexpr -> bitfield_t {
    bitfield_t bf{};
    bf.flags = 0b1010;
    bf.mode = 0b0011;
    bf.value = 255;
    bf.priority = -8;
    return bf;
  };
  EXPECT_EQ_STATIC(R"({"flags":10,"mode":3,"value":255,"priority":-8})",
                   rfl::serialize_to_json(make_bf()));
}

struct bitfield_with_other_members_t {
  std::string name;
  unsigned int enabled : 1;
  unsigned int readonly : 1;
  unsigned int hidden : 1;
  int data;
};

TEST(TypeOperationsSerializeToJson, StructWithBitFieldsAndOtherMembers) {
  constexpr auto make_bf = []() constexpr -> bitfield_with_other_members_t {
    return {"config", 1, 0, 1, 42};
  };
  EXPECT_EQ_STATIC(R"({"name":"config","enabled":1,"readonly":0,"hidden":1,"data":42})",
                   rfl::serialize_to_json(make_bf()));
}

struct bitfield_mixed_t {
  int a;
  unsigned int b : 3;
  int c;
  unsigned int d : 5;
  double e;
};

TEST(TypeOperationsSerializeToJson, StructWithBitFieldsMixed) {
  constexpr auto make_bf = []() constexpr -> bitfield_mixed_t { return {1, 7, 2, 31, 3.14}; };
  EXPECT_EQ(R"({"a":1,"b":7,"c":2,"d":31,"e":3.14})", rfl::serialize_to_json(make_bf()));
}

namespace test_invalid_utf_sequence_replacement {
struct utf8_invalid_t {
  std::u8string data;
};
struct utf16_invalid_t {
  std::u16string data;
};
struct utf32_invalid_t {
  std::u32string data;
};
}  // namespace test_invalid_utf_sequence_replacement

TEST(TypeOperationsSerializeToJson, InvalidUtf8SequenceReplacement) {
  constexpr auto make_s = []() constexpr -> test_invalid_utf_sequence_replacement::utf8_invalid_t {
    std::u8string s = u8"Hello";
    s += static_cast<char8_t>(0xFF);  // Invalid UTF-8 byte
    s += static_cast<char8_t>(0xFE);  // Invalid UTF-8 byte
    s += u8"World";
    return {s};
  };
  constexpr auto expected = std::u8string_view{u8"{\"data\":\"Hello\uFFFDWorld\"}"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, InvalidUtf16SurrogateReplacement) {
  constexpr auto make_s = []() constexpr -> test_invalid_utf_sequence_replacement::utf16_invalid_t {
    std::u16string s = u"Hello";
    s += 0xD800;  // Orphaned high surrogate (not followed by low surrogate)
    s += u"World";
    s += 0xDC00;  // Orphaned low surrogate (not preceded by high surrogate)
    return {s};
  };
  constexpr auto expected = std::u16string_view{u"{\"data\":\"Hello\uFFFDWorld\uFFFD\"}"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, InvalidUtf32CodePointReplacement) {
  constexpr auto make_s = []() constexpr -> test_invalid_utf_sequence_replacement::utf32_invalid_t {
    std::u32string s = U"Hello";
    s += 0x110000;    // Invalid: code point > 0x10FFFF
    s += 0xFFFFFFFF;  // Invalid: max uint32_t
    s += U"World";
    return {s};
  };
  constexpr auto expected = std::u32string_view{U"{\"data\":\"Hello\uFFFDWorld\"}"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char32_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, InvalidUtf8InStruct) {
  constexpr auto make_s = []() constexpr -> test_invalid_utf_sequence_replacement::utf8_invalid_t {
    std::u8string s = u8"name=";
    s += static_cast<char8_t>(0x80);  // Invalid UTF-8 continuation byte without start
    s += u8"value";
    return {s};
  };
  constexpr auto expected = std::u8string_view{u8"{\"data\":\"name=\uFFFDvalue\"}"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, InvalidUtf8MultipleSequences) {
  constexpr auto make_s = []() constexpr -> test_invalid_utf_sequence_replacement::utf8_invalid_t {
    std::u8string s = u8"Start";
    s += static_cast<char8_t>(0xC0);  // Invalid: overlong encoding
    s += static_cast<char8_t>(0x80);
    s += static_cast<char8_t>(0xE0);  // Invalid: overlong encoding
    s += static_cast<char8_t>(0x80);
    s += u8"End";
    return {s};
  };
  constexpr auto expected = std::u8string_view{u8"{\"data\":\"Start\uFFFDEnd\"}"};
  EXPECT_TRUE_STATIC(expected == rfl::serialize_to_json<char8_t>(make_s()));
}

TEST(TypeOperationsSerializeToJson, InvalidUtf16SurrogatePairReplacement) {
  constexpr auto make_s = []() constexpr -> test_invalid_utf_sequence_replacement::utf16_invalid_t {
    std::u16string s = u"Valid";
    s += 0xD800;  // Orphaned high surrogate (followed by another high surrogate, not low)
    s += 0xD900;  // Valid surrogate pair
    s += 0xDC00;
    s += 0xDFFF;  // Orphaned low surrogate (not preceded by high)
    s += u"Text\n";
    return {s};
  };
  constexpr auto expected =
      std::u16string_view{u"{\"data\":\"Valid\uFFFD\U00050000\uFFFDText\\n\"}"};
  EXPECT_EQ_STATIC(expected, rfl::serialize_to_json<char16_t>(make_s()));
}
