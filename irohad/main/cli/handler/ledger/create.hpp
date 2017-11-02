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

#ifndef IROHA_MAIN_CLI_LEDGER_CREATE_HPP_
#define IROHA_MAIN_CLI_LEDGER_CREATE_HPP_

#include "main/cli/config.hpp"

namespace iroha {
  namespace cli {
    namespace handler {
      namespace ledger {

        void create(
            config::Postgres *pg,
            config::Redis *rd,
            config::BlockStorage *bs,
            const std::string& genesis_content
        ) {
          // TODO: implement
          // raw block inserter?
          printf("genesis block content: \n%s", genesis_content.c_str());
        }

      }
    }
  }
}

#endif //  IROHA_MAIN_CLI_LEDGER_CREATE_HPP_
