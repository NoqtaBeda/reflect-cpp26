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
/**
 * Usage (functions below are in namespace std::meta):
 * - type_of(member) gets its value type
 * - identifier_of(member) gets its identifier
 * - parent_of(member) gets the direct class that defines this member
 *   - May be some (possibly indirect) base class of T
 * - offset_of(member) gets its offset inside parent_of(member)
 *   - Note: to get the offset that is relative to T. Use actual_offset instead.
 * - alignment_of(member) gets its alignment.
 */
struct flattened_data_member_info {
  /**
   * Reflection to a public non-static data member of T,
   * either defined by T directly or inherited from some base class.
   */
  std::meta::info member;
  /**
   * Actual offset of member relative to T.
   */
  std::meta::member_offset actual_offset;

  consteval size_t actual_offset_bits() const {
    return actual_offset.bytes * CHAR_BIT + actual_offset.bits;
  }

  consteval size_t actual_offset_bytes() const {
    if (actual_offset.bits != 0) {
      compile_error("Can not get offset bytes of bit-fields.");
    }
    return actual_offset.bytes;
  }
};

namespace impl {
template <size_t N>
consteval auto make_flattened_nsdm_array(
  const std::vector<flattened_data_member_info>& input) /* -> std::array */
{
  if (input.size() != N) {
    compile_error("Implementation error: size mismatch.");
  }
  auto res = std::array<flattened_data_member_info, N>{};
  std::ranges::copy(input, res.begin());
  return res;
}

consteval size_t count_public_flattened_ndsm(std::meta::info T);

template <class T>
consteval auto walk_public_nsdm() -> std::vector<flattened_data_member_info>;

template <class T>
constexpr auto public_flattened_nsdm_count_v =
  impl::count_public_flattened_ndsm(remove_cv(^^T));
} // namespace impl

/**
 * Gets a full list of non-static data members (NSDM) with public access
 * of non-union class T, including:
 * (1) Direct public NSDMs of T;
 * (2) All public NSDMs inherited from public base classes of T.
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
constexpr auto public_flattened_nsdm_v =
  impl::make_flattened_nsdm_array<impl::public_flattened_nsdm_count_v<T>>(
    impl::walk_public_nsdm<std::remove_cv_t<T>>());

namespace impl {
consteval size_t count_public_flattened_ndsm(std::meta::info T)
{
  auto res = public_direct_nsdm_of(T).size();
  for (auto base: public_direct_bases_of(T)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    res += extract<size_t>(^^public_flattened_nsdm_count_v, type_of(base));
  }
  return res;
}

template <class T>
consteval auto walk_public_nsdm() -> std::vector<flattened_data_member_info>
{
  auto members = std::vector<flattened_data_member_info>{};
  template for (constexpr auto base: public_direct_bases_v<T>) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    auto base_offset = offset_of(base).bytes;

    using B = [:type_of(base):];
    for (auto [member, offset]: public_flattened_nsdm_v<B>) {
      offset.bytes += base_offset;
      members.push_back({member, offset});
    }
  }
  for (auto member: public_direct_nsdm_of(^^T)) {
    members.push_back({member, offset_of(member)});
  }
  return members;
}

template <class T>
consteval auto check_flattened_members()
{
  for (auto [member, _]: public_flattened_nsdm_v<T>) {
    auto M = std::meta::reflect_constant(member);
    if (!extract_bool(^^is_accessible_by_member_reflection_v, ^^T, M)) {
      return false;
    }
  }
  return true;
}
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
  impl::check_flattened_members<std::remove_cv_t<T>>();

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
constexpr auto is_flattenable_v<T> =
  is_partially_flattenable_v<T> && !has_non_public_nsdm_v<T>;

template <class T>
concept flattenable = is_flattenable_v<T>;

template <class T>
concept flattenable_class = is_flattenable_v<T> && std::is_class_v<T>;

namespace impl {
consteval bool is_flattenable_aggregate(std::meta::info T);
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
constexpr auto is_flattenable_aggregate_v<T> =
  is_flattenable_v<T> && impl::is_flattenable_aggregate(remove_cv(^^T));

template <class T>
concept flattenable_aggregate = is_flattenable_aggregate_v<T>;

template <class T>
concept flattenable_aggregate_class =
  is_flattenable_aggregate_v<T> && std::is_class_v<T>;

namespace impl {
consteval bool is_flattenable_aggregate(std::meta::info T)
{
  if (!is_class_type(T)) {
    compile_error("Non-class types are not allowed here.");
  }
  if (!is_aggregate_type(T)) {
    return false;
  }
  for (auto base: all_direct_bases_of(T)) {
    auto ok = is_public(base) && !is_virtual(base) &&
      extract_bool(^^is_flattenable_aggregate_v, type_of(base));
    if (!ok) {
      return false;
    }
  }
  for (auto member: all_direct_bases_of(T)) {
    auto ok = is_public(member) && !is_union_type(type_of(member));
    if (!ok) {
      return false;
    }
  }
  return true;
}
} // namespace impl
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP
