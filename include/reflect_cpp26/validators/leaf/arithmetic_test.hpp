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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_ARITHMETIC_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_ARITHMETIC_TEST_HPP

#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/to_string.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>
#include <cmath>

namespace reflect_cpp26::validators {
namespace impl {
template <arithmetic_type T>
constexpr auto is_finite(T value)
{
  if constexpr (std::is_floating_point_v<T>) {
    return std::isfinite(value);
  } else {
    return true; // Integral types are always finite
  }
}
} // namespace impl

#define REFLECT_CPP26_ARITHMETIC_RANGE_VALIDATOR_FOR_EACH(F)      \
  F(is_positive, input > 0)                                       \
  F(is_negative, input < 0)                                       \
  F(is_non_positive, input <= 0)                                  \
  F(is_non_negative, input >= 0)                                  \
  F(is_finite, impl::is_finite(input))                            \
  F(is_finite_positive, impl::is_finite(input) && input > 0)      \
  F(is_finite_negative, impl::is_finite(input) && input < 0)      \
  F(is_finite_non_positive, impl::is_finite(input) && input <= 0) \
  F(is_finite_non_negative, impl::is_finite(input) && input >= 0)

#define REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR_T(name, cond)     \
  struct name##_validator_t                                               \
      : impl::validator_without_params<name##_validator_t> {              \
    template <arithmetic_type InputT>                                     \
    constexpr bool test(InputT input) const { return (cond); }            \
                                                                          \
    template <arithmetic_type InputT>                                     \
    constexpr auto make_error_message(InputT input) const -> std::string  \
    {                                                                     \
      return "Expects value to meet the condition '" #name "', "          \
             "but actual value = " + to_display_string(input);            \
    }                                                                     \
  };                                                                      \
  constexpr auto name = name##_validator_t{};

// See above
REFLECT_CPP26_ARITHMETIC_RANGE_VALIDATOR_FOR_EACH(
  REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR_T)

#undef REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR_T
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_ARITHMETIC_TEST_HPP
