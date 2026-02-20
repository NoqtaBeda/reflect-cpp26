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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_COMMON_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_COMMON_HPP

#include <reflect_cpp26/fixed_map/impl/common.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>

#define REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE                                  \
  constexpr auto get(non_bool_integral auto key) const -> std::pair<result_type, bool> { \
    if (!in_range<key_type>(key)) {                                                      \
      return {map_null_value_v<value_type>, false};                                      \
    }                                                                                    \
    return get(static_cast<key_type>(key));                                              \
  }                                                                                      \
                                                                                         \
  constexpr result_type operator[](non_bool_integral auto key) const {                   \
    return get(key).first;                                                               \
  }

namespace reflect_cpp26::impl {
template <class KVPair>
concept integral_key_kv_pair =
    pair_like<KVPair> && integral_or_enum_type<std::tuple_element_t<0, KVPair>>;

template <class T>
struct validity_wrapper {
  bool is_valid;
  T underlying;
};

template <class T>
constexpr auto unwrap(const validity_wrapper<T>& wrapper) -> const T& {
  return unwrap(wrapper.underlying);
}

template <class T>
constexpr auto unwrap(validity_wrapper<T>&& wrapper) -> T {
  return unwrap(std::move(wrapper.underlying));
}

template <class T>
using alignment_adjusted_validity_wrapper = alignment_adjusted_wrapper<validity_wrapper<T>>;
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_INTEGRAL_COMMON_HPP
