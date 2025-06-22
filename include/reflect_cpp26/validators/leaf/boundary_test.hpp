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

#include <reflect_cpp26/type_operations/comparison.hpp>
#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/validators/impl/maker_common.hpp>

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_BOUNDARY_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_BOUNDARY_TEST_HPP

#define REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(F)  \
  F(min, std::is_gteq, ">=")                          \
  F(max, std::is_lteq, "<=")                          \
  F(min_exclusive, std::is_gt, '>')                   \
  F(max_exclusive, std::is_lt, '<')                   \
  F(equals_to, std::is_eq, "==")                      \
  F(not_equal_to, std::is_neq, "!=")

namespace reflect_cpp26::validators {
namespace impl {
template <auto Comp>
constexpr auto boundary_test_type_notation = compile_error("Placeholder.");

#define REFLECT_CPP26_DEFINE_BOUNDARY_TEST_TYPE_NOTATION(_, Comp, notation) \
  template <>                                                               \
  constexpr auto boundary_test_type_notation<Comp> = notation;

REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(
  REFLECT_CPP26_DEFINE_BOUNDARY_TEST_TYPE_NOTATION)

#undef REFLECT_CPP26_DEFINE_BOUNDARY_TEST_TYPE_NOTATION
} // namespace impl

template <auto Comp, class BoundaryT>
struct boundary_test_t : validator_tag_t {
  BoundaryT boundary;

  template <generic_three_way_comparable_with<BoundaryT> InputT>
  constexpr bool test(const InputT& value) const
  {
    auto res_3way = generic_compare_three_way(value, boundary);
    return Comp(res_3way);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& value) const -> std::string
  {
    auto res = std::string{"Expects value "};
    res += impl::boundary_test_type_notation<Comp>;
    res += ' ';
    res += generic_to_display_string(boundary, "specified boundary");
    if constexpr (is_generic_to_string_invocable_v<InputT>) {
      res += ", while actual value = ";
      res += generic_to_display_string(value);
    }
    return res;
  }
};

template <auto Comp>
struct make_boundary_test_t : impl::validator_maker_tag_t {
  template <class T>
  static consteval auto operator()(const T& value)
  {
    auto boundary = to_structured(value);
    return boundary_test_t<Comp, decltype(boundary)>{.boundary = boundary};
  }

  // Lexicographical comparison with range given by std::initializer_list.
  template <class T>
  static consteval auto operator()(std::initializer_list<T> value)
  {
    auto boundary = to_structured(value);
    return boundary_test_t<Comp, decltype(boundary)>{.boundary = boundary};
  }
};

#define REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR(name, op, _)  \
  constexpr auto name = make_boundary_test_t<op>{};

// See above
REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(
  REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR)

#undef REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_BOUNDARY_TEST_HPP
