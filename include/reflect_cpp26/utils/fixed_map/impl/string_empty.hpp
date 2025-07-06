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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_EMPTY_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_EMPTY_HPP

#include <reflect_cpp26/utils/fixed_map/impl/common.hpp>

namespace reflect_cpp26::impl {
template <class KVPair>
struct empty_string_key_map {
  using key_type = std::tuple_element_t<0, KVPair>;
  using value_type = std::tuple_element_t<1, KVPair>;
  using result_type = const value_type&;
  using character_type = char_type_t<key_type>;

  static constexpr size_t size() {
    return 0;
  }
  static constexpr auto get(std::basic_string_view<character_type>)
    -> std::pair<result_type, bool> {
    return {map_null_value_v<value_type>, false};
  }

  static constexpr auto operator[](std::basic_string_view<character_type>)
    -> const value_type& {
    return map_null_value_v<value_type>;
  }
};

// -------- Builder --------

template <class KVPair>
consteval auto make_empty_string_key_map() -> std::meta::info {
  return std::meta::reflect_constant(empty_string_key_map<KVPair>{});
}
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_STRING_EMPTY_HPP
