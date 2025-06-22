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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_SORTED_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_SORTED_TEST_HPP

#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>
#include <algorithm>
#include <ranges>

namespace reflect_cpp26::validators {
struct is_sorted_validator_t : validator_tag_t {
  bool is_descending_order = false;
  bool checks_uniqueness = false;

  template <std::ranges::forward_range T>
  constexpr bool test(const T& input) const
  {
    auto is_sorted = is_descending_order
      ? std::ranges::is_sorted(input, greater)
      : std::ranges::is_sorted(input, less);
    if (!is_sorted) {
      return false;
    }
    return !checks_uniqueness
      || std::ranges::adjacent_find(input) == std::ranges::end(input);
  }

  template <std::ranges::forward_range T>
  constexpr auto make_error_message(const T& input) const -> std::string
  {
    auto res = std::string{"Expects input range to be sorted in "};
    res += (is_descending_order ? "descending" : "ascending");
    res += " order";
    if (checks_uniqueness) {
      res += " and unique";
    }
    res += ", while actual value = " + generic_to_display_string(input);
    return res;
  }
};

struct make_is_sorted_t : impl::validator_maker_tag_t {
  bool is_descending_order = false;
  bool checks_uniqueness = false;

  consteval auto operator()() const
  {
    return is_sorted_validator_t{
      .is_descending_order = is_descending_order,
      .checks_uniqueness = checks_uniqueness
    };
  }
};

constexpr auto is_sorted = make_is_sorted_t{};
constexpr auto is_sorted_unique = make_is_sorted_t{.checks_uniqueness = true};
constexpr auto is_ascending = is_sorted;
constexpr auto is_ascending_unique = is_sorted_unique;
constexpr auto is_descending = make_is_sorted_t{.is_descending_order = true};
constexpr auto is_descending_unique = make_is_sorted_t{
  .is_descending_order = true,
  .checks_uniqueness = true,
};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_SORTED_TEST_HPP
