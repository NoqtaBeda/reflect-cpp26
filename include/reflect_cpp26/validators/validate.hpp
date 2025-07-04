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
 * Validates all flattened public non-static data members of obj
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 */
template <partially_flattenable_class T>
constexpr bool validate_members(const T& obj)
{
  template for (constexpr auto m: public_flattened_nsdm_v<T>) {
    template for (constexpr auto v: validators_of_member_v<m.member>) {
      constexpr auto cur_validator = extract<v>();
      if (!cur_validator.test(obj.[:m.member:])) {
        return false;
      }
    }
  }
  return true;
}

/**
 * Validates all flattened public non-static data members of obj
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 * Error description of the first detected validator violation
 * will be written to error_output if not nullptr.
 */
template <partially_flattenable_class T>
constexpr bool validate_members_with_error_info(
  const T& obj, std::string* error_output)
{
  if (error_output == nullptr) {
    return validate_members(obj);
  }
  template for (constexpr auto m: public_flattened_nsdm_v<T>) {
    template for (constexpr auto v: validators_of_member_v<m.member>) {
      constexpr auto cur_validator = extract<v>();
      if (!cur_validator.test(obj.[:m.member:])) {
        if (error_output != nullptr) {
          *error_output += "Invalid member '";
          *error_output += identifier_of(m.member);
          *error_output += "': ";
          *error_output += cur_validator.make_error_message(obj.[:m.member:]);
        }
        return false;
      }
    }
  }
  return true;
}

/**
 * Validates all flattened public non-static data members of obj
 * (i.e. including those inherited from base classes)
 * with annotated validators of each member.
 * Datailed error description of all detected validator violations
 * will be written to error_output if not nullptr.
 */
template <partially_flattenable_class T>
constexpr bool validate_members_with_full_error_info(
  const T& obj, std::string* error_output)
{
  if (error_output == nullptr) {
    return validate_members(obj);
  }
  auto res = true;
  template for (constexpr auto m: public_flattened_nsdm_v<T>) {
    auto res_cur_value = true;

    template for (constexpr auto v: validators_of_member_v<m.member>) {
      constexpr auto cur_validator = extract<v>();
      auto cur_res = cur_validator.test(obj.[:m.member:]);
      if (!cur_res) {
        if (res_cur_value) {
          *error_output += "Invalid member '";
          *error_output += std::meta::identifier_of(m.member);
          *error_output += "':\n";
        }
        *error_output += "* ";
        *error_output += cur_validator.make_error_message(obj.[:m.member:]);
        *error_output += '\n';
      }
      res_cur_value &= cur_res;
    }
    res &= res_cur_value;
  }
  if (!res) {
    error_output->pop_back(); // Removes the trailing '\n'
  }
  return res;
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_VALIDATORS_VALIDATE_HPP
