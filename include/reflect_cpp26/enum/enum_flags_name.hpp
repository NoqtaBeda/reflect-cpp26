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
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26 {
namespace impl {
constexpr size_t delim_length(char) {
  return 1;
}

constexpr size_t delim_length(std::string_view delim) {
  return delim.length();
}

#define REFLECT_CPP26_APPEND_ENUM_FLAGS_NAME_COMMON(cur_name) \
  do {                                                        \
    if (buffer != buffer_head) {                              \
      if constexpr (std::is_same_v<Delim, char>) {            \
        *buffer++ = delim;                                    \
      } else {                                                \
        buffer = std::ranges::copy(delim, buffer).out;        \
      }                                                       \
    }                                                         \
    buffer = std::ranges::copy(cur_name, buffer).out;         \
  } while (0)

#define REFLECT_CPP26_APPEND_ENUM_FLAGS_NAME_REGULAR()    \
  do {                                                    \
    REFLECT_CPP26_APPEND_ENUM_FLAGS_NAME_COMMON(e.name);  \
    remaining ^= e.underlying;                            \
    if (remaining == 0) {                                 \
      return buffer - buffer_head;                        \
    }                                                     \
  } while (0)

template <class E, class Delim>
constexpr auto regular_enum_flags_name_impl(
  E flags, Delim delim, std::string_view alt) -> std::string
{
  constexpr auto decomp = enum_flags_decomposer_v<E>;
  auto remaining = zero_extend<uint64_t>(std::to_underlying(flags));
  if (remaining == 0) {
    return "";
  }
  if ((remaining & decomp.full_set) != remaining) {
    return std::string{alt};
  }

  auto reserved_size =
    decomp.sum_name_length + delim_length(delim) * decomp.units.size();
  auto res = std::string{};
  res.resize_and_overwrite(reserved_size,
    [delim, &decomp, &remaining](char* buffer, size_t buffer_size) {
      const char* buffer_head = buffer;
      template for (constexpr auto e: decomp.units) {
        if constexpr (e.popcount != 1) {
          auto intersection = remaining & e.underlying;
          if (intersection == e.underlying) {
            REFLECT_CPP26_APPEND_ENUM_FLAGS_NAME_REGULAR();
          } else if (intersection != 0) {
            return buffer - buffer_head; // Stops on failure
          }
        } else if ((remaining & e.underlying) != 0) {
          REFLECT_CPP26_APPEND_ENUM_FLAGS_NAME_REGULAR();
        }
      }
      return buffer - buffer_head;
    });
  return (remaining == 0) ? res : std::string{alt};
}

template <class E, class Delim>
constexpr auto irregular_enum_flags_name_impl(
  E flags, Delim delim, std::string_view alt) -> std::string
{
  constexpr auto decomp = enum_flags_decomposer_v<E>;
  auto underlying = zero_extend<uint64_t>(std::to_underlying(flags));
  if (underlying == 0) {
    return "";
  }
  if ((underlying & decomp.full_set) != underlying) {
    return std::string{alt};
  }

  auto covered = uint64_t{0};
  auto res = std::string{};
  auto reserved_size =
    decomp.sum_name_length + delim_length(delim) * decomp.units.size();
  res.resize_and_overwrite(reserved_size,
    [delim, underlying, &decomp, &covered](char* buffer, size_t buffer_size) {
      const char* buffer_head = buffer;
      auto vis = std::array<uint8_t, decomp.units.size()>{};

      for (auto i = 0zU, n = decomp.units.size(); i < n; i++) {
        if (vis[i]) { continue; }
        auto u = decomp.units[i].underlying;
        if ((underlying & u) == u) {
          REFLECT_CPP26_APPEND_ENUM_FLAGS_NAME_COMMON(decomp.units[i].name);
          if ((covered |= u) == underlying) {
            return buffer - buffer_head;
          }
          auto j_end = decomp.heads[i + 1];
          for (auto j = decomp.heads[i]; j < j_end; j++) {
            vis[decomp.subset_indices[j]] = true;
          }
        }
      }
      return buffer_head - buffer;
    });
  return (covered == underlying) ? res : std::string{alt};
}

#undef REFLECT_CPP26_APPEND_ENUM_FLAGS_NAME

template <class E, class Delim>
constexpr auto enum_flags_name_impl(E flags, Delim delim, std::string_view alt)
  -> std::string
{
  if constexpr (enum_flags_is_empty_v<E>) {
    return std::to_underlying(flags) == 0 ? "" : std::string{alt};
  } else if constexpr (enum_flags_is_regular_v<E>) {
    return regular_enum_flags_name_impl(flags, delim, alt);
  } else {
    return irregular_enum_flags_name_impl(flags, delim, alt);
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
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_FLAGS_NAME_HPP
