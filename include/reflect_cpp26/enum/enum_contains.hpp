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

#ifndef REFLECT_CPP26_ENUM_ENUM_CONTAINS_HPP
#define REFLECT_CPP26_ENUM_ENUM_CONTAINS_HPP

#include <reflect_cpp26/enum/impl/enum_maps.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26 {
/**
 * Whether value is an entry of enum type E.
 */
template <enum_type E>
constexpr bool enum_contains(E value) {
  return impl::enum_name_map_v<E>.get(impl::to_int64_or_uint64(value)).second;
}

/**
 * Whether an entry of enum type E with given underlying value exists.
 */
template <enum_type E, std::integral I>
constexpr bool enum_contains(I value)
{
  using ENoCV = std::remove_cv_t<E>;
  using UnderlyingT = std::underlying_type_t<ENoCV>;
  if (!in_range<UnderlyingT>(value)) {
    return false;
  }
  auto value_as_enum = static_cast<ENoCV>(value);
  return impl::enum_name_map_v<E>.get(
    impl::to_int64_or_uint64(value_as_enum)).second;
}

/**
 * Whether an entry of enum type E with given name exists.
 */
template <enum_type E>
constexpr bool enum_contains(std::string_view str) {
  return impl::enum_from_string_map_v<E>.get(str).second;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_CONTAINS_HPP
