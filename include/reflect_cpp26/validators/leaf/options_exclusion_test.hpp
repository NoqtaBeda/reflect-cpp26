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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_OPTIONS_EXCLUSION_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_OPTIONS_EXCLUSION_TEST_HPP

#include <reflect_cpp26/type_operations/comparison.hpp>
#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>

namespace reflect_cpp26::validators {
namespace impl {
template <class T, class U>
constexpr auto make_generic_options_test_error_message(
  std::string_view any_or_none, const T& list, const U& actual_value)
  -> std::string
{
  auto res = std::string{"Expects value to be "};
  res += any_or_none;
  res += " of ";
  res += generic_to_display_string(list, "specified values");
  if constexpr (is_generic_to_string_invocable_v<U>) {
    res += ", while actual value = ";
    res += generic_to_display_string(actual_value);
  }
  return res;
}

template <class CharT, class U>
constexpr auto make_char_options_test_error_message(
  std::string_view any_or_none, meta_basic_string_view<CharT> list,
  const U& actual_value) -> std::string
{
  auto as_span = std::span{list.head, list.tail};
  return make_generic_options_test_error_message(
    any_or_none, as_span, actual_value);
}
} // namespace impl

template <class OptionT>
struct options_range_t : validator_tag_t {
  meta_span<OptionT> options;

  template <generic_equal_comparable_with<OptionT> InputT>
  constexpr bool test(const InputT& input) const
  {
    return std::ranges::any_of(options, [&input](const OptionT& value) {
      return generic_equal(input, value);
    });
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_generic_options_test_error_message(
      "any", options, input);
  }
};

template <class CharT>
struct options_chars_t : validator_tag_t {
  meta_basic_string_view<CharT> char_options;

  constexpr bool test(CharT input) const {
    return std::ranges::contains(char_options, input);
  }

  constexpr auto make_error_message(CharT input) const -> std::string
  {
    return impl::make_char_options_test_error_message(
      "any", char_options, input);
  }
};

template <class OptionT>
struct excludes_range_t : validator_tag_t {
  meta_span<OptionT> excluded;

  template <generic_equal_comparable_with<OptionT> InputT>
  constexpr bool test(const InputT& input) const
  {
    return std::ranges::none_of(excluded, [&input](const OptionT& value) {
      return generic_equal(input, value);
    });
  }

  template <class InputT>
    requires (!generic_equal_comparable_with<InputT, OptionT>
            && generic_equal_comparable_with<InputT, meta_span<OptionT>>)
  constexpr bool test(const InputT& input) const
  {
    return !generic_equal(input, excluded);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_generic_options_test_error_message(
      "none", excluded, input);
  }
};

template <class CharT>
struct excludes_chars_t : validator_tag_t {
  meta_basic_string_view<CharT> excluded_chars;

  constexpr bool test(CharT input) const {
    return !std::ranges::contains(excluded_chars, input);
  }

  constexpr bool test(std::basic_string_view<CharT> input) const {
    return excluded_chars != input;
  }

  constexpr auto make_error_message(CharT input) const -> std::string
  {
    return impl::make_char_options_test_error_message(
      "none", excluded_chars, input);
  }
};

struct make_options_t : impl::validator_maker_tag_t {
  template <std::ranges::forward_range R>
  static consteval auto operator()(const R& values)
  {
    auto span_or_sv = to_structured(values);
    if constexpr (is_char_type_v<std::ranges::range_value_t<R>>) {
      return options_chars_t{.char_options = span_or_sv};
    } else {
      return options_range_t{.options = span_or_sv};
    }
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> values) {
    return operator()(std::span{values});
  }

  template <char_type CharT>
  static consteval auto operator()(const CharT* literal)
    -> options_chars_t<CharT>
  {
    auto sv = to_structured(literal);
    return options_chars_t{.char_options = sv};
  }
};

struct make_excludes_t : impl::validator_maker_tag_t {
  template <std::ranges::forward_range R>
  static consteval auto operator()(const R& values)
  {
    auto span_or_sv = to_structured(values);
    if constexpr (is_char_type_v<std::ranges::range_value_t<R>>) {
      return excludes_chars_t{.excluded_chars = span_or_sv};
    } else {
      return excludes_range_t{.excluded = span_or_sv};
    }
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> values) {
    return operator()(std::span{values});
  }

  template <char_type CharT>
  static consteval auto operator()(const CharT* literal)
  {
    auto sv = to_structured(literal);
    return excludes_chars_t{.excluded_chars = sv};
  }
};

constexpr auto options = make_options_t{};
constexpr auto excludes = make_excludes_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_OPTIONS_EXCLUSION_TEST_HPP
