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

#ifndef REFLECT_CPP26_UTILS_META_TUPLE_HPP
#define REFLECT_CPP26_UTILS_META_TUPLE_HPP

#include <ranges>
#include <reflect_cpp26/type_traits/tuple_like_types.hpp>
#include <reflect_cpp26/utils/functional.hpp>
#include <reflect_cpp26/utils/meta_utility.hpp>

namespace reflect_cpp26 {
namespace impl {
constexpr auto predefined_field_names = std::array{
    "_0",   "_1",   "_2",   "_3",   "_4",   "_5",   "_6",   "_7",   "_8",   "_9",   "_10",  "_11",
    "_12",  "_13",  "_14",  "_15",  "_16",  "_17",  "_18",  "_19",  "_20",  "_21",  "_22",  "_23",
    "_24",  "_25",  "_26",  "_27",  "_28",  "_29",  "_30",  "_31",  "_32",  "_33",  "_34",  "_35",
    "_36",  "_37",  "_38",  "_39",  "_40",  "_41",  "_42",  "_43",  "_44",  "_45",  "_46",  "_47",
    "_48",  "_49",  "_50",  "_51",  "_52",  "_53",  "_54",  "_55",  "_56",  "_57",  "_58",  "_59",
    "_60",  "_61",  "_62",  "_63",  "_64",  "_65",  "_66",  "_67",  "_68",  "_69",  "_70",  "_71",
    "_72",  "_73",  "_74",  "_75",  "_76",  "_77",  "_78",  "_79",  "_80",  "_81",  "_82",  "_83",
    "_84",  "_85",  "_86",  "_87",  "_88",  "_89",  "_90",  "_91",  "_92",  "_93",  "_94",  "_95",
    "_96",  "_97",  "_98",  "_99",  "_100", "_101", "_102", "_103", "_104", "_105", "_106", "_107",
    "_108", "_109", "_110", "_111", "_112", "_113", "_114", "_115", "_116", "_117", "_118", "_119",
    "_120", "_121", "_122", "_123", "_124", "_125", "_126", "_127", "_128", "_129", "_130", "_131",
    "_132", "_133", "_134", "_135", "_136", "_137", "_138", "_139", "_140", "_141", "_142", "_143",
    "_144", "_145", "_146", "_147", "_148", "_149", "_150", "_151", "_152", "_153", "_154", "_155",
    "_156", "_157", "_158", "_159", "_160", "_161", "_162", "_163", "_164", "_165", "_166", "_167",
    "_168", "_169", "_170", "_171", "_172", "_173", "_174", "_175", "_176", "_177", "_178", "_179",
    "_180", "_181", "_182", "_183", "_184", "_185", "_186", "_187", "_188", "_189", "_190", "_191",
    "_192", "_193", "_194", "_195", "_196", "_197", "_198", "_199", "_200", "_201", "_202", "_203",
    "_204", "_205", "_206", "_207", "_208", "_209", "_210", "_211", "_212", "_213", "_214", "_215",
    "_216", "_217", "_218", "_219", "_220", "_221", "_222", "_223", "_224", "_225", "_226", "_227",
    "_228", "_229", "_230", "_231", "_232", "_233", "_234", "_235", "_236", "_237", "_238", "_239",
    "_240", "_241", "_242", "_243", "_244", "_245", "_246", "_247", "_248", "_249", "_250", "_251",
    "_252", "_253", "_254", "_255"};

constexpr auto get_field_name(size_t index) -> std::string_view {
  if (index < predefined_field_names.size()) {
    return predefined_field_names[index];
  }
  compile_error("Index too large.");
}
}  // namespace impl

template <class... Args>
struct meta_tuple {
  static constexpr auto tuple_size = sizeof...(Args);

  struct underlying_type;
  consteval {
    auto types = std::vector{^^Args...};
    auto members = std::vector<std::meta::info>{};
    members.reserve(sizeof...(Args));
    for (size_t i = 0; i < sizeof...(Args); i++) {
      auto options = std::meta::data_member_options{
          .name = impl::get_field_name(i),
      };
      members.push_back(data_member_spec(types[i], options));
    }
    define_aggregate(^^underlying_type, members);
  }
  // values are exposed as public data member to make meta_tuple
  // structural aggregate.
  underlying_type values;

private:
  static consteval auto get_nth_field(size_t n) -> std::meta::info {
    return all_direct_nonstatic_data_members_v<underlying_type>[n];
  }

public:
  constexpr meta_tuple() = default;

  // cvref dropped during CTAD
  constexpr meta_tuple(const Args&... args) : values{args...} {}

  /**
   * Free get function of meta_tuple.
   */
  template <size_t I>
    requires(I < tuple_size)
  friend constexpr auto& get(meta_tuple& tuple) {
    return tuple.values.[:get_nth_field(I):];
  }

  template <size_t I>
    requires(I < tuple_size)
  friend constexpr const auto& get(const meta_tuple& tuple) {
    return tuple.values.[:get_nth_field(I):];
  }

  template <size_t I>
    requires(I < tuple_size)
  friend constexpr auto&& get(meta_tuple&& tuple) {
    return std::move(tuple.values.[:get_nth_field(I):]);
  }

  template <size_t I>
    requires(I < tuple_size)
  friend constexpr const auto&& get(const meta_tuple&& tuple) {
    return std::move(tuple.values.[:get_nth_field(I):]);
  }
};

// TODO: We need better way to implement operator == and <=>
//       between meta_tuple and other tuple-like types.

namespace impl {
template <class Tuple1, class Tuple2>
constexpr bool is_memberwise_eq_comparable() {
  constexpr auto N = std::tuple_size_v<Tuple1>;
  if constexpr (std::tuple_size_v<Tuple2> == N) {
    template for (constexpr auto I : std::views::iota(0zU, N)) {
      using E1 = std::tuple_element_t<I, Tuple1>;
      using E2 = std::tuple_element_t<I, Tuple2>;
      if (!is_equal_comparable_v<E1, E2>) return false;
    }
    return true;
  } else {
    return false;
  }
}

template <class Tuple1, class Tuple2>
constexpr auto is_memberwise_eq_comparable_v = is_memberwise_eq_comparable<Tuple1, Tuple2>();
}  // namespace impl

template <tuple_like TupleLike, class... Args>
  requires(impl::is_memberwise_eq_comparable_v<meta_tuple<Args...>, TupleLike>)
constexpr bool operator==(const meta_tuple<Args...>& lhs, const TupleLike& rhs) {
  constexpr auto N = sizeof...(Args);
  template for (constexpr auto I : std::views::iota(0zU, N)) {
    const auto& x = get_ith_element<I>(lhs);
    const auto& y = get_ith_element<I>(rhs);
    if (!(x == y)) {
      return false;
    }
  }
  return true;
}

// Deduction guide (cvref dropped, same behavior as std::tuple)
template <class... Args>
meta_tuple(Args...) -> meta_tuple<Args...>;
}  // namespace reflect_cpp26

template <class... Args>
struct std::tuple_size<reflect_cpp26::meta_tuple<Args...>>
    : std::integral_constant<size_t, sizeof...(Args)> {};

template <size_t I, class... Args>
struct std::tuple_element<I, reflect_cpp26::meta_tuple<Args...>> {
  using type = Args...[I];
};

#endif  // REFLECT_CPP26_UTILS_META_TUPLE_HPP
