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

#ifndef IROHA_UTIL_HPP_
#define IROHA_UTIL_HPP_

#include <cstdio>  // for size_t
#include <string>

namespace iroha {
  namespace secure {

    /**
     * @brief Secure erase of memory.
     * Guarantees that given buffer will contain zeroes after function completes.
     * @param[in] dest memory to be erased.
     * @param[in] size size of memory in bytes.
     */
    void erase(void *dest, size_t size);

    /** @brief Secure move from src to dest.
     * Copies data first and then securely erases src.
     * Destination memory should be at least same size as source.
     * @param[out] dest destination memory
     * @param[in/out] src sources memory, will be erased at the end
     * @param size size ofs src memory in bytes
     */
    void move(void *dest, void *src, size_t size);

    /**
     * @brief Secure compare of two chunks of memory.
     * Resistant to timing attacks.
     * @param lhs
     * @param rhs
     * @param size both chunks should be the same size.
     * @return true if memory chunks are equal, false otherwise.
     */
    bool compare(void *lhs, void *rhs, size_t size);
  }  // namespace secure
}  // namespace iroha

#endif  //  IROHA_UTIL_HPP_
