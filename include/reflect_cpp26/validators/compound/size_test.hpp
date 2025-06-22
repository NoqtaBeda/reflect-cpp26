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

#ifndef REFLECT_CPP26_VALIDATORS_COMPOUND_SIZE_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_COMPOUND_SIZE_TEST_HPP

#include <reflect_cpp26/validators/impl/maker_common.hpp>
#include <ranges>

namespace reflect_cpp26::validators {
template <validator_of<size_t> Nested>
struct size_validator_t : validator_tag_t {
  Nested nested;

  // O(1) for sized range, O(n) otherwise.
  template <std::ranges::forward_range InputT>
  constexpr bool test(const InputT& input) const
  {
    return nested.test(std::ranges::distance(input));
  }

  template <std::ranges::forward_range InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    auto size = std::ranges::distance(input);
    return "Invalid size -> " + nested.make_error_message(size);
  }
};

constexpr auto size = impl::compound_validator_node_t<size_validator_t>{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_COMPOUND_SIZE_TEST_HPP
