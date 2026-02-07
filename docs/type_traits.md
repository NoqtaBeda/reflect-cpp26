## Introduction

reflect_cpp26 provides functionality for advanced type traits useful in common cases, some of which are unable to be implemented without compiler magic in pre-C++26 era.

## Core Components

### Structural Type

Defined in header `<reflect_cpp26/type_traits/structural_types.hpp>`.

```cpp
template <class T>
concept structural_type = /* ... */;
```

The concept `structural_type` tests whether `std::remove_cv_t<T>` is a [structural](https://en.cppreference.com/w/cpp/language/template_parameters) type, i.e. a type whose value can be used as a non-type template parameter. A structural type is one of the following types (optionally cv-qualified, qualifiers are ignored):
* Lvalue reference type (to object or to function);
* An integral type;
* A pointer type (to object or to function);
* A pointer to member type (to member object or to member function);
* An enumeration type;
* `std::nullptr_t`;
* A floating-point type (since C++20);
* A lambda closure type whose lambda expression has no capture (since C++20);
* A non-closure literal class type with the following properties (since C++20):
  * All base classes and non-static data members are public and non-mutable;
  * Types of all base classes and non-static data members are structural types or (possibly multi-dimensional) array thereof.

See [unit test](../tests/type_traits/test_structural_types.cpp) for examples and details.

### Flattenable Type

Defined in header `<reflect_cpp26/type_traits/class_types/flattenable.hpp>`.

```cpp
template <class T>
concept partially_flattenable_class = /* ... */;

template <class T>
concept partially_flattenable = /* ... */;

template <class T>
concept flattenable_class = /* ... */;

template <class T>
concept flattenable = /* ... */;

template <class T>
concept flattenable_aggregate_class = /* ... */;

template <class T>
concept flattenable_aggregate = /* ... */;
```

This group of concepts tests whether a type can be "flattened", i.e. whether we can construct an aggregate type which contains all the non-static data members (or public ones only) without ambiguity or semantic loss. Flattening can be helpful during serialization or deserialization of C++ types.

The concept `partially_flattenable_class` tests whether `std::remove_cv_t<T>` is a non-union class type that is partially flattenable. A class type is partially flattenable if it satisfies all the following:
* No virtual base classes;
* No ambiguous base classes;
* Every direct base class is also partially flattenable, recursively.

The concept `partially_flattenable` tests whether `std::remove_cv_t<T>` is a partially flattenable type. A type is partially flattenable if it can be classified as one of the following:
* **Scalar types**: arithmetic types, enum types, pointers (including `std::nullptr_t` and function pointers), pointers to members, etc.;
* **C-style array types**: including both bounded `U[N]` and unbounded `U[]`;
* **Partially flattenable class types**: as described above.

The concept `flattenable_class` tests whether `std::remove_cv_t<T>` is a non-union class type that is flattenable. A class type is flattenable if it is a partially flattenable class type that additionally satisfies the following conditions:
* No private or protected non-static data members;
* No private or protected direct base classes that are not empty;
* Every direct base class is also flattenable, recursively.

The concept `flattenable` tests whether `std::remove_cv_t<T>` is a flattenable type. A type is flattenable if it satisfies one of the following:
* **Scalar types**: same as `partially_flattenable`;
* **Array types**: same as `partially_flattenable`;
* **Flattenable class types**: as described above.

The concept `flattenable_aggregate_class` tests whether `std::remove_cv_t<T>` is a non-union class type that is a flattenable aggregate. A class type is a flattenable aggregate if it is a flattenable class type that additionally satisfies the following constraints:
* The class is an aggregate;
* Every direct base class is also a flattenable aggregate, recursively.

The concept `flattenable_aggregate` tests whether `std::remove_cv_t<T>` is a flattenable aggregate type. A type is a flattenable aggregate if it satisfies one of the following:
* **Array types**: both bounded `U[N]` and unbounded `U[]`;
* **Flattenable aggregate class types**: as described above.

Note that references, unions, and classes with virtual inheritance are never flattenable. Additionally, classes with private or protected members (non-static data members or non-empty base classes) are not `flattenable`, though they may be `partially_flattenable`.

Example:

```cpp
namespace refl = reflect_cpp26;

struct foo_t {
  std::string first_name;
  std::string last_name;
};
struct bar_t : foo_t {
  int age;
};

// 1. Flattenable aggregate
static_assert(refl::flattenable_aggregate_class<bar_t>);

struct baz_t : foo_t {
  virtual void introduce_myself() {
    std::println("Hello. I am {} {}.", this->first_name, this->last_name);
  }
};

// 2. Flattenable
static_assert(refl::flattenable_class<baz_t>);
// Not aggregate (due to existence of implicit v-table pointer)
static_assert(! refl::flattenable_aggregate_class<baz_t>);

class qux_t : foo_t {
private:
  uint64_t last_visit_timestamp_;
};

// 3. Partially flattenable (we can still try to flatten its public members)
static_assert(refl::partially_flattenable_class<qux_t>);
// Not flattenable (due to existance of private non-static data members)
static_assert(! refl::flattenable_class<qux_t>);
// Not flattenable aggregate (stronger constraints than flattenable concept)
static_assert(! refl::flattenable_aggregate_class<qux_t>);

// Hint: partially_flattenable has loose constraints that most class types in practice can satisfy.
// A class type can be partially flattenable even if there's nothing public to be flattened.
static_assert(refl::partially_flattenable_class<std::string>);
static_assert(refl::partially_flattenable_class<std::vector<std::string>>);
```

See [unit test](../tests/type_traits/class_types/test_flattenable_types.cpp) for more examples and details.

### Details of Flattened Class Types

Defined in header `<reflect_cpp26/type_traits/class_types/flattened_nsdm.hpp>`.

```cpp
struct flattened_data_member_info {
  std::meta::info member;
  std::meta::member_offset actual_offset;

  consteval auto type() const -> std::meta::info;
  consteval auto direct_parent() const -> std::meta::info;
  consteval auto offset_in_parent() const -> std::meta::member_offset;
  consteval auto offset_bits_in_parent() const -> size_t;
  consteval auto offset_bytes_in_parent() const -> size_t;
  consteval auto actual_offset_bits() const -> size_t;
  consteval auto actual_offset_bytes() const -> size_t;
};

template <partially_flattenable_class T>
constexpr std::array<flattened_data_member_info, /*N*/> all_flattened_nonstatic_data_members_v;

template <partially_flattenable_class T>
constexpr std::array<flattened_data_member_info, /*N*/> public_flattened_nonstatic_data_members_v;
```

The struct `flattened_data_member_info` holds information about a non-static data member of class type `T`, which may be inherited from base classes of `T`. It contains the following members:
* `member` - Reflection to a non-static data member of `T`, either defined by `T` directly or inherited from some base class;
* `actual_offset` - Offset of the member relative to `T`;
* `type()` - Returns the reflection to the member's type;
* `direct_parent()` - Returns the reflection to the class that the member is directly defined in;
* `offset_in_parent()` - Returns the offset of the member in its direct parent class;
* `offset_bits_in_parent()` - Returns the offset in bits of the member in its direct parent class;
* `offset_bytes_in_parent()` - Returns the offset in bytes of the member in its direct parent class (compile error if member is a bit-field);
* `actual_offset_bits()` - Returns the actual offset in bits of the member relative to `T`;
* `actual_offset_bytes()` - Returns the actual offset in bytes of the member relative to `T` (compile error if member is a bit-field).

The following example shows the difference between "offset in parent" and "actual offset". Actual offsets are relative to `DemoStructC`.
```cpp
struct DemoStructA {
  int64_t a1;
  int32_t a2;
};
struct DemoStructB : DemoStructA {
  int64_t b1;
  std::array<int32_t, 4> b2;
};
struct DemoStructC : DemoStructB {
private:
  int32_t c1;
public:
  int32_t c2;
}
// Layout of class DemoStructC (each '*' represents one byte):
// 0               8               16              24              32              40
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// |---- a1 -----| |-a2--|         |---- b1 -----| |------------ b2 -------------| |-c1--| |-c2--|
```
| Data Member | Size | `actual_offset_bytes()` | `offset_bytes_in_parent()` |
|:--:|:--:|:--:|:--:|
| `DemoStructA::a1` | 8 | 0 | 0 |
| `DemoStructA::a2` | 4 | 8 | 8 |
| `DemoStructB::b1` | 8 | 16 | 0 |
| `DemoStructB::b2` | 16 | 24 | 8 |
| `DemoStructC::c1` | 4 | 40 | 0 |
| `DemoStructC::c2` | 4 | 44 | 4 |

The variable template `all_flattened_nonstatic_data_members_v` gets a full list of non-static data members of non-union class `T`, including direct and inherited ones. Members are sorted in ascending order by actual offset relative to `T`.

The variable template `public_flattened_nonstatic_data_members_v` gets a full list of non-static data members with public access of non-union class `T` (i.e. each member in the list can be accessed globally via class `T`), including direct and inherited ones. Members are sorted in ascending order by actual offset relative to `T`.

Example:
```cpp
namespace refl = reflect_cpp26;

// | `DemoStructA::a1` | 8 | 0 | 0 |
// | `DemoStructA::a2` | 4 | 8 | 8 |
// | `DemoStructB::b1` | 8 | 16 | 0 |
// | `DemoStructB::b2` | 16 | 24 | 8 |
// | `DemoStructC::c1` | 4 | 40 | 0 |
// | `DemoStructC::c2` | 4 | 44 | 4 |
template for (constexpr auto M : refl::all_flattened_nonstatic_data_members_v<DemoStructC>) {
  std::println("| `{}::{}` | {} | {} | {} |",
               identifier_of(M.direct_parent()),
               identifier_of(M.member),
               size_of(M.member),
               M.actual_offset_bytes(),
               M.offset_bytes_in_parent());
}
```

## Auxiliary Components

### Type Classification

#### Arithmetic Types

Defined in header `<reflect_cpp26/type_traits/arithmetic_types.hpp>`.

```cpp
template <class T>
concept char_type = /* ... */;

template <class T>
concept non_bool_integral = /* ... */;

template <class T>
concept integer_type = /* ... */;

template <std::integral T>
using integral_to_integer_t = /* ... */;
```

The concept `char_type` checks whether `T` is a (maybe cv-qualified) character type, i.e. one of `char`, `wchar_t`, `char8_t`, `char16_t` or `char32_t`.

The concept `non_bool_integral` checks whether `T` is a (maybe cv-qualified) *integral* type which is not `bool`, i.e. `std::is_integral_v<T>` is true and `std::is_same_v<std::remove_cv_t<T>, bool>` is false.

The concept `integer_type` checks whether `T` is a (maybe cv-qualified) *integer* type, i.e. `std::integral - char_type - bool` (here we use concept to represent candidate type set).

The type alias `integral_to_integer_t` converts *integral* type `T` to an *integer* type with the same size and signedness. The major usage is to convert `bool` and character types to the corresponding `intN_t` or `uintN_t`.

Note that `signed char` and `unsigned char` are not `char_type`s as they are defined as character's *integer representation* rather than the character itself in C++ standard.

#### String-like Types

Defined in header `<reflect_cpp26/type_traits/string_like_types.hpp>`.

```cpp
template <class T>
concept string_like = /* ... */;

template <class T, class CharT>
concept string_like_of = /* ... */;

template <class T>
concept c_style_string = /* ... */;

template <string_like T>
using char_type_t = /* ... */;
```

The concept `string_like` tests whether `T` is a (maybe cv-qualified) string-like type. String-like types are those which can be converted to `std::basic_string_view<CharT>` for some character type `CharT` via the `std::basic_string_view` constructor, including but not limited to:
* C-style non-volatile character arrays: `CharT[]`, `const CharT[N]`, etc.
* C-style string pointer: `CharT*` and `const CharT*`;
* `std::basic_string<CharT, Traits, Alloc>` for any `Traits` and `Alloc`;
* `std::basic_string_view<CharT, Traits>` for any `Traits`;
* `meta_basic_string_view<CharT>` (defined in `<reflect_cpp26/utils/meta_string_view.hpp>`, a [structural](https://cppreference.com/w/cpp/language/template_parameters.html) replacement of `std::basic_string_view`);
* Contiguous range of characters: `std::vector<CharT>`, `std::array<CharT, N>`, etc.

The concept `string_like_of` tests whether `T` is a (maybe cv-qualified) string-like type whose character type is exactly `CharT`.

The concept `c_style_string` tests whether `T` is a C-style string, i.e. one of the following:
* C-style non-volatile character arrays: `CharT[]`, `const CharT[N]`, etc.
* C-style string pointer: `CharT*` and `const CharT*`.

The type alias `char_type_t` extracts the character type of the string-like type `T`.
* For contiguous ranges, `char_type_t` is its `value_type`;
* For C-style strings that decay to `CharT*` or `const CharT*`, `char_type_t` is `CharT`.

See [unit test](../tests/type_traits/test_string_like_types.cpp) for examples and details.

#### Tuple-like Types

Defined in header `<reflect_cpp26/type_traits/tuple_like_types.hpp>`.

```cpp
template <class T>
concept tuple_like = /* ... */;

template <class T>
concept pair_like = /* ... */;

template <class T, class... Args>
concept tuple_like_of = /* ... */;

template <class T, class... Args>
concept tuple_like_of_exactly = /* ... */;

template <class... Args>
constexpr bool are_tuple_like_of_same_size_v;

template <class T, class Tuple>
concept tuple_like_of_same_size_with = are_tuple_like_of_same_size_v<T, Tuples>;
```

The concept `tuple_like` tests whether `T` is a (maybe cv-qualified) [tuple-like](https://en.cppreference.com/w/cpp/utility/tuple/tuple-like) type. Tuple-like types are those whose `std::tuple_size` and `std::tuple_element` specializations are provided, and members can be accessed via either `std::get<I>(t)` or `t.get<I>()` for all valid indices. This includes but is not limited to:
* `std::tuple`, `std::pair`, `std::array`;
* Custom tuple-like types that satisfy the tuple interface.

The concept `pair_like` tests whether `T` is a (maybe cv-qualified) pair-like type, i.e. a tuple-like type with size exactly 2.

The concept `tuple_like_of` tests whether `T` is a (maybe cv-qualified) tuple-like type whose element types can be converted to `Args...` respectively. The element types are compared using `std::is_convertible_v`.

The concept `tuple_like_of_exactly` tests whether `T` is a (maybe cv-qualified) tuple-like type whose element types are exactly `Args...` respectively. The element types are compared using `std::is_same_v`.

The variable template `are_tuple_like_of_same_size_v` checks whether `std::remove_cv_t<Args>...` are tuple-like types with the same tuple size.

The concept `tuple_like_of_same_size_with` tests whether `std::remove_cv_t<T>` and `std::remove_cv_t<Tuple>` are tuple-like types with the same tuple size.

### Qualifier Manipulation

Defined in header `<reflect_cpp26/type_traits/cvref.hpp>`.

```cpp
consteval auto add_cv_like(std::meta::info dest, std::meta::info src) -> std::meta::info;

consteval auto add_cvref_like(std::meta::info dest, std::meta::info src) -> std::meta::info;

consteval auto propagate_cv_like(std::meta::info dest, std::meta::info src) -> std::meta::info;

template <non_reference_type Dest, class Src>
using add_cv_like_t = [:add_cv_like(^^Dest, ^^Src):];

template <non_reference_type Dest, class Src>
using add_cvref_like_t = [:add_cvref_like(^^Dest, ^^Src):];

template <class Dest, class Src>
using propagate_cv_like_t = [:propagate_cv_like(^^Dest, ^^Src):];
```

The function `add_cv_like()` takes 2 reflections designating types `dest` and `src` as arguments. The function adds const-qualifier to `dest` if `remove_reference(src)` is const-qualified, and adds volatile-qualifier to `dest` if `remove_reference(src)` is volatile-qualified. Compilation error is raised on either of the following:
* `dest` or `src` is not a reflection designating some type;
* `dest` is reference-qualified.

The function `add_cvref_like()` takes 2 reflections designating types `dest` and `src` as arguments. The function adds const-qualifier to `dest` if `remove_reference(src)` is const-qualified, and adds volatile-qualifier to `dest` if `remove_reference(src)` is volatile-qualified, and finally adds lvalue-reference or rvalue-reference qualifier to `dest` if `src` is lvalue-reference or rvalue-reference. Compilation error is raised on either of the following:
* `dest` or `src` is not a reflection designating some type;
* `dest` is reference-qualified.

The function `propagate_cv_like()` takes 2 reflections designating types `dest` and `src` as arguments.
* If `dest` reflects a pointer type, the function propagates cv-qualifiers of `remove_reference(src)` to the pointee type of `dest` *recursively*, then adds cv-qualifiers of `remove_reference(src)` to `dest` itself;
* If `dest` reflects a reference type, the function propagates cv-qualifiers of `remove_reference(src)` to `remove_reference(dest)`, then adds the reference-qualifier of `dest` back;
* Otherwise, `propagate_cv_like()` is equivalent to `add_cv_like()`.
Compilation error is raised if `dest` or `src` is not a reflection designating some type.

The type aliases `add_cv_like_t`, `add_cvref_like_t` and `propagate_cv_like_t` are template-based interfaces to query the type conversion result.

Example (see [unit test](../tests/type_traits/test_cvref.cpp) for more examples):
```cpp
using refl = reflect_cpp26;

// Adds const-qualifier to T = int*
static_assert(std::is_same_v<refl::add_cv_like_t<int*, const double&>,
                            int* const>);  // const T = int* const

// Adds volatile-qualifier to T = const int*
static_assert(std::is_same_v<refl::add_cv_like_t<const int*, volatile double&&>,
                            const int* volatile>);  // volatile T = const int* volatile

// Adds volatile-qualifier to T = int* const
static_assert(std::is_same_v<refl::add_cv_like_t<int* const, volatile double&&>,
                            int* const volatile>);  // volatile T = int* const volatile

// Adds const-qualifier and lvalue-reference qualifier to T = int*
static_assert(std::is_same_v<refl::add_cvref_like_t<int*, const double&>,
                            int* const&>);  // const T& = int* const &

// Adds volatile-qualifier and rvalue-reference qualifier to T = const int*
static_assert(std::is_same_v<refl::add_cvref_like_t<const int*, volatile double&&>,
                            const int* volatile&&>);  // volatile T&& = const int* volatile &&

// Propagates volatile-qualifier to every level, including the pointer type itself.
// For a k-level pointer, the cv-qualifiers are applied (k+1) times.
static_assert(std::is_same_v<refl::propagate_cv_like_t<const int***, volatile double>,
                            const volatile int* volatile* volatile* volatile>);

// Propagates volatile-qualifier to the rvalue-referenced type.
// Note that the result is still a rvalue-reference.
static_assert(std::is_same_v<refl::propagate_cv_like_t<const int***&&, volatile double&>,
                            const volatile int* volatile* volatile* volatile&&>);
```

### Advanced Invocability Testing

Defined in header `<reflect_cpp26/type_traits/is_invocable.hpp>`.

```cpp
// ---- Group 1.1: Exact result type testing ----

template <class R, class Func, class... Args>
constexpr auto is_invocable_exactly_r_v = /* ... */;

template <class R, class Func, class... Args>
constexpr auto is_nothrow_invocable_exactly_r_v = /* ... */;

// ---- Group 1.2: Repetitive argument type testing ----

template <class Func, class Arg, size_t N>
constexpr auto is_invocable_n_v = /* ... */;

template <class Func, class Arg, size_t N>
constexpr auto is_nothrow_invocable_n_v = /* ... */;

template <class R, class Func, class Arg, size_t N>
constexpr auto is_invocable_r_n_v = /* ... */;

template <class R, class Func, class Arg, size_t N>
constexpr auto is_nothrow_invocable_r_n_v = /* ... */;

template <class R, class Func, class Arg, size_t N>
constexpr auto is_invocable_exactly_r_n_v = /* ... */;

template <class R, class Func, class Arg, size_t N>
constexpr auto is_nothrow_invocable_exactly_r_n_v = /* ... */;

// ---- Group 2.1: Common testing ----

template <class F, class... Args>
concept invocable = /* ... */;

template <class F, class... Args>
concept nothrow_invocable = /* ... */;

template <class F, class R, class... Args>
concept invocable_r = /* ... */;

template <class F, class R, class... Args>
concept nothrow_invocable_r = /* ... */;

// ---- Group 2.2: Exact result type testing ----

template <class F, class R, class... Args>
concept invocable_exactly_r = /* ... */;

template <class F, class R, class... Args>
concept nothrow_invocable_exactly_r = /* ... */;

// ---- Group 2.3: Repetitive argument type testing

template <class F, class Arg, size_t N>
concept invocable_n = /* ... */;

template <class F, class Arg, size_t N>
concept nothrow_invocable_n = /* ... */;

template <class F, class R, class Arg, size_t N>
concept invocable_r_n = /* ... */;

template <class F, class R, class Arg, size_t N>
concept nothrow_invocable_r_n = /* ... */;

template <class F, class R, class Arg, size_t N>
concept invocable_exactly_r_n = /* ... */;

template <class F, class R, class Arg, size_t N>
concept nothrow_invocable_exactly_r_n = /* ... */;
```

Various concepts are provided for testing callable types:
* `invocable` - checks whether `F` is invocable with `Args...`;
* `invocable_r` - checks whether `F` is invocable with `Args...` and the result is convertible to `R`;
* `invocable_exactly_r` - checks whether `F` is invocable with `Args...` and the result type is *exactly* `R` (no implicit conversion allowed). This is a stronger constraint than `invocable_r` when implicit conversion might cause ambiguity;
* `invocable_n` - checks whether `F` is invocable with `N` arguments of type `Arg`;
* `invocable_r_n` - checks whether `F` is invocable with `N` arguments of type `Arg` and the result is convertible to `R`;
* `invocable_exactly_r_n` - checks whether `F` is invocable with `N` arguments of type `Arg` and the result type is *exactly* `R`.

For each concept listed above, an additional `nothrow_*` variant above is provided which tests whether `F` is nothrow-invocable, i.e. the `operator()` is `noexcept`-qualified.

Each concept `name` is equivalent to the corresponding variable template `is_name_v`:

| Concept | Equivalent Variable Template |
|---------|---------------------------|
| `invocable<F, Args...>`<br/>`nothrow_invocable<F, Args...>` | `std::is_invocable_v<F, Args...>`<br/>`std::is_nothrow_invocable_v<F, Args...>` |
| `invocable_r<F, R, Args...>`<br/>`nothrow_invocable_r<F, R, Args...>` | `std::is_invocable_r_v<R, F, Args...>`<br/>`std::is_nothrow_invocable_r_v<R, F, Args...>` |
| `invocable_exactly_r<F, R, Args...>`<br/>`nothrow_invocable_exactly_r<F, R, Args...>` | `is_invocable_exactly_r_v<R, F, Args...>`<br/>`is_nothrow_invocable_exactly_r_v<R, F, Args...>` |
| `invocable_n<F, Arg, N>`<br/>`nothrow_invocable_n<F, Arg, N>` | `is_invocable_n_v<F, Arg, N>`<br/>`is_nothrow_invocable_n_v<F, Arg, N>` |
| `invocable_r_n<F, R, Arg, N>`<br/>`nothrow_invocable_r_n<F, R, Arg, N>` | `is_invocable_r_n_v<R, F, Arg, N>`<br/>`is_nothrow_invocable_r_n_v<R, F, Arg, N>` |
| `invocable_exactly_r_n<F, R, Arg, N>`<br/>`nothrow_invocable_exactly_r_n<F, R, Arg, N>` | `is_invocable_exactly_r_n_v<R, F, Arg, N>`<br/>`is_nothrow_invocable_exactly_r_n_v<R, F, Arg, N>` |

### Class Type Property Testing

Defined in header `<reflect_cpp26/type_traits/class_types.hpp>`, or the following subcomponent headers respectively:
1. `<reflect_cpp26/type_traits/class_types/ambiguous_inheritance.hpp>`
2. `<reflect_cpp26/type_traits/class_types/non_public_nsdm.hpp>`
3. `<reflect_cpp26/type_traits/class_types/virtual_inheritance.hpp>`

```cpp
// 1.
template <class T>
concept class_without_ambiguous_inheritance = /* ... */;
// 2.
template <class T>
concept class_without_non_public_nonstatic_data_members = /* ... */;
// 3.
template <class T>
concept class_without_virtual_inheritance = /* ... */;
```

The concept `class_without_ambiguous_inheritance` tests whether `std::remove_cv_t<T>` is a non-union class type without ambiguous non-empty base, i.e. no non-virtual base class `B` is inherited more than once, either directly or indirectly.
* Empty base classes (typically tag types in practice) may cause ambiguity as well even if they make no effect on class layout.
* Always evaluates to `false` for non-class types (including references).

The concept `class_without_non_public_nonstatic_data_members` tests whether `std::remove_cv_t<T>` is a non-union class without inaccessible non-static data members, i.e.:
* No direct protected or private data members;
* No protected or private direct base classes that are not empty;
* No protected or private data members from public base classes, recursively.
* Always evaluates to `false` for non-class types: scalar types, references, arrays, unions, etc.

The concept `class_without_virtual_inheritance` checks whether `std::remove_cv_t<T>` is a non-union class without virtual base classes (either directly or indirectly).
* Always evaluates to `false` for non-class types: scalar types, references, arrays, unions, etc.
