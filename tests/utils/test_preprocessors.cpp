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
#include <reflect_cpp26/utils/preprocessors.h>

TEST(UtilsPreprocessors, NArgs)
{
  EXPECT_EQ_STATIC(1, REFLECT_CPP26_MACRO_N_ARGS(x));
  EXPECT_EQ_STATIC(2, REFLECT_CPP26_MACRO_N_ARGS(x, y));
  EXPECT_EQ_STATIC(6, REFLECT_CPP26_MACRO_N_ARGS(x, y, z, a, b, c));

  EXPECT_EQ_STATIC(0, REFLECT_CPP26_MACRO_N_ARGS());
  EXPECT_EQ_STATIC(2, REFLECT_CPP26_MACRO_N_ARGS(x, ));
  EXPECT_EQ_STATIC(2, REFLECT_CPP26_MACRO_N_ARGS(, y));
  EXPECT_EQ_STATIC(2, REFLECT_CPP26_MACRO_N_ARGS(,));
  EXPECT_EQ_STATIC(8, REFLECT_CPP26_MACRO_N_ARGS(, , , , , , ,)); // 7 commas

  EXPECT_EQ_STATIC(64, REFLECT_CPP26_MACRO_N_ARGS(
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3));

  using matrix_double_4x4 = std::array<std::array<double, 4>, 4>;
  // A common misusage in C++: Macro arguments that contain commas
  EXPECT_EQ_STATIC(3, // _____ | -----------a------------ |, b-, c-
    REFLECT_CPP26_MACRO_N_ARGS(std::array<std::array<double, 4>, 4>));
  EXPECT_EQ_STATIC(1, // _____ | ------------- braced ------------- |
    REFLECT_CPP26_MACRO_N_ARGS((std::array<std::array<double, 4>, 4>)));
  EXPECT_EQ_STATIC(1,
    REFLECT_CPP26_MACRO_N_ARGS(matrix_double_4x4));
}

template <size_t Index>
struct demo_type {
  static constexpr auto value = std::string_view{"<default>"};
};

#define INSTANTIATE_DEMO_TYPE(k, v)                     \
  template <>                                           \
  struct demo_type<k> {                                 \
    static constexpr auto value = std::string_view{v};  \
  };
#define INSTANTIATE_DEMO_TYPE_KVPAIR(kv_pair) \
  INSTANTIATE_DEMO_TYPE kv_pair // Removes parentheses

REFLECT_CPP26_MACRO_FOREACH(INSTANTIATE_DEMO_TYPE_KVPAIR,
  (10, "ten"),
  (20, "twelve"),
  (30, "thirty"),
  (40, "fourty"))

#undef INSTANTIATE_DEMO_TYPE
#undef INSTANTIATE_DEMO_TYPE_KVPAIR

TEST(UtilsPreprocessors, ForEach)
{
  int32_t sum = 0;
#define INCREMENT(x) sum += (x);
  REFLECT_CPP26_MACRO_FOREACH(INCREMENT, 10, 20, 30, 40, 50, 60, 70, 80, 90)
  EXPECT_EQ(450, sum);

  sum = 0;
  REFLECT_CPP26_MACRO_FOREACH(INCREMENT, 10)
  EXPECT_EQ(10, sum);

  sum = 0;
  REFLECT_CPP26_MACRO_FOREACH(INCREMENT) // No-op
  EXPECT_EQ(0, sum);
#undef INCREMENT

  EXPECT_EQ_STATIC("ten", demo_type<10>::value);
  EXPECT_EQ_STATIC("twelve", demo_type<20>::value);
  EXPECT_EQ_STATIC("thirty", demo_type<30>::value);
  EXPECT_EQ_STATIC("fourty", demo_type<40>::value);
  EXPECT_EQ_STATIC("<default>", demo_type<0>::value);
}
