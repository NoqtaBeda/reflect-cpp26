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

#ifndef REFLECT_CPP26_UTILS_DEFINE_STATIC_VALUES_HPP
#define REFLECT_CPP26_UTILS_DEFINE_STATIC_VALUES_HPP

#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/utils/config.h>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_string_view.hpp>
#include <iterator>
#include <ranges>

namespace reflect_cpp26 {
namespace impl {
template <auto V>
using meta_string_view_type_t = meta_basic_string_view<
  std::ranges::range_value_t<decltype(V)>>;

template <auto V>
constexpr auto array_to_string_view_v =
  meta_string_view_type_t<V>::from_array(V);

template <size_t N, class T, class Range>
consteval auto define_static_string_impl(Range& range)
  -> meta_basic_string_view<T>
{
  auto arr = std::array<T, N + 1>{};
  std::ranges::copy(range, arr.begin());
  arr.back() = '\0';
  auto sv = extract<meta_basic_string_view<T>>(
    substitute(^^array_to_string_view_v, { std::meta::reflect_constant(arr) }));
  return sv;
}

template <class BidirRange>
consteval auto try_remove_trailing_null_characters(
  const BidirRange& range, bool removes_trailing_null_characters)
{
  if (!removes_trailing_null_characters) {
    return std::ranges::subrange{range};
  }
  auto start_pos = std::ranges::begin(range);
  auto end_pos = std::ranges::end(range);
  for (; start_pos != end_pos; ) {
    auto prev = std::ranges::prev(end_pos);
    if (*prev != '\0') {
      break; // end_pos stops at the first '\0'
    }
    end_pos = prev;
  }
  return std::ranges::subrange{start_pos, end_pos};
}
} // namespace impl

/**
 * Alternative to C++26 std::meta::define_static_array.
 */
template <std::ranges::input_range Range>
consteval auto define_static_array(Range&& range) /* -> meta_span<T> */
{
  using T = std::ranges::range_value_t<Range>;
  auto span = std::define_static_array(std::forward<Range>(range));
  return meta_span<T>::from_std_span(span);
}

// Specialization to prevent repeated meta-definition.
template <class T>
consteval auto define_static_array(meta_span<T> range) {
  return range;
}

/**
 * Alternative to C++26 std::meta::define_static_string.
 * It's guaranteed that the resulted meta_string_view is null-terminated,
 * i.e. *end() == '\0'.
 */
template <std::ranges::input_range Range>
  requires (is_char_type_v<std::ranges::range_value_t<Range>>)
consteval auto define_static_string(
  Range&& range, bool removes_trailing_null_characters = false)
  /* -> meta_basic_string_view<T> */
{
  using T = std::ranges::range_value_t<Range>;
  using ResultT = meta_basic_string_view<T>;

  // If it's an array, check if it's a string literal and adjust accordingly
  if constexpr (requires { is_string_literal(range); }) {
    if (is_string_literal(range)) {
      return define_static_string(std::basic_string_view{range});
    }
  }
  if constexpr (std::ranges::bidirectional_range<Range>) {
    auto subrange = impl::try_remove_trailing_null_characters(
      range, removes_trailing_null_characters);

    using SubrangeT = decltype(subrange);
    using ImplFnSignature = ResultT(*)(SubrangeT&);
    auto N = std::meta::reflect_constant(std::ranges::distance(subrange));
    auto impl_fn = extract<ImplFnSignature>(
      substitute(^^impl::define_static_string_impl, {N, ^^T, ^^SubrangeT}));
    return impl_fn(subrange);
  } else {
    auto vec = range | std::ranges::to<std::vector>();
    return define_static_string(vec, removes_trailing_null_characters);
  }
}

// Specialization to prevent repeated meta-definition.
template <class T>
consteval auto define_static_string(
  meta_basic_string_view<T> range,
  bool removes_trailing_null_characters = false) -> meta_basic_string_view<T>
{
  // Makes sure the resulted range is null-terminated
  if (*range.end() != '\0') {
    auto sv = static_cast<std::basic_string_view<T>>(range);
    return define_static_string(sv);
  }
  if (!removes_trailing_null_characters) {
    return range;
  }
  auto [head, tail] = range;
  for (; head < tail && tail[-1] == '\0'; --tail) {}
  return {head, tail};
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_DEFINE_STATIC_VALUES_HPP
