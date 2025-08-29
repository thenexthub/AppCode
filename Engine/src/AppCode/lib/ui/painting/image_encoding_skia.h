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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_SKIA_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_SKIA_H_

#if !SLIMPELLER

#include "appcode/common/task_runners.h"
#include "appcode/display_list/image/dl_image.h"
#include "appcode/fml/synchronization/sync_switch.h"
#include "appcode/lib/ui/snapshot_delegate.h"

namespace appcode {

void ConvertImageToRasterSkia(
    const sk_sp<DlImage>& dl_image,
    std::function<void(sk_sp<SkImage>)> encode_task,
    const fml::RefPtr<fml::TaskRunner>& raster_task_runner,
    const fml::RefPtr<fml::TaskRunner>& io_task_runner,
    const fml::WeakPtr<GrDirectContext>& resource_context,
    const fml::TaskRunnerAffineWeakPtr<SnapshotDelegate>& snapshot_delegate,
    const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch);

}  // namespace appcode

#endif  //  !SLIMPELLER

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_SKIA_H_
