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

#ifndef REFLECT_CPP26_UTILS_META_STRING_VIEW_HPP
#define REFLECT_CPP26_UTILS_META_STRING_VIEW_HPP

#include <algorithm>
#include <compare>
#include <concepts>
#include <format>
#include <iterator>
#include <reflect_cpp26/utils/config.hpp>
#include <string>
#include <string_view>

namespace reflect_cpp26 {
/**
 * Structured alternative to std::basic_string_view<CharT>.
 * It's ensured that the referenced string is always null-terminated, i.e.
 * the constraint *tail == '\0' always holds as long as meta_string_view is
 * not default-constructed.
 * Semantic constraints: meta_basic_string_view<CharT> is used for strings
 * with static constant storage only.
 */
template <class CharT>
struct meta_basic_string_view {
  using value_type = CharT;
  using pointer = CharT*;
  using const_pointer = const CharT*;
  using reference = CharT&;
  using const_reference = const CharT&;
  using iterator = const_pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<const_iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

private:
  // Note: Customized char traits is disabled to prevent ambiguity
  // with sementics of different traits.
  template <class Alloc>
  using std_string_of_alloc = std::basic_string<CharT, std::char_traits<CharT>, Alloc>;

public:
  const CharT* head = nullptr;
  const CharT* tail = nullptr;

  constexpr meta_basic_string_view() = default;

  static consteval auto from_literal(const CharT* literal) {
    if (literal == nullptr) {
      compile_error("String literal must not be null pointer.");
    }
    auto res = meta_basic_string_view{};
    res.head = literal;
    res.tail = std::ranges::find(literal, std::unreachable_sentinel, '\0');
    return res;
  }

  template <size_t N>
  static consteval auto from_literal(const CharT (&literal)[N]) -> meta_basic_string_view {
    if (literal[N - 1] != '\0') {
      compile_error("String literal must be null-terminated.");
    }
    return from_literal(static_cast<const CharT*>(literal));
  }

  template <size_t N>
  static consteval auto from_array(const CharT (&arr)[N]) -> meta_basic_string_view {
    return from_literal(arr);
  }

  template <size_t N>
  static consteval auto from_array(const std::array<CharT, N>& arr) -> meta_basic_string_view {
    if (arr[N - 1] != '\0') {
      compile_error("String literal must be null-terminated.");
    }
    return from_literal(arr.data());
  }

  static consteval auto from_std_string_view(std::basic_string_view<CharT> sv)
      -> meta_basic_string_view {
    if (*sv.end() != '\0') {
      compile_error("String literal must be null-terminated.");
    }
    return from_literal(sv.data());
  }

  // Implicit conversion to std::string_view
  constexpr operator std::basic_string_view<CharT>() const {
    return {head, tail};
  }

  constexpr auto operator[](size_t index) const -> CharT {
    return head[index];
  }

  constexpr auto front() const -> CharT {
    return head[0];
  }

  constexpr auto back() const -> CharT {
    return tail[-1];
  }

  constexpr auto size() const -> size_t {
    return tail - head;
  }

  constexpr auto length() const -> size_t {
    return tail - head;
  }

  constexpr auto empty() const -> bool {
    return head == tail;
  }

  constexpr auto data() const -> const CharT* {
    return head;
  }

  constexpr auto begin() const -> const CharT* {
    return head;
  }

  constexpr auto end() const -> const CharT* {
    return tail;
  }

  // Note: remove_suffix(n) is not provided since we need to ensure
  // the constraint *tail == '\0' is always satisfied.
  constexpr auto remove_prefix(size_t n) const -> meta_basic_string_view {
    auto res = meta_basic_string_view{};
    res.head = this->head + n;
    res.tail = this->tail;
    return res;
  }

  template <std::same_as<meta_basic_string_view<CharT>> RhsType>
  constexpr auto operator<=>(RhsType rhs) const -> std::strong_ordering {
    return std::lexicographical_compare_three_way(head, tail, rhs.head, rhs.tail);
  }

  template <std::same_as<meta_basic_string_view<CharT>> RhsType>
  constexpr bool operator==(RhsType rhs) const {
    return std::ranges::equal(head, tail, rhs.head, rhs.tail);
  }

  template <class Alloc>
  constexpr auto operator<=>(const std_string_of_alloc<Alloc>& rhs) const -> std::strong_ordering {
    return std::lexicographical_compare_three_way(head, tail, rhs.begin(), rhs.end());
  }

  template <class Alloc>
  constexpr bool operator==(const std_string_of_alloc<Alloc>& rhs) const {
    return std::ranges::equal(head, tail, rhs.begin(), rhs.end());
  }

  constexpr auto operator<=>(const CharT* rhs) const -> std::strong_ordering {
    if (rhs == nullptr) {
      return size() <=> 0;  // nullptr as empty string
    }
    if (head == nullptr) {
      return (rhs == nullptr) || (*rhs == '\0') ? std::strong_ordering::equal
                                                : std::strong_ordering::less;
    }
    const auto* rhs_tail = std::ranges::find(rhs, std::unreachable_sentinel, '\0');
    return std::lexicographical_compare_three_way(head, tail, rhs, rhs_tail);
  }

  constexpr bool operator==(const CharT* rhs) const {
    if (rhs == nullptr) {
      return size() == 0;  // nullptr as empty string
    }
    const auto* rhs_tail = std::ranges::find(rhs, std::unreachable_sentinel, '\0');
    return std::ranges::equal(head, tail, rhs, rhs_tail);
  }
};

using meta_string_view = meta_basic_string_view<char>;
using meta_wstring_view = meta_basic_string_view<wchar_t>;
using meta_u8string_view = meta_basic_string_view<char8_t>;
using meta_u16string_view = meta_basic_string_view<char16_t>;
using meta_u32string_view = meta_basic_string_view<char32_t>;

}  // namespace reflect_cpp26

template <class CharT>
struct std::formatter<reflect_cpp26::meta_basic_string_view<CharT>, CharT>
    : std::formatter<std::basic_string_view<CharT>, CharT> {
  using self_type = reflect_cpp26::meta_basic_string_view<CharT>;

  template <class FormatContext>
  auto format(self_type str, FormatContext& ctx) const -> FormatContext::iterator {
    return std::ranges::copy(str, ctx.out()).out;
  }
};

#endif  // REFLECT_CPP26_UTILS_META_STRING_VIEW_HPP
