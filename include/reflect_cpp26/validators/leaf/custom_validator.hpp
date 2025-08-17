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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_CUSTOM_VALIDATOR_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_CUSTOM_VALIDATOR_HPP

#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/validators/impl/trivial_validator.hpp>

namespace reflect_cpp26::validators {
template <class Func>
struct custom_validator_t : impl::trivial_validator<custom_validator_t<Func>> {
  Func func;

  template <class InputT>
    requires (std::is_invocable_r_v<bool, Func, InputT>)
  constexpr bool test(const InputT& input) const
  {
    return func(input);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    if constexpr (is_generic_to_string_invocable_v<InputT>) {
      return "Custom validator fails with value "
        + generic_to_display_string(input);
    } else {
      return "Custom validator fails.";
    }
  }
};

struct make_custom_validator_t : impl::validator_maker_tag_t {
  template <class F>
  static consteval auto operator()(F&& func) {
    return custom_validator_t{.func = std::forward<F>(func)};
  }
};

constexpr auto custom_validator = make_custom_validator_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_CUSTOM_VALIDATOR_HPP
