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

#include <functional>
#include <print>
#include <reflect_cpp26/enum/enum_cast.hpp>
#include <reflect_cpp26/lookup.hpp>
#include <string>

namespace refl = reflect_cpp26;

enum class fruit_type {
  apple,
  banana,
  mango,
  orange,
  watermelon
};

struct dog_t {
  // Identifier "name" does not match the pattern "eat_*". Ignored.
  std::string name;

  // Identifier "eat_apple" matches the pattern "eat_*". The matched part '*' -> "apple".
  // Then, refl::enum_cast<fruit_type>("apple") -> fruit_type::apple.
  void eat_apple(int n) const {
    std::println("My dog {} eats {} apple(s).", name, n);
  }

  // Similarly: '*' -> "banana" -> refl::enum_cast<fruit_type>("banana") -> fruit_type::banana
  void eat_banana(int n) const {
    std::println("My dog {} eats {} banana(s).", name, n);
  }

  // Similarly: '*' -> "orange" -> refl::enum_cast<fruit_type>("orange") -> fruit_type::orange
  void eat_orange(int n) const {
    std::println("My dog {} eats {} orange(s).", name, n);
  }

  // Identifier "eat_chicken_leg" matches the pattern "eat_*", with '*' -> "chicken_leg".
  // Then, refl::enum_cast<fruit_type>("chicken_leg") -> std::nullopt
  // (since there is no entry named "chicked_leg" in enum class fruit_type). Ignored.
  void eat_chicken_leg(int n) const {
    std::println("My dog {} eats {} chicken leg(s)... Wait, this is not fruit at all!", name, n);
  }

  void eat_fruit(fruit_type type, int n) const {
    // How does it work:
    // For each member whose identifier matches pattern "eat_*", we call the 3rd arg,
    // i.e. the transformer function, with the '*'-matched part.
    // If the transformation result R != std::nullopt, then we take R.value() as the key.
    // Otherwise, this member is ignored.
    constexpr auto dispatch_table = REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(
        dog_t,    // 1st arg: The class whose members are to be traversed
        "eat_*",  // 2nd arg: The name pattern
        // 3rd arg: Name pattern transformer function
        // In this example, the transformer is a function object (see ./enum.md for details)
        // with call signature: (std::string_view) -> std::optional<fruit_type>
        refl::enum_cast<fruit_type>
        // 4th arg: Options of the lookup table. We use the default values in this example.

        // 5th arg: Access context, std::meta::access_context::current() by default.
        // Here we use the default value. Current access context has access to private members of
        // dog_t, thus &dog_t::eat_watermelon can be added to the table.
    );
    // Contents in dispatch_table:
    //   (fruit_type::apple, &dog_t::eat_apple)
    //   (fruit_type::banana, &dog_t::eat_banana)
    //   (fruit_type::orange, &dog_t::eat_orange)
    //   (fruit_type::watermelon, &dog_t::eat_watermelon)

    // Use decltype(dispatch_table)::value_type to get the value type.
    static_assert(std::is_same_v<void (dog_t::*)(int) const, decltype(dispatch_table)::value_type>);
    // Use size() member function to get the size, i.e. how many entries in the table.
    static_assert(dispatch_table.size() == 4);

    // Use operator[] to access the corresponding value, i.e. member function pointer.
    if (auto mptr = dispatch_table[type]; mptr != nullptr) {
      // mptr != nullptr: type is a key in dispatch_table
      std::invoke(mptr, this, n);
    } else {
      // mptr == nullptr: type is not a key in dispatch_table
      std::println("My dog {} can not eat this kind of fruit.", name);
    }
  }

private:
  // '*' -> "watermelon" -> refl::enum_cast<fruit_type>("watermelon") -> fruit_type::watermelon
  void eat_watermelon(int n) const {
    std::println("My dog {} eats {} WATERMELON(S)! OH MY GOD! DON'T TELL OTHER PEOPLE!", name, n);
  }
};

int main() {
  auto my_dog = dog_t{"Mr. Foobar"};
  my_dog.eat_fruit(fruit_type::apple, 3);
  my_dog.eat_fruit(fruit_type::banana, 4);
  my_dog.eat_fruit(fruit_type::watermelon, 5);
  my_dog.eat_fruit(fruit_type::mango, 6);
  return 0;
}

// Expected output:
// My dog Mr. Foobar eats 3 apple(s).
// My dog Mr. Foobar eats 4 banana(s).
// My dog Mr. Foobar eats 5 WATERMELON(S)! OH MY GOD! DON'T TELL OTHER PEOPLE!
// My dog Mr. Foobar can not eat this kind of fruit.
