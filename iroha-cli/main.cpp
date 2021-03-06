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

#include <gflags/gflags.h>
#include <responses.pb.h>
#include <fstream>
#include <iostream>
#include "bootstrap_network.hpp"
#include "common/assert_config.hpp"
#include "genesis_block_client_impl.hpp"
#include "validators.hpp"

#include "client.hpp"
#include "grpc_response_handler.hpp"
#include "impl/keys_manager_impl.hpp"
#include "logger/logger.hpp"

// ** Genesis Block and Provisioning ** //
// Reference is here (TODO: move to doc):
// https://hackmd.io/GwRmwQ2BmCFoCsAGARtOAWBIBMcAcS0GcAZjhNNPvpAKZIDGQA==

DEFINE_string(config, "", "Trusted peer's ip addresses");
// DEFINE_validator(config, &iroha_cli::validate_config);

DEFINE_string(genesis_block, "", "Genesis block for sending network");
// DEFINE_validator(genesis_block, &iroha_cli::validate_genesis_block);

DEFINE_bool(new_account, false, "Choose if account does not exist");
DEFINE_string(name, "", "Name of the account");
DEFINE_string(pass_phrase, "", "Name of the account");

// Sending transaction to Iroha
DEFINE_bool(grpc, false, "Send sample transaction to IrohaNetwork");
DEFINE_string(address, "0.0.0.0", "Address of the Iroha node");
DEFINE_int32(torii_port, 50051, "Port of iroha's Torii");
DEFINE_validator(torii_port, &iroha_cli::validate_port);
DEFINE_string(json_transaction, "", "Transaction in json format");
DEFINE_string(json_query, "", "Query in json format");

using namespace iroha::protocol;

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  gflags::ShutDownCommandLineFlags();
  auto logger = logger::log("CLI-MAIN");
  if (FLAGS_new_account) {
    // Create new pub/priv key
    auto keysManager = iroha_cli::KeysManagerImpl(FLAGS_name);
    if (not keysManager.createKeys(FLAGS_pass_phrase)) {
      logger->error("Keys already exist");
    } else {
      logger->info(
          "Public and private key has been generated in current directory");
    };
  } else if (not FLAGS_config.empty() && not FLAGS_genesis_block.empty()) {
    iroha_cli::GenesisBlockClientImpl genesis_block_client;
    auto bootstrap = iroha_cli::BootstrapNetwork(genesis_block_client);
    auto peers = bootstrap.parse_trusted_peers(FLAGS_config);
    auto block = bootstrap.parse_genesis_block(FLAGS_genesis_block);
    block = bootstrap.merge_tx_add_trusted_peers(block, peers);
    bootstrap.run_network(peers, block);
  } else if (FLAGS_grpc) {
    iroha_cli::CliClient client(FLAGS_address, FLAGS_torii_port);
    iroha_cli::GrpcResponseHandler response_handler;
    if (not FLAGS_json_transaction.empty()) {
      logger->info("Send transaction to {}:{} ", FLAGS_address,
                   FLAGS_torii_port);
      std::ifstream file(FLAGS_json_transaction);
      std::string str((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
      response_handler.handle(client.sendTx(str));
    }
    if (not FLAGS_json_query.empty()) {
      logger->info("Send query to {}:{}", FLAGS_address, FLAGS_torii_port);
      std::ifstream file(FLAGS_json_query);
      std::string str((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
      response_handler.handle(client.sendQuery(str));
    }

  } else {
    assert_config::assert_fatal(false, "Invalid flags");
  }
  return 0;
}
