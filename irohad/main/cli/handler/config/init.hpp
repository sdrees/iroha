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

#ifndef IROHA_MAIN_CLI_CONFIG_INIT_HPP_
#define IROHA_MAIN_CLI_CONFIG_INIT_HPP_

#include <iostream>
#include <sstream>
#include "main/cli/config.hpp"
#include "main/cli/env-vars.hpp"

namespace iroha {
  namespace cli {
    namespace handler {
      namespace config {

        namespace util {
          template <typename T>
          std::string make_env_str(const std::string &key, T value) {
            std::stringstream ss;
            ss << "export " << key << "=" << value;
            return ss.str();
          }
        }

        void init(iroha::config::Postgres *pg,
                  iroha::config::Redis *rd,
                  iroha::config::BlockStorage *bs,
                  iroha::config::OtherOptions *other,
                  iroha::config::Cryptography *crypto,
                  iroha::config::Torii *torii) {
          using util::make_env_str;
          std::stringstream ss;
          ss  // postgres
              << make_env_str(IROHA_PGHOST, pg->host) << '\n'
              << make_env_str(IROHA_PGPORT, pg->port) << '\n'
              << make_env_str(IROHA_PGDATABASE, pg->database) << '\n'
              << make_env_str(IROHA_PGUSER, pg->username) << '\n'
              << make_env_str(IROHA_PGPASSWORD, pg->password) << '\n'
              // redis
              << make_env_str(IROHA_RDHOST, rd->host) << '\n'
              << make_env_str(IROHA_RDPORT, rd->port) << '\n'
              // block storage
              << make_env_str(IROHA_BLOCKSPATH, bs->path) << '\n'
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
              << make_env_str(IROHA_TORII_PORT, torii->port) << '\n';

          std::cout << ss.str();
        }
      }
    }
  }
}

#endif  //  IROHA_MAIN_CLI_CONFIG_INIT_HPP_
