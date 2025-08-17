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

#ifndef REFLECT_CPP26_VALIDATORS_COMPOUND_ENUM_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_COMPOUND_ENUM_TEST_HPP

#include <reflect_cpp26/validators/impl/maker.hpp>
#include <reflect_cpp26/utils/concepts.hpp>

namespace reflect_cpp26::validators {
template <class Nested>
struct enum_underlying_validator_t : validator_tag_t {
  Nested nested;

  template <enum_type E>
  constexpr bool test(E input) const {
    return nested.test(std::to_underlying(input));
  }

  template <enum_type E>
  constexpr auto make_error_message(E input) const -> std::string
  {
    return "Invalid underlying value -> " +
      nested.make_error_message(std::to_underlying(input));
  }
};

constexpr auto underlying =
  impl::compound_validator_node_t<enum_underlying_validator_t>{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_COMPOUND_ENUM_TEST_HPP
