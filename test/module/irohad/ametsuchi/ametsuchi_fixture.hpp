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

#ifndef IROHA_AMETSUCHI_FIXTURE_HPP
#define IROHA_AMETSUCHI_FIXTURE_HPP

#include "common/files.hpp"
#include "logger/logger.hpp"

#include <gtest/gtest.h>
#include <cpp_redis/cpp_redis>
#include <pqxx/pqxx>
#include "main/env-vars.hpp"
#include "util/string.hpp"
#include "main/common.hpp"

using namespace std::literals::string_literals;
using iroha::string::parseEnv;
#define LOCALHOST "127.0.0.1"s
#define ALLHOST "0.0.0.0"s

namespace iroha {
  namespace ametsuchi {
    /**
     * Class with ametsuchi initialization
     */
    class AmetsuchiTest : public ::testing::Test {
     public:
      iroha::config::Redis redis;
      iroha::config::Postgres postgres;
      iroha::config::BlockStorage storage;

     protected:
      virtual void SetUp() {
        auto log = logger::testLog("AmetsuchiTest");

        redis.host = parseEnv(IROHA_RDHOST, LOCALHOST);
        redis.port = parseEnv(IROHA_RDPORT, 6379);

        postgres.host = parseEnv(IROHA_PGHOST, LOCALHOST);
        postgres.port = parseEnv(IROHA_PGPORT, 5432);
        postgres.database = parseEnv(IROHA_PGDATABASE, "iroha"s);
        postgres.username = parseEnv(IROHA_PGUSER, "postgres"s);
        postgres.password = parseEnv(IROHA_PGPASSWORD, "mysecretpassword"s);

        storage.path = parseEnv(IROHA_BLOCKSPATH, "/tmp/blocks"s);

        mkdir(storage.path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      }
      virtual void TearDown() {
        const auto drop = R"(
DROP TABLE IF EXISTS account_has_signatory;
DROP TABLE IF EXISTS account_has_asset;
DROP TABLE IF EXISTS role_has_permissions;
DROP TABLE IF EXISTS account_has_roles;
DROP TABLE IF EXISTS account_has_grantable_permissions;
DROP TABLE IF EXISTS account;
DROP TABLE IF EXISTS asset;
DROP TABLE IF EXISTS domain;
DROP TABLE IF EXISTS signatory;
DROP TABLE IF EXISTS peer;
DROP TABLE IF EXISTS role;
)";

        pqxx::connection connection(this->postgres.options());
        pqxx::work txn(connection);
        txn.exec(drop);
        txn.commit();
        connection.disconnect();

        cpp_redis::redis_client client;
        client.connect(this->redis.host, this->redis.port);
        client.flushall();
        client.sync_commit();
        client.disconnect();

        iroha::remove_all(this->storage.path);
      }
    };
  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_AMETSUCHI_FIXTURE_HPP
