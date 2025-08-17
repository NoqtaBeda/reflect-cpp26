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

#ifndef REFLECT_CPP26_VALIDATORS_LEAF_NON_NULL_TEST_HPP
#define REFLECT_CPP26_VALIDATORS_LEAF_NON_NULL_TEST_HPP

#include <reflect_cpp26/validators/impl/trivial_validator.hpp>
#include <any>
#include <memory>
#include <optional>

namespace reflect_cpp26::validators {
struct is_non_null_validator_t
  : impl::trivial_validator_without_params<is_non_null_validator_t>
{
  template <class T>
  static constexpr bool test(T* ptr) {
    return ptr != nullptr;
  }

  template <class T>
  static constexpr auto make_error_message(T*) -> std::string {
    return "Expects pointer to be non-null";
  }

  template <class T>
  static bool test(const std::shared_ptr<T>& ptr) {
    return ptr.get() != nullptr;
  }

  template <class T>
  static auto make_error_message(const std::shared_ptr<T>&) -> std::string {
    return "Expects shared pointer to be non-null";
  }

  template <class T, class D>
  static bool test(const std::unique_ptr<T, D>& ptr) {
    return ptr.get() != nullptr;
  }

  template <class T, class D>
  static auto make_error_message(const std::unique_ptr<T, D>&) -> std::string {
    return "Expects unique pointer to be non-null";
  }

  template <class T>
  static bool test(const std::weak_ptr<T>& ptr) {
    return !ptr.expired();
  }

  template <class T>
  static auto make_error_message(const std::weak_ptr<T>&) -> std::string {
    return "Expects weak pointer to be non-null";
  }

  static bool test(const std::any& a) {
    return a.has_value();
  }

  static auto make_error_message(const std::any&) -> std::string {
    return "Expects std::any to be non-null";
  }

  template <class T>
  static constexpr bool test(const std::optional<T>& opt) {
    return opt.has_value();
  }

  template <class T>
  static constexpr auto make_error_message(const std::optional<T>&)
    -> std::string {
    return "Expects std::optional to be non-null";
  }
};

constexpr auto is_non_null = is_non_null_validator_t{};
} // namespace reflect_cpp26::validators

#endif // REFLECT_CPP26_VALIDATORS_LEAF_NON_NULL_TEST_HPP
