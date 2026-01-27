## Introduction

reflect_cpp26 contains a series of utility components for various usages.

## Components

### Converting Identifier Naming

Defined in header `<reflect_cpp26/utils/identifier_naming.hpp>`.

```cpp
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
  static constexpr auto operator()(non_alpha_as_lower_tag_t tag, std::string_view identifier)
      -> std::optional<std::string>;
  // (3.1)
  static constexpr auto operator()(non_alpha_as_upper_tag_t tag, std::string_view identifier)
      -> std::optional<std::string>;
  // (3.2)
  static constexpr auto operator()(std::string_view identifier) -> std::optional<std::string>;
};

inline constexpr auto to_*_case = to_*_case_t{};
inline constexpr auto to_*_case_opt = to_*_case_opt_t{};
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
consteval bool is_addressable_class_member(std::meta::info member);
consteval bool is_addressable_non_class_member(std::meta::info member);
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
