/**
 * Copyright (c) 2025 NoqtaBeda (noqtabeda@163.com)
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

#ifndef REFLECT_CPP26_ENUM_IMPL_CONSTANTS_HPP
#define REFLECT_CPP26_ENUM_IMPL_CONSTANTS_HPP

#include <cstddef>

namespace reflect_cpp26::impl::enum_constants {
#ifdef REFLECT_CPP26_ENUM_ENABLE_VALUE_TABLE_LOOKUP_THRESHOLD
constexpr bool   enable_value_table_lookup_threshold_is_custom = true;
constexpr size_t enable_value_table_lookup_threshold =
  REFLECT_CPP26_ENUM_ENABLE_VALUE_TABLE_LOOKUP_THRESHOLD;
#else
constexpr bool   enable_value_table_lookup_threshold_is_custom = false;
constexpr size_t enable_value_table_lookup_threshold = 4;
#endif

#ifdef REFLECT_CPP26_ENUM_ENABLE_HASH_TABLE_LOOKUP_THRESHOLD
constexpr bool   enable_hash_table_lookup_threshold_is_custom = true;
constexpr size_t enable_hash_table_lookup_threshold =
  REFLECT_CPP26_ENUM_ENABLE_HASH_TABLE_LOOKUP_THRESHOLD;
#else
constexpr bool   enable_hash_table_lookup_threshold_is_custom = false;
constexpr size_t enable_hash_table_lookup_threshold = 4;
#endif

#ifdef REFLECT_CPP26_ENUM_DISABLE_HASH_TABLE_LOOKUP_THRESHOLD
constexpr bool   disable_hash_table_lookup_threshold_is_custom = true;
constexpr size_t disable_hash_table_lookup_threshold =
  REFLECT_CPP26_ENUM_DISABLE_HASH_TABLE_LOOKUP_THRESHOLD;
#else
constexpr bool   disable_hash_table_lookup_threshold_is_custom = false;
constexpr size_t disable_hash_table_lookup_threshold = 64;
#endif

#ifdef REFLECT_CPP26_ENUM_ENABLE_BINARY_SEARCH_THRESHOLD
constexpr bool   enable_binary_search_threshold_is_custom = true;
constexpr size_t enable_binary_search_threshold =
  REFLECT_CPP26_ENUM_ENABLE_BINARY_SEARCH_THRESHOLD;
#else
constexpr bool   enable_binary_search_threshold_is_custom = false;
constexpr size_t enable_binary_search_threshold = 12;
#endif

#ifdef REFLECT_CPP26_ENUM_HASH_INV_MIN_LOAD_FACTOR
constexpr bool   inv_min_load_factor_is_custom = true;
constexpr size_t inv_min_load_factor =
  REFLECT_CPP26_ENUM_HASH_INV_MIN_LOAD_FACTOR;
#else
constexpr bool   inv_min_load_factor_is_custom = false;
constexpr size_t inv_min_load_factor = 4; // Load factor >= 0.25 by default
#endif
} // namespace reflect_cpp26::impl::enum_constants

#endif // REFLECT_CPP26_ENUM_IMPL_CONSTANTS_HPP
