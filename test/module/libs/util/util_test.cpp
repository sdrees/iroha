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

#include <gtest/gtest.h>
#include <cstdlib>

#include "util/network.hpp"
#include "util/string.hpp"


using namespace std::literals::string_literals;
using namespace iroha;

TEST(String, IsPrintable) {
  ASSERT_TRUE(string::is_printable("abcde"));
  ASSERT_FALSE(string::is_printable(std::string(5, 0x1)));
}

TEST(String, FromString) {
  // correct data, no throw
  ASSERT_NO_THROW(string::from_string<int>("123"));
  ASSERT_TRUE(string::from_string<int>("123"));
  ASSERT_EQ(string::from_string<int>("123").value(), 123);

  // incorrect data, nullopt
  ASSERT_NO_THROW(string::from_string<int>("a123"));
  ASSERT_FALSE(string::from_string<int>("a123"));

  // incorrect data, nullopt
  ASSERT_NO_THROW(string::from_string<int>("123a"));
  ASSERT_FALSE(string::from_string<int>("123a"));
}

TEST(String, ParseEnv_EnvExists) {
  // set test env
  setenv("IROHA_PARSEENV_TEST", "1", 1 /* overwrite */);

  int res = string::parse_env("IROHA_PARSEENV_TEST", 2);
  ASSERT_EQ(res, 1) << "expected value is 1, but got something else";
}

TEST(String, ParseEnv_EnvDoesNotExist) {
  int res = string::parse_env("TRY_TO_READ_RANDOM_ENV", 2);
  ASSERT_EQ(res, 2) << "expected value is 2, because no such env var";
}

TEST(Network, IsPortValid) {
  uint64_t bignum = ((uint64_t)1 << 33) + 5432;
  /// in case if is_port_valid received uint16_t below expression would be true,
  /// which is wrong.
  ASSERT_FALSE(network::is_port_valid(bignum));  // false negative
  ASSERT_TRUE(network::is_port_valid(5432));     // true positive
}

TEST(Network, IsHostValid) {
  std::vector<std::pair<std::string, bool> > hosts = {
      {"сиплюсплюс.рф"s, true},
      {"sooper-domain.com"s, true},
      {"sooper_domain.com"s, true}, // in hosts we can write anything
      {"192.168.1.1"s, true},
      {""s, false},
      {"lolkekcheburek"s, true}
  };

  for(auto entry  : hosts) {
    ASSERT_EQ(network::is_host_valid(entry.first), entry.second)
                  << entry.first + " should be " + (entry.second ? "true" : "false");
  }
}
