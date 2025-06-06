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

#ifndef REFLECT_CPP26_ENUM_ENUM_HASH_HPP
#define REFLECT_CPP26_ENUM_ENUM_HASH_HPP

#include <reflect_cpp26/enum/enum_json.hpp>
#include <reflect_cpp26/utils/string_hash.hpp>

namespace reflect_cpp26 {
namespace impl {
template <class E>
constexpr auto enum_hash()
{
  constexpr auto Order = enum_entry_order::by_name;
  auto repr_str = reflect_cpp26::enum_json<E, Order>();
  return bkdr_hash64(repr_str);
}

template <class E>
constexpr auto enum_hash_v = enum_hash<E>();
} // namespace impl

/**
 * Gets the 64-bit hash value of enum type E.
 * Any change to the entry set (including insertion, deletion and
 * modification to name or value) will change the hash value.
 * But only changing entry order will not.
 */
template <enum_type E>
constexpr auto enum_hash() -> uint64_t {
  return impl::enum_hash_v<std::remove_cv_t<E>>;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_HASH_HPP
