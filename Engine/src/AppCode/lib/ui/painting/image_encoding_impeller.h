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

#ifndef APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_IMPELLER_H_
#define APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_IMPELLER_H_

#include "appcode/common/task_runners.h"
#include "appcode/display_list/image/dl_image.h"
#include "appcode/fml/status_or.h"
#include "appcode/fml/synchronization/sync_switch.h"

namespace impeller {
class Context;
}  // namespace impeller

namespace appcode {

class ImageEncodingImpeller {
 public:
  static int GetColorSpace(const std::shared_ptr<impeller::Texture>& texture);

  /// Converts a DlImage to a SkImage.
  /// This should be called from the thread that corresponds to
  /// `dl_image->owning_context()` when gpu access is guaranteed.
  /// See also: `ConvertImageToRaster`.
  /// Visible for testing.
  static void ConvertDlImageToSkImage(
      const sk_sp<DlImage>& dl_image,
      std::function<void(fml::StatusOr<sk_sp<SkImage>>)> encode_task,
      const std::shared_ptr<impeller::Context>& impeller_context);

  /// Converts a DlImage to a SkImage.
  /// `encode_task` is executed with the resulting `SkImage`.
  static void ConvertImageToRaster(
      const sk_sp<DlImage>& dl_image,
      std::function<void(fml::StatusOr<sk_sp<SkImage>>)> encode_task,
      const fml::RefPtr<fml::TaskRunner>& raster_task_runner,
      const fml::RefPtr<fml::TaskRunner>& io_task_runner,
      const std::shared_ptr<const fml::SyncSwitch>& is_gpu_disabled_sync_switch,
      const std::shared_ptr<impeller::Context>& impeller_context);
};
}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_IMAGE_ENCODING_IMPELLER_H_
