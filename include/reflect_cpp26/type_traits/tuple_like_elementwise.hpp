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

#ifndef REFLECT_CPP26_TYPE_TRAITS_TUPLE_LIKE_ELEMENTWISE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_TUPLE_LIKE_ELEMENTWISE_HPP

#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval auto min_tuple_size(std::initializer_list<std::meta::info> Ts)
{
  auto transform_fn = [](std::meta::info T) {
    return extract<size_t>(substitute(^^std::tuple_size_v, {T}));
  };
  return std::ranges::min(Ts | std::views::transform(transform_fn));
}

struct reduce_elementwise_result_t {
  size_t true_count;
  size_t false_count;
};

template <template <class...> class Pred, class... Tuples>
consteval auto reduce_elementwise() -> reduce_elementwise_result_t
{
  constexpr auto min_size = min_tuple_size({^^Tuples...});
  auto true_count = 0zU;
  auto false_count = 0zU;

  template for (constexpr auto I: std::views::iota(0zU, min_size)) {
    constexpr auto ith_elements =
      std::array{^^std::tuple_element_t<I, Tuples>...};
    constexpr auto cur_result = [:substitute(^^Pred, ith_elements):]::value;
    (cur_result ? true_count : false_count) += 1;
  }
  return {true_count, false_count};
}

template <template <class...> class Pred, class... Tuples>
constexpr auto reduce_elementwise_v = reduce_elementwise<Pred, Tuples...>();

consteval auto elementwise_zip_substitute(
  std::meta::info Transform, std::initializer_list<std::meta::info> Tuples)
{
  auto min_size = min_tuple_size(Tuples);
  auto n = std::ranges::size(Tuples);
  auto results = make_reserved_vector<std::meta::info>(min_size);

  for (auto i = 0zU; i < min_size; i++) {
    auto I = std::meta::reflect_constant(i);
    auto ith_elements = make_reserved_vector<std::meta::info>(n);
    for (auto T: Tuples) {
      ith_elements.push_back(substitute(^^std::tuple_element_t, I, T));
    }
    results.push_back(substitute(Transform, ith_elements));
  }
  return substitute(^^type_tuple, results);
}

template <template <class...> class Transform, class... Tuples>
using elementwise_zip_substituted =
  [: elementwise_zip_substitute(^^Transform, {^^Tuples...}) :];

template <class Traits>
struct extract_traits_type {
  using type = typename Traits::type;
};

template <class Traits>
struct extract_traits_value {
  static constexpr auto value = Traits::value;
};
} // namespace impl

/**
 * Whether Predicate<
 *   tuple_element_t<j, Tuples...[0]>, ...,
 *   tuple_element_t<j, Tuples...[K - 1]>>::value is false
 * for every j = 0 to N - 1 where
 *   N = min(tuple_size_v<Tuples>...), K = sizeof...(Tuples)
 */
template <template <class...> class Predicate, tuple_like... Tuples>
constexpr auto all_of_elementwise_v =
  impl::reduce_elementwise_v<Predicate, std::remove_cvref_t<Tuples>...>
    .false_count == 0;

/**
 * Whether Predicate<
 *   tuple_element_t<j, Tuples...[0]>, ...,
 *   tuple_element_t<j, Tuples...[K - 1]>>::value is false
 * for every j = 0 to N - 1 where
 *   N = min(tuple_size_v<Tuples>...), K = sizeof...(Tuples)
 */
template <template <class...> class Predicate, tuple_like... Tuples>
constexpr auto any_of_elementwise_v =
  impl::reduce_elementwise_v<Predicate, std::remove_cvref_t<Tuples>...>
    .true_count != 0;

/**
 * Whether Predicate<
 *   tuple_element_t<j, Tuples...[0]>, ...,
 *   tuple_element_t<j, Tuples...[K - 1]>>::value is false
 * for every j = 0 to N - 1 where
 *   N = min(tuple_size_v<Tuples>...), K = sizeof...(Tuples)
 */
template <template <class...> class Predicate, tuple_like... Tuples>
constexpr auto none_of_elementwise_v =
  impl::reduce_elementwise_v<Predicate, std::remove_cvref_t<Tuples>...>
    .true_count == 0;

/**
 * Makes a type_tuple<T0, ..., Tn> where Tj = Template<
 *   tuple_element_t<j, Tuples...[0]>, ...,
 *   tuple_element_t<j, Tuples...[K - 1]>>
 * where n = min(tuple_size_v<Tuples...[i]>) - 1, K = sizeof...(Tuples).
 */
template <template <class...> class Template, tuple_like T, tuple_like... Ts>
using elementwise_zip_substitute_t = impl::elementwise_zip_substituted<
  Template, std::remove_cvref_t<T>, std::remove_cvref_t<Ts>...>;

/**
 * Makes a type_tuple<T0, ..., Tn> where Tj = Transform<
 *   tuple_element_t<j, Tuples...[0]>, ...,
 *   tuple_element_t<j, Tuples...[K - 1]>>::type
 * where n = min(tuple_size_v<Tuples...[i]>) - 1, K = sizeof...(Tuples).
 */
template <template <class...> class Transform, tuple_like T, tuple_like... Ts>
using elementwise_zip_transform_t =
  type_tuple_transform_t<impl::extract_traits_type,
    impl::elementwise_zip_substituted<
      Transform, std::remove_cvref_t<T>, std::remove_cvref_t<Ts>...>>;

/**
 * Makes a constant<V0, ..., Vn> where Vj = Transform<
 *   tuple_element_t<j, Tuples...[0]>, ...,
 *   tuple_element_t<j, Tuples...[K - 1]>>::value
 * where n = min(tuple_size_v<Tuples...[i]>) - 1, K = sizeof...(Tuples)
 */
template <template <class...> class Transform, tuple_like T, tuple_like... Ts>
constexpr auto elementwise_zip_transform_v =
  type_tuple_transform_v<impl::extract_traits_value,
    impl::elementwise_zip_substituted<
      Transform, std::remove_cvref_t<T>, std::remove_cvref_t<Ts>...>>;
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_TUPLE_LIKE_ELEMENTWISE_HPP
