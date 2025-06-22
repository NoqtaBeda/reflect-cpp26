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

#ifndef REFLECT_CPP26_VALIDATORS_COMMON_HPP
#define REFLECT_CPP26_VALIDATORS_COMMON_HPP

#include <reflect_cpp26/utils/concepts.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <reflect_cpp26/utils/utility.hpp>

namespace reflect_cpp26 {
struct validator_tag_t {};

template <class T>
constexpr auto is_validator_v = std::derived_from<T, validator_tag_t>;

template <class T>
concept validator = is_validator_v<T>;

template <class T, class InputT>
concept validator_of = is_validator_v<T> &&
  requires (const T& validator, const InputT& input) {
    { validator.test(input) } -> std::same_as<bool>;
    { validator.make_error_message(input) } -> std::same_as<std::string>;
  };

namespace impl {
consteval auto get_validators_list(std::meta::info member)
{
  auto annotations = annotations_of(member);
  auto res = make_reserved_vector<std::meta::info>(annotations.size());
  for (auto a: annotations) {
    if (extract_bool(^^is_validator_v, type_of(a))) {
      res.push_back(a);
    }
  }
  return res;
}
} // namespace impl

template <std::meta::info Member>
constexpr auto validators_of_member_v =
  std::define_static_array(impl::get_validators_list(Member));
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_VALIDATORS_COMMON_HPP
