## Introduction

reflect_cpp26 contains a series of utility components for various usages.

## Components

### Structural Alternative Types

Defined in headers:

- `<reflect_cpp26/utils/meta_span.hpp>`;
- `<reflect_cpp26/utils/meta_string_view.hpp>`;
- `<reflect_cpp26/utils/meta_tuple.hpp>`.

These headers define structural alternative types to `std::span`, `std::basic_string_view`, and `std::tuple` respectively. A structural type is a type whose value can be used as a non-type template parameter (see [cppreference](https://en.cppreference.com/w/cpp/language/template_parameters) for details).

- `meta_span<T>` - A structural alternative to `std::span<const T>`, which stores two pointers `head` and `tail`. For default-constructed `meta_span` instances, both `head` and `tail` are null pointers denoting an empty range; Otherwise, these 2 pointers denote the range `[head, tail)`. It is designed for contiguous ranges with static constant storage only.
- `meta_basic_string_view<CharT>` - A structural alternative to `std::basic_string_view<CharT>`, which stores two pointers (`head` and `tail`): For default-constructed `meta_basic_string_view` instances, both `head` and `tail` are null pointers denoting an empty string; Otherwise, these 2 pointers denote the character range `[head, tail)`. It is ensured that the referenced string is always null-terminated (i.e., `tail == nullptr || *tail == '\0'` always holds). Type aliases are provided for all character types: `meta_string_view`, `meta_u8string_view`, etc.
- `meta_tuple<Args...>` - A structural alternative to `std::tuple<Args...>`, which uses `define_aggregate` to create an underlying aggregate type at compile time, making the tuple itself a structural type (as long as all of its elements are structural).

### Testing Addressable Members

Defined in header `<reflect_cpp26/utils/addressable_member.hpp>`.

```cpp
namespace reflect_cpp26 {

consteval bool is_addressable_class_member(std::meta::info member);
consteval bool is_addressable_non_class_member(std::meta::info member);

}  // namespace reflect_cpp26
```

`is_addressable_class_member(member)` checks whether `member` is some _class_ member which is addressable, i.e. `&[:member:]` is a valid constant expression. Addressable class member is one of the following:

- Non-static data member which is:
  - not template;
  - neither reference nor bit-field;
- Static data member (can be reference) which is not template;
- Non-static member function which is:
  - not template;
  - neither of constructor, destructor, or deleted;
- Static member function which is:
  - not template;
  - not deleted.

`is_addressable_non_class_member(member)` checks whether `member` is some _non-class_ member which is addressable, i.e. `&[:member:]` is a valid constant expression. Addressable non-class member is one of the following:

- Variable (can be reference) which is not template;
- Function which is:
  - not template;
  - not deleted.

Detailed examples are shown in [unit test cases](../tests/utils/test_addressable_member.cpp).

### UTF Encoding Conversion

Defined in header `<reflect_cpp26/utils/string_encoding.hpp>`.

This module provides constexpr-compatible UTF encoding conversion between UTF-8, UTF-16, and UTF-32. It also supports escaping by various modes (including but not limited to JSON escaping rule).

#### Synopsis

```cpp
namespace reflect_cpp26 {

enum class escaping_mode {
  none = 0,
  json = 1,
  display_char = 2,
  display_string = 3,
};

enum class encoding_status {
  done = 0,
  invalid_character = 1,
  buffer_run_out = 2,
};

template <class OutT, class InT>
struct encode_result_t {
  OutT* out_ptr;
  const InT* in_ptr;
  encoding_status status;
};

template <escaping_mode Mode>
struct utf8_to_utf16_base_t {
  template <class OutT, class InT>
  static constexpr auto operator()(OutT* dest, const OutT* dest_end,
                                   const InT* input, const InT* input_end)
      -> encode_result_t<OutT, InT>;
};
template <escaping_mode Mode>
struct utf8_to_utf32_base_t {
  template <class OutT, class InT>
  static constexpr auto operator()(OutT* dest, const OutT* dest_end,
                                   const InT* input, const InT* input_end)
      -> encode_result_t<OutT, InT>;
};
template <escaping_mode Mode>
struct utf16_to_utf8_base_t {
  template <class OutT, class InT>
  static constexpr auto operator()(OutT* dest, const OutT* dest_end,
                                   const InT* input, const InT* input_end)
      -> encode_result_t<OutT, InT>;
};
template <escaping_mode Mode>
struct utf16_to_utf32_base_t {
  template <class OutT, class InT>
  static constexpr auto operator()(OutT* dest, const OutT* dest_end,
                                   const InT* input, const InT* input_end)
      -> encode_result_t<OutT, InT>;
};
template <escaping_mode Mode>
struct utf32_to_utf8_base_t {
  template <class OutT, class InT>
  static constexpr auto operator()(OutT* dest, const OutT* dest_end,
                                   const InT* input, const InT* input_end)
      -> encode_result_t<OutT, InT>;
};
template <escaping_mode Mode>
struct utf32_to_utf16_base_t {
  template <class OutT, class InT>
  static constexpr auto operator()(OutT* dest, const OutT* dest_end,
                                   const InT* input, const InT* input_end)
      -> encode_result_t<OutT, InT>;
};
template <escaping_mode Mode>
struct utf_convert_base_t {
  template <class OutT, class InT>
  static constexpr auto operator()(OutT* dest, OutT* dest_end,
                                   const InT* input, const InT* input_end)
      -> encode_result_t<OutT, InT>;
};

using utf8_to_utf16_t = utf8_to_utf16_base_t<escaping_mode::none>;
using utf8_to_utf16_json_escaped_t = utf8_to_utf16_base_t<escaping_mode::json>;

using utf8_to_utf32_t = utf8_to_utf32_base_t<escaping_mode::none>;
using utf8_to_utf32_json_escaped_t = utf8_to_utf32_base_t<escaping_mode::json>;

using utf16_to_utf8_t = utf16_to_utf8_base_t<escaping_mode::none>;
using utf16_to_utf8_json_escaped_t = utf16_to_utf8_base_t<escaping_mode::json>;

using utf16_to_utf32_t = utf16_to_utf32_base_t<escaping_mode::none>;
using utf16_to_utf32_json_escaped_t = utf16_to_utf32_base_t<escaping_mode::json>;

using utf32_to_utf8_t = utf32_to_utf8_base_t<escaping_mode::none>;
using utf32_to_utf8_json_escaped_t = utf32_to_utf8_base_t<escaping_mode::json>;

using utf32_to_utf16_t = utf32_to_utf16_base_t<escaping_mode::none>;
using utf32_to_utf16_json_escaped_t = utf32_to_utf16_base_t<escaping_mode::json>;

using utf_convert_t = utf_convert_base_t<escaping_mode::none>;
using utf_convert_json_escaped_t = utf_convert_base_t<escaping_mode::json>;

constexpr auto utf8_to_utf16 = utf8_to_utf16_t{};
constexpr auto utf8_to_utf16_json_escaped = utf8_to_utf16_json_escaped_t{};
template <escaping_mode Mode>
constexpr auto utf8_to_utf16_by = utf8_to_utf16_base_t<Mode>{};

constexpr auto utf8_to_utf32 = utf8_to_utf32_t{};
constexpr auto utf8_to_utf32_json_escaped = utf8_to_utf32_json_escaped_t{};
template <escaping_mode Mode>
constexpr auto utf8_to_utf32_by = utf8_to_utf32_base_t<Mode>{};

constexpr auto utf16_to_utf8 = utf16_to_utf8_t{};
constexpr auto utf16_to_utf8_json_escaped = utf16_to_utf8_json_escaped_t{};
template <escaping_mode Mode>
constexpr auto utf16_to_utf8_by = utf16_to_utf8_base_t<Mode>{};

constexpr auto utf16_to_utf32 = utf16_to_utf32_t{};
constexpr auto utf16_to_utf32_json_escaped = utf16_to_utf32_json_escaped_t{};
template <escaping_mode Mode>
constexpr auto utf16_to_utf32_by = utf16_to_utf32_base_t<Mode>{};

constexpr auto utf32_to_utf8 = utf32_to_utf8_t{};
constexpr auto utf32_to_utf8_json_escaped = utf32_to_utf8_json_escaped_t{};
template <escaping_mode Mode>
constexpr auto utf32_to_utf8_by = utf32_to_utf8_base_t<Mode>{};

constexpr auto utf32_to_utf16 = utf32_to_utf16_t{};
constexpr auto utf32_to_utf16_json_escaped = utf32_to_utf16_json_escaped_t{};
template <escaping_mode Mode>
constexpr auto utf32_to_utf16_by = utf32_to_utf16_base_t<Mode>{};

constexpr auto utf_convert = utf_convert_t{};
constexpr auto utf_convert_json_escaped = utf_convert_json_escaped_t{};
template <escaping_mode Mode>
constexpr auto utf_convert_by = utf_convert_base_t<Mode>{};

}  // namespace reflect_cpp26
```

#### Description

This submodule provides range-based transcoding between UTF-8, UTF-16, and UTF-32. The public API is organized around dedicated converters such as `utf8_to_utf16` and the size-based dispatcher `utf_convert`.

Each converter reads from an explicit input range `[input, input_end)` and writes to an explicit destination range `[dest, dest_end)`. Conversion stops when:

- the whole input range has been consumed successfully;
- an invalid input sequence is encountered;
- the destination buffer has insufficient remaining space.

Besides the default non-escaping variants, the header also provides `_json_escaped` function objects and `*_by<Mode>` variable templates for explicitly selecting an `escaping_mode`.

#### Result Semantics

`encode_result_t<OutT, InT>` contains:

- `out_ptr`: one-past-the-end of the output already written;
- `in_ptr`: next input position to process;
- `status`: operation status.

Its status semantics are:

- `encoding_status::done`: all input in `[input, input_end)` was consumed;
- `encoding_status::invalid_character`: `in_ptr` points at the first invalid code unit sequence;
- `encoding_status::buffer_run_out`: `in_ptr` points at the next still-unprocessed input position.

#### Character Type Support

The converter templates are constrained by `impl::non_bool_integral_of_size`, defined as:

```cpp
// Accepts integral type (except bool) whose size is one of SizeCandidates...
template <class T, size_t... SizeCandidates>
concept non_bool_integral_of_size =
    non_bool_integral<T> && ((sizeof(T) == SizeCandidates) || ...);
```

The accepted types are not limited to the usual character types. Any integral type (excluding `bool`) is accepted as long as its size matches the required UTF code unit width:

- UTF-8 code units: any non-`bool` integral type of size 1 (`char8_t`, `char`, `signed char`, `unsigned char`, `int8_t`, `uint8_t`);
- UTF-16 code units: any non-`bool` integral type of size 2 (`char16_t`, `int16_t`, `uint16_t`);
- UTF-32 code units: any non-`bool` integral type of size 4 (`char32_t`, `int32_t`, `uint32_t`, etc.).

The dedicated converters further restrict source and destination size bytes by encoding direction:

- `utf8_to_utf16`: output type size 2, input type size 1
- `utf8_to_utf32`: output type size 4, input type size 1
- `utf16_to_utf8`: output type size 1, input type size 2
- `utf16_to_utf32`: output type size 4, input type size 2
- `utf32_to_utf8`: output type size 1, input type size 4
- `utf32_to_utf16`: output type size 2, input type size 4
- `utf_convert`: dispatches to one of above by `sizeof(OutT)` and `sizeof(InT)`

#### Escaping Modes

For transcoding, `escaping_mode::json` escapes `"`, `\\`, `/`, `\b`, `\f`, `\n`, `\r`, `\t`, and other ASCII control characters in range `[0x0, 0x1F]` as the standard pattern `\u00XX`. Details of all `escaping_mode` candidates are shown below in section "UTF Encoding/Decoding Helpers - Writing Escaped Characters".

The transcoding path is constexpr-friendly and is used internally by `<reflect_cpp26/utils/string_builder.hpp>`, especially in `append_utf_string()` and `append_utf_string_json_escaped()`.

### UTF Encoding/Decoding Helpers

This section documents the other public API components in `<reflect_cpp26/utils/string_encoding.hpp>`, including code point validation, single-code-point encode/decode, escaping helpers, and invalid-sequence consumers.

#### Synopsis

```cpp
namespace reflect_cpp26 {

enum class escaping_mode {
  none = 0,
  json = 1,
  display_char = 2,
  display_string = 3,
};

enum class escaping_status {
  done = 0,
  no_escape = 1,
  error = 2,
};

enum class encoding_status {
  done = 0,
  invalid_character = 1,
  buffer_run_out = 2,
};

constexpr auto invalid_code_point = static_cast<char32_t>(-1);
constexpr auto replacement_code_point = static_cast<char32_t>(0xFFFD);

constexpr bool is_valid_code_point(char32_t code_point);

template <escaping_mode Mode>
using write_escaped_character_t = /* implementation-defined */;
template <escaping_mode Mode>
struct write_escaped_character_unsafe_t;

using write_escaped_character_for_json_t = write_escaped_character_t<escaping_mode::json>;
using write_escaped_character_for_char_t = write_escaped_character_t<escaping_mode::display_char>;
using write_escaped_character_for_string_t = write_escaped_character_t<escaping_mode::display_string>;

using write_escaped_character_for_json_unsafe_t =
    write_escaped_character_unsafe_t<escaping_mode::json>;
using write_escaped_character_for_char_unsafe_t =
    write_escaped_character_unsafe_t<escaping_mode::display_char>;
using write_escaped_character_for_string_unsafe_t =
    write_escaped_character_unsafe_t<escaping_mode::display_string>;

template <escaping_mode Mode>
constexpr auto write_escaped_character = write_escaped_character_t<Mode>{};
template <escaping_mode Mode>
constexpr auto write_escaped_character_unsafe = write_escaped_character_unsafe_t<Mode>{};

constexpr auto write_escaped_character_for_json = write_escaped_character_for_json_t{};
constexpr auto write_escaped_character_for_char = write_escaped_character_for_char_t{};
constexpr auto write_escaped_character_for_string = write_escaped_character_for_string_t{};

constexpr auto write_escaped_character_for_json_unsafe =
    write_escaped_character_for_json_unsafe_t{};
constexpr auto write_escaped_character_for_char_unsafe =
    write_escaped_character_for_char_unsafe_t{};
constexpr auto write_escaped_character_for_string_unsafe =
    write_escaped_character_for_string_unsafe_t{};

struct decode_code_point_t {
  template <class CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end)
      -> std::pair<char32_t, const CharT*>;
};

struct decode_code_point_from_utf8_t {
  template <class CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end)
      -> std::pair<char32_t, const CharT*>;
};

struct decode_code_point_from_utf16_t {
  template <class CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end)
      -> std::pair<char32_t, const CharT*>;
};

constexpr auto decode_code_point = decode_code_point_t{};
constexpr auto decode_code_point_from_utf8 = decode_code_point_from_utf8_t{};
constexpr auto decode_code_point_from_utf16 = decode_code_point_from_utf16_t{};

struct encode_code_point_t {
  template <class CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> std::pair<CharT*, encoding_status>;
};

struct encode_code_point_unsafe_t {
  template <class CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> CharT*;
  template <class CharT>
  static constexpr auto operator()(CharT* dest, char32_t code_point) -> CharT*;
};

struct encode_code_point_to_utf8_t {
  template <class CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> std::pair<CharT*, encoding_status>;
};

struct encode_code_point_to_utf8_unsafe_t {
  template <class CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> CharT*;
  template <class CharT>
  static constexpr auto operator()(CharT* dest, char32_t code_point) -> CharT*;
};

struct encode_code_point_to_utf16_t {
  template <class CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> std::pair<CharT*, encoding_status>;
};

struct encode_code_point_to_utf16_unsafe_t {
  template <class CharT>
  static constexpr auto operator()(CharT* dest, const CharT* dest_end, char32_t code_point)
      -> CharT*;
  template <class CharT>
  static constexpr auto operator()(CharT* dest, char32_t code_point) -> CharT*;
};

constexpr auto encode_code_point = encode_code_point_t{};
constexpr auto encode_code_point_unsafe = encode_code_point_unsafe_t{};
constexpr auto encode_code_point_to_utf8 = encode_code_point_to_utf8_t{};
constexpr auto encode_code_point_to_utf8_unsafe = encode_code_point_to_utf8_unsafe_t{};
constexpr auto encode_code_point_to_utf16 = encode_code_point_to_utf16_t{};
constexpr auto encode_code_point_to_utf16_unsafe = encode_code_point_to_utf16_unsafe_t{};

struct consume_utf_invalid_sequence_t {
  template <class CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT*;
};

struct consume_utf8_invalid_sequence_t {
  template <class CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT*;
};

struct consume_utf16_invalid_sequence_t {
  template <class CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT*;
};

struct consume_utf32_invalid_sequence_t {
  template <class CharT>
  static constexpr auto operator()(const CharT* input, const CharT* input_end) -> const CharT*;
};

constexpr auto consume_utf8_invalid_sequence = consume_utf8_invalid_sequence_t{};
constexpr auto consume_utf16_invalid_sequence = consume_utf16_invalid_sequence_t{};
constexpr auto consume_utf32_invalid_sequence = consume_utf32_invalid_sequence_t{};
constexpr auto consume_utf_invalid_sequence = consume_utf_invalid_sequence_t{};

}  // namespace reflect_cpp26
```

#### Constants

These constants are shared by the decoding, encoding, and recovery helpers.

- `invalid_code_point` is the sentinel returned by the decoders on failure.
- `replacement_code_point` is the Unicode replacement character U+FFFD (which is '�'), typically used by higher-level recovery code after `consume_utf*_invalid_sequence`.

#### Code Point Validation

This helper checks whether a `char32_t` value is a valid Unicode scalar value.

`is_valid_code_point(code_point)` accepts exactly `U+0000..U+D7FF` and `U+E000..U+10FFFF`. Surrogates and values above `U+10FFFF` are invalid.

#### Decoding Functions

These helpers decode one code point from the input range `[input, input_end)`.

- `decode_code_point_from_utf8` decodes one UTF-8 scalar value from a 1-byte code unit sequence (`char8_t`, `char`, `uint8_t`, etc.).
- `decode_code_point_from_utf16` decodes one UTF-16 scalar value from a 2-byte code unit sequence (`char16_t`, `uint16_t`, etc.).
- `decode_code_point` dispatches by `sizeof(CharT)`:
  - `1` byte: UTF-8
  - `2` bytes: UTF-16
  - `4` bytes: UTF-32 validation + direct return

All decoders returns `{code_point, next}` whose type is `std::pair<char32_t, const CharT*>`.

- The first element `code_point` is the decoded result code point, or `invalid_code_point` on failure.
- The second element `next` is an input pointer:
  - on success, it points to the first unconsumed code unit after the decoded character;
  - on failure, it is exactly the original `input` pointer passed to the function.

So `{code_point, next}` means "one scalar value was decoded and the input cursor advanced to `next`", while `{invalid_code_point, next = input}` means "decoding failed and no input was consumed".

#### Encoding Single Code Points

These helpers encode one Unicode code point into the destination range `[dest, dest_end)`.

- `encode_code_point_to_utf8` / `encode_code_point_to_utf16` validate both `code_point` and output capacity, then return `{next_dest, status}` whose type is `std::pair<CharT*, encoding_status>`.
- `encode_code_point` dispatches by `sizeof(CharT)` and also supports trivial UTF-32 output (after validation).
- `*_unsafe` variants skip validation and return only `next_dest` (in this case `status` is always `encoding_status::done`). They **assume** a valid code point and enough buffer space.

For the safe variants, the returned pair has the following meaning:

- the first element `next_dest` is the first output position after the written code units on success, or `dest` unchanged on failure;
- the second element `status` is the corresponding `encoding_status`.

So `{next_dest, status}` means "writing stopped at `next_dest` and produced status `status`".

The possible status values are:

- `encoding_status::done`: the code point was written successfully;
- `encoding_status::invalid_character`: the input code point is not a valid Unicode scalar value;
- `encoding_status::buffer_run_out`: the destination range is too small.

#### Writing Escaped Characters

These helpers try to write an escaped form of a single character value `c` into the destination range `[dest, dest_end)`.

`write_escaped_character*` returns a `std::pair<escaping_status, CharT*>`.

- Safe variants take `(dest, dest_end, c)` and check capacity.
- Unsafe variants additionally support `(dest, c)` and assume enough space.

For the returned pair:

- the first element is the escape result status;
- the second element is the first output position after the characters written; below this pointer is denoted as `next_dest`.

So `{status, next_dest}` means "the escape attempt finished with status `status`, and the output cursor is now `next_dest`".

`escaping_status` is one of the following:

- `done`: an escaped form was written;
- `no_escape`: `c` does not need escaping in the selected mode;
- `error`: the destination buffer is too small.

Escaping behavior:

| Mode                            | Behavior                                                                                                           |
| ------------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| `escaping_mode::json`           | Escapes `"`, `\\`, `/`, `\b`, `\f`, `\n`, `\r`, `\t`, and other ASCII control characters as `\u00XX`.              |
| `escaping_mode::display_char`   | Escapes `\\`, `\0`, `\a`, `\b`, `\f`, `\n`, `\r`, `\t`, `\v`, and other non-printable ASCII bytes as `\xHH`.       |
| `escaping_mode::display_string` | Escapes `"`,, `\0`, `\\`, `\a`, `\b`, `\f`, `\n`, `\r`, `\t`, `\v`, and other non-printable ASCII bytes as `\xHH`. |

#### Invalid Sequence Consumers

These helpers are used for recovery after a decoding failure. They inspect the input range `[input, input_end)` and consume the maximal invalid prefix starting at `input`.

`consume_utf*_invalid_sequence` consumes the maximal continuous invalid prefix starting at `input` and returns the first position where normal decoding may resume.

- `consume_utf8_invalid_sequence`: consumes invalid UTF-8 leading / continuation patterns, overlong forms, truncated sequences, surrogates encoded in UTF-8, and values above `U+10FFFF`.
- `consume_utf16_invalid_sequence`: consumes orphaned UTF-16 surrogates until a valid BMP code unit or a valid surrogate pair is reached.
- `consume_utf32_invalid_sequence`: consumes consecutive invalid UTF-32 code points.
- `consume_utf_invalid_sequence`: dispatches by `sizeof(CharT)`.

These helpers are intended for replacement-based recovery, where the whole consumed invalid slice is typically replaced by a single `replacement_code_point` (common practice according to Unicode 11 standard).

### String Builder

Defined in header `<reflect_cpp26/utils/string_builder.hpp>`.

```cpp
namespace reflect_cpp26 {

template <class CharT, class Allocator = std::allocator<CharT>>
class basic_string_builder {
public:
  // Construction / Destruction
  constexpr basic_string_builder();
  explicit constexpr basic_string_builder(Allocator alloc);
  explicit constexpr basic_string_builder(size_t initial_size);
  explicit constexpr basic_string_builder(size_t initial_size, Allocator alloc);

  // No copy sementics
  basic_string_builder(const basic_string_builder&) = delete;
  auto operator=(const basic_string_builder&) -> basic_string_builder& = delete;

  // Trivial move semantics
  constexpr basic_string_builder(basic_string_builder&&) = default;
  constexpr auto operator=(basic_string_builder&&) -> basic_string_builder& = default;

  constexpr ~basic_string_builder();

  // Querying
  constexpr size_t size() const;
  constexpr auto str() const -> std::basic_string<CharT>;
  constexpr auto strview() const& -> std::basic_string_view<CharT>;

  // Buffer management
  constexpr auto reserve_at_least(size_t n) -> basic_string_builder&;

  // -------- Append single char --------
  constexpr auto append_char(CharT c) -> basic_string_builder&;
  constexpr auto append_char(CharT c, size_t count) -> basic_string_builder&;
  constexpr auto append_char_unsafe(CharT c) -> basic_string_builder&;
  constexpr auto append_char_unsafe(CharT c, size_t count) -> basic_string_builder&;

  template <escaping_mode Mode>
  constexpr auto append_char_by(CharT c) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_char_by_unsafe(CharT c) -> basic_string_builder&;

  constexpr auto append_char_json_escaped(CharT c) -> basic_string_builder&;
  constexpr auto append_char_json_escaped_unsafe(CharT c) -> basic_string_builder&;

  // -------- Append single code point --------
  constexpr auto append_utf_code_point(char32_t code_point) -> basic_string_builder&;
  constexpr auto append_utf_code_point_unsafe(char32_t code_point) -> basic_string_builder&;

  template <escaping_mode Mode>
  constexpr auto append_utf_code_point_by(char32_t code_point) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_utf_code_point_by_unsafe(char32_t code_point) -> basic_string_builder&;

  constexpr auto append_utf_code_point_json_escaped(char32_t code_point) -> basic_string_builder&;
  constexpr auto append_utf_code_point_json_escaped_unsafe(char32_t code_point) -> basic_string_builder&;

  // -------- Append string (no UTF conversion) --------
  constexpr auto append_string(const CharT* str, const CharT* str_end) -> basic_string_builder&;
  constexpr auto append_string(const CharT* str) -> basic_string_builder&;
  constexpr auto append_string(std::basic_string_view<CharT> str) -> basic_string_builder&;
  constexpr auto append_string_unsafe(const CharT* str, const CharT* str_end) -> basic_string_builder&;
  constexpr auto append_string_unsafe(const CharT* str) -> basic_string_builder&;
  constexpr auto append_string_unsafe(std::basic_string_view<CharT> str) -> basic_string_builder&;

  template <escaping_mode Mode>
  constexpr auto append_string_by(const CharT* str, const CharT* str_end) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_string_by(const CharT* str) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_string_by(std::basic_string_view<CharT> str) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_string_by_unsafe(const CharT* str, const CharT* str_end) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_string_by_unsafe(const CharT* str) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_string_by_unsafe(std::basic_string_view<CharT> str) -> basic_string_builder&;

  constexpr auto append_string_json_escaped(const CharT* str, const CharT* str_end) -> basic_string_builder&;
  constexpr auto append_string_json_escaped(const CharT* str) -> basic_string_builder&;
  constexpr auto append_string_json_escaped(std::basic_string_view<CharT> str) -> basic_string_builder&;
  constexpr auto append_string_json_escaped_unsafe(const CharT* str, const CharT* str_end) -> basic_string_builder&;
  constexpr auto append_string_json_escaped_unsafe(const CharT* str) -> basic_string_builder&;
  constexpr auto append_string_json_escaped_unsafe(std::basic_string_view<CharT> str) -> basic_string_builder&;

  // -------- Append C-style string (no UTF conversion) --------
  constexpr auto append_c_string_unsafe(const char* str, const char* str_end) -> basic_string_builder&;
  constexpr auto append_c_string_unsafe(const char* str) -> basic_string_builder&;
  constexpr auto append_c_string_unsafe(std::string_view str) -> basic_string_builder&;

  constexpr auto append_c_string(const char* str, const char* str_end) -> basic_string_builder&;
  constexpr auto append_c_string(const char* str) -> basic_string_builder&;
  constexpr auto append_c_string(std::string_view str) -> basic_string_builder&;

  template <escaping_mode Mode>
  constexpr auto append_c_string_by_unsafe(const char* str, const char* str_end) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_c_string_by_unsafe(const char* str) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_c_string_by_unsafe(std::string_view str) -> basic_string_builder&;

  template <escaping_mode Mode>
  constexpr auto append_c_string_by(const char* str, const char* str_end) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_c_string_by(const char* str) -> basic_string_builder&;
  template <escaping_mode Mode>
  constexpr auto append_c_string_by(std::string_view str) -> basic_string_builder&;

  constexpr auto append_c_string_json_escaped_unsafe(const char* str, const char* str_end) -> basic_string_builder&;
  constexpr auto append_c_string_json_escaped_unsafe(const char* str) -> basic_string_builder&;
  constexpr auto append_c_string_json_escaped_unsafe(std::string_view str) -> basic_string_builder&;

  constexpr auto append_c_string_json_escaped(const char* str, const char* str_end) -> basic_string_builder&;
  constexpr auto append_c_string_json_escaped(const char* str) -> basic_string_builder&;
  constexpr auto append_c_string_json_escaped(std::string_view str) -> basic_string_builder&;

  // -------- Append string (with UTF conversion) --------
  template <char_type OtherCharT>
  constexpr auto append_utf_string(const OtherCharT* str, const OtherCharT* str_end) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string(const OtherCharT* str) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string(std::basic_string_view<OtherCharT> str) -> basic_string_builder&;
  template <char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string(const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_unsafe(const OtherCharT* str, const OtherCharT* str_end) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_unsafe(const OtherCharT* str) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_unsafe(std::basic_string_view<OtherCharT> str) -> basic_string_builder&;
  template <char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string_unsafe(const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder&;

  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by(const OtherCharT* str, const OtherCharT* str_end) -> basic_string_builder&;
  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by(const OtherCharT* str) -> basic_string_builder&;
  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by(std::basic_string_view<OtherCharT> str) -> basic_string_builder&;
  template <escaping_mode Mode, char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string_by(const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder&;
  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by_unsafe(const OtherCharT* str, const OtherCharT* str_end) -> basic_string_builder&;
  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by_unsafe(const OtherCharT* str) -> basic_string_builder&;
  template <escaping_mode Mode, char_type OtherCharT>
  constexpr auto append_utf_string_by_unsafe(std::basic_string_view<OtherCharT> str) -> basic_string_builder&;
  template <escaping_mode Mode, char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string_by_unsafe(const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder&;

  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped(const OtherCharT* str, const OtherCharT* str_end) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped(const OtherCharT* str) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped(std::basic_string_view<OtherCharT> str) -> basic_string_builder&;
  template <char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string_json_escaped(const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped_unsafe(const OtherCharT* str, const OtherCharT* str_end) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped_unsafe(const OtherCharT* str) -> basic_string_builder&;
  template <char_type OtherCharT>
  constexpr auto append_utf_string_json_escaped_unsafe(std::basic_string_view<OtherCharT> str) -> basic_string_builder&;
  template <char_type OtherCharT, class Traits, class OtherAllocator>
  constexpr auto append_utf_string_json_escaped_unsafe(const std::basic_string<OtherCharT, Traits, OtherAllocator>& str) -> basic_string_builder&;

  // -------- Append arithmetic types --------
  constexpr auto append_bool(bool value) -> basic_string_builder&;

  template <non_bool_integral IntegerT>
    requires(sizeof(IntegerT) <= sizeof(int64_t))
  constexpr auto append_integer(IntegerT value, int base = 10) -> basic_string_builder&;

  constexpr auto append_floating_point(float value) -> basic_string_builder&;
  constexpr auto append_floating_point(double value) -> basic_string_builder&;
  constexpr auto append_floating_point(long double value) -> basic_string_builder&;

  template <std::floating_point FloatT>
  constexpr auto append_floating_point(FloatT value, std::chars_format fmt) -> basic_string_builder&;

  template <std::floating_point FloatT>
  constexpr auto append_floating_point(FloatT value, std::chars_format fmt, int precision) -> basic_string_builder&;

  // -------- Output iterators --------
  class safe_output_iterator;
  constexpr auto out() -> safe_output_iterator;

  class unsafe_output_iterator;
  constexpr auto out_unsafe() -> unsafe_output_iterator;
};

using string_builder = basic_string_builder<char>;
using u8string_builder = basic_string_builder<char8_t>;
using u16string_builder = basic_string_builder<char16_t>;
using u32string_builder = basic_string_builder<char32_t>;
using wstring_builder = basic_string_builder<wchar_t>;

using pmr_string_builder = basic_string_builder<char, std::pmr::polymorphic_allocator<char>>;
using pmr_u8string_builder = basic_string_builder<char8_t, std::pmr::polymorphic_allocator<char8_t>>;
using pmr_u16string_builder = basic_string_builder<char16_t, std::pmr::polymorphic_allocator<char16_t>>;
using pmr_u32string_builder = basic_string_builder<char32_t, std::pmr::polymorphic_allocator<char32_t>>;
using pmr_wstring_builder = basic_string_builder<wchar_t, std::pmr::polymorphic_allocator<wchar_t>>;

}  // namespace reflect_cpp26
```

The `basic_string_builder` class provides a constexpr-compatible string builder with automatic buffer management and support for UTF encoding conversion.

#### Construction / Destruction

- Default constructor creates a builder with no initial buffer.
- Constructor taking `Allocator` creates a builder with a custom allocator.
- Constructor taking `initial_size` allocates an initial buffer of that many characters.
- Copy construction and copy assignment are **deleted**.
- Move construction and move assignment are **defaulted**.
- Destructor deallocates the internal buffer (if any) using the allocator.

#### Type Aliases

| Type Alias          | Character Type | Allocator                         |
| ------------------- | -------------- | --------------------------------- |
| `string_builder`    | `char`         | `std::allocator<char>`            |
| `u8string_builder`  | `char8_t`      | `std::allocator<char8_t>`         |
| `u16string_builder` | `char16_t`     | `std::allocator<char16_t>`        |
| `u32string_builder` | `char32_t`     | `std::allocator<char32_t>`        |
| `wstring_builder`   | `wchar_t`      | `std::allocator<wchar_t>`         |
| `pmr_*_builder`     | Same as above  | `std::pmr::polymorphic_allocator` |

Note: `wstring_builder` does not support UTF conversion because `wchar_t` size varies by platform.

#### Core Methods

- `size()`: Returns the current number of characters in the builder.
- `str()`: Returns a `std::basic_string<CharT>` with a copy of the current content.
- `strview()`: Returns a `std::basic_string_view<CharT>` referencing the current content.
- `reserve_at_least(n)`: Ensures the buffer has capacity for at least `n` additional characters. Grows exponentially if needed.

#### Safe vs Unsafe Variants

Most `append_*` methods come in two forms:

| Variant    | Naming            | Behavior                                                                                                                                             |
| ---------- | ----------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Safe**   | `append_*`        | Calls `reserve_at_least` before writing. **Preferred in most cases.**                                                                                |
| **Unsafe** | `append_*_unsafe` | Assumes the caller has already ensured sufficient buffer space. Skips `reserve_at_least` for performance when buffer capacity is known to be enough. |

Suggested buffer reservation size for each `_unsafe` function is shown in the following table (Constant 6 refers to the max escaped size `\u00xx`. `s, t` refers to UTF-`s` -> UTF-`t` conversion):

| `_unsafe` Method                                                                                 | Worst-Case Buffer Usage                         |
| ------------------------------------------------------------------------------------------------ | ----------------------------------------------- |
| `append_char_unsafe(c)`                                                                          | 1                                               |
| `append_char_unsafe(c, count)`                                                                   | `count`                                         |
| `append_char_by_unsafe<Mode>(c)` <br> `append_char_json_escaped_unsafe(c)`                       | 6 (escaped: max 6 chars)                        |
| `append_utf_code_point_unsafe(cp)`                                                               | `32/t` (UTF-8: max 4 bytes)                     |
| `append_utf_code_point_by_unsafe<Mode>(cp)` <br> `append_utf_code_point_json_escaped_unsafe(cp)` | 6 (escaped: max 6 chars)                        |
| `append_string_unsafe(s, s+n)` <br> `append_c_string_unsafe(s, s+n)`                             | `n`                                             |
| `append_string_by_unsafe<Mode>(s, s+n)` <br> `append_c_string_by_unsafe<Mode>(s, s+n)`           | `6 × n` (escaped: worst-case 6×)                |
| `append_string_json_escaped_unsafe(s, s+n)` <br> `append_c_string_json_escaped_unsafe(s, s+n)`   | `6 × n` (escaped: worst-case 6×)                |
| `append_utf_string_unsafe(s, s+n)`                                                               | `max(1, s/t) × n` (same-type or UTF conversion) |
| `append_utf_string_by_unsafe<Mode>(s, s+n)` <br> `append_utf_string_json_escaped_unsafe(s, s+n)` | `6 × n` (escaped: worst-case 6×)                |

The safe variant always delegates to the unsafe variant after reserving enough space.

Typical usage of `_unsafe` variants is chained operation sequence whose total buffer usage is known as prior knowledge. For example:

```cpp
reflect_cpp26::u8string_builder builder;

// Known: 6 chars for "Name: " + at most 256 chars for name + 12 chars for ", status: OK"
builder.reserve_at_least(6 + 256 + 12);

// Now all _unsafe variants can be chained without intermediate reserve checks
builder
  .append_string_unsafe(u8"Name: ")
  .append_utf_string_unsafe(name)          // u16string, with at most 128 UTF-16 units
  .append_string_unsafe(u8", status: OK");

auto result = builder.str();
```

#### Escaping-mode Template Methods

Methods suffixed `_by` accept an `escaping_mode` template parameter, enabling arbitrary escaping modes (see [Escaping Modes](#escaping-modes)):

- `append_char_by<Mode>`: Append a single character using the specified escaping mode.
- `append_string_by<Mode>`: Append a same-type string using the specified escaping mode.
- `append_utf_string_by<Mode>`: Append a (potentially different-type) string using the specified escaping mode.
- `append_utf_code_point_by<Mode>`: Append a code point using the specified escaping mode.

Each `_by` method also has a `_by_unsafe` counterpart.

Convenience aliases for common modes:

- `append_char_json_escaped` ≈ `append_char_by<escaping_mode::json>`
- `append_string_json_escaped` ≈ `append_string_by<escaping_mode::json>`
- `append_utf_string_json_escaped` ≈ `append_utf_string_by<escaping_mode::json>`
- `append_utf_code_point_json_escaped` ≈ `append_utf_code_point_by<escaping_mode::json>`

And corresponding `_unsafe` versions.

#### Appending Characters

- `append_char(c)`: Appends a single character.
- `append_char(c, count)`: Appends `count` copies of character `c`.
- `append_char_unsafe(c)` / `append_char_unsafe(c, count)`: Unsafe variants (no reserve).
- `append_char_json_escaped(c)`: Appends a single character with JSON escaping.
- `append_char_json_escaped_unsafe(c)`: Unsafe variant.

#### Appending Code Points

- `append_utf_code_point(code_point)`: Appends a Unicode code point, encoding it appropriately for the builder's character type. Invalid code points are replaced with U+FFFD.
- `append_utf_code_point_unsafe(code_point)`: Unsafe variant.
- `append_utf_code_point_json_escaped(code_point)`: Appends a code point with JSON escaping.
- `append_utf_code_point_json_escaped_unsafe(code_point)`: Unsafe variant.

#### Appending Strings

The builder supports three types of string append operations:

1. **`append_string`**: Appends raw characters without UTF conversion or escaping.
   - Safe overloads: pointer with end, null-terminated pointer, `std::basic_string_view`
   - Unsafe variants: `append_string_unsafe` with the same overloads

2. **`append_string_json_escaped`**: Appends raw characters with JSON escaping.
   - Safe overloads: pointer with end, null-terminated pointer, `std::basic_string_view`
   - Unsafe variants: `append_string_json_escaped_unsafe` with the same overloads

3. **`append_utf_string`**: Appends strings with automatic UTF encoding conversion. Invalid UTF sequences are replaced with U+FFFD.
   - Safe overloads: pointer with end, null-terminated pointer, `std::basic_string_view`, `std::basic_string`
   - Unsafe variants: `append_utf_string_unsafe` with the same overloads
   - Supports all UTF conversions (UTF-8↔UTF-16, UTF-8↔UTF-32, UTF-16↔UTF-32, and same-type)

4. **`append_utf_string_json_escaped`**: Combines UTF conversion and JSON escaping.
   - Safe overloads: pointer with end, null-terminated pointer, `std::basic_string_view`, `std::basic_string`
   - Unsafe variants: `append_utf_string_json_escaped_unsafe` with the same overloads

#### Appending C-style Strings

The `append_c_string` series accepts `const char*` or `std::string_view` input regardless of the builder's character type. Characters are converted to `CharT` via `static_cast`—no UTF encoding conversion is performed.

1. **`append_c_string`**: Copies `char` characters with `static_cast<CharT>`.
   - Safe overloads: pointer with end, null-terminated pointer, `std::string_view`
   - Unsafe variants: `append_c_string_unsafe` with the same overloads

2. **`append_c_string_json_escaped`**: Same as `append_c_string` with JSON escaping.
   - Safe overloads: pointer with end, null-terminated pointer, `std::string_view`
   - Unsafe variants: `append_c_string_json_escaped_unsafe` with the same overloads

Null-terminated string overloads (single `const char*` parameter) use `std::ranges::find` to locate `'\0'` then delegate to the `(str, str_end)` overload.

#### Output Iterators

The builder provides two output iterator classes, following the same idiom as `std::back_insert_iterator` (`operator*` and `operator++` are no-ops returning `*this`; `operator=(CharT c)` does the actual work):

**`safe_output_iterator`** — Calls `append_char(c)` on each assignment (calling `reserve_at_least` internally each time):

```cpp
auto iter = builder.out();  // safe_output_iterator
*iter++ = 'A';              // calls builder.append_char('A')
```

**`unsafe_output_iterator`** — Writes directly to the internal buffer. Automatic buffer expansion is **not** performed and you need to ensure the buffer has enough space via manual `reserve_at_least()` call.

```cpp
auto iter = builder.out_unsafe();  // unsafe_output_iterator
*iter++ = 'A';
```

These iterators satisfy `std::output_iterator<CharT>` and can be used with standard algorithms or functions that write through output iterators (e.g. `std::ranges::copy`).

Factory methods:

- `out()`: Returns a `safe_output_iterator` bound to this builder.
- `out_unsafe()`: Returns an `unsafe_output_iterator` bound to this builder.

#### Appending Numbers

- `append_bool(value)`: Appends "true" or "false".
- `append_integer(value, base)`: Appends an integer in the specified base (default: decimal). Supports bases 2-36.
- `append_floating_point(value)`: Appends a floating-point number.
- `append_floating_point(value, fmt)`: Appends with specified format (`std::chars_format::general`, `scientific`, `fixed`, `hex`).
- `append_floating_point(value, fmt, precision)`: Appends with specified format and precision.

#### Buffer Management

The builder manages a buffer allocated via the provided allocator:

- Initial size can be specified in the constructor
- When the buffer is full, it grows exponentially (doubles capacity)
- `reserve_at_least(n)` can be called explicitly to pre-allocate space
- All operations are constexpr-compatible
- Buffer memory is deallocated in the destructor using the allocator

#### Chaining

All `append_*` methods return `basic_string_builder&`, enabling method chaining:

```cpp
auto result = rfl::u8string_builder{}
    .append_string(u8"Name: ")
    .append_string(u8"John")
    .append_char(' ')
    .append_integer(30)
    .append_string(u8", Score: ")
    .append_floating_point(98.5)
    .append_bool(true)
    .strview();
```

#### Error Handling for UTF Conversion

When `append_utf_string` encounters invalid UTF sequences, it:

1. Replaces the invalid sequence with the Unicode replacement character (U+FFFD, which is '�')
2. Continues processing the remaining input

```cpp
rfl::u16string_builder builder;
const char8_t invalid[] = u8"hello\xFF\xFEworld";
builder.append_utf_string(invalid, invalid + sizeof(invalid) - 1);
// Result: u"hello�world"
```

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

- `identifier` should be non-empty;
- Each character in `identifier` should be one of:
  - Letters: `[A-Za-z]`;
  - Digits: `[0-9]`;
  - Dollar, underscore or hyphen: `[$_-]`.
- The first character is not a digit.

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

- `"exampleInput" -> ["example", "Input"]`;
- `"ParseJSONDocument" -> ["Parse", "JSON", "Document"]`. Note that for consecutive $N+1$ upper-case letters, the first $N$ forms a single word, then the last one leads the next word;
- `"TestCase1" -> ["Test", "Case1"]` for overload (2.1) and (3.1) which handles non-letter characters (i.e. digits or `'$'`) as lower-case letters, or
- `"TestCase1" -> ["Test", "Case", "1"]` for overload (2.2) and (3.2) which handles non-letter characters as upper-case ones.

4. Assume we are converting to upper camel case with overload (2.1). For each word obtained in step 1.2, we convert the first character to upper-case and all following letters to lower-case. Finally, words are converted to `"Example", "Input", "Parse", "Json", "Document", "Test", "Case1"` (Note that `JSON` is converted to `Json`: The uniform conversion rule is applied to each word regardless of its input form);
5. After concatenation: `"ExampleInputParseJsonDocumentTestCase1"`.

### Locale-Independent Character Categorization & Conversion

Defined in header `<reflect_cpp26/utils/ctype.hpp>`.

```cpp
namespace reflect_cpp26 {

// ASCII character/string check
struct is_ascii_char_t {
  static constexpr bool operator()(/* non-bool integral */ auto c);
};
struct is_ascii_string_t {
  static constexpr bool operator()(const /* string-like */ auto& str);
};
constexpr auto is_ascii_char = is_ascii_char_t{};
constexpr auto is_ascii_string = is_ascii_string_t{};

// ASCII character classification (constexpr, locale-independent)
struct ascii_isalnum_t { /* ... */ };
struct ascii_isalpha_t { /* ... */ };
struct ascii_islower_t { /* ... */ };
struct ascii_isupper_t { /* ... */ };
struct ascii_isdigit_t { /* ... */ };
struct ascii_isxdigit_t { /* ... */ };
struct ascii_isblank_t { /* ... */ };
struct ascii_iscntrl_t { /* ... */ };
struct ascii_isgraph_t { /* ... */ };
struct ascii_isspace_t { /* ... */ };
struct ascii_isprint_t { /* ... */ };
struct ascii_ispunct_t { /* ... */ };

constexpr auto ascii_isalnum = ascii_isalnum_t{};
constexpr auto ascii_isalpha = ascii_isalpha_t{};
constexpr auto ascii_islower = ascii_islower_t{};
constexpr auto ascii_isupper = ascii_isupper_t{};
constexpr auto ascii_isdigit = ascii_isdigit_t{};
constexpr auto ascii_isxdigit = ascii_isxdigit_t{};
constexpr auto ascii_isblank = ascii_isblank_t{};
constexpr auto ascii_iscntrl = ascii_iscntrl_t{};
constexpr auto ascii_isgraph = ascii_isgraph_t{};
constexpr auto ascii_isspace = ascii_isspace_t{};
constexpr auto ascii_isprint = ascii_isprint_t{};
constexpr auto ascii_ispunct = ascii_ispunct_t{};

// ASCII case conversion
struct ascii_tolower_t {
  static constexpr auto operator()(/* char-type */ auto c);
  static constexpr auto operator()(const /* string-like */ auto& str)
      -> std::basic_string</* CharT */>;
};
struct ascii_toupper_t {
  static constexpr auto operator()(/* char-type */ auto c);
  static constexpr auto operator()(const /* string-like */ auto& str)
      -> std::basic_string</* CharT */>;
};
constexpr auto ascii_tolower = ascii_tolower_t{};
constexpr auto ascii_toupper = ascii_toupper_t{};

// ASCII whitespace trimming
struct ascii_trim_t {
  static constexpr auto operator()(const /* string-like */ auto& str)
      -> std::basic_string_view</* CharT */>;
};
constexpr auto ascii_trim = ascii_trim_t{};

}  // namespace reflect_cpp26
```

These functors provide constexpr, locale-independent alternatives to the character classification and conversion functions in `<cctype>` and `<cwctype>`.

#### ASCII Character/String Check

- `is_ascii_char(c)`: Returns `true` if `c` is in range `[0, 127]`.
- `is_ascii_string(str)`: Returns `true` if all characters in `str` are in range `[0, 127]`.

#### ASCII Character Classification

These predicates match the behavior of their `std::` counterparts for ASCII characters (0-127). For non-ASCII values, they always return `false`:

| Functor          | Description                                  |
| ---------------- | -------------------------------------------- |
| `ascii_isalnum`  | Alphanumeric character (`[0-9A-Za-z]`)       |
| `ascii_isalpha`  | Alphabetic character (`[A-Za-z]`)            |
| `ascii_islower`  | Lowercase letter (`[a-z]`)                   |
| `ascii_isupper`  | Uppercase letter (`[A-Z]`)                   |
| `ascii_isdigit`  | Decimal digit (`[0-9]`)                      |
| `ascii_isxdigit` | Hexadecimal digit (`[0-9A-Fa-f]`)            |
| `ascii_isblank`  | Blank character (space or tab)               |
| `ascii_iscntrl`  | Control character (`[0-31, 127]`)            |
| `ascii_isgraph`  | Graphical character (printable except space) |
| `ascii_isspace`  | Whitespace character (`[ \f\n\r\t\v]`)       |
| `ascii_isprint`  | Printable character (`[32-126]`)             |
| `ascii_ispunct`  | Punctuation character                        |

#### ASCII Case Conversion

- `ascii_tolower(c)`: Converts uppercase ASCII letter to lowercase, otherwise returns `c` unchanged.
- `ascii_toupper(c)`: Converts lowercase ASCII letter to uppercase, otherwise returns `c` unchanged.
- `ascii_tolower(str)`: Returns a new string with all ASCII letters converted to lowercase.
- `ascii_toupper(str)`: Returns a new string with all ASCII letters converted to uppercase.

For non-ASCII characters, the functions above always return the character unchanged.

#### ASCII Whitespace Trimming

`ascii_trim(str)` removes leading and trailing ASCII whitespace characters (`' '`, `'\f'`, `'\n'`, `'\r'`, `'\t'`, `'\v'`). Returns a `std::basic_string_view`.

Example:

```cpp
namespace refl = reflect_cpp26;

// Character classification
refl::ascii_isalpha('A');     // true
refl::ascii_isalpha('1');     // false
refl::ascii_islower('a');     // true
refl::ascii_isdigit('5');     // true

// Case conversion
refl::ascii_tolower('A');     // 'a'
refl::ascii_toupper('z');     // 'Z'
refl::ascii_tolower("HeLLo"); // "hello"
refl::ascii_toupper("HeLLo"); // "HELLO"

// Trimming
refl::ascii_trim("  hello  "); // "hello"
refl::ascii_trim("\t\nhi\r\n"); // "hi"
```

### String Hash Calculation

Defined in header `<reflect_cpp26/utils/string_hash.hpp>`.

```cpp
namespace reflect_cpp26 {

// BKDR hash functors (case-sensitive)
struct bkdr_hash_t {
  template </* char_type */ class CharT>
  static constexpr auto operator()(const CharT* begin, const CharT* end) -> size_t;
  template </* string_like */ class StringT>
  static constexpr auto operator()(const StringT& str) -> size_t;
};

// ASCII case-insensitive, locale-independent
struct ascii_ci_bkdr_hash_t {
  template </* char_type */ class CharT>
  static constexpr auto operator()(const CharT* begin, const CharT* end) -> size_t;
  template </* string_like */ class StringT>
  static constexpr auto operator()(const StringT& str) -> size_t;
};

constexpr auto bkdr_hash32 = bkdr_hash32_t{};
constexpr auto ascii_ci_bkdr_hash64 = ascii_ci_bkdr_hash64_t{};

}  // namespace reflect_cpp26
```

These functors implement the modified BKDR hash algorithm for string hashing:

```
result = 0
for each c in the input string:
    result = result * 131 + c
return result
```

- `bkdr_hash`: Case-sensitive BKDR hash. Returns hash value of `size_t`. Works with all character types.
- `ascii_ci_bkdr_hash` : ASCII case-insensitive BKDR hash which uses `ascii_tolower` to convert characters to lowercase before hashing. Works with all character types.

Each functor provides three overloads:

1. Pointer range `[begin, end)`
2. Any string-like type (including C-style null-terminated strings. See [String-like Types](#string-like-types))

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

The `in_range_t<bool>` specialization checks whether the value is exactly `0` or `1`, since `bool` can only represent these two values.

Example:

```cpp
reflect_cpp26::in_range<uint8_t>(255);   // true
reflect_cpp26::in_range<uint8_t>(256);   // false
reflect_cpp26::in_range<uint8_t>(-1);    // false
reflect_cpp26::in_range<int8_t>(127);    // true
reflect_cpp26::in_range<int8_t>(128);    // false
reflect_cpp26::in_range<bool>(0);        // true
reflect_cpp26::in_range<bool>(1);        // true
reflect_cpp26::in_range<bool>(2);        // false
reflect_cpp26::in_range<bool>(-1);       // false
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

- `unprivileged_context()`: Returns an access context with no special privileges. Equivalent to `std::meta::access_context::unprivileged()`.
- `unchecked_context()`: Returns an access context that bypasses all access checks. Equivalent to `std::meta::access_context::unchecked()`.

#### Member Query Functions

These functions query members of a type with different access levels:

- `public_direct_*_of(a)`: Returns all direct members accessible in global scope (equivalent to `std::meta::*_of(a, unprivileged_context())`).
- `all_direct_*_of(a)`: Returns all direct members regardless of accessibility (equivalent to `std::meta::*_of(a, unchecked_context())`).

The `*` can be one of: `members`, `bases`, `static_data_members`, `nonstatic_data_members`.

#### Member Query Values

These variable templates store the results of member queries as static arrays:

- `public_direct_*_v<T>`: Equivalent to `std::define_static_array(public_direct_*_of(^^T))`.
- `all_direct_*_v<T>`: Equivalent to `std::define_static_array(all_direct_*_of(^^T))`.

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
