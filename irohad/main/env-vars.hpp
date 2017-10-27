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

#ifndef IROHA_ENV_VARS_HPP_
#define IROHA_ENV_VARS_HPP_

// peer flags
#define IROHA_HOST "IROHA_HOST"
#define IROHA_PORT "IROHA_PORT"
#define IROHA_PUBKEY "IROHA_PUBKEY"
#define IROHA_PRIVKEY "IROHA_PRIVKEY"

// postgres flags
#define IROHA_PGHOST "IROHA_PGHOST"
#define IROHA_PGPORT "IROHA_PGPORT"
#define IROHA_PGUSER "IROHA_PGUSER"
#define IROHA_PGPASSWORD "IROHA_PGPASSWORD"
#define IROHA_PGDATABASE "IROHA_PGDATABASE"

// redis flags
#define IROHA_RDHOST "IROHA_RDHOST"
#define IROHA_RDPORT "IROHA_RDPORT"

// block storage flags
#define IROHA_BLOCKSPATH "IROHA_BLOCKSPATH"

// other
#define IROHA_OTHER_LOADDELAY "IROHA_OTHER_LOADDELAY"
#define IROHA_OTHER_VOTEDELAY "IROHA_OTHER_VOTEDELAY"
#define IROHA_OTHER_PROPOSALDELAY "IROHA_OTHER_PROPOSALDELAY"
#define IROHA_OTHER_PROPOSALSIZE "IROHA_OTHER_PROPOSALSIZE"

#endif //  IROHA_ENV_VARS_HPP_
