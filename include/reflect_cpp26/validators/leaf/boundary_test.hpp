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
#include <reflect_cpp26/validators/impl/maker.hpp>
#include <reflect_cpp26/validators/impl/utils.hpp>
#include <reflect_cpp26/validators/member_relation_tags.hpp>

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_BOUNDARY_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_BOUNDARY_TEST_HPP

#define REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(F)  \
  F(min, std::is_gteq, ">=")                          \
  F(max, std::is_lteq, "<=")                          \
  F(min_exclusive, std::is_gt, '>')                   \
  F(max_exclusive, std::is_lt, '<')                   \
  F(equal_to, std::is_eq, "==")                       \
  F(not_equal_to, std::is_neq, "!=")

namespace reflect_cpp26::validators {
namespace impl {
template <auto Comp>
constexpr auto boundary_test_type_notation = compile_error("Placeholder.");

#define REFLECT_CPP26_BOUNDARY_TYPE_NOTATION(_, Comp, notation) \
  template <> constexpr auto boundary_test_type_notation<Comp> = notation;

REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(REFLECT_CPP26_BOUNDARY_TYPE_NOTATION)
#undef REFLECT_CPP26_BOUNDARY_TYPE_NOTATION

template <auto Comp>
constexpr auto boundary_range_notation = compile_error("Placeholder.");

template <> constexpr auto boundary_range_notation<std::is_gteq> = '[';
template <> constexpr auto boundary_range_notation<std::is_gt> = '(';
template <> constexpr auto boundary_range_notation<std::is_lteq> = ']';
template <> constexpr auto boundary_range_notation<std::is_lt> = ')';
} // namespace impl

template <auto Comp, class BoundaryT>
struct boundary_test_t : validator_tag_t {
  [[no_unique_address]] BoundaryT boundary;

  template <size_t I, partially_flattenable_class T>
  constexpr bool test_ith_nsdm(const T& obj) const
  {
    auto&& value = get_ith_flattened_nsdm<I>(obj);
    if constexpr (impl::nsdm_relation_tag_type<BoundaryT>) {
      auto&& boundary_value = BoundaryT::template get<I>(obj);
      auto res_3way = generic_compare_three_way(value, boundary_value);
      return Comp(res_3way);
    } else {
      auto res_3way = generic_compare_three_way(value, boundary);
      return Comp(res_3way);
    }
  }

  template <size_t I, class T>
  constexpr auto make_error_message_of_ith_nsdm(const T& obj) const
    -> std::string
  {
    auto res = std::string{"Expects value "};
    res += impl::boundary_test_type_notation<Comp>;
    res += ' ';
    if constexpr (impl::nsdm_relation_tag_type<BoundaryT>) {
      impl::dump_nsdm_by_relation_tag<I, BoundaryT>(&res, obj);
    } else {
      res += generic_to_display_string(boundary, "specified boundary");
    }

    using cur_nsdm_type = ith_flattened_nsdm_type_t<I, T>;
    if constexpr (is_generic_to_string_invocable_v<cur_nsdm_type>) {
      auto&& value = get_ith_flattened_nsdm<I>(obj);
      res += ", while actual value = ";
      res += generic_to_display_string(value);
    }
    return res;
  }
};

template <auto CompLeft, auto CompRight, class LeftT, class RightT>
struct boundary_range_test_t : validator_tag_t {
  [[no_unique_address]] LeftT left_boundary;
  [[no_unique_address]] RightT right_boundary;

private:
  static consteval auto get_left_boundary_type(
    size_t cur_index, std::meta::info T) -> std::meta::info
  {
    if constexpr (impl::nsdm_relation_tag_type<LeftT>) {
      auto I = std::meta::reflect_constant(cur_index);
      return substitute(^^impl::target_type_t, ^^LeftT, I, T);
    } else {
      return ^^LeftT;
    }
  }

  static consteval auto get_right_boundary_type(
    size_t cur_index, std::meta::info T) -> std::meta::info
  {
    if constexpr (impl::nsdm_relation_tag_type<RightT>) {
      auto I = std::meta::reflect_constant(cur_index);
      return substitute(^^impl::target_type_t, ^^RightT, I, T);
    } else {
      return ^^RightT;
    }
  }

public:
  template <size_t I, partially_flattenable_class T>
  constexpr bool test_ith_nsdm(const T& obj) const
  {
    auto&& value = get_ith_flattened_nsdm<I>(obj);
    auto res_left = [this, &obj, &value]() {
      if constexpr (impl::nsdm_relation_tag_type<LeftT>) {
        auto&& left_value = LeftT::template get<I>(obj);
        return generic_compare_three_way(value, left_value);
      } else {
        return generic_compare_three_way(value, this->left_boundary);
      }
    }();
    if (!CompLeft(res_left)) {
      return false;
    }
    auto res_right = [this, &obj, &value]() {
      if constexpr (impl::nsdm_relation_tag_type<RightT>) {
        auto&& right_value = RightT::template get<I>(obj);
        return generic_compare_three_way(value, right_value);
      } else {
        return generic_compare_three_way(value, this->right_boundary);
      }
    }();
    return CompRight(res_right);
  }

  template <size_t I, class T>
  constexpr auto make_error_message_of_ith_nsdm(const T& obj) const
    -> std::string
  {
    using left_boundary_type = [: get_left_boundary_type(I, ^^T) :];
    using right_boundary_type = [: get_right_boundary_type(I, ^^T) :];

    auto res = std::string{"Expects value to fall in "};
    if constexpr (is_generic_to_string_invocable_v<left_boundary_type> &&
                  is_generic_to_string_invocable_v<right_boundary_type>) {
      res += impl::boundary_range_notation<CompLeft>;
      if constexpr (impl::nsdm_relation_tag_type<LeftT>) {
        impl::dump_nsdm_by_relation_tag<I, LeftT>(&res, obj);
      } else {
        res += generic_to_display_string(this->left_boundary);
      }
      res += ", ";
      if constexpr (impl::nsdm_relation_tag_type<RightT>) {
        impl::dump_nsdm_by_relation_tag<I, RightT>(&res, obj);
      } else {
        res += generic_to_display_string(this->right_boundary);
      }
      res += impl::boundary_range_notation<CompRight>;
    } else {
      res += "specified range";
    }

    using cur_nsdm_type = ith_flattened_nsdm_type_t<I, T>;
    if constexpr (is_generic_to_string_invocable_v<cur_nsdm_type>) {
      auto&& value = get_ith_flattened_nsdm<I>(obj);
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
    // Including the case where T is member relation tag
    using result_type = boundary_test_t<Comp, to_structured_t<T>>;
    return result_type{.boundary = to_structured(value)};
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> value)
  {
    // Lexicographical comparison with range given by std::initializer_list.
    return operator()(std::span{value});
  }
};

template <auto CompLeft, auto CompRight>
struct make_boundary_range_test_t : impl::validator_maker_tag_t {
  template <class LeftT, class RightT>
  static consteval auto operator()(const LeftT& left, const RightT& right)
  {
    // Including the cases where LeftT and/or RightT is member relation tag
    using result_type = boundary_range_test_t<
      CompLeft, CompRight, to_structured_t<LeftT>, to_structured_t<RightT>>;
    return result_type{
      .left_boundary = to_structured(left),
      .right_boundary = to_structured(right),
    };
  }

  template <class LeftT, class RightT>
  static consteval auto operator()(
    std::initializer_list<LeftT> left, std::initializer_list<RightT> right)
  {
    return operator()(std::span{left}, std::span{right});
  }

  template <class LeftT, class RightT>
  static consteval auto operator()(
    std::initializer_list<LeftT> left, const RightT& right)
  {
    return operator()(std::span{left}, right);
  }

  template <class LeftT, class RightT>
  static consteval auto operator()(
    const LeftT& left, std::initializer_list<RightT> right)
  {
    return operator()(left, std::span{right});
  }
};

// See above
#define REFLECT_CPP26_BOUNDARY_VALIDATOR(name, op, _)  \
  constexpr auto name = make_boundary_test_t<op>{};

REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(REFLECT_CPP26_BOUNDARY_VALIDATOR)
#undef REFLECT_CPP26_BOUNDARY_VALIDATOR

constexpr auto in_closed_range =
  make_boundary_range_test_t<std::is_gteq, std::is_lteq>{};
constexpr auto in_half_closed_range =
  make_boundary_range_test_t<std::is_gteq, std::is_lt>{};
constexpr auto in_open_range =
  make_boundary_range_test_t<std::is_gt, std::is_lt>{};

// Aliases
constexpr auto greater_equal = min;
constexpr auto less_equal = max;
constexpr auto greater_than = min_exclusive;
constexpr auto less_than = max_exclusive;

constexpr auto ge = greater_equal;
constexpr auto le = less_equal;
constexpr auto gt = greater_than;
constexpr auto lt = less_than;
constexpr auto eq = equal_to;
constexpr auto ne = not_equal_to;

#undef REFLECT_CPP26_BOUNDARY_VALIDATOR
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_BOUNDARY_TEST_HPP
