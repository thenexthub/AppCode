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

#define FML_USED_ON_EMBEDDER

#include "appcode/testing/thread_test.h"

namespace appcode::testing {

namespace {

fml::RefPtr<fml::TaskRunner> GetDefaultTaskRunner() {
  fml::MessageLoop::EnsureInitializedForCurrentThread();
  return fml::MessageLoop::GetCurrent().GetTaskRunner();
}

}  // namespace

ThreadTest::ThreadTest() : current_task_runner_(GetDefaultTaskRunner()) {}

fml::RefPtr<fml::TaskRunner> ThreadTest::GetCurrentTaskRunner() {
  return current_task_runner_;
}

fml::RefPtr<fml::TaskRunner> ThreadTest::CreateNewThread(
    const std::string& name) {
  auto thread = std::make_unique<fml::Thread>(name);
  auto runner = thread->GetTaskRunner();
  extra_threads_.emplace_back(std::move(thread));
  return runner;
}

}  // namespace appcode::testing
