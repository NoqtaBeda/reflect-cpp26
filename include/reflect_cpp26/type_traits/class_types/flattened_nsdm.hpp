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

#ifndef REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENED_NSDM_HPP
#define REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENED_NSDM_HPP

#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#include <reflect_cpp26/utils/constant.hpp>

namespace reflect_cpp26 {
struct flattened_data_member_info {
  // Index of current non-static data member in
  // all_flattened_nonstatic_data_members_v<T>.
  size_t index;
  // If current member is public in T, then public_index = index of current
  // non-static data member in public_flattened_nonstatic_data_members_v<T>.
  // Otherwise, public_index = npos;
  size_t public_index;
  // Reflection to a public non-static data member of T,
  // either defined by T directly or inherited from some base class.
  std::meta::info member;
  // Actual offset of member relative to T.
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

  consteval size_t alignment() const {
    return alignment_of(member);
  }

  consteval auto type() const -> std::meta::info {
    return type_of(member);
  }

  consteval bool has_identifier() const {
    return std::meta::has_identifier(member);
  }

  // Returns alt if the member is anonymous
  consteval auto identifier(std::string_view alt = "") const -> std::string_view {
    return reflect_cpp26::identifier_of(member, alt);
  }

  // Which class current member is defined directly in.
  consteval auto direct_parent() const -> std::meta::info {
    return parent_of(member);
  }

  consteval auto offset_in_parent() const -> std::meta::member_offset {
    return offset_of(member);
  }

  consteval size_t offset_bits_in_parent() const {
    auto offset = offset_in_parent();
    return offset.bytes * CHAR_BIT + offset.bits;
  }

  consteval size_t offset_bytes_in_parent() const {
    auto offset = offset_in_parent();
    if (offset.bits != 0) {
      compile_error("Can not get offset bytes of bit-fields.");
    }
    return offset.bytes;
  }
};

namespace impl {
consteval size_t count_flattened_nsdm(access_mode mode, std::meta::info T);

template <access_mode Mode, class T>
constexpr auto flattened_nsdm_count_v = count_flattened_nsdm(Mode, ^^T);

template <class T>
consteval auto walk_all_nsdm() -> std::vector<flattened_data_member_info>;

template <class T>
consteval auto walk_public_nsdm() -> std::vector<flattened_data_member_info>;

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

template <class T>
consteval auto make_all_flattened_nsdm_array()
/* -> std::array<flattened_data_member_info, N> */
{
  constexpr auto N = impl::flattened_nsdm_count_v<access_mode::unchecked, T>;
  return impl::make_flattened_nsdm_array<N>(impl::walk_all_nsdm<T>());
}

template <class T>
consteval auto make_public_flattened_nsdm_array()
/* -> std::array<flattened_data_member_info, N> */
{
  constexpr auto N = impl::flattened_nsdm_count_v<access_mode::unprivileged, T>;
  return impl::make_flattened_nsdm_array<N>(impl::walk_public_nsdm<T>());
}
}  // namespace impl

/**
 * Gets a full list of non-static data members of non-union
 * class T, including:
 * (1) Direct non-static data members of T;
 * (2) All flattened non-static data members inherited from base classes of T,
 *     recursively.
 * Members are sorted in ascending order by offset in T.
 */
template <partially_flattenable_class T>
constexpr auto all_flattened_nonstatic_data_members_v =
    impl::make_all_flattened_nsdm_array<std::remove_cv_t<T>>();

/**
 * Gets a full list of non-static data members with public access of non-union
 * class T (i.e. each member in the list can be accessed globally via class T).
 * Details same as above.
 */
template <partially_flattenable_class T>
constexpr auto public_flattened_nonstatic_data_members_v =
    impl::make_public_flattened_nsdm_array<std::remove_cv_t<T>>();

/**
 * Gets a full list of non-static data members with given access mode of
 * non-union class T. Details same as above.
 */
template <access_mode Mode, partially_flattenable_class T>
constexpr auto flattened_nonstatic_data_members_v = []() consteval {
  if constexpr (Mode == access_mode::unprivileged) {
    return public_flattened_nonstatic_data_members_v<T>;
  } else if constexpr (Mode == access_mode::unchecked) {
    return all_flattened_nonstatic_data_members_v<T>;
  } else {
    static_assert(false, "Invalid access mode.");
  }
}();

namespace impl {
consteval size_t count_flattened_nsdm(access_mode mode, std::meta::info T) {
  auto res = direct_nonstatic_data_members_of(mode, T).size();
  for (auto base : direct_bases_of(mode, T)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is not allowed.");
    }
    auto Mode = std::meta::reflect_constant(mode);
    res += extract<size_t>(^^flattened_nsdm_count_v, Mode, type_of(base));
  }
  return res;
}

template <class T>
consteval auto walk_all_nsdm() -> std::vector<flattened_data_member_info> {
  auto members = std::vector<flattened_data_member_info>{};
  auto public_count = 0zU;

  auto append_member = [&members, &public_count](bool is_public,
                                                 std::meta::info member,
                                                 std::meta::member_offset offset) {
    members.push_back({
        .index = size(members),
        .public_index = is_public ? public_count++ : npos,
        .member = member,
        .actual_offset = offset,
    });
  };
  template for (constexpr auto base : all_direct_bases_v<T>) {
    using B = [:type_of(base):];
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    auto base_offset_bytes = offset_of(base).bytes;

    for (const auto& cur : all_flattened_nonstatic_data_members_v<B>) {
      auto is_public = std::meta::is_public(base) && cur.public_index != npos;
      auto actual_offset = std::meta::member_offset{
          .bytes = cur.actual_offset.bytes + base_offset_bytes,
          .bits = cur.actual_offset.bits,
      };
      append_member(is_public, cur.member, actual_offset);
    }
  }
  for (auto member : all_direct_nonstatic_data_members_of(^^T)) {
    append_member(is_public(member), member, offset_of(member));
  }
  return members;
}

template <class T>
consteval auto walk_public_nsdm() -> std::vector<flattened_data_member_info> {
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
  template for (constexpr auto base : all_direct_bases_v<T>) {
    using B = [:type_of(base):];
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    if constexpr (is_public(base)) {
      auto base_offset_bytes = offset_of(base).bytes;
      for (const auto& cur : public_flattened_nonstatic_data_members_v<B>) {
        auto actual_offset = std::meta::member_offset{
            .bytes = cur.actual_offset.bytes + base_offset_bytes,
            .bits = cur.actual_offset.bits,
        };
        append_member(all_count + cur.index, cur.member, actual_offset);
      }
    }
    all_count += flattened_nsdm_count_v<access_mode::unchecked, B>;
  }

  auto direct_members = all_direct_nonstatic_data_members_of(^^T);
  for (auto i = 0zU, n = size(direct_members); i < n; i++) {
    auto cur = direct_members[i];
    if (is_public(cur)) {
      append_member(all_count + i, cur, offset_of(cur));
    }
  }
  return members;
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENED_NSDM_HPP
