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

#ifndef IROHA_SECURE_BASE_PRNG_HPP_
#define IROHA_SECURE_BASE_PRNG_HPP_

#include <random>

namespace iroha {
  namespace secure {

    class BasePRNG {
     public:
      BasePRNG(uint64_t seed = true_random_uint64()) : tw(seed) {}

      /**
       * @brief Cryptographically secure true random 64 bit generator, which can
       * be used for seeding. 32-bit seeds are not safe to use.
       * @return 64 bits of true randomness.
       */
      static uint64_t true_random_uint64() {
        // rd() returns only 32 bits, we want 64.
        uint64_t seed = rd();        // top 32 random bits
        seed = (seed << 32) | rd();  // bottom 32 random bits
        return seed;
      }

     protected:
      std::mt19937_64 tw;
      static std::random_device rd;
    };

    std::random_device BasePRNG::rd{};
  }
}

#endif  //  IROHA_SECURE_BASE_PRNG_HPP_
