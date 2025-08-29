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

#ifndef APPCODE_TESTING_ANDROID_NATIVE_ACTIVITY_GTEST_ACTIVITY_H_
#define APPCODE_TESTING_ANDROID_NATIVE_ACTIVITY_GTEST_ACTIVITY_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/thread.h"
#include "appcode/testing/android/native_activity/native_activity.h"

namespace appcode {

//------------------------------------------------------------------------------
/// @brief      A native activity subclass an in implementation of
///             `appcode::NativeActivityMain` that return it.
///
///             This class runs a Google Test harness on a background thread and
///             redirects progress updates to `logcat` instead of STDOUT.
///
class GTestActivity final : public NativeActivity {
 public:
  explicit GTestActivity(ANativeActivity* activity);

  ~GTestActivity() override;

  GTestActivity(const GTestActivity&) = delete;

  GTestActivity& operator=(const GTestActivity&) = delete;

  // |NativeActivity|
  void OnNativeWindowCreated(ANativeWindow* window) override;

 private:
  fml::Thread background_thread_;
};

}  // namespace appcode

#endif  // APPCODE_TESTING_ANDROID_NATIVE_ACTIVITY_GTEST_ACTIVITY_H_
