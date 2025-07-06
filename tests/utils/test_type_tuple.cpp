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
#include <reflect_cpp26/utils/meta_tuple.hpp>
#include <reflect_cpp26/utils/type_tuple.hpp>

namespace rfl = reflect_cpp26;

constexpr int constant_int = 42;

template <rfl::tuple_like T>
struct tuple_wrapper {
  using tuple_elements = rfl::tuple_elements_t<T>;
  T underlying;

  template <size_t I, class Self>
  constexpr decltype(auto) get(this Self&& self) {
    return rfl::get_ith_element<I>(std::forward_like<Self>(self.underlying));
  }
};

TEST(UtilsTypeTuple, TupleElements)
{
  using std_pair_t = std::pair<int, double>;
  static_assert(std::is_same_v<
    rfl::tuple_elements_t<std_pair_t>,
    rfl::type_tuple<int, double>>);

  using std_tuple_t = std::tuple<int, std::string, std::vector<int>>;
  static_assert(std::is_same_v<
    rfl::tuple_elements_t<std_tuple_t>,
    rfl::type_tuple<int, std::string, std::vector<int>>>);

  using std_array_t = std::array<double, 4>;
  static_assert(rfl::are_all_same_v<
    rfl::tuple_elements_t<std_array_t>,
    rfl::type_tuple_repeat_t<double, 4>,
    rfl::type_tuple<double, double, double, double>>);

  using meta_tuple_t = rfl::meta_tuple<int, const int*, const char*>;
  static_assert(std::is_same_v<
    rfl::tuple_elements_t<meta_tuple_t>,
    rfl::type_tuple<int, const int*, const char*>>);

  using wrapped_meta_tuple_t = tuple_wrapper<meta_tuple_t>;
  constexpr auto w =
    wrapped_meta_tuple_t{rfl::meta_tuple{1, &constant_int, "asdf"}};
  static_assert(w.get<0>() == 1);
  static_assert(w.get<1>() == &constant_int);
  static_assert(w.get<2>() == std::string_view{"asdf"});

  static_assert(std::is_same_v<
    rfl::tuple_elements_t<wrapped_meta_tuple_t>,
    rfl::type_tuple<int, const int*, const char*>>);
}
