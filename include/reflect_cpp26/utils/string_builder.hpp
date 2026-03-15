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
  constexpr basic_string_builder() : buffer_(), cur_(nullptr), end_(nullptr), alloc_() {}

  explicit constexpr basic_string_builder(Allocator alloc)
      : buffer_(), cur_(nullptr), end_(nullptr), alloc_(std::move(alloc)) {}

  explicit constexpr basic_string_builder(size_t initial_size) : alloc_() {
    buffer_.reset(alloc_.allocate(initial_size));
    cur_ = buffer_.get();
    end_ = buffer_.get() + initial_size;
  }

  constexpr basic_string_builder(size_t initial_size, Allocator alloc) : alloc_(std::move(alloc)) {
    buffer_.reset(alloc_.allocate(initial_size));
    cur_ = buffer_.get();
    end_ = buffer_.get() + initial_size;
  }

  constexpr size_t size() const {
    return static_cast<size_t>(cur_ - buffer_.get());
  }

  constexpr auto strview() const& -> std::basic_string_view<CharT> {
    return {buffer_.get(), cur_};
  }

  constexpr auto get() && -> std::pair<std::unique_ptr<CharT[]>, CharT*> {
    return {std::move(buffer_), cur_};  // Transfers ownership
  }

  constexpr auto append_char(CharT c) -> basic_string_builder& {
    reserve_at_least(1);
    *cur_++ = c;
    return *this;
  }

  constexpr auto append_utf_code_point(char32_t code_point) -> basic_string_builder& {
    reserve_at_least(4);
    cur_ = encode_code_point_unsafe(cur_, code_point);
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

  template <char_type OtherCharT>
  constexpr auto append_utf_string(const OtherCharT* str, const OtherCharT* str_end)
      -> basic_string_builder& {
    if constexpr (sizeof(OtherCharT) == sizeof(CharT)) {
      // No UTF conversion.
      reserve_at_least(static_cast<size_t>(str_end - str));
      cur_ = std::ranges::copy(str, str_end, cur_).out;
    } else {
      while (str < str_end) {
        auto res = utf_convert(cur_, end_, str, str_end);
        cur_ = res.out_ptr;
        if (std::errc{} == res.ec) {
          return *this;
        }
        if (std::errc::value_too_large == res.ec) {
          reserve_at_least(static_cast<size_t>(str_end - res.in_ptr) * 4);
          str = res.in_ptr;
        } else {
          str = consume_utf_invalid_sequence(res.in_ptr, str_end);
          append_utf_code_point(0xFFFD);
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
    append_numeric_by_to_chars(buffer_size, value, base);
    return *this;
  }

  constexpr auto append_floating_point(float value) -> basic_string_builder& {
    constexpr size_t buffer_size = 50;
    append_numeric_by_to_chars(buffer_size, value);
    return *this;
  }

  constexpr auto append_floating_point(double value) -> basic_string_builder& {
    constexpr size_t buffer_size = 330;
    append_numeric_by_to_chars(buffer_size, value);
    return *this;
  }

  constexpr auto append_floating_point(long double value) -> basic_string_builder& {
    constexpr size_t buffer_size = 330;
    append_numeric_by_to_chars_with_retry(buffer_size, value);
    return *this;
  }

  template <std::floating_point FloatT>
  constexpr auto append_floating_point(FloatT value, std::chars_format fmt)
      -> basic_string_builder& {
    constexpr size_t buffer_size = sizeof(FloatT) <= sizeof(float) ? 50 : 330;
    append_numeric_by_to_chars_with_retry(buffer_size, value, fmt);
    return *this;
  }

  template <std::floating_point FloatT>
  constexpr auto append_floating_point(FloatT value, std::chars_format fmt, int precision)
      -> basic_string_builder& {
    constexpr size_t buffer_size = sizeof(FloatT) <= sizeof(float) ? 50 : 330;
    append_numeric_by_to_chars_with_retry(buffer_size, value, fmt, precision);
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
    auto cur_capacity = static_cast<size_t>(end_ - buffer_.get());
    auto new_capacity = cur_capacity + std::max<size_t>(cur_capacity, n);

    auto* cur_buffer = buffer_.get();
    auto* new_buffer = alloc_.allocate(new_capacity);
    cur_ = std::ranges::copy(cur_buffer, cur_, new_buffer).out;
    end_ = new_buffer + new_capacity;
    buffer_.reset(new_buffer);
  }

  template <class... Args>
  constexpr void append_numeric_by_to_chars(size_t buffer_size, Args... args) {
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
  constexpr void append_numeric_by_to_chars_with_retry(size_t buffer_size, Args... args) {
    reserve_at_least(buffer_size);

    if constexpr (std::is_same_v<CharT, char>) {
      while (true) {
        auto [ptr, ec] = std::to_chars(cur_, end_, args...);
        if (std::errc{} != ec) [[unlikely]] {
          buffer_size *= 2;
          reserve_at_least(buffer_size);
        } else {
          cur_ = ptr;
          return;
        }
      }
    } else {
      while (true) {
        auto temp_buffer_size = remaining_capacity();
        auto temp_buffer = std::make_unique<char[]>(temp_buffer_size);
        auto [ptr, ec] =
            std::to_chars(temp_buffer.get(), temp_buffer.get() + temp_buffer_size, args...);
        if (std::errc{} != ec) [[unlikely]] {
          buffer_size *= 2;
          reserve_at_least(buffer_size);
        } else {
          cur_ = std::ranges::copy(temp_buffer.get(), ptr, cur_).out;
          return;
        }
      }
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

  std::unique_ptr<CharT[]> buffer_;
  CharT* cur_;
  CharT* end_;
  [[no_unique_address]] Allocator alloc_;
};

#define REFLECT_CPP26_STRING_BUILDER_ALIAS(string_type, CharT) \
  using string_type##_builder = basic_string_builder<CharT>;   \
  using pmr_##string_type##_builder =                          \
      basic_string_builder<CharT, std::pmr::polymorphic_allocator<CharT>>;

REFLECT_CPP26_STRING_BUILDER_ALIAS(string, char)
REFLECT_CPP26_STRING_BUILDER_ALIAS(wstring, wchar_t)
REFLECT_CPP26_STRING_BUILDER_ALIAS(u8string, char8_t)
REFLECT_CPP26_STRING_BUILDER_ALIAS(u16string, char16_t)
REFLECT_CPP26_STRING_BUILDER_ALIAS(u32string, char32_t)

#undef REFLECT_CPP26_STRING_BUILDER_ALIAS
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_STRING_BUILDER_HPP
