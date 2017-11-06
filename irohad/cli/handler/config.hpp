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

#ifndef IROHA_CLI_HANDLER_CONFIG_INIT_HPP_
#define IROHA_CLI_HANDLER_CONFIG_INIT_HPP_

#include "ametsuchi/config.hpp"
#include "cli/common.hpp"
#include "torii/config.hpp"

namespace iroha {
  namespace cli {
    namespace handler {
      namespace config {

        namespace util {

          /**
           * Performs preformatting from given key, value to environment
           * variable format:
           * export KEY=value
           */
          template <typename T>
          std::string make_env_str(const std::string &key, T value) {
            std::stringstream ss;
            ss << "export " << key << "=" << value;
            return ss.str();
          }
        }

        /**
         * Usage:
         * irohad --flag1=a config > env.sh
         * source env.sh
         *
         * Now config is applied.
         */
        inline void config(const ametsuchi::config::Ametsuchi *am,
                           const iroha::config::Cryptography *crypto,
                           const iroha::config::OtherOptions *other,
                           const iroha::config::Peer *peer,
                           const torii::config::Torii *torii) {
          using util::make_env_str;
          std::stringstream ss;

          ss  // postgres
              << make_env_str(IROHA_PGHOST, am->postgres.host) << '\n'
              << make_env_str(IROHA_PGPORT, am->postgres.port) << '\n'
              << make_env_str(IROHA_PGDATABASE, am->postgres.database) << '\n'
              << make_env_str(IROHA_PGUSER, am->postgres.username) << '\n'
              << make_env_str(IROHA_PGPASSWORD, am->postgres.password) << '\n'
              // redis
              << make_env_str(IROHA_RDHOST, am->redis.host) << '\n'
              << make_env_str(IROHA_RDPORT, am->redis.port) << '\n'
              // block storage
              << make_env_str(IROHA_BLOCKSPATH, am->blockStorage.path) << '\n'
              // other options
              << make_env_str(IROHA_OTHER_LOADDELAY, other->load_delay.count())
              << '\n'
              << make_env_str(IROHA_OTHER_VOTEDELAY, other->vote_delay.count())
              << '\n'
              << make_env_str(IROHA_OTHER_PROPOSALDELAY,
                              other->proposal_delay.count())
              << '\n'
              << make_env_str(IROHA_OTHER_PROPOSALSIZE,
                              other->max_proposal_size)
              << '\n'
              // cryptography
              << make_env_str(IROHA_PEER_PUBKEY, crypto->public_key) << '\n'
              << make_env_str(IROHA_PEER_PRIVKEY, crypto->private_key) << '\n'
              // torii
              << make_env_str(IROHA_TORII_HOST, torii->host) << '\n'
              << make_env_str(IROHA_TORII_PORT, torii->port) << '\n'
              // peer
              << make_env_str(IROHA_PEER_HOST, peer->host) << '\n'
              << make_env_str(IROHA_PEER_PORT, peer->port) << '\n';

          std::cout << ss.str();
          std::exit(EXIT_SUCCESS);
        }
      }
    }
  }
}

#endif  //  IROHA_CLI_HANDLER_CONFIG_INIT_HPP_
