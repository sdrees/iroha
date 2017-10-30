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
#include <boost/optional.hpp>
#include <cstring>
#include <sstream>
#include <string>

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
     *
     * @example T=double, s=std::string("1.337"), result will be 1.337
     * @tparam T
     * @throws std::exception if string can not be parsed.
     * @param s any string
     * @return
     */
    template <typename T>
    boost::optional<T> from_string(const char* s) {
      if(!s) {
        return boost::none;
      }

      std::stringstream ss(s);
      ss.exceptions();  // disable exceptions
      T result;
      // ss.eof() rejects this case
      // from_string<int>("123a") => 123 - incorrect, should be none
      if (ss >> result && ss.eof()) {
        return result;
      } else {
        return boost::none;
      }
    }

    /**
     * Parse Env variable. If not successful, returns default value.
     * @tparam T arbitrary simple type (string, int, etc)
     * @param name name of environment variable
     * @param default_ default value, if variable does not exist
     */
    template <typename T>
    T parse_env(const char *name, T default_) {
      return from_string<T>(std::getenv(name)).value_or(default_);
    }
  }  // namespace string
}  // namespace iroha
#endif  // IROHA_UTIL_STRING_HPP_
