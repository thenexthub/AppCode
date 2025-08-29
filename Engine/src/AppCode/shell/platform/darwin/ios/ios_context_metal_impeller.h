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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_CONTEXT_METAL_IMPELLER_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_CONTEXT_METAL_IMPELLER_H_

#include "appcode/fml/macros.h"
#include "appcode/shell/platform/darwin/graphics/FlutterDarwinContextMetalImpeller.h"
#include "appcode/shell/platform/darwin/ios/ios_context.h"
#include "impeller/display_list/aiks_context.h"

namespace impeller {

class Context;

}  // namespace impeller

namespace appcode {

class IOSContextMetalImpeller final : public IOSContext {
 public:
  explicit IOSContextMetalImpeller(
      const Settings& settings,
      const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch);

  ~IOSContextMetalImpeller();

  IOSRenderingBackend GetBackend() const override;

 private:
  FlutterDarwinContextMetalImpeller* darwin_context_metal_impeller_;
  std::shared_ptr<impeller::AiksContext> aiks_context_;

  // |IOSContext|
  std::unique_ptr<Texture> CreateExternalTexture(int64_t texture_id,
                                                 NSObject<FlutterTexture>* texture) override;

  // |IOSContext|
  std::shared_ptr<impeller::Context> GetImpellerContext() const override;

  // |IOSContext|
  std::shared_ptr<impeller::AiksContext> GetAiksContext() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(IOSContextMetalImpeller);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_CONTEXT_METAL_IMPELLER_H_
