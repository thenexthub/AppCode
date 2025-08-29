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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_IMAGE_FILTER_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_IMAGE_FILTER_H_

#include "appcode/display_list/effects/dl_image_filter.h"
#include "impeller/toolkit/interop/formats.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"

namespace impeller::interop {

class ImageFilter final
    : public Object<ImageFilter,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerImageFilter)> {
 public:
  static ScopedObject<ImageFilter> MakeBlur(Scalar x_sigma,
                                            Scalar y_sigma,
                                            appcode::DlTileMode tile_mode);

  static ScopedObject<ImageFilter> MakeDilate(Scalar x_radius, Scalar y_radius);

  static ScopedObject<ImageFilter> MakeErode(Scalar x_radius, Scalar y_radius);

  static ScopedObject<ImageFilter> MakeMatrix(
      const Matrix& matrix,
      appcode::DlImageSampling sampling);

  static ScopedObject<ImageFilter> MakeCompose(const ImageFilter& outer,
                                               const ImageFilter& inner);

  explicit ImageFilter(std::shared_ptr<appcode::DlImageFilter> filter);

  ~ImageFilter() override;

  ImageFilter(const ImageFilter&) = delete;

  ImageFilter& operator=(const ImageFilter&) = delete;

  const std::shared_ptr<appcode::DlImageFilter>& GetImageFilter() const;

 private:
  std::shared_ptr<appcode::DlImageFilter> filter_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_IMAGE_FILTER_H_
