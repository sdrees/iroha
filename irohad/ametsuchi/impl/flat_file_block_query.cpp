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

#include "ametsuchi/impl/flat_file_block_query.hpp"

#include <deque>
#include "crypto/hash.hpp"
#include "model/commands/add_asset_quantity.hpp"
#include "model/commands/transfer_asset.hpp"

namespace iroha {
  namespace ametsuchi {
    FlatFileBlockQuery::FlatFileBlockQuery(FlatFile &block_store)
        : block_store_(block_store) {}

    rxcpp::observable<model::Block> FlatFileBlockQuery::getBlocks(
        uint32_t height, uint32_t count) {
      auto to = height + count;
      auto last_id = block_store_.last_id();
      if (to > last_id) {
        to = last_id;
      }
      if (height > to) {
        return rxcpp::observable<>::empty<model::Block>();
      }
      return rxcpp::observable<>::range(height, to).flat_map([this](auto i) {
        auto bytes = block_store_.get(i);
        return rxcpp::observable<>::create<model::Block>([this, bytes](auto s) {
          if (not bytes.has_value()) {
            s.on_completed();
            return;
          }
          auto document =
              model::converters::stringToJson(bytesToString(bytes.value()));
          if (not document.has_value()) {
            s.on_completed();
            return;
          }
          auto block = serializer_.deserialize(document.value());
          if (not block.has_value()) {
            s.on_completed();
            return;
          }
          s.on_next(block.value());
          s.on_completed();
        });
      });
    }

    rxcpp::observable<model::Block> FlatFileBlockQuery::getBlocksFrom(
        uint32_t height) {
      return getBlocks(height, block_store_.last_id());
    }

    rxcpp::observable<model::Block> FlatFileBlockQuery::getTopBlocks(
        uint32_t count) {
      auto last_id = block_store_.last_id();
      if (count > last_id) {
        count = last_id;
      }
      return getBlocks(last_id - count + 1, count);
    }

    rxcpp::observable<model::Transaction>
    FlatFileBlockQuery::getAccountTransactions(
        const std::string& account_id, const model::Pager& pager) {
      //TODO 06/11/17 motxx: Improve API for BlockQueries for on-demand fetching
      std::deque<model::Transaction> reverser;
      getBlocksFrom(1)
        .flat_map([](auto block) {
          return rxcpp::observable<>::iterate(block.transactions);
        })
        .take_while([&pager](auto tx) { return iroha::hash(tx) != pager.tx_hash; })
        // filter txs by specified creator after take_while until tx_hash
        // to deal with other creator's tx_hash
        .filter([&account_id](auto tx) {
          return tx.creator_account_id == account_id;
        })
        // size of retrievable blocks and transactions should be restricted
        // in stateless validation.
        .take_last(pager.limit)
        // reverse transactions by pushing front of deque.
        .subscribe([&reverser](auto tx) { reverser.push_front(tx); });
      return rxcpp::observable<>::iterate(reverser);
    }

    bool FlatFileBlockQuery::hasAssetRelatedCommand(
        const std::string& account_id,
        const std::vector<std::string>& assets_id,
        const std::shared_ptr<iroha::model::Command>& command) const {

      const auto hasTransferAsset = searchCommand<model::TransferAsset>(
        command, [&account_id, &assets_id](const auto& transfer){
          const auto cond_src = transfer.src_account_id == account_id;
          const auto cond_dst = transfer.dest_account_id == account_id;
          if (not(cond_src) and not(cond_dst)) { return false; }
          // size of commands should be restricted in stateless validation.
          return std::any_of(
            assets_id.begin(), assets_id.end(),
            [&transfer](auto const &a) { return a == transfer.asset_id; });
        });

      const auto hasAddAssetQuantity = searchCommand<model::AddAssetQuantity>(
        command, [&account_id, &assets_id](const auto& add){
          const auto cond_account_id = add.account_id == account_id;
          const auto cond_has_asset = std::any_of(
            assets_id.begin(), assets_id.end(),
            [&add](auto const &a) { return a == add.asset_id; });
          return cond_account_id and cond_has_asset;
        });

      return hasTransferAsset or hasAddAssetQuantity;
    }

    rxcpp::observable<model::Transaction>
    FlatFileBlockQuery::getAccountAssetTransactions(
        const std::string& account_id,
        const std::vector<std::string>& assets_id,
        const model::Pager& pager) {
      //TODO 06/11/17 motxx: Improve API for BlockQueries for on-demand fetching
      std::deque<model::Transaction> reverser;
      getBlocksFrom(1)
        .flat_map([](auto block) {
          return rxcpp::observable<>::iterate(block.transactions);
        })
        // local variables can be captured because this observable will be
        // subscribed in this function.
        .take_while([&pager](auto tx) {
          return iroha::hash(tx) != pager.tx_hash;
        })
        .filter([this, &account_id, &assets_id](auto tx) {
          return std::any_of(
            tx.commands.begin(), tx.commands.end(),
            [this, &account_id, &assets_id](auto command){
              // This "this->" is required by gcc.
              return this->hasAssetRelatedCommand(account_id, assets_id, command);
            });
        })
        // size of retrievable blocks and transactions should be
        // restricted in stateless validation.
        .take_last(pager.limit)
        // reverse transactions by pushing front of deque.
        .subscribe([&reverser](auto tx) { reverser.push_front(tx); });
      return rxcpp::observable<>::iterate(reverser);
    }
  }  // namespace ametsuchi
}  // namespace iroha
