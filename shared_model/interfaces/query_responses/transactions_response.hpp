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

#ifndef IROHA_SHARED_MODEL_TRANSACTIONS_RESPONSE_HPP
#define IROHA_SHARED_MODEL_TRANSACTIONS_RESPONSE_HPP

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <rxcpp/rx-observable.hpp>
#include "interfaces/common_objects/types.hpp"
#include "interfaces/polymorphic_wrapper.hpp"
#include "interfaces/primitive.hpp"
#include "interfaces/transaction.hpp"
#include "interfaces/visitor_apply_for_all.hpp"
#include "model/queries/responses/transactions_response.hpp"
#include "utils/string_builder.hpp"

namespace shared_model {
  namespace interface {
    /**
     * Container of asset, for fetching data.
     */
    class TransactionsResponse
        : public Primitive<TransactionsResponse,
                           iroha::model::TransactionsResponse> {
     public:
      /// Type of a single Transaction
      using TransactionType = detail::PolymorphicWrapper<Transaction>;

      /// Type of transactions' collection
      using TransactionsCollectionType = std::vector<TransactionType>;

      /**
       * @return Attached transactions
       */
      virtual TransactionsCollectionType transactions() const = 0;

      /**
       * Stringify the data.
       * @return string representation of data.
       */
      std::string toString() const override {
        return detail::PrettyStringBuilder()
            .init("TransactionsResponse")
            .append("transactions")
            .appendAll(transactions(), [](auto &tx) { return tx->toString(); })
            .finalize();
      }

      /**
       * @return true if the data are same.
       */
      bool operator==(const ModelType &rhs) const override {
        return transactions() == rhs.transactions();
      }

      /**
       * Makes old model.
       * @return An allocated old model of transactions response.
       */
      OldModelType *makeOldModel() const override {
        OldModelType *oldModel = new OldModelType();
        oldModel->transactions = rxcpp::observable<>::iterate([this] {
          using OldTxType = iroha::model::Transaction;
          std::vector<OldTxType> ret;
          const auto txs = transactions();
          std::for_each(txs.begin(), txs.end(), [&ret](const auto &tx) {
            auto p = std::shared_ptr<OldTxType>(tx->makeOldModel());
            ret.push_back(*p);
          });
          return ret;
        }());
        return oldModel;
      }
    };
  }  // namespace interface
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_TRANSACTIONS_RESPONSE_HPP
