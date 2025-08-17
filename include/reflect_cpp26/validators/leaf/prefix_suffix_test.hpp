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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_PREFIX_SUFFIX_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_PREFIX_SUFFIX_TEST_HPP

#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/validators/impl/maker.hpp>
#include <reflect_cpp26/validators/impl/utils.hpp>
#include <reflect_cpp26/validators/member_relation_tags.hpp>

namespace reflect_cpp26::validators {
namespace impl {
template <class Range>
constexpr auto last_iter(const Range& range)
{
  if constexpr (std::ranges::bidirectional_range<Range>) {
    return std::ranges::rbegin(range);
  } else {
    auto it = std::ranges::begin(range);
    auto end = std::ranges::end(range);
    auto next = std::next(it);
    for (; next != end; it = next, next = std::next(next)) {}
    return it;
  }
}

/**
 * Case (1): test(input: T[], prefix_or_suffix: U)
 * where T and U are comparable via generic_equal.
 */
template <bool IsSuffix, class PrefixSuffixT,
          impl::forward_range_comparable_with<PrefixSuffixT> InputT>
constexpr bool test_prefix_or_suffix(
  const InputT& input, const PrefixSuffixT& prefix_or_suffix)
{
  if (std::ranges::empty(input)) {
    return false;
  }
  if constexpr (IsSuffix) {
    return generic_equal(*impl::last_iter(input), prefix_or_suffix);
  } else {
    return generic_equal(*std::ranges::begin(input), prefix_or_suffix);
  }
}

/**
 * Case (2): test(input: T[], prefix_or_suffix: U[])
 * where T and U or CharU are comparable via generic_equal.
 */
template <bool IsSuffix, std::ranges::forward_range PrefixSuffixT,
          generic_equal_comparable_with<PrefixSuffixT> InputT>
constexpr bool test_prefix_or_suffix(
  const InputT& input, const PrefixSuffixT& prefix_or_suffix)
{
  if constexpr (IsSuffix) {
    return std::ranges::ends_with(input, prefix_or_suffix, generic_equal);
  } else {
    return std::ranges::starts_with(input, prefix_or_suffix, generic_equal);
  }
}

/**
 * Case (3): test(input: const T*, prefix_or_suffix: U[])
 * where T and U are both character types.
 */
template <bool IsSuffix, char_type InputCharT,
          impl::forward_range_comparable_with<InputCharT> PrefixSuffixT>
constexpr bool test_prefix_or_suffix(
  const InputCharT* input, const PrefixSuffixT& prefix_or_suffix)
{
  if (input == nullptr) {
    return std::ranges::empty(prefix_or_suffix);
  }
  auto input_sv = std::basic_string_view{input};
  return test_prefix_or_suffix<IsSuffix>(input_sv, prefix_or_suffix);
}

/**
 * Case (4): test(input: const T*, prefix_or_suffix: const U*)
 * where T and U are both character types.
 */
template <bool IsSuffix, char_type InputCharT, char_type PrefixSuffixCharT>
constexpr bool test_prefix_or_suffix(
  const InputCharT* input, const PrefixSuffixCharT* prefix_or_suffix)
{
  if (input == nullptr) {
    return prefix_or_suffix == nullptr || *prefix_or_suffix == '\0';
  }
  if (prefix_or_suffix == nullptr) {
    // prefix_or_suffix is considered as empty string, which is trivial prefix
    // or suffix of every string (including another empty one).
    return true;
  }
  return test_prefix_or_suffix<IsSuffix>(
    std::basic_string_view{input}, std::basic_string_view{prefix_or_suffix});
}

/**
 * Case (5): test(input: const T*, prefix_or_suffix: U)
 * where T and U are both character types.
 */
template <bool IsSuffix, char_type InputCharT, char_type PrefixSuffixCharT>
constexpr bool test_prefix_or_suffix(
  const InputCharT* input, PrefixSuffixCharT prefix_or_suffix)
{
  if (input == nullptr || *input == '\0') {
    return false;
  }
  if constexpr (IsSuffix) {
    for (; input[1] != '\0'; ++input) {}
  }
  return *input == prefix_or_suffix;
}

/**
 * Case (6): test(input: T[], prefix_or_suffix: const U*)
 * where T and U are both character types.
 */
template <bool IsSuffix, char_type PrefixSuffixCharT,
          impl::forward_range_comparable_with<PrefixSuffixCharT> InputT>
constexpr bool test_prefix_or_suffix(
  const InputT& input, const PrefixSuffixCharT* prefix_or_suffix)
{
  if (prefix_or_suffix == nullptr || *prefix_or_suffix == '\0') {
    return true;
  }
  auto sv = std::basic_string_view{prefix_or_suffix};
  return test_prefix_or_suffix<IsSuffix>(input, sv);
}

/**
 * Case (7): test(input: T[][], prefix_or_suffix: const U*)
 * where T and U are both character types.
 */
template <bool IsSuffix, char_type PrefixSuffixCharT,
          impl::forward_range_comparable_with<
            std::basic_string_view<PrefixSuffixCharT>> InputT>
constexpr bool test_prefix_or_suffix(
  const InputT& input, const PrefixSuffixCharT* prefix_or_suffix)
{
  auto sv = std::basic_string_view<PrefixSuffixCharT>{};
  if (prefix_or_suffix != nullptr) {
    sv = prefix_or_suffix;
  }
  return test_prefix_or_suffix<IsSuffix>(input, sv);
}
} // namespace impl

template <bool IsSuffix, class PrefixSuffixT>
struct starts_or_ends_with_t : validator_tag_t
{
  [[no_unique_address]] PrefixSuffixT prefix_or_suffix;

  template <size_t I, partially_flattenable_class T>
  constexpr bool test_ith_nsdm(const T& obj) const
  {
    const auto& cur_value = get_ith_flattened_nsdm<I>(obj);
    if constexpr (impl::nsdm_relation_tag_type<PrefixSuffixT>) {
      const auto& prefix_or_suffix_value = PrefixSuffixT::template get<I>(obj);
      return impl::test_prefix_or_suffix<IsSuffix>(
        cur_value, prefix_or_suffix_value);
    } else {
      return impl::test_prefix_or_suffix<IsSuffix>(cur_value, prefix_or_suffix);
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
    res += IsSuffix ? "does not end with " : "does not start with ";

    if constexpr (impl::nsdm_relation_tag_type<PrefixSuffixT>) {
      auto name = PrefixSuffixT::template get_name<T>(I);
      impl::dump_nsdm(&res, name, PrefixSuffixT::template get<I>(obj));
    } else {
      constexpr auto alt = std::string_view{"specified value(s)"};
      res += generic_to_display_string(prefix_or_suffix, alt);
    }
    return res;
  }
};

template <bool IsSuffix>
struct make_starts_or_ends_with_t : impl::validator_maker_tag_t {
  static consteval auto operator()(const auto& values)
  {
    // Including the cases with relation tags
    auto v = to_structured(values);
    using result_type = starts_or_ends_with_t<IsSuffix, decltype(v)>;
    return result_type{.prefix_or_suffix = v};
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> il) {
    return operator()(std::span{il});
  }
};

constexpr auto starts_with = make_starts_or_ends_with_t<false>{};
constexpr auto ends_with = make_starts_or_ends_with_t<true>{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_PREFIX_SUFFIX_TEST_HPP
