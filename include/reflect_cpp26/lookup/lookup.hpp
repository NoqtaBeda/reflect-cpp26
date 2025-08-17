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

#ifndef REFLECT_CPP26_LOOKUP_LOOKUP_HPP
#define REFLECT_CPP26_LOOKUP_LOOKUP_HPP

#include <reflect_cpp26/enum/enum_index.hpp>
#include <reflect_cpp26/type_traits/is_invocable.hpp>
#include <reflect_cpp26/type_traits/template_instance.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>
#include <optional>

namespace reflect_cpp26 {
namespace impl::lookup {
template <class Predicate>
consteval void lookup_type_members_impl(
  std::vector<std::meta::info>& dest, std::meta::info T,
  const Predicate& predicate)
{

  for (auto member: public_direct_members_of(T)) {
    if (predicate(member)) {
      dest.push_back(member);
    }
  }
  if (is_class_type(T)) {
    for (auto base: public_direct_bases_of(T)) {
      if (is_virtual(base)) {
        compile_error("Virtual inheritance is not supported.");
      }
      lookup_type_members_impl(dest, type_of(base), predicate);
    }
  }
}

template <class Predicate>
consteval void lookup_type_members(
  std::vector<std::meta::info>& dest, std::meta::info T,
  const Predicate& predicate)
{
  if (!is_class_type(T) && !is_union_type(T)) {
    compile_error("T must be either of (1) class type; (2) or union type.");
  }
  lookup_type_members_impl(dest, T, predicate);
}

template <class Predicate>
consteval void lookup_namespace_members(
  std::vector<std::meta::info>& dest, std::meta::info ns,
  const Predicate& predicate)
{
  if (!is_namespace(ns)) {
    compile_error("ns must be namespace.");
  }
  for (auto member: public_direct_members_of(ns)) {
    if (predicate(member)) {
      dest.push_back(member);
    }
  }
}
} // namespace impl::lookup

/**
 * Gets members of class or union type T (including those inherited from
 * bases of T if T is a class type) that satisfy given predicate.
 */
template <invocable_r_with<bool, std::meta::info> Predicate>
consteval auto lookup_type_members(
  std::meta::info T, const Predicate& pred) -> std::vector<std::meta::info>
{
  auto res = std::vector<std::meta::info>{};
  impl::lookup::lookup_type_members(res, T, pred);
  return res;
}

/**
 * Gets direct members of namespace ns that satisfy given predicate.
 */
template <invocable_r_with<bool, std::meta::info> Predicate>
consteval auto lookup_namespace_members(
  std::meta::info ns, const Predicate& pred) -> std::vector<std::meta::info>
{
  auto res = std::vector<std::meta::info>{};
  impl::lookup::lookup_namespace_members(res, ns, pred);
  return res;
}

/**
 * Gets members of class or union type T (including those inherited from
 * bases of T if T is a class type) whose identifier satisfy given predicate.
 * All the members without identifier will be filtered out.
 */
template <invocable_r_with<bool, std::string_view> Predicate>
consteval auto lookup_type_members_by_name(
  std::meta::info T, const Predicate& pred) -> std::vector<std::meta::info>
{
  auto res = std::vector<std::meta::info>{};
  impl::lookup::lookup_type_members(res, T, [&pred](std::meta::info m) {
    return has_identifier(m) && pred(std::meta::identifier_of(m));
  });
  return res;
}

/**
 * Gets members of namespace ns whose identifier satisfy given predicate.
 * All the members without identifier will be filtered out.
 */
template <invocable_r_with<bool, std::string_view> Predicate>
consteval auto lookup_namespace_members_by_name(
  std::meta::info ns, const Predicate& pred) -> std::vector<std::meta::info>
{
  auto res = std::vector<std::meta::info>{};
  impl::lookup::lookup_namespace_members(res, ns,
    [&pred](std::meta::info m) {
      return has_identifier(m) && pred(std::meta::identifier_of(m));
    });
  return res;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_LOOKUP_LOOKUP_HPP
