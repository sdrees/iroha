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

#ifndef IROHA_FLAT_FILE_BLOCK_QUERY_HPP
#define IROHA_FLAT_FILE_BLOCK_QUERY_HPP

#include "ametsuchi/block_query.hpp"

#include "ametsuchi/impl/flat_file/flat_file.hpp"
#include "model/converters/json_block_factory.hpp"
#include "model/queries/get_account_transactions.hpp"
#include "model/queries/get_account_asset_transactions.hpp"

namespace iroha {
  namespace ametsuchi {
    class FlatFileBlockQuery : public BlockQuery {
     public:
      explicit FlatFileBlockQuery(FlatFile &block_store);

      rxcpp::observable<model::Block> getBlocks(uint32_t height,
                                                uint32_t count) override;

      rxcpp::observable<model::Block> getBlocksFrom(uint32_t height) override;

      rxcpp::observable<model::Block> getTopBlocks(uint32_t count) override;

      rxcpp::observable<model::Transaction> getAccountTransactions(
        const std::string& account_id, const model::Pager& pager) override;

      rxcpp::observable<model::Transaction> getAccountAssetTransactions(
        const std::string& account_id,
        const std::vector<std::string>& assets_id,
        const model::Pager& pager) override;

     protected:
      FlatFile &block_store_;
      model::converters::JsonBlockFactory serializer_;

     private:
      template <typename CommandType, typename Predicate>
      bool searchCommand(const std::shared_ptr<iroha::model::Command>& command,
                         const Predicate& predicate) const {
        if (const auto p = std::dynamic_pointer_cast<CommandType>(command)) {
          return predicate(*p);
        }
        return false;
      }

      bool hasAssetRelatedCommand(
        const std::string& account_id,
        const std::vector<std::string>& assets_id,
        const std::shared_ptr<iroha::model::Command>& command) const;
    };
  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_FLAT_FILE_BLOCK_QUERY_HPP
