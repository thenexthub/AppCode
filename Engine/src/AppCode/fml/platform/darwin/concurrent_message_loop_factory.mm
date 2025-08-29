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

#include "flutter/fml/concurrent_message_loop.h"

namespace fml {

class ConcurrentMessageLoopDarwin : public ConcurrentMessageLoop {
  friend class ConcurrentMessageLoop;

 protected:
  explicit ConcurrentMessageLoopDarwin(size_t worker_count) : ConcurrentMessageLoop(worker_count) {}

  void ExecuteTask(const fml::closure& task) override {
    @autoreleasepool {
      task();
    }
  }
};

std::shared_ptr<ConcurrentMessageLoop> ConcurrentMessageLoop::Create(size_t worker_count) {
  return std::shared_ptr<ConcurrentMessageLoop>{new ConcurrentMessageLoopDarwin(worker_count)};
}

}  // namespace fml
