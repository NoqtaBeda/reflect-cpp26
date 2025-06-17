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

#ifndef REFLECT_CPP26_ENUM_ENUM_CONTAINS_HPP
#define REFLECT_CPP26_ENUM_ENUM_CONTAINS_HPP

#include <reflect_cpp26/enum/impl/enum_maps.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/tags.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26 {
template <enum_type E>
struct enum_contains_t {
private:
  using ENoCV = std::remove_cv_t<E>;
  using EUnderlying = std::underlying_type_t<ENoCV>;

public:
  static constexpr bool operator()(ENoCV value) {
    auto key = impl::to_int64_or_uint64(value);
    return impl::enum_name_map_v<E>.get(key).second;
  }

  static constexpr bool operator()(std::integral auto value) {
    if (!in_range<EUnderlying>(value)) {
      return false;
    }
    auto value_as_enum = static_cast<ENoCV>(value);
    return impl::enum_name_map_v<E>.get(impl::to_int64_or_uint64(value_as_enum)).second;
  }

  static constexpr bool operator()(std::string_view str) {
    return impl::enum_from_string_map_v<E>.get(str).second;
  }

  static constexpr bool operator()(ascii_case_insensitive_tag_t, std::string_view str) {
    return impl::enum_from_ci_string_map_v<E>.get(str).second;
  }
};

template <enum_type E>
constexpr auto enum_contains = enum_contains_t<std::remove_cv_t<E>>{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_ENUM_ENUM_CONTAINS_HPP
