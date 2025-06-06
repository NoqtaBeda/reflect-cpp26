/**
 * Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
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

#include "tests/enum/test_cases.hpp"
#include "tests/test_options.hpp"
#include <reflect_cpp26/enum/impl/hash_collision_check.hpp>

using reflect_cpp26::impl::enum_name_has_hash_collision_v;

static_assert(! enum_name_has_hash_collision_v<foo_signed>);
static_assert(! enum_name_has_hash_collision_v<foo_signed_reorder>);
static_assert(! enum_name_has_hash_collision_v<foo_signed_rep>);
static_assert(! enum_name_has_hash_collision_v<bar_unsigned>);
static_assert(! enum_name_has_hash_collision_v<baz_signed>);
static_assert(! enum_name_has_hash_collision_v<qux_unsigned>);
static_assert(! enum_name_has_hash_collision_v<empty>);
static_assert(! enum_name_has_hash_collision_v<single>);
static_assert(! enum_name_has_hash_collision_v<single_rep>);
static_assert(! enum_name_has_hash_collision_v<color>);
static_assert(! enum_name_has_hash_collision_v<terminal_color>);
static_assert(enum_name_has_hash_collision_v<hash_collision>);

TEST(EnumImpl, HashCollisionCheck) {
  EXPECT_TRUE(true); // All test cases done by static assertions above.
}
