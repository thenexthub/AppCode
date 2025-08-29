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

#ifndef APPCODE_LIB_UI_IO_MANAGER_H_
#define APPCODE_LIB_UI_IO_MANAGER_H_

#include "appcode/flow/skia_gpu_object.h"
#include "appcode/fml/memory/weak_ptr.h"
#include "appcode/fml/synchronization/sync_switch.h"
#include "third_party/skia/include/gpu/ganesh/GrDirectContext.h"

namespace impeller {
class Context;
}  // namespace impeller

namespace appcode {
// Interface for methods that manage access to the resource GrDirectContext and
// Skia unref queue.  Meant to be implemented by the owner of the resource
// GrDirectContext, i.e. the shell's IOManager.
class IOManager {
 public:
  virtual ~IOManager() = default;

  virtual fml::WeakPtr<IOManager> GetWeakIOManager() const = 0;

  virtual fml::WeakPtr<GrDirectContext> GetResourceContext() const = 0;

  virtual fml::RefPtr<appcode::SkiaUnrefQueue> GetSkiaUnrefQueue() const = 0;

  virtual std::shared_ptr<const fml::SyncSwitch>
  GetIsGpuDisabledSyncSwitch() = 0;

  /// @brief Retrieve the impeller::Context.
  ///
  /// Call may block until context is availabe.
  virtual std::shared_ptr<impeller::Context> GetImpellerContext() const = 0;
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_IO_MANAGER_H_
