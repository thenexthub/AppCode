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

#ifndef APPCODE_SHELL_COMMON_SNAPSHOT_CONTROLLER_SKIA_H_
#define APPCODE_SHELL_COMMON_SNAPSHOT_CONTROLLER_SKIA_H_

#if !SLIMPELLER

#include "appcode/shell/common/snapshot_controller.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace appcode {

class SnapshotControllerSkia : public SnapshotController {
 public:
  explicit SnapshotControllerSkia(const SnapshotController::Delegate& delegate)
      : SnapshotController(delegate) {}

  void MakeRasterSnapshot(
      sk_sp<DisplayList> display_list,
      SkISize picture_size,
      std::function<void(const sk_sp<DlImage>&)> callback) override;

  sk_sp<DlImage> MakeRasterSnapshotSync(sk_sp<DisplayList> display_list,
                                        SkISize size) override;

  virtual sk_sp<SkImage> ConvertToRasterImage(sk_sp<SkImage> image) override;

  void CacheRuntimeStage(
      const std::shared_ptr<impeller::RuntimeStage>& runtime_stage) override;

 private:
  sk_sp<DlImage> DoMakeRasterSnapshot(
      SkISize size,
      std::function<void(SkCanvas*)> draw_callback);

  FML_DISALLOW_COPY_AND_ASSIGN(SnapshotControllerSkia);
};

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_SHELL_COMMON_SNAPSHOT_CONTROLLER_SKIA_H_
