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

#ifndef REFLECT_CPP26_ENUM_ENUM_FLAGS_CAST_HPP
#define REFLECT_CPP26_ENUM_ENUM_FLAGS_CAST_HPP

#include <reflect_cpp26/enum/enum_cast.hpp>
#include <reflect_cpp26/enum/enum_flags_contains.hpp>
#include <reflect_cpp26/utils/tags.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26 {
namespace impl {
template <bool CaseInsensitive, class E, class Delim>
constexpr auto enum_flags_cast_impl(std::string_view str, Delim delim)
  -> std::optional<E>
{
  auto underlying = std::underlying_type_t<E>{0};
  for (auto token: std::views::split(str, delim)) {
    auto trimmed = ascii_trim(token);
    if (trimmed.empty()) {
      continue;
    }
    auto opt = [trimmed] constexpr {
      if constexpr (CaseInsensitive) {
        return enum_cast<E>(ascii_case_insensitive, trimmed);
      } else {
        return enum_cast<E>(trimmed);
      }
    }();
    if (!opt.has_value()) {
      return std::nullopt;
    }
    underlying |= std::to_underlying(*opt);
  }
  return static_cast<E>(underlying);
}
} // namespace impl

template <enum_type E>
struct enum_flags_cast_t {
private:
  using ENoCV = std::remove_cv_t<E>;

public:
  /**
   * Returns the enum flag value if input str can be decomposed as enum entry
   * names split by given delimiter, or std::nullopt otherwise.
   *
   * Input segments are trimmed such that leading and trailing ASCII space
   * characters (' ', '\n', '\t', etc.) are removed. Example:
   *   str = "first | second | third\n", delim = '|', then
   *   segments = ["first", "second", "third"].
   */
  static constexpr auto operator()(std::string_view str, char delim = '|')
    -> std::optional<ENoCV>
  {
    return impl::enum_flags_cast_impl<false, ENoCV>(str, delim);
  }

  /**
   * Returns the enum flag value if input str can be decomposed as enum entry
   * names split by given delimiter, or std::nullopt otherwise.
   */
  static constexpr auto operator()(std::string_view str, std::string_view delim)
    -> std::optional<ENoCV>
  {
    return impl::enum_flags_cast_impl<false, ENoCV>(str, delim);
  }

  /**
   * Returns the enum flag value if input str can be decomposed as enum entry
   * names split by given delimiter, or std::nullopt otherwise. ASCII
   * case-insensitive string comparison is applied.
   */
  static constexpr auto operator()(
    ascii_case_insensitive_tag_t, std::string_view str, char delim = '|')
    -> std::optional<ENoCV>
  {
    return impl::enum_flags_cast_impl<true, ENoCV>(str, delim);
  }

  /**
   * Returns the enum flag value if input str can be decomposed as enum entry
   * names split by given delimiter, or std::nullopt otherwise. ASCII
   * case-insensitive string comparison is applied.
   */
  static constexpr auto operator()(
    ascii_case_insensitive_tag_t, std::string_view str,
    std::string_view delim) -> std::optional<ENoCV>
  {
    return impl::enum_flags_cast_impl<true, ENoCV>(str, delim);
  }

  /**
   * Casts value to E if value can be decomposed as disjunction of enum entries
   * in E, or returns std::nullopt otherwise.
   */
  static constexpr auto operator()(std::integral auto value)
    -> std::optional<ENoCV>
  {
    if (enum_flags_contains<ENoCV>(value)) {
      return static_cast<ENoCV>(value);
    }
    return std::nullopt;
  }

  /**
   * Bind expression is supported.
   * Example: enum_flags_cast<E>(_1) is equivalent to
   * std::bind(enum_flags_cast<E>, _1).
   */
  REFLECT_CPP26_FUNCTOR_BIND_VARIADIC(enum_flags_cast_t<E>)
};

template <enum_type E>
constexpr auto enum_flags_cast = enum_flags_cast_t<std::remove_cv_t<E>>{};
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_ENUM_ENUM_FLAGS_CAST_HPP
