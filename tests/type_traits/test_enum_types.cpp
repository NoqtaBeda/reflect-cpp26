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

#include <reflect_cpp26/enum/enum_bitwise_operators.hpp>
#include <reflect_cpp26/type_traits/enum_types.hpp>
#include <system_error>
#include <utility>

#include "tests/test_options.hpp"

namespace rfl = reflect_cpp26;

enum unscoped_t {
  UNSCOPED_A = 1,
  UNSCOPED_B = 2,
};

enum class scoped_1_t {
  foo = 1,
  bar = 2,
  baz = 4,
};
// operator&, operator|, operator^
REFLECT_CPP26_DEFINE_ENUM_BITWISE_BINARY_OPERATORS(scoped_1_t)

enum class scoped_2_t {
  foo = 1,
  bar = 2,
  baz = 4,
};
// Marked as enum flag via is_enum_flag_v specialization
template <>
constexpr auto rfl::is_enum_flag_v<scoped_2_t> = true;

// Marked as enum flag via annotation
enum class AS_ENUM_FLAG annotated_scoped_t {
  foo = 1,
  bar = 2,
  baz = 4,
};

enum AS_ENUM_FLAG annotated_unscoped_t {
  ANNOTATED_UNSCOPED_A = 1,
  ANNOTATED_UNSCOPED_B = 2,
  ANNOTATED_UNSCOPED_C = 4,
};

TEST(TypeTraits, EnumFlag) {
  // Unscoped enum
  static_assert(NOT rfl::enum_flag_type<unscoped_t>);
  // Scoped enum, without is_enum_flag_v specialization
  static_assert(NOT rfl::enum_flag_type<std::errc>);
  // Not classified as enum flag even if operator&, operator| and operator^ are all provided
  static_assert(NOT rfl::enum_flag_type<std::byte>);
  static_assert(NOT rfl::enum_flag_type<scoped_1_t>);
  // Scoped enum, with is_enum_flag_v specialization
  static_assert(rfl::enum_flag_type<scoped_2_t>);
  // Scoped, annotated
  static_assert(rfl::enum_flag_type<const annotated_scoped_t>);
  // Unscoped, annotated
  static_assert(rfl::enum_flag_type<volatile annotated_unscoped_t>);

  // Not enum at all
  static_assert(NOT rfl::enum_flag_type<annotated_scoped_t&>);
  static_assert(NOT rfl::enum_flag_type<void>);
  static_assert(NOT rfl::enum_flag_type<int>);
}
