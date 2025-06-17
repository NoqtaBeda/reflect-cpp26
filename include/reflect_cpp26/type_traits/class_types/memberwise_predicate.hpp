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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_MEMBERWISE_PREDICATE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_MEMBERWISE_PREDICATE_HPP

#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#include <reflect_cpp26/type_traits/function_types.hpp>
#include <reflect_cpp26/type_traits/is_invocable.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/expand.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>

namespace reflect_cpp26 {
namespace impl {
struct reduce_memberwise_result_t {
  size_t true_count;
  size_t false_count;
};

template <template <class...> class Predicate, class... Ts>
consteval auto reduce_direct_memberwise()
{
  auto true_count = 0zU;
  auto false_count = 0zU;
  constexpr auto min_member_size = std::ranges::min(
    {public_direct_nsdm_of(^^Ts).size()...});

  auto ith_nsdm_type = [](std::meta::info T, size_t I) {
    return type_of(public_direct_nsdm_of(T)[I]);
  };
  REFLECT_CPP26_EXPAND_I(min_member_size).for_each(
    [&true_count, &false_count, ith_nsdm_type](auto I) {
      constexpr auto cur_pred_meta = 
        substitute(^^Predicate, {ith_nsdm_type(^^Ts, I)...});
      using CurPred = [:cur_pred_meta:];
      (CurPred::value ? true_count : false_count) += 1;
    });
  return reduce_memberwise_result_t{true_count, false_count};
}

template <template <class...> class Predicate, class... Ts>
constexpr auto reduce_direct_memberwise_v =
  reduce_direct_memberwise<Predicate, std::remove_cv_t<Ts>...>();

template <class Arg, size_t N, invocable_r_with_n<bool, Arg, N> Predicate>
consteval auto reduce_memberwise_meta(
  const Predicate& pred, const std::array<std::vector<Arg>, N>& nsdm)
{
  auto true_count = 0zU;
  auto false_count = 0zU;
  auto min_member_size = std::ranges::min(
    nsdm | std::views::transform(std::ranges::size));

  constexpr auto indices = REFLECT_CPP26_EXPAND_I(N);
  for (auto i = 0zU; i < min_member_size; i++) {
    auto cur_args = indices.map([nsdm, i](size_t j) {
      return nsdm[j][i];
    });
    auto cur_res = std::apply(pred, cur_args);
    cur_res ? (true_count += 1) : (false_count += 1);
  }
  return reduce_memberwise_result_t{true_count, false_count};
}

template <class Predicate, class... Ts>
consteval auto reduce_direct_memberwise_meta()
{
  auto nsdm = std::array{public_direct_nsdm_of(^^Ts)...};
  return reduce_memberwise_meta(Predicate{}, nsdm);
}

template <class Predicate, class... Ts>
constexpr auto reduce_direct_memberwise_meta_v =
  reduce_direct_memberwise_meta<Predicate, std::remove_cv_t<Ts>...>();

template <template <class...> class Predicate, class... Ts>
consteval auto reduce_flattened_memberwise()
{
  auto true_count = 0zU;
  auto false_count = 0zU;
  constexpr auto min_member_size = std::min({
    public_flattened_nsdm_v<Ts>.size()...});

  auto ith_nsdm_type = []<class T>(std::type_identity<T>, auto I) {
    return type_of(get<I>(public_flattened_nsdm_v<T>).member);
  };
  REFLECT_CPP26_EXPAND_I(min_member_size).for_each(
    [&true_count, &false_count, ith_nsdm_type](auto I) {
      constexpr auto cur_pred_meta = substitute(
        ^^Predicate, {ith_nsdm_type(std::type_identity<Ts>(), I)...});
      using CurPred = [:cur_pred_meta:];
      (CurPred::value ? true_count : false_count) += 1;
    });
  return reduce_memberwise_result_t{true_count, false_count};
}

template <template <class...> class Predicate, class... Ts>
constexpr auto reduce_flattened_memberwise_v =
  reduce_flattened_memberwise<Predicate, std::remove_cv_t<Ts>...>();

template <class Predicate, class... Ts>
consteval auto reduce_flattened_memberwise_meta()
{
  // predicate(flattened_data_member_spec...) -> bool
  constexpr auto with_specs = is_invocable_r_n_v<
    bool, Predicate, flattened_data_member_spec, sizeof...(Ts)>;
  // predicate(std::meta::info...) -> bool
  constexpr auto with_info = is_invocable_r_n_v<
    bool, Predicate, std::meta::info, sizeof...(Ts)>;

  if constexpr (with_specs) {
    auto nsdm = std::array{public_flattened_nsdm_v<Ts>.to_vector()...};
    return reduce_memberwise_meta(Predicate{}, nsdm);
  } else {
    static_assert(with_info, "Invalid call signature of predicate.");
    auto nsdm = std::array{
      public_flattened_nsdm_v<Ts>.to_members().to_vector()...};
    return reduce_memberwise_meta(Predicate{}, nsdm);
  }
}

template <class Predicate, class... Ts>
constexpr auto reduce_flattened_memberwise_meta_v =
  reduce_flattened_memberwise_meta<Predicate, std::remove_cv_t<Ts>...>();
} // namespace impl

#define REFLECT_CPP26_MEMBERWISE_REDUCTION_V_LIST(F)  \
  F(all, false_count == 0)                            \
  F(any, true_count > 0)                              \
  F(none, true_count == 0)

#define REFLECT_CPP26_MAKE_DIRECT_MEMBERWISE_REDUCTION_V(reduction, pred) \
  template <template <class...> class Predicate, class_type... Ts>        \
  constexpr auto reduction##_of_direct_memberwise_v =                     \
    impl::reduce_direct_memberwise_v<Predicate, Ts...>.pred;              \
                                                                          \
  template <class Predicate, class_type... Ts>                            \
  constexpr auto reduction##_of_direct_memberwise_meta_v =                \
    impl::reduce_direct_memberwise_meta_v<Predicate, Ts...>.pred;

/**
 * all|any|none_of_direct_memberwise_v<Predicate, Ts...>
 *   Checks whether Predicate<
 *     jth-direct-member-type-of(Ts...[0], j), ...,
 *     jth-direct-member-type-of(Ts...[K - 1], j)>::value
 *   is true for all/any/none of j = 0 to N - 1 where
 *     jth-direct-member-type-of(T, j) gets the type
 *       (qualifiers preserved) of j-th direct accessible NSDM of class T.
 *     N = min(count of direct accessible NSDMs of T) for T in Ts...
 *     K = sizeof...(Ts)
 *
 * all|any|none_of_direct_memberwise_meta_v<Predicate, Ts...>
 *   Checks whether Predicate{}.operator()(
 *     jth-direct-member-of(Ts...[0], j), ...,
 *     jth-direct-member-of(Ts...[K - 1], j))
 *   is true for all/any/none of j = 0 to N - 1 where
 *     jth-direct-member-of(T, j) gets the std::meta::info
 *       of j-th direct accessible non-static data member of class T.
 *     N and K are same as above.
 */
REFLECT_CPP26_MEMBERWISE_REDUCTION_V_LIST(
  REFLECT_CPP26_MAKE_DIRECT_MEMBERWISE_REDUCTION_V)

#define REFLECT_CPP26_MAKE_FLATTENED_MEMBERWISE_REDUCTION_V(reduction, pred)  \
  template <template <class...> class Predicate, partially_flattenable... Ts> \
  constexpr auto reduction##_of_flattened_memberwise_v =                      \
    impl::reduce_flattened_memberwise_v<Predicate, Ts...>.pred;               \
                                                                              \
  template <class Predicate, partially_flattenable... Ts>                     \
  constexpr auto reduction##_of_flattened_memberwise_meta_v =                 \
    impl::reduce_flattened_memberwise_meta_v<Predicate, Ts...>.pred;

/**
 * all|any|none_of_flattened_memberwise_v<Predicate, Ts...>
 *   Checks whether Predicate<
 *     jth-flattened-member-type-of(Ts...[0], j), ...,
 *     jth-flattened-member-type-of(Ts...[K - 1], j)>::value
 *   is true for all/any/none of j = 0 to N - 1 where
 *     jth-flattened-member-type-of(T, j) gets the type
 *       (qualifiers preserved) of j-th flattened accessible NSDM of class T.
 *     N = min(count of flattened accessible NSDMs of T) for T in Ts...
 *     K = sizeof...(Ts)
 *
 * all|any|none_of_flattened_memberwise_meta_v<Predicate, Ts...>
 *   Checks whether Predicate{}.operator()(
 *     jth-flattened-member-of(Ts...[0], j), ...,
 *     jth-flattened-member-of(Ts...[K - 1], j))
 *   is true for all/any/none of j = 0 to N - 1 where
 *     jth-flattened-member-of(T, j) gets the
 *       std::meta::info or flattened_data_member_spec
 *       of j-th flattened accessible non-static data member of class T.
 *     N and K are same as above.
 */
 REFLECT_CPP26_MEMBERWISE_REDUCTION_V_LIST(
  REFLECT_CPP26_MAKE_FLATTENED_MEMBERWISE_REDUCTION_V)

#undef REFLECT_CPP26_MEMBERWISE_REDUCTION_V_LIST
#undef REFLECT_CPP26_MAKE_DIRECT_MEMBERWISE_REDUCTION_V
#undef REFLECT_CPP26_MAKE_FLATTENED_MEMBERWISE_REDUCTION_V
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_MEMBERWISE_PREDICATE_HPP
