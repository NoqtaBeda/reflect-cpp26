## Introduction

reflect_cpp26 provides functionality for common type operations, including:
* Compile-time conversion to static storage
* Accessing non-static data members by member index

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
   * If `V` is a character type, the result is `meta_basic_string_view<V>`;
   * Otherwise, the result is `meta_span<to_structural_result_t<V>>`.
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

### Auxiliary Components

#### Data Member Access by Index

Defined in header `<reflect_cpp26/type_operations/member_access.hpp>`.

```cpp
namespace reflect_cpp26 {

// (1) Direct member access
template <size_t I, class T>
constexpr decltype(auto) get_ith_direct_nonstatic_data_member(T&& obj);

// (2) Public direct member access
template <size_t I, class T>
constexpr decltype(auto) get_ith_public_direct_nonstatic_data_member(T&& obj);

// (3) Flattened member access
template <size_t I, class T>
constexpr decltype(auto) get_ith_nonstatic_data_member(T&& obj);

// (4) Public flattened member access
template <size_t I, class T>
constexpr decltype(auto) get_ith_public_nonstatic_data_member(T&& obj);

}  // namespace reflect_cpp26
```

These functions provide indexed access to non-static data members of class, union, or flattenable types:

* (1) `get_ith_direct_nonstatic_data_member<I>(obj)` returns the I-th (0-based index) direct non-static data member of `obj` by definition order;
* (2) `get_ith_public_direct_nonstatic_data_member<I>(obj)` returns the I-th direct non-static data member with public access;
* (3) `get_ith_nonstatic_data_member<I>(obj)` returns the I-th flattened non-static data member (see [flattenable types](./type_traits.md) for details);
* (4) `get_ith_public_nonstatic_data_member<I>(obj)` returns the I-th flattened non-static data member with public access.

**Requirements to Template Parameters:**

* `I` - The index of the member to access. Must be less than the number of corresponding non-static data members.
* `T` - The type of `obj`:
  * For (1) and (2): `std::remove_cvref_t<T>` must satisfy `class_or_union_type`;
  * For (3) and (4): `std::remove_cvref_t<T>` must satisfy `partially_flattenable_class`.

**Return Type:**

The return type depends on the member type and cvref-qualification of `T`:
1. If the member is a bit-field with underlying type `M`, the result type is `M` (cv-qualifiers discarded);
2. If the member is a reference of type `M&` or `M&&`, the result type is `M&` or `M&&`;
3. Otherwise, if the member type is `M`, the result type is `add_cvref_like_t<M, T>`.

Example:

```cpp
namespace refl = reflect_cpp26;

struct foo_t : std::pair<int, int> {
private:
  int a;
public:
  int x, y, z;
  explicit constexpr foo_t(int base)
    : std::pair<int, int>{base - 2, base - 1},
      a(base), x(base + 1), y(base + 2), z(base + 3), b(base + 4) {}
protected:
  int b;
};

constexpr auto foo = foo_t{10};
// Layout: base.first=8, base.second=9, a=10, x=11, y=12, z=13, b=14

// Direct access: only direct members of foo_t (not from base class)
static_assert(refl::get_ith_direct_nonstatic_data_member<0>(foo) == 10);  // a
static_assert(refl::get_ith_direct_nonstatic_data_member<1>(foo) == 11);  // x
static_assert(refl::get_ith_direct_nonstatic_data_member<4>(foo) == 14);  // b

// Flattened access: base class members first, then direct members
static_assert(refl::get_ith_nonstatic_data_member<0>(foo) == 8);   // first
static_assert(refl::get_ith_nonstatic_data_member<1>(foo) == 9);   // second
static_assert(refl::get_ith_nonstatic_data_member<2>(foo) == 10);  // a
static_assert(refl::get_ith_nonstatic_data_member<6>(foo) == 14);  // b

// Public flattened access: only public members (first, second, x, y, z)
static_assert(refl::get_ith_public_nonstatic_data_member<0>(foo) == 8);   // first
static_assert(refl::get_ith_public_nonstatic_data_member<2>(foo) == 11);  // x
static_assert(refl::get_ith_public_nonstatic_data_member<4>(foo) == 13);  // z
```

For examples with inheritance, bit-fields, references, and unions, see the [unit test](../tests/type_operations/test_member_access.cpp).
