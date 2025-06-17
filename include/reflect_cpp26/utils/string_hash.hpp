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

#ifndef REFLECT_CPP26_UTILS_STRING_HASH_HPP
#define REFLECT_CPP26_UTILS_STRING_HASH_HPP

#include <cstdint>
#include <string_view>

namespace reflect_cpp26 {
constexpr unsigned bkdr_hash_default_p = 131u;

constexpr uint32_t bkdr_hash32(
  const char* begin, const char* end, uint32_t p = bkdr_hash_default_p)
{
  uint32_t res = 0;
  for (; begin < end; ++begin) {
    res = res * p + (uint32_t)(*begin);
  }
  return res;
}

constexpr uint64_t bkdr_hash64(
  const char* begin, const char* end, uint64_t p = bkdr_hash_default_p)
{
  uint64_t res = 0;
  for (; begin < end; ++begin) {
    res = res * p + (uint64_t)(*begin);
  }
  return res;
}

constexpr uint32_t bkdr_hash32(
  std::string_view str, uint32_t p = bkdr_hash_default_p)
{
  return bkdr_hash32(str.data(), str.data() + str.size(), p);
}

constexpr uint64_t bkdr_hash64(
  std::string_view str, uint64_t p = bkdr_hash_default_p)
{
  return bkdr_hash64(str.data(), str.data() + str.size(), p);
}
} // namespace reflect_cpp26

#endif // REFLECT_CPP26_UTILS_STRING_HASH_HPP
