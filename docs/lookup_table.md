## Introduction

reflect_cpp26 provides functionality to generate compile-time lookup table for class or namespace members. The lookup table is a fixed map whose keys are of either integral, enum or string type, and values are either pointers (to member variables), function pointers or member function pointers.

Lookup table utilizes [compile-time fixed map](./fixed_map.md) as the underlying data structures, which automatically selects the most suitable data structure (hash table, binary search, etc.). Read the document of fixed map for its implementation details.

The primary usage is to replace hand-written dispatching patterns of selecting candidate member or function, which is lengthy, inefficient and error-prone. The following example shows how reflect_cpp26 library helps to simplify dispatching with an auto-generated lookup table:

> Even in LLM era, reflect_cpp26 has its unique advantage that an efficient data structure can be applied automatically with your code kept clean. You don't have to worry that (1) you forget to instruct the agent to update all related components; (2) the agent generates flawed code for several items under illusion which is hard to be detected by human reviewing (especially when the number of items or related components is large); (3) the agent either fails to pick some good data structure, or generates a huge block of boilerplate code which harms the overall readability.

```cpp
namespace binary_ops {
int do_add(int x, int y);
int do_sub(int x, int y);
int do_mul(int x, int y);
int do_div(int x, int y);
// A lot more ...
}  // namespace binary_ops

// ⚠️ You may forget to keep consistency with namespace binary_ops
int do_binary_op_before(std::string_view key, int x, int y) {
  // Complexity: O(N) times of string comparison.
  if (key == "add") { return binary_ops::do_add(x, y); }
  else if (key == "sub") { return binary_ops::do_sub(x, y); }
  else if (key == "nul") { return binary_ops::do_mul(x, y); }  // ⚠️ TYPO, maybe hard to debug
  else if (key == "div") { return binary_ops::do_div(x, y); }
  // A lot more ...
  return 0;  // On mismatch
}

int do_binary_op_after(std::string_view key, int x, int y) {
  // Complexity: Depends on the underlying data structure. Typically better than O(N).
  constexpr auto dispatch_table = REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(binary_ops, "do_*");
  if (auto fptr = dispatch_table[key]; fptr != nullptr) { return fptr(x, y); }
  return 0;  // On mismatch
}
```

Components of lookup table are defined in header `<reflect_cpp26/lookup.hpp>`.

## Quick Start

More examples can be found in examples/lookup folder.

```cpp
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
```

## Description

The two macros below are the interface of creating lookup tables. The wrapped `make_*_member_lookup_table()` functions return the reflector of a compile-time constant object which is the lookup table instance generated, then the macros extract the instance with splice operator. Examples of usage are shown in "Quick Start" section.

```cpp
#define REFLECT_CPP26_CLASS_MEMBER_LOOKUP_TABLE(T, ...) \
  [:reflect_cpp26::make_class_member_lookup_table(^^T, ##__VA_ARGS__):]

#define REFLECT_CPP26_NAMESPACE_MEMBER_LOOKUP_TABLE(ns, ...) \
  [:reflect_cpp26::make_namespace_member_lookup_table(^^ns, ##__VA_ARGS__):]
```

Core functions (both of below have a collection of overloads, see section "Components" for details):
* `make_class_member_lookup_table()` generates a lookup table of members in class `T` at compile-time.
  * Members in its base classes (including indirect ones) can be **included**;
  * Members in its nested classes are **not included**.
* `make_namespace_member_lookup_table()` generates a lookup table of members in namespace `ns` at compile-time.   
  * Members in parent namespace and nested namespaces (if any) are **not included**.

Key type of the generated lookup tables can only be one of the following:
* Integral types. In this case signedness-safe and narrowing-safe comparisons are performed on keys;
* Enum types;
* `std::string_view`.

Duplicated keys are not allowed. This constraint inherits from the [undelying fixed map](./fixed_map.md).

Value type of the generated lookup tables for class `T` can be one of the following:
* For non-static data members: Let `M` be the member type (`M` may be cv-qualified). Value type of lookup table is `M T::*`;
* For static data members: Let `M` be the member type (`M` may be cv-qualified). Value type of lookup table is `M*`;
* For non-static member functions: Let `R(Args...) [qualifiers...]` be the function type. Value type of lookup table is `R (T::*)(Arg...) [qualifiers...]`;
* For static member functions: Let `R(Args...) [noexcept]` be the function type. Value type of lookup table is `R (*)(Args...) [noexcept]`.

Value type of the generated lookup tables for namespace `ns` can be one of the following:
* For variables: Let `M` be the variable type (`M` may be cv-qualified). Value type of lookup table is `M*`;
* For functions: Let `R(Args...) [noexcept]` be the function type. Value type of lookup table is `R (*)(Args...) [noexcept]`.

All members that can be added to the lookup table must share **exactly** the same type. Please take care of qualifiers like `const`, `noexcept`, etc. Compilation error is raised otherwise.

The result is a `std::meta::info`, reflector of a compile-time constant which is the instance of the lookup table generated, which can be extracted with splice operator (as the macros above do).

Each overload of `make_*_member_lookup_table()` has 2 trailing arguments in common:
1. `const *_member_lookup_table_options& options = {}` specifies customized options of the lookup table;
2. `std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT` specifies the access context. The default value `REFLECT_CPP26_CURRENT_CONTEXT = std::meta::access_context::current()`.

Members are added to the lookup table only if all the following common constraints are satisfied:
* Its category matches `options.category`, or `options.category == unspecified`;
* It is accessible under given context;
* It is addressable (see [utils: section "Testing Addressable Members"](./utils.md) for details).

Each overload of `make_*_member_lookup_table()` has its own additional constraints.

## Components

All the components shown below are defined in header `<reflect_cpp26/lookup/lookup_table.hpp>`.

### Configuration Parameters

```cpp
enum class class_member_category {
  unspecified,
  nonstatic_data_members,
  static_data_members,
  nonstatic_member_functions,
  static_member_functions,
};

enum class namespace_member_category {
  unspecified,
  variables,
  functions,
};

struct class_member_lookup_table_options {
  class_member_category category = class_member_category::unspecified;
  fixed_map_options_variant fixed_map_options;
};

struct namespace_member_lookup_table_options {
  namespace_member_category category = namespace_member_category::unspecified;
  fixed_map_options_variant fixed_map_options;
};
```

`class_member_category` is used to specify which category of class members to include in the lookup table:
  * `unspecified`: All addressable members are candidates;
  * `nonstatic_data_members`: Only non-static data members;
  * `static_data_members`: Only static data members;
  * `nonstatic_member_functions`: Only non-static member functions;
  * `static_member_functions`: Only static member functions.
  
`namespace_member_category` is used to specify which category of namespace members to include in the lookup table:
  * `unspecified`: All addressable members are candidates;
  * `variables`: Only variables;
  * `functions`: Only functions.

Configuration parameters are defined as `struct class_member_lookup_table_options` and `struct namespace_member_lookup_table_options`, for class members and namespace members respectively:
* `category` selects which category of class or namespace members will be picked;
* `fixed_map_options` provides options for the underlying fixed map implementation. See [document of fixed map](./fixed_map.md) for details.
  * If the desired key of lookup table is of integral or enum type, then `fixed_map_options` should hold an object of `struct integral_key_fixed_map_options` with customized options;
  * If the desired key of lookup table is of string type, then `fixed_map_options` should hold an object of `struct string_key_fixed_map_options` with customized options.

### Member Lookup by Identifier Pattern

```cpp
// (1.1)
consteval auto make_class_member_lookup_table(
    std::meta::info T,
    std::string_view pattern,
    const class_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info;

// (1.2)
consteval auto make_namespace_member_lookup_table(
    std::meta::info ns,
    std::string_view pattern,
    const namespace_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info;
```

Members are added to the lookup table only if:
* It is non-anonymous and its identifier matches the `pattern`.

The `pattern` string is constrained to the format `"prefix*suffix"` where `'*'` represents the matched part. Then the matched part (i.e. prefix & suffix removed) is used as the key of corresponding member. For example:
* `"get*"` matches `"getName"`, `"getUserList"`, etc., then
  * `"Name"` is the key to member `getName`;
  * `"UserList"` is the key to member `getUserList`;
* `"is_*_member"` matches `"is_static_data_member"`, `"is_valid_member"`, etc., then
  * `"static_data"` is the key to member `is_static_data_member`;
  * `"valid"` is the key to member `is_valid_member`.

### Member Lookup by Transformation Function

```cpp
// (2.1)
template <class TransformFn>
consteval auto make_class_member_lookup_table(
    std::meta::info T,
    const TransformFn& transform_fn,
    const class_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info;

// (2.2)
template <class TransformFn>
consteval auto make_namespace_member_lookup_table(
    std::meta::info ns,
    const TransformFn& transform_fn,
    const namespace_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info;

// (3.1)
template <class TransformFn>
consteval auto make_namespace_member_lookup_table(
    std::meta::info ns,
    std::string_view pattern,
    const TransformFn& transform_fn,
    const namespace_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info;

// (3.2)
template <class TransformFn>
consteval auto make_class_member_lookup_table(
    std::meta::info T,
    std::string_view pattern,
    const TransformFn& transform_fn,
    const class_member_lookup_table_options& options = {},
    std::meta::access_context ctx = REFLECT_CPP26_CURRENT_CONTEXT) -> std::meta::info;
```

For overload (2), `transform_fn` is a callable which has one of the following call signatures (the result type `R` below should be either integral, enum or string-like type):
1. `transform_fn(std::meta::info) -> R`;
2. `transform_fn(std::meta::info) -> std::optional<R>`;
3. `transform_fn(std::string_view) -> R`;
4. `transform_fn(std::string_view) -> std::optional<R>`.

For call signature #2 and #4, each member is added to the lookup table only if the result of `transform_fn` is not `std::nullopt`.
For call signature #3 and #4, each member is added only if it is non-anonymous. In this case its identifier is passed to `transform_fn`.

For overload (3), `transform_fn` is a callable which has one of the following call signatures (the result type `R` below should be either integral, enum or string-like type):
1. `transform_fn(std::string_view) -> R`;
2. `transform_fn(std::string_view) -> std::optional<R>`.

Each member is added to the lookup table only if all the following additional constraints are satisfied:
* It is non-anonymous and its identifier matches the given `pattern`. Requirements of `pattern` format are the same as above. The *matched part* is passed to `transform_fn`;
* For call signature #2 of `transform_fn`, the result is not `std::nullopt`.
