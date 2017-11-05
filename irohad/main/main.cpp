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
#include "ametsuchi/config.hpp"
#include "cli/defaults.hpp"
#include "cli/flags.hpp"
#include "cli/handler/all.hpp"
#include "torii/config.hpp"
#include "util/filesystem.hpp"

using namespace iroha;
using std::literals::string_literals::operator""s;   // std::string
using std::literals::chrono_literals::operator""ms;  // milliseconds

// IROHA_VERSION should be defined at compile-time with -DIROHA_VERSION=abcd
#ifdef IROHA_VERSION
// it is a preprocessor trick, which converts X to "X" (const char*)
// refer to https://stackoverflow.com/a/240370/1953079
// example: https://ideone.com/F9g67M
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define IROHA_VERSION_STR STRINGIFY(IROHA_VERSION)
#else
#define IROHA_VERSION_STR "undefined"
#endif

int main(int argc, char *argv[]) {
  CLI::App main("iroha - simple decentralized ledger"s);
  main.require_subcommand(1);
  main.add_config();
  main.add_flag("-v,--version"s,
                [&argv](size_t) {
                  std::cout << argv[0] << " version " IROHA_VERSION_STR "\n";
                  exit(EXIT_SUCCESS);
                },
                "Current version"s);

  /// DEFAULTS
  std::string genesis_path;

  iroha::ametsuchi::config::Ametsuchi ametsuchi{};
  ametsuchi.postgres.host = defaults::postgresHost;
  ametsuchi.postgres.port = defaults::postgresPort;
  ametsuchi.redis.host = defaults::redisHost;
  ametsuchi.redis.port = defaults::redisPort;
  ametsuchi.blockStorage.path = defaults::blockStoragePath;

  iroha::torii::config::Torii torii{};
  torii.host = defaults::toriiHost;
  torii.port = defaults::toriiPort;

  iroha::config::Cryptography crypto{};

  iroha::config::OtherOptions other{};
  other.load_delay = defaults::loadDelay;
  other.vote_delay = defaults::voteDelay;
  other.proposal_delay = defaults::proposalDelay;
  other.max_proposal_size = defaults::proposalSize;

  addPeerFlags(&main, &torii, &crypto);
  addPostgresFlags(&main, &ametsuchi.postgres);
  addRedisFlags(&main, &ametsuchi.redis);
  addBlockStorageFlags(&main, &ametsuchi.blockStorage);
  addOtherOptionsFlags(&main, &other);

  /// OPTIONS
  // start
  auto start = main.add_subcommand("start"s, "Start iroha"s);
  start->set_callback(
      [&]() { cli::handler::start(&ametsuchi, &crypto, &other, &torii); });

  // ledger
  auto ledger =
      main.add_subcommand("ledger"s, "Manage ledger"s)->require_subcommand(1);

  // ledger create
  auto ledger_create = ledger->add_subcommand(
      "create"s, "Create new network with given genesis block"s);
  addCreateLedgerFlags(ledger_create, genesis_path);
  ledger_create->set_callback([&]() {
    cli::handler::ledger::create(&ametsuchi, genesis_path);
  });

  // ledger clear
  auto ledger_clear =
      ledger->add_subcommand("clear"s,
                             "Clear peer's ledger"s,
                             false /* disable help for this command */);
  ledger_clear->set_callback(
      [&]() { cli::handler::ledger::clear(&ametsuchi); });

  // config
  auto config =
      main.add_subcommand("config"s, "Dump current configuration"s, false);
  config->set_callback([&]() {
    cli::handler::config::config(&ametsuchi, &other, &crypto, &torii);
  });

  CLI11_PARSE(main, argc, argv);

  return 0;
}
