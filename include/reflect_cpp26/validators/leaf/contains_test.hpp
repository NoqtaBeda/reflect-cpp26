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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_CONTAINS_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_CONTAINS_TEST_HPP

#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/validators/impl/maker.hpp>
#include <reflect_cpp26/validators/impl/utils.hpp>
#include <reflect_cpp26/validators/member_relation_tags.hpp>

namespace reflect_cpp26::validators {
namespace impl {
/**
 * Case (1): test(input: T[], element: U)
 * where T and U are comparable via generic_equal.
 */
template <class ElementT, impl::forward_range_comparable_with<ElementT> InputT>
constexpr bool test_contains(const InputT& input, const ElementT& element)
{
  auto pos = std::ranges::find_if(input, [&element](const auto& e) {
    return generic_equal(e, element);
  });
  return pos != std::ranges::end(input);
}

/**
 * Case (2): test(input: T[], subrange: U[])
 * where T and U are comparable via generic_equal.
 */
template <std::ranges::forward_range SubrangeT,
          generic_equal_comparable_with<SubrangeT> InputT>
constexpr bool test_contains(const InputT& input, const SubrangeT& subrange)
{
  return std::ranges::contains_subrange(input, subrange, generic_equal);
}

template <class InputT, class SubT>
constexpr bool test_contains_dispatch(const InputT& input, const SubT& sub)
{
  return test_contains(impl::to_string_view_or_identity(input),
                       impl::to_string_view_or_identity(sub));
}
} // namespace impl

template <class SubT>
struct contains_t : validator_tag_t {
  [[no_unique_address]] SubT sub;

  template <size_t I, partially_flattenable_class T>
  constexpr bool test_ith_nsdm(const T& obj) const
  {
    const auto& cur_value = get_ith_flattened_nsdm<I>(obj);
    if constexpr (impl::nsdm_relation_tag_type<SubT>) {
      const auto& sub_value = SubT::template get<I>(obj);
      return impl::test_contains_dispatch(cur_value, sub_value);
    } else {
      return impl::test_contains_dispatch(cur_value, sub);
    }
  }

  template <size_t I, class T>
  constexpr auto make_error_message_of_ith_nsdm(const T& obj) const
    -> std::string
  {
    using cur_nsdm_type = std::remove_cvref_t<ith_flattened_nsdm_type_t<I, T>>;
    constexpr auto input_is_string = is_string_like_v<cur_nsdm_type>;

    const auto& cur_value = get_ith_flattened_nsdm<I>(obj);
    auto is_empty = impl::is_empty(cur_value);
    if (is_empty != impl::is_empty_result::non_empty) {
      return input_is_string ? "Input string is empty" : "Input range is empty";
    }
    auto res = std::string{input_is_string ? "Input string " : "Input range "};
    if constexpr (is_generic_to_string_invocable_v<cur_nsdm_type>) {
      res += generic_to_display_string(cur_value);
      res += ' ';
    }
    res += "does not contain ";

    if constexpr (impl::nsdm_relation_tag_type<SubT>) {
      auto name = SubT::template get_name<T>(I);
      impl::dump_nsdm(&res, name, SubT::template get<I>(obj));
    } else {
      res += generic_to_display_string(sub, "specified value(s)");
    }
    return res;
  }
};

struct make_contains_t : impl::validator_maker_tag_t {
  template <class T>
  static consteval auto operator()(const T& sub)
  {
    // Including the cases with relation tags
    return contains_t{.sub = to_structured(sub)};
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> il) {
    return operator()(std::span{il});
  }
};

constexpr auto contains = make_contains_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_CONTAINS_TEST_HPP
