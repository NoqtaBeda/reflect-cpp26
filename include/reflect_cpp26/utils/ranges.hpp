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

#ifndef REFLECT_CPP26_UTILS_RANGES_HPP
#define REFLECT_CPP26_UTILS_RANGES_HPP

#include <ranges>
#include <type_traits>

namespace reflect_cpp26 {
// -------- Extension of Ranges concepts --------

template <class T>
concept non_range = !std::ranges::range<T>;

#if __cpp_lib_ranges_as_const >= 202207L
#define REFLECT_CPP26_CONST_RANGE_CONCEPTS_FOR_EACH(F) F(constant_range)
#else
#define REFLECT_CPP26_CONST_RANGE_CONCEPTS_FOR_EACH(F)  // No-op
#endif

#define REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH(F) \
  F(range)                                       \
  F(borrowed_range)                              \
  F(sized_range)                                 \
  F(view)                                        \
  F(input_range)                                 \
  F(forward_range)                               \
  F(bidirectional_range)                         \
  F(random_access_range)                         \
  F(contiguous_range)                            \
  F(common_range)                                \
  F(viewable_range)                              \
  REFLECT_CPP26_CONST_RANGE_CONCEPTS_FOR_EACH(F)

#define REFLECT_CPP26_DEFINE_RANGE_CONCEPTS(range_concept)                                       \
  template <class Range, class T>                                                                \
  concept range_concept##_of = std::ranges::range_concept<Range>                                 \
                            && std::is_convertible_v<std::ranges::range_value_t<Range>, T>;      \
                                                                                                 \
  template <class Range, class T>                                                                \
  concept range_concept##_of_exactly =                                                           \
      std::ranges::range_concept<Range> && std::is_same_v<std::ranges::range_value_t<Range>, T>; \
                                                                                                 \
  /* 's' suffix as plural form: 'ranges' and 'views' */                                          \
  template <class... Ranges>                                                                     \
  constexpr auto are_##range_concept##s_v = (std::ranges::range_concept<Ranges> && ...);         \
                                                                                                 \
  template <class T, class... Ranges>                                                            \
  constexpr auto are_##range_concept##s_of_v = (range_concept##_of<Ranges, T> && ...);           \
                                                                                                 \
  template <class T, class... Ranges>                                                            \
  constexpr auto are_##range_concept##s_of_exactly_v =                                           \
      (range_concept##_of_exactly<Ranges, T> && ...);

/**
 * concept ${range_concept}_of<Range, T>
 *   Whether Range is ${range_concept} whose value type is convertible to T.
 * concept ${range_concept}_of_exactly<Range, T>
 *   Whether Range is ${range_concept} whose value type is exactly T.
 * constexpr bool are_${range_concept}s_v<Ranges...>
 * constexpr bool are_${range_concept}s_of_v<Ranges...>
 * constexpr bool are_${range_concept}s_of_exactly_v<Ranges...>
 *
 * ${range_concept} is one of:
 * (1) those listed in REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH above;
 * (2) output_range (whose definition see below).
 */
REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH(REFLECT_CPP26_DEFINE_RANGE_CONCEPTS)

#undef REFLECT_CPP26_DEFINE_RANGE_CONCEPTS
#undef REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH

#define REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH(F) \
  F(borrowed_range)                              \
  F(sized_range)                                 \
  F(view)                                        \
  F(input_range)                                 \
  F(forward_range)                               \
  F(bidirectional_range)                         \
  F(random_access_range)                         \
  F(contiguous_range)                            \
  F(viewable_range)                              \
  REFLECT_CPP26_CONST_RANGE_CONCEPTS_FOR_EACH(F)

#define REFLECT_CPP26_DEFINE_RANGE_CONCEPTS(range_concept)                                       \
  template <class Range, class T>                                                                \
  concept common_##range_concept##_of =                                                          \
      std::ranges::range_concept<Range>                                                          \
      && std::is_convertible_v<std::ranges::range_value_t<Range>, T>                             \
      && std::ranges::common_range<Range>;                                                       \
                                                                                                 \
  template <class Range, class T>                                                                \
  concept common_##range_concept##_of_exactly =                                                  \
      std::ranges::range_concept<Range> && std::is_same_v<std::ranges::range_value_t<Range>, T>  \
      && std::ranges::common_range<Range>;                                                       \
                                                                                                 \
  template <class... Ranges>                                                                     \
  constexpr auto are_common_##range_concept##s_v =                                               \
      (std::ranges::range_concept<Ranges> && ...) && (std::ranges::common_range<Ranges> && ...); \
                                                                                                 \
  template <class T, class... Ranges>                                                            \
  constexpr auto are_common_##range_concept##s_of_v =                                            \
      (common_##range_concept##_of<Ranges, T> && ...);                                           \
                                                                                                 \
  template <class T, class... Ranges>                                                            \
  constexpr auto are_common_##range_concept##s_of_exactly_v =                                    \
      (common_##range_concept##_of_exactly<Ranges, T> && ...);

/**
 * concept common_${range_concept}_of<Range, T>
 * concept common_${range_concept}_of_exactly<Range, T>
 * constexpr bool are_common_${range_concept}s_v<Ranges...>
 * constexpr bool are_common_${range_concept}s_of_v<Ranges...>
 * constexpr bool are_common_${range_concept}s_of_exactly_v<Ranges...>
 *
 * ${range_concept} is one of:
 * (1) those listed in REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH above;
 * (2) output_range (whose definition see below).
 *
 * The prefix "common_" refers to common range, whose iterator and sentinel
 * types are identical (i.e. begin() and end() has the same type).
 */
REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH(REFLECT_CPP26_DEFINE_RANGE_CONCEPTS)

#undef REFLECT_CPP26_DEFINE_RANGE_CONCEPTS
#undef REFLECT_CPP26_RANGE_CONCEPTS_FOR_EACH
#undef REFLECT_CPP26_CONST_RANGE_CONCEPTS_FOR_EACH

// -------- Concepts and constexpr variables with output_range --------

template <class Range, class T>
concept output_range_of = std::ranges::output_range<Range, T>;

template <class Range, class T>
concept output_range_of_exactly =
    std::ranges::output_range<Range, T> && std::is_same_v<std::ranges::range_value_t<Range>, T>;

template <class Range, class T>
concept common_output_range_of = output_range_of<Range, T> && std::ranges::common_range<Range>;

template <class Range, class T>
concept common_output_range_of_exactly =
    output_range_of_exactly<Range, T> && std::ranges::common_range<Range>;

template <class T, class... Ranges>
constexpr auto are_output_ranges_of_v = (output_range_of<Ranges, T> && ...);

template <class T, class... Ranges>
constexpr auto are_output_ranges_of_exactly_v = (output_range_of_exactly<Ranges, T> && ...);

template <class T, class... Ranges>
constexpr auto are_common_output_ranges_of_v = (common_output_range_of<Ranges, T> && ...);

template <class T, class... Ranges>
constexpr auto are_common_output_ranges_of_exactly_v =
    (common_output_range_of_exactly<Ranges, T> && ...);

}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_RANGES_HPP
