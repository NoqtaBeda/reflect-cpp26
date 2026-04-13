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

#ifndef REFLECT_CPP26_UTILS_STRING_BUILDER_HPP
#define REFLECT_CPP26_UTILS_STRING_BUILDER_HPP

#include <algorithm>
#include <charconv>
#include <climits>
#include <iterator>
#include <reflect_cpp26/type_traits/arithmetic_types.hpp>
#include <reflect_cpp26/utils/string_encoding.hpp>

namespace std::pmr {
template <class T>
class polymorphic_allocator;  // Defined in <memory_resource>
}

namespace reflect_cpp26 {
template <char_type CharT, class Allocator = std::allocator<CharT>>
class basic_string_builder {
public:
  constexpr basic_string_builder() : buffer_(nullptr), cur_(nullptr), end_(nullptr), alloc_() {}

  explicit constexpr basic_string_builder(Allocator alloc)
      : buffer_(nullptr), cur_(nullptr), end_(nullptr), alloc_(std::move(alloc)) {}

  explicit constexpr basic_string_builder(size_t initial_size) : alloc_() {
    buffer_ = alloc_.allocate(initial_size);
    cur_ = buffer_;
    end_ = buffer_ + initial_size;
  }

  constexpr basic_string_builder(size_t initial_size, Allocator alloc) : alloc_(std::move(alloc)) {
    buffer_ = alloc_.allocate(initial_size);
    cur_ = buffer_;
    end_ = buffer_ + initial_size;
  }

  constexpr ~basic_string_builder() {
    if (buffer_ != nullptr) {
      alloc_.deallocate(buffer_, end_ - buffer_);
    }
  }

  constexpr auto size() const -> size_t {
    return static_cast<size_t>(cur_ - buffer_);
  }

  constexpr auto str() const -> std::basic_string<CharT> {
    return {buffer_, cur_};
  }

  constexpr auto strview() const& -> std::basic_string_view<CharT> {
    return {buffer_, cur_};
  }

  constexpr auto append_char(CharT c) -> basic_string_builder& {
    reserve_at_least(1);
    *cur_++ = c;
    return *this;
  }

  constexpr auto append_char(CharT c, size_t count) -> basic_string_builder& {
    reserve_at_least(count);
    cur_ = std::ranges::fill_n(cur_, count, c);
    return *this;
  }

  template <escaping_mode Mode>
  constexpr auto append_char_by(CharT c) -> basic_string_builder& {
    reserve_at_least(6);  // 6 : Maximum length of escaped character (e.g. "\u0000")
    auto [status, next_head] = write_escaped_character_unsafe<Mode>(cur_, c);
    if (escaping_status::done == status) {
      cur_ = next_head;
    } else {
      *cur_++ = c;  // No escaping or conversion
    }
    return *this;
  }

  constexpr auto append_char_json_escaped(CharT c) -> basic_string_builder& {
    return append_char_by<escaping_mode::json>(c);
  }

  constexpr auto append_utf_code_point(char32_t code_point) -> basic_string_builder& {
    reserve_at_least(4);
    if (is_valid_code_point(code_point)) [[likely]] {
      cur_ = encode_code_point_unsafe(cur_, code_point);
    } else {
      cur_ = encode_code_point_unsafe(cur_, replacement_code_point);
    }
    return *this;
  }

  constexpr auto append_utf_code_point_json_escaped(char32_t code_point) -> basic_string_builder& {
    reserve_at_least(6);  // 6 : Maximum length of escaped character (e.g. "\u0000")
    auto [status, next_head] = write_escaped_character_for_json_unsafe(cur_, code_point);
    if (escaping_status::done == status) {
      cur_ = next_head;
    } else if (is_valid_code_point(code_point)) [[likely]] {
      cur_ = encode_code_point_unsafe(cur_, code_point);
    } else {
      cur_ = encode_code_point_unsafe(cur_, replacement_code_point);
    }
    return *this;
  }

  constexpr auto append_string(const CharT* str, const CharT* str_end) -> basic_string_builder& {
    reserve_at_least(str_end - str);
    cur_ = std::ranges::copy(str, str_end, cur_).out;
    return *this;
  }

  constexpr auto append_string(const CharT* str) -> basic_string_builder& {
    auto str_end = std::ranges::find(str, std::unreachable_sentinel, static_cast<CharT>('\0'));
    reserve_at_least(str_end - str);
    cur_ = std::ranges::copy(str, str_end, cur_).out;
    return *this;
  }

  constexpr auto append_string(std::basic_string_view<CharT> str) -> basic_string_builder& {
    reserve_at_least(str.length());
    cur_ = std::ranges::copy(str, cur_).out;
    return *this;
  }

  template <escaping_mode Mode>
  constexpr auto append_string_by(const CharT* str, const CharT* str_end) -> basic_string_builder& {
    reserve_at_least(6 * (str_end - str));  // 6 : Max length of escaped character (e.g. "\u0000")
    for (; str < str_end; ++str) {
      auto [status, next_head] = write_escaped_character_unsafe<Mode>(cur_, *str);
      if (escaping_status::done == status) {
        cur_ = next_head;
      } else {
        *cur_++ = *str;  // No escaping or conversion
      }
    }
    return *this;
  }

  template <escaping_mode Mode>
  constexpr auto append_string_by(const CharT* str) -> basic_string_builder& {
    auto str_end = std::ranges::find(str, std::unreachable_sentinel, static_cast<CharT>('\0'));
    return append_string_by<Mode>(str, str_end);
  }

  template <escaping_mode Mode>
  constexpr auto append_string_by(std::basic_string_view<CharT> str) -> basic_string_builder& {
    return append_string_by<Mode>(str.data(), str.data() + str.size());
  }

  constexpr auto append_string_json_escaped(const CharT* str, const CharT* str_end)
      -> basic_string_builder& {
    return append_string_by<escaping_mode::json>(str, str_end);
  }

  constexpr auto append_string_json_escaped(const CharT* str) -> basic_string_builder& {
    auto str_end = std::ranges::find(str, std::unreachable_sentinel, static_cast<CharT>('\0'));
    return append_string_json_escaped(str, str_end);
  }

  constexpr auto append_string_json_escaped(std::basic_string_view<CharT> str)
      -> basic_string_builder& {
    return append_string_json_escaped(str.data(), str.data() + str.size());
  }

  template <char_type OtherCharT>
  constexpr auto append_utf_string(const OtherCharT* str, const OtherCharT* str_end)
      -> basic_string_builder& {
    if constexpr (sizeof(OtherCharT) == sizeof(CharT)) {
      // No UTF conversion. Replaces invalid UTF sequences with '�'.
      reserve_at_least(4 * (str_end - str));
      while (str < str_end) {
        auto [code_point, next_str] = decode_code_point(str, str_end);
        if (code_point != invalid_code_point) [[likely]] {
          cur_ = encode_code_point_unsafe(cur_, code_point);
          str = next_str;
          continue;
        }
        str = consume_utf_invalid_sequence(str, str_end);
        cur_ = encode_code_point_unsafe(cur_, replacement_code_point);
      }
    } else {
      // UTF conversion. Replaces invalid UTF sequences with '�'.
      while (str < str_end) {
        auto res = utf_convert(cur_, end_, str, str_end);
        cur_ = res.out_ptr;
        if (encoding_status::done == res.status) {
          return *this;
        }
        if (encoding_status::buffer_run_out == res.status) {
          reserve_at_least(static_cast<size_t>(str_end - res.in_ptr) * 4);
          str = res.in_ptr;
        } else {
          str = consume_utf_invalid_sequence(res.in_ptr, str_end);
          append_utf_code_point(replacement_code_point);
        }
      }
    }
    return *this;
  }

  template <char_type OtherCharT>
  constexpr auto append_utf_string(const OtherCharT* str) -> basic_string_builder& {
    auto str_end = std::ranges::find(str, std::unreachable_sentinel, static_cast<OtherCharT>('\0'));
    return append_utf_string(str, str_end);
  }

  template <char_type OtherCharT>
  constexpr auto append_utf_string(std::basic_string_view<OtherCharT> str)
      -> basic_string_builder& {
    return append_utf_string(str.data(), str.data() + str.length());
  }

  template <char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string(const std::basic_string<OtherCharT, Traits, OtherAllocator>& str)
      -> basic_string_builder& {
    return append_utf_string(str.data(), str.data() + str.length());
  }

  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by(const OtherCharT* str, const OtherCharT* str_end)
      -> basic_string_builder& {
    if constexpr (sizeof(OtherCharT) == sizeof(CharT)) {
      // No UTF conversion. Replaces invalid UTF sequences with '�'.
      reserve_at_least(6 * (str_end - str));  // 6 : Max length of escaped character (e.g. "\u0000")
      while (str < str_end) {
        auto [status, next_head] = write_escaped_character_unsafe<Mode>(cur_, *str);
        if (escaping_status::done == status) {
          cur_ = next_head;
          str += 1;
          continue;
        }
        auto [code_point, next_str] = decode_code_point(str, str_end);
        if (code_point != invalid_code_point) [[likely]] {
          cur_ = encode_code_point_unsafe(cur_, code_point);
          str = next_str;
          continue;
        }
        str = consume_utf_invalid_sequence(str, str_end);
        cur_ = encode_code_point_unsafe(cur_, replacement_code_point);
      }
    } else {
      // UTF conversion. Replaces invalid UTF sequences with '�'.
      while (str < str_end) {
        auto res = utf_convert_json_escaped(cur_, end_, str, str_end);
        cur_ = res.out_ptr;
        if (encoding_status::done == res.status) {
          return *this;
        }
        if (encoding_status::buffer_run_out == res.status) {
          reserve_at_least(static_cast<size_t>(str_end - res.in_ptr) * 6);
          str = res.in_ptr;
        } else {
          str = consume_utf_invalid_sequence(res.in_ptr, str_end);
          append_utf_code_point(replacement_code_point);
        }
      }
    }
    return *this;
  }

  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by(const OtherCharT* str) -> basic_string_builder& {
    auto str_end = std::ranges::find(str, std::unreachable_sentinel, static_cast<OtherCharT>('\0'));
    return append_utf_string_by<Mode>(str, str_end);
  }

  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by(std::basic_string_view<OtherCharT> str)
      -> basic_string_builder& {
    return append_utf_string_by<Mode>(str.data(), str.data() + str.length());
  }

  template <escaping_mode Mode, char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string_by(
      const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder& {
    return append_utf_string_by<Mode>(str.data(), str.data() + str.length());
  }

  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped(const OtherCharT* str, const OtherCharT* str_end)
      -> basic_string_builder& {
    return append_utf_string_by<escaping_mode::json>(str, str_end);
  }

  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped(const OtherCharT* str) -> basic_string_builder& {
    auto str_end = std::ranges::find(str, std::unreachable_sentinel, static_cast<OtherCharT>('\0'));
    return append_utf_string_json_escaped(str, str_end);
  }

  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped(std::basic_string_view<OtherCharT> str)
      -> basic_string_builder& {
    return append_utf_string_json_escaped(str.data(), str.data() + str.length());
  }

  template <char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string_json_escaped(
      const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder& {
    return append_utf_string_json_escaped(str.data(), str.data() + str.length());
  }

  constexpr auto append_bool(bool value) -> basic_string_builder& {
    reserve_at_least(5);
    constexpr auto true_str = std::string_view{"true"};
    constexpr auto false_str = std::string_view{"false"};
    cur_ = std::ranges::copy(value ? true_str : false_str, cur_).out;
    return *this;
  }

  template <non_bool_integral IntegerT>
    requires(sizeof(IntegerT) <= sizeof(int64_t))
  constexpr auto append_integer(IntegerT value, int base = 10) -> basic_string_builder& {
    constexpr size_t buffer_size = sizeof(IntegerT) * CHAR_BIT + 1;
    append_numeric(buffer_size, value, base);
    return *this;
  }

  constexpr auto append_floating_point(float value) -> basic_string_builder& {
    constexpr size_t buffer_size = 50;
    append_numeric(buffer_size, value);
    return *this;
  }

  constexpr auto append_floating_point(double value) -> basic_string_builder& {
    constexpr size_t buffer_size = 330;
    append_numeric(buffer_size, value);
    return *this;
  }

  constexpr auto append_floating_point(long double value) -> basic_string_builder& {
    constexpr size_t buffer_size = 330;
    append_numeric_with_retry(buffer_size, value);
    return *this;
  }

  template <std::floating_point FloatT>
  constexpr auto append_floating_point(FloatT value, std::chars_format fmt)
      -> basic_string_builder& {
    constexpr size_t buffer_size = sizeof(FloatT) <= sizeof(float) ? 50 : 330;
    append_numeric_with_retry(buffer_size, value, fmt);
    return *this;
  }

  template <std::floating_point FloatT>
  constexpr auto append_floating_point(FloatT value, std::chars_format fmt, int precision)
      -> basic_string_builder& {
    constexpr size_t buffer_size = sizeof(FloatT) <= sizeof(float) ? 50 : 330;
    append_numeric_with_retry(buffer_size, value, fmt, precision);
    return *this;
  }

private:
  constexpr size_t remaining_capacity() const {
    return static_cast<size_t>(end_ - cur_);
  }

  constexpr void reserve_at_least(size_t n) {
    if (cur_ + n <= end_) [[likely]] {
      return;
    }
    auto cur_capacity = static_cast<size_t>(end_ - buffer_);
    auto new_capacity = cur_capacity + std::max<size_t>(cur_capacity, n);

    auto* cur_buffer = buffer_;
    auto* new_buffer = alloc_.allocate(new_capacity);
    cur_ = std::ranges::copy(cur_buffer, cur_, new_buffer).out;
    end_ = new_buffer + new_capacity;
    alloc_.deallocate(buffer_, cur_capacity);
    buffer_ = new_buffer;
  }

  template <class... Args>
  constexpr void append_numeric(size_t buffer_size, Args... args) {
    reserve_at_least(buffer_size);

    if constexpr (std::is_same_v<CharT, char>) {
      cur_ = std::to_chars(cur_, end_, args...).ptr;
    } else if constexpr (std::is_same_v<CharT, char8_t>) {
      if !consteval {
        auto* cur_char = reinterpret_cast<char*>(cur_);
        auto* end_char = reinterpret_cast<char*>(end_);
        cur_ = reinterpret_cast<char8_t*>(std::to_chars(cur_char, end_char, args...).ptr);
      } else {
        append_numeric_with_char_buffer(args...);
      }
    } else {
      append_numeric_with_char_buffer(args...);
    }
  }

  template <class... Args>
  consteval void append_numeric_with_retry_consteval(size_t buffer_size, Args... args) {
    reserve_at_least(buffer_size);
    while (true) {
      auto temp_buffer_size = remaining_capacity();
      // For compile-time evaluation: We ignore Allocator since it may not be constexpr.
      auto temp_buffer = new char[temp_buffer_size];
      auto [ptr, ec] = std::to_chars(temp_buffer, temp_buffer + temp_buffer_size, args...);
      if (std::errc{} == ec) {
        cur_ = std::ranges::copy(temp_buffer, ptr, cur_).out;
        delete[] temp_buffer;
        return;
      }
      buffer_size *= 2;
      reserve_at_least(buffer_size);
      delete[] temp_buffer;
    }
  }

  template <class... Args>
  void append_numeric_with_retry_non_consteval(size_t buffer_size, Args... args) {
    reserve_at_least(buffer_size);
    if constexpr (sizeof(CharT) == sizeof(char)) {
      // (1) No additional buffer needed
      while (true) {
        char* cur_as_char = reinterpret_cast<char*>(cur_);
        char* end_as_char = reinterpret_cast<char*>(end_);
        auto [ptr, ec] = std::to_chars(cur_as_char, end_as_char, args...);
        if (std::errc{} == ec) [[likely]] {
          cur_ = reinterpret_cast<CharT*>(ptr);
          return;
        }
        buffer_size *= 2;
        reserve_at_least(buffer_size);
      }
    } else {
      // (2) Additional buffer needed
      while (true) {
        char* buffer = reinterpret_cast<char*>(alloc_.allocate(buffer_size));
        auto [ptr, ec] = std::to_chars(buffer, buffer + buffer_size, args...);
        if (std::errc{} == ec) [[likely]] {
          cur_ = std::ranges::copy(buffer, ptr, cur_).out;
          alloc_.deallocate(reinterpret_cast<CharT*>(buffer), buffer_size);
          return;
        }
        buffer_size *= 2;
        reserve_at_least(buffer_size);
        alloc_.deallocate(reinterpret_cast<CharT*>(buffer), buffer_size);
      }
    }
  }

  template <class... Args>
  constexpr void append_numeric_with_retry(size_t buffer_size, Args... args) {
    if consteval {
      append_numeric_with_retry_consteval(buffer_size, args...);
    } else {
      append_numeric_with_retry_non_consteval(buffer_size, args...);
    }
  }

  template <class IntegerT>
    requires(non_bool_integral<IntegerT>)
  constexpr void append_numeric_with_char_buffer(IntegerT value, int base) {
    constexpr auto buffer_size = sizeof(IntegerT) * CHAR_BIT + 1;
    char buffer[buffer_size];
    char* ptr = std::to_chars(buffer, buffer + buffer_size, value, base).ptr;
    cur_ = std::ranges::copy(buffer, ptr, cur_).out;
  }

  constexpr void append_numeric_with_char_buffer(float value) {
    constexpr auto buffer_size = 50;
    char buffer[buffer_size];
    char* ptr = std::to_chars(buffer, buffer + buffer_size, value).ptr;
    cur_ = std::ranges::copy(buffer, ptr, cur_).out;
  }

  constexpr void append_numeric_with_char_buffer(double value) {
    constexpr auto buffer_size = 330;
    char buffer[buffer_size];
    char* ptr = std::to_chars(buffer, buffer + buffer_size, value).ptr;
    cur_ = std::ranges::copy(buffer, ptr, cur_).out;
  }

  CharT* buffer_;
  CharT* cur_;
  CharT* end_;
  [[no_unique_address]] Allocator alloc_;
};

#define REFLECT_CPP26_STRING_BUILDER_ALIAS(string_type, CharT) \
  using string_type##_builder = basic_string_builder<CharT>;   \
  using pmr_##string_type##_builder =                          \
      basic_string_builder<CharT, std::pmr::polymorphic_allocator<CharT>>;

REFLECT_CPP26_STRING_BUILDER_ALIAS(string, char)
REFLECT_CPP26_STRING_BUILDER_ALIAS(u8string, char8_t)
REFLECT_CPP26_STRING_BUILDER_ALIAS(u16string, char16_t)
REFLECT_CPP26_STRING_BUILDER_ALIAS(u32string, char32_t)
// Note: wchar_t specialization is flawed since UTF conversion is unsupported
REFLECT_CPP26_STRING_BUILDER_ALIAS(wstring, wchar_t)

#undef REFLECT_CPP26_STRING_BUILDER_ALIAS
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_STRING_BUILDER_HPP
