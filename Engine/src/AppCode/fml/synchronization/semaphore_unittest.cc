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

#include <thread>

#include "appcode/fml/synchronization/semaphore.h"
#include "appcode/fml/thread.h"
#include "appcode/fml/time/time_point.h"
#include "gtest/gtest.h"

TEST(SemaphoreTest, SimpleValidity) {
  fml::Semaphore sem(100);
  ASSERT_TRUE(sem.IsValid());
}

TEST(SemaphoreTest, WaitOnZero) {
  fml::Semaphore sem(0);
  ASSERT_FALSE(sem.TryWait());
}

TEST(SemaphoreTest, WaitOnZeroSignalThenWait) {
  fml::Semaphore sem(0);
  ASSERT_FALSE(sem.TryWait());
  std::thread thread([&sem]() { sem.Signal(); });
  thread.join();
  ASSERT_TRUE(sem.TryWait());
  ASSERT_FALSE(sem.TryWait());
}

TEST(SemaphoreTest, IndefiniteWait) {
  auto start = fml::TimePoint::Now();
  constexpr double wait_in_seconds = 0.25;
  fml::Semaphore sem(0);
  ASSERT_TRUE(sem.IsValid());
  fml::Thread signaller("signaller_thread");
  signaller.GetTaskRunner()->PostTaskForTime(
      [&sem]() { sem.Signal(); },
      start + fml::TimeDelta::FromSecondsF(wait_in_seconds));
  ASSERT_TRUE(sem.Wait());
  auto delta = fml::TimePoint::Now() - start;
  ASSERT_GE(delta.ToSecondsF(), wait_in_seconds);
  signaller.Join();
}
