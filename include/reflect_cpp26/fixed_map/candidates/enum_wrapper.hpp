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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_ENUM_WRAPPER_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_ENUM_WRAPPER_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

namespace reflect_cpp26::impl::map {
// Precondition: std::to_underlying_t<E> is the same as Underlying::key_type
template <class E, class Underlying>
struct enum_wrapper {
  using key_type = E;
  using value_type = typename Underlying::value_type;

private:
  using result_type = std::pair<const value_type&, bool>;

public:
  constexpr auto size() const -> size_t {
    return underlying.size();
  }

  constexpr auto get(key_type key) const -> result_type {
    return underlying.get(std::to_underlying(key));
  }

  constexpr auto operator[](key_type key) const -> const value_type& {
    return underlying.operator[](std::to_underlying(key));
  }

  Underlying underlying;
};

// -------- Builder --------

template <class E, auto Underlying>
constexpr auto enum_wrapper_v =
    enum_wrapper<E, std::remove_cvref_t<decltype(Underlying)>>{Underlying};
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_ENUM_WRAPPER_HPP
