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
#include "main/cli/flags.hpp"
#include "main/cli/handler/all.hpp"

using namespace iroha;
using std::literals::string_literals::operator""s;   // std::string
using std::literals::chrono_literals::operator""ms;  // milliseconds

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
  CLI::App main("iroha - simple decentralized ledger"s);
  main.require_subcommand(1);
  main.add_flag("-v,--version"s,
                [&argv, &main](size_t) {
                  // note (@warchant): do not use logger here, it
                  // looks ugly.
                  std::cout << argv[0] << " version " IROHA_VERSION_STR "\n";
                  exit(0);
                },
                "Current version"s);

  /// DEFAULTS
  config::Torii torii{};
  torii.host = ALLHOST;
  torii.port = 50051;

  config::OtherOptions other{};
  other.load_delay = 5000ms;
  other.vote_delay = 5000ms;
  other.proposal_delay = 5000ms;
  other.max_proposal_size = 10;

  config::Redis redis{};
  redis.host = LOCALHOST;
  redis.port = 6379;

  config::Postgres postgres{};
  postgres.host = LOCALHOST;
  postgres.port = 5432;

  config::BlockStorage storage{};
  storage.path = "blocks"s;

  config::Cryptography crypto{};

  /// OPTIONS
  // start
  auto start = main.add_subcommand("start"s, "Start peer"s);
  addPeerFlags(start, &torii, &crypto);
  addPostgresFlags(start, &postgres);
  addRedisFlags(start, &redis);
  addBlockStorageFlags(start, &storage);
  addOtherOptionsFlags(start, &other);
  start->set_callback([&]() {
    cli::handler::start(&postgres, &redis, &storage, &other, &crypto, &torii);
  });

  // ledger
  auto ledger =
      main.add_subcommand("ledger"s, "Manage ledger"s)->require_subcommand(1);

  // ledger create
  auto ledger_create = ledger->add_subcommand(
      "create"s, "Create new network with given genesis block"s);
  addPostgresFlags(ledger_create, &postgres);
  addRedisFlags(ledger_create, &redis);
  addBlockStorageFlags(ledger_create, &storage);
  addCreateLedgerFlags(ledger_create, [&](std::string genesis_path) {
    // at this point we can be sure that file exists, because CLI performs check
    std::ifstream f(genesis_path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();

    std::string genesis_content = buffer.str();

    cli::handler::ledger::create(&postgres, &redis, &storage, genesis_content);
  });

  // ledger clear
  auto ledger_clear =
      ledger->add_subcommand("clear"s, "Clear peer's ledger"s, false);
  addPostgresFlags(ledger_clear, &postgres);
  addRedisFlags(ledger_clear, &redis);
  addBlockStorageFlags(ledger_clear, &storage);
  ledger_clear->set_callback(
      [&]() { cli::handler::ledger::clear(&postgres, &redis, &storage); });

  // config
  auto config = main.add_subcommand("config"s, "Configuration management"s)
                    ->require_subcommand(1);
  auto config_show =
      config->add_subcommand("show"s, "Show current config"s, false);
  config_show->set_callback([&]() {
    cli::handler::config::init(
        &postgres, &redis, &storage, &other, &crypto, &torii);
  });

  CLI11_PARSE(main, argc, argv);

  return 0;
}
