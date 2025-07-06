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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_COMMON_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_COMMON_HPP

#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/type_traits/string_like_types.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <bit>
#include <utility>

#define REFLECT_CPP26_INTEGRAL_KEY_MAP_COMMON_INTERFACE                 \
  constexpr auto get(non_bool_integral auto key) const                  \
    -> std::pair<result_type, bool>                                     \
  {                                                                     \
    if (!in_range<key_type>(key)) {                                     \
      return {map_null_value_v<value_type>, false};                     \
    }                                                                   \
    return get(static_cast<key_type>(key));                             \
  }                                                                     \
                                                                        \
  constexpr result_type operator[](non_bool_integral auto key) const {  \
    return get(key).first;                                              \
  }

#define REFLECT_CPP26_STRING_KEY_MAP_COMMON_INTERFACE                         \
  constexpr auto operator[](std::basic_string_view<character_type> key) const \
    -> result_type {                                                          \
    return get(key).first;                                                    \
  }

namespace reflect_cpp26::impl {
template <class Value>
constexpr auto map_null_value_v = Value{};

template <class KVPair>
concept integral_key_kv_pair = pair_like<KVPair> &&
  std::is_integral_v<std::tuple_element_t<0, std::remove_cvref_t<KVPair>>>;

template <class KVPair>
concept string_key_kv_pair = pair_like<KVPair> &&
  is_string_like_v<std::tuple_element_t<0, std::remove_cvref_t<KVPair>>>;

template <class T>
struct alignment_adjusted_wrapper {
  static constexpr auto adjusted_alignment = std::bit_ceil(sizeof(T));
  alignas(adjusted_alignment) T underlying;

  static constexpr auto make(T value) -> alignment_adjusted_wrapper<T> {
    return {.underlying = std::move(value)};
  }

  template <class InputIter>
  static constexpr auto make_static_array(InputIter first, InputIter last)
    -> meta_span<alignment_adjusted_wrapper<T>>
  {
    auto res = std::vector<alignment_adjusted_wrapper<T>>{};
    for (; first != last; ++first) {
      res.push_back(make(*first));
    }
    return reflect_cpp26::define_static_array(res);
  }
};

template <class KVPair>
struct string_key_kv_pair_wrapper_with_hash {
  uint64_t hash;
  KVPair underlying;
};

template <class KVPair>
using string_key_kv_pair_aligned_wrapper_with_hash =
  alignment_adjusted_wrapper<string_key_kv_pair_wrapper_with_hash<KVPair>>;

namespace flat_map {
constexpr bool do_is_null(std::integral auto v) {
  return v == 0; // value initialization result is 0
}

constexpr bool do_is_null(std::floating_point auto v) {
  return v == 0.0; // value initialization result is 0.0
}

constexpr bool do_is_null(pointer_type auto v) {
  return v == nullptr;
}

template <class T, class C>
constexpr bool do_is_null(T C::* v) {
  return v == nullptr;
}

template <char_type CharT, class Traits>
constexpr bool do_is_null(std::basic_string_view<CharT, Traits> v) {
  return v.data() == nullptr;
}

template <char_type CharT>
constexpr bool do_is_null(meta_basic_string_view<CharT> v) {
  return v.head == nullptr;
}

constexpr bool do_is_null(const string_key_kv_pair auto& kv_pair) {
  return do_is_null(get_first(kv_pair));
}

template <class T>
concept is_null_defined_via_adl = requires (const T& v) {
  { is_null(v) } -> std::same_as<bool>;
};

template <is_null_defined_via_adl T>
constexpr bool do_is_null(const T& v) {
  return is_null(v);
}
} // namespace flat_map

template <class T>
constexpr bool is_null(const T& v) {
  return flat_map::do_is_null(v);
}

template <auto R>
constexpr auto extract_fixed_map_v = compile_error("Invalid result type.");

#define REFLECT_CPP26_EXTRACT_FIXED_MAP(T)    \
  template <auto R>                           \
    requires (std::is_same_v<decltype(R), T>) \
  constexpr auto extract_fixed_map_v<R>

REFLECT_CPP26_EXTRACT_FIXED_MAP(std::meta::info) = extract<R>();
} // namespace reflect_cpp26::impl

#endif // REFLECT_CPP26_UTILS_FIXED_MAP_IMPL_COMMON_HPP
