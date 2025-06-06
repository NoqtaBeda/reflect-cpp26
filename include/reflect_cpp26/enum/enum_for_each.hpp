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

#ifndef REFLECT_CPP26_ENUM_ENUM_FOR_EACH_HPP
#define REFLECT_CPP26_ENUM_ENUM_FOR_EACH_HPP

#include <reflect_cpp26/enum/enum_values.hpp>

namespace reflect_cpp26 {
/**
 * Performs for-each operation to values of enum type E with given order.
 * Supported signatures:
 *   func(constant<i>, constant<ei>)
 *     where ei is the i-th value as E
 *   func(constant<ei>)
 * See constant::for_each for more details.
 */
template <enum_type E, enum_entry_order Order = enum_entry_order::original,
          class Func>
constexpr auto enum_for_each(Func&& func) -> void {
  enum_value_constants<E, Order>().for_each(std::forward<Func>(func));
}

/**
 * Performs for-each operation to values of enum type E with given order.
 * Supported signatures:
 *   func(constant<i>, constant<meta_ei>)
 *     where meta_ei is the i-th enumerator as std::meta::info
 *   func(constant<meta_ei>)
 * See constant::for_each for more details.
 */
template <enum_type E, enum_entry_order Order = enum_entry_order::original,
          class Func>
constexpr auto enum_meta_for_each(Func&& func) -> void {
  enum_meta_entries<E, Order>().for_each(std::forward<Func>(func));
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_FOR_EACH_HPP
