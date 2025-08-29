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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_CONTEXT_NOOP_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_CONTEXT_NOOP_H_

#import "appcode/shell/platform/darwin/ios/ios_context.h"

namespace appcode {

/// @brief A noop rendering context for usage on simulators without metal support.
class IOSContextNoop final : public IOSContext {
 public:
  IOSContextNoop();

  // |IOSContext|
  ~IOSContextNoop();

  // |IOSContext|
  std::unique_ptr<Texture> CreateExternalTexture(int64_t texture_id,
                                                 NSObject<FlutterTexture>* texture) override;

  IOSRenderingBackend GetBackend() const override;

 private:
  IOSContextNoop(const IOSContextNoop&) = delete;

  IOSContextNoop& operator=(const IOSContextNoop&) = delete;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_CONTEXT_NOOP_H_
