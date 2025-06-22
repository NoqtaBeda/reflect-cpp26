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

#ifndef REFLECT_CPP26_VALIDATORS_COMPOUND_MIN_MAX_ELEMENT_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_COMPOUND_MIN_MAX_ELEMENT_TEST_HPP

#include <reflect_cpp26/validators/impl/maker_common.hpp>
#include <ranges>

namespace reflect_cpp26::validators {
namespace impl {
template <auto Fn>
constexpr auto min_max_type_notation = compile_error("See below");

template <>
constexpr auto min_max_type_notation<std::ranges::min_element> = "minimum";

template <>
constexpr auto min_max_type_notation<std::ranges::max_element> = "maximum";
} // namespace impl

template <auto MinOrMaxElementFn, class Nested>
struct min_or_max_element_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    auto&& min_or_max = *MinOrMaxElementFn(input);
    return nested.test(min_or_max);
  }

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    constexpr auto op = impl::min_max_type_notation<MinOrMaxElementFn>;
    if (std::ranges::empty(input)) {
      return std::string{"Can not validate the "} + op
        + " value since input range is empty.";
    }
    auto res = std::string{"Invalid "} + op + " value -> ";
    res += nested.make_error_message(*MinOrMaxElementFn(input));
    return res;
  }
};

template <class Nested>
using min_element_validator_t =
  min_or_max_element_validator_t<std::ranges::min_element, Nested>;

template <class Nested>
using max_element_validator_t =
  min_or_max_element_validator_t<std::ranges::max_element, Nested>;

constexpr auto min_element =
  impl::compound_validator_node_t<min_element_validator_t>{};
constexpr auto max_element =
  impl::compound_validator_node_t<max_element_validator_t>{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_COMPOUND_MIN_MAX_ELEMENT_TEST_HPP
