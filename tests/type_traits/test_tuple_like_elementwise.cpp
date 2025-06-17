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

#include "tests/test_options.hpp"
#include <reflect_cpp26/utils/constant.hpp>
#include <reflect_cpp26/utils/meta_tuple.hpp>

#ifdef ENABLE_FULL_HEADER_TEST
#include <reflect_cpp26/type_traits.hpp>
#else
#include <reflect_cpp26/type_traits/tuple_like_elementwise.hpp>
#endif

namespace rfl = reflect_cpp26;

// ---- all/any/none_of_elementwise_v ----

// Unary predicate for-each (with cvref)
static_assert(rfl::all_of_elementwise_v<std::is_pointer,
  std::tuple<int*, const int**, volatile int***, void (*)(int, int, int)>>);
static_assert(rfl::any_of_elementwise_v<std::is_pointer,
  const std::tuple<int, const int&, volatile int*> &>);
static_assert(rfl::none_of_elementwise_v<std::is_pointer,
  const volatile std::tuple<int, const int*&, volatile int*&> &&>);

static_assert(NOT rfl::all_of_elementwise_v<std::is_pointer,
  std::tuple<int*, const int**, volatile int, void (*)(int, int, int)>>);
static_assert(NOT rfl::any_of_elementwise_v<std::is_pointer,
  const std::tuple<int, const int&, volatile int*&> &>);
static_assert(NOT rfl::none_of_elementwise_v<std::is_pointer,
  const volatile std::tuple<int, const int*, volatile int*&> &&>);

// Binary predicate for-each (tuple sizes may be different)
static_assert(rfl::all_of_elementwise_v<std::is_convertible,
  std::tuple<int, int*, std::string>,
  std::tuple<bool, void*> &>);
static_assert(rfl::any_of_elementwise_v<std::is_convertible,
  const std::tuple<int, int*, std::string> &,
  const std::tuple<void*, int**, std::string_view, std::nullptr_t>>);
static_assert(rfl::none_of_elementwise_v<std::is_convertible,
  volatile std::tuple<int, int*, std::string> &,
  volatile std::tuple<void*, int**, std::u32string_view> &&>);

static_assert(NOT rfl::all_of_elementwise_v<std::is_convertible,
  const volatile std::tuple<int, int*, std::string>,
  const volatile std::tuple<bool, void**> &>);
static_assert(NOT rfl::any_of_elementwise_v<std::is_convertible,
  std::tuple<int, int*, std::string>,
  std::tuple<void*, int**, std::u16string_view, std::nullptr_t>>);
static_assert(NOT rfl::none_of_elementwise_v<std::is_convertible,
  std::tuple<int, int*, std::string>,
  std::tuple<void*, const int*, std::u32string_view>>);

// ---- elementwise_zip_substitute_t ----

// with cvref
static_assert(std::is_same_v<
  rfl::elementwise_zip_substitute_t<rfl::meta_tuple,
    std::tuple<short, int, long, double> &,
    std::tuple<short*, int**, long***, double****> &&,
    std::tuple<const char*, const wchar_t*, const char16_t*, const char32_t*>>,
  rfl::type_tuple<
    rfl::meta_tuple<short, short*, const char*>,
    rfl::meta_tuple<int, int**, const wchar_t*>,
    rfl::meta_tuple<long, long***, const char16_t*>,
    rfl::meta_tuple<double, double****, const char32_t*>>>);

// ---- elementwise_zip_transform_t ----

// Tuple size may be different
static_assert(std::is_same_v<
  rfl::elementwise_zip_transform_t<std::common_type,
    std::tuple<std::strong_ordering, int, const void*> &,
    std::tuple<std::weak_ordering, long, volatile int*> &&,
    std::tuple<std::partial_ordering, short, char**, int>>,
  rfl::type_tuple<std::partial_ordering, long, const volatile void*>>);

// ---- elementwise_zip_transform_v ----

template <class... Args>
struct identifier_length_product {
  static constexpr auto value = (identifier_of(^^Args).size() * ...);
};

// Tuple size may be different
static_assert(std::is_same_v<
  decltype(rfl::elementwise_zip_transform_v<
    identifier_length_product,
    std::tuple<std::strong_ordering, std::weak_ordering, std::partial_ordering>,
    std::tuple<std::errc, std::to_chars_result>>),
  const rfl::constant<
    15zU * 4zU, // len("strong_ordering") * len("errc")
    13zU * 15zU // len("weak_ordering") * len("to_chars_result")
  >>);

TEST(TypeTraits, TupleLikeElementwise) {
  EXPECT_TRUE(true); // All test cases done with static-asserts above
}
