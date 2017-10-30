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

#include "main/application.hpp"

#define PRECONDITION_TRUE(module) \
  BOOST_ASSERT_MSG(module, "[precondition] #module failed");
#define POSTCONDITION_TRUE(module) \
  BOOST_ASSERT_MSG(module, "[postcondition] #module failed");

using namespace iroha;
using namespace iroha::ametsuchi;
using namespace iroha::simulator;
using namespace iroha::validation;
using namespace iroha::network;
using namespace iroha::model;
using namespace iroha::synchronizer;
using namespace iroha::torii;
using namespace iroha::model::converters;
using namespace iroha::consensus::yac;

Application::Application() : log_(logger::log("application")) {}

Application::~Application() {
  if (internal_server) {
    internal_server->Shutdown();
  }
  if (torii_server) {
    torii_server->shutdown();
  }
  if (server_thread.joinable()) {
    server_thread.join();
  }
}

void Application::dropStorage() { storage->dropStorage(); }

void Application::initStorage(const Postgres &pg,
                              const Redis &rd,
                              const BlockStorage &bs) {
  storage = StorageImpl::create(pg, rd, bs);

  // TODO (@warchant): if the storage is nullptr, then we do not have conenction
  // to redis OR postgres. Handle this.
  POSTCONDITION_TRUE(storage);

  log_->info("[Init] => storage", logger::logBool(storage));
}

void Application::initProtoFactories() {
  pb_tx_factory = std::make_shared<PbTransactionFactory>();
  pb_query_factory = std::make_shared<PbQueryFactory>();
  pb_query_response_factory = std::make_shared<PbQueryResponseFactory>();

  POSTCONDITION_TRUE(pb_tx_factory);
  POSTCONDITION_TRUE(pb_query_factory);
  POSTCONDITION_TRUE(pb_query_response_factory);

  log_->info("[Init] => converters");
}

void Application::initPeerQuery() {
  PRECONDITION_TRUE(storage);
  wsv = std::make_shared<ametsuchi::PeerQueryWsv>(storage->getWsvQuery());
  POSTCONDITION_TRUE(wsv);

  log_->info("[Init] => peer query");
}

void Application::initCryptoProvider(const Cryptography &crypto) {
  // TODO(@warchant): parse crypto for keypair

  crypto_verifier = std::make_shared<ModelCryptoProviderImpl>(keypair);

  POSTCONDITION_TRUE(crypto_verifier);

  log_->info("[Init] => crypto provider");
}

void Application::initValidators() {
  PRECONDITION_TRUE(crypto_verifier);

  stateless_validator =
      std::make_shared<StatelessValidatorImpl>(crypto_verifier);
  POSTCONDITION_TRUE(stateless_validator);

  stateful_validator = std::make_shared<StatefulValidatorImpl>();
  POSTCONDITION_TRUE(stateful_validator);

  chain_validator = std::make_shared<ChainValidatorImpl>();
  POSTCONDITION_TRUE(chain_validator);

  log_->info("[Init] => validators");
}

void Application::initOrderingGate(const OtherOptions &other) {
  PRECONDITION_TRUE(wsv);

  auto proposal_delay = std::chrono::milliseconds(other.proposal_delay);

  ordering_gate = ordering_init.initOrderingGate(
      wsv, other.max_proposal_size, proposal_delay);

  POSTCONDITION_TRUE(ordering_gate);

  log_->info("[Init] => init ordering gate - [{}]",
             logger::logBool(ordering_gate));
}

void Application::initSimulator() {
  PRECONDITION_TRUE(ordering_gate);
  PRECONDITION_TRUE(stateful_validator);
  PRECONDITION_TRUE(storage);
  PRECONDITION_TRUE(crypto_verifier);

  simulator = std::make_shared<Simulator>(ordering_gate,
                                          stateful_validator,
                                          storage,
                                          storage->getBlockQuery(),
                                          crypto_verifier);

  POSTCONDITION_TRUE(simulator);

  log_->info("[Init] => init simulator");
}

void Application::initBlockLoader() {
  PRECONDITION_TRUE(wsv);
  PRECONDITION_TRUE(storage);
  PRECONDITION_TRUE(crypto_verifier);

  block_loader = loader_init.initBlockLoader(
      wsv, storage->getBlockQuery(), crypto_verifier);

  POSTCONDITION_TRUE(block_loader);

  log_->info("[Init] => block loader");
}

void Application::initConsensusGate(const Torii &torii,
                                    const OtherOptions &other) {
  PRECONDITION_TRUE(wsv);
  PRECONDITION_TRUE(simulator);
  PRECONDITION_TRUE(block_loader);

  auto vote_delay = std::chrono::milliseconds(other.vote_delay);
  auto load_delay = std::chrono::milliseconds(other.load_delay);

  consensus_gate = yac_init.initConsensusGate(torii.listenAddress(),
                                              wsv,
                                              simulator,
                                              block_loader,
                                              keypair,
                                              vote_delay,
                                              load_delay);

  POSTCONDITION_TRUE(consensus_gate);

  log_->info("[Init] => consensus gate");
}

void Application::initSynchronizer() {
  PRECONDITION_TRUE(consensus_gate);
  PRECONDITION_TRUE(chain_validator);
  PRECONDITION_TRUE(storage);
  PRECONDITION_TRUE(block_loader);

  synchronizer = std::make_shared<SynchronizerImpl>(
      consensus_gate, chain_validator, storage, block_loader);

  POSTCONDITION_TRUE(synchronizer);

  log_->info("[Init] => synchronizer");
}

void Application::initPeerCommunicationService() {
  PRECONDITION_TRUE(ordering_gate);
  PRECONDITION_TRUE(synchronizer);

  pcs = std::make_shared<PeerCommunicationServiceImpl>(ordering_gate,
                                                       synchronizer);

  POSTCONDITION_TRUE(pcs);

  pcs->on_proposal().subscribe(
      [this](auto) { log_->info("~~~~~~~~~| PROPOSAL ^_^ |~~~~~~~~~ "); });

  pcs->on_commit().subscribe(
      [this](auto) { log_->info("~~~~~~~~~| COMMIT =^._.^= |~~~~~~~~~ "); });

  log_->info("[Init] => pcs");
}

void Application::initTransactionCommandService() {
  PRECONDITION_TRUE(pcs);
  PRECONDITION_TRUE(stateless_validator);
  PRECONDITION_TRUE(pb_tx_factory);

  auto tx_processor =
      std::make_shared<TransactionProcessorImpl>(pcs, stateless_validator);

  POSTCONDITION_TRUE(tx_processor);

  command_service =
      std::make_unique<::torii::CommandService>(pb_tx_factory, tx_processor);
  POSTCONDITION_TRUE(command_service);

  log_->info("[Init] => command service");
}

void Application::initQueryService() {
  PRECONDITION_TRUE(storage);
  PRECONDITION_TRUE(stateless_validator);
  PRECONDITION_TRUE(pb_query_factory);
  PRECONDITION_TRUE(pb_query_response_factory);

  auto query_proccessing_factory = std::make_unique<QueryProcessingFactory>(
      storage->getWsvQuery(), storage->getBlockQuery());

  POSTCONDITION_TRUE(query_proccessing_factory);

  auto query_processor = std::make_shared<QueryProcessorImpl>(
      std::move(query_proccessing_factory), stateless_validator);

  POSTCONDITION_TRUE(query_processor);

  query_service = std::make_unique<::torii::QueryService>(
      pb_query_factory, pb_query_response_factory, query_processor);

  POSTCONDITION_TRUE(query_service);

  log_->info("[Init] => query service");
}

void Application::run(const Torii &torii) {
  torii_server = std::make_unique<ServerRunner>(torii.listenAddress());
  POSTCONDITION_TRUE(torii_server);

  grpc::ServerBuilder builder;
  int port;
  builder.AddListeningPort(
      torii.listenAddress(), grpc::InsecureServerCredentials(), &port);

  PRECONDITION_TRUE(ordering_init.ordering_gate_transport);
  PRECONDITION_TRUE(ordering_init.ordering_service_transport);
  PRECONDITION_TRUE(yac_init.consensus_network);
  PRECONDITION_TRUE(loader_init.service);

  builder.RegisterService(ordering_init.ordering_gate_transport.get());
  builder.RegisterService(ordering_init.ordering_service_transport.get());
  builder.RegisterService(yac_init.consensus_network.get());
  builder.RegisterService(loader_init.service.get());

  internal_server = builder.BuildAndStart();
  POSTCONDITION_TRUE(internal_server);

  server_thread = std::thread([this] {
    torii_server->run(std::move(command_service), std::move(query_service));
  });

  log_->info("===> iroha initialized");

  torii_server->waitForServersReady();
  internal_server->Wait();
}
