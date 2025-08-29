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

#include "impeller/toolkit/interop/image_filter.h"

#include "appcode/display_list/effects/dl_image_filters.h"

namespace impeller::interop {

ImageFilter::ImageFilter(std::shared_ptr<appcode::DlImageFilter> filter)
    : filter_(std::move(filter)) {}

ImageFilter::~ImageFilter() = default;

ScopedObject<ImageFilter> ImageFilter::MakeBlur(Scalar x_sigma,
                                                Scalar y_sigma,
                                                appcode::DlTileMode tile_mode) {
  auto filter = appcode::DlBlurImageFilter::Make(x_sigma, y_sigma, tile_mode);
  if (!filter) {
    return nullptr;
  }
  return Create<ImageFilter>(std::move(filter));
}

ScopedObject<ImageFilter> ImageFilter::MakeDilate(Scalar x_radius,
                                                  Scalar y_radius) {
  auto filter = appcode::DlDilateImageFilter::Make(x_radius, y_radius);
  if (!filter) {
    return nullptr;
  }
  return Create<ImageFilter>(std::move(filter));
}

ScopedObject<ImageFilter> ImageFilter::MakeErode(Scalar x_radius,
                                                 Scalar y_radius) {
  auto filter = appcode::DlErodeImageFilter::Make(x_radius, y_radius);
  if (!filter) {
    return nullptr;
  }
  return Create<ImageFilter>(std::move(filter));
}

ScopedObject<ImageFilter> ImageFilter::MakeMatrix(
    const Matrix& matrix,
    appcode::DlImageSampling sampling) {
  auto filter = appcode::DlMatrixImageFilter::Make(matrix, sampling);
  if (!filter) {
    return nullptr;
  }
  return Create<ImageFilter>(std::move(filter));
}

ScopedObject<ImageFilter> ImageFilter::MakeCompose(const ImageFilter& outer,
                                                   const ImageFilter& inner) {
  auto filter = appcode::DlComposeImageFilter::Make(outer.GetImageFilter(),
                                                    inner.GetImageFilter());
  if (!filter) {
    return nullptr;
  }
  return Create<ImageFilter>(std::move(filter));
}

const std::shared_ptr<appcode::DlImageFilter>& ImageFilter::GetImageFilter()
    const {
  return filter_;
}

}  // namespace impeller::interop
