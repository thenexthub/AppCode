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

#ifndef APPCODE_TESTING_TEST_TIMEOUT_LISTENER_H_
#define APPCODE_TESTING_TEST_TIMEOUT_LISTENER_H_

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/fml/task_runner.h"
#include "appcode/fml/thread.h"
#include "appcode/testing/testing.h"

namespace appcode::testing {

class PendingTests;

class TestTimeoutListener : public ::testing::EmptyTestEventListener {
 public:
  explicit TestTimeoutListener(fml::TimeDelta timeout);

  ~TestTimeoutListener();

 private:
  const fml::TimeDelta timeout_;
  fml::Thread listener_thread_;
  fml::RefPtr<fml::TaskRunner> listener_thread_runner_;
  std::shared_ptr<PendingTests> pending_tests_;

  // |testing::EmptyTestEventListener|
  void OnTestStart(const ::testing::TestInfo& test_info) override;

  // |testing::EmptyTestEventListener|
  void OnTestEnd(const ::testing::TestInfo& test_info) override;

  FML_DISALLOW_COPY_AND_ASSIGN(TestTimeoutListener);
};

}  // namespace appcode::testing

#endif  // APPCODE_TESTING_TEST_TIMEOUT_LISTENER_H_
