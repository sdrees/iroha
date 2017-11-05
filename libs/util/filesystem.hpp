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

#ifndef IROHA_UTIL_FILESYSTEM_HPP_
#define IROHA_UTIL_FILESYSTEM_HPP_

#include <boost/optional.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace iroha {
  namespace filesystem {

    /**
     * Reads file by provided path, then returns its contents.
     * @param path path to the file
     * @return file content
     */
    boost::optional<std::string> read_file(const std::string &path) {
      if (path.empty()) {
        return boost::none;
      }

      std::ifstream f(path);
      f.exceptions();  // disable exceptions
      std::stringstream buffer;
      buffer << f.rdbuf();
      f.close();

      if (buffer.fail()) {
        return boost::none;
      }

      return buffer.str();
    }
  }
}

#endif  //  IROHA_UTIL_FILESYSTEM_HPP_
