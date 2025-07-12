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

#include <cstdint>

// -------- Regular cases --------

enum class D1 : unsigned {
  one = 1,
  two = 2,
  four = 4,
  eight = 8,
};

enum class D2 : int {
  Zero = 0,
  LING = 0,
  One = 1,
  Two = 2,
  YI = 1,
  ER = 2,
  Three = 3,
  Four = 4,
  Five = 5,
  Six = 6,
  Seven = 7,
  WU = 5,
  Liu = 6,
  Qi = 7,
  Eight = 8,
};

enum class D3 : uint8_t {
  FIRST = 0b0100'1001,
  SECOND = 0b1001'0010,
  THIRD = 0b0010'0100,
};

enum class D4 : int8_t {
  A = static_cast<int8_t>(0b0000'0011),
  B = static_cast<int8_t>(0b0000'1100),
  C = static_cast<int8_t>(0b0111'0000),
  D = static_cast<int8_t>(0b0000'1111),
  E = static_cast<int8_t>(0b1000'0000),
  F = static_cast<int8_t>(0b1111'0000),
  G = static_cast<int8_t>(0b1111'1100),
  H = static_cast<int8_t>(0b1111'0011),
  I = static_cast<int8_t>(0b1111'1111),
  J = static_cast<int8_t>(0b0111'0000),
  K = static_cast<int8_t>(0b0000'1111),
  L = static_cast<int8_t>(0b1111'0011),
  M = static_cast<int8_t>(0b1111'1111),
};

enum class D5 : int16_t {
  A = 1,
  B = 2,
  C = 3,
  D = 12,
  E = 96,
  F = -32768,
};

// -------- Irregular cases --------

enum class E1 : unsigned {
  A = 0b0001,
  B = 0b0011,
  C = 0b0111,
  D = 0b1111,
  E = 0b1100,
  F = 0b1000,
};

enum class E2 : int8_t {
  Zero = 0,
  A = 0b0000'0001,
  B = 0b0000'0010,
  C = 0b0000'0100,
  D = 0b0000'1000,
  E = 0b0001'1000,
  F = -128,
  G = -1,
};

// -------- Special cases --------

enum class empty {};

enum class single_one {
  value = 1,
};

enum class single_one_rep: uint8_t {
  the = 1,
  ONLY = 1,
  Value = 1,
  IS = 1,
  One = 1,
};

enum class single_zero {
  value = 0,
};

enum class single_zero_rep: unsigned {
  ONLY = 0,
  Zero = 0,
  inside = 0,
};
