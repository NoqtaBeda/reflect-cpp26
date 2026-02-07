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
  std::meta::info member;
  std::meta::member_offset actual_offset;

  consteval auto type() const -> std::meta::info {
    return type_of(member);
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
consteval size_t count_all_flattened_nsdm(std::meta::info T);
consteval size_t count_public_flattened_nsdm(std::meta::info T);

template <class T>
constexpr auto all_flattened_nsdm_count_v = count_all_flattened_nsdm(^^T);
template <class T>
constexpr auto public_flattened_nsdm_count_v = count_public_flattened_nsdm(^^T);

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
  constexpr auto N = impl::all_flattened_nsdm_count_v<T>;
  return impl::make_flattened_nsdm_array<N>(impl::walk_all_nsdm<T>());
}

template <class T>
consteval auto make_public_flattened_nsdm_array()
/* -> std::array<flattened_data_member_info, N> */
{
  constexpr auto N = impl::public_flattened_nsdm_count_v<T>;
  return impl::make_flattened_nsdm_array<N>(impl::walk_public_nsdm<T>());
}
}  // namespace impl

template <partially_flattenable_class T>
constexpr auto all_flattened_nonstatic_data_members_v =
    impl::make_all_flattened_nsdm_array<std::remove_cv_t<T>>();

template <partially_flattenable_class T>
constexpr auto public_flattened_nonstatic_data_members_v =
    impl::make_public_flattened_nsdm_array<std::remove_cv_t<T>>();

namespace impl {
consteval size_t count_all_flattened_nsdm(std::meta::info T) {
  auto res = all_direct_nonstatic_data_members_of(T).size();
  for (auto base : all_direct_bases_of(T)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is not allowed.");
    }
    res += extract<size_t>(^^all_flattened_nsdm_count_v, type_of(base));
  }
  return res;
}

consteval size_t count_public_flattened_nsdm(std::meta::info T) {
  auto res = public_direct_nonstatic_data_members_of(T).size();
  for (auto base : public_direct_bases_of(T)) {
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is not allowed.");
    }
    res += extract<size_t>(^^public_flattened_nsdm_count_v, type_of(base));
  }
  return res;
}

template <class T>
consteval auto walk_all_nsdm() -> std::vector<flattened_data_member_info> {
  auto members = std::vector<flattened_data_member_info>{};

  template for (constexpr auto base : all_direct_bases_v<T>) {
    using B = [:type_of(base):];
    if (is_virtual(base)) {
      compile_error("Virtual inheritance is disallowed.");
    }
    auto base_offset_bytes = offset_of(base).bytes;

    for (const auto& cur : all_flattened_nonstatic_data_members_v<B>) {
      auto actual_offset = std::meta::member_offset{
          .bytes = cur.actual_offset.bytes + base_offset_bytes,
          .bits = cur.actual_offset.bits,
      };
      members.push_back({.member = cur.member, .actual_offset = actual_offset});
    }
  }
  for (auto member : all_direct_nonstatic_data_members_of(^^T)) {
    members.push_back({.member = member, .actual_offset = offset_of(member)});
  }
  return members;
}

template <class T>
consteval auto walk_public_nsdm() -> std::vector<flattened_data_member_info> {
  auto members = std::vector<flattened_data_member_info>{};

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
        members.push_back({.member = cur.member, .actual_offset = actual_offset});
      }
    }
  }

  auto direct_members = all_direct_nonstatic_data_members_of(^^T);
  for (auto i = 0zU, n = size(direct_members); i < n; i++) {
    auto cur = direct_members[i];
    if (is_public(cur)) {
      members.push_back({.member = cur, .actual_offset = offset_of(cur)});
    }
  }
  return members;
}
}  // namespace impl
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_CLASS_TYPES_FLATTENED_NSDM_HPP
