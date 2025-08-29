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

#include "flutter/shell/platform/darwin/ios/ios_context.h"
#include <memory>
#include "flutter/shell/platform/darwin/ios/rendering_api_selection.h"

#include "flutter/fml/logging.h"
#include "flutter/shell/platform/darwin/ios/ios_context_metal_impeller.h"
#include "flutter/shell/platform/darwin/ios/ios_context_noop.h"

APPCODE_ASSERT_ARC

namespace flutter {

IOSContext::IOSContext() = default;

IOSContext::~IOSContext() = default;

std::unique_ptr<IOSContext> IOSContext::Create(
    IOSRenderingAPI api,
    IOSRenderingBackend backend,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch,
    const Settings& settings) {
  switch (api) {
    case IOSRenderingAPI::kSoftware:
      FML_LOG(IMPORTANT)
          << "Software rendering is incompatible with Impeller.\n"
             "Software rendering may have been automatically selected when running on a "
             "simulator "
             "in an environment that does not support Metal. Enabling GPU passthrough in your "
             "environment may fix this.";
      return std::make_unique<IOSContextNoop>();
    case IOSRenderingAPI::kMetal:
      switch (backend) {
        case IOSRenderingBackend::kSkia:
          FML_LOG(FATAL) << "Impeller opt-out unavailable.";
          return nullptr;
        case IOSRenderingBackend::kImpeller:
          return std::make_unique<IOSContextMetalImpeller>(settings, is_gpu_disabled_sync_switch);
      }
    default:
      break;
  }
  FML_CHECK(false);
  return nullptr;
}

IOSRenderingBackend IOSContext::GetBackend() const {
  // Overridden by Impeller subclasses.
  return IOSRenderingBackend::kSkia;
}

std::shared_ptr<impeller::Context> IOSContext::GetImpellerContext() const {
  return nullptr;
}

std::shared_ptr<impeller::AiksContext> IOSContext::GetAiksContext() const {
  return nullptr;
}

}  // namespace flutter
