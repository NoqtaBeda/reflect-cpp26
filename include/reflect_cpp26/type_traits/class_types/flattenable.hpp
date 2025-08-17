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
   * Index of current non-static data member in all_flattened_nsdm_v<T>.
   */
  size_t index;
  /**
   * If current member is public in T, then public_index = index of current
   * non-static data member in public_flattened_nsdm_v<T>.
   * Otherwise, public_index = npos;
   */
  size_t public_index;
  /**
   * Reflection to a public non-static data member of T,
   * either defined by T directly or inherited from some base class.
   */
  std::meta::info member;
  /**
   * Actual offset of member relative to T.
   */
  std::meta::member_offset actual_offset;

  consteval bool is_public() const {
    return public_index != npos;
  }

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
// Note: Manual conversion from compile-time std::vector to std::array is
// required since define_static_array() is unavailable with compile error:
// "pointer into an object of consteval-only type is not a constant expression
// unless it also has consteval-only type"
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

consteval size_t count_flattened_nsdm(access_mode mode, std::meta::info T);

template <class T>
consteval auto walk_all_nsdm() -> std::vector<flattened_data_member_info>;

template <class T>
consteval auto walk_public_nsdm() -> std::vector<flattened_data_member_info>;

template <access_mode Mode, class T>
constexpr auto flattened_nsdm_count_v = count_flattened_nsdm(Mode, ^^T);
} // namespace impl

/**
 * Gets a full list of non-static data members (NSDM) of non-union class T,
 * including:
 * (1) Direct NSDMs of T;
 * (2) All NSDMs inherited from base classes of T with given access mode.
 *
 * Define the inheritance graph of T as a DAG where each directed edge X -> Y
 * represents direct inheritance relationship (Y is a direct base class
 * of X). Data members are traversed via post-order DFS from T so that
 * actual_offset is ascending as long as the inheritance graph is a tree
 * (i.e. no class is base of multiple classes).
 *
 * Note: duplicated data members may exist in the traversed list if the
 * inheritance graph is not a tree (e.g. "diamond inheritance").
 */
template <class_without_virtual_inheritance T>
constexpr auto all_flattened_nsdm_v = []() consteval {
  using TNoCV = std::remove_cv_t<T>;
  constexpr auto N = impl::flattened_nsdm_count_v<
    access_mode::unchecked, TNoCV>;
  return impl::make_flattened_nsdm_array<N>(impl::walk_all_nsdm<TNoCV>());
}();

/**
 * Gets a full list of non-static data members (NSDM) with public access of
 * non-union class T. Details same as above.
 */
template <class_without_virtual_inheritance T>
constexpr auto public_flattened_nsdm_v = []() consteval {
  using TNoCV = std::remove_cv_t<T>;
  constexpr auto N = impl::flattened_nsdm_count_v<
    access_mode::unprivileged, TNoCV>;
  return impl::make_flattened_nsdm_array<N>(impl::walk_public_nsdm<TNoCV>());
}();

template <access_mode Mode, class_without_virtual_inheritance T>
constexpr auto flattened_nsdm_v = compile_error("Invalid access mode.");

template <class_without_virtual_inheritance T>
constexpr auto flattened_nsdm_v<access_mode::unprivileged, T> =
  public_flattened_nsdm_v<T>;

template <class_without_virtual_inheritance T>
constexpr auto flattened_nsdm_v<access_mode::unchecked, T> =
  all_flattened_nsdm_v<T>;

namespace impl {
consteval size_t count_flattened_nsdm(access_mode mode, std::meta::info T)
{
  auto res = direct_nsdm_of(mode, T).size();
  for (auto base: direct_bases_of(mode, T)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is not allowed.");
    }
    auto Mode = std::meta::reflect_constant(mode);
    res += extract<size_t>(^^flattened_nsdm_count_v, Mode, type_of(base));
  }
  return res;
}

template <class T>
consteval auto walk_all_nsdm() -> std::vector<flattened_data_member_info>
{
  auto members = std::vector<flattened_data_member_info>{};
  auto public_count = 0zU;

  auto append_member = [&members, &public_count](
    bool is_public, std::meta::info member, std::meta::member_offset offset) {
      members.push_back({
        .index = size(members),
        .public_index = is_public ? public_count++ : npos,
        .member = member,
        .actual_offset = offset,
      });
    };

  template for (constexpr auto base: all_direct_bases_v<T>) {
    using B = [: type_of(base) :];
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    auto base_offset_bytes = offset_of(base).bytes;

    for (const auto& cur: all_flattened_nsdm_v<B>) {
      auto is_public = std::meta::is_public(base) && cur.public_index != npos;
      auto actual_offset = std::meta::member_offset{
        .bytes = cur.actual_offset.bytes + base_offset_bytes,
        .bits = cur.actual_offset.bits,
      };
      append_member(is_public, cur.member, actual_offset);
    }
  }
  for (auto member: all_direct_nsdm_of(^^T)) {
    append_member(is_public(member), member, offset_of(member));
  }
  return members;
}

template <class T>
consteval auto walk_public_nsdm() -> std::vector<flattened_data_member_info>
{
  auto members = std::vector<flattened_data_member_info>{};
  auto all_count = 0zU;

  auto append_member = [&members](
    size_t index, std::meta::info member, std::meta::member_offset offset) {
      members.push_back({
        .index = index,
        .public_index = size(members),
        .member = member,
        .actual_offset = offset,
      });
    };

  template for (constexpr auto base: all_direct_bases_v<T>) {
    using B = [: type_of(base) :];
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    if constexpr (is_public(base)) {
      auto base_offset_bytes = offset_of(base).bytes;
      for (const auto& cur: public_flattened_nsdm_v<B>) {
        auto actual_offset = std::meta::member_offset{
          .bytes = cur.actual_offset.bytes + base_offset_bytes,
          .bits = cur.actual_offset.bits,
        };
        append_member(all_count + cur.index, cur.member, actual_offset);
      }
    }
    all_count += flattened_nsdm_count_v<access_mode::unchecked, B>;
  }

  auto direct_members = all_direct_nsdm_of(^^T);
  for (auto i = 0zU, n = size(direct_members); i < n; i++) {
    auto cur = direct_members[i];
    if (is_public(cur)) {
      append_member(all_count + i, cur, offset_of(cur));
    }
  }
  return members;
}

template <access_mode Mode, class T>
consteval auto check_flattened_members()
{
  for (const auto& cur: flattened_nsdm_v<Mode, T>) {
    auto M = std::meta::reflect_constant(cur.member);
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
constexpr auto is_partially_flattenable_v<T> = impl::check_flattened_members<
  access_mode::unprivileged, std::remove_cv_t<T>>();

template <class T>
concept partially_flattenable = is_partially_flattenable_v<T>;

template <class T>
concept partially_flattenable_class =
  is_partially_flattenable_v<T> && std::is_class_v<T>;

REFLECT_CPP26_DEFINE_CONCEPT_WITH_CVREF(reflect_cpp26, partially_flattenable)
REFLECT_CPP26_DEFINE_CONCEPT_WITH_CVREF(
  reflect_cpp26, partially_flattenable_class)

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

REFLECT_CPP26_DEFINE_CONCEPT_WITH_CVREF(reflect_cpp26, flattenable)
REFLECT_CPP26_DEFINE_CONCEPT_WITH_CVREF(reflect_cpp26, flattenable_class)

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

REFLECT_CPP26_DEFINE_CONCEPT_WITH_CVREF(reflect_cpp26, flattenable_aggregate)
REFLECT_CPP26_DEFINE_CONCEPT_WITH_CVREF(
  reflect_cpp26, flattenable_aggregate_class)

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
    if (!ok) { return false; }
  }
  for (auto member: all_direct_bases_of(T)) {
    auto ok = is_public(member) && !is_union_type(type_of(member));
    if (!ok) { return false; }
  }
  return true;
}
} // namespace impl
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENABLE_HPP
