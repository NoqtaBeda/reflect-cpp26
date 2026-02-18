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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_TO_structural_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_TO_structural_HPP

#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/type_traits/structural_types.hpp>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>

namespace reflect_cpp26 {
struct to_structural_t {
  template <class T>
  static consteval auto operator()(const T& value);

  template <class T>
  static consteval auto operator()(std::initializer_list<T> values);
};

constexpr auto to_structural = to_structural_t{};

namespace impl {
template <class InputRange>
consteval auto range_to_structural(const InputRange& range) {
  using ValueT = std::ranges::range_value_t<InputRange>;
  if constexpr (char_type<ValueT>) {
    return reflect_cpp26::define_static_string(range);
  } else {
    auto converted =
        range | std::views::transform([](const auto& elem) { return to_structural(elem); });
    return reflect_cpp26::define_static_array(converted);
  }
}

template <size_t I, class TupleLike>
consteval auto tuple_like_ith_to_structural(const TupleLike& tuple) {
  return to_structural(get<I>(tuple));
}

template <size_t... Is, class TupleLike>
consteval auto tuple_like_to_structural(const TupleLike& tuple, std::index_sequence<Is...>) {
  return meta_tuple{tuple_like_ith_to_structural<Is>(tuple)...};
}

template <class TupleLike>
consteval auto tuple_like_to_structural(const TupleLike& tuple) {
  constexpr auto N = std::tuple_size_v<TupleLike>;
  return tuple_like_to_structural(tuple, std::make_index_sequence<N>{});
}

template <class Pointer>
  requires(std::is_pointer_v<Pointer>)
consteval auto pointer_to_structural(Pointer ptr) {
  using T = std::remove_pointer_t<Pointer>;
  if constexpr (char_type<T>) {
    auto tail = std::ranges::find(ptr, std::unreachable_sentinel, '\0');
    return reflect_cpp26::define_static_string(std::basic_string_view{ptr, tail});
  } else {
    return ptr;
  }
}

consteval auto pointer_to_structural(std::nullptr_t) {
  return nullptr;
}
}  // namespace impl

template <class T>
consteval auto to_structural_t::operator()(const T& value) {
  if constexpr (std::ranges::input_range<T>) {
    return impl::range_to_structural(value);
  } else if constexpr (tuple_like<T>) {
    return impl::tuple_like_to_structural(value);
  } else if constexpr (std::is_pointer_v<T> || std::is_null_pointer_v<T>) {
    return impl::pointer_to_structural(value);
  } else if constexpr (std::is_function_v<T>) {
    return static_cast<std::add_pointer_t<T>>(value);
  } else {
    static_assert(structural_type<T>, "T is not structural type.");
    return value;
  }
}

template <class T>
consteval auto to_structural_t::operator()(std::initializer_list<T> values) {
  return impl::range_to_structural(values);
}

template <class T>
using to_structural_result_t = decltype(to_structural(std::declval<T>()));
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_OPERATIONS_TO_structural_HPP
