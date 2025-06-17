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

#ifndef REFLECT_CPP26_ANNOTATIONS_TAGS_HPP
#define REFLECT_CPP26_ANNOTATIONS_TAGS_HPP

#include <reflect_cpp26/utils/expand.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <concepts>

namespace reflect_cpp26::annotations {
struct property_tag_t {};
struct validator_tag_t {};

template <class Annotation>
constexpr auto is_property_v = std::derived_from<Annotation, property_tag_t>;

template <class Annotation>
constexpr auto is_validator_v = std::derived_from<Annotation, validator_tag_t>;

template <class Annotation>
concept property = is_property_v<Annotation>;

template <class Annotation>
concept validator = is_validator_v<Annotation>;

namespace impl {
struct is_property_fn_t {
  template <std::meta::info M>
  static consteval bool operator()(constant<M>)
  {
    using T = [:type_of(M):];
    return is_property_v<T>;
  }
};

struct is_validator_fn_t {
  template <std::meta::info M>
  static consteval bool operator()(constant<M>)
  {
    using T = [:type_of(M):];
    return is_validator_v<T>;
  }
};

constexpr auto is_property_fn = is_property_fn_t{};
constexpr auto is_validator_fn = is_validator_fn_t{};
} // namespace impl

template <std::meta::info M>
constexpr auto properties_of_meta_v = REFLECT_CPP26_EXPAND(annotations_of(M))
  .template filter<impl::is_property_fn>();

template <std::meta::info M>
constexpr auto validators_of_meta_v = REFLECT_CPP26_EXPAND(annotations_of(M))
  .template filter<impl::is_validator_fn>();
} // namespace reflect_cpp26::annotations

#endif // REFLECT_CPP26_ANNOTATIONS_TAGS_HPP
