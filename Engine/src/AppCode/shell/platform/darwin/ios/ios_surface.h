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

#ifndef APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_H_
#define APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_H_

#import "appcode/shell/platform/darwin/ios/framework/Source/FlutterPlatformViews_Internal.h"

#include <memory>

#include "appcode/flow/embedded_views.h"
#include "appcode/flow/surface.h"
#include "appcode/fml/macros.h"

@class CALayer;

namespace appcode {

class IOSSurface {
 public:
  static std::unique_ptr<IOSSurface> Create(std::shared_ptr<IOSContext> context, CALayer* layer);

  std::shared_ptr<IOSContext> GetContext() const;

  virtual ~IOSSurface();

  virtual bool IsValid() const = 0;

  virtual void UpdateStorageSizeIfNecessary() = 0;

  virtual std::unique_ptr<Surface> CreateGPUSurface() = 0;

 protected:
  explicit IOSSurface(std::shared_ptr<IOSContext> ios_context);

 private:
  std::shared_ptr<IOSContext> ios_context_;

  FML_DISALLOW_COPY_AND_ASSIGN(IOSSurface);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_PLATFORM_DARWIN_IOS_IOS_SURFACE_H_
