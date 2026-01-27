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

#ifndef REFLECT_CPP26_UTILS_IDENTIFIER_NAMING_HPP
#define REFLECT_CPP26_UTILS_IDENTIFIER_NAMING_HPP

#include <algorithm>
#include <reflect_cpp26/utils/config.hpp>
#include <reflect_cpp26/utils/ctype.hpp>
#include <string>
#include <string_view>

namespace reflect_cpp26 {
enum class identifier_naming_rule {
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
constexpr auto is_valid_identifier_char_table = []() {
  auto res = std::array<bool, 128>{};
  for (auto i = 'A'; i <= 'Z'; i++) res[i] = true;
  for (auto i = 'a'; i <= 'z'; i++) res[i] = true;
  for (auto i = '0'; i <= '9'; i++) res[i] = true;
  for (auto i : {'_', '-', '$'}) res[i] = true;
  return res;
}();

constexpr bool is_valid_identifier_char(char c) {
  return c >= 0 && c <= 127 && is_valid_identifier_char_table[c];
}

constexpr bool is_identifier_delimiter_char(char c) {
  return c == '-' || c == '_';
}
}  // namespace impl

constexpr bool is_valid_identifier(std::string_view identifier) {
  if (identifier.empty()) [[unlikely]] {
    return false;
  }
  auto first_char = identifier[0];
  if (ascii_isdigit(first_char)) [[unlikely]] {
    return false;
  }
  return std::ranges::all_of(identifier, impl::is_valid_identifier_char);
}

namespace impl {
enum class identifier_parsing_state_nal {
  init,
  lower_or_digit,
  first_upper,
  subsequent_upper,
};

enum class identifier_parsing_state_nau {
  init,
  lower,
  first_upper_or_digit,
  subsequent_upper_or_digit,
};

template <class Visitor>
constexpr void visit_identifier_segments_inner(non_alpha_as_lower_tag_t,
                                               std::string_view input,
                                               Visitor&& visitor) {
  using enum identifier_parsing_state_nal;
  auto state = init;
  auto head = 0zU;
  auto tail = 0zU;
  for (auto len = input.length(); tail < len; ++tail) {
    switch (state) {
      case init:
        state = ascii_isupper(input[tail]) ? first_upper : lower_or_digit;
        break;
      case lower_or_digit:
        if (ascii_isupper(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head));
          state = first_upper;
          head = tail;
        }
        break;
      case first_upper:
        state = ascii_isupper(input[tail]) ? subsequent_upper : lower_or_digit;
        break;
      case subsequent_upper:
        if (!ascii_isupper(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head - 1));
          state = lower_or_digit;
          head = tail - 1;
        }
        break;
      default:
        break;
    }
  }
  if (head < tail) {
    visitor.visit_non_empty_word(input.substr(head));
  }
}

template <class Visitor>
constexpr void visit_identifier_segments_inner(non_alpha_as_upper_tag_t,
                                               std::string_view input,
                                               Visitor&& visitor) {
  using enum identifier_parsing_state_nau;
  auto state = init;
  auto head = 0zU;
  auto tail = 0zU;
  for (auto len = input.length(); tail < len; ++tail) {
    switch (state) {
      case init:
        state = ascii_islower(input[tail]) ? lower : first_upper_or_digit;
        break;
      case lower:
        if (!ascii_islower(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head));
          state = first_upper_or_digit;
          head = tail;
        }
        break;
      case first_upper_or_digit:
        state = ascii_islower(input[tail]) ? lower : subsequent_upper_or_digit;
        break;
      case subsequent_upper_or_digit:
        if (ascii_islower(input[tail])) {
          visitor.visit_non_empty_word(input.substr(head, tail - head - 1));
          state = lower;
          head = tail - 1;
        }
        break;
      default:
        break;
    }
  }
  if (head < tail) {
    visitor.visit_non_empty_word(input.substr(head));
  }
}

template <class Tag, class Visitor>
constexpr void visit_identifier_segments(Tag tag, std::string_view input, Visitor&& visitor) {
  auto len = input.length();
  auto head = 0zU;
  for (auto tail = 0zU; tail < len; head = tail = tail + 1) {
    for (; tail < len; ++tail) {
      if (is_identifier_delimiter_char(input[tail])) break;
    }
    if (head == tail) {
      visitor.visit_empty_word();
      continue;
    }
    auto segment = input.substr(head, tail - head);
    visit_identifier_segments_inner(tag, segment, visitor);
  }
  if (head == len) {
    visitor.visit_trailing_empty_word();
  }
}

constexpr auto make_snake_case_word(std::string_view non_empty_word,
                                    char* buffer,
                                    bool* first_is_lower) -> char* {
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

  constexpr void visit_non_empty_word(std::string_view word) {
    is_first ? (void)(is_first = false) : (void)(*buffer++ = delimiter);
    buffer = std::ranges::transform(word, buffer, TransformFn).out;
  }
};

template <auto TransformFn, class Tag>
constexpr auto to_snake_or_kebab_case_impl(Tag tag, std::string_view identifier, char delimiter)
    -> std::optional<std::string> {
  if (!is_valid_identifier(identifier)) [[unlikely]] {
    return std::nullopt;
  }
  auto res = std::string{};
  auto reserved_size = identifier.size() * 2;
  res.resize_and_overwrite(reserved_size, [&](char* buffer_head, size_t) {
    auto visitor = snake_or_kebab_case_word_visitor_t<TransformFn>{
        .buffer = buffer_head,
        .delimiter = delimiter,
    };
    visit_identifier_segments(tag, identifier, visitor);
    return visitor.buffer - buffer_head;
  });
  return {std::move(res)};
}

struct camel_case_word_visitor_t {
  static constexpr auto delimiter = '_';
  char* buffer;
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

  constexpr void visit_non_empty_word(std::string_view word) {
    buffer = make_snake_case_word(word, buffer, &first_is_lower);
    is_first = false;
  }
};

template <class Tag>
constexpr auto to_camel_case_impl(Tag tag, std::string_view identifier, bool first_is_lower)
    -> std::optional<std::string> {
  if (!is_valid_identifier(identifier)) [[unlikely]] {
    return std::nullopt;
  }
  auto res = std::string{};
  res.resize_and_overwrite(identifier.size(), [&](char* buffer_head, size_t) {
    auto visitor = camel_case_word_visitor_t{
        .buffer = buffer_head,
        .first_is_lower = first_is_lower,
    };
    visit_identifier_segments(tag, identifier, visitor);
    return visitor.buffer - buffer_head;
  });
  return {std::move(res)};
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

  constexpr void visit_non_empty_word(std::string_view word) {
    is_first ? (void)(is_first = false) : (void)(*buffer++ = delimiter);
    buffer = make_snake_case_word(word, buffer, &first_is_lower);
  }
};

template <class Tag>
constexpr auto to_camel_snake_or_kebab_case_impl(Tag tag,
                                                 std::string_view identifier,
                                                 char delimiter,
                                                 bool first_is_lower)
    -> std::optional<std::string> {
  if (!is_valid_identifier(identifier)) [[unlikely]] {
    return std::nullopt;
  }
  auto res = std::string{};
  auto reserved_size = identifier.size() * 2;
  res.resize_and_overwrite(reserved_size, [&](char* buffer_head, size_t) {
    auto visitor = camel_snake_case_word_visitor_t{
        .buffer = buffer_head,
        .delimiter = delimiter,
        .first_is_lower = first_is_lower,
    };
    visit_identifier_segments(tag, identifier, visitor);
    return visitor.buffer - buffer_head;
  });
  return std::optional{std::move(res)};
}
}  // namespace impl

#define REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE_FUNCTOR(name, transform, delim)                      \
  struct to_##name##_case_opt_t {                                                                 \
    static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)   \
        -> std::optional<std::string> {                                                           \
      return impl::to_snake_or_kebab_case_impl<transform>(tag, identifier, delim);                \
    }                                                                                             \
    static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)   \
        -> std::optional<std::string> {                                                           \
      return impl::to_snake_or_kebab_case_impl<transform>(tag, identifier, delim);                \
    }                                                                                             \
    static constexpr auto operator()(std::string_view identifier) -> std::optional<std::string> { \
      return operator()(non_alpha_as_lower, identifier);                                          \
    }                                                                                             \
  };                                                                                              \
  struct to_##name##_case_t {                                                                     \
    static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)   \
        -> std::string {                                                                          \
      return to_##name##_case_opt_t::operator()(tag, identifier).value_or("");                    \
    }                                                                                             \
    static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)   \
        -> std::string {                                                                          \
      return to_##name##_case_opt_t::operator()(tag, identifier).value_or("");                    \
    }                                                                                             \
    static constexpr auto operator()(std::string_view identifier) -> std::string {                \
      return operator()(non_alpha_as_lower, identifier);                                          \
    }                                                                                             \
  };                                                                                              \
  inline constexpr auto to_##name##_case_opt = to_##name##_case_opt_t{};                          \
  inline constexpr auto to_##name##_case = to_##name##_case_t{};

REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE_FUNCTOR(snake, ascii_tolower, '_')
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE_FUNCTOR(all_caps_snake, ascii_toupper, '_')
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE_FUNCTOR(kebab, ascii_tolower, '-')
REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE_FUNCTOR(all_caps_kebab, ascii_toupper, '-')

#undef REFLECT_CPP26_TO_SNAKE_OR_KEBAB_CASE_FUNCTOR

#define REFLECT_CPP26_TO_CAMEL_CASE_FUNCTOR(name, first_lower)                                    \
  struct to_##name##_case_opt_t {                                                                 \
    static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)   \
        -> std::optional<std::string> {                                                           \
      return impl::to_camel_case_impl(tag, identifier, first_lower);                              \
    }                                                                                             \
    static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)   \
        -> std::optional<std::string> {                                                           \
      return impl::to_camel_case_impl(tag, identifier, first_lower);                              \
    }                                                                                             \
    static constexpr auto operator()(std::string_view identifier) -> std::optional<std::string> { \
      return operator()(non_alpha_as_lower, identifier);                                          \
    }                                                                                             \
  };                                                                                              \
  struct to_##name##_case_t {                                                                     \
    static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)   \
        -> std::string {                                                                          \
      return to_##name##_case_opt_t::operator()(tag, identifier).value_or("");                    \
    }                                                                                             \
    static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)   \
        -> std::string {                                                                          \
      return to_##name##_case_opt_t::operator()(tag, identifier).value_or("");                    \
    }                                                                                             \
    static constexpr auto operator()(std::string_view identifier) -> std::string {                \
      return operator()(non_alpha_as_lower, identifier);                                          \
    }                                                                                             \
  };                                                                                              \
  inline constexpr auto to_##name##_case_opt = to_##name##_case_opt_t{};                          \
  inline constexpr auto to_##name##_case = to_##name##_case_t{};

REFLECT_CPP26_TO_CAMEL_CASE_FUNCTOR(lower_camel, true)
REFLECT_CPP26_TO_CAMEL_CASE_FUNCTOR(upper_camel, false)

#undef REFLECT_CPP26_TO_CAMEL_CASE_FUNCTOR

#define REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE_FUNCTOR(name, delim, first_upper)              \
  struct to_##name##_case_opt_t {                                                                 \
    static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)   \
        -> std::optional<std::string> {                                                           \
      return impl::to_camel_snake_or_kebab_case_impl(tag, identifier, delim, first_upper);        \
    }                                                                                             \
    static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)   \
        -> std::optional<std::string> {                                                           \
      return impl::to_camel_snake_or_kebab_case_impl(tag, identifier, delim, first_upper);        \
    }                                                                                             \
    static constexpr auto operator()(std::string_view identifier) -> std::optional<std::string> { \
      return operator()(non_alpha_as_lower, identifier);                                          \
    }                                                                                             \
  };                                                                                              \
  struct to_##name##_case_t {                                                                     \
    static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)   \
        -> std::string {                                                                          \
      return to_##name##_case_opt_t::operator()(tag, identifier).value_or("");                    \
    }                                                                                             \
    static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)   \
        -> std::string {                                                                          \
      return to_##name##_case_opt_t::operator()(tag, identifier).value_or("");                    \
    }                                                                                             \
    static constexpr auto operator()(std::string_view identifier) -> std::string {                \
      return operator()(non_alpha_as_lower, identifier);                                          \
    }                                                                                             \
  };                                                                                              \
  inline constexpr auto to_##name##_case_opt = to_##name##_case_opt_t{};                          \
  inline constexpr auto to_##name##_case = to_##name##_case_t{};

REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE_FUNCTOR(lower_camel_snake, '_', true)
REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE_FUNCTOR(upper_camel_snake, '_', false)
REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE_FUNCTOR(http_header, '-', false)

#undef REFLECT_CPP26_TO_SNAKE_CAMEL_OR_KEBAB_CASE_FUNCTOR

struct convert_identifier_t {
  static constexpr auto operator()(std::string_view identifier, identifier_naming_rule to_rule)
      -> std::string {
    switch (to_rule) {
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
        return "";
    }
  }
};

struct convert_identifier_opt_t {
  static constexpr auto operator()(std::string_view identifier, identifier_naming_rule to_rule)
      -> std::optional<std::string> {
    switch (to_rule) {
      case identifier_naming_rule::snake_case:
        return to_snake_case_opt(identifier);
      case identifier_naming_rule::all_caps_snake_case:
        return to_all_caps_snake_case_opt(identifier);
      case identifier_naming_rule::kebab_case:
        return to_kebab_case_opt(identifier);
      case identifier_naming_rule::all_caps_kebab_case:
        return to_all_caps_kebab_case_opt(identifier);
      case identifier_naming_rule::lower_camel_case:
        return to_lower_camel_case_opt(identifier);
      case identifier_naming_rule::upper_camel_case:
        return to_upper_camel_case_opt(identifier);
      case identifier_naming_rule::lower_camel_snake_case:
        return to_lower_camel_snake_case_opt(identifier);
      case identifier_naming_rule::upper_camel_snake_case:
        return to_upper_camel_snake_case_opt(identifier);
      case identifier_naming_rule::http_header_case:
        return to_http_header_case_opt(identifier);
      default:
        REFLECT_CPP26_ERROR_IF_CONSTEVAL("Invalid naming rule.");
        return std::nullopt;
    }
  }
};

inline constexpr auto convert_identifier = convert_identifier_t{};
inline constexpr auto convert_identifier_opt = convert_identifier_opt_t{};

}  // namespace reflect_cpp26
#endif
