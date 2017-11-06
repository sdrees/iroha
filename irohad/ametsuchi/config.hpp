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

#ifndef IROHA_AMETSUCHI_CONFIG_HPP_
#define IROHA_AMETSUCHI_CONFIG_HPP_

#include <sstream>
#include <string>
#include "cli/common.hpp"

namespace iroha {
  namespace ametsuchi {
    namespace config {

      /**
       * Config for Redis.
       */
      struct Redis : public iroha::config::Service {
        Redis() {
          host = defaults::redisHost;
          port = defaults::redisPort;
        }
      };

      /**
       * @struct Postgres
       * @brief Postgres config defined here.
       */
      struct Postgres : public iroha::config::AuthService {
        Postgres() {
          host = defaults::postgresHost;
          port = defaults::postgresPort;
        }

        std::string database;

        /**
         * Returns preformatted "options"
         */
        std::string options() const noexcept {
          std::stringstream ss;

          if (not database.empty()) {
            ss << "dbname=" << database << " ";
          }

          ss << "host=" << host << " port=" << port << " user=" << username
             << " password=" << password;

          return ss.str();
        }
      };

      /**
       * Config for a block storage.
       *
       * @note: there may be a config for NuDB or something else.
       */
      struct BlockStorage {
        BlockStorage() : path(defaults::blockStoragePath) {}

        std::string path;  ///< path to the folder with blocks
      };

      /**
       * Aggregate type for ametsuchi configuration.
       */
      struct Ametsuchi {
        Redis redis;
        Postgres postgres;
        BlockStorage blockStorage;
      };
    }
  }
}

#endif  //  IROHA_AMETSUCHI_CONFIG_HPP_
