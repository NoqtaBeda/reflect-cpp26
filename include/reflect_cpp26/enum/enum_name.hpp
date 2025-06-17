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

#ifndef REFLECT_CPP26_ENUM_ENUM_NAME_HPP
#define REFLECT_CPP26_ENUM_ENUM_NAME_HPP

#include <reflect_cpp26/enum/impl/enum_maps.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26 {
struct enum_name_t {
  template <enum_type E>
  static constexpr auto operator()(E value) -> std::string_view {
    const auto& [name, found] = impl::enum_name_map_v<E>.get(impl::to_int64_or_uint64(value));
    return found ? static_cast<std::string_view>(name) : std::string_view{};
  }
};

struct enum_name_opt_t {
  template <enum_type E>
  static constexpr auto operator()(E value) -> std::optional<std::string_view> {
    const auto& [name, found] = impl::enum_name_map_v<E>.get(impl::to_int64_or_uint64(value));
    if (found) {
      return static_cast<std::string_view>(name);
    }
    return std::nullopt;
  }
};

constexpr auto enum_name = enum_name_t{};
constexpr auto enum_name_opt = enum_name_opt_t{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_ENUM_ENUM_NAME_HPP
