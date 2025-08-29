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

#include "appcode/fml/synchronization/count_down_latch.h"

#include <chrono>
#include <thread>

#include "appcode/fml/build_config.h"
#include "appcode/fml/thread.h"
#include "appcode/testing/testing.h"

namespace fml {

TEST(CountDownLatchTest, CanWaitOnZero) {
  CountDownLatch latch(0);
  latch.Wait();
}

TEST(CountDownLatchTest, CanWait) {
  fml::Thread thread("test_thread");
  const size_t count = 100;
  size_t current_count = 0;
  CountDownLatch latch(count);
  auto decrement_latch_on_thread = [runner = thread.GetTaskRunner(), &latch,
                                    &current_count]() {
    runner->PostTask([&latch, &current_count]() {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
      current_count++;
      latch.CountDown();
    });
  };
  for (size_t i = 0; i < count; ++i) {
    decrement_latch_on_thread();
  }
  latch.Wait();
  ASSERT_EQ(current_count, count);
}

}  // namespace fml
