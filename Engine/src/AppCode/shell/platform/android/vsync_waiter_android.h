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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_VSYNC_WAITER_ANDROID_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_VSYNC_WAITER_ANDROID_H_

#include <jni.h>

#include <memory>

#include "appcode/fml/macros.h"
#include "appcode/shell/common/vsync_waiter.h"

namespace appcode {

class AndroidChoreographer;

class VsyncWaiterAndroid final : public VsyncWaiter {
 public:
  static bool Register(JNIEnv* env);

  explicit VsyncWaiterAndroid(const appcode::TaskRunners& task_runners);

  ~VsyncWaiterAndroid() override;

 private:
  // |VsyncWaiter|
  void AwaitVSync() override;

  static void OnVsyncFromNDK(int64_t frame_nanos, void* data);

  static void OnVsyncFromJava(JNIEnv* env,
                              jclass jcaller,
                              jlong frameDelayNanos,
                              jlong refreshPeriodNanos,
                              jlong java_baton);

  static void ConsumePendingCallback(std::weak_ptr<VsyncWaiter>* weak_this,
                                     fml::TimePoint frame_start_time,
                                     fml::TimePoint frame_target_time);

  static void OnUpdateRefreshRate(JNIEnv* env,
                                  jclass jcaller,
                                  jfloat refresh_rate);

  FML_DISALLOW_COPY_AND_ASSIGN(VsyncWaiterAndroid);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_VSYNC_WAITER_ANDROID_H_
