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

#ifndef IROHA_NETWORK_HPP_
#define IROHA_NETWORK_HPP_

#include <cstdint>
#include <limits>
#include <string>
#include "string.hpp"

namespace iroha {
  namespace network {

    /**
     * @return true if 2^16 > port > 0, false otherwise.
     */
    constexpr inline bool is_port_valid(uint64_t port) {
      return port > 0 && port < 65536 /* 1 << 16 */;
    }

    /**
     * It is not possible to determine all possible types of hostnames, since
     * they may be localized (domain .рф in Russia is example).
     *
     * @todo probably it is good to resolve a hostname to determine if it is valid or not
     * @return true if given string looks like a domain name or ip.
     */
    inline bool is_host_valid(const std::string &host) {
      return !host.empty() && host.size() <= 253 /* max size is 253 for domains */;
    }

  } //   namespace network
} //   namespace iroha

#endif  //  IROHA_NETWORK_HPP_
