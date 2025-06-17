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

#ifndef REFLECT_CPP26_ENUM_ENUM_FLAGS_NAME_HPP
#define REFLECT_CPP26_ENUM_ENUM_FLAGS_NAME_HPP

#include <functional>
#include <iostream>
#include <reflect_cpp26/enum/impl/enum_flags.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26 {
template <class Iter>
struct enum_flags_name_to_result {
  std::errc ec;
  Iter out;
};

namespace impl {
constexpr size_t delim_length(char) {
  return 1;
}

constexpr size_t delim_length(std::string_view delim) {
  return delim.length();
}

template <class Iter, class Sentinel>
constexpr bool do_copy_segment(Iter& iter, Sentinel sentinel, std::string_view seg) {
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
    return (seg_iter == seg_end);
  }
}

template <class Iter, class Sentinel>
constexpr bool do_copy_segment(Iter& iter, Sentinel sentinel, char delim) {
  if (iter == sentinel) {
    return false;
  }
  *iter++ = delim;
  return true;
}

template <class Iter>
constexpr bool do_copy_segment(Iter& iter, std::unreachable_sentinel_t, std::string_view seg) {
  iter = std::ranges::copy(seg, iter).out;
  return true;
}

template <class Iter>
constexpr bool do_copy_segment(Iter& iter, std::unreachable_sentinel_t, char delim) {
  *iter++ = delim;
  return true;
}

template <class Iter, class Sentinel, class E, class Delim>
constexpr auto regular_enum_flags_name_to_impl(Iter iter, Sentinel sentinel, E flags, Delim delim)
    -> enum_flags_name_to_result<Iter> {
  constexpr auto decomp = enum_flags_decomposer_v<E>;
  auto remaining = zero_extend<uint64_t>(std::to_underlying(flags));
  if (remaining == 0) {
    return {.ec = std::errc{}, .out = iter};  // Nothing to copy from ""
  }
  if ((remaining & decomp.full_set) != remaining) {
    return {.ec = std::errc::invalid_argument, .out = iter};
  }

  // First pass checks whether flags can be decomposed
  template for (constexpr auto e : decomp.units) {
    if constexpr (e.popcount != 1) {
      auto intersection = remaining & e.underlying;
      if (intersection != e.underlying && intersection != 0) {
        return {.ec = std::errc::invalid_argument, .out = iter};
      }
    }
  }
  // Second pass generates the flags string
  auto is_first = true;
  template for (constexpr auto e : decomp.units) {
    if ((remaining & e.underlying) != 0) {
      if (!is_first && !do_copy_segment(iter, sentinel, delim)) {
        return {.ec = std::errc::value_too_large, .out = iter};
      }
      is_first = false;
      if (!do_copy_segment(iter, sentinel, e.name)) {
        return {.ec = std::errc::value_too_large, .out = iter};
      }
    }
  }
  return {.ec = std::errc{}, .out = iter};  // OK
}

template <class Iter, class Sentinel, class E, class Delim>
constexpr auto irregular_enum_flags_name_to_impl(Iter iter, Sentinel sentinel, E flags, Delim delim)
    -> enum_flags_name_to_result<Iter> {
  constexpr auto decomp = enum_flags_decomposer_v<E>;
  auto underlying = zero_extend<uint64_t>(std::to_underlying(flags));
  if (underlying == 0) {
    return {.ec = std::errc{}, .out = iter};  // Nothing to copy from ""
  }
  if ((underlying & decomp.full_set) != underlying) {
    return {.ec = std::errc::invalid_argument, .out = iter};
  }

  // First passchecks whether flags can be decomposed properly
  auto covered = uint64_t{0};
  for (auto i = 0zU, n = decomp.units.size(); i < n; i++) {
    auto u = decomp.units[i].underlying;
    if ((underlying & u) == u && (covered |= u) == underlying) {
      break;
    }
  }
  if (covered != underlying) {
    return {.ec = std::errc::invalid_argument, .out = iter};
  }

  // Second pass generates the flags string
  auto vis = std::array<uint8_t, decomp.units.size()>{};
  covered = 0;
  for (auto i = 0zU, n = decomp.units.size(); i < n; i++) {
    auto u = decomp.units[i].underlying;
    if (vis[i] || (underlying & u) != u) {
      continue;
    }
    if (covered != 0 && !do_copy_segment(iter, sentinel, delim)) {
      return {.ec = std::errc::value_too_large, .out = iter};
    }
    auto cur_name = decomp.units[i].name;
    if (!do_copy_segment(iter, sentinel, cur_name)) {
      return {.ec = std::errc::value_too_large, .out = iter};
    }
    if ((covered |= u) == underlying) {
      break;
    }
    auto j_end = decomp.heads[i + 1];
    for (auto j = decomp.heads[i]; j < j_end; j++) {
      vis[decomp.subset_indices[j]] = true;
    }
  }
  return {.ec = std::errc{}, .out = iter};
}

template <class Iter, class Sentinel, class E, class Delim>
constexpr auto enum_flags_name_to_impl(Iter iter, Sentinel sentinel, E flags, Delim delim)
    -> enum_flags_name_to_result<Iter> {
  if constexpr (enum_flags_is_empty_v<E>) {
    auto ec = (std::to_underlying(flags) != 0) ? std::errc::invalid_argument : std::errc{};
    return {.ec = ec, .out = iter};
  } else if constexpr (enum_flags_is_regular_v<E>) {
    return regular_enum_flags_name_to_impl(iter, sentinel, flags, delim);
  } else {
    return irregular_enum_flags_name_to_impl(iter, sentinel, flags, delim);
  }
}

template <class E, class Delim>
constexpr auto enum_flags_name_impl(E flags, Delim delim) -> std::optional<std::string> {
  if constexpr (enum_flags_is_empty_v<E>) {
    if (std::to_underlying(flags) == 0) {
      return std::string{};
    }
    return std::nullopt;
  } else {
    constexpr auto decomp = enum_flags_decomposer_v<E>;
    auto reserved_size = decomp.sum_name_length + delim_length(delim) * decomp.units.size();
    auto res = std::string{};
    auto ok = false;
    res.resize_and_overwrite(reserved_size, [&](char* buffer, size_t) {
      auto [ec, out] = enum_flags_name_to_impl(buffer, std::unreachable_sentinel, flags, delim);
      ok = (std::errc{} == ec);
      return out - buffer;
    });
    if (!ok) {
      return std::nullopt;
    }
    return res;
  }
}
}  // namespace impl

struct enum_flags_name_t {
  template <enum_type E>
  static constexpr auto operator()(E flags, char delim = '|') -> std::optional<std::string> {
    return impl::enum_flags_name_impl(flags, delim);
  }

  template <enum_type E>
  static constexpr auto operator()(E flags, std::string_view delim) -> std::optional<std::string> {
    return impl::enum_flags_name_impl(flags, delim);
  }
};

struct enum_flags_name_to_t {
  template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel, enum_type E>
  static constexpr auto operator()(Iter iter, Sentinel sentinel, E value, char delim = '|')
      -> enum_flags_name_to_result<Iter> {
    return impl::enum_flags_name_to_impl(iter, sentinel, value, delim);
  }

  template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel, enum_type E>
  static constexpr auto operator()(Iter iter, Sentinel sentinel, E value, std::string_view delim)
      -> enum_flags_name_to_result<Iter> {
    return impl::enum_flags_name_to_impl(iter, sentinel, value, delim);
  }

  template <enum_type E>
  static constexpr auto operator()(std::ostream& out, E value, char delim = '|') -> std::errc {
    auto [ec, _] = impl::enum_flags_name_to_impl(
        std::ostreambuf_iterator<char>(out), std::unreachable_sentinel, value, delim);
    return ec;
  }

  template <enum_type E>
  static constexpr auto operator()(std::ostream& out, E value, std::string_view delim)
      -> std::errc {
    auto [ec, _] = impl::enum_flags_name_to_impl(
        std::ostreambuf_iterator<char>(out), std::unreachable_sentinel, value, delim);
    return ec;
  }
};

constexpr auto enum_flags_name = enum_flags_name_t{};
constexpr auto enum_flags_name_opt = enum_flags_name_t{};  // Alias

constexpr auto enum_flags_name_to = enum_flags_name_to_t{};
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_ENUM_ENUM_FLAGS_NAME_HPP
