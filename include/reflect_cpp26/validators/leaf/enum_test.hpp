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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_ENUM_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_ENUM_TEST_HPP

#include <reflect_cpp26/enum/enum_contains.hpp>
#include <reflect_cpp26/enum/enum_flags_contains.hpp>
#include <reflect_cpp26/enum/enum_type_name.hpp>
#include <reflect_cpp26/utils/to_string.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>

namespace reflect_cpp26::validators {
struct enum_is_contained_t
  : impl::validator_without_params<enum_is_contained_t>
{
  template <enum_type E>
  static constexpr bool test(E input) {
    return enum_contains(input);
  }

  template <enum_type E>
  static constexpr auto make_error_message(E input) -> std::string
  {
    auto res = std::string{"Input enum value ("};
    res += enum_type_name<E>();
    res += ')';
    res += to_string(std::to_underlying(input));
    res += " is not an entry defined in enum type";
    return res;
  }
};

struct enum_flags_is_contained_t
  : impl::validator_without_params<enum_flags_is_contained_t>
{
  template <enum_type E>
  static constexpr bool test(E input) {
    return enum_flags_contains(input);
  }

  template <enum_type E>
  static constexpr auto make_error_message(E input) -> std::string
  {
    auto res = std::string{"Input enum value ("};
    res += enum_type_name<E>();
    res += ')';
    res += to_string(std::to_underlying(input));
    res += " is not disjunction of entries defined in enum type";
    return res;
  }
};

constexpr auto enum_is_contained = enum_is_contained_t{};
constexpr auto enum_flags_is_contained = enum_flags_is_contained_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_ENUM_TEST_HPP
