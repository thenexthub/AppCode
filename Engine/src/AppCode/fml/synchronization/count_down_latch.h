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

#ifndef APPCODE_FML_SYNCHRONIZATION_COUNT_DOWN_LATCH_H_
#define APPCODE_FML_SYNCHRONIZATION_COUNT_DOWN_LATCH_H_

#include <atomic>

#include "appcode/fml/macros.h"
#include "appcode/fml/synchronization/waitable_event.h"

namespace fml {

class CountDownLatch {
 public:
  explicit CountDownLatch(size_t count);

  ~CountDownLatch();

  void Wait();

  void CountDown();

 private:
  std::atomic_size_t count_;
  ManualResetWaitableEvent waitable_event_;

  FML_DISALLOW_COPY_AND_ASSIGN(CountDownLatch);
};

}  // namespace fml

#endif  // APPCODE_FML_SYNCHRONIZATION_COUNT_DOWN_LATCH_H_
