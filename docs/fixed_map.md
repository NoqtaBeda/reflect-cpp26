## Introduction

reflect_cpp26 provides functionality to generate compile-time fixed map structures whose keys are of either integral, enum or string type, supporting auto selection along numerous data structures and manual fine-tuning to the underlying data structure with various configuration parameters.

Multi-maps are not supported. Compilation error will occur if duplicated keys exist in the input entries.

Fixed map serves as the base of multiple reflect_cpp26 components, including but not limited to:
* [Enum integer-to-string and string-to-integer conversion](./enum.md);
* [Lookup table](./lookup_table.md).

> Even in LLM era, reflect_cpp26 has its unique advantage that an efficient data structure can be applied automatically with your code kept clean. You don't have to worry that (1) you forget to instruct the agent to update all related components; (2) the agent generates flawed code for several items under illusion which is hard to be detected by human reviewing (especially when the number of items or related components is large); (3) the agent either fails to pick some good data structure, or generates a huge block of boilerplate code which harms the overall readability.

## Candidate Data Structures

### Integral-Key & Enum-Key

Let:
* $n$ be the number of entries;
* $k_{\text{min}}$ and $k_{\text{max}}$ be the minimum and maximum key of input entries;
* `K` and `V` be the key type and value type respectively.

The underlying data structure of integral-key fixed map can be one of the following, selected by reflect_cpp26 automatically according to the input data:

1. **Fully-dense, O(1)**: Applied only if $k_{\text{max}} - k_{\text{min}} = n - 1$ , i.e. *all* the keys are continuous. The underlying data structure is an array of values sorted by their corresponding keys, plus `min_key` and `max_key` fields denoting the key range;
2. **Dense, O(1)**: Applied only if $k_{\text{max}} - k_{\text{min}} + 1 \le n/\alpha$ where $\alpha$ is the minimum load factor (default value is 0.5). The underlying data structure is an array of length $k_{\text{max}} - k_{\text{min}} + 1$ whose items are `(value, is_valid)` pairs. If input entries contain the key $k_{\text{min}} + i$, then the array item with index $i$ is `(value of min_key+i, is_valid=true)`; Otherwise, the array item is `(V{}, is_valid=false)` indicating that current position is a *hole*. Additionally, `min_key` and `max_key` are stored to denote the key range;
3. **Sparse, O(log n)**: The underlying data structure is an array of length $n$ whose items are simply `(key, value)` pairs sorted by `key`. If $n$ is greater or equal to some threshold (default value is 8), then binary search is applied for each fixed map access; Otherwise, linear search is applied.
4. **General, O(log n)**: The underlying data structure is decomposed to 3 parts: the middle dense (or fully-dense) part whose key falls in range $[k_{\text{min}}', k_{\text{max}}']$, the left sparse (or empty) part whose key $< k_{\text{min}}'$, and the right sparse (or empty) part whose key $> k_{\text{max}}'$.

For keys of enum type, the data structure is simply a wrapper of some integral-key fixed map.

Layout of each data structure:

```
1. Fully-dense
kv-pairs: {3: "A3", 4: "B4", 5: "C5", 6: "D6", 7: "E7"}
min_key = 3
max_key = 7
array:
+----------------------------------+
| "A3" | "B4" | "C5" | "D6" | "E7" |
+----------------------------------+
pseudo-code:
if key < min_key or key > max_key:
    return {value: None, is_valid: false}
return {value: array[key - min_key], is_valid = true}

2. Dense
kv-pairs: {3: "A3", 5: "B5", 6: "C6", 8: "D8"}
min_key = 3
max_key = 10
array:
+-------------------------------------------------------------------------------------------------+
|  value="A3"   |    value=""    |  value="B5"   |  value="C6"   |    value=""    |  value="D8"   |
| is_valid=true | is_valid=false | is_valid=true | is_valid=true | is_valid=false | is_valid=true |
+-------------------------------------------------------------------------------------------------+
pseudo-code:
if key < min_key or key > max_key or not array[key - min_key].is_valid:
    return {value: None, is_valid: false}
return {value: array[key - min_key].value, is_valid: true}

3. Sparse
kv-pairs: {1: "A1", 3: "B3", 9: "C9", 27: "D27", 81: "E81"}
+------------------------------------------------------------------+
|   key=1    |   key=3    |   key=9    |   key=27    |   key=81    |
| value="A1" | value="B3" | value="C9" | value="D27" | value="E81" |
+------------------------------------------------------------------+
algorithm: linear search or binary search, depending on the array length

4. General
+--------------------------------------------------------------------+
| -200, -100, -50, -9, | 0, 1, 2, 4, 5, 6, 9 | 25, 75, 200, 512, 999 |  <--- Keys
+--------------------------------------------------------------------+
|   Left sparse part   |      Dense part     |   Right sparse part   |
| keys < dense_min_key |  dense_min_key = 3  |  keys > dense_max_key |
|                      |  dense_max_key = 10 |                       |
+--------------------------------------------------------------------+
```

### String-Key

Let $n$ be the number of input entries, $L$ be the maximum length of input keys. The underlying data structure of string-key fixed map can be one of the following, selected by reflect_cpp26 automatically according to the input data:
1. **Hash table, O(L)**: The underlying data structure is a hash table with open addressing. The fixed map builder tries with various remainder values. For each remainder $M$, hash table is applied only if (1) No hash collision in the input entries; (2) $M \le n/\alpha$ where $\alpha$ is the minimum load factor (default value is 0.5); (3) At most 2 entries share the same modulo of hash value.
2. **Linear array with hash, O(L + log n)**: The underlying data structure is an array of triplets `(key_hash, key, value)` sorted by `key_hash`. If $n$ is greater or equal to some threshold (default value is 8), then binary search by hash value is applied for each fixed map access; Otherwise, linear search is applied.
3. **Naive linear array, O(L log n)**: The underlying data structure is an array of pairs `(key, value)` sorted by `key`. If $n$ is greater or equal to some threshold (default value is 8), then binary search by key string is applied for each fixed map access; Otherwise, linear search is applied.

## Components

### Integral-Key & Enum-Key

Defined in header `<reflect_cpp26/fixed_map/integral_key.hpp>`.

```cpp
namespace reflect_cpp26 {

struct integral_key_fixed_map_options {
  bool already_sorted = false;
  bool already_unique = false;
  bool adjusts_alignment = false;
  bool default_value_is_always_invalid = false;
  double min_load_factor = 0.5;
  size_t dense_lookup_threshold = 4;
  size_t binary_search_threshold = 8;
};

template <class KVPairRange>
consteval auto make_integral_key_fixed_map(
    KVPairRange kv_pairs,
    integral_key_fixed_map_options options = {}) -> std::meta::info;

}  // namespace reflect_cpp26

#define REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(kv_pairs, ...) \
  [:reflect_cpp26::make_integral_key_fixed_map(kv_pairs, ##__VA_ARGS__):]
```

Input `kv_pairs` should be a range of key-value pairs whose:
* Keys are unique;
* Keys are of either integral or enum type;
* Values can be converted to structural type during compile-time (using [`to_structural()` in type_operations](./type_operations.md)).

The function returns a reflected constant of the fixed map, which can be extracted via `[: V :]`. Typically, the macro `REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP` can be used to extract the constant fixed map object.

The generated fixed map supports the following operations (`using value_type = to_structural_result_t<InputValueType>`, *signedness-safe and narrowing-safe* key comparison applied):
* `size() -> size_t`: Returns the number of entries.
* `get(key) -> std::pair<const value_type&, bool>`: If the input key exists, then returns the corresponding value and `true` which indicates that the returned value is valid; Otherwise, returns `{value_type{}, false}`.
* `operator[](key) -> const value_type&`: Returns the value for the given key, or `value_type{}` if the input key does not exist.

The argument `options` contains parameters to fine-tune the behavior during fixed map construction:

* `already_sorted` (default: `false`): Whether input key-value-pair range is already sorted. This option helps to improve compile-time performance by skipping sorting if it's ensured that kv-pairs are sorted already. UB or wrong result may occur if this flag is set as true but the input range is not sorted actually.
* `already_unique` (default: `false`): Whether input keys are already deduplicated. This option helps to improve compile-time performance by skipping key duplication check if it's ensured that keys are unique. UB or wrong result may occur if this flag is set as true but the input keys are not deduplicated actually.
* `adjusts_alignment` (default: `false`): Whether alignment optimization is enabled. If enabled, then the elements of underlying arrays will be aligned to $2^x$ bytes for maximized random-access performance.
* `default_value_is_always_invalid` (default: `false`): Whether `value_type{}` always represents an invalid value. Some underlying data structures (dense flat map) may require an extra boolean field `is_valid` to distinguish between valid values which happen to be equal to `value_type{}` and invalid values. To eliminate this field and reduce memory usage when it's ensured that `value_type{}` never appear in the input kv-pair range, set this option to `true` and ensure an `is_valid(v)` predicate is accessible from `namespace reflect_cpp26` or via ADL for `value_type`. For C++ builtin types (arithmetic, pointers, etc.) and `meta_basic_string_view<CharT>`, `is_valid(v)` predicate is already predefined and can be implicitly included via `<reflect_cpp26/fixed_map/integral_key.hpp>`.
* `min_load_factor` (default: `0.5`): Minimum load factor for dense flat map.
* `dense_lookup_threshold` (default: `4`): Dense subrange length threshold. If the longest dense subrange of input has length no less than this threshold, fast lookup is enabled for this dense subrange.
* `binary_search_threshold` (default: `8`): Sparse subrange length threshold. If the length of an input sparse (sub-)range is no less than this threshold, binary search is applied during lookup. Linear search is applied otherwise.

**Example:**

```cpp
// Integral key
constexpr auto map1_entries() -> std::vector<std::pair<int, std::string>> {
  return {{1, "one"}, {2, "two"}, {3, "three"}};
}
constexpr auto map1 = REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(map1_entries());
static_assert(map1.size() == 3);
static_assert(map1[2] == "two");
static_assert(map1[4] == "");  // Returns meta_string_view{} representing an empty string

// Enum key
enum class color { red, green, blue };
constexpr auto map2_entries() -> std::vector<std::pair<color, int>> {
  return {{color::red, 0xFF0000}, {color::green, 0x00FF00}, {color::blue, 0x0000FF}};
}
constexpr auto map2 = REFLECT_CPP26_INTEGRAL_KEY_FIXED_MAP(map2_entries());
static_assert(map2[color::green] == 0x00FF00);
```

### String-Key

Defined in header `<reflect_cpp26/fixed_map/string_key.hpp>`.

```cpp
namespace reflect_cpp26 {

struct string_key_fixed_map_options {
  bool already_ascii_only = false;
  bool already_unique = false;
  bool ascii_case_insensitive = false;
  bool adjusts_alignment = false;
  double min_load_factor = 0.5;
  size_t max_n_iterations = 64;
  size_t optimization_threshold = 4;
  size_t binary_search_threshold = 8;
};

template <std::ranges::input_range KVPairRange>
consteval auto make_string_key_fixed_map(
    KVPairRange&& kv_pairs,
    string_key_fixed_map_options options = {}) -> std::meta::info;

}  // namespace reflect_cpp26

#define REFLECT_CPP26_STRING_KEY_FIXED_MAP(kv_pairs, ...) \
  [:reflect_cpp26::make_string_key_fixed_map(kv_pairs, ##__VA_ARGS__):]
```

Input `kv_pairs` should be a range of key-value pairs whose:
* Keys are unique;
* Keys are of string-like type with any character type;
* Values can be converted to structural type during compile-time.

The function returns a reflected constant of the fixed map, which can be extracted via `[: V :]`. Typically, the macro `REFLECT_CPP26_STRING_KEY_FIXED_MAP` can be used to extract the constant fixed map object.

The generated fixed map supports the following operations (`using value_type = to_structural_result_t<InputValueType>`):
* `size() -> size_t`: Returns the number of entries.
* `get(key) -> std::pair<const value_type&, bool>`: If the input key exists, then returns the corresponding value and `true` which indicates that the returned value is valid; Otherwise, returns `{value_type{}, false}`.
* `operator[](key) -> const value_type&`: Returns the value for the given key, or `value_type{}` if the input key does not exist.

The argument `options` contains parameters to fine-tune the behavior during fixed map construction:

* `already_ascii_only` (default: `false`): Whether input keys contain ASCII characters only. This option has effect only if already_unique is true (see below), and helps to improve compile-time performance by skipping case conversion if it's ensured that keys only contain ASCII characters and are already lower-case. UB or wrong result may occur if this flag is set as true but the input keys are not ASCII-only actually.
* `already_unique` (default: `false`): Whether input keys are already deduplicated. This option helps to improve compile-time performance by skipping key duplication check if it's ensured that keys are unique. UB or wrong result may occur if this flag is set as true but the input keys are not deduplicated actually.
* `ascii_case_insensitive` (default: `false`): Whether the fixed map is built in a case-insensitive manner. Only ASCII characters are allowed in input keys when this option is enabled (since no locale data is available during compile-time).
* `adjusts_alignment` (default: `false`): Whether alignment optimization is enabled. If enabled, then the elements of underlying arrays will be aligned to $2^x$ bytes for maximized random-access performance.
* `min_load_factor` (default: `0.5`): Minimum load factor for underlying data structures (hash table, etc.).
* `max_n_iterations` (default: `64`): Maximum number of attempts to find suitable remainder $M$ for hash table structure, where hashed index = `string_hash(key) % M`.
* `optimization_threshold` (default: `4`): Length threshold to enable optimized data structures. Naive linear list searching is used if the input length is less than this threshold.
* `binary_search_threshold` (default: `8`): Length threshold to enable binary search for hash-based or naive string-key flat map. Linear search is applied otherwise.

**Example:**

```cpp
constexpr auto map_entries() -> std::vector<std::pair<std::string, int>> {
  return {{"apple", 1}, {"banana", 2}, {"cherry", 3}};
}
constexpr auto map = REFLECT_CPP26_STRING_KEY_FIXED_MAP(map_entries());
static_assert(map.size() == 3);
static_assert(map["banana"] == 2);
static_assert(map["BANANA"] == 0);  // Returns int{}

// Case-insensitive lookup
constexpr auto ci_map_entries() -> std::vector<std::pair<std::string, int>> {
  return {{"Apple", 1}, {"BANANA", 2}};
}
constexpr auto ci_map = REFLECT_CPP26_STRING_KEY_FIXED_MAP(
    ci_map_entries(),
    {.ascii_case_insensitive = true});
static_assert(ci_map["apple"] == 1);
static_assert(ci_map["Banana"] == 2);
```
