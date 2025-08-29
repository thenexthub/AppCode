//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#ifndef APPCODE_IMPELLER_COMPILER_LOGGER_H_
#define APPCODE_IMPELLER_COMPILER_LOGGER_H_

#include <sstream>
#include <string>

#include "appcode/fml/logging.h"

namespace impeller {
namespace compiler {

class AutoLogger {
 public:
  explicit AutoLogger(std::stringstream& logger) : logger_(logger) {}

  ~AutoLogger() {
    logger_ << std::endl;
    logger_.flush();
  }

  template <class T>
  AutoLogger& operator<<(const T& object) {
    logger_ << object;
    return *this;
  }

 private:
  std::stringstream& logger_;

  AutoLogger(const AutoLogger&) = delete;

  AutoLogger& operator=(const AutoLogger&) = delete;
};

#define COMPILER_ERROR(stream) \
  ::impeller::compiler::AutoLogger(stream) << GetSourcePrefix()

#define COMPILER_ERROR_NO_PREFIX(stream) \
  ::impeller::compiler::AutoLogger(stream)

}  // namespace compiler
}  // namespace impeller

#endif  // APPCODE_IMPELLER_COMPILER_LOGGER_H_
