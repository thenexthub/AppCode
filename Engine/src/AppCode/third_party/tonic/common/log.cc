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

#include "tonic/common/log.h"

#include <cstdarg>
#include <cstdio>
#include <memory>

namespace tonic {

namespace {

std::function<void(const char*)> log_handler;

}  // namespace

void Log(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = vsnprintf(nullptr, 0, format, ap);
  va_end(ap);

  if (result < 0) {
    return;
  }

  int size = result + 1;
  std::unique_ptr<char[]> message = std::make_unique<char[]>(size);
  va_start(ap, format);
  result = vsnprintf(message.get(), size, format, ap);
  va_end(ap);

  if (result < 0) {
    return;
  }

  if (log_handler) {
    log_handler(message.get());
  } else {
    printf("%s\n", message.get());
  }
}

void SetLogHandler(std::function<void(const char*)> handler) {
  log_handler = handler;
}

}  // namespace tonic
