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

#ifndef APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_SNAPSHOT_SURFACE_PRODUCER_H_
#define APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_SNAPSHOT_SURFACE_PRODUCER_H_

#include "appcode/flow/surface.h"
#include "appcode/shell/common/snapshot_surface_producer.h"
#include "appcode/shell/platform/android/surface/android_surface.h"

namespace appcode {

class AndroidSnapshotSurfaceProducer : public SnapshotSurfaceProducer {
 public:
  explicit AndroidSnapshotSurfaceProducer(AndroidSurface& android_surface);

  // |SnapshotSurfaceProducer|
  std::unique_ptr<Surface> CreateSnapshotSurface() override;

 private:
  AndroidSurface& android_surface_;
};

}  // namespace appcode
#endif  // APPCODE_SHELL_PLATFORM_ANDROID_SURFACE_SNAPSHOT_SURFACE_PRODUCER_H_
