/**
 * Copyright (c) 2026 NoqtaBeda (noqtabeda@163.com)
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

#ifndef REFLECT_CPP26_UTILS_META_TUPLE_HPP
#define REFLECT_CPP26_UTILS_META_TUPLE_HPP

#include <charconv>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/functional_tuple.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>

namespace reflect_cpp26 {
namespace impl {
consteval auto get_meta_tuple_field_name(size_t i, size_t n) -> std::string {
  // Small tuple: first, second, third
  constexpr auto predefined = std::array{"first", "second", "third"};
  if (n <= predefined.size()) {
    return predefined[i];
  }
  // Large tuple: v1, v2, v3, ...
  char buffer[32] = {'v'};
  std::to_chars(buffer + 1, buffer + 32, i + 1);
  return buffer;
}

consteval bool is_elementwise_constructible(  //
    std::vector<std::meta::info> tuple_args,
    std::vector<std::meta::info> ctor_args) {
  auto n = tuple_args.size();
  if (ctor_args.size() != n) {
    return false;
  }
  for (auto i = 0zU; i < n; i++) {
    auto params_il = {ctor_args[i]};
    if (!is_constructible_type(tuple_args[i], params_il)) {
      return false;
    }
  }
  return true;
}

template <class T, class U>
constexpr auto is_elementwise_constructible_v =
    is_elementwise_constructible(template_arguments_of(^^T), template_arguments_of(^^U));
}  // namespace impl

template <class... Args>
struct meta_tuple {
  struct underlying_type;

  consteval {
    auto elems = std::array{^^Args...};
    auto members = std::vector<std::meta::info>{};
    auto n = elems.size();

    members.reserve(n);
    for (auto i = 0zU; i < n; i++) {
      auto options = std::meta::data_member_options{
          .name = impl::get_meta_tuple_field_name(i, n),
      };
      auto cur_member = data_member_spec(elems[i], options);
      members.push_back(cur_member);
    }
    define_aggregate(^^underlying_type, members);
  }

  constexpr meta_tuple() : elements{} {}

  template <class... CtorArgs>
    requires(impl::is_elementwise_constructible_v<type_tuple<Args...>, type_tuple<CtorArgs...>>)
  constexpr meta_tuple(CtorArgs&&... args) : elements{std::forward<CtorArgs>(args)...} {}

  constexpr meta_tuple(const meta_tuple&) = default;
  constexpr meta_tuple(meta_tuple&&) = default;

  constexpr auto operator=(const meta_tuple&) -> meta_tuple& = default;
  constexpr auto operator=(meta_tuple&&) -> meta_tuple& = default;

private:
  using self_type = meta_tuple<Args...>;

  static consteval auto ith_field(size_t i) -> std::meta::info {
    return all_direct_nonstatic_data_members_v<underlying_type>[i];
  }

public:
  template <size_t I>
  friend constexpr auto& get(meta_tuple& tuple) {
    return tuple.elements.[:ith_field(I):];
  }

  template <size_t I>
  friend constexpr const auto& get(const meta_tuple& tuple) {
    return tuple.elements.[:ith_field(I):];
  }

  template <size_t I>
  friend constexpr auto&& get(meta_tuple&& tuple) {
    return std::move(tuple.elements.[:ith_field(I):]);
  }

  template <size_t I>
  friend constexpr const auto&& get(const meta_tuple&& tuple) {
    return std::move(tuple.elements.[:ith_field(I):]);
  }

  template <class TupleLike>
    requires(is_elementwise_equal_comparable_v<self_type, TupleLike>)
  constexpr auto operator==(const TupleLike& rhs) const -> bool {
    return elementwise_equal(*this, rhs);
  }

  template <class TupleLike>
    requires(is_elementwise_compare_three_way_comparable_v<self_type, TupleLike>)
  constexpr auto operator<=>(const TupleLike& rhs) const -> std::partial_ordering {
    return elementwise_compare_three_way(*this, rhs);
  }

  underlying_type elements;
};

// Deduction guide (cvref removed)
template <class... Args>
meta_tuple(Args...) -> meta_tuple<Args...>;
}  // namespace reflect_cpp26

template <class... Args>
struct std::tuple_size<reflect_cpp26::meta_tuple<Args...>> {
  static constexpr auto value = sizeof...(Args);
};

template <size_t I, class... Args>
struct std::tuple_element<I, reflect_cpp26::meta_tuple<Args...>> {
  using type = Args...[I];
};

#endif  // REFLECT_CPP26_UTILS_META_TUPLE_HPP
