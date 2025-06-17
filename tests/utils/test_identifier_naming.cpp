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

#include <reflect_cpp26/utils/identifier_naming.hpp>

#include "tests/test_options.hpp"

using namespace std::literals;
namespace rfl = reflect_cpp26;

constexpr auto INVALID = "";

#define TEST_INVALID_IDENTIFIERS(func)                                           \
  EXPECT_EQ(INVALID, rfl::func(""));                                             \
  EXPECT_EQ(std::nullopt, rfl::func##_opt(""));                                  \
  EXPECT_EQ(INVALID, rfl::func("1"));                                            \
  EXPECT_EQ(std::nullopt, rfl::func##_opt("1"));                                 \
  EXPECT_EQ(INVALID, rfl::func("1xy"));                                          \
  EXPECT_EQ(std::nullopt, rfl::func##_opt("1xy"));                               \
  EXPECT_EQ(INVALID, rfl::func("printf()"));                                     \
  EXPECT_EQ(std::nullopt, rfl::func##_opt("printf()"));                          \
  /* '#' and '@' are not supported. */                                           \
  EXPECT_EQ(INVALID, rfl::func("#values"));                                      \
  EXPECT_EQ(std::nullopt, rfl::func##_opt("#values"));                           \
  EXPECT_EQ(INVALID, rfl::func("@values"));                                      \
  EXPECT_EQ(std::nullopt, rfl::func##_opt("@values"));                           \
                                                                                 \
  EXPECT_EQ(INVALID, rfl::func(rfl::non_alpha_as_upper, ""));                    \
  EXPECT_EQ(std::nullopt, rfl::func##_opt(rfl::non_alpha_as_upper, ""));         \
  EXPECT_EQ(INVALID, rfl::func(rfl::non_alpha_as_upper, "1"));                   \
  EXPECT_EQ(std::nullopt, rfl::func##_opt(rfl::non_alpha_as_upper, "1"));        \
  EXPECT_EQ(INVALID, rfl::func(rfl::non_alpha_as_upper, "1xy"));                 \
  EXPECT_EQ(std::nullopt, rfl::func##_opt(rfl::non_alpha_as_upper, "1xy"));      \
  EXPECT_EQ(INVALID, rfl::func(rfl::non_alpha_as_upper, "printf()"));            \
  EXPECT_EQ(std::nullopt, rfl::func##_opt(rfl::non_alpha_as_upper, "printf()")); \
  /* '#' and '@' are not supported. */                                           \
  EXPECT_EQ(INVALID, rfl::func(rfl::non_alpha_as_upper, "#values"));             \
  EXPECT_EQ(std::nullopt, rfl::func##_opt(rfl::non_alpha_as_upper, "#values"));  \
  EXPECT_EQ(INVALID, rfl::func(rfl::non_alpha_as_upper, "@values"));             \
  EXPECT_EQ(std::nullopt, rfl::func##_opt(rfl::non_alpha_as_upper, "@values"))

TEST(UtilsIdentifierNaming, ToSnakeCase) {
  EXPECT_EQ_STATIC("get_name", rfl::to_snake_case("getName"));
  EXPECT_EQ_STATIC("set_json_file_path", rfl::to_snake_case("setJsonFilePath"));
  EXPECT_EQ_STATIC("set_json_file_path", rfl::to_snake_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("value_offset", rfl::to_snake_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("yaml_parser", rfl::to_snake_case("YamlParser"));
  EXPECT_EQ_STATIC("yaml_parser", rfl::to_snake_case("YAMLParser"));
  EXPECT_EQ_STATIC("num_gc_threads", rfl::to_snake_case("num-gcThreads"));
  EXPECT_EQ_STATIC("a", rfl::to_snake_case("a"));
  EXPECT_EQ_STATIC("a", rfl::to_snake_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_snake_case("$"));

  EXPECT_EQ_STATIC("snake_case", rfl::to_snake_case("snake_case"));
  EXPECT_EQ_STATIC("all_caps_snake_case", rfl::to_snake_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("kebab_case", rfl::to_snake_case("kebab-case"));
  EXPECT_EQ_STATIC("all_caps_kebab_case", rfl::to_snake_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("lower_camel_case", rfl::to_snake_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("upper_camel_case", rfl::to_snake_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("lower_camel_snake_case", rfl::to_snake_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("upper_camel_snake_case", rfl::to_snake_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("http_header_case", rfl::to_snake_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("get_x1_and_y1", rfl::to_snake_case("getX1AndY1"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_snake_case("x1y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_snake_case("X1y2"));
  EXPECT_EQ_STATIC("x1_y2", rfl::to_snake_case("x1Y2"));
  EXPECT_EQ_STATIC("x1_y2", rfl::to_snake_case("X1Y2"));
  EXPECT_EQ_STATIC("$num_args", rfl::to_snake_case("$numArgs"));
  EXPECT_EQ_STATIC("$_num_args", rfl::to_snake_case("$NumArgs"));
  EXPECT_EQ_STATIC("$_n_args", rfl::to_snake_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("get_x1_and_y1", rfl::to_snake_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("x_1y_2", rfl::to_snake_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("x_1y_2", rfl::to_snake_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("x_1y2", rfl::to_snake_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_snake_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$num_args", rfl::to_snake_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$_num_args", rfl::to_snake_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$n_args", rfl::to_snake_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("matrix_4x4", rfl::to_snake_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("matrix_4x4", rfl::to_snake_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  EXPECT_EQ_STATIC("is_ieee754_float",
                   rfl::to_snake_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("num_args_", rfl::to_snake_case("numArgs_"));
  EXPECT_EQ_STATIC("num_args____", rfl::to_snake_case("numArgs_-_-"));
  EXPECT_EQ_STATIC("_noreturn", rfl::to_snake_case("_Noreturn"));
  EXPECT_EQ_STATIC("__no_return", rfl::to_snake_case("__NoReturn"));
  EXPECT_EQ_STATIC("__x__y__", rfl::to_snake_case("__X__Y__"));
  EXPECT_EQ_STATIC("___ab_c__d_ef___", rfl::to_snake_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("__num_processes", rfl::to_snake_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("_", rfl::to_snake_case("_"));
  EXPECT_EQ_STATIC("_", rfl::to_snake_case("-"));
  EXPECT_EQ_STATIC("____", rfl::to_snake_case("____"));
  EXPECT_EQ_STATIC("____", rfl::to_snake_case("----"));
  EXPECT_EQ_STATIC("________", rfl::to_snake_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_snake_case);
}

TEST(UtilsIdentifierNaming, ToAllCapsSnakeCase) {
  EXPECT_EQ_STATIC("GET_NAME", rfl::to_all_caps_snake_case("getName"));
  EXPECT_EQ_STATIC("SET_JSON_FILE_PATH", rfl::to_all_caps_snake_case("setJsonFilePath"));
  EXPECT_EQ_STATIC("SET_JSON_FILE_PATH", rfl::to_all_caps_snake_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("VALUE_OFFSET", rfl::to_all_caps_snake_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("YAML_PARSER", rfl::to_all_caps_snake_case("YamlParser"));
  EXPECT_EQ_STATIC("YAML_PARSER", rfl::to_all_caps_snake_case("YAMLParser"));
  EXPECT_EQ_STATIC("NUM_GC_THREADS", rfl::to_all_caps_snake_case("num-gcThreads"));
  EXPECT_EQ_STATIC("A", rfl::to_all_caps_snake_case("a"));
  EXPECT_EQ_STATIC("A", rfl::to_all_caps_snake_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_all_caps_snake_case("$"));

  EXPECT_EQ_STATIC("SNAKE_CASE", rfl::to_all_caps_snake_case("snake_case"));
  EXPECT_EQ_STATIC("ALL_CAPS_SNAKE_CASE", rfl::to_all_caps_snake_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("KEBAB_CASE", rfl::to_all_caps_snake_case("kebab-case"));
  EXPECT_EQ_STATIC("ALL_CAPS_KEBAB_CASE", rfl::to_all_caps_snake_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("LOWER_CAMEL_CASE", rfl::to_all_caps_snake_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("UPPER_CAMEL_CASE", rfl::to_all_caps_snake_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("LOWER_CAMEL_SNAKE_CASE", rfl::to_all_caps_snake_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("UPPER_CAMEL_SNAKE_CASE", rfl::to_all_caps_snake_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("HTTP_HEADER_CASE", rfl::to_all_caps_snake_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("GET_X1_AND_Y1", rfl::to_all_caps_snake_case("getX1AndY1"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_all_caps_snake_case("x1y2"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_all_caps_snake_case("X1y2"));
  EXPECT_EQ_STATIC("X1_Y2", rfl::to_all_caps_snake_case("x1Y2"));
  EXPECT_EQ_STATIC("X1_Y2", rfl::to_all_caps_snake_case("X1Y2"));
  EXPECT_EQ_STATIC("$NUM_ARGS", rfl::to_all_caps_snake_case("$numArgs"));
  EXPECT_EQ_STATIC("$_NUM_ARGS", rfl::to_all_caps_snake_case("$NumArgs"));
  EXPECT_EQ_STATIC("$_N_ARGS", rfl::to_all_caps_snake_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("GET_X1_AND_Y1",
                   rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("X_1Y_2", rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("X_1Y_2", rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("X_1Y2", rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$NUM_ARGS", rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$_NUM_ARGS", rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$N_ARGS", rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("MATRIX_4X4",
                   rfl::to_all_caps_snake_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("MATRIX_4X4",
                   rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  EXPECT_EQ_STATIC("IS_IEEE754_FLOAT",
                   rfl::to_all_caps_snake_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("NUM_ARGS_", rfl::to_all_caps_snake_case("numArgs_"));
  EXPECT_EQ_STATIC("_NORETURN", rfl::to_all_caps_snake_case("_Noreturn"));
  EXPECT_EQ_STATIC("__NO_RETURN", rfl::to_all_caps_snake_case("__NoReturn"));
  EXPECT_EQ_STATIC("__X__Y__", rfl::to_all_caps_snake_case("__X__Y__"));
  EXPECT_EQ_STATIC("___AB_C__D_EF___", rfl::to_all_caps_snake_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("__NUM_PROCESSES", rfl::to_all_caps_snake_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("_", rfl::to_all_caps_snake_case("_"));
  EXPECT_EQ_STATIC("_", rfl::to_all_caps_snake_case("-"));
  EXPECT_EQ_STATIC("____", rfl::to_all_caps_snake_case("____"));
  EXPECT_EQ_STATIC("____", rfl::to_all_caps_snake_case("----"));
  EXPECT_EQ_STATIC("________", rfl::to_all_caps_snake_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_all_caps_snake_case);
}

TEST(UtilsIdentifierNaming, ToKebabCase) {
  EXPECT_EQ_STATIC("get-name", rfl::to_kebab_case("getName"));
  EXPECT_EQ_STATIC("set-json-file-path", rfl::to_kebab_case("setJsonFilePath"));
  EXPECT_EQ_STATIC("set-json-file-path", rfl::to_kebab_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("value-offset", rfl::to_kebab_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("yaml-parser", rfl::to_kebab_case("YamlParser"));
  EXPECT_EQ_STATIC("yaml-parser", rfl::to_kebab_case("YAMLParser"));
  EXPECT_EQ_STATIC("num-gc-threads", rfl::to_kebab_case("num-gcThreads"));
  EXPECT_EQ_STATIC("a", rfl::to_kebab_case("a"));
  EXPECT_EQ_STATIC("a", rfl::to_kebab_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_kebab_case("$"));

  EXPECT_EQ_STATIC("snake-case", rfl::to_kebab_case("snake_case"));
  EXPECT_EQ_STATIC("all-caps-snake-case", rfl::to_kebab_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("kebab-case", rfl::to_kebab_case("kebab-case"));
  EXPECT_EQ_STATIC("all-caps-kebab-case", rfl::to_kebab_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("lower-camel-case", rfl::to_kebab_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("upper-camel-case", rfl::to_kebab_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("lower-camel-snake-case", rfl::to_kebab_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("upper-camel-snake-case", rfl::to_kebab_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("http-header-case", rfl::to_kebab_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("get-x1-and-y1", rfl::to_kebab_case("getX1AndY1"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_kebab_case("x1y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_kebab_case("X1y2"));
  EXPECT_EQ_STATIC("x1-y2", rfl::to_kebab_case("x1Y2"));
  EXPECT_EQ_STATIC("x1-y2", rfl::to_kebab_case("X1Y2"));
  EXPECT_EQ_STATIC("$num-args", rfl::to_kebab_case("$numArgs"));
  EXPECT_EQ_STATIC("$-num-args", rfl::to_kebab_case("$NumArgs"));
  EXPECT_EQ_STATIC("$-n-args", rfl::to_kebab_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("get-x1-and-y1", rfl::to_kebab_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("x-1y-2", rfl::to_kebab_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("x-1y-2", rfl::to_kebab_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("x-1y2", rfl::to_kebab_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_kebab_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$num-args", rfl::to_kebab_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$-num-args", rfl::to_kebab_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$n-args", rfl::to_kebab_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("matrix-4x4", rfl::to_kebab_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("matrix-4x4", rfl::to_kebab_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  EXPECT_EQ_STATIC("is-ieee754-float",
                   rfl::to_kebab_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("num-args-", rfl::to_kebab_case("numArgs_"));
  EXPECT_EQ_STATIC("-noreturn", rfl::to_kebab_case("_Noreturn"));
  EXPECT_EQ_STATIC("--no-return", rfl::to_kebab_case("__NoReturn"));
  EXPECT_EQ_STATIC("--x--y--", rfl::to_kebab_case("__X__Y__"));
  EXPECT_EQ_STATIC("---ab-c--d-ef---", rfl::to_kebab_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("--num-processes", rfl::to_kebab_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("-", rfl::to_kebab_case("_"));
  EXPECT_EQ_STATIC("-", rfl::to_kebab_case("-"));
  EXPECT_EQ_STATIC("----", rfl::to_kebab_case("____"));
  EXPECT_EQ_STATIC("----", rfl::to_kebab_case("----"));
  EXPECT_EQ_STATIC("--------", rfl::to_kebab_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_kebab_case);
}

TEST(UtilsIdentifierNaming, ToAllCapsKebabCase) {
  EXPECT_EQ_STATIC("GET-NAME", rfl::to_all_caps_kebab_case("getName"));
  EXPECT_EQ_STATIC("SET-JSON-FILE-PATH", rfl::to_all_caps_kebab_case("setJsonFilePath"));
  EXPECT_EQ_STATIC("SET-JSON-FILE-PATH", rfl::to_all_caps_kebab_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("VALUE-OFFSET", rfl::to_all_caps_kebab_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("YAML-PARSER", rfl::to_all_caps_kebab_case("YamlParser"));
  EXPECT_EQ_STATIC("YAML-PARSER", rfl::to_all_caps_kebab_case("YAMLParser"));
  EXPECT_EQ_STATIC("NUM-GC-THREADS", rfl::to_all_caps_kebab_case("num-gcThreads"));
  EXPECT_EQ_STATIC("A", rfl::to_all_caps_kebab_case("a"));
  EXPECT_EQ_STATIC("A", rfl::to_all_caps_kebab_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_all_caps_kebab_case("$"));

  EXPECT_EQ_STATIC("SNAKE-CASE", rfl::to_all_caps_kebab_case("snake_case"));
  EXPECT_EQ_STATIC("ALL-CAPS-SNAKE-CASE", rfl::to_all_caps_kebab_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("KEBAB-CASE", rfl::to_all_caps_kebab_case("kebab-case"));
  EXPECT_EQ_STATIC("ALL-CAPS-KEBAB-CASE", rfl::to_all_caps_kebab_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("LOWER-CAMEL-CASE", rfl::to_all_caps_kebab_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("UPPER-CAMEL-CASE", rfl::to_all_caps_kebab_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("LOWER-CAMEL-SNAKE-CASE", rfl::to_all_caps_kebab_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("UPPER-CAMEL-SNAKE-CASE", rfl::to_all_caps_kebab_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("HTTP-HEADER-CASE", rfl::to_all_caps_kebab_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("GET-X1-AND-Y1", rfl::to_all_caps_kebab_case("getX1AndY1"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_all_caps_kebab_case("x1y2"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_all_caps_kebab_case("X1y2"));
  EXPECT_EQ_STATIC("X1-Y2", rfl::to_all_caps_kebab_case("x1Y2"));
  EXPECT_EQ_STATIC("X1-Y2", rfl::to_all_caps_kebab_case("X1Y2"));
  EXPECT_EQ_STATIC("$NUM-ARGS", rfl::to_all_caps_kebab_case("$numArgs"));
  EXPECT_EQ_STATIC("$-NUM-ARGS", rfl::to_all_caps_kebab_case("$NumArgs"));
  EXPECT_EQ_STATIC("$-N-ARGS", rfl::to_all_caps_kebab_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("GET-X1-AND-Y1",
                   rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("X-1Y-2", rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("X-1Y-2", rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("X-1Y2", rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$NUM-ARGS", rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$-NUM-ARGS", rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$N-ARGS", rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("MATRIX-4X4",
                   rfl::to_all_caps_kebab_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("MATRIX-4X4",
                   rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  EXPECT_EQ_STATIC("IS-IEEE754-FLOAT",
                   rfl::to_all_caps_kebab_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("NUM-ARGS-", rfl::to_all_caps_kebab_case("numArgs_"));
  EXPECT_EQ_STATIC("-NORETURN", rfl::to_all_caps_kebab_case("_Noreturn"));
  EXPECT_EQ_STATIC("--NO-RETURN", rfl::to_all_caps_kebab_case("__NoReturn"));
  EXPECT_EQ_STATIC("--X--Y--", rfl::to_all_caps_kebab_case("__X__Y__"));
  EXPECT_EQ_STATIC("---AB-C--D-EF---", rfl::to_all_caps_kebab_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("--NUM-PROCESSES", rfl::to_all_caps_kebab_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("-", rfl::to_all_caps_kebab_case("_"));
  EXPECT_EQ_STATIC("-", rfl::to_all_caps_kebab_case("-"));
  EXPECT_EQ_STATIC("----", rfl::to_all_caps_kebab_case("____"));
  EXPECT_EQ_STATIC("----", rfl::to_all_caps_kebab_case("----"));
  EXPECT_EQ_STATIC("--------", rfl::to_all_caps_kebab_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_all_caps_kebab_case);
}

TEST(UtilsIdentifierNaming, ToLowerCamelCase) {
  EXPECT_EQ_STATIC("getName", rfl::to_lower_camel_case("getName"));
  EXPECT_EQ_STATIC("setJsonFilePath", rfl::to_lower_camel_case("setJsonFilePath"));
  // Note: irreversible convertion
  EXPECT_EQ_STATIC("setJsonFilePath", rfl::to_lower_camel_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("valueOffset", rfl::to_lower_camel_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("yamlParser", rfl::to_lower_camel_case("YamlParser"));
  EXPECT_EQ_STATIC("yamlParser", rfl::to_lower_camel_case("YAMLParser"));
  EXPECT_EQ_STATIC("numGcThreads", rfl::to_lower_camel_case("num-gcThreads"));
  EXPECT_EQ_STATIC("a", rfl::to_lower_camel_case("a"));
  EXPECT_EQ_STATIC("a", rfl::to_lower_camel_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_lower_camel_case("$"));

  EXPECT_EQ_STATIC("snakeCase", rfl::to_lower_camel_case("snake_case"));
  EXPECT_EQ_STATIC("allCapsSnakeCase", rfl::to_lower_camel_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("kebabCase", rfl::to_lower_camel_case("kebab-case"));
  EXPECT_EQ_STATIC("allCapsKebabCase", rfl::to_lower_camel_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("lowerCamelCase", rfl::to_lower_camel_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("upperCamelCase", rfl::to_lower_camel_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("lowerCamelSnakeCase", rfl::to_lower_camel_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("upperCamelSnakeCase", rfl::to_lower_camel_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("httpHeaderCase", rfl::to_lower_camel_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("getX1AndY1", rfl::to_lower_camel_case("getX1AndY1"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_case("x1y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_case("X1y2"));
  EXPECT_EQ_STATIC("x1Y2", rfl::to_lower_camel_case("x1Y2"));
  EXPECT_EQ_STATIC("x1Y2", rfl::to_lower_camel_case("X1Y2"));
  EXPECT_EQ_STATIC("$numArgs", rfl::to_lower_camel_case("$numArgs"));
  EXPECT_EQ_STATIC("$NumArgs", rfl::to_lower_camel_case("$NumArgs"));
  EXPECT_EQ_STATIC("$NArgs", rfl::to_lower_camel_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("getX1AndY1", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$numArgs", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$NumArgs", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$nArgs", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("matrix4x4", rfl::to_lower_camel_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("matrix4x4", rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  // Note: irreversible
  EXPECT_EQ_STATIC("isIeee754Float",
                   rfl::to_lower_camel_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("numArgs_", rfl::to_lower_camel_case("numArgs_"));
  EXPECT_EQ_STATIC("_noreturn", rfl::to_lower_camel_case("_Noreturn"));
  EXPECT_EQ_STATIC("__noReturn", rfl::to_lower_camel_case("__NoReturn"));
  EXPECT_EQ_STATIC("__x_Y__", rfl::to_lower_camel_case("__X__Y__"));
  EXPECT_EQ_STATIC("___abC_DEf___", rfl::to_lower_camel_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("__numProcesses", rfl::to_lower_camel_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("_", rfl::to_lower_camel_case("_"));
  EXPECT_EQ_STATIC("_", rfl::to_lower_camel_case("-"));
  EXPECT_EQ_STATIC("____", rfl::to_lower_camel_case("____"));
  EXPECT_EQ_STATIC("____", rfl::to_lower_camel_case("----"));
  EXPECT_EQ_STATIC("________", rfl::to_lower_camel_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_lower_camel_case);
}

TEST(UtilsIdentifierNaming, ToUpperCamelCase) {
  EXPECT_EQ_STATIC("GetName", rfl::to_upper_camel_case("getName"));
  EXPECT_EQ_STATIC("SetJsonFilePath", rfl::to_upper_camel_case("setJsonFilePath"));
  // Note: irreversible convertion
  EXPECT_EQ_STATIC("SetJsonFilePath", rfl::to_upper_camel_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("ValueOffset", rfl::to_upper_camel_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("YamlParser", rfl::to_upper_camel_case("YamlParser"));
  EXPECT_EQ_STATIC("YamlParser", rfl::to_upper_camel_case("YAMLParser"));
  EXPECT_EQ_STATIC("NumGcThreads", rfl::to_upper_camel_case("num-gcThreads"));
  EXPECT_EQ_STATIC("A", rfl::to_upper_camel_case("a"));
  EXPECT_EQ_STATIC("A", rfl::to_upper_camel_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_upper_camel_case("$"));

  EXPECT_EQ_STATIC("SnakeCase", rfl::to_upper_camel_case("snake_case"));
  EXPECT_EQ_STATIC("AllCapsSnakeCase", rfl::to_upper_camel_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("KebabCase", rfl::to_upper_camel_case("kebab-case"));
  EXPECT_EQ_STATIC("AllCapsKebabCase", rfl::to_upper_camel_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("LowerCamelCase", rfl::to_upper_camel_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("UpperCamelCase", rfl::to_upper_camel_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("LowerCamelSnakeCase", rfl::to_upper_camel_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("UpperCamelSnakeCase", rfl::to_upper_camel_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("HttpHeaderCase", rfl::to_upper_camel_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("GetX1AndY1", rfl::to_upper_camel_case("getX1AndY1"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_case("x1y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_case("X1y2"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_upper_camel_case("x1Y2"));
  EXPECT_EQ_STATIC("X1Y2", rfl::to_upper_camel_case("X1Y2"));
  EXPECT_EQ_STATIC("$numArgs", rfl::to_upper_camel_case("$numArgs"));
  EXPECT_EQ_STATIC("$NumArgs", rfl::to_upper_camel_case("$NumArgs"));
  EXPECT_EQ_STATIC("$NArgs", rfl::to_upper_camel_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("GetX1AndY1", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$numArgs", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$NumArgs", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$nArgs", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("Matrix4x4", rfl::to_upper_camel_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("Matrix4x4", rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  // Note: irreversible
  EXPECT_EQ_STATIC("IsIeee754Float",
                   rfl::to_upper_camel_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("NumArgs_", rfl::to_upper_camel_case("numArgs_"));
  EXPECT_EQ_STATIC("_Noreturn", rfl::to_upper_camel_case("_Noreturn"));
  EXPECT_EQ_STATIC("__NoReturn", rfl::to_upper_camel_case("__NoReturn"));
  EXPECT_EQ_STATIC("__X_Y__", rfl::to_upper_camel_case("__X__Y__"));
  EXPECT_EQ_STATIC("___AbC_DEf___", rfl::to_upper_camel_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("__NumProcesses", rfl::to_upper_camel_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("_", rfl::to_upper_camel_case("_"));
  EXPECT_EQ_STATIC("_", rfl::to_upper_camel_case("-"));
  EXPECT_EQ_STATIC("____", rfl::to_upper_camel_case("____"));
  EXPECT_EQ_STATIC("____", rfl::to_upper_camel_case("----"));
  EXPECT_EQ_STATIC("________", rfl::to_upper_camel_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_upper_camel_case);
}

TEST(UtilsIdentifierNaming, ToLowerCamelSnakeCase) {
  EXPECT_EQ_STATIC("get_Name", rfl::to_lower_camel_snake_case("getName"));
  EXPECT_EQ_STATIC("set_Json_File_Path", rfl::to_lower_camel_snake_case("setJsonFilePath"));
  // Note: irreversible convertion
  EXPECT_EQ_STATIC("set_Json_File_Path", rfl::to_lower_camel_snake_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("value_Offset", rfl::to_lower_camel_snake_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("yaml_Parser", rfl::to_lower_camel_snake_case("YamlParser"));
  EXPECT_EQ_STATIC("yaml_Parser", rfl::to_lower_camel_snake_case("YAMLParser"));
  EXPECT_EQ_STATIC("num_Gc_Threads", rfl::to_lower_camel_snake_case("num-gcThreads"));
  EXPECT_EQ_STATIC("a", rfl::to_lower_camel_snake_case("a"));
  EXPECT_EQ_STATIC("a", rfl::to_lower_camel_snake_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_lower_camel_snake_case("$"));

  EXPECT_EQ_STATIC("snake_Case", rfl::to_lower_camel_snake_case("snake_case"));
  EXPECT_EQ_STATIC("all_Caps_Snake_Case", rfl::to_lower_camel_snake_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("kebab_Case", rfl::to_lower_camel_snake_case("kebab-case"));
  EXPECT_EQ_STATIC("all_Caps_Kebab_Case", rfl::to_lower_camel_snake_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("lower_Camel_Case", rfl::to_lower_camel_snake_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("upper_Camel_Case", rfl::to_lower_camel_snake_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("lower_Camel_Snake_Case",
                   rfl::to_lower_camel_snake_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("upper_Camel_Snake_Case",
                   rfl::to_lower_camel_snake_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("http_Header_Case", rfl::to_lower_camel_snake_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("get_X1_And_Y1", rfl::to_lower_camel_snake_case("getX1AndY1"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_snake_case("x1y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_snake_case("X1y2"));
  EXPECT_EQ_STATIC("x1_Y2", rfl::to_lower_camel_snake_case("x1Y2"));
  EXPECT_EQ_STATIC("x1_Y2", rfl::to_lower_camel_snake_case("X1Y2"));
  EXPECT_EQ_STATIC("$num_Args", rfl::to_lower_camel_snake_case("$numArgs"));
  EXPECT_EQ_STATIC("$_Num_Args", rfl::to_lower_camel_snake_case("$NumArgs"));
  EXPECT_EQ_STATIC("$_N_Args", rfl::to_lower_camel_snake_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("get_X1_And_Y1",
                   rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("x_1y_2", rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("x_1y_2", rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("x_1y2", rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("x1y2", rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$num_Args",
                   rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$_Num_Args",
                   rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$n_Args", rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("matrix_4x4",
                   rfl::to_lower_camel_snake_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("matrix_4x4",
                   rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  // Note: irreversible
  EXPECT_EQ_STATIC("is_Ieee754_Float",
                   rfl::to_lower_camel_snake_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("num_Args_", rfl::to_lower_camel_snake_case("numArgs_"));
  EXPECT_EQ_STATIC("_noreturn", rfl::to_lower_camel_snake_case("_Noreturn"));
  EXPECT_EQ_STATIC("__no_Return", rfl::to_lower_camel_snake_case("__NoReturn"));
  EXPECT_EQ_STATIC("__x__Y__", rfl::to_lower_camel_snake_case("__X__Y__"));
  EXPECT_EQ_STATIC("___ab_C__D_Ef___", rfl::to_lower_camel_snake_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("__num_Processes", rfl::to_lower_camel_snake_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("_", rfl::to_lower_camel_snake_case("_"));
  EXPECT_EQ_STATIC("_", rfl::to_lower_camel_snake_case("-"));
  EXPECT_EQ_STATIC("____", rfl::to_lower_camel_snake_case("____"));
  EXPECT_EQ_STATIC("____", rfl::to_lower_camel_snake_case("----"));
  EXPECT_EQ_STATIC("________", rfl::to_lower_camel_snake_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_lower_camel_snake_case);
}

TEST(UtilsIdentifierNaming, ToUpperCamelSnakeCase) {
  EXPECT_EQ_STATIC("Get_Name", rfl::to_upper_camel_snake_case("getName"));
  EXPECT_EQ_STATIC("Set_Json_File_Path", rfl::to_upper_camel_snake_case("setJsonFilePath"));
  // Note: irreversible convertion
  EXPECT_EQ_STATIC("Set_Json_File_Path", rfl::to_upper_camel_snake_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("Value_Offset", rfl::to_upper_camel_snake_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("Yaml_Parser", rfl::to_upper_camel_snake_case("YamlParser"));
  EXPECT_EQ_STATIC("Yaml_Parser", rfl::to_upper_camel_snake_case("YAMLParser"));
  EXPECT_EQ_STATIC("Num_Gc_Threads", rfl::to_upper_camel_snake_case("num-gcThreads"));
  EXPECT_EQ_STATIC("A", rfl::to_upper_camel_snake_case("a"));
  EXPECT_EQ_STATIC("A", rfl::to_upper_camel_snake_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_upper_camel_snake_case("$"));

  EXPECT_EQ_STATIC("Snake_Case", rfl::to_upper_camel_snake_case("snake_case"));
  EXPECT_EQ_STATIC("All_Caps_Snake_Case", rfl::to_upper_camel_snake_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("Kebab_Case", rfl::to_upper_camel_snake_case("kebab-case"));
  EXPECT_EQ_STATIC("All_Caps_Kebab_Case", rfl::to_upper_camel_snake_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("Lower_Camel_Case", rfl::to_upper_camel_snake_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("Upper_Camel_Case", rfl::to_upper_camel_snake_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("Lower_Camel_Snake_Case",
                   rfl::to_upper_camel_snake_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("Upper_Camel_Snake_Case",
                   rfl::to_upper_camel_snake_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("Http_Header_Case", rfl::to_upper_camel_snake_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("Get_X1_And_Y1", rfl::to_upper_camel_snake_case("getX1AndY1"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_snake_case("x1y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_snake_case("X1y2"));
  EXPECT_EQ_STATIC("X1_Y2", rfl::to_upper_camel_snake_case("x1Y2"));
  EXPECT_EQ_STATIC("X1_Y2", rfl::to_upper_camel_snake_case("X1Y2"));
  EXPECT_EQ_STATIC("$num_Args", rfl::to_upper_camel_snake_case("$numArgs"));
  EXPECT_EQ_STATIC("$_Num_Args", rfl::to_upper_camel_snake_case("$NumArgs"));
  EXPECT_EQ_STATIC("$_N_Args", rfl::to_upper_camel_snake_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("Get_X1_And_Y1",
                   rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("X_1y_2", rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("X_1y_2", rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("X_1y2", rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$num_Args",
                   rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$_Num_Args",
                   rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$n_Args", rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("Matrix_4x4",
                   rfl::to_upper_camel_snake_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("Matrix_4x4",
                   rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  // Note: irreversible
  EXPECT_EQ_STATIC("Is_Ieee754_Float",
                   rfl::to_upper_camel_snake_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("Num_Args_", rfl::to_upper_camel_snake_case("numArgs_"));
  EXPECT_EQ_STATIC("_Noreturn", rfl::to_upper_camel_snake_case("_Noreturn"));
  EXPECT_EQ_STATIC("__No_Return", rfl::to_upper_camel_snake_case("__NoReturn"));
  EXPECT_EQ_STATIC("__X__Y__", rfl::to_upper_camel_snake_case("__X__Y__"));
  EXPECT_EQ_STATIC("___Ab_C__D_Ef___", rfl::to_upper_camel_snake_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("__Num_Processes", rfl::to_upper_camel_snake_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("_", rfl::to_upper_camel_snake_case("_"));
  EXPECT_EQ_STATIC("_", rfl::to_upper_camel_snake_case("-"));
  EXPECT_EQ_STATIC("____", rfl::to_upper_camel_snake_case("____"));
  EXPECT_EQ_STATIC("____", rfl::to_upper_camel_snake_case("----"));
  EXPECT_EQ_STATIC("________", rfl::to_upper_camel_snake_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_upper_camel_snake_case);
}

TEST(UtilsIdentifierNaming, ToHttpHeaderCase) {
  EXPECT_EQ_STATIC("Get-Name", rfl::to_http_header_case("getName"));
  EXPECT_EQ_STATIC("Set-Json-File-Path", rfl::to_http_header_case("setJsonFilePath"));
  // Note: irreversible convertion
  EXPECT_EQ_STATIC("Set-Json-File-Path", rfl::to_http_header_case("setJSONFilePath"));
  EXPECT_EQ_STATIC("Value-Offset", rfl::to_http_header_case("VALUE_OFFSET"));
  EXPECT_EQ_STATIC("Yaml-Parser", rfl::to_http_header_case("YamlParser"));
  EXPECT_EQ_STATIC("Yaml-Parser", rfl::to_http_header_case("YAMLParser"));
  EXPECT_EQ_STATIC("Num-Gc-Threads", rfl::to_http_header_case("num-gcThreads"));
  EXPECT_EQ_STATIC("A", rfl::to_http_header_case("a"));
  EXPECT_EQ_STATIC("A", rfl::to_http_header_case("A"));
  EXPECT_EQ_STATIC("$", rfl::to_http_header_case("$"));

  EXPECT_EQ_STATIC("Snake-Case", rfl::to_http_header_case("snake_case"));
  EXPECT_EQ_STATIC("All-Caps-Snake-Case", rfl::to_http_header_case("ALL_CAPS_SNAKE_CASE"));
  EXPECT_EQ_STATIC("Kebab-Case", rfl::to_http_header_case("kebab-case"));
  EXPECT_EQ_STATIC("All-Caps-Kebab-Case", rfl::to_http_header_case("ALL-CAPS-KEBAB-CASE"));
  EXPECT_EQ_STATIC("Lower-Camel-Case", rfl::to_http_header_case("lowerCamelCase"));
  EXPECT_EQ_STATIC("Upper-Camel-Case", rfl::to_http_header_case("UpperCamelCase"));
  EXPECT_EQ_STATIC("Lower-Camel-Snake-Case", rfl::to_http_header_case("lower_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("Upper-Camel-Snake-Case", rfl::to_http_header_case("Upper_Camel_Snake_Case"));
  EXPECT_EQ_STATIC("Http-Header-Case", rfl::to_http_header_case("HTTP-Header-Case"));

  // Non-alpha characters as lower case
  EXPECT_EQ_STATIC("Get-X1-And-Y1", rfl::to_http_header_case("getX1AndY1"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_http_header_case("x1y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_http_header_case("X1y2"));
  EXPECT_EQ_STATIC("X1-Y2", rfl::to_http_header_case("x1Y2"));
  EXPECT_EQ_STATIC("X1-Y2", rfl::to_http_header_case("X1Y2"));
  EXPECT_EQ_STATIC("$num-Args", rfl::to_http_header_case("$numArgs"));
  EXPECT_EQ_STATIC("$-Num-Args", rfl::to_http_header_case("$NumArgs"));
  EXPECT_EQ_STATIC("$-N-Args", rfl::to_http_header_case("$NArgs"));

  // Non-alpha characters as upper case
  EXPECT_EQ_STATIC("Get-X1-And-Y1",
                   rfl::to_http_header_case(rfl::non_alpha_as_upper, "getX1AndY1"));
  EXPECT_EQ_STATIC("X-1y-2", rfl::to_http_header_case(rfl::non_alpha_as_upper, "x1y2"));
  EXPECT_EQ_STATIC("X-1y-2", rfl::to_http_header_case(rfl::non_alpha_as_upper, "X1y2"));
  EXPECT_EQ_STATIC("X-1y2", rfl::to_http_header_case(rfl::non_alpha_as_upper, "x1Y2"));
  EXPECT_EQ_STATIC("X1y2", rfl::to_http_header_case(rfl::non_alpha_as_upper, "X1Y2"));
  EXPECT_EQ_STATIC("$num-Args", rfl::to_http_header_case(rfl::non_alpha_as_upper, "$numArgs"));
  EXPECT_EQ_STATIC("$-Num-Args", rfl::to_http_header_case(rfl::non_alpha_as_upper, "$NumArgs"));
  EXPECT_EQ_STATIC("$n-Args", rfl::to_http_header_case(rfl::non_alpha_as_upper, "$NArgs"));

  EXPECT_EQ_STATIC("Matrix-4x4", rfl::to_http_header_case(rfl::non_alpha_as_lower, "MATRIX_4x4"));
  EXPECT_EQ_STATIC("Matrix-4x4", rfl::to_http_header_case(rfl::non_alpha_as_upper, "MATRIX_4X4"));
  // Note: irreversible
  EXPECT_EQ_STATIC("Is-Ieee754-Float",
                   rfl::to_http_header_case(rfl::non_alpha_as_upper, "IsIEEE754Float"));

  EXPECT_EQ_STATIC("Num-Args-", rfl::to_http_header_case("numArgs_"));
  EXPECT_EQ_STATIC("-Noreturn", rfl::to_http_header_case("_Noreturn"));
  EXPECT_EQ_STATIC("--No-Return", rfl::to_http_header_case("__NoReturn"));
  EXPECT_EQ_STATIC("--X--Y--", rfl::to_http_header_case("__X__Y__"));
  EXPECT_EQ_STATIC("---Ab-C--D-Ef---", rfl::to_http_header_case("___AbC__DEf___"));
  // When you forget to remove the "--" prefix:
  EXPECT_EQ_STATIC("--Num-Processes", rfl::to_http_header_case("--num-processes"));

  // Delimiters only
  EXPECT_EQ_STATIC("-", rfl::to_http_header_case("_"));
  EXPECT_EQ_STATIC("-", rfl::to_http_header_case("-"));
  EXPECT_EQ_STATIC("----", rfl::to_http_header_case("____"));
  EXPECT_EQ_STATIC("----", rfl::to_http_header_case("----"));
  EXPECT_EQ_STATIC("--------", rfl::to_http_header_case("_--__--_"));

  // Invalid identifiers
  TEST_INVALID_IDENTIFIERS(to_http_header_case);
}

TEST(UtilsIdentifierNaming, ConvertIdentifier) {
  using rules = rfl::identifier_naming_rule;
  EXPECT_EQ_STATIC("num_args", rfl::convert_identifier("numArgs", rules::snake_case));
  EXPECT_EQ_STATIC("NUM_ARGS", rfl::convert_identifier("numArgs", rules::all_caps_snake_case));
  EXPECT_EQ_STATIC("num-args", rfl::convert_identifier("numArgs", rules::kebab_case));
  EXPECT_EQ_STATIC("NUM-ARGS", rfl::convert_identifier("numArgs", rules::all_caps_kebab_case));
  EXPECT_EQ_STATIC("numArgs", rfl::convert_identifier("num-args", rules::lower_camel_case));
  EXPECT_EQ_STATIC("NumArgs", rfl::convert_identifier("num-args", rules::upper_camel_case));
  EXPECT_EQ_STATIC("num_Args", rfl::convert_identifier("num-args", rules::lower_camel_snake_case));
  EXPECT_EQ_STATIC("Num_Args", rfl::convert_identifier("num-args", rules::upper_camel_snake_case));
  EXPECT_EQ_STATIC("Num-Args", rfl::convert_identifier("num-args", rules::http_header_case));
  // Fallback on invalid naming rule
  EXPECT_EQ("", rfl::convert_identifier("numArgs", static_cast<rules>(123)));
}

TEST(UtilsIdentifierNaming, ConvertIdentifierOpt) {
  using rules = rfl::identifier_naming_rule;
  EXPECT_EQ_STATIC("num_args", rfl::convert_identifier_opt("numArgs", rules::snake_case));
  EXPECT_EQ_STATIC("NUM_ARGS", rfl::convert_identifier_opt("numArgs", rules::all_caps_snake_case));
  EXPECT_EQ_STATIC("num-args", rfl::convert_identifier_opt("numArgs", rules::kebab_case));
  EXPECT_EQ_STATIC("NUM-ARGS", rfl::convert_identifier_opt("numArgs", rules::all_caps_kebab_case));
  EXPECT_EQ_STATIC("numArgs", rfl::convert_identifier_opt("num-args", rules::lower_camel_case));
  EXPECT_EQ_STATIC("NumArgs", rfl::convert_identifier_opt("num-args", rules::upper_camel_case));
  EXPECT_EQ_STATIC("num_Args",
                   rfl::convert_identifier_opt("num-args", rules::lower_camel_snake_case));
  EXPECT_EQ_STATIC("Num_Args",
                   rfl::convert_identifier_opt("num-args", rules::upper_camel_snake_case));
  EXPECT_EQ_STATIC("Num-Args", rfl::convert_identifier_opt("num-args", rules::http_header_case));
  // Fallback on invalid naming rule
  EXPECT_EQ(std::nullopt, rfl::convert_identifier_opt("numArgs", static_cast<rules>(123)));
}
