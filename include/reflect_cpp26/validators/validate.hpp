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

#ifndef REFLECT_CPP26_VALIDATORS_VALIDATE_HPP
#define REFLECT_CPP26_VALIDATORS_VALIDATE_HPP

#include <reflect_cpp26/type_traits/class_types/flattenable.hpp>
#include <reflect_cpp26/validators/common.hpp>

namespace reflect_cpp26 {
/**
 * Validates all flattened non-static data members of obj with given access mode
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 */
template <access_mode Mode, partially_flattenable_class T>
constexpr bool validate_members(const T& obj)
{
  template for (constexpr auto m: flattened_nsdm_v<Mode, T>) {
    template for (constexpr auto v: validators_of_member_v<m.member>) {
      constexpr auto cur_validator = extract<v>();
      auto test_res = cur_validator.template test_ith_nsdm<m.index>(obj);
      if (!test_res) { return false; }
    }
  }
  return true;
}

template <partially_flattenable_class T>
constexpr bool validate_public_members(const T& obj) {
  return validate_members<access_mode::unprivileged>(obj);
}

template <partially_flattenable_class T>
constexpr bool validate_all_members(const T& obj) {
  return validate_members<access_mode::unchecked>(obj);
}

/**
 * Validates all flattened non-static data members of obj with given access mode
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 * Error description of the first detected validator violation
 * will be written to error_output if not nullptr.
 */
template <access_mode Mode, partially_flattenable_class T>
constexpr bool validate_members(const T& obj, std::string* error_output)
{
  if (error_output == nullptr) {
    return validate_members<Mode>(obj);
  }
  template for (constexpr auto m: flattened_nsdm_v<Mode, T>) {
    template for (constexpr auto v: validators_of_member_v<m.member>) {
      constexpr auto cur_validator = extract<v>();
      auto test_res = cur_validator.template test_ith_nsdm<m.index>(obj);
      if (!test_res) {
        *error_output += "Invalid member '";
        *error_output +=
          reflect_cpp26::identifier_of(m.member, "(anonymous data member)");
        *error_output += "': ";
        *error_output +=
          cur_validator.template make_error_message_of_ith_nsdm<m.index>(obj);
        return false;
      }
    }
  }
  return true;
}

template <partially_flattenable_class T>
constexpr bool validate_public_members(
  const T& obj, std::string* error_output)
{
  return validate_members<access_mode::unprivileged>(obj, error_output);
}

template <partially_flattenable_class T>
constexpr bool validate_all_members(const T& obj, std::string* error_output) {
  return validate_members<access_mode::unchecked>(obj, error_output);
}

/**
 * Validates all flattened non-static data members of obj with given access mode
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 * Datailed error description of all detected validator violations
 * will be written to error_output if not nullptr.
 */
template <access_mode Mode, partially_flattenable_class T>
constexpr bool validate_members_verbose(const T& obj, std::string* error_output)
{
  if (error_output == nullptr) {
    return validate_members<Mode>(obj);
  }
  auto res = true;
  template for (constexpr auto m: flattened_nsdm_v<Mode, T>) {
    auto res_cur_value = true;

    template for (constexpr auto v: validators_of_member_v<m.member>) {
      constexpr auto cur_validator = extract<v>();
      auto test_res = cur_validator.template test_ith_nsdm<m.index>(obj);
      if (!test_res) {
        if (res_cur_value) {
          *error_output += "Invalid member '";
          *error_output +=
            reflect_cpp26::identifier_of(m.member, "(anonymous data member)");
          *error_output += "':\n";
        }
        *error_output += "* ";
        *error_output +=
          cur_validator.template make_error_message_of_ith_nsdm<m.index>(obj);
        *error_output += '\n';
      }
      res_cur_value &= test_res;
    }
    res &= res_cur_value;
  }
  if (!res) { error_output->pop_back(); }
  return res;
}

template <partially_flattenable_class T>
constexpr bool validate_public_members_verbose(
  const T& obj, std::string* error_output)
{
  return validate_members_verbose<access_mode::unprivileged>(obj, error_output);
}

template <partially_flattenable_class T>
constexpr bool validate_all_members_verbose(
  const T& obj, std::string* error_output)
{
  return validate_members_verbose<access_mode::unchecked>(obj, error_output);
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_VALIDATORS_VALIDATE_HPP
