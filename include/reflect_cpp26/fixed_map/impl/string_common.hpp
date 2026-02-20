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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_STRING_COMMON_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_STRING_COMMON_HPP

#include <reflect_cpp26/fixed_map/impl/common.hpp>
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>

#define REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE                                        \
  constexpr auto operator[](std::basic_string_view<character_type> key) const->result_type { \
    return get(key).first;                                                                   \
  }

namespace reflect_cpp26::impl {
template <class KVPair>
concept string_key_kv_pair = pair_like<KVPair> && string_like<std::tuple_element_t<0, KVPair>>;

template <class T>
struct string_hash_wrapper {
  uint64_t hash;
  T underlying;
};

template <class T>
using alignment_adjusted_string_hash_wrapper = alignment_adjusted_wrapper<string_hash_wrapper<T>>;
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_STRING_COMMON_HPP
