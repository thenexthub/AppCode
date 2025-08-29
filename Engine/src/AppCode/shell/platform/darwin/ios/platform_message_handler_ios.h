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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_PLATFORM_MESSAGE_HANDLER_IOS_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_PLATFORM_MESSAGE_HANDLER_IOS_H_

#include "appcode/fml/task_runner.h"
#include "appcode/shell/common/platform_message_handler.h"
#import "appcode/shell/platform/darwin/ios/appcode_task_queue_dispatch.h"

namespace appcode {

class PlatformMessageHandlerIos : public PlatformMessageHandler {
 public:
  static NSObject<FlutterTaskQueue>* MakeBackgroundTaskQueue();

  explicit PlatformMessageHandlerIos(fml::RefPtr<fml::TaskRunner> platform_task_runner);

  void HandlePlatformMessage(std::unique_ptr<PlatformMessage> message) override;

  bool DoesHandlePlatformMessageOnPlatformThread() const override;

  void InvokePlatformMessageResponseCallback(int response_id,
                                             std::unique_ptr<fml::Mapping> mapping) override;

  void InvokePlatformMessageEmptyResponseCallback(int response_id) override;

  void SetMessageHandler(const std::string& channel,
                         FlutterBinaryMessageHandler handler,
                         NSObject<FlutterTaskQueue>* task_queue);

  struct HandlerInfo {
    NSObject<FlutterTaskQueueDispatch>* task_queue;
    FlutterBinaryMessageHandler handler;
  };

 private:
  std::unordered_map<std::string, HandlerInfo> message_handlers_;
  const fml::RefPtr<fml::TaskRunner> platform_task_runner_;
  std::mutex message_handlers_mutex_;
  FML_DISALLOW_COPY_AND_ASSIGN(PlatformMessageHandlerIos);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_PLATFORM_MESSAGE_HANDLER_IOS_H_
