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

#include "appcode/shell/platform/embedder/embedder_platform_message_response.h"

#include "appcode/fml/make_copyable.h"

namespace appcode {

EmbedderPlatformMessageResponse::EmbedderPlatformMessageResponse(
    fml::RefPtr<fml::TaskRunner> runner,
    const Callback& callback)
    : runner_(std::move(runner)), callback_(callback) {}

EmbedderPlatformMessageResponse::~EmbedderPlatformMessageResponse() = default;

// |PlatformMessageResponse|
void EmbedderPlatformMessageResponse::Complete(
    std::unique_ptr<fml::Mapping> data) {
  if (!data) {
    CompleteEmpty();
    return;
  }

  runner_->PostTask(
      // The static leak checker gets confused by the use of fml::MakeCopyable.
      // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
      fml::MakeCopyable([data = std::move(data), callback = callback_]() {
        callback(data->GetMapping(), data->GetSize());
      }));
}

// |PlatformMessageResponse|
void EmbedderPlatformMessageResponse::CompleteEmpty() {
  Complete(std::make_unique<fml::NonOwnedMapping>(nullptr, 0u));
}

}  // namespace appcode
