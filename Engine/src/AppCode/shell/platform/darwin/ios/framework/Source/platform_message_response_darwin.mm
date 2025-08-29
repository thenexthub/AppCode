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

#import "flutter/shell/platform/darwin/ios/framework/Source/platform_message_response_darwin.h"

#import "flutter/shell/platform/darwin/common/framework/Headers/FlutterMacros.h"

APPCODE_ASSERT_ARC

namespace flutter {

PlatformMessageResponseDarwin::PlatformMessageResponseDarwin(
    PlatformMessageResponseCallback callback,
    fml::RefPtr<fml::TaskRunner> platform_task_runner)
    : callback_(callback), platform_task_runner_(std::move(platform_task_runner)) {}

PlatformMessageResponseDarwin::~PlatformMessageResponseDarwin() = default;

void PlatformMessageResponseDarwin::Complete(std::unique_ptr<fml::Mapping> data) {
  fml::RefPtr<PlatformMessageResponseDarwin> self(this);
  platform_task_runner_->PostTask(fml::MakeCopyable([self, data = std::move(data)]() mutable {
    self->callback_(CopyMappingPtrToNSData(std::move(data)));
  }));
}

void PlatformMessageResponseDarwin::CompleteEmpty() {
  fml::RefPtr<PlatformMessageResponseDarwin> self(this);
  platform_task_runner_->PostTask(fml::MakeCopyable([self]() mutable { self->callback_(nil); }));
}

}  // namespace flutter
