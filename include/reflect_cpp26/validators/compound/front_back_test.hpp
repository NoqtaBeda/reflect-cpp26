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

#ifndef REFLECT_CPP26_VALIDATORS_COMPOUND_FRONT_BACK_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_COMPOUND_FRONT_BACK_TEST_HPP

#include <reflect_cpp26/validators/impl/maker.hpp>
#include <ranges>

namespace reflect_cpp26::validators {
template <class Nested>
struct front_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::forward_range InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return nested.test(*std::ranges::cbegin(input));
  }

  template <std::ranges::forward_range InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    if (std::ranges::empty(input)) {
      return "Can not validate first value since input range is empty.";
    }
    auto&& front = *std::ranges::cbegin(input);
    return "Invalid first value -> " + nested.make_error_message(front);
  }
};

template <class Nested>
struct back_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::bidirectional_range InputT>
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return nested.test(*std::ranges::crbegin(input));
  }

  template <std::ranges::bidirectional_range InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    if (std::ranges::empty(input)) {
      return "Can not validate last value since input range is empty.";
    }
    auto&& front = *std::ranges::crbegin(input);
    return "Invalid last value -> " + nested.make_error_message(front);
  }
};

constexpr auto front = impl::compound_validator_node_t<front_validator_t>{};
constexpr auto back = impl::compound_validator_node_t<back_validator_t>{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_COMPOUND_FRONT_BACK_TEST_HPP
