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

#ifndef REFLECT_CPP26_UTILS_PTR_RANGE_HPP
#define REFLECT_CPP26_UTILS_PTR_RANGE_HPP

#include <functional>
#include <meta>
#include <ranges>
#include <reflect_cpp26/type_traits/type_comparison.hpp>
#include <reflect_cpp26/utils/constant.hpp>

#define REFLECT_CPP26_IL(...)              \
  std::initializer_list<std::meta::info> { \
    __VA_ARGS__                            \
  }

namespace reflect_cpp26 {
namespace impl {
consteval int ptr_variant_alternative_index(std::meta::info T,
                                            std::span<const std::meta::info> Args) {
  auto n = Args.size();
  for (auto i = 0zU; i < n; i++) {
    auto conv = is_convertible_type(add_pointer(T), add_pointer(Args[i]));
    if (conv) return static_cast<int>(i);
  }
  return static_cast<int>(npos);
}

consteval bool is_ptr_variant_unary_visitor(std::meta::info Visitor,
                                            std::span<const std::meta::info> Args) {
  auto ret_type = std::meta::info{};
  auto n = Args.size();
  for (auto i = 0zU; i < n; i++) {
    auto params_il = {add_pointer(Args[i])};
    auto invocable = is_invocable_type(Visitor, params_il);
    if (!invocable) {
      return false;
    }
    auto cur_ret_type = invoke_result(Visitor, params_il);
    if (i == 0) {
      ret_type = cur_ret_type;
    } else if (!is_same_type(cur_ret_type, ret_type)) {
      return false;
    }
  }
  return true;
}

template <class T, class... Args>
constexpr int ptr_variant_alternative_index_v =
    ptr_variant_alternative_index(^^T, REFLECT_CPP26_IL(^^Args...));

template <class Visitor, class... Args>
concept ptr_variant_unary_visitor =
    is_ptr_variant_unary_visitor(^^Visitor, REFLECT_CPP26_IL(^^Args...));

struct ptr32_variant_data {
  int32_t index_;
  uint32_t pointer_;
};

struct ptr64_variant_data {
  ssize_t index_ : 16;
  uint64_t pointer_ : 48;
};

using ptr_variant_data =
    std::conditional_t<sizeof(void*) == 8, ptr64_variant_data, ptr32_variant_data>;
}  // namespace impl

template <class... Args>
class ptr_variant {
  static constexpr auto N = sizeof...(Args);

public:
  ptr_variant() : data_{.index_ = static_cast<int>(npos), .pointer_ = 0} {}
  ptr_variant(std::nullptr_t) : data_{.index_ = static_cast<int>(npos), .pointer_ = 0} {}

  template <class T>
    requires(convertible_to_one_of<T*, Args*...>)
  ptr_variant(T* ptr)
      : data_{.index_ = impl::ptr_variant_alternative_index_v<T, Args...>,
              .pointer_ = reinterpret_cast<uintptr_t>(ptr)} {}

  ptr_variant(const ptr_variant&) = default;
  ptr_variant(ptr_variant&&) = default;

  ptr_variant& operator=(const ptr_variant&) = default;
  ptr_variant& operator=(ptr_variant&&) = default;

  auto index() const -> size_t {
    return static_cast<size_t>(data_.index_);
  }

  template <same_as_one_of<std::add_pointer_t<Args>...> Pointer>
  auto get() const -> Pointer {
    using T = std::remove_pointer_t<Pointer>;
    constexpr auto I = impl::ptr_variant_alternative_index_v<T, Args...>;
    if (data_.index_ == I) {
      return reinterpret_cast<Pointer>(data_.pointer_);
    }
    return nullptr;
  }

  template <size_t I>
    requires(I < N)
  auto get() const /* -> T* */ {
    using T = Args...[I];
    if (static_cast<size_t>(data_.index_) == I) {
      return reinterpret_cast<T*>(data_.pointer_);
    }
    return static_cast<T*>(nullptr);
  }

  template <same_as_one_of<std::add_pointer_t<Args>...> Pointer>
  auto holds_alternative() const -> bool {
    using T = std::remove_pointer_t<Pointer>;
    constexpr auto I = impl::ptr_variant_alternative_index_v<T, Args...>;
    return data_.index_ == I;
  }

  template <size_t I>
    requires(I < N)
  auto holds_alternative() const -> bool {
    return static_cast<size_t>(data_.index_) == I;
  }

  template <impl::ptr_variant_unary_visitor<Args...> Visitor>
  auto visit(Visitor&& visitor) const {
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      using T = Args...[I];
      if (static_cast<size_t>(data_.index_) == I) {
        auto* cur = reinterpret_cast<T*>(data_.pointer_);
        return std::invoke(std::forward<Visitor>(visitor), cur);
      }
    }
    return std::invoke(std::forward<Visitor>(visitor), nullptr);
  }

private:
  impl::ptr_variant_data data_;
};

template <class Pointer, class... Args>
  requires(same_as_one_of<Pointer, std::add_pointer_t<Args>...>)
auto get(ptr_variant<Args...> v) -> Pointer {
  return v.template get<Pointer>();
}

template <size_t I, class... Args>
  requires(I < sizeof...(Args))
auto get(ptr_variant<Args...> v) {
  return v.template get<I>();
}

template <class Pointer, class... Args>
  requires(same_as_one_of<Pointer, std::add_pointer_t<Args>...>)
auto holds_alternative(ptr_variant<Args...> v) -> bool {
  return v.template holds_alternative<Pointer>();
}

template <size_t I, class... Args>
  requires(I < sizeof...(Args))
auto holds_alternative(ptr_variant<Args...> v) -> bool {
  return v.template holds_alternative<I>();
}

template <class... Args, impl::ptr_variant_unary_visitor<Args...> Visitor>
auto visit(Visitor&& visitor, ptr_variant<Args...> v) {
  return v.template visit(std::forward<Visitor>(visitor));
}

template <class... Args>
using const_ptr_variant = ptr_variant<const Args...>;
}  // namespace reflect_cpp26

#undef REFLECT_CPP26_IL

#endif  // REFLECT_CPP26_UTILS_PTR_RANGE_HPP
