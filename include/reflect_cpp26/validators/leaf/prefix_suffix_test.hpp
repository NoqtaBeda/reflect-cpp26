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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_PREFIX_SUFFIX_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_PREFIX_SUFFIX_TEST_HPP

#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>
#include <reflect_cpp26/validators/impl/utils.hpp>

namespace reflect_cpp26::validators {
namespace impl {
template <bool IsSuffix, std::ranges::forward_range InputT, class PrefixSuffixT>
constexpr auto make_prefix_suffix_test_error_message(
  const InputT& input, const PrefixSuffixT& prefix_or_suffix) -> std::string
{
  constexpr auto input_is_string =
    is_char_type_v<std::ranges::range_value_t<InputT>>;

  if (std::ranges::empty(input)) {
    return input_is_string ? "Input string is empty" : "Input range is empty";
  }
  auto res = std::string{input_is_string ? "Input string " : "Input range "};
  if constexpr (is_generic_to_string_invocable_v<InputT>) {
    res += generic_to_display_string(input);
    res += ' ';
  }
  res += IsSuffix ? "does not end with " : "does not start with ";
  if constexpr (is_generic_to_string_invocable_v<PrefixSuffixT>) {
    res += generic_to_display_string(prefix_or_suffix);
  } else {
    res += "specified value(s)";
  }
  return res;
}

template <bool IsSuffix, char_type CharT, class PrefixSuffixT>
constexpr auto make_prefix_suffix_test_error_message(
  const CharT* input, const PrefixSuffixT& prefix_or_suffix) -> std::string
{
  auto sv = std::basic_string_view<CharT>(input == nullptr ? "" : input);
  return make_prefix_suffix_test_error_message<IsSuffix>(sv, prefix_or_suffix);
}

template <class Range>
constexpr auto last_iter(const Range& range)
{
  if constexpr (std::ranges::bidirectional_range<Range>) {
    return std::ranges::rbegin(range);
  } else {
    auto it = std::ranges::begin(range);
    auto end = std::ranges::end(range);
    auto next = std::next(it);
    for (; next != end; it = next, next = std::next(next)) {}
    return it;
  }
}
} // namespace impl

template <class PrefixT>
struct starts_with_single_t : validator_tag_t {
  PrefixT prefix_value;

  template <impl::forward_range_comparable_with<PrefixT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return generic_equal(*std::ranges::begin(input), prefix_value);
  }

  template <class = void>
    requires (is_char_type_v<PrefixT>)
  constexpr bool test(const PrefixT* str) const
  {
    return str != nullptr && *str == prefix_value;
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_prefix_suffix_test_error_message<false>(
      input, prefix_value);
  }
};

template <class SuffixT>
struct ends_with_single_t : validator_tag_t {
  SuffixT suffix_value;

  template <impl::forward_range_comparable_with<SuffixT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return generic_equal(*impl::last_iter(input), suffix_value);
  }

  template <class = void>
    requires (is_char_type_v<SuffixT>)
  constexpr bool test(const SuffixT* str) const
  {
    if (str == nullptr || *str == '\0') {
      return false;
    }
    for (; str[1] != '\0'; ++str) {}
    return suffix_value == *str;
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_prefix_suffix_test_error_message<true>(
      input, suffix_value);
  }
};

template <class CharT>
struct starts_with_string_t : validator_tag_t {
  meta_basic_string_view<CharT> prefix;

  template <string_like_of<CharT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if constexpr (std::is_pointer_v<InputT>) {
      if (input == nullptr) { return prefix.empty(); }
    }
    auto input_sv = std::basic_string_view{input};
    return input_sv.starts_with(prefix);
  }

  template <impl::forward_range_of_string_like<CharT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    auto first_sv = std::basic_string_view{*std::ranges::begin(input)};
    return prefix == first_sv;
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_prefix_suffix_test_error_message<false>(input, prefix);
  }
};

template <class CharT>
struct ends_with_string_t : validator_tag_t {
  meta_basic_string_view<CharT> suffix;

  template <string_like_of<CharT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if constexpr (std::is_pointer_v<InputT>) {
      if (input == nullptr) { return suffix.empty(); }
    }
    auto input_sv = std::basic_string_view{input};
    return input_sv.ends_with(suffix);
  }

  template <impl::forward_range_of_string_like<CharT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    auto last_sv = std::basic_string_view{*impl::last_iter(input)};
    return suffix == last_sv;
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_prefix_suffix_test_error_message<true>(input, suffix);
  }
};

template <class PrefixT>
struct starts_with_range_t : validator_tag_t {
  meta_span<PrefixT> prefix;

  template <impl::forward_range_comparable_with<PrefixT> InputT>
  constexpr bool test(const InputT& input) const
  {
    return std::ranges::starts_with(input, prefix, generic_equal);
  }

  template <impl::forward_range_comparable_with<meta_span<PrefixT>> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return generic_equal(*std::ranges::begin(input), prefix);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_prefix_suffix_test_error_message<false>(input, prefix);
  }
};

template <class SuffixT>
struct ends_with_range_t : validator_tag_t {
  meta_span<SuffixT> suffix;

  template <impl::forward_range_comparable_with<SuffixT> InputT>
  constexpr bool test(const InputT& input) const
  {
    return std::ranges::ends_with(input, suffix, generic_equal);
  }

  template <impl::forward_range_comparable_with<meta_span<SuffixT>> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return generic_equal(*impl::last_iter(input), suffix);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_prefix_suffix_test_error_message<true>(input, suffix);
  }
};

struct make_starts_with_t : impl::validator_maker_tag_t {
  template <std::ranges::forward_range R>
  static consteval auto operator()(const R& values)
  {
    auto span_or_sv = to_structured(values);
    if constexpr (is_char_type_v<std::ranges::range_value_t<R>>) {
      return starts_with_string_t{.prefix = span_or_sv};
    } else {
      return starts_with_range_t{.prefix = span_or_sv};
    }
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> il) {
    return operator()(std::span{il});
  }

  template <non_range T>
  static consteval auto operator()(const T& value) {
    return starts_with_single_t{.prefix_value = value};
  }
};

struct make_ends_with_t : impl::validator_maker_tag_t {
  template <std::ranges::forward_range R>
  static consteval auto operator()(const R& values)
  {
    auto span_or_sv = to_structured(values);
    if constexpr (is_char_type_v<std::ranges::range_value_t<R>>) {
      return ends_with_string_t{.suffix = span_or_sv};
    } else {
      return ends_with_range_t{.suffix = span_or_sv};
    }
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> il) {
    return operator()(std::span{il});
  }

  template <non_range T>
  static consteval auto operator()(const T& value) {
    return ends_with_single_t{.suffix_value = value};
  }
};

constexpr auto starts_with = make_starts_with_t{};
constexpr auto ends_with = make_ends_with_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_PREFIX_SUFFIX_TEST_HPP
