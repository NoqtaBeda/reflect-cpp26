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

#ifndef REFLECT_CPP26_VALIDATORS_COMPOUND_FOR_EACH_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_COMPOUND_FOR_EACH_TEST_HPP

#include <reflect_cpp26/validators/impl/maker.hpp>
#include <reflect_cpp26/utils/to_string.hpp>

namespace reflect_cpp26::validators {
namespace impl {
template <auto Fn>
constexpr auto for_each_type_notation = compile_error("See below");

template <>
constexpr auto for_each_type_notation<std::ranges::all_of> = "all";

template <>
constexpr auto for_each_type_notation<std::ranges::any_of> = "any";

template <>
constexpr auto for_each_type_notation<std::ranges::none_of> = "none";
} // namespace impl

template <auto ForEachFn, class Nested>
struct for_each_of_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::forward_range InputT>
  constexpr bool test(const InputT& input) const
  {
    return ForEachFn(input, [this](const auto& cur) {
      return nested.test(cur);
    });
  }

  template <std::ranges::forward_range InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    constexpr auto op = impl::for_each_type_notation<ForEachFn>;
    auto res = std::string{"Expects "} + op
      + " of values meets given condition, but ";
    if constexpr (op == impl::for_each_type_notation<std::ranges::none_of>) {
      auto count = std::ranges::count_if(input, [this](const auto& cur) {
        return nested.test(cur);
      });
      res += to_string(count) + " value(s) satisfy actually.";
    } else {
      auto count = std::ranges::count_if(input, [this](const auto& cur) {
        return !nested.test(cur);
      });
      res += to_string(count) + " value(s) dissatisfy actually.";
    }
    return res;
  }
};

template <class Nested>
using all_of_validator_t =
  for_each_of_validator_t<std::ranges::all_of, Nested>;

template <class Nested>
using any_of_validator_t =
  for_each_of_validator_t<std::ranges::any_of, Nested>;

template <class Nested>
using none_of_validator_t =
  for_each_of_validator_t<std::ranges::none_of, Nested>;

// ---- Compound validators ----

constexpr auto all_of = impl::compound_validator_node_t<all_of_validator_t>{};
constexpr auto any_of = impl::compound_validator_node_t<any_of_validator_t>{};
constexpr auto none_of =impl:: compound_validator_node_t<none_of_validator_t>{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_COMPOUND_FOR_EACH_TEST_HPP
