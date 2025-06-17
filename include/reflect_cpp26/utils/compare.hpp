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

#ifndef REFLECT_CPP26_UTILS_COMPARE_HPP
#define REFLECT_CPP26_UTILS_COMPARE_HPP

#include <compare>

// Reused by multiple headers in reflect_cpp26
#define REFLECT_CPP26_COMPARISON_CONCEPT(type) \
  template <class T, class... Us>              \
  concept type##_comparable_with = (is_##type##_comparable_v<T, Us> && ...);

namespace reflect_cpp26 {
/**
 * Whether T (possibly with cv-qualifiers) is some result type of C++
 * three-way comparison.
 */
template <class T>
concept three_way_comparison_result = std::is_same_v<std::remove_cv_t<T>, std::strong_ordering>
                                   || std::is_same_v<std::remove_cv_t<T>, std::weak_ordering>
                                   || std::is_same_v<std::remove_cv_t<T>, std::partial_ordering>;

#define REFLECT_CPP26_IS_OPERATOR_COMPARABLE(op_name, op)                                  \
  template <class T, class U>                                                              \
  constexpr bool is_operator_##op_name##_comparable_v = requires(const T& t, const U& u) { \
    { t op u } -> std::same_as<bool>;                                                      \
  };

/**
 * is_operator_*_comparable_v<T, U>:
 * Whether (t op u) is a valid boolean expression where op is
 * ==, !=, <, >, <=, >= respectively.
 */
REFLECT_CPP26_IS_OPERATOR_COMPARABLE(eq, ==)
REFLECT_CPP26_IS_OPERATOR_COMPARABLE(ne, !=)
REFLECT_CPP26_IS_OPERATOR_COMPARABLE(lt, <)
REFLECT_CPP26_IS_OPERATOR_COMPARABLE(gt, >)
REFLECT_CPP26_IS_OPERATOR_COMPARABLE(le, <=)
REFLECT_CPP26_IS_OPERATOR_COMPARABLE(ge, >=)

/**
 * Whether (t <=> u) is a valid three-way comparison expression.
 * Note: is_operator_3way_comparable_v<T, U> == false if T, U are
 * integral types with different signedness.
 * Possible compile error message: "argument to 'operator<=>' cannot be
 * narrowed from type 'int' to 'unsigned int'"
 */
template <class T, class U>
constexpr bool is_operator_3way_comparable_v = requires(const T& t, const U& u) {
  { t <=> u } -> three_way_comparison_result;
};

/**
 * operator_*_comparable_with<T, U1, U2, ...>
 * Whether (t op u) is valid comparison expression where:
 * (1) u is of any type in {U1, U2, ...};
 * (2) op is ==, !=, <, >, <=, >=, <=> respectively.
 */
REFLECT_CPP26_COMPARISON_CONCEPT(operator_eq)
REFLECT_CPP26_COMPARISON_CONCEPT(operator_ne)
REFLECT_CPP26_COMPARISON_CONCEPT(operator_lt)
REFLECT_CPP26_COMPARISON_CONCEPT(operator_le)
REFLECT_CPP26_COMPARISON_CONCEPT(operator_gt)
REFLECT_CPP26_COMPARISON_CONCEPT(operator_ge)
REFLECT_CPP26_COMPARISON_CONCEPT(operator_3way)
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_UTILS_COMPARE_HPP
