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

#ifndef IROHA_MAIN_COMMON_HPP_
#define IROHA_MAIN_COMMON_HPP_

#include <chrono>
#include <sstream>
#include <string>
#include "cli/defaults.hpp"

namespace iroha {
  namespace config {

    /**
     * @struct Service
     * @brief Network service.
     */
    struct Service {
      std::string host;
      uint16_t port;

      std::string listenAddress() const noexcept {
        return this->host + ":" + std::to_string(this->port);
      }
    };

    /**
     * @struct AuthService
     * @brief Network service with authentication.
     */
    struct AuthService : public Service {
      std::string username;
      std::string password;
    };

    /**
       * @struct Cryptography
       * @brief Everything that is required for cryptography is here.
       */
    struct Cryptography {
      std::string public_key;   ///< content of the public key
      std::string private_key;  ///< content of the private key
    };

    struct OtherOptions {
      OtherOptions()
          : max_proposal_size(defaults::proposalSize),
            proposal_delay(defaults::proposalDelay),
            vote_delay(defaults::voteDelay),
            load_delay(defaults::loadDelay) {}

      size_t max_proposal_size;
      std::chrono::milliseconds proposal_delay;
      std::chrono::milliseconds vote_delay;
      std::chrono::milliseconds load_delay;
    };

    /**
     * @struct Peer
     * @brief Config data for peer connection: consensus, block loader, ordering
     * service.
     */
    struct Peer : public Service {
      Peer() {
        host = defaults::peerHost;
        port = defaults::peerPort;
      }
    };
  }  // namespace config
}  // namespace iroha

#endif  //  IROHA_MAIN_COMMON_HPP_
