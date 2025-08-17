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

#include <reflect_cpp26/type_operations/member_access.hpp>
#include <reflect_cpp26/validators/impl/maker.hpp>

#ifndef REFLECT_CPP26_VALIDATORS_IMPL_TRIVIAL_VALIDATOR_HPP
#define REFLECT_CPP26_VALIDATORS_IMPL_TRIVIAL_VALIDATOR_HPP

namespace reflect_cpp26::validators::impl {
template <class Derived>
struct trivial_validator : validator_tag_t {
  template <size_t I, class T>
  constexpr bool test_ith_nsdm(const T& obj) const
  {
    const auto& input = get_ith_flattened_nsdm<I>(obj);
    return static_cast<const Derived*>(this)->test(input);
  }

  template <size_t I, class T>
  constexpr auto make_error_message_of_ith_nsdm(const T& obj) const
    -> std::string
  {
    const auto& input = get_ith_flattened_nsdm<I>(obj);
    return static_cast<const Derived*>(this)->make_error_message(input);
  }
};

template <class Derived>
struct trivial_validator_without_params
  : trivial_validator<Derived>, validator_maker_tag_t
{
  // Dummy maker
  static constexpr auto operator()() {
    return Derived{};
  }
};
} // namespace reflect_cpp26::validators::impl

#endif // REFLECT_CPP26_VALIDATORS_IMPL_TRIVIAL_VALIDATOR_HPP
