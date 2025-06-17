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

#ifndef REFLECT_CPP26_ANNOTATIONS_VALIDATORS_HPP
#define REFLECT_CPP26_ANNOTATIONS_VALIDATORS_HPP

#include <reflect_cpp26/annotations/tags.hpp>
#include <reflect_cpp26/type_operations/comparison.hpp>
#include <reflect_cpp26/type_operations/to_string.hpp>
#include <reflect_cpp26/type_operations/to_structured.hpp>
#include <reflect_cpp26/type_traits/class_types.hpp>
#include <reflect_cpp26/type_traits/reduction.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/debug_helper.hpp>
#include <cmath>
#include <memory>
#include <optional>

namespace reflect_cpp26::annotations {
namespace impl {
struct validator_maker_tag_t {};

template <class T>
concept validator_maker = std::derived_from<T, validator_maker_tag_t>;

template <class Derived>
struct validator_no_bind : validator_tag_t, validator_maker_tag_t {
  // Dummy maker
  static constexpr auto operator()() {
    return Derived{};
  }
};

template <arithmetic_type T>
constexpr auto is_finite(T value)
{
  if constexpr (std::is_floating_point_v<T>) {
    return std::isfinite(value);
  } else {
    return true; // Integral types are always finite
  }
}

template <class T, class U>
constexpr auto make_boundary_test_error_message(
  std::string_view op, const T& boundary, const U& actual_value) -> std::string
{
  auto res = std::string{"Expects value "};
  res += op;
  res += ' ';
  res += generic_to_display_string(boundary, "specified boundary");
  if constexpr (is_generic_to_string_invocable_v<U>) {
    res += ", while actual value = ";
    res += generic_to_display_string(actual_value);
  }
  return res;
}

template <class T, class U>
constexpr auto make_generic_options_test_error_message(
  std::string_view any_or_none, const T& list, const U& actual_value)
  -> std::string
{
  auto res = std::string{"Expects value to be "};
  res += any_or_none;
  res += " of ";
  res += generic_to_display_string(list, "specified values");
  if constexpr (is_generic_to_string_invocable_v<U>) {
    res += ", while actual value = ";
    res += generic_to_display_string(actual_value);
  }
  return res;
}

template <class CharT, class U>
constexpr auto make_char_options_test_error_message(
  std::string_view any_or_none, meta_basic_string_view<CharT> list,
  const U& actual_value) -> std::string
{
  auto as_span = std::span{list.head, list.tail};
  return make_generic_options_test_error_message(
    any_or_none, as_span, actual_value);
}

template <auto Comp>
constexpr auto operand_notation = compile_error("Invalid comparator");

#define REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(Comp, notation)     \
  template <>                                                         \
  constexpr auto operand_notation<Comp> = std::string_view{notation};

REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::is_lt, "<")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::is_lteq, "<=")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::is_gt, ">")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::is_gteq, ">=")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::is_eq, "==")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::is_neq, "!=")

REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::ranges::min_element, "mininum")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::ranges::max_element, "maximum")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::ranges::all_of, "all")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::ranges::any_of, "any")
REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION(std::ranges::none_of, "none")

#undef REFLECT_CPP26_SPECIALIZE_OPERAND_NOTATION
} // namespace impl

template <class T, class InputT>
concept validator_of = is_validator_v<T> &&
  requires (const T& validator, const InputT& input) {
    { validator.test(input) } -> std::same_as<bool>;
  };

// ---- Leaf validators ----

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
    constexpr auto op = impl::operand_notation<Comp>;
    return impl::make_boundary_test_error_message(op, boundary, value);
  }
};

#define REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(F)  \
  F(min, std::is_gteq)                                \
  F(max, std::is_lteq)                                \
  F(min_exclusive, std::is_gt)                        \
  F(max_exclusive, std::is_lt)                        \
  F(equals_to, std::is_eq)                            \
  F(not_equal_to, std::is_neq)

#define REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR(name, op) \
  template <class T>                                      \
  using name##_validator_t = boundary_test_t<op, T>;

REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(
  REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR)

#undef REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR

struct size_is_validator_t : validator_tag_t {
  size_t expected_size;

  // O(1) for sized range, O(n) otherwise.
  template <std::ranges::forward_range InputT>
  constexpr bool test(const InputT& input) const {
    return std::ranges::distance(input) == expected_size;
  }

  template <std::ranges::forward_range InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    auto size = std::ranges::distance(input);
    return std::string{"Expects size to be "} + to_string(expected_size)
      + ", while actual size is " + to_string(size);
  }
};

template <class OptionT>
struct options_range_t : validator_tag_t {
  meta_span<OptionT> options;

  template <generic_equal_comparable_with<OptionT> InputT>
  constexpr bool test(const InputT& input) const
  {
    return std::ranges::any_of(options, [&input](const OptionT& value) {
      return generic_equal(input, value);
    });
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_generic_options_test_error_message(
      "any", options, input);
  }
};

template <class CharT>
struct options_chars_t : validator_tag_t {
  meta_basic_string_view<CharT> char_options;

  constexpr bool test(CharT input) const {
    return std::ranges::contains(char_options, input);
  }

  constexpr auto make_error_message(CharT input) const -> std::string
  {
    return impl::make_char_options_test_error_message(
      "any", char_options, input);
  }
};

template <class OptionT>
struct excludes_range_t : validator_tag_t {
  meta_span<OptionT> excluded;

  template <generic_equal_comparable_with<OptionT> InputT>
  constexpr bool test(const InputT& input) const
  {
    return std::ranges::none_of(excluded, [&input](const OptionT& value) {
      return generic_equal(input, value);
    });
  }

  template <class InputT>
    requires (!generic_equal_comparable_with<InputT, OptionT>
            && generic_equal_comparable_with<InputT, meta_span<OptionT>>)
  constexpr bool test(const InputT& input) const
  {
    return !generic_equal(input, excluded);
  }

  template <class InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    return impl::make_generic_options_test_error_message(
      "none", excluded, input);
  }
};

template <class CharT>
struct excludes_chars_t : validator_tag_t {
  meta_basic_string_view<CharT> excluded_chars;

  constexpr bool test(CharT input) const {
    return !std::ranges::contains(excluded_chars, input);
  }

  constexpr bool test(std::basic_string_view<CharT> input) const {
    return excluded_chars != input;
  }

  constexpr auto make_error_message(CharT input) const -> std::string
  {
    return impl::make_char_options_test_error_message(
      "none", excluded_chars, input);
  }
};

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

#define REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR_T(name, cond)       \
  struct name##_validator_t : impl::validator_no_bind<name##_validator_t> { \
    template <arithmetic_type InputT>                                       \
    constexpr bool test(InputT input) const { return (cond); }              \
                                                                            \
    template <arithmetic_type InputT>                                       \
    constexpr auto make_error_message(InputT input) const -> std::string    \
    {                                                                       \
      return "Expects value to meet the condition '" #name "', "            \
             "but actual value = " + to_display_string(input);              \
    }                                                                       \
  };

REFLECT_CPP26_ARITHMETIC_RANGE_VALIDATOR_FOR_EACH(
  REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR_T)

#undef REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR_T

struct is_not_null_validator_t
  : impl::validator_no_bind<is_not_null_validator_t>
{
  static constexpr bool test(const volatile void* ptr) {
    return ptr != nullptr;
  }

  static constexpr auto make_error_message(const volatile void*)
    -> std::string {
    return "Expects pointer to be non-null.";
  }

  template <class T>
  static bool test(const std::shared_ptr<T>& ptr) {
    return ptr.get() != nullptr;
  }

  template <class T>
  static auto make_error_message(const std::shared_ptr<T>&) -> std::string {
    return "Expects shared pointer to be non-null.";
  }

  template <class T, class D>
  static bool test(const std::unique_ptr<T, D>& ptr) {
    return ptr.get() != nullptr;
  }

  template <class T, class D>
  static auto make_error_message(const std::unique_ptr<T, D>&) -> std::string {
    return "Expects unique pointer to be non-null.";
  }

  template <class T>
  static bool test(const std::weak_ptr<T>& ptr) {
    return !ptr.expired();
  }

  template <class T>
  static auto make_error_message(const std::weak_ptr<T>&) -> std::string {
    return "Expects weak pointer to be non-null.";
  }

  static bool test(const std::any& a) {
    return a.has_value();
  }

  static auto make_error_message(const std::any&) -> std::string {
    return "Expects std::any to be non-null.";
  }

  template <class T>
  static constexpr bool test(const std::optional<T>& opt) {
    return opt.has_value();
  }

  template <class T>
  static constexpr auto make_error_message(const std::optional<T>&)
    -> std::string {
    return "Expects std::optional to be non-null.";
  }
};

struct is_not_empty_validator_t
  : impl::validator_no_bind<is_not_empty_validator_t>
{
  template <std::ranges::forward_range T>
  static constexpr bool test(const T& input) {
    return !std::ranges::empty(input);
  }

  template <std::ranges::forward_range T>
  static constexpr auto make_error_message(const T& input) -> std::string {
    return "Expects input range to be non-empty.";
  }
};

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

template <class Func>
struct custom_validator_t : validator_tag_t {
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

// ---- Compound validators ----

template <validator_of<size_t> Nested>
struct size_validator_t : validator_tag_t {
  Nested nested;

  // O(1) for sized range, O(n) otherwise.
  template <std::ranges::forward_range InputT>
  constexpr bool test(const InputT& input) const
  {
    return nested.test(std::ranges::distance(input));
  }

  template <std::ranges::forward_range InputT>
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    auto size = std::ranges::distance(input);
    return "Invalid size -> " + nested.make_error_message(size);
  }
};

template <class Nested>
struct front_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return nested.test(*std::ranges::cbegin(input));
  }

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    if (std::ranges::empty(input)) {
      return "Can not validate first value since input range is empty.";
    }
    auto&& front = *std::ranges::cbegin(input);
    return "Invalid first value -> " + nested.make_error_message(front);
  }
};

template <class Nested>
struct back_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::bidirectional_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    return nested.test(*std::ranges::crbegin(input));
  }

  template <std::ranges::bidirectional_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    if (std::ranges::empty(input)) {
      return "Can not validate last value since input range is empty.";
    }
    auto&& front = *std::ranges::crbegin(input);
    return "Invalid last value -> " + nested.make_error_message(front);
  }
};

template <auto MinOrMaxElementFn, class Nested>
struct min_or_max_element_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr bool test(const InputT& input) const
  {
    if (std::ranges::empty(input)) {
      return false;
    }
    auto&& min_or_max = *MinOrMaxElementFn(input);
    return nested.test(min_or_max);
  }

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    constexpr auto op = impl::operand_notation<MinOrMaxElementFn>;
    if (std::ranges::empty(input)) {
      return std::string{"Can not validate "} + op
        + " value since input range is empty.";
    }
    auto res = std::string{"Invalid "} + op + " value -> ";
    res += nested.make_error_message(*MinOrMaxElementFn(input));
    return res;
  }
};

template <class Nested>
using min_element_validator_t =
  min_or_max_element_validator_t<std::ranges::min_element, Nested>;

template <class Nested>
using max_element_validator_t =
  min_or_max_element_validator_t<std::ranges::max_element, Nested>;

template <auto ForEachFn, class Nested>
struct for_each_of_validator_t : validator_tag_t {
  Nested nested;

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr bool test(const InputT& input) const
  {
    return ForEachFn(input, [this](const auto& cur) {
      return nested.test(cur);
    });
  }

  template <std::ranges::forward_range InputT>
    requires (validator_of<Nested, std::ranges::range_value_t<InputT>>)
  constexpr auto make_error_message(const InputT& input) const -> std::string
  {
    constexpr auto op = impl::operand_notation<ForEachFn>;
    auto res = std::string{"Expects "} + op
      + " of values meets given condition, but ";
    if constexpr (op == impl::operand_notation<std::ranges::none_of>) {
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

// -------- Validator makers --------

template <template <class> class Which>
struct make_boundary_test_t : impl::validator_maker_tag_t {
  template <class T>
  static consteval auto operator()(const T& value) {
    return Which{.boundary = to_structured(value)};
  }

  // Lexicographical comparison with range given by std::initializer_list.
  template <class T>
  static consteval auto operator()(std::initializer_list<T> value) {
    return Which{.boundary = to_structured(value)};
  }
};

struct make_size_is_t : impl::validator_maker_tag_t {
  static consteval auto operator()(size_t n) {
    return size_is_validator_t{.expected_size = n};
  }
};

struct make_options_t : impl::validator_maker_tag_t {
  template <std::ranges::forward_range R>
  static consteval auto operator()(const R& values)
  {
    auto span_or_sv = to_structured(values);
    if constexpr (is_char_type_v<std::ranges::range_value_t<R>>) {
      return options_chars_t{.char_options = span_or_sv};
    } else {
      return options_range_t{.options = span_or_sv};
    }
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> values) {
    return operator()(std::span{values});
  }

  template <char_type CharT>
  static consteval auto operator()(const CharT* literal)
    -> options_chars_t<CharT>
  {
    auto sv = to_structured(literal);
    return options_chars_t{.char_options = sv};
  }
};

struct make_excludes_t : impl::validator_maker_tag_t {
  template <std::ranges::forward_range R>
  static consteval auto operator()(const R& values)
  {
    auto span_or_sv = to_structured(values);
    if constexpr (is_char_type_v<std::ranges::range_value_t<R>>) {
      return excludes_chars_t{.excluded_chars = span_or_sv};
    } else {
      return excludes_range_t{.excluded = span_or_sv};
    }
  }

  template <class T>
  static consteval auto operator()(std::initializer_list<T> values) {
    return operator()(std::span{values});
  }

  template <char_type CharT>
  static consteval auto operator()(const CharT* literal)
  {
    auto sv = to_structured(literal);
    return excludes_chars_t{.excluded_chars = sv};
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

struct make_custom_validator_t : impl::validator_maker_tag_t {
  template <class F>
  static consteval auto operator()(F&& func) {
    return custom_validator_t{.func = std::forward<F>(func)};
  }
};

template <impl::validator_maker LeafMaker, template <class> class... Compounds>
struct make_compound_validator_t : impl::validator_maker_tag_t {
  static constexpr auto compound_refl =
    REFLECT_CPP26_EXPAND({^^Compounds...}).reverse();

private:
  template <class... Inputs>
  static constexpr auto make(Inputs&&... inputs)
  {
    auto leaf = LeafMaker{}.operator()(std::forward<Inputs>(inputs)...);
    return compound_refl.reduce(
      []<class Nested>(const Nested& cur, auto C) {
        using ResultT = typename [: substitute(C, ^^Nested) :];
        return ResultT{.nested = cur};
      }, leaf);
  }

public:
  template <class... Inputs>
    requires (std::is_invocable_v<LeafMaker, Inputs...>)
  static consteval auto operator()(Inputs&&... inputs)
  {
    return make(std::forward<Inputs>(inputs)...);
  }

  template <class T>
    requires (std::is_invocable_v<LeafMaker, std::initializer_list<T>>)
  static consteval auto operator()(std::initializer_list<T> inputs)
  {
    return make(inputs);
  }
};

template <template <class> class... Validators>
struct compound_validator_node_t {
  template <template <class> class... Next>
  consteval auto operator>>(compound_validator_node_t<Next...>) const
  {
    return compound_validator_node_t<Validators..., Next...>{};
  }

  template <impl::validator_maker LeafMaker>
  consteval auto operator>>(LeafMaker) const
  {
    return make_compound_validator_t<LeafMaker, Validators...>{};
  }
};

// ---- Leaf validators ----

#define REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR_MAKER(name, ...)    \
  constexpr auto name = make_boundary_test_t<name##_validator_t>{};

REFLECT_CPP26_BOUNDARY_VALIDATOR_FOR_EACH(
  REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR_MAKER)

#undef REFLECT_CPP26_DEFINE_BOUNDARY_VALIDATOR_MAKER

constexpr auto size_is = make_size_is_t{};
constexpr auto options = make_options_t{};
constexpr auto excludes = make_excludes_t{};

constexpr auto is_sorted = make_is_sorted_t{};
constexpr auto is_sorted_unique = make_is_sorted_t{.checks_uniqueness = true};
constexpr auto is_ascending = is_sorted;
constexpr auto is_ascending_unique = is_sorted_unique;
constexpr auto is_descending = make_is_sorted_t{.is_descending_order = true};
constexpr auto is_descending_unique = make_is_sorted_t{
  .is_descending_order = true,
  .checks_uniqueness = true,
};
constexpr auto custom_validator = make_custom_validator_t{};

#define REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR(name, ...)  \
  constexpr auto name = name##_validator_t{};

REFLECT_CPP26_ARITHMETIC_RANGE_VALIDATOR_FOR_EACH(
  REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR)

#undef REFLECT_CPP26_DEFINE_ARITHMETIC_RANGE_VALIDATOR

constexpr auto is_not_empty = is_not_empty_validator_t{};
constexpr auto is_not_null = is_not_null_validator_t{};

// ---- Compound validators ----

constexpr auto size =
  compound_validator_node_t<size_validator_t>{};
constexpr auto front =
  compound_validator_node_t<front_validator_t>{};
constexpr auto back =
  compound_validator_node_t<back_validator_t>{};
constexpr auto min_element =
  compound_validator_node_t<min_element_validator_t>{};
constexpr auto max_element =
  compound_validator_node_t<max_element_validator_t>{};
constexpr auto all_of =
  compound_validator_node_t<all_of_validator_t>{};
constexpr auto any_of =
  compound_validator_node_t<any_of_validator_t>{};
constexpr auto none_of =
  compound_validator_node_t<none_of_validator_t>{};

/**
 * Validates all flattened public non-static data members of obj
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 */
template <partially_flattenable_class T>
constexpr bool validate_members(const T& obj)
{
  constexpr auto members = public_flattened_nsdm_v<T>.to_members();
  template for (constexpr auto m: members.values) {
    constexpr auto validators = validators_of_meta_v<m>;

    template for (constexpr auto v: validators.values) {
      constexpr auto cur_validator = extract<v>();
      if (!cur_validator.test(obj.[:m:])) {
        return false;
      }
    }
  }
  return true;
}

/**
 * Validates all flattened public non-static data members of obj
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 * Error description of the first detected validator violation
 * will be written to error_output if not nullptr.
 */
template <partially_flattenable_class T>
constexpr bool validate_members_with_error_info(
  const T& obj, std::string* error_output)
{
  if (error_output == nullptr) {
    return validate_members(obj);
  }
  constexpr auto members = public_flattened_nsdm_v<T>.to_members();
  template for (constexpr auto m: members.values) {
    constexpr auto validators = validators_of_meta_v<m>;

    template for (constexpr auto v: validators.values) {
      constexpr auto cur_validator = extract<v>();
      if (!cur_validator.test(obj.[:m:])) {
        if (error_output != nullptr) {
          *error_output += "Invalid member '";
          *error_output += identifier_of(m);
          *error_output += "': ";
          *error_output += cur_validator.make_error_message(obj.[:m:]);
        }
        return false;
      }
    }
  }
  return true;
}

/**
 * Validates all flattened public non-static data members of obj
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 * Datailed error description of all detected validator violations
 * will be written to error_output if not nullptr.
 */
template <partially_flattenable_class T>
constexpr bool validate_members_with_full_error_info(
  const T& obj, std::string* error_output)
{
  if (error_output == nullptr) {
    return validate_members(obj);
  }
  constexpr auto members = public_flattened_nsdm_v<T>.to_members();
  auto res = true;
  template for (constexpr auto m: members.values) {
    constexpr auto validators = validators_of_meta_v<m>;
    auto res_cur_value = true;

    template for (constexpr auto v: validators.values) {
      constexpr auto cur_validator = extract<v>();
      auto cur_res = cur_validator.test(obj.[:m:]);
      if (!cur_res) {
        if (res_cur_value) {
          *error_output += "Invalid member '";
          *error_output += std::meta::identifier_of(m);
          *error_output += "':\n";
        }
        *error_output += "* ";
        *error_output += cur_validator.make_error_message(obj.[:m:]);
        *error_output += '\n';
      }
      res_cur_value &= cur_res;
    }
    res &= res_cur_value;
  }
  if (!res) {
    error_output->pop_back(); // Removes the trailing '\n'
  }
  return res;
}
} // namespace reflect_cpp26::annotations

#endif // REFLECT_CPP26_ANNOTATIONS_VALIDATORS_HPP
