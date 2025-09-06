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
  snake_case,             // snake_case
  all_caps_snake_case,    // ALL_CAPS_SNAKE_CASE
  kebab_case,             // kebab-case
  all_caps_kebab_case,    // ALL-CAPS-KABAB-CASE
  lower_camel_case,       // lowerCamelCase
  upper_camel_case,       // UpperCamelCase
  lower_camel_snake_case, // lower_Camel_Snake_Case
  upper_camel_snake_case, // Upper_Camel_Snake_Case
  http_header_case,       // Http-Header-Case
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
constexpr void visit_identifier_segments_inner(
  std::string_view input, non_alpha_as_lower_tag_t, Visitor&& visitor)
{
  auto state = identifier_parsing_state_nal::init;
  auto head = 0zU;
  auto tail = 0zU;
  for (auto len = input.length(); tail < len; ++tail) {
    switch (state) {
      case identifier_parsing_state_nal::init:
        state = ascii_isupper(input[tail])
          ? identifier_parsing_state_nal::first_upper
          : identifier_parsing_state_nal::lower_or_digit;
        break;
      case identifier_parsing_state_nal::lower_or_digit:
        if (ascii_isupper(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head));
          state = identifier_parsing_state_nal::first_upper;
          head = tail;
        }
        break;
      case identifier_parsing_state_nal::first_upper:
        state = ascii_isupper(input[tail])
          ? identifier_parsing_state_nal::subsequent_upper
          : identifier_parsing_state_nal::lower_or_digit;
        break;
      case identifier_parsing_state_nal::subsequent_upper:
        if (!ascii_isupper(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head - 1));
          state = identifier_parsing_state_nal::lower_or_digit;
          head = tail - 1;
        }
        break;
      default:
        break; // Unreachable
    }
  }
  if (head < tail) {
    visitor.visit_non_empty_word(input.substr(head));
  }
}

template <class Visitor>
constexpr void visit_identifier_segments_inner(
  std::string_view input, non_alpha_as_upper_tag_t, Visitor&& visitor)
{
  auto state = identifier_parsing_state_nau::init;
  auto head = 0zU;
  auto tail = 0zU;
  for (auto len = input.length(); tail < len; ++tail) {
    switch (state) {
      case identifier_parsing_state_nau::init:
        state = ascii_islower(input[tail])
          ? identifier_parsing_state_nau::lower
          : identifier_parsing_state_nau::first_upper_or_digit;
        break;
      case identifier_parsing_state_nau::lower:
        if (!ascii_islower(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head));
          state = identifier_parsing_state_nau::first_upper_or_digit;
          head = tail;
        }
        break;
      case identifier_parsing_state_nau::first_upper_or_digit:
        state = ascii_islower(input[tail])
          ? identifier_parsing_state_nau::lower
          : identifier_parsing_state_nau::subsequent_upper_or_digit;
        break;
      case identifier_parsing_state_nau::subsequent_upper_or_digit:
        if (ascii_islower(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head - 1));
          state = identifier_parsing_state_nau::lower;
          head = tail - 1;
        }
        break;
      default:
        break; // Unreachable
    }
  }
  if (head < tail) {
    visitor.visit_non_empty_word(input.substr(head));
  }
}

template <class Tag, class Visitor>
constexpr bool visit_identifier_segments(
  std::string_view input, Tag tag, Visitor&& visitor)
{
  if (input.empty()) {
    return false;
  }
  auto first_char = input.front();
  if (ascii_isdigit(first_char) || !is_valid_identifier_char(first_char)) {
    return false;
  }
  auto len = input.length();
  auto head = 0zU;
  for (auto tail = 0zU; tail < len; head = tail = tail + 1) {
    for (; tail < len; ++tail) {
      if (!is_valid_identifier_char(input[tail])) {
        return false;
      }
      if (is_identifier_delimiter_char(input[tail])) {
        break; // [head, tail) contains $A-Za-z0-9 only.
      }
    }
    if (head == tail) {
      visitor.visit_empty_word();
      continue;
    }
    auto segment = input.substr(head, tail - head);
    visit_identifier_segments_inner(segment, tag, visitor);
  }
  if (head == len) {
    visitor.visit_trailing_empty_word();
  }
  return true;
}

constexpr auto make_snake_case_word(
  std::string_view non_empty_word, char* buffer, bool* first_is_lower) -> char*
{
  if (*first_is_lower) {
    *buffer++ = ascii_tolower(non_empty_word.front());
    *first_is_lower = false;
  } else {
    *buffer++ = ascii_toupper(non_empty_word.front());
  }
  for (auto i = 1zU, n = non_empty_word.length(); i < n; i++) {
    *buffer++ = ascii_tolower(non_empty_word[i]);
  }
  return buffer;
}

template <auto TransformFn>
struct snake_or_kebab_case_word_visitor_t {
  char* buffer;
  char delimiter;
  bool is_first = true;

  constexpr void visit_empty_word() {
    *buffer++ = delimiter;
  }

  constexpr void visit_trailing_empty_word() {
    if (!is_first) {
      *buffer++ = delimiter;
    }
  }

  constexpr void visit_non_empty_word(std::string_view word)
  {
    is_first ? (void)(is_first = false)
             : (void)(*buffer++ = delimiter);
    buffer = std::ranges::transform(word, buffer, TransformFn).out;
  }
};

template <auto TransformFn, class Tag>
constexpr auto to_snake_or_kebab_case_impl(
  std::string_view identifier, char delimiter, Tag tag) -> std::string
{
  auto res = std::string{};
  auto is_valid_identifier = true;
  auto reserved_size = identifier.size() * 2;
  res.resize_and_overwrite(reserved_size, [&](char* buffer_head, size_t) {
    auto visitor = snake_or_kebab_case_word_visitor_t<TransformFn>{
      .buffer = buffer_head,
      .delimiter = delimiter,
    };
    is_valid_identifier = visit_identifier_segments(identifier, tag, visitor);
    return visitor.buffer - buffer_head;
  });
  return is_valid_identifier ? res : invalid_identifier_str;
}

struct camel_case_word_visitor_t {
  static constexpr auto delimiter = '_';
  char* buffer;
  bool first_is_lower; // Whether the first word is all-lower case.
  bool is_first = true;

  constexpr void visit_empty_word() {
    *buffer++ = delimiter;
  }

  constexpr void visit_trailing_empty_word() {
    if (!is_first) {
      *buffer++ = delimiter;
    }
  }

  constexpr void visit_non_empty_word(std::string_view word) {
    buffer = make_snake_case_word(word, buffer, &first_is_lower);
    is_first = false;
  }
};

template <class Tag>
constexpr auto to_camel_case_impl(
  std::string_view identifier, bool first_is_lower, Tag tag) -> std::string
{
  auto res = std::string{};
  auto is_valid_identifier = true;
  res.resize_and_overwrite(identifier.size(), [&](char* buffer_head, size_t) {
    auto visitor = camel_case_word_visitor_t{
      .buffer = buffer_head,
      .first_is_lower = first_is_lower,
    };
    is_valid_identifier = visit_identifier_segments(identifier, tag, visitor);
    return visitor.buffer - buffer_head;
  });
  return is_valid_identifier ? res : invalid_identifier_str;
}

struct camel_snake_case_word_visitor_t {
  char* buffer;
  char delimiter;
  bool first_is_lower;
  bool is_first = true;

  constexpr void visit_empty_word() {
    *buffer++ = delimiter;
  }

  constexpr void visit_trailing_empty_word() {
    if (!is_first) {
      *buffer++ = delimiter;
    }
  }

  constexpr void visit_non_empty_word(std::string_view word)
  {
    is_first ? (void)(is_first = false)
             : (void)(*buffer++ = delimiter);
    buffer = make_snake_case_word(word, buffer, &first_is_lower);
  }
};

template <class Tag>
constexpr auto to_camel_snake_or_kebab_case_impl(
  std::string_view identifier, char delimiter, bool first_is_lower, Tag tag)
  -> std::string
{
  auto res = std::string{};
  auto is_valid_identifier = true;
  auto reserved_size = identifier.size() * 2;
  res.resize_and_overwrite(reserved_size, [&](char* buffer_head, size_t) {
    auto visitor = camel_snake_case_word_visitor_t{
      .buffer = buffer_head,
      .delimiter = delimiter,
      .first_is_lower = first_is_lower,
    };
    is_valid_identifier = visit_identifier_segments(identifier, tag, visitor);
    return visitor.buffer - buffer_head;
  });
  return is_valid_identifier ? res : invalid_identifier_str;
}
} // namespace impl

#define REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(name, transform, delim)    \
  constexpr std::string to_##name##_case(                               \
    std::string_view identifier, non_alpha_as_lower_tag_t tag)          \
  {                                                                     \
    return impl::to_snake_or_kebab_case_impl<transform>(                \
      identifier, delim, tag);                                          \
  }                                                                     \
                                                                        \
  constexpr std::string to_##name##_case(                               \
    std::string_view identifier, non_alpha_as_upper_tag_t tag)          \
  {                                                                     \
    return impl::to_snake_or_kebab_case_impl<transform>(                \
      identifier, delim, tag);                                          \
  }                                                                     \
  /* Digits as lower case by default. */                                \
  constexpr std::string to_##name##_case(std::string_view identifier) { \
    return to_##name##_case(identifier, non_alpha_as_lower);            \
  }

/**
 * (a) to_snake_case(std::string_view) -> std::string
 * (b) to_snake_case(std::string_view, non_alpha_as_lower_tag_t) -> std::string
 * (c) to_snake_case(std::string_view, non_alpha_as_upper_tag_t) -> std::string
 *
 * Transforms input identifier to snake_case.
 * Input identifier is valid only if:
 * (1) it contains ASCII characters '0-9', 'A-Z', 'a-z', '-', '_' and '$' only;
 * (2) the leading character is not digit.
 * Conversion is as the following steps:
 * (1) Split input identifier as word segments;
 * (2) Convert each segment to all-lower case;
 * (3) Concatenate converted segments with '_'.
 * Word segmentation, step (1) above, is done by the following steps:
 * (1.1) Split input identifier by '-' or '_';
 * (1.2) For each segment obtained from step (1.1), split again into words.
 * Example:
 * input: "exampleInput_ParseJSONDocument_TestCase1"
 * After step (1.1): ["exampleInput", "ParseJSONDocument", "TestCase1"]
 * After step (1.2): [
 *   ["example", "Input"],
 *   ["Parse", "JSON", "Document"], // For consecutive N+1 upper-case letters,
 *                                  // the first N form a single word, then the
 *                                  // last one leads the next word.
 *   ["Test", "Case1"]
 *     or ["Test", "Case", "1"] // For overloads (a,b), non-alpha characters
 *                                 (digits and '$') are equivalent to small-case
 *                                 letters during segmentation.
 *                                 For overloads (c), non-alpha characters are
 *                                 equivalent to upper-case letters.
 * ]
 */
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(snake, ascii_tolower, '_')
/**
 * (a) to_all_caps_snake_case(std::string_view) -> std::string
 * (b) to_all_caps_snake_case(std::string_view, non_alpha_as_lower_tag_t)
 *       -> std::string
 * (c) to_all_caps_snake_case(std::string_view, non_alpha_as_upper_tag_t)
 *       -> std::string
 *
 * Transforms input identifier to ALL_CAPS_SNAKE_CASE.
 * Conversion steps (unmentioned are same as above):
 * (2) Convert each segment to all-upper case.
 */
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(all_caps_snake, ascii_toupper, '_')
/**
 * (a) to_kebab_case(std::string_view) -> std::string
 * (b) to_kebab_case(std::string_view, non_alpha_as_lower_tag_t) -> std::string
 * (c) to_kebab_case(std::string_view, non_alpha_as_upper_tag_t) -> std::string
 *
 * Transforms input identifier to kebab-case (words concatenated by hyphens).
 * Conversion steps (unmentioned are same as above):
 * (3) Concatenate converted segments with '-'.
 */
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(kebab, ascii_tolower, '-')
/**
 * (a) to_all_caps_kebab_case(std::string_view) -> std::string
 * (b) to_all_caps_kebab_case(std::string_view, non_alpha_as_lower_tag_t)
 *       -> std::string
 * (c) to_all_caps_kebab_case(std::string_view, non_alpha_as_upper_tag_t)
 *       -> std::string
 *
 * Transforms input identifier to ALL-CAPS-KEBAB-CASE.
 * Conversion steps (unmentioned are same as above):
 * (2) Convert each segment to all-upper case;
 * (3) Concatenate converted segments with '-'.
 */
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE(all_caps_kebab, ascii_toupper, '-')

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

/**
 * (a) to_lower_camel_case(std::string_view) -> std::string
 * (b) to_lower_camel_case(std::string_view, non_alpha_as_lower_tag_t)
 *       -> std::string
 * (c) to_lower_camel_case(std::string_view, non_alpha_as_upper_tag_t)
 *       -> std::string
 *
 * Transforms input identifier to lowerCamelCase.
 * Conversion steps (unmentioned are same as above):
 * (2.1) Convert the first segment to all-lower case;
 * (2.2) Convert the rest segments to leading upper-case + rest lower-case;
 * (3) Concatenate converted segments.
 */
REFLECT_CPP26_TO_CAMEL_CASE(lower_camel, true)
/**
 * (a) to_upper_camel_case(std::string_view) -> std::string
 * (b) to_upper_camel_case(std::string_view, non_alpha_as_lower_tag_t)
 *       -> std::string
 * (c) to_upper_camel_case(std::string_view, non_alpha_as_upper_tag_t)
 *       -> std::string
 *
 * Transforms input identifier to UpperCamelCase.
 * Conversion steps (unmentioned are same as above):
 * (2) Convert segments to leading upper-case + rest lower-case;
 * (3) Concatenate converted segments.
 */
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

/**
 * (a) to_lower_camel_snake_case(std::string_view) -> std::string
 * (b) to_lower_camel_snake_case(std::string_view, non_alpha_as_lower_tag_t)
 *       -> std::string
 * (c) to_lower_camel_snake_case(std::string_view, non_alpha_as_upper_tag_t)
 *       -> std::string
 *
 * Transforms input identifier to lower_Camel_Snake_Case.
 * Conversion steps (unmentioned are same as above):
 * (2.1) Convert the first segment to all-lower case;
 * (2.2) Convert the rest segments to leading upper-case + rest lower-case;
 */
REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE(lower_camel_snake, '_', true)
/**
 * (a) to_upper_camel_snake_case(std::string_view) -> std::string
 * (b) to_upper_camel_snake_case(std::string_view, non_alpha_as_lower_tag_t)
 *       -> std::string
 * (c) to_upper_camel_snake_case(std::string_view, non_alpha_as_upper_tag_t)
 *       -> std::string
 *
 * Transforms input identifier to Upper_Camel_Snake_Case.
 * Conversion steps (unmentioned are same as above):
 * (2) Convert segments to leading upper-case + rest lower-case;
 */
REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE(upper_camel_snake, '_', false)
/**
 * (a) to_http_header_case(std::string_view) -> std::string
 * (b) to_http_header_case(std::string_view, non_alpha_as_lower_tag_t)
 *       -> std::string
 * (c) to_http_header_case(std::string_view, non_alpha_as_upper_tag_t)
 *       -> std::string
 *
 * Transforms input identifier to Http-Header-Case.
 * Conversion steps (unmentioned are same as above):
 * (2) Convert segments to leading upper-case + rest lower-case;
 * (3) Concatenate converted segments with '-'.
 */
REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE(http_header, '-', false)

#undef REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE

/**
 * Transforms input identifier to specified style.
 */
constexpr auto convert_identifier(
  std::string_view identifier, identifier_naming_rule to_rule) -> std::string
{
  switch (to_rule) {
    case identifier_naming_rule::no_change: {
      auto is_valid = !identifier.empty()
        && !ascii_isdigit(identifier.front())
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
