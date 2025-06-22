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

#ifndef REFLECT_CPP26_TYPE_OPERATIONS_DEFINE_AGGREGATE_HPP
#define REFLECT_CPP26_TYPE_OPERATIONS_DEFINE_AGGREGATE_HPP

#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#include <reflect_cpp26/type_traits/is_invocable.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26 {
namespace impl {
// (std::meta::info...) -> std::meta::info
template <class F, size_t N>
concept transformer_from_info = std::is_default_constructible_v<F> &&
  is_invocable_r_n_v<std::meta::info, F, std::meta::info, N>;

// (flattened_data_member_info...) -> std::meta::info
template <class F, size_t N>
concept transformer_from_info_struct = std::is_default_constructible_v<F> &&
  is_invocable_r_n_v<std::meta::info, F, flattened_data_member_info, N>;

template <class Transform, class... Ts>
  requires (transformer_from_info<Transform, sizeof...(Ts)>)
struct aggregate_by_direct_memberwise {
  struct type;

  consteval {
    auto min_member_size = std::min({public_direct_nsdm_of(^^Ts).size()...});
    auto members = std::vector<std::meta::info>{};
    members.reserve(min_member_size);

    for (auto i = 0zU; i < min_member_size; i++) {
      auto cur = Transform{}(public_direct_nsdm_of(^^Ts)[i]...);
      if (!is_data_member_spec(cur)) {
        compile_error("Transform function result must be data member spec.");
      }
      members.push_back(cur);
    }
    define_aggregate(^^type, members);
  }
};

template <class Transform, class... Ts>
  requires (transformer_from_info<Transform, sizeof...(Ts)>
         || transformer_from_info_struct<Transform, sizeof...(Ts)>)
struct aggregate_by_flattened_memberwise {
  static consteval auto ith_member_getter(size_t i) -> std::meta::info
  {
    constexpr auto uses_info_struct =
      transformer_from_info_struct<Transform, sizeof...(Ts)>;

    if constexpr (uses_info_struct) {
      return Transform{}(public_flattened_nsdm_v<Ts>[i]...);
    } else {
      return Transform{}(public_flattened_nsdm_v<Ts>[i].member...);
    }
  }

  struct type;

  consteval {
    auto min_member_size = std::min({public_flattened_nsdm_v<Ts>.size()...});
    auto members = std::vector<std::meta::info>{};
    members.reserve(min_member_size);

    for (auto i = 0zU; i < min_member_size; i++) {
      auto cur = ith_member_getter(i);
      if (!is_data_member_spec(cur)) {
        compile_error("Transform function result must be data member spec.");
      }
      members.push_back(cur);
    }
    define_aggregate(^^type, members);
  }
};
} // namespace impl

/**
  * Makes aggregate type whose j-th data member spec is
  *   Predicate{}.operator()(
  *     jth-direct-member-of(Ts...[0], j), ...,
  *     jth-direct-member-of(Ts...[K - 1], j)) for j = 0 to N - 1 where
  * N = min(count of direct accessible NSDMs of T) for T in Ts...
  * K = sizeof...(Ts)
  */
template <class Transform, class_type... Ts>
using aggregate_by_direct_memberwise_t =
  typename impl::aggregate_by_direct_memberwise<
    Transform, std::remove_cv_t<Ts>...>::type;

/**
  * Makes aggregate type whose j-th data member spec is
  *   Predicate{}.operator()(
  *     jth-flattened-member-of(Ts...[0], j), ...,
  *     jth-flattened-member-of(Ts...[K - 1], j)) for j = 0 to N - 1 where
  * N = min(count of flattened accessible NSDMs of T) for T in Ts...
  * K = sizeof...(Ts)
  */
template <class Transform, partially_flattenable... Ts>
using aggregate_by_flattened_memberwise_t =
  typename impl::aggregate_by_flattened_memberwise<
    Transform, std::remove_cv_t<Ts>...>::type;
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_OPERATIONS_DEFINE_AGGREGATE_HPP
