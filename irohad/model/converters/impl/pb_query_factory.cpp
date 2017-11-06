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

#include "model/converters/pb_query_factory.hpp"
#include <queries.pb.h>
#include "common/types.hpp"
#include "crypto/hash.hpp"
#include "model/common.hpp"
#include "model/queries/get_account.hpp"
#include "model/queries/get_account_assets.hpp"
#include "model/queries/get_asset_info.hpp"
#include "model/queries/get_roles.hpp"
#include "model/queries/get_signatories.hpp"
#include "model/queries/get_account_transactions.hpp"
#include "model/queries/get_account_asset_transactions.hpp"

namespace iroha {
  namespace model {
    namespace converters {

      PbQueryFactory::PbQueryFactory() {
        log_ = logger::log("PbQueryFactory");
        serializers_[typeid(GetAccount)] = &PbQueryFactory::serializeGetAccount;
        serializers_[typeid(GetAccountAssets)] =
            &PbQueryFactory::serializeGetAccountAssets;
        serializers_[typeid(GetAccountTransactions)] =
            &PbQueryFactory::serializeGetAccountTransactions;
        serializers_[typeid(GetAccountAssetTransactions)] =
            &PbQueryFactory::serializeGetAccountAssetTransactions;
        serializers_[typeid(GetSignatories)] =
            &PbQueryFactory::serializeGetSignatories;
        serializers_[typeid(GetRolePermissions)] =
            &PbQueryFactory::serializeGetRolePermissions;
        serializers_[typeid(GetAssetInfo)] =
            &PbQueryFactory::serializeGetAssetInfo;
        serializers_[typeid(GetRoles)] = &PbQueryFactory::serializeGetRoles;
      }

      // Specific deserializer
      model::Pager PbQueryFactory::deserializePager(
          const protocol::Pager &pb_pager) const {
        model::Pager pager;
        if (const auto byte_tx_hash =
          iroha::hexstringToBytestring(pb_pager.tx_hash())) {
          pager.tx_hash.from_string(*byte_tx_hash);
        } else {
          pager.tx_hash.fill(0);
        }
        pager.limit = static_cast<decltype(pager.limit)>(pb_pager.limit());
        return pager;
      }

      optional_ptr<model::Query> PbQueryFactory::deserialize(
          const protocol::Query &pb_query) const {
        std::shared_ptr<model::Query> val;

        const auto &pl = pb_query.payload();

        {
          using protocol::Query_Payload;
          switch (pl.query_case()) {
            case Query_Payload::QueryCase::kGetAccount: {
              // Convert to get Account
              const auto &pb_cast = pl.get_account();
              auto account_query = GetAccount();
              account_query.account_id = pb_cast.account_id();
              val = std::make_shared<model::GetAccount>(account_query);
              break;
            }
            case Query_Payload::QueryCase::kGetAccountAssets: {
              // Convert to get Account Asset
              const auto &pb_cast = pl.get_account_assets();
              auto query = GetAccountAssets();
              query.account_id = pb_cast.account_id();
              query.asset_id = pb_cast.asset_id();
              val = std::make_shared<model::GetAccountAssets>(query);
              break;
            }
            case Query_Payload::QueryCase::kGetAccountSignatories: {
              // Convert to get Signatories
              const auto &pb_cast = pl.get_account_signatories();
              auto query = GetSignatories();
              query.account_id = pb_cast.account_id();
              val = std::make_shared<model::GetSignatories>(query);
              break;
            }
            case Query_Payload::QueryCase::kGetAccountTransactions: {
              // Convert to get Signatories
              const auto &pb_cast = pl.get_account_transactions();
              auto query = GetAccountTransactions();
              query.account_id = pb_cast.account_id();
              query.pager = deserializePager(pb_cast.pager());
              val = std::make_shared<model::GetAccountTransactions>(
                query);
              break;
            }
            case Query_Payload::QueryCase::
              kGetAccountAssetTransactions: {
              const auto &pb_cast =
                pl.get_account_asset_transactions();
              auto query = GetAccountAssetTransactions();
              query.account_id = pb_cast.account_id();
              std::copy(pb_cast.assets_id().begin(),
                        pb_cast.assets_id().end(),
                        std::back_inserter(query.assets_id));
              query.pager = deserializePager(pb_cast.pager());
              val = std::make_shared<
                model::GetAccountAssetTransactions>(query);
              break;
            }
            case Query_Payload::QueryCase::kGetRoles: {
              // Convert to get Roles
              val = std::make_shared<GetRoles>();
              break;
            }
            case Query_Payload::QueryCase::kGetAssetInfo: {
              // Convert to get asset info
              const auto &pb_cast = pl.get_asset_info();
              val = std::make_shared<GetAssetInfo>(pb_cast.asset_id());
              break;
            }
            case Query_Payload::QueryCase::kGetRolePermissions: {
              const auto &pb_cast = pl.get_role_permissions();
              val = std::make_shared <GetRolePermissions>(pb_cast.role_id());
              break;
            }
            default: {
              // Query not implemented
              return nonstd::nullopt;
            }
          }
        }

        const auto &pb_sign = pb_query.signature();

        Signature sign{};
        sign.pubkey = pubkey_t::from_string(pb_sign.pubkey());
        sign.signature = sig_t::from_string(pb_sign.signature());

        val->query_counter = pl.query_counter();
        val->signature = sign;
        val->created_ts = pl.created_time();
        val->creator_account_id = pl.creator_account_id();
        return val;
      }

      void PbQueryFactory::serializeQueryMetaData(
          protocol::Query &pb_query, std::shared_ptr<const Query> query) const {
        auto pl = pb_query.mutable_payload();
        pl->set_creator_account_id(query->creator_account_id);
        pl->set_created_time(query->created_ts);
        // Set signatures
        auto sig = pb_query.mutable_signature();
        sig->set_signature(query->signature.signature.to_string());
        sig->set_pubkey(query->signature.pubkey.to_string());
      }

      nonstd::optional<protocol::Query> PbQueryFactory::serialize(
          std::shared_ptr<const Query> query) const {
        auto it = serializers_.find(typeid(*query));
        if (it != serializers_.end()) {
          return (this->*it->second)(query);
        }
        log_->error("Query type not found");
        return nonstd::nullopt;
      }

      protocol::Query PbQueryFactory::serializeGetAccount(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        auto pl = pb_query.mutable_payload();
        serializeQueryMetaData(pb_query, query);
        auto tmp = std::static_pointer_cast<const GetAccount>(query);
        auto account_id = tmp->account_id;
        auto pb_query_mut = pl->mutable_get_account();
        pb_query_mut->set_account_id(account_id);
        return pb_query;
      };

      protocol::Query PbQueryFactory::serializeGetAccountAssets(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        serializeQueryMetaData(pb_query, query);
        auto tmp = std::static_pointer_cast<const GetAccountAssets>(query);
        auto pb_query_mut =
            pb_query.mutable_payload()->mutable_get_account_assets();
        pb_query_mut->set_account_id(tmp->account_id);
        pb_query_mut->set_asset_id(tmp->asset_id);
        return pb_query;
      };

      protocol::Query PbQueryFactory::serializeGetAccountTransactions(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        serializeQueryMetaData(pb_query, query);
        auto tmp =
            std::static_pointer_cast<const GetAccountTransactions>(query);
        auto pb_query_mut =
            pb_query.mutable_payload()->mutable_get_account_transactions();
        pb_query_mut->set_account_id(tmp->account_id);
        auto pb_pager = pb_query_mut->mutable_pager();
        pb_pager->set_tx_hash(tmp->pager.tx_hash.to_hexstring());
        pb_pager->set_limit(tmp->pager.limit);
        return pb_query;
      }

      protocol::Query PbQueryFactory::serializeGetAccountAssetTransactions(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        serializeQueryMetaData(pb_query, query);
        auto tmp =
            std::static_pointer_cast<const GetAccountAssetTransactions>(query);
        auto account_id = tmp->account_id;
        auto assets_id = tmp->assets_id;
        auto pb_query_mut = pb_query.mutable_payload()
                                ->mutable_get_account_asset_transactions();
        pb_query_mut->set_account_id(account_id);
        std::for_each(assets_id.begin(), assets_id.end(), [&pb_query_mut](auto id) {
          (*pb_query_mut->add_assets_id()) = id;
        });
        auto pb_pager = pb_query_mut->mutable_pager();
        pb_pager->set_tx_hash(tmp->pager.tx_hash.to_hexstring());
        pb_pager->set_limit(tmp->pager.limit);
        return pb_query;
      }

      protocol::Query PbQueryFactory::serializeGetSignatories(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        serializeQueryMetaData(pb_query, query);
        auto tmp = std::static_pointer_cast<const GetSignatories>(query);
        auto pb_query_mut =
            pb_query.mutable_payload()->mutable_get_account_signatories();
        pb_query_mut->set_account_id(tmp->account_id);
        return pb_query;
      }

      protocol::Query PbQueryFactory::serializeGetAssetInfo(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        serializeQueryMetaData(pb_query, query);
        auto tmp = std::static_pointer_cast<const GetAssetInfo>(query);
        auto ast_id = tmp->asset_id;
        auto pb_query_mut =
            pb_query.mutable_payload()->mutable_get_asset_info();
        pb_query_mut->set_asset_id(ast_id);
        return pb_query;
      }

      protocol::Query PbQueryFactory::serializeGetRoles(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        pb_query.mutable_payload()->mutable_get_roles();
        serializeQueryMetaData(pb_query, query);
        auto tmp = std::static_pointer_cast<const GetRoles>(query);
        return pb_query;
      }

      protocol::Query PbQueryFactory::serializeGetRolePermissions(
          std::shared_ptr<const Query> query) const {
        protocol::Query pb_query;
        serializeQueryMetaData(pb_query, query);
        auto tmp = std::static_pointer_cast<const GetRolePermissions>(query);
        auto role = tmp->role_id;
        auto pb_query_mut =
            pb_query.mutable_payload()->mutable_get_role_permissions();
        pb_query_mut->set_role_id(role);
        return pb_query;
      }

    }  // namespace converters
  }    // namespace model
}  // namespace iroha
