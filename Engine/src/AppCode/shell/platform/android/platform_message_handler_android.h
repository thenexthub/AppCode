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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_MESSAGE_HANDLER_ANDROID_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_MESSAGE_HANDLER_ANDROID_H_

#include <jni.h>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "appcode/lib/ui/window/platform_message.h"
#include "appcode/shell/common/platform_message_handler.h"
#include "appcode/shell/platform/android/jni/platform_view_android_jni.h"

namespace appcode {
class PlatformMessageHandlerAndroid : public PlatformMessageHandler {
 public:
  explicit PlatformMessageHandlerAndroid(
      const std::shared_ptr<PlatformViewAndroidJNI>& jni_facade);
  void HandlePlatformMessage(std::unique_ptr<PlatformMessage> message) override;
  bool DoesHandlePlatformMessageOnPlatformThread() const override {
    return false;
  }
  void InvokePlatformMessageResponseCallback(
      int response_id,
      std::unique_ptr<fml::Mapping> mapping) override;

  void InvokePlatformMessageEmptyResponseCallback(int response_id) override;

 private:
  const std::shared_ptr<PlatformViewAndroidJNI> jni_facade_;
  std::atomic<int> next_response_id_ = 1;
  std::unordered_map<int, fml::RefPtr<appcode::PlatformMessageResponse>>
      pending_responses_;
  std::mutex pending_responses_mutex_;
};
}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_ANDROID_PLATFORM_MESSAGE_HANDLER_ANDROID_H_
