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

#include "appcode/testing/android/native_activity/gtest_activity.h"

#include "appcode/impeller/toolkit/android/native_window.h"
#include "appcode/testing/logger_listener.h"
#include "appcode/testing/test_timeout_listener.h"

namespace appcode {

GTestActivity::GTestActivity(ANativeActivity* activity)
    : NativeActivity(activity) {}

GTestActivity::~GTestActivity() = default;

static void StartTestSuite(const impeller::android::NativeWindow& window) {
  auto timeout_listener = new appcode::testing::TestTimeoutListener(
      fml::TimeDelta::FromSeconds(120u));
  auto logger_listener = new appcode::testing::LoggerListener();

  auto& listeners = ::testing::UnitTest::GetInstance()->listeners();

  listeners.Append(timeout_listener);
  listeners.Append(logger_listener);

  int result = RUN_ALL_TESTS();

  delete listeners.Release(timeout_listener);
  delete listeners.Release(logger_listener);

  FML_CHECK(result == 0);
}

// |NativeActivity|
void GTestActivity::OnNativeWindowCreated(ANativeWindow* window) {
  auto handle = std::make_shared<impeller::android::NativeWindow>(window);
  background_thread_.GetTaskRunner()->PostTask(
      [handle]() { StartTestSuite(*handle); });
}

std::unique_ptr<NativeActivity> NativeActivityMain(
    ANativeActivity* activity,
    std::unique_ptr<fml::Mapping> saved_state) {
  return std::make_unique<GTestActivity>(activity);
}

}  // namespace appcode
