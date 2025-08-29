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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PLATFORM_MESSAGE_RESPONSE_DARWIN_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PLATFORM_MESSAGE_RESPONSE_DARWIN_H_

#include <Foundation/Foundation.h>

#include "appcode/fml/macros.h"
#include "appcode/fml/make_copyable.h"
#include "appcode/fml/task_runner.h"
#include "appcode/lib/ui/window/platform_message_response.h"
#import "appcode/shell/platform/darwin/common/buffer_conversions.h"

typedef void (^PlatformMessageResponseCallback)(NSData*);

namespace appcode {

class PlatformMessageResponseDarwin : public appcode::PlatformMessageResponse {
 public:
  void Complete(std::unique_ptr<fml::Mapping> data) override;

  void CompleteEmpty() override;

 private:
  explicit PlatformMessageResponseDarwin(PlatformMessageResponseCallback callback,
                                         fml::RefPtr<fml::TaskRunner> platform_task_runner);

  ~PlatformMessageResponseDarwin() override;

  PlatformMessageResponseCallback callback_;
  fml::RefPtr<fml::TaskRunner> platform_task_runner_;

  FML_FRIEND_MAKE_REF_COUNTED(PlatformMessageResponseDarwin);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_FRAMEWORK_SOURCE_PLATFORM_MESSAGE_RESPONSE_DARWIN_H_
