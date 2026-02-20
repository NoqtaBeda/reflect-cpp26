## Introduction

reflect_cpp26 contains a series of utility components for various usages.

## Core Components

### Structural Alternative Types

Defined in headers:
* `<reflect_cpp26/utils/meta_span.hpp>`;
* `<reflect_cpp26/utils/meta_string_view.hpp>`;
* `<reflect_cpp26/utils/meta_tuple.hpp>`.

These headers define structural alternative types to `std::span`, `std::basic_string_view`, and `std::tuple` respectively. A structural type is a type whose value can be used as a non-type template parameter (see [cppreference](https://en.cppreference.com/w/cpp/language/template_parameters) for details).

* `meta_span<T>` - A structural alternative to `std::span<const T>`, which stores two pointers `head` and `tail`. For default-constructed `meta_span` instances, both `head` and `tail` are null pointers denoting an empty range; Otherwise, these 2 pointers denote the range `[head, tail)`. It is designed for contiguous ranges with static constant storage only.
* `meta_basic_string_view<CharT>` - A structural alternative to `std::basic_string_view<CharT>`, which stores two pointers (`head` and `tail`): For default-constructed `meta_basic_string_view` instances, both `head` and `tail` are null pointers denoting an empty string; Otherwise, these 2 pointers denote the character range `[head, tail)`. It is ensured that the referenced string is always null-terminated (i.e., `tail == nullptr || *tail == '\0'` always holds). Type aliases are provided for all character types: `meta_string_view`, `meta_u8string_view`, etc.
* `meta_tuple<Args...>` - A structural alternative to `std::tuple<Args...>`, which uses `define_aggregate` to create an underlying aggregate type at compile time, making the tuple itself a structural type.

### Converting Identifier Naming

Defined in header `<reflect_cpp26/utils/identifier_naming.hpp>`.

```cpp
namespace reflect_cpp26 {

enum class identifier_naming_rule {
  snake_case,
  all_caps_snake_case,
  kebab_case,
  all_caps_kebab_case,
  lower_camel_case,
  upper_camel_case,
  lower_camel_snake_case,
  upper_camel_snake_case,
  http_header_case,
};

struct non_alpha_as_lower_tag_t {};
struct non_alpha_as_upper_tag_t {};

constexpr auto non_alpha_as_lower = non_alpha_as_lower_tag_t{};
constexpr auto non_alpha_as_upper = non_alpha_as_upper_tag_t{};

// (1) Identifier validation check
constexpr bool is_valid_identifier(std::string_view identifier);

// (2) Identifier conversion returning std::string
//     '*' refers to one of the 9 cases in enum class identifier_naming_rule: to_snake_case_t, etc.
struct to_*_case_t {
  // (2.1), equivalent to below
  static constexpr auto operator()(std::string_view identifier) -> std::string;
  // (2.1)
  static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)
      -> std::string;
  // (2.2)
  static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)
      -> std::string;
};

// (3) Identifier conversion returning std::optional<std::string>
//     '*' refers to one of the 9 cases in enum class identifier_naming_rule: to_snake_case_t, etc.
struct to_*_case_opt_t {
  // (3.1), equivalent to below
  static constexpr auto operator()(std::string_view identifier) -> std::optional<std::string>;
  // (3.1)
  static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)
      -> std::optional<std::string>;
  // (3.2)
  static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)
      -> std::optional<std::string>;
};

inline constexpr auto to_*_case = to_*_case_t{};
inline constexpr auto to_*_case_opt = to_*_case_opt_t{};

}  // namespace reflect_cpp26
```

(1) `is_valid_identifier(std::string_view identifier)` checks whether `identifier` is valid by the following rules:
* `identifier` should be non-empty;
* Each character in `identifier` should be one of:
  * Letters: `[A-Za-z]`;
  * Digits: `[0-9]`;
  * Dollar, underscore or hyphen: `[$_-]`.
* The first character is not a digit.

(2) The function `to_*_case` converts the input identifier to specified form. If input identifier is invalid (checked by `is_valid_identifier()`), then an empty string is returned.

(3) The function `to_*_case_opt` converts the input identifier to specified form. If input identifier is invalid (checked by `is_valid_identifier()`), then `std::nullopt` is returned.

All supported identifier cases:
1. `to_snake_case` and `to_snake_case_opt`: Converts to `snake_case`;
2. `to_all_caps_snake_case` and `to_all_caps_snake_case_opt`: Converts to `ALL_CAPS_SNAKE_CASE`;
3. `to_kebab_case` and `to_kebab_case_opt`: Converts to `kebab-case`;
4. `to_all_caps_kebab_case` and `to_all_caps_kebab_case_opt`: Converts to `ALL-CAPS-KEBAB-CASE`;
5. `to_lower_camel_case` and `to_lower_camel_case_opt`: Converts to `lowerCamelCase`;
6. `to_upper_camel_case` and `to_upper_camel_case_opt`: Converts to `UpperCamelCase`;
7. `to_lower_camel_snake_case` and `to_lower_camel_snake_case_opt`: Converts to `lower_Camel_Snake_Case`;
8. `to_upper_camel_snake_case` and `to_upper_camel_snake_case_opt`: Converts to `Upper_Camel_Snake_Case`;
9. `to_http_header_case` and `to_http_header_case_opt`: Converts to `Http-Header-Case`.

For each valid identifier, conversion is done by the following steps:
1. Split input identifier to word segments:
  1.1. Split input identifier by `'-'` and `'_'`;
  1.2. For each segment obtained from step 1.1, split again by letter case (details see example below).
2. Convert each segment by the specified destination case;
3. Concatenate the converted segments.

Example:
1. Let input identifier be `"exampleInput_ParseJSONDocument_TestCase1"`;
2. After step 1.1: `["exampleInput", "ParseJSONDocument", "TestCase1"]`;
3. After step 1.2:
  * `"exampleInput" -> ["example", "Input"]`;
  * `"ParseJSONDocument" -> ["Parse", "JSON", "Document"]`. Note that for consecutive $N+1$ upper-case letters, the first $N$ forms a single word, then the last one leads the next word;
  * `"TestCase1" -> ["Test", "Case1"]` for overload (2.1) and (3.1) which handles non-letter characters (i.e. digits or `'$'`) as lower-case letters, or
  * `"TestCase1" -> ["Test", "Case", "1"]` for overload (2.2) and (3.2) which handles non-letter characters as upper-case ones.
4. Assume we are converting to upper camel case with overload (2.1). For each word obtained in step 1.2, we convert the first character to upper-case and all following letters to lower-case. Finally, words are converted to `"Example", "Input", "Parse", "Json", "Document", "Test", "Case1"` (Note that `JSON` is converted to `Json`: The uniform conversion rule is applied to each word regardless of its input form);
5. After concatenation: `"ExampleInputParseJsonDocumentTestCase1"`.

### Testing Addressable Members

Defined in header `<reflect_cpp26/utils/addressable_member.hpp>`.

```cpp
namespace reflect_cpp26 {

consteval bool is_addressable_class_member(std::meta::info member);
consteval bool is_addressable_non_class_member(std::meta::info member);

}  // namespace reflect_cpp26
```

`is_addressable_class_member(member)` checks whether `member` is some *class* member which is addressable, i.e. `&[:member:]` is a valid constant expression. Addressable class member is one of the following:
* Non-static data member which is:
  * not template;
  * neither reference nor bit-field;
* Static data member (can be reference) which is not template;
* Non-static member function which is:
  * not template;
  * neither of constructor, destructor, or deleted;
* Static member function which is:
  * not template;
  * not deleted.

`is_addressable_non_class_member(member)` checks whether `member` is some *non-class* member which is addressable, i.e. `&[:member:]` is a valid constant expression. Addressable non-class member is one of the following:
* Variable (can be reference) which is not template;
* Function which is:
  * not template;
  * not deleted.

Detailed examples are shown in [unit test cases](../tests/utils/test_addressable_member.cpp).

## Auxiliary Components

### Alternatives to `<utility>` Components

Defined in header `<reflect_cpp26/utils/utility.hpp>`.

```cpp
namespace reflect_cpp26 {

// Integer comparison functors
struct cmp_equal_t { /* ... */ };
struct cmp_not_equal_t { /* ... */ };
struct cmp_less_t { /* ... */ };
struct cmp_greater_t { /* ... */ };
struct cmp_less_equal_t { /* ... */ };
struct cmp_greater_equal_t { /* ... */ };
struct cmp_three_way_t { /* ... */ };

constexpr auto cmp_equal = cmp_equal_t{};
constexpr auto cmp_not_equal = cmp_not_equal_t{};
constexpr auto cmp_less = cmp_less_t{};
constexpr auto cmp_greater = cmp_greater_t{};
constexpr auto cmp_less_equal = cmp_less_equal_t{};
constexpr auto cmp_greater_equal = cmp_greater_equal_t{};
constexpr auto cmp_three_way = cmp_three_way_t{};

// In-range check functor
template <std::integral R>
struct in_range_t {
  template <std::integral T>
  static constexpr bool operator()(T t);
};
template <class R>
constexpr auto in_range = in_range_t<R>{};

// Underlying type conversion
struct to_underlying_t {
  template <enum_type E>
  static constexpr auto operator()(E e);
};
constexpr auto to_underlying = to_underlying_t{};

// Zero/sign extension functors
template <std::unsigned_integral To>
struct zero_extend_t {
  template <std::integral From>
  static constexpr auto operator()(From from);
};
template <class To>
constexpr auto zero_extend = zero_extend_t<To>{};

template <std::signed_integral To>
struct sign_extend_t {
  template <std::integral From>
  static constexpr auto operator()(From from);
};
template <class To>
constexpr auto sign_extend = sign_extend_t<To>{};

}  // namespace reflect_cpp26
```

#### Integer Comparison

The integer comparison functors (`cmp_equal`, `cmp_not_equal`, `cmp_less`, `cmp_greater`, `cmp_less_equal`, `cmp_greater_equal`, `cmp_three_way`) are relaxed alternatives to `std::cmp_*` functions. Unlike the standard library versions which only accept integer types (excluding `bool` and character types), these functors accept all integral types including `bool`, `char`, `wchar_t`, `char8_t`, `char16_t`, `char32_t`, etc.

Example:
```cpp
// Standard library rejects these:
// std::cmp_less('a', 100);  // Error: char not allowed
// std::cmp_less(true, 1);   // Error: bool not allowed

// reflect_cpp26 accepts them:
reflect_cpp26::cmp_less('a', 100);  // OK: compares as int
reflect_cpp26::cmp_less(true, 1);   // OK: compares as int
```

#### In-Range Check

`in_range<R>(t)` checks whether the integer value `t` can be represented by type `R`. It is equivalent to `std::in_range<R>(t)` but uses the relaxed comparison functors internally.

Example:
```cpp
reflect_cpp26::in_range<uint8_t>(255);   // true
reflect_cpp26::in_range<uint8_t>(256);   // false
reflect_cpp26::in_range<uint8_t>(-1);    // false
reflect_cpp26::in_range<int8_t>(127);    // true
reflect_cpp26::in_range<int8_t>(128);    // false
```

#### Underlying Type Conversion

`to_underlying(e)` is a functor wrapper for `std::to_underlying(e)`, which converts an enum value to its underlying type.

Example:
```cpp
enum class color : uint8_t { red = 1, green = 2, blue = 3 };
auto r = reflect_cpp26::to_underlying(color::red);  // r is uint8_t{1}
```

#### Zero/Sign Extension

`zero_extend<To>(from)` and `sign_extend<To>(from)` convert an integer value to a larger integer type with zero-extension or sign-extension respectively:

* `zero_extend<To>(from)`: Converts `from` to unsigned type `To`, interpreting the source as unsigned.
* `sign_extend<To>(from)`: Converts `from` to signed type `To`, preserving the sign.

Example:
```cpp
int8_t x = -1;
// x = int8_t(0xFF). Zero-extends to uint64_t(0x00'00'00'FF)
reflect_cpp26::zero_extend<uint64_t>(x);  // uint64_t{255}
// x = int8_t(0xFF). Sign-extends to int64_t(0xFF'FF'FF'FF)
reflect_cpp26::sign_extend<int64_t>(x);   // int64_t{-1}

uint8_t y = 255;
// y = uint8_t(0xFF). Zero-extends to uint64_t(0x00'00'00'FF)
reflect_cpp26::zero_extend<uint64_t>(y);  // uint64_t{255}
// y = uint8_t(0xFF). Sign-extends to int64_t(0xFF'FF'FF'FF)
reflect_cpp26::sign_extend<int64_t>(y);   // int64_t{-1}
```

### Wrappers of `<meta>` Components

Defined in header `<reflect_cpp26/utils/meta_utility.hpp>`.

```cpp
namespace reflect_cpp26 {

// Access context helpers
consteval auto unprivileged_context();
consteval auto unchecked_context();

// Member query functions
consteval auto public_direct_members_of(std::meta::info a)
    -> std::vector<std::meta::info>;
consteval auto all_direct_members_of(std::meta::info a)
    -> std::vector<std::meta::info>;

consteval auto public_direct_bases_of(std::meta::info a)
    -> std::vector<std::meta::info>;
consteval auto all_direct_bases_of(std::meta::info a)
    -> std::vector<std::meta::info>;

consteval auto public_direct_static_data_members_of(std::meta::info a)
    -> std::vector<std::meta::info>;
consteval auto all_direct_static_data_members_of(std::meta::info a)
    -> std::vector<std::meta::info>;

consteval auto public_direct_nonstatic_data_members_of(std::meta::info a)
    -> std::vector<std::meta::info>;
consteval auto all_direct_nonstatic_data_members_of(std::meta::info a)
    -> std::vector<std::meta::info>;

// Member query values
template <class_or_union_type T>
constexpr auto public_direct_members_v = /* ... */;
template <class_or_union_type T>
constexpr auto all_direct_members_v = /* ... */;

template <class_or_union_type T>
constexpr auto public_direct_bases_v = /* ... */;
template <class_or_union_type T>
constexpr auto all_direct_bases_v = /* ... */;

template <class_or_union_type T>
constexpr auto public_direct_static_data_members_v = /* ... */;
template <class_or_union_type T>
constexpr auto all_direct_static_data_members_v = /* ... */;

template <class_or_union_type T>
constexpr auto public_direct_nonstatic_data_members_v = /* ... */;
template <class_or_union_type T>
constexpr auto all_direct_nonstatic_data_members_v = /* ... */

// Extraction helper
template <class T, std::same_as<std::meta::info>... Args>
consteval T extract(std::meta::info templ, Args... templ_params);

// Identifier helper
struct identifier_of_t {
  static consteval auto operator()(std::meta::info m, std::string_view alt = "")
      -> std::string_view;
};
constexpr auto identifier_of = identifier_of_t{};

}  // namespace reflect_cpp26
```

#### Access Contexts

These functions are wrappers of special access contexts:

* `unprivileged_context()`: Returns an access context with no special privileges. Equivalent to `std::meta::access_context::unprivileged()`.
* `unchecked_context()`: Returns an access context that bypasses all access checks. Equivalent to `std::meta::access_context::unchecked()`.

#### Member Query Functions

These functions query members of a type with different access levels:

* `public_direct_*_of(a)`: Returns all direct members accessible in global scope (equivalent to `std::meta::*_of(a, unprivileged_context())`).
* `all_direct_*_of(a)`: Returns all direct members regardless of accessibility (equivalent to `std::meta::*_of(a, unchecked_context())`).

The `*` can be one of: `members`, `bases`, `static_data_members`, `nonstatic_data_members`.

#### Member Query Values

These variable templates store the results of member queries as static arrays:

* `public_direct_*_v<T>`: Equivalent to `std::define_static_array(public_direct_*_of(^^T))`.
* `all_direct_*_v<T>`: Equivalent to `std::define_static_array(all_direct_*_of(^^T))`.

Example:
```cpp
namespace refl = reflect_cpp26;

struct MyClass { /*...*/ };

// Before: More typing required
constexpr auto members = std::define_static_array(
    std::meta::nonstatic_data_members_of(^^MyClass, std::meta::access_context::unprivileged()));
template for (constexpr auto m : members) { /*...*/ }
// After:
template for (constexpr auto m : refl::public_direct_nonstatic_data_members_v<MyClass>) { /*...*/ }
```

#### Extraction Helper

`extract<T>(templ, params...)` is a convenience function equivalent to `extract<T>(substitute(templ, {params...}))`. It substitutes template parameters and extracts the result in one call.

Example:
```cpp
namespace refl = reflect_cpp26;

constexpr auto T = substitute(^^std::vector, {^^int});
// Equivelent to extract<bool>(substitute(^^std::ranges::range, {T}))
// (where std::meta::extract and std::meta::substitute are found via ADL)
static_assert(refl::extract<bool>(^^std::ranges::range, T));
```

#### Identifier Helper

`identifier_of(m, alt)` returns the identifier of a reflection `m` if it has one, otherwise returns `alt`.

Example:
```cpp
namespace refl = reflect_cpp26;

int my_variable;
auto id1 = refl::identifier_of(^^my_variable);  // "my_variable"
auto id2 = refl::identifier_of(^^int, "unknown");  // "unknown" (type has no identifier)
// Equivalent to:
auto id3 = [] consteval -> std::string_view {
  // std::meta::identifier_of() raises error on anonymous entities
  if (has_identifier(^^int)) {
    return identifier_of(^^int);
  }
  return "unknown";
}();
```
