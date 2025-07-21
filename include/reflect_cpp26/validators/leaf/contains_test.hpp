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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_CONTAINS_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_CONTAINS_TEST_HPP

#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>
#include <reflect_cpp26/validators/impl/utils.hpp>

namespace reflect_cpp26::validators {
namespace impl {
template <std::ranges::forward_range InputT, class SubT>
constexpr auto make_contains_test_error_message(
  const InputT& input, const SubT& sub) -> std::string
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
  res += "does not contain ";
  if constexpr (is_generic_to_string_invocable_v<SubT>) {
    res += generic_to_display_string(sub);
  } else {
    res += "specified value(s)";
  }
  return res;
}

template <char_type CharT, class SubT>
constexpr auto make_contains_test_error_message(
  const CharT* input, const SubT& sub) -> std::string
{
  auto sv = std::basic_string_view{input == nullptr ? "" : input};
  return make_contains_test_error_message(sv, sub);
}
} // namespace impl

template <class T>
struct contains_single_t : validator_tag_t {
  T value;

  template <impl::forward_range_comparable_with<T> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    auto pos = std::ranges::find_if(input, [this](const auto& elem) {
      return generic_equal(elem, value);
    });
    return pos != std::ranges::end(input);
  }

  template <class = void>
    requires (is_char_type_v<T>)
  constexpr bool test(const T* input) const
  {
    if (input == nullptr) {
      return false;
    }
    auto sv = std::basic_string_view<T>{input};
    return sv.contains(value);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_contains_test_error_message(input, value);
  }
};

template <class CharT>
struct contains_string_t : validator_tag_t {
  meta_basic_string_view<CharT> substring;

  template <string_like_of<CharT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if constexpr (std::is_pointer_v<InputT>) {
      if (input == nullptr) { return substring.empty(); }
    }
    auto input_sv = std::basic_string_view{input};
    return input_sv.contains(substring);
  }

  template <impl::forward_range_of_string_like<CharT> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return std::ranges::contains(input, substring, [](const auto& s) {
      return std::basic_string_view{s};
    });
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_contains_test_error_message(input, substring);
  }
};

template <class T>
struct contains_range_t : validator_tag_t {
  meta_span<T> subrange;

  template <impl::forward_range_comparable_with<T> InputT>
  constexpr bool test(const InputT& input) const
  {
    return std::ranges::contains_subrange(input, subrange, generic_equal);
  }

  template <impl::forward_range_comparable_with<meta_span<T>> InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    auto pos = std::ranges::find_if(input, [this](const auto& elem) {
      return generic_equal(elem, subrange);
    });
    return pos != std::ranges::end(input);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_contains_test_error_message(input, subrange);
  }
};

struct make_contains_t : impl::validator_maker_tag_t {
  template <std::ranges::forward_range R>
  static consteval auto operator()(const R& values)
  {
    auto span_or_sv = to_structured(values);
    if constexpr (is_char_type_v<std::ranges::range_value_t<R>>) {
      return contains_string_t{.substring = span_or_sv};
    } else {
      return contains_range_t{.subrange = span_or_sv};
    }
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> il) {
    return operator()(std::span{il});
  }

  template <non_range T>
  static consteval auto operator()(const T& value) {
    return contains_single_t{.value = value};
  }
};

constexpr auto contains = make_contains_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_CONTAINS_TEST_HPP
