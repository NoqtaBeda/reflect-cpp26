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

#ifndef REFLECT_CPP26_FIXED_MAP_IMPL_COMMON_HPP
#define REFLECT_CPP26_FIXED_MAP_IMPL_COMMON_HPP

#include <bit>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>

namespace reflect_cpp26::impl {
template <class Value>
constexpr auto map_null_value_v = Value{};

template <class T>
struct alignment_adjusted_wrapper {
  static constexpr auto adjusted_alignment = std::bit_ceil(sizeof(T));
  alignas(adjusted_alignment) T underlying;

  static constexpr auto make(T value) -> alignment_adjusted_wrapper<T> {
    return {.underlying = std::move(value)};
  }

  template <class InputIter>
  static constexpr auto make_static_array(InputIter first, InputIter last)
      -> meta_span<alignment_adjusted_wrapper<T>> {
    auto res = std::vector<alignment_adjusted_wrapper<T>>{};
    for (; first != last; ++first) {
      res.push_back(make(*first));
    }
    return reflect_cpp26::define_static_array(res);
  }

  template <class InputRange>
  static constexpr auto make_static_array(const InputRange& range) {
    auto begin = std::ranges::begin(range);
    auto end = std::ranges::end(range);
    return make_static_array(begin, end);
  }
};

template <class T>
constexpr decltype(auto) unwrap(T&& value) {
  return std::forward<T>(value);  // End of recursion
}

template <class T>
constexpr decltype(auto) unwrap_once(T&& value) {
  return std::forward<T>(value);  // End of recursion
}

template <class T>
constexpr auto unwrap(const alignment_adjusted_wrapper<T>& wrapper) -> const T& {
  return unwrap(wrapper.underlying);
}

template <class T>
constexpr auto unwrap(alignment_adjusted_wrapper<T>&& wrapper) -> T {
  return unwrap(std::move(wrapper.underlying));
}

template <class T>
constexpr auto unwrap_once(const alignment_adjusted_wrapper<T>& wrapper) -> const T& {
  return wrapper.underlying;
}

template <class T>
constexpr auto unwrap_once(alignment_adjusted_wrapper<T>&& wrapper) -> T {
  return std::move(wrapper.underlying);
}
}  // namespace reflect_cpp26::impl

#endif  // REFLECT_CPP26_FIXED_MAP_IMPL_COMMON_HPP
