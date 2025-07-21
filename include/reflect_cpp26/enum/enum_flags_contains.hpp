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

#ifndef REFLECT_CPP26_ENUM_ENUM_FLAGS_CONTAINS_HPP
#define REFLECT_CPP26_ENUM_ENUM_FLAGS_CONTAINS_HPP

#include <reflect_cpp26/enum/enum_contains.hpp>
#include <reflect_cpp26/utils/tags.hpp>
#include <reflect_cpp26/enum/impl/enum_flags.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/ctype.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26 {
namespace impl {
template <class E>
constexpr bool regular_enum_flags_contains_impl(uint64_t input_underlying)
{
  constexpr const auto& decomp = enum_flags_decomposer_v<E>;
  if ((input_underlying & decomp.full_set) != input_underlying) {
    return false;
  }
  template for (constexpr auto u: decomp.units) {
    if constexpr (u.popcount > 1) {
      auto i = u.underlying & input_underlying;
      if (i != 0 && i != u.underlying) { return false; }
    }
  }
  return true;
}

template <class E>
constexpr bool irregular_enum_flags_contains_impl(uint64_t input_underlying)
{
  constexpr const auto& decomp = enum_flags_decomposer_v<E>;
  if (input_underlying == 0) {
    return true;
  }
  if ((input_underlying & decomp.full_set) != input_underlying) {
    return false;
  }
  auto covered = uint64_t{0};
  template for (constexpr auto u: decomp.units) {
    if ((u.underlying & input_underlying) == u.underlying) {
      covered |= u.underlying;
      if (covered == input_underlying) { return true; }
    }
  }
  return false;
}

template <class E>
constexpr bool enum_flags_contains_impl(uint64_t underlying)
{
  if constexpr (enum_flags_is_empty_v<E>) {
    return underlying == 0;
  } else if constexpr (enum_flags_is_regular_v<E>) {
    return regular_enum_flags_contains_impl<E>(underlying);
  } else {
    return irregular_enum_flags_contains_impl<E>(underlying);
  }
}

template <class E, class Delim>
constexpr bool enum_flags_contains_impl(std::string_view str, Delim delim)
{
  for (auto token: std::views::split(str, delim)) {
    auto trimmed = ascii_trim(token);
    if (!trimmed.empty() && !enum_contains<E>(trimmed)) { return false; }
  }
  return true;
}

template <class E, class Delim>
constexpr bool enum_flags_contains_ci_impl(std::string_view str, Delim delim)
{
  for (auto token: std::views::split(str, delim)) {
    auto trimmed = ascii_trim(token);
    if (!trimmed.empty() && !enum_contains<E>(trimmed, case_insensitive)) {
      return false;
    }
  }
  return true;
}
} // namespace impl

template <enum_type E>
constexpr bool enum_flags_contains(E flags)
{
  auto u = zero_extend<uint64_t>(std::to_underlying(flags));
  return impl::enum_flags_contains_impl<E>(u);
}

template <enum_type E>
constexpr bool enum_flags_contains(std::integral auto flags)
{
  auto u = zero_extend<uint64_t>(flags);
  return impl::enum_flags_contains_impl<std::remove_cv_t<E>>(u);
}

template <enum_type E>
constexpr bool enum_flags_contains(std::string_view str, char delim = '|')
{
  return impl::enum_flags_contains_impl<std::remove_cv_t<E>>(str, delim);
}

template <enum_type E>
constexpr bool enum_flags_contains(std::string_view str, std::string_view delim)
{
  return impl::enum_flags_contains_impl<std::remove_cv_t<E>>(str, delim);
}

template <enum_type E>
constexpr bool enum_flags_contains(
  std::string_view str, case_insensitive_tag_t, char delim = '|')
{
  return impl::enum_flags_contains_ci_impl<std::remove_cv_t<E>>(str, delim);
}

template <enum_type E>
constexpr bool enum_flags_contains(
  std::string_view str, case_insensitive_tag_t, std::string_view delim)
{
  return impl::enum_flags_contains_ci_impl<std::remove_cv_t<E>>(str, delim);
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_FLAGS_CONTAINS_HPP
