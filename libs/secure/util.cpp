/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "secure/util.hpp"

namespace util {
  /**
   * cast void ptr to volatile ptr of volatile T
   */
  template <typename T>
  inline constexpr T volatile *volatile void2volatileptr(void *mem) {
    return const_cast<volatile T *>(reinterpret_cast<T *>(mem));
  }
}

void erase(void *dest, size_t size) {
  char volatile *volatile p = util::void2volatileptr<char>(dest);

  if (size == 0) {
    return;
  }

  do {
    *p = 0;
  } while (--size);
}

void move(void *dest, void *src, size_t size) {
  auto to = util::void2volatileptr<char>(dest);
  auto from = util::void2volatileptr<char>(src);
  auto count = size;

  if (size == 0) {
    return;
  }

  do {
    *to++ = *from++;
  } while (--count);

  erase(src, size);
}

bool compare(void *lhs, void *rhs, size_t size) {
  auto a = util::void2volatileptr<char>(lhs);
  auto b = util::void2volatileptr<char>(rhs);

  size_t s = 0;
  do {
    s |= (*a) ^ (*b);
  } while (--size);

  return s == 0;
}

bool compare(std::string const &lhs, std::string const &rhs) {
  return lhs.size() == rhs.size()
      && compare((void *)lhs.data(), (void *)rhs.data(), lhs.size());
}
