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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP

#include <reflect_cpp26/type_traits/class_types/has_non_public_nsdm.hpp>
#include <reflect_cpp26/type_traits/class_types/member_traits.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
namespace impl {
template <class T>
consteval auto walk_public_nsdm() -> std::meta::info;

template <class T>
using public_flattened_nsdm_t = [: walk_public_nsdm<T>() :];
} // namespace impl

/**
 * Usage (functions below are in namespace std::meta):
 * - type_of(member) gets its value type
 * - identifier_of(member) gets its identifier
 * - parent_of(member) gets the direct class that defines this member
 *   - May be some (possibly indirect) base class of T
 * - offset_of(member) gets its offset inside parent_of(member)
 *   - Note: this offset is NOT relative to T. Use actual_offset instead.
 * - alignment_of(member) gets its alignment.
 */
struct flattened_data_member_spec {
  /**
   * Reflection to a public non-static data member of T,
   * either defined by T directly or inherited from some base class.
   */
  std::meta::info member;
  /**
   * Actual offset of member relative to T.
   */
  std::meta::member_offset actual_offset;
};

template <flattened_data_member_spec... Members>
struct flattened_data_member_spec_constant : constant<Members...> {
  using base = constant<Members...>;

  static constexpr auto to_members()
  {
    constexpr auto map_fn = [](auto cur) { return cur.value.member; };
    return base::template map<map_fn>();
  }

  static constexpr auto to_actual_offsets()
  {
    constexpr auto map_fn = [](auto cur) { return cur.value.actual_offset; };
    return base::template map<map_fn>();
  }

  static constexpr auto to_actual_offset_bytes()
  {
    constexpr auto map_fn = [](auto cur) {
      if (is_bit_field(cur.value.member)) {
        compile_error("Invalid transformation with bit-fields.");
      }
      return cur.value.actual_offset.bytes;
    };
    return base::template map<map_fn>();
  }

  static constexpr auto to_actual_offset_bits()
  {
    constexpr auto map_fn = [](auto cur) {
      auto a = cur.value.actual_offset;
      return a.bytes * CHAR_BIT + a.bits;
    };
    return base::template map<map_fn>();
  }
};

/**
 * Gets a full list of non-static data members (NSDM) with public access
 * of non-union class T, including:
 * (1) Direct public NSDMs of T;
 * (2) All public NSDMs inherited from public base classes of T.
 *
 * The result NSDM list is of type constant<m1, m2, ...>, where each mi is
 * a flattened_data_member_spec of the data member.
 *
 * Define the inheritance graph of T as a DAG where each directed edge X -> Y
 * represents direct inheritance relationship (Y is a direct public base class
 * of X). Data members are traversed via post-order DFS from T so that
 * actual_offset is ascending as long as the inheritance graph is a tree
 * (i.e. no class is base of multiple classes).
 *
 * Note: duplicated data members may exist in the traversed list if the
 * inheritance graph is not a tree (e.g. "diamond inheritance").
 */
template <class_without_virtual_inheritance T>
constexpr auto public_flattened_nsdm_v = impl::public_flattened_nsdm_t<T>{};

namespace impl {
template <class T>
consteval auto walk_public_nsdm() -> std::meta::info
{
  auto members = std::vector<std::meta::info>{};
  REFLECT_CPP26_EXPAND(public_direct_bases_of(^^T)).for_each(
    [&members](auto base) {
      if (is_virtual(base.value)) {
        compile_error("Virtual inheritance is disallowed.");
      }
      using BaseType = [:type_of(base.value):];
      constexpr auto base_offset = offset_of(base.value).bytes;
      constexpr auto from_base =
        public_flattened_nsdm_v<BaseType>;

      from_base.for_each([&members](flattened_data_member_spec sp) {
        sp.actual_offset.bytes += base_offset;
        members.push_back(std::meta::reflect_constant(sp));
      });
    });
  REFLECT_CPP26_EXPAND(public_direct_nsdm_of(^^T))
    .for_each([&members](auto cur_member) {
      auto cur_member_spec = flattened_data_member_spec{
        .member = cur_member,
        .actual_offset = offset_of(cur_member.value),
      };
      members.push_back(std::meta::reflect_constant(cur_member_spec));
    });
  return substitute(^^flattened_data_member_spec_constant, members);
}

template <class T>
consteval auto check_flattened_members()
{
  constexpr auto members = public_flattened_nsdm_v<T>;
  return members.all_of([](auto cur_spec) {
    // Each member must be accessible; No union member is allowed.
    return is_accessible_by_member_reflection_v<T, cur_spec.value.member> &&
      !is_union_type(type_of(cur_spec.value.member));
  });
}

template <class T>
constexpr auto check_flattened_members_v =
  check_flattened_members<std::remove_cv_t<T>>();
} // namespace impl

/**
 * Whether T is a partially flattenable type.
 * Type T is partially flattenable if either condition below is satisfied:
 * 1. T is a scalar type, including:
 *   (1) arithmetic types;
 *   (2) enum types;
 *   (3) pointers (including std::nullptr_t and function pointers);
 *   (4) pointers to member (variable or function).
 * 2. T is an array type (bounded U[N] or unbounded U[]);
 * 3. T is a non-union class type that satisfies all the constraints below:
 *   (1) No direct non-static data members of union type;
 *   (2) No virtual direct base classes;
 *   (3) Every direct base class is partially flattenable, recursively;
 *   (4) Every member in public_flattened_nsdm_v<T>
 *       is accessible (no ambiguity due to multiple inheritance).
 *
 * Note: identifier duplication is allowed as long as such ambiguity is
 * not from multiple inheritance. Example:
 *   struct A { int x; int y; };
 *   struct B : A { int* x; int* y; };
 *   static_assert(is_partially_flattenable_v<B>); // PASS
 *
 *   constexpr auto specs = public_flattened_nsdm_v<B>;
 *   auto b = B{};
 *   auto some_int = 42;
 *   b.[: get<0>(specs).member :] = 42;         // A::x
 *   b.[: get<2>(specs).member :] = &some_int;  // B::x
 */
template <class T>
constexpr auto is_partially_flattenable_v =
  std::is_scalar_v<T> || std::is_array_v<T>;

template <class_without_virtual_inheritance T>
constexpr auto is_partially_flattenable_v<T> =
  impl::check_flattened_members_v<std::remove_cv_t<T>>;

template <class T>
concept partially_flattenable = is_partially_flattenable_v<T>;

template <class T>
concept partially_flattenable_class =
  is_partially_flattenable_v<T> && std::is_class_v<T>;

/**
 * Whether T is a flattenable type.
 * Type T is flattenable if either condition below is satisfied:
 * 1. T is a scalar type;
 * 2. T is an array type (bounded U[N] or unbounded U[]);
 * 3. T is a partially flattenable class type that satisfies all the
 *    additional constraints below:
 *   (1) No private or protected direct non-static data members;
 *   (2) No private or protected direct base classes that are not empty;
 *   (3) Every direct base class is a flattenable type, recursively.
 */
template <class T>
constexpr auto is_flattenable_v = std::is_scalar_v<T> || std::is_array_v<T>;

template <class_without_virtual_inheritance T>
constexpr auto is_flattenable_v<T> = is_partially_flattenable_v<T>
  && !has_non_public_nsdm_v<T>;

template <class T>
concept flattenable = is_flattenable_v<T>;

template <class T>
concept flattenable_class = is_flattenable_v<T> && std::is_class_v<T>;

namespace impl {
template <class T>
consteval auto is_flattenable_aggregate() -> bool;
} // namespace impl

/**
 * Whether T is a flattenable aggregate type.
 * Type T is a flattenable aggregate if either condition below is satisfied:
 * 1. T is an array type (bounded U[N] or unbounded U[]);
 * 2. T is a class type that satisfies all the additional constraints below:
 *   (1) T is an aggregate;
 *   (2) T has no virtual member functions;
 *   (3) Every direct base class is a flattenable aggregate, recursively.
 */
template <class T>
constexpr auto is_flattenable_aggregate_v = std::is_array_v<T>;

template <class T>
  requires (std::is_class_v<T> && std::is_aggregate_v<T>)
constexpr auto is_flattenable_aggregate_v<T> = is_flattenable_v<T>
  && impl::is_flattenable_aggregate<std::remove_cv_t<T>>();

template <class T>
concept flattenable_aggregate = is_flattenable_aggregate_v<T>;

template <class T>
concept flattenable_aggregate_class =
  is_flattenable_aggregate_v<T> && std::is_class_v<T>;

namespace impl {
template <class T>
consteval auto is_flattenable_aggregate() -> bool
{
  static_assert(std::is_class_v<T>, "Non-class type not allowed.");
  if constexpr (!std::is_aggregate_v<T>) {
    return false;
  } else {
    auto all_bases_are_flattenable =
      REFLECT_CPP26_EXPAND(all_direct_bases_of(^^T)).all_of([](auto base) {
        // Each base should be flattenable **aggregate** as well;
        using BaseType = [:std::meta::type_of(base):];
        return is_public(base.value) && !is_virtual(base.value) &&
          is_flattenable_aggregate_v<BaseType>;
      });
    if (!all_bases_are_flattenable) {
      return false;
    }
    // Checks direct members of T
    auto all_members_are_flattenable = std::ranges::all_of(
      all_direct_nsdm_of(^^T),
      [](std::meta::info member) {
        return is_public(member) && !is_union_type(type_of(member));
      });
    return all_members_are_flattenable;
  }
}
} // namespace impl
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP
