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

#ifndef REFLECT_CPP26_UTILS_DEBUG_HELPER_HPP
#define REFLECT_CPP26_UTILS_DEBUG_HELPER_HPP

#include <reflect_cpp26/utils/define_static_values.hpp>
#include <reflect_cpp26/utils/to_string.hpp>

namespace reflect_cpp26 {
template <class T>
constexpr auto type_name_of() -> std::string_view {
  return std::meta::display_string_of(^^T);
}

template <class T>
constexpr auto type_name_of(const T&) -> std::string_view {
  return type_name_of<T>();
}

namespace impl {
template <class T>
consteval auto make_type_description() -> std::string
{
  auto type_name = type_name_of<T>();
  auto location = source_location_of(^^T);

  auto res = std::string{type_name};
  if (location.line() > 0) {
    res += " (defined in ";
    res += location.file_name();
    res += ':';
    res += reflect_cpp26::to_string(location.line());
    res += ')';
  }
  return res;
}

template <class T>
constexpr auto type_description_v =
  reflect_cpp26::define_static_string(make_type_description<T>());
} // namespace impl

template <class T>
constexpr auto type_description_of() -> std::string_view {
  return impl::type_description_v<T>;
}

template <class T>
constexpr auto type_description_of(const T&) -> std::string_view {
  return type_description_of<T>();
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_DEBUG_HELPER_HPP
