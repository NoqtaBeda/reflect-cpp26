## Introduction

reflect_cpp26 provides functionality for common type operations, including:

- Compile-time conversion to static storage
- Accessing non-static data members by member index

## Core Components

### To Static Storage

Defined in header `<reflect_cpp26/type_operations/to_structural.hpp>`.

```cpp
namespace reflect_cpp26 {

struct to_structural_t {
  template <class T>
  static consteval auto operator()(const T& value) /* -> to_structural_result_t<T> */;

  template <class T>
  static consteval auto operator()(std::initializer_list<T> values) /* -> to_structural_result_t<T> */;
};

constexpr auto to_structural = to_structural_t{};

template <class T>
using to_structural_result_t = decltype(to_structural(std::declval<T>()));

}  // namespace reflect_cpp26
```

The function object `to_structural` converts input to a compile-time static constant with [structural](https://en.cppreference.com/w/cpp/language/template_parameters) storage type. For ranges, strings and tuples, reflect_cpp26 provides [structural alternative types](./utils.md) which are applied in the conversion result.

The conversion rules are as follows:

1. **Ranges**: If `T` satisfies `std::ranges::input_range` with value type `V`:
   - If `V` is a character type, the result is `meta_basic_string_view<V>`;
   - Otherwise, the result is `meta_span<to_structural_result_t<V>>`.
2. **Tuple-like types**: If `T` satisfies `tuple_like`, the result is `meta_tuple<to_structural_result_t<Vs>...>` where `Vs...` are the element types of `T`.
3. **Character pointers**: If `T` is `const CharT*` where `CharT` is a character type, the result is `meta_basic_string_view<CharT>` (assumes the pointer points to a null-terminated string).
4. **Other types**: For any other type `T`, if `T` is a structural type, the result is `T` (identity conversion). Otherwise, a compile error occurs.

The type alias `to_structural_result_t<T>` obtains the result type of `to_structural` for a given input type `T`.

Example:

```cpp
namespace refl = reflect_cpp26;

// (1) Range (including initializer list) to meta_span
constexpr auto span_1 = refl::to_structural(std::vector<int>{1, 2, 3, 4, 5});
static_assert(std::is_same_v<std::remove_const_t<decltype(span_1)>, refl::meta_span<int>>);

constexpr auto span_2 = refl::to_structural({"Cat", "Dog", "Rabbit"});
static_assert(std::is_same_v<std::remove_const_t<decltype(span_2)>,
                             refl::meta_span<refl::meta_string_view>>);

// (2) Tuple-like to meta_tuple
constexpr auto meta_tup = refl::to_structural(
    std::tuple<int, double, std::u16string>{42, 3.14, u"Hello"});
static_assert(std::is_same_v<std::remove_const_t<decltype(meta_tup)>,
                             refl::meta_tuple<int, double, refl::meta_u16_string_view>>);

// (3) Const char pointer to meta_string_view
constexpr auto cstr = refl::to_structural("world");
static_assert(std::is_same_v<std::remove_const_t<decltype(cstr)>,
                             refl::meta_string_view>);

// (4) Identity
struct point_t {
  int x;
  int y;
};
constexpr auto val = refl::to_structural(point_t{.x = 12, .y = 34});
static_assert(std::is_same_v<decltype(val), const point_t>);
```

See [unit test](../tests/type_operations/test_to_structural.cpp) for more examples and details.

### Serialize to JSON

Defined in header `<reflect_cpp26/type_operations/serialize_to_json.hpp>`.

This module provides constexpr-compatible serialization of C++ types to JSON format.

#### Options

```cpp
namespace reflect_cpp26 {

struct serialize_options {
  bool char_to_string = false;         // Serialize char as string ("x") or integer (120)
  bool enum_to_string = false;         // Serialize enum as string ("red") or integer (0)
  bool halts_on_invalid_enum = false;  // Halt if enum has no name (requires enum_to_string)
  bool halts_on_non_finite_floating_point = false;  // Halt on NaN/Inf (else serializes as string)
};

}  // namespace reflect_cpp26
```

#### Public API

```cpp
namespace reflect_cpp26 {

// (1) Serialize to builder (compact, no indent)
template <serialize_options Options = {}, class CharT, class Allocator, serializable T>
constexpr bool serialize_to_json(basic_string_builder<CharT, Allocator>& dest, const T& value);

// (2) Serialize to builder (with indent)
template <serialize_options Options = {}, class CharT, class Allocator, serializable T>
constexpr bool serialize_to_json(basic_string_builder<CharT, Allocator>& dest,
                                const T& value,
                                int indent_size,
                                CharT indent_char = static_cast<CharT>(' '));

// (3) Serialize and return string (compact, no indent)
template <class CharT = char, serialize_options Options = {}, serializable T>
constexpr auto serialize_to_json(const T& value);
    // -> std::basic_string<CharT> or std::optional<std::basic_string<CharT>>, details see below

// (4) Serialize and return string (with indent)
template <class CharT = char, serialize_options Options = {}, serializable T>
constexpr auto serialize_to_json(const T& value,
                               int indent_size,
                               CharT indent_char = static_cast<CharT>(' '));
    // -> std::basic_string<CharT> or std::optional<std::basic_string<CharT>>, details see below

}  // namespace reflect_cpp26
```

**Return type rules:**

- **(1) & (2) Serialize to builder**: Returns `bool`
  - `true`: Serialization completed successfully
  - `false`: Serialization halted due to invalid input, including:
    - invalid enum with no name, or invalid enum flag value which can not be decomposed as disjunction of defined values (when `halts_on_invalid_enum = true`)
    - non-finite floating-point value (when `halts_on_non_finite_floating_point = true`)

- **(3) & (4) Serialize and return string**:
  - If `Options.never_halts()` returns `true`: returns `std::basic_string<CharT>` directly
  - Otherwise: returns `std::optional<std::basic_string<CharT>>`, where `std::nullopt` indicates the serialization halted due to invalid input.

#### Supported Types

The function serializes the following types according to the `serializable` concept:

| Type                                                        | JSON Output                                  | Notes                                                             |
| ----------------------------------------------------------- | -------------------------------------------- | ----------------------------------------------------------------- |
| `std::monostate`                                            | `null`                                       |                                                                   |
| Character types (`char`, `char8_t`, `char16_t`, `char32_t`) | `"x"` or `120`                               | Controlled by `char_to_string` option                             |
| Boolean types                                               | `true`, `false`                              |                                                                   |
| Integer types                                               | `42`, `-123`                                 |                                                                   |
| Floating-point types                                        | `3.14`, `"NaN"`, `"Infinity"`, `"-Infinity"` | See [Floating-Point Serialization](#floating-point-serialization) |
| Enumeration types                                           | `"red"` or `0`                               | Controlled by `enum_to_string` option                             |
| String-like types                                           | `"hello"`                                    | Escape special characters                                         |
| `std::map<K, V>` where `K` is string-like                   | `{"key": value, ...}`                        | JSON object                                                       |
| Other range types                                           | `[1, 2, 3]`                                  | JSON array                                                        |
| Tuple-like types                                            | `[elem1, elem2, ...]`                        | JSON array                                                        |
| `std::optional<T>`                                          | `42` or `null`                               | Value or null if empty                                            |
| `std::variant<Ts...>`                                       | `value`                                      | Serializes current alternative                                    |
| Flattenable class types                                     | `{"field1": value1, ...}`                    | JSON object                                                       |

**Note:** `std::set`, `std::multimap`, and `std::map<K, V>` where `K` is not string-like are serialized as arrays of `[key, value]` pairs.

#### Floating-Point Serialization

Floating-point values (`float`, `double`, `long double`) are serialized according to the `halts_on_non_finite_floating_point` option:

**When `halts_on_non_finite_floating_point = false` (default):**
| Value | JSON Output | Description |
|-------|-------------|-------------|
| Finite values (e.g., `3.14`, `-0.5`) | `3.14`, `-0.5` | Standard decimal notation |
| Positive infinity | `"Infinity"` | String (not valid JSON per RFC 8259) |
| Negative infinity | `"-Infinity"` | String (not valid JSON per RFC 8259) |
| Quiet NaN | `"NaN"` | String (not valid JSON per RFC 8259) |
| Signaling NaN | `"NaN"` | String (not valid JSON per RFC 8259) |

**When `halts_on_non_finite_floating_point = true`:**
| Value | Behavior |
|-------|----------|
| Finite values | Serialized normally |
| Positive infinity | Function returns `false` / `std::nullopt` |
| Negative infinity | Function returns `false` / `std::nullopt` |
| NaN | Function returns `false` / `std::nullopt` |

**Note:** RFC 8259 does not define numeric values for infinity or NaN. By default, this implementation serializes them as strings for round-trip preservation, but this produces non-standard JSON that may not be parseable by strict JSON parsers.

#### Enum Serialization

Enum types are serialized according to the `enum_to_string` and `halts_on_invalid_enum` options:

**When `enum_to_string = false` (default):**
| Type | Valid | Invalid (halts=false) | Invalid (halts=true) |
|------|-------|----------------------|----------------------|
| Regular enum | `0`, `1`, `2` | `255` (raw underlying value) | returns `false` / `std::nullopt` |
| Enum flag | `3`, `7` | `255` (raw underlying value) | returns `false` / `std::nullopt` |

When `halts_on_invalid_enum = true`, integer serialization checks `enum_contains<T>(value)` before proceeding. This guards against out-of-range values even when not converting to string.

**When `enum_to_string = true`:**
| Type | Valid | Invalid (halts=false) | Invalid (halts=true) |
|------|-------|----------------------|----------------------|
| Regular enum| `"red"` | `null` | returns `false` / `std::nullopt` |
| Enum flag | `"read\|write"` | `null` | returns `false` / `std::nullopt` |

For enum flags, validity means the value can be decomposed as a disjunction of defined flag entries (e.g., `read | write = 3`). Values that cannot be decomposed (e.g., `8` when only `1, 2, 4` are defined) are treated as invalid — producing `null` (JSON null literal) when `halts = false` or halting when `halts = true`.

For the empty flag set (`0`), it is always treated as valid and serializes to `""` (empty string).

Enumeration names containing non-ASCII characters (e.g., Chinese, Japanese, emoji) are fully supported. The builder automatically chooses between ASCII-only fast paths (which calls `append_c_string_unsafe()` internally) and UTF conversion paths (which calls `append_utf_string_unsafe` internally) based on compile-time analysis of the enum's identifier names.

#### Examples

```cpp
namespace refl = reflect_cpp26;

// Basic types
auto i = refl::serialize_to_json(42);           // "42"
auto f = refl::serialize_to_json(3.14);         // "3.14"
auto b = refl::serialize_to_json(true);          // "true"
auto s = refl::serialize_to_json(std::string{"hello"});  // "\"hello\""

// With options
auto c = refl::serialize_to_json<char, refl::serialize_options{.char_to_string = true}>('A');  // "\"A\""
auto e = refl::serialize_to_json<refl::serialize_options{.enum_to_string = true}>(color_t::red);  // "\"red\""

// With indent
struct person_t { std::string name; int age; };
person_t p{"Alice", 30};
auto json = refl::serialize_to_json(p, 2, ' ');
// {
//   "name": "Alice",
//   "age": 30
// }

// Nested structures
std::vector<std::map<std::string, int>> data = {{{"a", 1}, {"b", 2}}};
auto nested = refl::serialize_to_json(data);
// [{"a": 1, "b": 2}]

// Using builder
refl::u8string_builder builder;
refl::serialize_to_json(builder, p);
auto result = builder.str();
```

### Dump to JSON-style Format

Defined in header `<reflect_cpp26/type_operations/dump_to_json_like.hpp>`.

`dump_to_json_like` provides a debug-friendly serialization to a human-readable JSON-like format. Unlike `serialize_to_json`, which strictly follows RFC 8259, the dump format:

- Produces **unquoted** field names (shell-style)
- Serializes enums as unquoted values (e.g., `red` instead of `"red"`)
- Serializes characters with single quotes (e.g., `'A'`)
- Dumps pointers as hexadecimal addresses (e.g., `0x12345678` or `nullptr`)
- Dumps pointers-to-members with class scope (e.g., `&MyClass::field`)

This format is not valid JSON, but is easier to read during debugging, tracing, and logging.

#### Public API

```cpp
namespace reflect_cpp26 {

// (1) Dump to builder (compact, no indent)
template <class CharT, class Allocator, class T>
constexpr void dump_to_json_like(basic_string_builder<CharT, Allocator>& dest, const T& value);

// (2) Dump to builder (with indent)
template <class CharT, class Allocator, class T>
constexpr void dump_to_json_like(basic_string_builder<CharT, Allocator>& dest,
                                 const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' '));

// (3) Dump and return string (compact, no indent)
template <class CharT = char, class T>
constexpr auto dump_to_json_like(const T& value) -> std::basic_string<CharT>;

// (4) Dump and return string (with indent)
template <class CharT = char, class T>
constexpr auto dump_to_json_like(const T& value,
                                 int indent_size,
                                 CharT indent_char = static_cast<CharT>(' '))
    -> std::basic_string<CharT>;

}  // namespace reflect_cpp26
```

Unlike `serialize_to_json`, overload (3) and (4) always return `std::basic_string<CharT>` (string overloads) — it never halts on invalid input. Invalid values are also dumped as human-readable hint message.

#### Supported Types

| Type                                | Dump Output             | Notes                                |
| ----------------------------------- | ----------------------- | ------------------------------------ |
| `std::monostate`                    | `monostate`             |                                      |
| Boolean types                       | `true`, `false`         |                                      |
| Character types                     | `'A'`, `'\n'`           | Single-quoted, special chars escaped |
| Integer types                       | `42`, `-123`            |                                      |
| Floating-point types                | `3.14`                  |                                      |
| Enumeration types                   | `red`, `read\|write`    | Unquoted; flag composition           |
| Enum (invalid)                      | `(EnumType)255`         | Type name + underlying value         |
| String-like types                   | `"hello"`               | Double-quoted, special chars escaped |
| Pointers                            | `0x12345678`, `nullptr` | Hex address                          |
| Pointer-to-member                   | `&MyClass::field`       | With class scope                     |
| `std::map<K, V>` (K is string-like) | `{key: value, ...}`     |                                      |
| Other range types                   | `[1, 2, 3]`             |                                      |
| Tuple-like types                    | `[elem1, elem2, ...]`   |                                      |
| `std::optional<T>`                  | `42` or `nullopt`       | Value or `nullopt` if empty          |
| `std::variant<Ts...>`               | `value`                 | Current alternative                  |
| Flattenable class types             | `{field: value, ...}`   | Unquoted field names                 |

#### Key Differences from serialize_to_json

| Feature                             | `serialize_to_json`          | `dump_to_json_like`        |
| ----------------------------------- | ---------------------------- | -------------------------- |
| Field names                         | `"quoted"`                   | `unquoted`                 |
| Enum values                         | `"red"` or `0`               | `red`                      |
| Characters                          | `"A"` or `65`                | `'A'`                      |
| Pointers                            | not supported                | `0x12345678`               |
| Pointers-to-members                 | not supported                | `&Class::field`            |
| Invalid enum                        | `null` or halts              | `(EnumType)N`              |
| std::optional empty                 | `null`                       | `nullopt`                  |
| std::variant valueless_by_exception | `null`                       | `(valueless by exception)` |
| Options                             | `serialize_options`          | none                       |
| Return type                         | `bool` or `optional<string>` | `void` or `string`         |

#### Name Collision Handling

When a class member name collides with an inherited member name, the dump format disambiguates using the `ClassName::` prefix, same as `serialize_to_json`:

```cpp
struct A { int x; };
struct B { int x; };
struct C : A, B { int c; };

C obj{{1}, {2}, 3};
// Dump: {A::x:1,B::x:2,c:3}
```

#### Character Encoding

The dump format supports all character types. When input strings use a different encoding than the builder, UTF conversion is performed automatically:

```cpp
// UTF-8 input → UTF-16 output
auto result = refl::dump_to_json_like<char16_t>(std::u8string{u8"你好"});
// Result: u"\"你好\""
```

Non-ASCII field names (e.g., Chinese, Japanese, Korean, emoji) are fully supported with compile-time ASCII detection for optimal performance.
