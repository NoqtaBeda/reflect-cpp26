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

#ifndef REFLECT_CPP26_TYPE_TRAITS_TYPE_COMPARISON_HPP
#define REFLECT_CPP26_TYPE_TRAITS_TYPE_COMPARISON_HPP

// Root header: Include only:
// (1) C++ stdlib; (2) utils/config.h; (3) Other root headers
#include <type_traits>

namespace reflect_cpp26 {
/**
 * Whether T is exactly the same as one of Args.
 */
template <class T, class... Args>
concept same_as_one_of = (std::is_same_v<T, Args> || ...);

/**
 * Whether T is exactly the same as neither of Args.
 */
template <class T, class... Args>
concept same_as_none_of = !same_as_one_of<T, Args...>;

/**
 * Whether T is the same as one of Args after removing cv-qualifiers.
 */
template <class T, class... Args>
concept same_as_one_of_without_cv =
    (std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<Args>> || ...);

/**
 * Whether T is the same as neither of Args after removing cv-qualifiers.
 */
template <class T, class... Args>
concept same_as_none_of_without_cv = !same_as_one_of_without_cv<T, Args...>;

/**
 * Whether T is the same as one of Args after removing cvref-qualifiers.
 */
template <class T, class... Args>
concept same_as_one_of_without_cvref =
    (std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Args>> || ...);

/**
 * Whether T is the same as neither of Args after removing cvref-qualifiers.
 */
template <class T, class... Args>
concept same_as_none_of_without_cvref = !same_as_one_of_without_cvref<T, Args...>;

/**
 * Whether T and Args... are all the exactly same.
 */
template <class T, class... Args>
constexpr bool are_all_same_v = (std::is_same_v<T, Args> && ...);

// -------- Extension of std::same_as --------

template <class T, class U>
concept same_as = std::is_same_v<T, U>;

template <class T, class U>
concept same_as_without_cv = std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>;

template <class T, class U>
concept same_as_without_cvref = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template <class T, class U>
concept not_same_as = !std::is_same_v<T, U>;

template <class T, class U>
concept not_same_as_without_cv = !std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>;

template <class T, class U>
concept not_same_as_without_cvref = !std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_TYPE_COMPARISON_HPP
