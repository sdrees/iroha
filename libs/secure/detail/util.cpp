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
#include <cstring>

namespace iroha {
  namespace secure {

    void erase(void *dest, size_t size) {
      // this construction discards compiler optimizations.
      // function erase is guaranteed to be executed.
      auto volatile *volatile p =
          const_cast<volatile char *>(reinterpret_cast<char *>(dest));

      if (size == 0) {
        return;
      }

      do {
        *p = 0;
      } while (*p++ == 0 && --size);
    }

    void move(void *dest, void *src, size_t size) {
      auto volatile *volatile to =
          const_cast<volatile char *>(reinterpret_cast<char *>(dest));
      auto volatile *volatile from =
          const_cast<volatile char *>(reinterpret_cast<char *>(src));
      auto count = size;

      if (size == 0) {
        return;
      }

      do {
        *to++ = *from++;
      } while (--count);

      secure::erase(src, size);
    }

    bool compare(void *lhs, void *rhs, size_t size) {
      auto volatile *volatile a =
          const_cast<volatile char *>(reinterpret_cast<char *>(lhs));
      auto volatile *volatile b =
          const_cast<volatile char *>(reinterpret_cast<char *>(rhs));

      size_t s = 0;
      do {
        s |= (*a++) ^ (*b++);
      } while (--size);

      return s == 0;
    }
  }
}
