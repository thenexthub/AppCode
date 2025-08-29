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

#include "appcode/shell/platform/android/platform_message_response_android.h"

#include <utility>

#include "appcode/fml/make_copyable.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"

namespace appcode {

PlatformMessageResponseAndroid::PlatformMessageResponseAndroid(
    int response_id,
    std::shared_ptr<PlatformViewAndroidJNI> jni_facade,
    fml::RefPtr<fml::TaskRunner> platform_task_runner)
    : response_id_(response_id),
      jni_facade_(std::move(jni_facade)),
      platform_task_runner_(std::move(platform_task_runner)) {}

PlatformMessageResponseAndroid::~PlatformMessageResponseAndroid() = default;

// |appcode::PlatformMessageResponse|
void PlatformMessageResponseAndroid::Complete(
    std::unique_ptr<fml::Mapping> data) {
  platform_task_runner_->PostTask(
      fml::MakeCopyable([response_id = response_id_,  //
                         data = std::move(data),      //
                         jni_facade = jni_facade_]() mutable {
        jni_facade->appcodeViewHandlePlatformMessageResponse(response_id,
                                                             std::move(data));
      }));
}

// |appcode::PlatformMessageResponse|
void PlatformMessageResponseAndroid::CompleteEmpty() {
  platform_task_runner_->PostTask(
      fml::MakeCopyable([response_id = response_id_,  //
                         jni_facade = jni_facade_     //
  ]() {
        // Make the response call into Java.
        jni_facade->appcodeViewHandlePlatformMessageResponse(response_id,
                                                             nullptr);
      }));
}
}  // namespace appcode
