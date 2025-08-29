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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_NOOP_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_NOOP_H_

#import "appcode/shell/platform/darwin/ios/ios_context.h"
#import "appcode/shell/platform/darwin/ios/ios_surface.h"

@class CALayer;

namespace appcode {

/// @brief A rendering surface that accepts rendering intent but does not render
///        anything.
///
/// This is useful for running on platforms that need an engine instance and
/// don't have the required drivers.
class IOSSurfaceNoop final : public IOSSurface {
 public:
  explicit IOSSurfaceNoop(std::shared_ptr<IOSContext> context);

  ~IOSSurfaceNoop() override;

  // |IOSSurface|
  bool IsValid() const override;

  // |IOSSurface|
  void UpdateStorageSizeIfNecessary() override;

  // |IOSSurface|
  std::unique_ptr<Surface> CreateGPUSurface() override;

 private:
  IOSSurfaceNoop(const IOSSurfaceNoop&) = delete;

  IOSSurfaceNoop& operator=(const IOSSurfaceNoop&) = delete;
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_NOOP_H_
