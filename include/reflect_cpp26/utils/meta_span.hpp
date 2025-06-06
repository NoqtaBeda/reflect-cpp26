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

#ifndef REFLECT_CPP26_UTILS_META_SPAN_HPP
#define REFLECT_CPP26_UTILS_META_SPAN_HPP

#include <cstddef>
#include <iterator>
#include <span>

namespace reflect_cpp26 {
/**
 * Structured alternative to std::span<const T>.
 * Semantic constraints: meta_span<T> is used for contiguous ranges
 * with static constant storage only.
 */
template <class T>
struct meta_span {
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = const_pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<const_iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  const T* head = nullptr;
  const T* tail = nullptr;

  constexpr meta_span() = default;

  template <size_t N>
  static consteval auto from_array(const T (&arr)[N]) -> meta_span
  {
    auto res = meta_span{};
    res.head = arr;
    res.tail = arr + N;
    return res;
  }

  template <size_t N>
  static consteval auto from_array(const std::array<T, N>& arr) -> meta_span
  {
    auto res = meta_span{};
    res.head = arr.data();
    res.tail = arr.data() + N;
    return res;
  }

  static consteval auto from_std_span(std::span<const T> span) -> meta_span
  {
    auto res = meta_span{};
    res.head = span.data();
    res.tail = span.data() + span.size();
    return res;
  }

  constexpr operator std::span<const T>() const {
    return {head, tail};
  }

  constexpr auto operator[](size_t index) const -> const T& {
    return head[index];
  }

  constexpr auto front() const -> const T& {
    return head[0];
  }

  constexpr auto back() const -> const T& {
    return tail[-1];
  }

  constexpr auto size() const -> size_t {
    return tail - head;
  }

  constexpr auto empty() const -> bool {
    return tail == head;
  }

  constexpr auto data() const -> const T* {
    return head;
  }

  constexpr auto begin() const -> const T* {
    return head;
  }

  constexpr auto end() const -> const T* {
    return tail;
  }

  constexpr auto first(size_t n) const -> meta_span {
    return subspan(0, n);
  }

  constexpr auto last(size_t n) const -> meta_span {
    return subspan(size() - n);
  }

  constexpr auto subspan(
    size_t offset, size_t count = std::dynamic_extent) const -> meta_span
  {
    auto res = meta_span{};
    res.head = this->head + offset;
    if (count == std::dynamic_extent) {
      res.tail = this->tail;
    } else {
      res.tail = res.head + count;
    }
    return res;
  }
};
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_META_SPAN_HPP
