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

#ifndef REFLECT_CPP26_TYPE_TRAITS_TEMPLATE_INSTANCE_HPP
#define REFLECT_CPP26_TYPE_TRAITS_TEMPLATE_INSTANCE_HPP

// Root header: Include only:
// (1) C++ stdlib; (2) utils/config.h; (3) Other root headers
#include <type_traits>

namespace reflect_cpp26 {
namespace impl {
template <class T, template <class...> class Templ>
struct is_template_instance_of : std::false_type {};

template <template <class...> class Templ, class... Args>
struct is_template_instance_of<Templ<Args...>, Templ> : std::true_type {};

template <class T, template <auto...> class Templ>
struct is_nontype_template_instance_of : std::false_type {};

template <template <auto...> class Templ, auto... Args>
struct is_nontype_template_instance_of<Templ<Args...>, Templ> : std::true_type {};
}  // namespace impl

/**
 * Whether std::remove_cvref_t<T> is a template instance of given type template.
 * Note: Templates with mixed type and non-type parameters
 * (e.g. std::array<T, N>) are not supported,
 * in which case std::meta::template_of() can be used instead.
 *
 * Example: static_assert(template_instance_of<
 *   std::vector<int, MyAllocator<int>>,
 *   std::vector>)
 */
template <class T, template <class...> class Templ>
concept template_instance_of = impl::is_template_instance_of<std::remove_cvref_t<T>, Templ>::value;

/**
 * Whether std::remove_cvref_t<T> is a template instance of given
 * non-type template.
 * Note: Templates with mixed type and non-type parameters
 * (e.g. std::array<T, N>) are not supported,
 * in which case std::meta::template_of() can be used instead.
 *
 * Example: static_assert(nontype_template_instance_of<
 *   std::ratio<2, 5>,
 *   std::ratio)
 */
template <class T, template <auto...> class Templ>
concept nontype_template_instance_of =
    impl::is_nontype_template_instance_of<std::remove_cvref_t<T>, Templ>::value;
}  // namespace reflect_cpp26

#endif  // REFLECT_CPP26_TYPE_TRAITS_TEMPLATE_INSTANCE_HPP
