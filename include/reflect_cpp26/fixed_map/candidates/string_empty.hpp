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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_EMPTY_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_EMPTY_HPP

#include <reflect_cpp26/fixed_map/impl/common.hpp>
#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>

namespace reflect_cpp26::impl::map {
template <class CharT, class V>
struct empty_with_skey {
  using key_type = meta_basic_string_view<CharT>;
  using value_type = V;

private:
  using result_type = std::pair<const V&, bool>;

public:
  static constexpr auto size() -> size_t {
    return 0;
  }

  constexpr auto get(std::basic_string_view<CharT>) const -> result_type {
    return {default_v<value_type>, false};
  }

  constexpr auto operator[](std::basic_string_view<CharT>) const -> const value_type& {
    return default_v<value_type>;
  }
};

// -------- Builder --------

template <class CharT, class V>
consteval auto make_empty_with_skey() -> std::meta::info {
  auto obj = empty_with_skey<CharT, V>{};
  return std::meta::reflect_constant(obj);
}
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_EMPTY_HPP
