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

namespace refl = reflect_cpp26;
using namespace std::string_view_literals;

class cat_t {
public:
  explicit cat_t(std::string_view name) : my_name_(name) {}

  using on_heard_callback_signature_t = void(std::string_view);
  using on_heard_callback_t = on_heard_callback_signature_t cat_t::*;

  // Filtered out due to identifier mismatch.
  void on_noticed(std::string_view who) {
    std::println("Hello {}!", who);
  }

  // Matches the filter.
  void on_heard_hello(std::string_view person_name) {
    std::println("The person {} greets to me.", person_name);
  }

  // Filtered out due to type mismatch:
  //     void(std::string_view, bool)
  // vs. void(std::string_view)
  void on_heard_hello(std::string_view person_name, bool is_stranger) {
    std::println("The person {} greets to me... Who is that? I {} remember.",
                 person_name,
                 is_stranger ? "can't" : "can");
  }

  // Matches the filter.
  void on_heard_meow(std::string_view cat_name) {
    std::println("The cat {} meows to me.", cat_name);
  }

  // Filtered out due to type mismatch:
  //     void(std::string_view) noexcept
  // vs. void(std::string_view)
  void on_heard_woof(std::string_view dog_name) noexcept {
    std::println("The dog {} woofs to me.", dog_name);
  }

  // Demo only: We do not let operator= return void in real practice.
  // Filtered out due to identifier missing.
  void operator=(std::string_view my_name) {
    my_name_ = my_name;
  }

private:
  std::string_view my_name_;

  // Filtered out due to inaccessibility.
  void on_heard_scream(std::string_view person_name) {
    std::println("The person {} is screaming. So horrible...", person_name);
  }
};

bool on_heard(cat_t cat, std::string_view sound, std::string_view who) {
  // How does it work:
  // For each addressable member, we invoke the filter function.
  // If the filter result R != std::nullopt, then we take R.value() as the key.
  // Otherwise, the member is filtered out.
  constexpr auto dispatch_table = REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(
      // 1st arg: The class whose members are to be traversed
      cat_t,
      // 2nd arg: The transformation function.
      // Returns std::nullopt if the member is to be filtered out.
      [](std::meta::info member) -> std::optional<std::string_view> {
        // Filters out all the members whose type is not exactly the expected.
        // Expects: void(std::string_view)
        if (!is_same_type(type_of(member), ^^cat_t::on_heard_callback_signature_t)) {
          return std::nullopt;
        }
        // Before checking its identifier, we need to check whether the member is not anonymous.
        // In this example we need to filter out an anonymous member function which is operator=.
        // Otherwise, compile error occurs on the following identifier_of() call.
        if (!has_identifier(member)) {
          return std::nullopt;
        }
        // Checks manually whether the identifier starts with "on_heard_".
        auto identifier = identifier_of(member);
        auto prefix = "on_heard_"sv;
        if (identifier.starts_with(prefix)) {
          identifier.remove_prefix(prefix.length());
          return identifier;
        }
        return std::nullopt;
      }
      // 3rd arg: Options of the lookup table. We use the default values in this example.

      // 4th arg: The access context, std::meta::access_context::current() by default.
      // Here we use the default value. The private member function on_heard_scream can not be
      // accessed in current context and thus filtered out.
  );
  // Contents in dispatch_table:
  //   ("hello", static_cast<cat_t::on_heard_callback_t>(&cat_t::on_heard_hello))
  //   ("meow", &cat_t::on_heard_meow)

  // Use decltype(dispatch_table)::value_type to get the value type.
  static_assert(std::is_same_v<cat_t::on_heard_callback_t, decltype(dispatch_table)::value_type>);
  // Use size() member function to get the size, i.e. how many entries in the table.
  static_assert(dispatch_table.size() == 2);

  // Use operator[] to access the corresponding value, i.e. member function pointer.
  if (auto mptr = dispatch_table[sound]; mptr != nullptr) {
    // mptr != nullptr: sound is a key in dispatch_table
    std::invoke(mptr, cat, who);
    return true;
  } else {
    // mptr == nullptr: sound is not a key in dispatch_table
    std::println("What sound is '{}'? I don't know.", sound);
    return false;
  }
}

int main() {
  auto my_cat = cat_t{"Ms. Kitty"};
  on_heard(my_cat, "hello", "Mohammud");
  on_heard(my_cat, "meow", "Cathy");
  on_heard(my_cat, "woof", "Mr. Goofy");
  on_heard(my_cat, "scream", "my neighbor");
  return 0;
}

// Expected output:
// The person Mohammud greets to me.
// The cat Cathy meows to me.
// What sound is 'woof'? I don't know.
// What sound is 'scream'? I don't know.
