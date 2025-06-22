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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_SIZE_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_SIZE_TEST_HPP

#include <reflect_cpp26/utils/to_string.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>
#include <ranges>

namespace reflect_cpp26::validators {
struct size_is_validator_t : validator_tag_t {
  size_t expected_size;

  // O(1) for sized range, O(n) otherwise.
  template <std::ranges::forward_range InputT>
  constexpr bool test(const InputT& input) const {
    return std::ranges::distance(input) == expected_size;
  }

  template <std::ranges::forward_range InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    auto size = std::ranges::distance(input);
    return std::string{"Expects size to be "} + to_string(expected_size)
      + ", while actual size is " + to_string(size);
  }
};

struct is_not_empty_validator_t
  : impl::validator_without_params<is_not_empty_validator_t>
{
  template <std::ranges::forward_range T>
  static constexpr bool test(const T& input) {
    return !std::ranges::empty(input);
  }

  template <std::ranges::forward_range T>
  static constexpr auto make_error_message(const T& input) -> std::string {
    return "Expects input range to be non-empty.";
  }
};

struct make_size_is_t : impl::validator_maker_tag_t {
  static consteval auto operator()(size_t n) {
    return size_is_validator_t{.expected_size = n};
  }
};

constexpr auto size_is = make_size_is_t{};
constexpr auto is_not_empty = is_not_empty_validator_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_SIZE_TEST_HPP
