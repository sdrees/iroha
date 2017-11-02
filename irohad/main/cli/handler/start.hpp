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

#ifndef IROHA_MAIN_CLI_HANDLER_START_HPP_
#define IROHA_MAIN_CLI_HANDLER_START_HPP_

#include "main/application.hpp"

namespace iroha {
  namespace cli {
    namespace handler {

      inline void start(config::Postgres *pg,
                        config::Redis *rd,
                        config::BlockStorage *bs,
                        config::OtherOptions *other,
                        config::Cryptography *crypto,
                        config::Torii *torii) {
        auto log = logger::log("irohad");

        try {
          Application irohad;
          irohad.initStorage(*pg, *rd, *bs);
          irohad.initProtoFactories();
          irohad.initPeerQuery();
          irohad.initCryptoProvider(*crypto);
          irohad.initValidators();
          irohad.initOrderingGate(*other);
          irohad.initSimulator();
          irohad.initBlockLoader();
          irohad.initConsensusGate(*torii, *other);
          irohad.initSynchronizer();
          irohad.initPeerCommunicationService();
          irohad.initTransactionCommandService();
          irohad.initQueryService();
          log->info("initialized");

          irohad.run(*torii);

        } catch (const std::exception &e) {
          log->error("FATAL: {}", e.what());
        }
      }
    }
  }
}

#endif  //  IROHA_MAIN_CLI_HANDLER_START_HPP_
