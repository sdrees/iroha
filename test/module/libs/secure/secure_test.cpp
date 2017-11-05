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

/**
 * @given initialized string
 * @when erasing string with secure::erase
 * @then string is guaranteed to be filled with 0s
 */
TEST(SecureUtil, Erase) {
  // given
  std::string s = "hello world";

  // when
  secure::erase((void *)s.data(), s.size());

  // then
  for (char i : s) {
    ASSERT_EQ(i, 0);
  }
}

/**
 * @given initialized string of size N, another string with preallocated N
 * symbols
 * @when use secure::move
 * @then source string is filled with zeroes, destination string is filled with
 * data
 */
TEST(SecureUtil, Move) {
  // given
  std::string expected = "hello world";
  // source string
  std::string s = expected;
  // destination container
  std::string d(s.size(), 'a' /* old content is not 0 */);

  // when
  secure::move((void *)d.data(), (void *)s.data(), s.size());

  // then
  for (char i : s) {
    ASSERT_EQ(i, 0);
  }

  for (char i : d) {
    ASSERT_NE(i, 0);
  }

  ASSERT_EQ(d, expected);
  ASSERT_EQ(d.size(), expected.size());
}

/**
 * @given two equal strings
 * @when use secure::compare
 * @then strings are equal
 */
TEST(SecureUtil, Compare) {
  // given
  std::string s = "hello world";
  std::string d = s;

  // when
  bool result = secure::compare((void *)s.data(), (void *)d.data(), s.size());

  // then
  ASSERT_TRUE(result);
}
