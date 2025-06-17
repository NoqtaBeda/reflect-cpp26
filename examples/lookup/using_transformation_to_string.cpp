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

#include <print>
#include <reflect_cpp26/lookup.hpp>
#include <reflect_cpp26/utils/identifier_naming.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>  // all_direct_nonstatic_data_members_v
#include <string>

namespace refl = reflect_cpp26;

struct RequestArgs {
  std::string category;
  std::string nameRegex;
  std::string startDateTime;
  std::string endDateTime;

  void DumpFields() const {
    template for (constexpr auto field : refl::all_direct_nonstatic_data_members_v<RequestArgs>) {
      std::println("Field '{}' is set to {:?}", identifier_of(field), this->[:field:]);
    }
  }
};

int main() {
  constexpr auto kFieldsTable = REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(
      // 1st arg: The class whose members are to be traversed
      RequestArgs,
      // 2nd arg: Name transformer function
      // In this example, the transformer is a function object defined in
      // <reflect_cpp26/utils/identifier_naming.hpp>
      // with call signature: (std::string_view) -> std::optional<std::string>
      // This function object converts the names to kebab-case.
      refl::to_kebab_case_opt,
      // 3rd arg: Options of the underlying fixed map.
      // In this example we pick non-static data members only.
      // All member functions (including the implicit ones) are filtered out.
      {.category = refl::class_member_category::nonstatic_data_members}
      // 4th arg: The access context, std::meta::access_context::current() by default.
      // In this example access context has no influence since all members in class RequestArgs
      // are public.
  );

  // Use decltype(kFieldsTable)::value_type to get the value type.
  static_assert(std::is_same_v<std::string RequestArgs::*, decltype(kFieldsTable)::value_type>);
  // Use size() member function to get the size, i.e. how many entries in the table.
  static_assert(kFieldsTable.size() == 4);

  auto args = RequestArgs{};
  // Accesses the table with operator[]. Null check is omitted in this example.
  args.*kFieldsTable["category"] = "Cats";
  args.*kFieldsTable["name-regex"] = "[A-Za-z]+";
  args.*kFieldsTable["start-date-time"] = "2026-01-01T00:00:00+08:00";
  args.*kFieldsTable["end-date-time"] = "2026-01-15T00:00:00+08:00";

  // Member functions are filtered out.
  static_assert(kFieldsTable["DumpFields"] == nullptr);
  static_assert(kFieldsTable["dump-fields"] == nullptr);

  // Prints all the fields of args.
  args.DumpFields();

  return 0;
}

// Expected output:
// Field 'category' is set to "Cats"
// Field 'nameRegex' is set to "[A-Za-z]+"
// Field 'startDateTime' is set to "2026-01-01T00:00:00+08:00"
// Field 'endDateTime' is set to "2026-01-15T00:00:00+08:00"
