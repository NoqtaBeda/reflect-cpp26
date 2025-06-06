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

#ifndef REFLECT_CPP26_ENUM_IMPL_HASH_COLLISION_CHECK_HPP
#define REFLECT_CPP26_ENUM_IMPL_HASH_COLLISION_CHECK_HPP

#include <reflect_cpp26/enum/enum_for_each.hpp>
#include <reflect_cpp26/utils/string_hash.hpp>
#include <algorithm>

namespace reflect_cpp26::impl {
template <class E>
consteval bool enum_name_has_hash_collision()
{
  if constexpr (enum_count<E>() != 0) {
    auto hash_values = enum_meta_entries<E>().map([](auto ec) {
      return bkdr_hash64(identifier_of(ec.value));
    });
    std::ranges::sort(hash_values);
    return hash_values[0] == 0
      || std::ranges::adjacent_find(hash_values) != hash_values.end();
  } else {
    return false;
  }
}

// Hash collision if either happens:
// (1) Hash value is zero in some entries;
// (2) Multiple entries share the same hash value.
template <class E>
constexpr auto enum_name_has_hash_collision_v =
  enum_name_has_hash_collision<E>();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_ENUM_IMPL_HASH_COLLISION_CHECK_HPP
