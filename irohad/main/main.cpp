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

#include <boost/filesystem.hpp>

#include "main/application.hpp"
#include "main/flags.hpp"

using namespace iroha;
using std::literals::string_literals::operator""s;

#define ALLHOST "0.0.0.0"s
#define LOCALHOST "localhost"s

#ifdef IROHA_VERSION
// it is a preprocessor trick, which converts X to "X" (const char*)
// refer to https://stackoverflow.com/a/240370/1953079
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define IROHA_VERSION_STR STRINGIFY(IROHA_VERSION)
#else
#define IROHA_VERSION_STR "undefined"
#endif

int main(int argc, char *argv[]) {
  auto log = logger::log("MAIN");
  CLI::App main("iroha - simple decentralized ledger");
  main.require_subcommand(1);
  main.add_flag("-v,--version"s,
                [&argv, &main](size_t) {
                  // note (@warchant): do not use logger here, it
                  // looks ugly.
                  std::cout << argv[0] << " version " << IROHA_VERSION_STR;
                  exit(0);
                },
                "Current version"s);

  /// DEFAULTS
  config::Torii torii{};
  torii.host = ALLHOST;
  torii.port = 50051;

  config::OtherOptions other{};
  other.load_delay = 5000;
  other.vote_delay = 5000;
  other.proposal_delay = 5000;
  other.max_proposal_size = 10;

  config::Redis redis{};
  redis.host = LOCALHOST;
  redis.port = 6379;

  config::Postgres postgres{};
  postgres.host = LOCALHOST;
  postgres.port = 5432;
  postgres.database = "iroha";

  config::BlockStorage storage{};
  storage.path = "blocks"s;

  config::Cryptography crypto{};

  /// OPTIONS
  auto start = main.add_subcommand("start"s, "Start peer"s);
  auto ledger =
      main.add_subcommand("ledger"s, "Manage ledger"s)->require_subcommand(1);
  auto lcreate = ledger->add_subcommand(
      "create"s, "Create new network with given genesis block"s);
  addCreateLedgerFlags(lcreate, [&argv, &log](std::string genesis) {
    log->info("{} ledger create {} is called", argv[0], genesis);
    BOOST_ASSERT_MSG(false, "not implemented");
  });

  auto lclear = ledger->add_subcommand("clear"s, "Clear peer's ledger"s);
  lclear->set_callback([&log, &argv]() {
    // TODO (@warchant) 20/10/17: implement
    log->info("{} ledger clear is called", argv[0]);
    BOOST_ASSERT_MSG(false, "not implemented");
  });

  addPeerFlags(start, torii, crypto);
  addPostgresFlags(start, postgres);
  addRedisFlags(start, redis);
  addBlockStorageFlags(start, storage);
  addOtherOptionsFlags(start, other);

  CLI11_PARSE(main, argc, argv);

  try {
    // if something critical can not be initialized, throws exceptions
    // descendants from std::exception
    Application irohad;
    irohad.initStorage(postgres, redis, storage);
    irohad.initProtoFactories();
    irohad.initPeerQuery();
    irohad.initCryptoProvider(crypto);
    irohad.initValidators();
    irohad.initOrderingGate(other);
    irohad.initSimulator();
    irohad.initBlockLoader();
    irohad.initConsensusGate(torii, other);
    irohad.initSynchronizer();
    irohad.initPeerCommunicationService();
    irohad.initTransactionCommandService();
    irohad.initQueryService();

    // runs iroha
    log->info("iroha initialized");
    irohad.run(torii);

  } catch (const std::exception &e) {
    log->error("FATAL: {}", e.what());
  }

  return 0;
}
