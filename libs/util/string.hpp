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

#ifndef IROHA_UTIL_STRING_HPP_
#define IROHA_UTIL_STRING_HPP_

#include <algorithm>
#include <boost/assert.hpp>
#include <sstream>
#include <string>
#include <string.h>

namespace iroha {
  namespace string {

    /// @returns true if all chars in string are printable
    inline bool is_printable(const std::string &s) {
      return std::all_of(
          s.begin(), s.end(), [](const auto c) { return std::isprint(c); });
    }

    /**
     * Parse arbitrary typeT: string, int, long, double... from
     * string-encoded format to T.
     * @tparam T
     * @throws std::exception if string can not be parsed.
     * @param s any string
     * @return
     */
    template <typename T>
    T from_string(const std::string &s) {
      // we think that check is performed in caller.
      BOOST_ASSERT(!s.empty());

      std::stringstream ss(s);
      T result;
      ss >> result;

      return result;
    }

    /**
     * Parse Env variable. If not successful, returns default value.
     * @tparam T arbitrary simple type (string, int, etc)
     * @param name name of environment variable
     * @param default_ default value, if variable does not exist
     */
    template <typename T>
    T parseEnv(const char *name, T default_) {
      auto v = std::getenv(name);
      return v && strlen(v) > 0 ? from_string<T>(v) : default_;
    }
  }  // namespace string
}  // namespace iroha
#endif  // IROHA_UTIL_STRING_HPP_
