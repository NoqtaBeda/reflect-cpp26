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

#ifndef REFLECT_CPP26_UTILS_IDENTIFIER_NAMING_HPP
#define REFLECT_CPP26_UTILS_IDENTIFIER_NAMING_HPP

#include <reflect_cpp26/utils/config.h>
#include <reflect_cpp26/utils/ctype.hpp>
#include <algorithm>
#include <string>
#include <string_view>

namespace reflect_cpp26 {
enum class identifier_naming_rule {
  no_change,
  snake_case,
  all_caps_snake_case,
  kebab_case,
  all_caps_kebab_case,
  lower_camel_case,
  upper_camel_case,
  lower_camel_snake_case,
  upper_camel_snake_case,
  http_header_case,
};

struct non_alpha_as_lower_tag_t {};
struct non_alpha_as_upper_tag_t {};

constexpr auto non_alpha_as_lower = non_alpha_as_lower_tag_t{};
constexpr auto non_alpha_as_upper = non_alpha_as_upper_tag_t{};

namespace impl {
// nal: Non-Alpha as Lower
enum class identifier_parsing_state_nal {
  init,
  lower_or_digit,
  first_upper,
  subsequent_upper,
};

// nau: Non-Alpha as Upper
enum class identifier_parsing_state_nau {
  init,
  lower,
  first_upper_or_digit,
  subsequent_upper_or_digit,
};

constexpr const char* invalid_identifier_str = "<invalid-identifier>";
constexpr const char* invalid_naming_rule_str = "<invalid-naming-rule>";

constexpr auto is_valid_identifier_char_table = []() {
  auto res = std::array<bool, 128>{};
  for (auto i = 'A'; i <= 'Z'; i++) { res[i] = true; }
  for (auto i = 'a'; i <= 'z'; i++) { res[i] = true; }
  for (auto i = '0'; i <= '9'; i++) { res[i] = true; }
  // '-' for kebab-case, '$' for other programming languages.
  for (auto i: {'_', '-', '$'}) { res[i] = true; }
  return res;
}();

constexpr bool is_valid_identifier_char(char c) {
  return c >= 0 && c <= 127 && is_valid_identifier_char_table[c];
}

constexpr bool is_identifier_delimiter_char(char c) {
  return c == '-' || c == '_';
}

template <class Visitor>
constexpr void visit_identifier_segments_inner_nal(
  std::string_view input, Visitor&& visitor)
{
  auto state = identifier_parsing_state_nal::init;
  auto head = 0zU;
  auto tail = 0zU;
  for (auto len = input.length(); tail < len; ++tail) {
    switch (state) {
      case identifier_parsing_state_nal::init:
        state = isupper(input[tail])
          ? identifier_parsing_state_nal::first_upper
          : identifier_parsing_state_nal::lower_or_digit;
        break;
      case identifier_parsing_state_nal::lower_or_digit:
        if (isupper(input[tail])) {
          visitor(input.substr(head, tail - head));
          state = identifier_parsing_state_nal::first_upper;
          head = tail;
        }
        break;
      case identifier_parsing_state_nal::first_upper:
        state = isupper(input[tail])
          ? identifier_parsing_state_nal::subsequent_upper
          : identifier_parsing_state_nal::lower_or_digit;
        break;
      case identifier_parsing_state_nal::subsequent_upper:
        if (!isupper(input[tail])) {
          visitor(input.substr(head, tail - head - 1));
          state = identifier_parsing_state_nal::lower_or_digit;
          head = tail - 1;
        }
        break;
      default:
        REFLECT_CPP26_UNREACHABLE("Unexpected state.");
    }
  }
  if (head < tail) {
    visitor(input.substr(head));
  }
}

template <class Visitor>
constexpr void visit_identifier_segments_inner_nau(
  std::string_view input, Visitor&& visitor)
{
  auto state = identifier_parsing_state_nau::init;
  auto head = 0zU;
  auto tail = 0zU;
  for (auto len = input.length(); tail < len; ++tail) {
    switch (state) {
      case identifier_parsing_state_nau::init:
        state = islower(input[tail])
          ? identifier_parsing_state_nau::lower
          : identifier_parsing_state_nau::first_upper_or_digit;
        break;
      case identifier_parsing_state_nau::lower:
        if (!islower(input[tail])) {
          visitor(input.substr(head, tail - head));
          state = identifier_parsing_state_nau::first_upper_or_digit;
          head = tail;
        }
        break;
      case identifier_parsing_state_nau::first_upper_or_digit:
        state = islower(input[tail])
          ? identifier_parsing_state_nau::lower
          : identifier_parsing_state_nau::subsequent_upper_or_digit;
        break;
      case identifier_parsing_state_nau::subsequent_upper_or_digit:
        if (islower(input[tail])) {
          visitor(input.substr(head, tail - head - 1));
          state = identifier_parsing_state_nau::lower;
          head = tail - 1;
        }
        break;
      default:
        REFLECT_CPP26_UNREACHABLE("Unexpected state.");
    }
  }
  if (head < tail) {
    visitor(input.substr(head));
  }
}

template <class Visitor>
constexpr void visit_identifier_segments_inner(
  std::string_view input, non_alpha_as_lower_tag_t, Visitor&& visitor)
{
  visit_identifier_segments_inner_nal(input, std::forward<Visitor>(visitor));
}

template <class Visitor>
constexpr void visit_identifier_segments_inner(
  std::string_view input, non_alpha_as_upper_tag_t, Visitor&& visitor)
{
  visit_identifier_segments_inner_nau(input, std::forward<Visitor>(visitor));
}

template <class Tag, class Visitor>
constexpr bool visit_identifier_segments(
  std::string_view input, Tag tag, Visitor&& visitor)
{
  if (input.empty()) {
    REFLECT_CPP26_ERROR_IF_CONSTEVAL("Empty string is invalid.");
    return false;
  }
  auto first_char = input.front();
  if (isdigit(first_char) || !is_valid_identifier_char(first_char)) {
    REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid first character.");
    return false;
  }
  auto len = input.length();
  auto head = 0zU;
  for (auto tail = 0zU; tail < len; head = tail = tail + 1) {
    for (; tail < len; ++tail) {
      if (!is_valid_identifier_char(input[tail])) {
        REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid identifier character.");
        return false;
      }
      if (is_identifier_delimiter_char(input[tail])) {
        break; // [head, tail) contains $A-Za-z0-9 only.
      }
    }
    if (head == tail) {
      visitor(std::string_view{}); // Empty segment
      continue;
    }
    auto segment = input.substr(head, tail - head);
    visit_identifier_segments_inner(segment, tag, visitor);
  }
  if (head == len) {
    visitor(std::string_view{}); // Trailing empty segment
  }
  return true;
}

constexpr auto make_snake_case_segment(
  std::string_view segment, char* buffer, bool& first_is_lower) -> char*
{
  if (segment.empty()) {
    return buffer;
  }
  if (first_is_lower) {
    *buffer++ = tolower(segment.front());
    first_is_lower = false;
  } else {
    *buffer++ = toupper(segment.front());
  }
  for (auto i = 1zU, n = segment.length(); i < n; i++) {
    *buffer++ = tolower(segment[i]);
  }
  return buffer;
}

template <auto TransformFn, class Tag>
constexpr auto to_snake_or_kebab_case_impl(
  std::string_view identifier, char delimiter, Tag tag) -> std::string
{
  auto res = std::string{};
  auto is_valid_identifier = true;
  res.resize_and_overwrite(identifier.size() * 2, [&](char* buffer, size_t) {
    const char* buffer_head = buffer;
    auto is_first = true;
    is_valid_identifier &= visit_identifier_segments(identifier, tag,
      [&](std::string_view seg) {
        is_first ? (void)(is_first = false)
                 : (void)(*buffer++ = delimiter);
        buffer = std::ranges::transform(seg, buffer, TransformFn).out;
      });
    return buffer - buffer_head;
  });
  return is_valid_identifier ? res : invalid_identifier_str;
}

template <class Tag>
constexpr auto to_camel_case_impl(
  std::string_view identifier, bool first_is_lower, Tag tag) -> std::string
{
  auto res = std::string{};
  auto is_valid_identifier = true;
  res.resize_and_overwrite(identifier.size(), [&](char* buffer, size_t) {
    const char* buffer_head = buffer;
    is_valid_identifier &= visit_identifier_segments(identifier, tag,
      [&](std::string_view seg) {
        if (!seg.empty()) {
          buffer = make_snake_case_segment(seg, buffer, first_is_lower);
          return;
        }
        *buffer++ = '_'; // Keeps leading and trailing delimiters
      });
    return buffer - buffer_head;
  });
  return is_valid_identifier ? res : invalid_identifier_str;
}

template <class Tag>
constexpr auto to_camel_snake_or_kebab_case_impl(
  std::string_view identifier, char delimiter, bool first_is_lower, Tag tag)
  -> std::string
{
  auto res = std::string{};
  auto is_valid_identifier = true;
  res.resize_and_overwrite(identifier.size() * 2, [&](char* buffer, size_t) {
    const char* buffer_head = buffer;
    auto is_first = true;
    is_valid_identifier &= visit_identifier_segments(identifier, tag,
      [&](std::string_view seg) {
        is_first ? (void)(is_first = false)
                 : (void)(*buffer++ = delimiter);
        buffer = make_snake_case_segment(seg, buffer, first_is_lower);
      });
    return buffer - buffer_head;
  });
  return is_valid_identifier ? res : invalid_identifier_str;
}
} // namespace impl

#define REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(name, transform, delim)    \
  constexpr std::string to_##name##_case(                               \
    std::string_view identifier, non_alpha_as_lower_tag_t tag)          \
  {                                                                     \
    return impl::to_snake_or_kebab_case_impl<transform<char>>(          \
      identifier, delim, tag);                                          \
  }                                                                     \
                                                                        \
  constexpr std::string to_##name##_case(                               \
    std::string_view identifier, non_alpha_as_upper_tag_t tag)          \
  {                                                                     \
    return impl::to_snake_or_kebab_case_impl<transform<char>>(          \
      identifier, delim, tag);                                          \
  }                                                                     \
  /* Digits as lower case by default. */                                \
  constexpr std::string to_##name##_case(std::string_view identifier) { \
    return to_##name##_case(identifier, non_alpha_as_lower);            \
  }

REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(snake, tolower, '_')
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(all_caps_snake, toupper, '_')
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(kebab, tolower, '-')
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(all_caps_kebab, toupper, '-')

#undef REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE

#define REFLECT_CPP26_TO_CAMEL_CASE(name, first_lower)                  \
  constexpr std::string to_##name##_case(                               \
    std::string_view identifier, non_alpha_as_lower_tag_t tag)          \
  {                                                                     \
    return impl::to_camel_case_impl(identifier, first_lower, tag);      \
  }                                                                     \
                                                                        \
  constexpr std::string to_##name##_case(                               \
    std::string_view identifier, non_alpha_as_upper_tag_t tag)          \
  {                                                                     \
    return impl::to_camel_case_impl(identifier, first_lower, tag);      \
  }                                                                     \
  /* Digits as lower case by default. */                                \
  constexpr std::string to_##name##_case(std::string_view identifier) { \
    return to_##name##_case(identifier, non_alpha_as_lower);            \
  }

REFLECT_CPP26_TO_CAMEL_CASE(lower_camel, true)
REFLECT_CPP26_TO_CAMEL_CASE(upper_camel, false)

#undef REFLECT_CPP26_TO_CAMEL_CASE

#define REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE(name, delim, first_upper)  \
  constexpr std::string to_##name##_case(                                     \
    std::string_view identifier, non_alpha_as_lower_tag_t tag)                \
  {                                                                           \
    return impl::to_camel_snake_or_kebab_case_impl(                           \
      identifier, delim, first_upper, tag);                                   \
  }                                                                           \
                                                                              \
  constexpr std::string to_##name##_case(                                     \
    std::string_view identifier, non_alpha_as_upper_tag_t tag)                \
  {                                                                           \
    return impl::to_camel_snake_or_kebab_case_impl(                           \
      identifier, delim, first_upper, tag);                                   \
  }                                                                           \
  /* Digits as lower case by default. */                                      \
  constexpr std::string to_##name##_case(std::string_view identifier) {       \
    return to_##name##_case(identifier, non_alpha_as_lower);                  \
  }

REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE(lower_camel_snake, '_', true)
REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE(upper_camel_snake, '_', false)
REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE(http_header, '-', false)

#undef REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE

constexpr auto convert_identifier(
  std::string_view identifier, identifier_naming_rule to_rule) -> std::string
{
  switch (to_rule) {
    case identifier_naming_rule::no_change: {
      auto is_valid = !identifier.empty()
        && !isdigit(identifier.front())
        && std::ranges::all_of(identifier, impl::is_valid_identifier_char);
      return is_valid ? std::string{identifier}
                      : std::string{impl::invalid_identifier_str};
    }
    case identifier_naming_rule::snake_case:
      return to_snake_case(identifier);
    case identifier_naming_rule::all_caps_snake_case:
      return to_all_caps_snake_case(identifier);
    case identifier_naming_rule::kebab_case:
      return to_kebab_case(identifier);
    case identifier_naming_rule::all_caps_kebab_case:
      return to_all_caps_kebab_case(identifier);
    case identifier_naming_rule::lower_camel_case:
      return to_lower_camel_case(identifier);
    case identifier_naming_rule::upper_camel_case:
      return to_upper_camel_case(identifier);
    case identifier_naming_rule::lower_camel_snake_case:
      return to_lower_camel_snake_case(identifier);
    case identifier_naming_rule::upper_camel_snake_case:
      return to_upper_camel_snake_case(identifier);
    case identifier_naming_rule::http_header_case:
      return to_http_header_case(identifier);
    default:
      REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid naming rule.");
      return impl::invalid_naming_rule_str;
  }
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_IDENTIFIER_NAMING_HPP
