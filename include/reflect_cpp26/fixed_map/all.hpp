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

#ifndef REFLECT_CPP26_UTILS_FIXED_MAP_ALL_HPP
#define REFLECT_CPP26_UTILS_FIXED_MAP_ALL_HPP

#include <reflect_cpp26/fixed_map/integral_key.hpp>
#include <reflect_cpp26/fixed_map/is_valid.hpp>
#include <reflect_cpp26/fixed_map/string_key.hpp>
#include <variant>

namespace reflect_cpp26 {
using fixed_map_options_variant = std::variant<std::monostate,  // All options by default
                                               integral_key_fixed_map_options,
                                               string_key_fixed_map_options>;

/**
 * Generates a fixed map in compile-time.
 * Result V is the reflected constant of the fixed map generated, which can be
 * extracted via [: V :] or reflect_cpp26::extract<V>().
 * See the following candidates for details:
 *   make_integral_key_fixed_map() in ./integral_key.hpp;
 *   make_string_key_fixed_map() in ./string_key.hpp.
 */
template <std::ranges::input_range KVPairRange>
consteval auto make_fixed_map(KVPairRange kv_pairs, const fixed_map_options_variant& options = {})
    -> std::meta::info {
  using KVPair = std::ranges::range_value_t<KVPairRange>;
  using KeyType = std::tuple_element_t<0, KVPair>;
  if constexpr (integral_or_enum_type<KeyType>) {
    if (std::holds_alternative<string_key_fixed_map_options>(options)) {
      compile_error("Options type mismatch.");
    }
    auto* opt = std::get_if<integral_key_fixed_map_options>(&options);
    if (opt != nullptr) {
      return make_integral_key_fixed_map(std::move(kv_pairs), *opt);
    } else {
      return make_integral_key_fixed_map(std::move(kv_pairs));
    }
  } else {
    if (std::holds_alternative<integral_key_fixed_map_options>(options)) {
      compile_error("Options type mismatch.");
    }
    auto* opt = std::get_if<string_key_fixed_map_options>(&options);
    if (opt != nullptr) {
      return make_string_key_fixed_map(std::move(kv_pairs), *opt);
    } else {
      return make_string_key_fixed_map(std::move(kv_pairs));
    }
  }
}
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_FIXED_MAP_ALL_HPP
