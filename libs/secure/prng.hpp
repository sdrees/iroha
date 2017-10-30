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

#ifndef IROHA_PRNG_HPP_
#define IROHA_PRNG_HPP_

#include <algorithm>
#include <boost/assert.hpp>
#include <functional>
#include <random>
#include "secure/detail/base_prng.hpp"

namespace iroha {
  namespace secure {

    /**
     * @class prng
     * @brief Pseudo Random Number Generator, seeded by random uint64
     * @tparam Distribution.
     */
    template <typename Distribution>
    class PRNG final: public BasePRNG {
     public:
      using result_type = typename Distribution::result_type;

      /**
       * @brief Constructor. PRNG returns values in range [min, max)
       * @param min
       * @param max
       */
      PRNG(uint64_t seed = BasePRNG::true_random_uint64()) : BasePRNG(seed) {}

      /**
       * @brief Get random number of 'IntType'.
       * @return random integer number with uniform distribution.
       */
      result_type get() { return dis(tw); }

      /**
       * @brief Get 'size' random numbers.
       * @param size the size of returned vector.
       * @return vector of random numbers.
       */
      std::vector<result_type> get(size_t size) {
        std::vector<result_type> v(size);
        std::generate_n(v.begin(), size, [this]() { return dis(tw); });
        BOOST_ASSERT_MSG(v.size() == size, "bad vector size");
        return v;
      }

     private:
      Distribution dis;
    };

  }  // namespace secure
}  // namespace iroha

#endif  //  IROHA_PRNG_HPP_
