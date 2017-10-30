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
#include <random>

#include "secure/prng.hpp"
#include "secure/util.hpp"

using namespace iroha;

TEST(SecureUtil, Erase) {
  std::string s = "hello world";
  secure::erase((void *)s.data(), s.size());

  for (char i : s) {
    ASSERT_EQ(i, 0);
  }
}

TEST(SecureUtil, Move) {
  std::string expected = "hello world";

  // source string
  std::string s = expected;
  // destination container
  std::string d(s.size(), 'a' /* old content is not 0 */);

  secure::move((void *)d.data(), (void *)s.data(), s.size());

  for (char i : s) {
    ASSERT_EQ(i, 0);
  }

  for (char i : d) {
    ASSERT_NE(i, 0);
  }

  ASSERT_EQ(d, expected);
  ASSERT_EQ(d.size(), expected.size());
}

TEST(SecureUtil, Compare) {
  std::string s = "hello world";
  std::string d = s;

  ASSERT_TRUE(secure::compare((void *)s.data(), (void *)d.data(), s.size()));
}

TEST(PRNG, SingleValue) {
  secure::PRNG<std::uniform_int_distribution<uint8_t> > prng{};

  for (int i = 0; i < 1024; i++) {
    auto n = prng.get();
    ASSERT_GE(n, 0);
    ASSERT_LE(n, 255);
  }
}
