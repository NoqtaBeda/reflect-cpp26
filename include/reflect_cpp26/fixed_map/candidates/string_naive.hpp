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

#ifndef REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_NAIVE_HPP
#define REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_NAIVE_HPP

#include <reflect_cpp26/fixed_map/candidates/string_empty.hpp>
#include <reflect_cpp26/fixed_map/impl/string_policy.hpp>
#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/meta_span.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>

namespace reflect_cpp26::impl::map {
template <class CharT, class V, template <class> class Policy>
struct naive_with_skey {
  using key_type = meta_basic_string_view<CharT>;
  using value_type = V;

private:
  using element_type = meta_tuple<key_type, V>;
  using result_type = std::pair<const V&, bool>;

public:
  constexpr auto size() const -> size_t {
    return entries.size();
  }

  constexpr auto get(std::basic_string_view<CharT> key) const -> result_type {
    for (const auto& cur : entries) {
      if (Policy<CharT>::equals(cur.elements.first, key)) {
        return {cur.elements.second, true};
      }
    }
    return {default_v<value_type>, false};
  }

  constexpr auto operator[](std::basic_string_view<CharT> key) const -> const value_type& {
    return get(key).first;
  }

  meta_span<element_type> entries;
};

// -------- Builder --------

struct naive_with_skey_options {
  bool ascii_case_insensitive;
};

template <class CharT, class V, template <class> class Policy>
consteval auto make_naive_with_skey_impl(
    std::span<const meta_tuple<meta_basic_string_view<CharT>, V>> kv_pairs) -> std::meta::info {
  // Makes obj
  auto entries = reflect_cpp26::define_static_array(kv_pairs);
  auto obj = naive_with_skey<CharT, V, Policy>{entries};
  return std::meta::reflect_constant(obj);
}

template <class CharT, class V>
consteval auto make_naive_with_skey(
    std::span<const meta_tuple<meta_basic_string_view<CharT>, V>> kv_pairs,
    const naive_with_skey_options& options) -> std::meta::info {
  // (1) Empty
  if (kv_pairs.empty()) {
    return make_empty_with_skey<CharT, V>();
  }
  // (2) Naive
  using call_signature =
      std::meta::info(std::span<const meta_tuple<meta_basic_string_view<CharT>, V>>);
  auto policy = get_skey_policy_template(options.ascii_case_insensitive);
  return extract<call_signature*>(^^make_naive_with_skey_impl, ^^CharT, ^^V, policy)(kv_pairs);
}
}  // namespace reflect_cpp26::impl::map

#endif  // REFLECT_CPP26_FIXED_MAP_CANDIDATES_STRING_NAIVE_HPP
