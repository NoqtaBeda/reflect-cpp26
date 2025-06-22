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

#ifndef REFLECT_CPP26_VALIDATORS_IMPL_MAKER_COMMON_HPP
#define REFLECT_CPP26_VALIDATORS_IMPL_MAKER_COMMON_HPP

#include <reflect_cpp26/utils/expand.hpp>
#include <reflect_cpp26/validators/common.hpp>
#include <concepts>

namespace reflect_cpp26::validators::impl {
struct validator_maker_tag_t {};

template <class T>
concept validator_maker = std::derived_from<T, validator_maker_tag_t>;

template <class Derived>
struct validator_without_params : validator_tag_t, validator_maker_tag_t {
  // Dummy maker
  static constexpr auto operator()() {
    return Derived{};
  }
};

template <validator_maker LeafMaker, template <class> class... Compounds>
struct make_compound_validator_t : validator_maker_tag_t {
private:
  static constexpr auto compound_refl =
    REFLECT_CPP26_EXPAND({^^Compounds...}).reverse();

  template <class... Inputs>
  static consteval auto make(Inputs&&... inputs)
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

  template <validator_maker LeafMaker>
  consteval auto operator>>(LeafMaker) const
  {
    return make_compound_validator_t<LeafMaker, Validators...>{};
  }
};
} // namespace reflect_cpp26::validators::impl

#endif // REFLECT_CPP26_VALIDATORS_IMPL_MAKER_COMMON_HPP
