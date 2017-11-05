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

#include "ametsuchi/impl/storage_impl.hpp"
#include "ametsuchi/storage.hpp"
#include "cli/common.hpp"
#include "main/raw_block_insertion.hpp"
#include "util/filesystem.hpp"

namespace iroha {
  namespace cli {
    namespace handler {
      namespace ledger {

        inline void create(ametsuchi::config::Ametsuchi *am,
                           const std::string &genesis_path) {
          BOOST_ASSERT_MSG(not genesis_path.empty(),
                           "content of genesis_path is empty");

          auto log = logger::log("create");
          auto storage = ametsuchi::StorageImpl::create(*am);

          auto genesis_content = filesystem::read_file(genesis_path);
          if(not genesis_content){
            log->error("Failed to read genesis block");
            exit(EXIT_FAILURE);
          }

          iroha::main::BlockInserter inserter(storage);
          auto block = inserter.parseBlock(genesis_content.value());
          if (not block) {
            log->error("Failed to parse genesis block");
            exit(EXIT_FAILURE);
          }

          // TODO(@warchant): what if create was called twice?
          log->info("Block is parsed");
          inserter.applyToLedger({block.value()});
          log->info("Genesis block inserted, number of transactions: {}",
                    block.value().transactions.size());
        }
      }
    }
  }
}

#endif  //  IROHA_MAIN_CLI_LEDGER_CREATE_HPP_
