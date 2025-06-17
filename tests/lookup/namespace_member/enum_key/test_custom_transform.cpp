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

#include <reflect_cpp26/enum/enum_cast.hpp>
#include <reflect_cpp26/lookup/lookup_table.hpp>
#include <reflect_cpp26/utils/identifier_naming.hpp>

#include "tests/lookup/lookup_test_options.hpp"

#define LOOKUP_TABLE(...) REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(__VA_ARGS__)

namespace rfl = reflect_cpp26;

namespace ops {
enum class Opcode {
  ADD = 1,
  SUB = 2,
  BITWISE_AND = 4,
  BITWISE_OR = 8,
};

constexpr unsigned HandleAdd(unsigned x, unsigned y) {
  return x + y;
}
constexpr unsigned HandleSub(unsigned x, unsigned y) {
  return x - y;
}
constexpr unsigned HandleBitwiseAnd(unsigned x, unsigned y) {
  return x & y;
}
constexpr unsigned HandleBitwiseOr(unsigned x, unsigned y) {
  return x | y;
}
constexpr bool Handle(unsigned* dest, unsigned x, unsigned y, Opcode opcode) {
  constexpr auto dispatch_table = LOOKUP_TABLE(ops, "Handle*", [](std::string_view sub) {
    return rfl::enum_cast<Opcode>(rfl::to_all_caps_snake_case(sub));
  });

  static_assert(
      std::is_same_v<unsigned (*)(unsigned, unsigned), decltype(dispatch_table)::value_type>);
  static_assert(dispatch_table.size() == 4);

  auto handle_fn = dispatch_table[opcode];
  if (dest == nullptr || handle_fn == nullptr) {
    return false;
  }
  *dest = handle_fn(x, y);
  return true;
}
}  // namespace ops

TEST(NamespaceLookupTableByName, CustomTransform) {
  auto dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 15, 50, ops::Opcode::ADD));
  EXPECT_EQ(65u, dest);
  dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 15, 50, ops::Opcode::SUB));
  EXPECT_EQ(-35u, dest);
  dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 0b1010u, 0b1100u, ops::Opcode::BITWISE_AND));
  EXPECT_EQ(0b1000u, dest);
  dest = 0u;
  EXPECT_TRUE(ops::Handle(&dest, 0b1010u, 0b1100u, ops::Opcode::BITWISE_OR));
  EXPECT_EQ(0b1110u, dest);

  EXPECT_FALSE_STATIC(ops::Handle(&dest, 1, 2, static_cast<ops::Opcode>(123)));
}
