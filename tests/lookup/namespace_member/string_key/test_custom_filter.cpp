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

#include <reflect_cpp26/lookup/lookup_table.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

namespace qux {
#define ANNOTATE(...) [[=(__VA_ARGS__)]]
constexpr long a = 1;
constexpr long b = 2;
constexpr long c = 3;
constexpr long d = 4;
ANNOTATE('?') constexpr long e = 5;

ANNOTATE('+')
constexpr long sum(long x, const long* y) {
  return y == nullptr ? x : x + *y;
}

ANNOTATE('-')
constexpr long subtract(long x, const long* y = nullptr) {
  return y == nullptr ? x : x - *y;
}
}  // namespace qux

TEST(NamespaceLookupTableByName, CustomFilter) {
  constexpr auto table_v =
      LOOKUP_TABLE(qux, [](std::string_view identifier) -> std::optional<std::string> {
        if (identifier.length() == 1 && (identifier[0] - 'a') % 2 == 0) {
          return std::string{identifier};
        }
        return std::nullopt;
      });
  static_assert(std::is_same_v<const long*, decltype(table_v)::value_type>);
  static_assert(table_v.size() == 3);

  CHECK_VARIABLE_STATIC(1, table_v["a"]);
  CHECK_VARIABLE_STATIC(3, table_v["c"]);
  CHECK_VARIABLE_STATIC(5, table_v["e"]);
  EXPECT_EQ_STATIC(nullptr, table_v["b"]);
  EXPECT_EQ_STATIC(nullptr, table_v["d"]);

  constexpr auto table_f =
      LOOKUP_TABLE(qux, [](std::meta::info member) -> std::optional<std::string> {
        if (is_variable(member) || annotations_of(member).size() != 1) {
          return std::nullopt;
        }
        auto annotation = extract<char>(annotations_of(member)[0]);
        return std::string(1zU, annotation);
      });
  static_assert(std::is_same_v<long (*)(long, const long*), decltype(table_f)::value_type>);
  static_assert(table_f.size() == 2);

  constexpr auto y = 42L;
  CHECK_FUNCTION_STATIC(45, table_f["+"], 3, &y);
  CHECK_FUNCTION_STATIC(-39, table_f["-"], 3, &y);
  EXPECT_EQ_STATIC(nullptr, table_f["?"]);
  EXPECT_EQ_STATIC(nullptr, table_f["sum"]);
  EXPECT_EQ_STATIC(nullptr, table_f["subtract"]);
}
