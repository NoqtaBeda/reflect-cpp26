## Introduction

reflect_cpp26 provides common functionality for enum types, including but not limited to:
* enum metadata: `enum_entries`, `enum_names`, `enum_values`, `enum_type_name`, etc.;
* conversion between enum value and string: `enum_name`, `enum_cast`, etc.;
* enum flag value decomposition and string parsing: `enum_flags_name`, `enum_flags_cast`, etc.;
* `std::format` and `fmt::format` support;
* operator overloading support (including bitwise and comparison operators).

Part of API design resembles [magic_enum](https://github.com/Neargye/magic_enum), a well-known enum static reflection library based on compiler magic before C++26.

## Components

### Ordering Configuration

Defined in header `<reflect_cpp26/enum/enum_entry_order.hpp>` (implicitly included by its users).

```cpp
namespace reflect_cpp26 {

enum class enum_entry_order {
  original,
  by_value,
  by_name,
};

}  // namespace reflect_cpp26
```

* `enum_entry_order` is used for customized ordering when accessing entry list of enum type:
  * `original`: Keeps the order of enum definition (which is the default option);
  * `by_value`: Sorts enum entries by their underlying values in ascending order. For multiple entries with the same underlying value, their original order is preserved;
  * `by_name`: Sorts enum entries by their names, case-sensitive in ascending order.

### Enum Metadata

#### Enum Type Name

Define in header `<reflect_cpp26/enum/enum_type_name.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E>
constexpr std::string_view enum_type_name_v;

template <enum_type E>
constexpr std::string_view enum_type_display_string_v;

}  // namespace reflect_cpp26
```

* `enum_type_name_v<E>` gets the type name (dealiased, cv-qualifiers discarded) of given enum type.
* `enum_type_display_string_v<E>` gets the prettier type name of given enum type, which is implementation-defined by the compiler.

Example:
```cpp
enum class foo: int;
namespace bar {
  enum class values: long;
}  // namespace bar
using bar_values = bar::values;

namespace refl = reflect_cpp26;
static_assert(refl::enum_type_name_v<foo> == "foo");
static_assert(refl::enum_type_name_v<const bar::values> == "values"); // cv discarded
static_assert(refl::enum_type_name_v<bar_values> == "values"); // Dealiased
// Implementation defined, may be "const bar::values"
printf("%s\n", refl::enum_type_display_string_v<const bar_values>.data());
```

#### Number of Enum Entries

Defined in header `<reflect_cpp26/enum/enum_count.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E>
constexpr size_t enum_count_v;

template <enum_type E>
constexpr size_t enum_unique_count_v;

}  // namespace reflect_cpp26
```

* `enum_count_v<E>` gets the number of entries in given enum type.
* `enum_unique_count_v<E>` gets the number of unique entries (i.e. with distinct underlying values).

Example:
```cpp
enum class foo {
  zero = 0,
  one = 1, uno = 1, eins = 1,
  two = 2, zwei = 2,
  three = 3, drei = 3,
  ten = 10,
};

namespace refl = reflect_cpp26;
static_assert(refl::enum_count_v<foo> == 9);
static_assert(refl::enum_unique_count_v<foo> == 5); // Five distinct values: 0, 1, 2, 3, 10
```

#### Enum Entry List as (Value, Name) Pair

Defined in header `<reflect_cpp26/enum/enum_entries.hpp>`

```cpp
namespace reflect_cpp26 {

template <enum_type E>
using enum_entry_t = std::pair<std::remove_cv_t<E>, std::string_view>;

template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr std::span<const enum_entry_t<E>> enum_entries_v;

}  // namespace reflect_cpp26
```

* `enum_entries_v<E>` gets the (value, string) pair list of given enum type.

Example:
```cpp
enum class foo {
  three = 3,
  one = 1,
  zero = 0,
  two = 2,
};

namespace refl = reflect_cpp26;

using enum enum_entry_order;
template for (constexpr auto order: {original, by_value, by_name}) {
  for (auto [value, name]: refl::enum_entries_v<foo>) {
    std::println("{:>5s}: {}", name, std::to_underlying(value));
  }
}
// Output of each order:
// | original | by_value |  by_name |
// |----------|----------|----------|
// | three: 3 |  zero: 0 |   one: 1 |
// |   one: 1 |   one: 1 | three: 3 |
// |  zero: 0 |   two: 2 |   two: 2 |
// |   two: 2 | three: 3 |  zero: 0 |
```

#### Enum Entry Name List

Defined in header `<reflect_cpp26/enum/enum_names.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr std::span<const std::string_view> enum_names_v;

template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr std::span<const std::string_view> enum_names_tolower_v;

}  // namespace reflect_cpp26
```

* `enum_names_v<E>` gets the name list of given enum type.
* `enum_names_tolower_v<E>` gets the case-insensitive name list of given enum type where each name is converted to all-lower-case. Note:
  * Duplicated strings may exist in `enum_names_tolower_v<E>` if multiple enum names are equal by case-insensitive comparison;
  * **ASCII only**: Non-ASCII characters are NOT supported.

Example:
```cpp
enum foo {
  foo_First = 1,
  foo_Second = 2,
  foo_Invalid = -1,
  foo_FIRST = 1,
};

namespace refl = reflect_cpp26;
using enum enum_entry_order;
template for (constexpr auto order: {original, by_value, by_name}) {
  for (size_t i = 0; i < refl::enum_count_v<foo>; i++) {
    std::println("{0:>11s}, {1:>11s}",
                 refl::enum_names_v<foo, order>[i],
                 refl::enum_names_tolower_v<foo, order>[i]);
  }
}
// Output of each order:
// |        original          |        by_value          |         by_name          |
// |--------------------------|--------------------------|--------------------------|
// |   foo_First,   foo_first | foo_Invalid, foo_invalid |   foo_FIRST,   foo_first |
// |  foo_Second,  foo_second |   foo_First,   foo_first |   foo_First,   foo_first |
// | foo_Invalid, foo_invalid |   foo_FIRST,   foo_first | foo_Invalid, foo_invalid |
// |   foo_FIRST,   foo_first |  foo_Second,  foo_second |  foo_Second,  foo_second |
```

#### Enum Entry Value List

Defined in header `<reflect_cpp26/enum/enum_values.hpp>`.

```cpp
namespace reflect_cpp26 {

template <class E, enum_entry_order Order = enum_entry_order::original>
constexpr std::span<const std::remove_cv_t<E>> enum_values_v;

}  // namespace reflect_cpp26
```

* `enum_values_v<E>` gets the value list of given enum type.
Example see below (combined with the next section "Enum Reflection Entry List").

#### Enum Reflection Entry List

Defined in header `<reflect_cpp26/enum/enum_meta_entries.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr std::span<const std::meta::info> enum_meta_entries_v;

}  // namespace reflect_cpp26
```

* `enum_meta_entries_v<E>` gets the reflector list of entries in given enum type.
For the default order, `enum_meta_entries_v<E>` is equivalent to `std::define_static_array(enumerators_of(^^E))` with less typing.

Example:
```cpp
enum class foo {
  three = 3,
  one = 1,
  zero = 0,
  two = 2,
};

namespace refl = reflect_cpp26;

using enum enum_entry_order;
// The following line is example of enum_meta_entries_v
template for (constexpr auto order: refl::enum_meta_entries_v<refl::enum_entry_order>) {
  std::print("{}:", identifier_of(order));
  for (auto value: refl::enum_values_v<foo, order>) { // Example of enum_values_v
    std::print(" {}", std::to_underlying(value));
  }
  std::print("; ");
}
// original: 3 1 0 2; by_value: 0 1 2 3; by_name: 1 3 2 0;
```

#### Enum Type Hash

Defined in header `<reflect_cpp26/enum/enum_hash.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E>
constexpr uint64_t enum_hash_v;

}  // namespace reflect_cpp26
```

* `enum_hash_v<E>` gets the 64-bit hash value of given enum type.

Any change to the entry set (including insertion, deletion and modification to entry name or value) will change the hash value. But only changing entry order or name of the enum type itself will not.
Enum hash is useful to guard an enum type from unexpected updates with:
* new entries added, until the developer finishes work with this new entry;
* existing entries deleted, until the developer finishes cleanup work with the deleted entry;
* entry name change, to enforce semantic consistency;
* entry value change.

Example:
```cpp
enum class foo_0 { one = 1, two = 2, three = 3 };
enum class foo_1 { one = 1, two = 2, three = 3 };
enum class foo_2 { one = 1, three = 3, two = 2 };
enum class foo_3 { one = 1, two = 2, three = 3, four = 4 };
enum class foo_4 { two = 2, three = 3 };
enum class foo_5 { one = 1, two = 2, three = -3 };
enum class foo_6 { one = 1, zwei = 2, three = 3 };

namespace refl = reflect_cpp26;
// Same hash
static_assert(refl::enum_hash_v<foo_0> == refl::enum_hash_v<foo_1>); // Identical entries
static_assert(refl::enum_hash_v<foo_0> == refl::enum_hash_v<foo_2>); // Order change only
// Different hash
static_assert(refl::enum_hash_v<foo_0> != refl::enum_hash_v<foo_3>); // Adds new entry
static_assert(refl::enum_hash_v<foo_0> != refl::enum_hash_v<foo_4>); // Deletes entry
static_assert(refl::enum_hash_v<foo_0> != refl::enum_hash_v<foo_5>); // Value change
static_assert(refl::enum_hash_v<foo_0> != refl::enum_hash_v<foo_6>); // Name change
```

#### Enum Entries Dumped as JSON String

Defined in header `<reflect_cpp26/enum/enum_json.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr std::string enum_json();

template <enum_type E, enum_entry_order Order = enum_entry_order::original>
constexpr std::string_view enum_json_static_v;

}  // namespace reflect_cpp26
```

* `enum_json<E>()` gets the JSON representation of given enum class in compact style. The returned `std::string` is built (possibly) during runtime.
* `enum_json_static_v<E>` gets the same string which is built during compile-time and guaranteed to be null-terminated.

Example:
```cpp
enum class foo {
  three = 3,
  one = 1,
  zero = 0,
  two = 2,
};

namespace refl = reflect_cpp26;
constexpr auto by_value = refl::enum_entry_order::by_value;
// original: {"three":3,"one":1,"zero":0,"two":2}
printf("original: %s\n", refl::enum_json<foo>().c_str());
// by_value: {"zero":0,"one":1,"two":2,"three":3}
printf("by_value: %s\n", refl::enum_json_static_v<foo, by_value>.data());
```

### Enum Operations

#### Conversion from Enum Value to Name

Defined in header `<reflect_cpp26/enum/enum_name.hpp>`.

```cpp
namespace reflect_cpp26 {

struct enum_name_t {
  // (1)
  template <enum_type E>
  static constexpr auto operator()(E e) -> std::string_view;
};

struct enum_name_opt_t {
  // (2)
  template <enum_type E>
  static constexpr auto operator()(E e) -> std::optional<std::string_view>;
};

constexpr auto enum_name = enum_name_t{};
constexpr auto enum_name_opt = enum_name_opt_t{};

}  // namespace reflect_cpp26
```

* (1) `enum_name(e)` gets the name of given enum value `e`, or `std::string_view{}` if no such value defined in the enum type.
* (2) `enum_name_opt(e)` gets the name of given enum value, or `std::nullopt` if no such value defined in the enum type. This version is helpful with monadic operations of `std::optional` since C++23.

If multiple enum entries have the same value as `e`, name of the first one by definition order is returned.

Example:
```cpp
enum class foo {
  one = 1,
  two = 2,
  three = 3,
  invalid = -1,
  first = 1,
  last = 3,
};

enum bar {
  bar_one = 1,
  bar_two = 2,
  bar_three = 3,
};

namespace refl = reflect_cpp26;
static_assert(refl::enum_name(foo::two) == "two");
// 'three' is defined before 'last' in enum class foo
static_assert(*refl::enum_name_opt(foo::last) == "three");
// No entry defined in enum class foo
static_assert(refl::enum_name(static_cast<foo>(-2)) == "");
static_assert(refl::enum_name_opt(static_cast<foo>(-3)) == std::nullopt);

// Example of monadic operation with std::optional (introduced in C++23)
auto convert_to_bar(foo value) -> std::optional<bar> {
  return refl::enum_name_opt(value)
    .and_then([](std::string_view foo_name) {
      auto bar_name = std::format("bar_{}", foo_name);
      return refl::enum_cast<bar>(bar_name); // Example of enum_cast (see below)
    });
}
```

#### Conversion from String or Integer to Enum Value

Defined in header `<reflect_cpp26/enum/enum_cast.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E>
struct enum_cast_t {
private:
  using ENoCV = std::remove_cv_t<E>;

public:
  // (1.1)
  static constexpr auto operator()(std::string_view str) -> std::optional<ENoCV>;
  // (1.2)
  static constexpr auto operator()(ascii_case_insensitive_tag_t, std::string_view str)
      -> std::optional<ENoCV>;
  // (2)
  static constexpr auto operator()(std::integral auto value) -> std::optional<ENoCV>;
};

template <enum_type E>
constexpr auto enum_cast = enum_cast_t<std::remove_cv_t<E>>{};

}  // namespace reflect_cpp26
```

* (1.1) `enum_cast<E>(str)` converts given string to the enum value whose name matches, or `std::nullopt` if such enum entry does not exist;
* (1.2) `enum_cast<E>(ascii_case_insensitive, str)` is similar to (1.1) yet finds enum entry in a case-insensitive manner. Compilation error will be raised in case of entry name duplication or non-ASCII characters in enum entry definition;
* (2) `enum_cast<E>(value)` converts given integer to the enum value whose underlying value matches, or `std::nullopt` if such enum entry does not exist. signedness-safe and narrowing-safe comparison is performed.

Examples:
```cpp
enum class foo : int {
  one = 1,
  two = 2,
  three = 3,
  invalid = -1,
  first = 1,
  last = 3,
};

namespace refl = reflect_cpp26;
static_assert(refl::enum_cast<foo>("first") == foo::one);
static_assert(refl::enum_cast<foo>(refl::ascii_case_insensitive, "LAST") == foo::three);
static_assert(refl::enum_cast<foo>(2) == foo::two);
// Signedness-safe comparison: -1u does not match -1 despite identical bit representation.
static_assert(refl::enum_cast<foo>(-1u) == std::nullopt);
// Narrowing-safe comparison.
static_assert(refl::enum_cast<foo>(0x1'0000'0001) == std::nullopt);
```

#### Checking Existence of Enum Value

Defined in header `<reflect_cpp26/enum/enum_includes.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E>
struct enum_contains_t {
private:
  using ENoCV = std::remove_cv_t<E>;

public:
  // (1)
  static constexpr bool operator()(ENoCV e);
  // (2)
  static constexpr bool operator()(std::integral auto value);
  // (3.1)
  static constexpr bool operator()(std::string_view str);
  // (3.2)
  static constexpr bool operator()(ascii_case_insensitive_tag_t, std::string_view str);
};

template <enum_type E>
constexpr auto enum_contains = enum_contains_t<std::remove_cv_t<E>>{};

}  // namespace reflect_cpp26
```

* (1) `enum_contains<E>(e)` checks whether given enum value is defined in the enum type;
* (2) `enum_contains<E>(value)` checks whether some enum entry with given underlying value exists in the enum type. Signedness-safe and narrowing-safe comparison is performed;
* (3.1) `enum_contains<E>(str)` checks whether some enum entry with given name exists in the enum type;
* (3.2) `enum_contains<E>(ascii_case_insensitive, str)` is similar to (3.1) yet in a case-insensitive manner. Compilation error will be raised in case of non-ASCII characters in enum entry definition.

Example:
```cpp
enum class foo : int {
  one = 1,
  two = 2,
  three = 3,
  invalid = -1,
  first = 1,
  last = 3,
};

namespace refl = reflect_cpp26;
static_assert(refl::enum_contains<foo>("first"));
static_assert(refl::enum_contains<foo>(refl::ascii_case_insensitive, "LAST"));
static_assert(refl::enum_contains<foo>(2));
static_assert(refl::enum_contains<foo>(foo::three));
// Signedness-safe comparison: -1u does not match -1 despite identical bit representation.
static_assert(! refl::enum_contains<foo>(-1u));
// Narrowing-safe comparison.
static_assert(! refl::enum_contains<foo>(0x1'0000'0001));
```

#### Index of Enum Value

Defined in header `<reflect_cpp26/enum/enum_index.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_entry_order Order>
struct enum_index_by_t {
  // (1)
  template <enum_type E>
  static constexpr size_t operator()(E e);
};

template <enum_entry_order Order>
struct enum_index_opt_by_t {
  // (2)
  template <enum_type E>
  static constexpr auto operator()(E e) -> std::optional<size_t>;
};

struct enum_unique_index_t {
  // (3)
  template <enum_type E>
  static constexpr size_t operator()(E e);
};

struct enum_unique_index_opt_t {
  // (4)
  template <enum_type E>
  static constexpr auto operator()(E e) -> std::optional<size_t>;
};

template <enum_entry_order Order>
constexpr auto enum_index_by = enum_index_by_t<Order>{};

template <enum_entry_order Order>
constexpr auto enum_index_opt_by = enum_index_opt_by_t<Order>{};

constexpr auto enum_index = enum_index_by<enum_entry_order::original>;
constexpr auto enum_index_opt = enum_index_opt_by<enum_entry_order::original>;

constexpr auto enum_unique_index = enum_unique_index_t{};
constexpr auto enum_unique_index_opt = enum_unique_index_opt_t{};

}  // namespace reflect_cpp26
```

* (1) `enum_index_by<Order>(e)` or `enum_index(e)` (taking the original definition order) gets the index (0-based) of given enum value by specific order, or returns `npos` (which is -1 of type `size_t`) if such enum value is not defined. For multiple enum entries with duplicated underlying value, the first one is taken.
* (2) `enum_index_opt_by<Order>(e)` or `enum_index_opt(e)` (taking the original definition order) is similar to (1) yet returns `std::nullopt` on undefined enum values, which is useful for monadic operations;
* (3) `enum_unique_index(e)` gets the index (0-based) of given value after sorting all the enum entries by underlying value and removing all duplicated entries, or `npos` if such enum value is not defined;
* (4) `enum_unique_index_opt(e)` is similar to (3) yet returns `std::nullopt` on undefined enum values.

Example:
```cpp
enum class foo : int {  // | original | by_value | by_name | enum_unique_index |
  one = 1,              // |     0    |     1    |    4    |         1         |
  two = 2,              // |     1    |     3    |    6    |         2         |
  three = 3,            // |     2    |     4    |    5    |         3         |
  invalid = -1,         // |     3    |     0    |    2    |         0         |
  first = 1,            // |               (same as foo::one)                  |
  last = 3,             // |              (same as foo::three)                 |
  four = 4,             // |     6    |     6    |    1    |         4         |
};

namespace refl = reflect_cpp26;
using enum refl::enum_entry_order;

static_assert(refl::enum_index_by<by_value>(foo::three) == 4);
static_assert(refl::enum_index_by_opt<by_name>(foo::four) == 1);

constexpr auto foo_null = static_cast<foo>(0);
static_assert(refl::enum_index(foo_null) == refl::npos); // which is static_cast<size_t>(-1)
static_assert(refl::enum_index_opt(foo_null) == std::nullopt);

using handler_t = bool (*)(const char*);
extern handler_t handlers[5];

// Example on monadic operations
bool call_handler(foo key, const char* message) {
  return refl::enum_unique_index_opt(key)
    .transform([](size_t index) { return (*handlers[index])(message); })
    .value_or(false);
}
```

### Enum Flag Operations

#### Conversion from Enum Flag Value to String

Defined in header `<reflect_cpp26/enum/enum_flags_name.hpp>`.

```cpp
namespace reflect_cpp26 {

template <class Iter>
struct enum_flags_name_to_result {
  std::errc ec;
  Iter out;
};

struct enum_flags_name_t {
  // (1.1)
  template <enum_type E>
  static constexpr auto operator()(E e, char delim = '|') -> std::optional<std::string>;
  // (1.2)
  template <enum_type E>
  static constexpr auto operator()(E e, std::string_view delim) -> std::optional<std::string>;
};

struct enum_flags_name_to_t {
  // (2.1)
  template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel, enum_type E>
  static constexpr auto operator()(Iter iter, Sentinel sentinel, E e, char delim = '|')
      -> enum_flags_name_to_result<Iter>;
  // (2.2)
  template <std::output_iterator<char> Iter, std::sentinel_for<Iter> Sentinel, enum_type E>
  static constexpr auto operator()(Iter iter, Sentinel sentinel, E e, std::string_view delim)
      -> enum_flags_name_to_result<Iter>;
  // (2.3)
  template <enum_type E>
  static constexpr auto operator()(std::ostream& out, E e, char delim = '|') -> std::errc;
  // (2.4)
  template <enum_type E>
  static constexpr auto operator()(std::ostream& out, E e, std::string_view delim)
      -> std::errc;
};

constexpr auto enum_flags_name = enum_flags_name_t{};
constexpr auto enum_flags_name_to = enum_flags_name_to_t{};

}  // namespace reflect_cpp26
```

* (1.1, 1.2) `enum_flags_name(e, delim)` gets the string representation of given enum flags value, entries separated by given delimiter (which is `'|'` by default). If `e` can not be decomposed as disjunction of enum entries defined, `std::nullopt` is returned;
* (2.1, 2.2) `enum_flags_name_to(iter, sentinel, e, delim)` writes the string representation of given enum flags value to buffer `[iter, sentinel)`, entries separated by given delimiter (which is `'|'` by default). Return value contains two fields `{ec, out}` by the following rules:
  * If `e` can be decomposed as disjunction of enum entries defined, and given buffer `[iter, sentinel)` is enough to hold the result string, then `ec = std::errc{}` and `out` points to end position of the string written, i.e. `[iter, out)` contains the result string;
  * If `e` can be decomposed as disjunction of enum entries defined, but given buffer `[iter, sentinel)` can not hold the result string, then `ec = std::errc::value_too_large` and `out` (along with the partial string written to `[iter, out)`) is unspecified;
  * If `e` can not be decomposed as disjunction of enum entries defined, then `ec = std::errc::invalid_argument` and `out = iter`, i.e. nothing will be written.
* (2.3, 2.4) `enum_flags_name_to(out, e, delim)` writes the string representation of given enum flags value to given output stream, entries separated by given delimiter (which is `'|'` by default). Return value is an error code by the following rules:
  * If `e` can be decomposed as disjunction of enum entries defined, then `std::errc{}` is returned;
  * Otherwise, `std::errc::invalid_argument` is returned and nothing is written to the given output stream.

For overload (2.1) and (2.2), `std::unreachable_sentinel` can be used as the second argument for better performance (boundary checks eliminated) when it's ensured that the destination buffer is large enough to hold all possible values.

Example:
```cpp
enum class permissions {
  read = 1,
  write = 2,
  execute = 4,
};

namespace refl = reflect_cpp26;
// Possibly "write|read"
std::println("{}", *refl::enum_flags_name(static_cast<permissions>(3)));
// Possibly "execute, read"
std::println("{}", *refl::enum_flags_name(static_cast<permissions>(5), ", "));
// "Empty set" is still considered a valid value. Empty string is returned in this case.
static_assert(refl::enum_flags_name(static_cast<permissions>(0)) == "");
// Invalid value
static_assert(refl::enum_flags_name(static_cast<permissions>(8)) == std::nullopt);

char buffer[20] = {};
auto [ec1, out1] =
    refl::enum_flags_name_to(buffer, buffer + 16, static_cast<permissions>(6), " | ");
assert(ec1 == std::errc{});
assert(out1 == buffer + 15);
printf("%s\n", buffer); // Possibly "execute | write"

std::ranges::fill(buffer, '\0');
auto [ec2, _] = refl::enum_flags_name_to(buffer, buffer + 16, static_cast<permissions>(7));
assert(ec2 == std::errc::value_too_large);

std::ranges::fill(buffer, '\0');
auto [ec3, _] = refl::enum_flags_name_to(buffer, buffer + 16, static_cast<permissions>(-1));
assert(ec3 == std::errc::invalid_argument);

// Possibly writes "execute | write | read" to stdout
auto ec4 = refl::enum_flags_name_to(std::cout, static_cast<permissions>(7), ", ");
assert(ec4 == std::errc{});
// Nothing will be written to stdout
auto ec5 = refl::enum_flags_name_to(std::cout, static_cast<permissions>(15));
assert(ec5 == std::errc::invalid_argument);

bool write_file_permissions_config(std::string_view file, std::string_view perm_string);
// Example of monadic operations
bool set_file_permissions(std::string_view file, permissions perms) {
  return refl::enum_flags_name(perms)
    .transform([file](std::string perms_string) {
      return write_file_permissions_config(file, perms_string);
    })
    .value_or(false);
}
```

#### Conversion from Enum Flag String to Value

Defined in header `<reflect_cpp26/enum/enum_flags_cast.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E>
struct enum_flags_cast_t {
private:
  using ENoCV = std::remove_cv_t<E>;

public:
  // (1.1)
  static constexpr auto operator()(std::string_view str, char delim = '|') -> std::optional<ENoCV>;
  // (1.2)
  static constexpr auto operator()(std::string_view str, std::string_view delim)
      -> std::optional<ENoCV>;
  // (2.1)
  static constexpr auto operator()(ascii_case_insensitive_tag_t,
                                   std::string_view str,
                                   char delim = '|') -> std::optional<ENoCV>;
  // (2.2)
  static constexpr auto operator()(ascii_case_insensitive_tag_t,
                                   std::string_view str,
                                   std::string_view delim) -> std::optional<ENoCV>;
  // (3)
  static constexpr auto operator()(std::integral auto value) -> std::optional<ENoCV>;
};

template <enum_type E>
constexpr auto enum_flags_cast = enum_flags_cast_t<std::remove_cv_t<E>>{};

}  // namespace reflect_cpp26
```

* (1.1, 1.2) `enum_flags_cast<E>(str, delim)` Returns the enum flag value if input str can be decomposed as enum entry names split by given delimiter, or `std::nullopt` otherwise. Each segment in input `str` is trimmed before parsing such that leading and trailing ASCII space characters of the segment are removed;
* (2.1, 2.2) `enum_flags_cast<E>(ascii_case_insensitive, str, delim)` is similar to (1.1) while input segments are case-insensitive. Compilation error will be raised if duplicated names exist in enum definition;
* (3) `enum_flags_cast<E>(value)` casts value to enum type `E` if it can be decomposed as disjunction of enum entries defined in `E`, or returns `std::nullopt` otherwise. Signedness-safe and narrowing-safe comparison is performed.

Example:
```cpp
enum class permissions {
  read = 1,
  write = 2,
  execute = 4,
};

namespace refl = reflect_cpp26;
using namespace refl::enum_bitwise_operators;

static_assert(refl::enum_flags_cast<permissions>("read | write | execute") ==
              permissions::read | permissions::write | permissions::execute);
// Space characters around "read " and "\twrite " will be trimmed.
// The empty token after the last '\n' will be ignored.
static_assert(refl::enum_flags_cast<permissions>("read \n\twrite \n", '\n') ==
              permissions::read | permissions::write);

static_assert(refl::enum_flags_cast<permissions>(6) ==
              permissions::write | permissions::execute)
static_assert(refl::enum_flags_cast<permissions>(8) == std::nullopt);
```

#### Checking Existence of Enum Flag Value

Defined in header `<reflect_cpp26/enum/enum_flags_contains.hpp>`.

```cpp
namespace reflect_cpp26 {

template <enum_type E>
struct enum_flags_contains_t {
private:
  using ENoCV = std::remove_cv_t<E>;

public:
  // (1.1)
  static constexpr bool operator()(std::string_view str, char delim = '|');
  // (1.2)
  static constexpr bool operator()(std::string_view str, std::string_view delim);
  // (2.1)
  static constexpr bool operator()(ascii_case_insensitive_tag_t,
                                   std::string_view str,
                                   char delim = '|');
  // (2.2)
  static constexpr bool operator()(ascii_case_insensitive_tag_t,
                                   std::string_view str,
                                   std::string_view delim);
  // (3)
  static constexpr bool operator()(E flags);
  // (4)
  static constexpr bool operator()(std::integral auto flags);
};

template <enum_type E>
constexpr auto enum_flags_contains = enum_flags_contains_t<std::remove_cv_t<E>>{};

}  // namespace reflect_cpp26
```

Overloads in `enum_flags_contains<E>` are similar to those in `enum_flags_cast<E>` while only persence check is performed (i.e. whether the input string or value is a disjunction of enum entries defined). If you do not need the casted enum value, `enum_flags_contains<E>` has better performance.

### Enum Switch-Case Support

Defined in header `<reflect_cpp26/enum/enum_switch.hpp>`.

```cpp
namespace reflect_cpp26 {

// (1)
template <enum_type E, class Func>
constexpr auto enum_switch(Func&& func, E e) -> void;
// (2)
template <enum_type E, class FallbackFunc>
constexpr auto enum_switch(Func&& func, FallbackFunc&& fallback, E e) -> void;
// (3)
template <non_reference_type T, enum_type E, class Func>
constexpr auto enum_switch(Func&& func, E e) -> std::optional<T>;
// (4)
template <class T, enum_type E, class Func>
constexpr auto enum_switch(Func&& func, E e, T&& default_value) -> std::decay_t<T>;

}  // namespace reflect_cpp26
```

* (1) `enum_switch(func, e)` invokes `func(std::integral_constant<E, e>{}) -> void`, or no-op if `e` does not exist in enum type definition;
* (2) `enum_switch(func, e)` invokes `func(std::integral_constant<E, e>{}) -> void`, or invokes `fallback(e)` if `e` does not exist in enum type definition;
* (3) `enum_switch<T>(func, e)` invokes `func(std::integral_constant<E, e>{}) -> T'`, or returns `std::nullopt` if `e` does not exist in enum type definition. Return type `T'` of `func` will be converted to destination type `T`;
* (4) `enum_switch<T>(func, e, default_value)` invokes `func(std::integral_constant<E, e>{}) -> T'`, or returns `default_value` if `e` does not exist in enum type definition. Keep an eye on the type `T` of `default_value` as the return type `T'` of `func` will be converted to destination type `T`.

The function object should be invocable with single argument of type `std::integral_constant<E, e>` for each `e` defined in the enum type.

For GCC and Clang, always-inline attributes are used to ensure the same performance as hand-written switch-case sequence.

Example:
```cpp
enum class opcode : uint8_t {
  nop, mov,
  add_i32, sub_i32, mul_i32, div_i32, mod_i32,
  add_f32, sub_f32, mul_f32, div_f32, mod_f32,
  load_i32, load_f32, load_obj,
  store_i32, store_f32, store_obj,
  return_void,
};

enum class opcode_category : uint8_t {
  nullary,
  unary_arithmetic,
  binary_arithmetic,
  load_store,
};

template <opcode Op>
constexpr auto opcode_category_v = /* Compilation error for invalid Op */;
// Specializations for each opcode
template <>
constexpr auto opcode_category_v<opcode::add_i32> = opcode_category::binary_arithmetic;
// ...

namespace refl = reflect_cpp26;
void run_interpreter(const uint8_t* bytecode, context_t* ctx) {
  bool done = false;
  while (!done) {
    // decltype(opc) is in [std::integral_constant<opcode, e> for e in enum class opcode],
    // i.e. refl::enum_count_v<opcode> overloads will be instantiated.
    refl::enum_switch(
      [&done, &bytecode, ctx](auto opc) {
        if constexpr (opc == opcode::return_void) {
          done = true;
        } else if constexpr (opcode_category_v<opc> == opcode_category::binary_arithmetic) {
          bytecode = handle_binary_arithmetic(bytecode, ctx);
        } else if constexpr (opcode_category_v<opc> == opcode_category::load_store) {
          bytecode = handle_load_store(bytecode, ctx);
        } else { /* ... */ }
      },
      [](opcode invalid_opcode) {
        auto msg = std::format("Invalid opcode {}. Please verify the input bytecode",
                               std::to_underlying(opcode));
        FATAL_ERROR(std::move(msg));
      },
      static_cast<opcode>(*bytecode));
  }
}
```

### `std::format` and `fmt::format` Support for Enum Types

Defined in header `<reflect_cpp26/enum/enum_flags_contains.hpp>`.

```cpp
namespace reflect_cpp26 {

// std::format support
template <reflect_cpp26::enum_type E>
struct std::formatter<E>;

// fmt::format support (conditional)
#if __has_include(<fmt/format.h>)
template <reflect_cpp26::enum_type E>
struct fmt::formatter<E>;
#endif

}  // namespace reflect_cpp26
```

reflect_cpp26 provides formatting support for both `std::format` and `fmt::format`. Include this header to enable enum formatting.
* For enum non-flag values, no format specifier is needed;
* For enum flag values, the format specifier should start with character `'f'` or `'F'`, then optionally follows the delimiter string.

`enum_name` and `enum_flags_name` are used respectively. Undefined enum values (or enum flag values not decomposable to disjunction of enum entries defined) will be output by pattern `(%1)%2`, where `%1` is the type name and `%2` is the underlying value.

Example:
```cpp
// Include this header to enable enum formatting
#include <reflect_cpp26/enum/enum_format.hpp>

enum class permissions {
  read = 1,
  write = 2,
  execute = 4,
};

// As non-flag: outputs "write"
std::println("{}", permissions::write);
// As non-flag: outputs "(permissions)7"
std::println("{}", static_cast<permissions>(7));
// As non-flag: outputs "(permissions)0"
std::println("{}", static_cast<permissions>(0));
// As flag: possibly outputs "execute|write|read"
std::println("{:F}", static_cast<permissions>(7));
// As flag with custom delimiter " | ": possibly outputs "execute | write | read"
std::println("{:F | }", static_cast<permissions>(7));
// As flags: outputs "" as 0 is a valid flag value which represents empty set
std::println("{:F}", static_cast<permissions>(0));
// Outputs "(permissions)-1"
std::println("{:F}", static_cast<permissions>(-1));
```

### Operator Overloading for Enum Types

#### Enum Bitwise Operators Support

Defined in header `<reflect_cpp26/enum/enum_bitwise_operators.hpp>`.

* (1) The macro `REFLECT_CPP26_DEFINE_ENUM_BITWISE_BINARY_OPERATORS(E)` defines overloads of the following operators for enum type `E`:
  * `operator&` and `operator&=`;
  * `operator|` and `operator|=`;
  * `operator^` and `operator^=`.
* (2) The macro `REFLECT_CPP26_DEFINE_ENUM_BITWISE_OPERATORS(E)` defines overloads of `operator~` along with all the binary operators in (1) for enum type `E`;
* (3) `using namespace reflect_cpp26::enum_bitwise_operators` enables all the bitwise operators in (2) for enum types in the `using` scope.

#### Enum Comparison Operators Support

Defined in header `<reflect_cpp26/enum/enum_comparison_operators.hpp>`.

* (1) The macro `REFLECT_CPP26_DEFINE_ENUM_COMPARISON_OPERATORS(E)` defines overloads of `operator<=>` and `operator==` for enum type `E` by its underlying value;
* (2) `using namespace reflect_cpp26::enum_comparison_operators` enables `operator<=>` and `operator==` for enum types in the `using` scope.
