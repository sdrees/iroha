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

#ifndef IROHA_TORII_CONFIG_HPP_
#define IROHA_TORII_CONFIG_HPP_

#include "cli/common.hpp"

namespace iroha {
  namespace torii {
    namespace config {
      /**
       * @struct Torii
       * @brief Config for torii gateway -- client API provider.
       */
      struct Torii : public iroha::config::Service {
        Torii() {
          host = defaults::toriiHost;
          port = defaults::toriiPort;
        }
      };
    }
  }
}

#endif  //  IROHA_TORII_CONFIG_HPP_
