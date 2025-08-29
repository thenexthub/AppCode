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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_MESSAGE_RESPONSE_ANDROID_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_MESSAGE_RESPONSE_ANDROID_H_

#include "appcode/fml/macros.h"
#include "appcode/fml/platform/android/jni_weak_ref.h"
#include "appcode/fml/task_runner.h"
#include "appcode/lib/ui/window/platform_message_response.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"

namespace appcode {

class PlatformMessageResponseAndroid : public appcode::PlatformMessageResponse {
 public:
  // |appcode::PlatformMessageResponse|
  void Complete(std::unique_ptr<fml::Mapping> data) override;

  // |appcode::PlatformMessageResponse|
  void CompleteEmpty() override;

 private:
  PlatformMessageResponseAndroid(
      int response_id,
      std::shared_ptr<PlatformViewAndroidJNI> jni_facade,
      fml::RefPtr<fml::TaskRunner> platform_task_runner);

  ~PlatformMessageResponseAndroid() override;

  const int response_id_;
  const std::shared_ptr<PlatformViewAndroidJNI> jni_facade_;
  const fml::RefPtr<fml::TaskRunner> platform_task_runner_;

  FML_FRIEND_MAKE_REF_COUNTED(PlatformMessageResponseAndroid);
  FML_DISALLOW_COPY_AND_ASSIGN(PlatformMessageResponseAndroid);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_MESSAGE_RESPONSE_ANDROID_H_
