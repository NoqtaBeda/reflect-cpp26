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

namespace stats {
// Identifier "g_call_count" matches the pattern "g_*_count".
// '*' -> "call".
int g_call_count = 0;
// '*' -> "gc"
int g_gc_count = 0;
// '*' -> "throw"
int g_throw_count = 0;
// '*' -> "catch"
int g_catch_count = 0;

// Nested namespaces are ignored since namespace is not addressable
// (i.e. &verbose is invalid C++ expression)
// Members in nested namespaces are ignored as well.
namespace verbose {
int g_jump_count = 0;
int g_load_field_count = 0;
int g_store_field_count = 0;
}  // namespace verbose

// For the function itself where the table is built:
// Identifier "increment_counter" does not match the pattern "g_*_count". Ignored.
bool increment_counter(std::string_view event_name, int n = 1) {
  // How does it work:
  // For each addressable member whose identifier matches pattern "g_*_count",
  // the matched part is key and pointer to the member is value.
  constexpr auto table = REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(
      stats,       // 1st arg: The namespace whose members are to be traversed
      "g_*_count"  // 2nd arg: The name pattern
      // 3rd arg: Options of the lookup table. We use the default values in this example.
  );
  // Contents in table:
  //   ("call", &g_call_count)
  //   ("gc", &g_gc_count)
  //   ("throw", &g_throw_count)
  //   ("catch", &g_catch_count)

  // Use decltype(table)::value_type to get the value type.
  static_assert(std::is_same_v<int*, decltype(table)::value_type>);
  // Use size() member function to get the size, i.e. how many entries in the table.
  static_assert(table.size() == 4);

  // Use operator[] to access the value, i.e. pointer to the variables.
  if (int* p = table[event_name]; p != nullptr) {
    // p != nullptr: event_name is a key in table.
    *p += n;
    return true;
  } else {
    // p == nullptr: event_name is not a key in table.
    std::println("WARN: Ignores unknown event '{}'.", event_name);
    return false;
  }
}
}  // namespace stats

int main() {
  stats::increment_counter("call");
  stats::increment_counter("gc", 2);
  stats::increment_counter("load_field");

  std::println("Function call: {} time(s).", stats::g_call_count);
  std::println("GC: {} time(s).", stats::g_gc_count);
  return 0;
}

// Expected output:
// WARN: Ignores unknown event 'load_field'.
// Function call: 1 time(s).
// GC: 2 time(s).
