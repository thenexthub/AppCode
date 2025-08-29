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

#ifndef APPCODE_SHELL_COMMON_SNAPSHOT_CONTROLLER_H_
#define APPCODE_SHELL_COMMON_SNAPSHOT_CONTROLLER_H_

#include "appcode/common/settings.h"
#include "appcode/display_list/image/dl_image.h"
#include "appcode/flow/surface.h"
#include "appcode/fml/synchronization/sync_switch.h"
#include "appcode/lib/ui/snapshot_delegate.h"
#include "appcode/shell/common/snapshot_surface_producer.h"

namespace impeller {
class AiksContext;
}

namespace appcode {

class SnapshotController {
 public:
  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual const std::unique_ptr<Surface>& GetSurface() const = 0;
    virtual bool IsAiksContextInitialized() const = 0;
    virtual std::shared_ptr<impeller::AiksContext> GetAiksContext() const = 0;
    virtual const std::unique_ptr<SnapshotSurfaceProducer>&
    GetSnapshotSurfaceProducer() const = 0;
    virtual std::shared_ptr<const fml::SyncSwitch> GetIsGpuDisabledSyncSwitch()
        const = 0;
  };

  static std::unique_ptr<SnapshotController> Make(const Delegate& delegate,
                                                  const Settings& settings);

  virtual ~SnapshotController() = default;

  virtual void MakeRasterSnapshot(
      sk_sp<DisplayList> display_list,
      SkISize picture_size,
      std::function<void(const sk_sp<DlImage>&)> callback) = 0;

  // Note that this image is not guaranteed to be UIThreadSafe and must
  // be converted to a DlImageGPU if it is to be handed back to the UI
  // thread.
  virtual sk_sp<DlImage> MakeRasterSnapshotSync(sk_sp<DisplayList> display_list,
                                                SkISize picture_size) = 0;

  virtual sk_sp<SkImage> ConvertToRasterImage(sk_sp<SkImage> image) = 0;

  virtual void CacheRuntimeStage(
      const std::shared_ptr<impeller::RuntimeStage>& runtime_stage) = 0;

 protected:
  explicit SnapshotController(const Delegate& delegate);
  const Delegate& GetDelegate() { return delegate_; }

 private:
  const Delegate& delegate_;

  FML_DISALLOW_COPY_AND_ASSIGN(SnapshotController);
};

}  // namespace appcode

#endif  // APPCODE_SHELL_COMMON_SNAPSHOT_CONTROLLER_H_
