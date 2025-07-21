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

#ifndef REFLECT_CPP26_ENUM_ENUM_FLAGS_NAME_HPP
#define REFLECT_CPP26_ENUM_ENUM_FLAGS_NAME_HPP

#include <reflect_cpp26/enum/impl/enum_flags.hpp>
#include <reflect_cpp26/enum/impl/tags.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <functional>

namespace reflect_cpp26 {
template <class Iter>
struct enum_flags_name_to_result {
  bool done;
  Iter out;
};

template <class F, class Iter, class Sentinel>
concept enum_flags_name_alt_invocable =
  std::is_invocable_r_v<enum_flags_name_to_result<Iter>, F, Iter, Sentinel>;

namespace impl {
constexpr size_t delim_length(char) {
  return 1;
}

constexpr size_t delim_length(std::string_view delim) {
  return delim.length();
}

template <class Iter, class Sentinel>
constexpr bool do_copy_enum_flags_segment(
  Iter& iter, Sentinel sentinel, std::string_view seg,
  without_allocated_buffer_tag_t)
{
  if constexpr (std::sized_sentinel_for<Sentinel, Iter>) {
    if (sentinel - iter < seg.length()) {
      return false;
    }
    iter = std::ranges::copy(seg, iter).out;
    return true;
  } else {
    auto seg_iter = seg.begin();
    auto seg_end = seg.end();
    for (; iter != sentinel && seg_iter != seg_end; ++iter, ++seg_iter) {
      *iter = *seg_iter;
    }
    return seg_iter == seg_end; // Whether copy finishes
  }
}

template <class Iter, class Sentinel>
constexpr bool do_copy_enum_flags_segment(
  Iter& iter, Sentinel sentinel, char delim, without_allocated_buffer_tag_t)
{
  if (iter == sentinel) {
    return false;
  }
  *iter++ = delim;
  return true;
}

template <class Iter, class Sentinel>
constexpr bool do_copy_enum_flags_segment(
  Iter& iter, Sentinel, std::string_view seg, with_allocated_buffer_tag_t)
{
  iter = std::ranges::copy(seg, iter).out;
  return true;
}

template <class Iter, class Sentinel>
constexpr bool do_copy_enum_flags_segment(
  Iter& iter, Sentinel, char delim, with_allocated_buffer_tag_t)
{
  *iter++ = delim;
  return true;
}

template <class Iter, class Sentinel, class E, class Delim, class Alt,
          class Tag>
constexpr auto regular_enum_flags_name_to_impl(
  Iter iter, Sentinel sentinel, E flags, Delim delim, Alt alt, Tag tag)
  -> enum_flags_name_to_result<Iter>
{
  auto make_alt = [](Iter iter, Sentinel sentinel, const Alt& alt)
      -> enum_flags_name_to_result<Iter> {
    if constexpr (std::is_invocable_v<Alt, Iter, Sentinel>) {
      return std::invoke(alt, iter, sentinel);
    } else {
      auto done = do_copy_enum_flags_segment(iter, sentinel, alt, Tag{});
      return {.done = done, .out = iter};
    }
  };

  constexpr auto decomp = enum_flags_decomposer_v<E>;
  auto remaining = zero_extend<uint64_t>(std::to_underlying(flags));
  if (remaining == 0) {
    return {.done = true, .out = iter}; // Nothing to copy from ""
  }
  if ((remaining & decomp.full_set) != remaining) {
    return make_alt(iter, sentinel, alt);
  }
  // First pass checks whether flags can be decomposed
  template for (constexpr auto e: decomp.units) {
    if constexpr (e.popcount != 1) {
      auto intersection = remaining & e.underlying;
      if (intersection != e.underlying && intersection != 0) {
        return make_alt(iter, sentinel, alt);
      }
    }
  }
  // Second pass generates the flags string
  auto is_first = true;
  auto do_copy_name = [&is_first, &iter, sentinel, delim](auto name) {
    constexpr auto tag = Tag{};
    if (!is_first && !do_copy_enum_flags_segment(iter, sentinel, delim, tag)) {
      return false;
    }
    is_first = false;
    return do_copy_enum_flags_segment(iter, sentinel, name, tag);
  };
  template for (constexpr auto e: decomp.units) {
    if constexpr (e.popcount != 1) {
      auto intersection = remaining & e.underlying;
      if (intersection == e.underlying && !do_copy_name(e.name)) {
        return {.done = false, .out = iter};
      }
    } else if ((remaining & e.underlying) != 0 && !do_copy_name(e.name)) {
      return {.done = false, .out = iter};
    }
  }
  return {.done = true, .out = iter};
}

template <class Iter, class Sentinel, class E, class Delim, class Alt,
          class Tag>
constexpr auto irregular_enum_flags_name_to_impl(
  Iter iter, Sentinel sentinel, E flags, Delim delim, Alt alt, Tag tag)
  -> enum_flags_name_to_result<Iter>
{
  auto make_alt = [](Iter iter, Sentinel sentinel, const Alt& alt)
      -> enum_flags_name_to_result<Iter> {
    if constexpr (std::is_invocable_v<Alt, Iter, Sentinel>) {
      return std::invoke(alt, iter, sentinel);
    } else {
      auto done = do_copy_enum_flags_segment(iter, sentinel, alt, Tag{});
      return {.done = done, .out = iter};
    }
  };

  constexpr auto decomp = enum_flags_decomposer_v<E>;
  auto underlying = zero_extend<uint64_t>(std::to_underlying(flags));
  if (underlying == 0) {
    return {.done = true, .out = iter}; // Nothing to copy from ""
  }
  if ((underlying & decomp.full_set) != underlying) {
    return make_alt(iter, sentinel, alt);
  }
  auto covered = uint64_t{0};
  // First passchecks whether flags can be decomposed properly
  for (auto i = 0zU, n = decomp.units.size(); i < n; i++) {
    auto u = decomp.units[i].underlying;
    if ((underlying & u) == u && (covered |= u) == underlying) { break; }
  }
  if (covered != underlying) {
    return make_alt(iter, sentinel, alt);
  }
  // Second pass generates the flags string
  auto vis = std::array<uint8_t, decomp.units.size()>{};
  covered = 0;
  for (auto i = 0zU, n = decomp.units.size(); i < n; i++) {
    auto u = decomp.units[i].underlying;
    if (vis[i] || (underlying & u) != u) { continue; }

    if (covered != 0) {
      if (!do_copy_enum_flags_segment(iter, sentinel, delim, tag)) {
        return {.done = false, .out = iter};
      }
    }
    auto cur_name = decomp.units[i].name;
    if (!do_copy_enum_flags_segment(iter, sentinel, cur_name, tag)) {
      return {.done = false, .out = iter};
    }
    if ((covered |= u) == underlying) { break; }
    auto j_end = decomp.heads[i + 1];
    for (auto j = decomp.heads[i]; j < j_end; j++) {
      vis[decomp.subset_indices[j]] = true;
    }
  }
  return {.done = true, .out = iter};
}

template <class Iter, class Sentinel, class E, class Delim, class Alt>
constexpr auto enum_flags_name_to_impl(
  Iter iter, Sentinel sentinel, E flags, Delim delim, Alt alt)
  -> enum_flags_name_to_result<Iter>
{
  using tag_type = std::conditional_t<
    std::is_same_v<Sentinel, std::unreachable_sentinel_t>,
    with_allocated_buffer_tag_t,
    without_allocated_buffer_tag_t>;

  if constexpr (enum_flags_is_empty_v<E>) {
    if (std::to_underlying(flags) == 0) {
      return {.done = true, .out = iter}; // Nothing to copy from ""
    }
    auto done = do_copy_enum_flags_segment(
      iter, sentinel, std::move(alt), tag_type{});
    return {.done = done, .out = iter};
  } else if constexpr (enum_flags_is_regular_v<E>) {
    return regular_enum_flags_name_to_impl(
      iter, sentinel, flags, delim, std::move(alt), tag_type{});
  } else {
    return irregular_enum_flags_name_to_impl(
      iter, sentinel, flags, delim, std::move(alt), tag_type{});
  }
}

template <class E, class Delim>
constexpr auto enum_flags_name_impl(E flags, Delim delim, std::string_view alt)
  -> std::string
{
  if constexpr (enum_flags_is_empty_v<E>) {
    return std::to_underlying(flags) == 0 ? "" : std::string{alt};
  } else {
    constexpr auto decomp = enum_flags_decomposer_v<E>;
    auto res = std::string{};
    auto reserved_size =
      decomp.sum_name_length + delim_length(delim) * decomp.units.size();
    if constexpr (enum_flags_is_regular_v<E>) {
      res.resize_and_overwrite(reserved_size, [&](char* buffer, size_t) {
        return regular_enum_flags_name_to_impl(
          buffer, std::unreachable_sentinel, flags, delim, alt,
          with_allocated_buffer).out - buffer;
      });
    } else {
      res.resize_and_overwrite(reserved_size, [&](char* buffer, size_t) {
        return irregular_enum_flags_name_to_impl(
          buffer, std::unreachable_sentinel, flags, delim, alt,
          with_allocated_buffer).out - buffer;
      });
    }
    return res;
  }
}
} // namespace impl

template <enum_type E>
constexpr auto enum_flags_name(
  E flags, char delim = '|', std::string_view alt = {}) -> std::string
{
  return impl::enum_flags_name_impl(flags, delim, alt);
}

template <enum_type E>
constexpr auto enum_flags_name(
  E flags, std::string_view delim, std::string_view alt = {}) -> std::string
{
  return impl::enum_flags_name_impl(flags, delim, alt);
}

template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel,
          enum_type E>
constexpr auto enum_flags_name_to(
  Iter iter, Sentinel sentinel, E value, char delim = '|',
  std::string_view alt = {}) -> enum_flags_name_to_result<Iter>
{
  return impl::enum_flags_name_to_impl(iter, sentinel, value, delim, alt);
}

template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel,
          enum_type E>
constexpr auto enum_flags_name_to(
  Iter iter, Sentinel sentinel, E value, std::string_view delim,
  std::string_view alt = {}) -> enum_flags_name_to_result<Iter>
{
  return impl::enum_flags_name_to_impl(iter, sentinel, value, delim, alt);
}

template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel,
          enum_type E, enum_flags_name_alt_invocable<Iter, Sentinel> AltFunc>
constexpr auto enum_flags_name_to(
  Iter iter, Sentinel sentinel, E value, char delim, AltFunc alt_func)
  -> enum_flags_name_to_result<Iter>
{
  return impl::enum_flags_name_to_impl(
    iter, sentinel, value, delim, std::move(alt_func));
}

template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel,
          enum_type E, enum_flags_name_alt_invocable<Iter, Sentinel> AltFunc>
constexpr auto enum_flags_name_to(
  Iter iter, Sentinel sentinel, E value, std::string_view delim,
  AltFunc alt_func) -> enum_flags_name_to_result<Iter>
{
  return impl::enum_flags_name_to_impl(
    iter, sentinel, value, delim, std::move(alt_func));
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_FLAGS_NAME_HPP
