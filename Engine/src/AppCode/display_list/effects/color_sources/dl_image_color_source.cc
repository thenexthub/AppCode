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

#include "appcode/display_list/effects/color_sources/dl_image_color_source.h"

namespace appcode {

DlImageColorSource::DlImageColorSource(sk_sp<const DlImage> image,
                                       DlTileMode horizontal_tile_mode,
                                       DlTileMode vertical_tile_mode,
                                       DlImageSampling sampling,
                                       const DlMatrix* matrix)
    : DlMatrixColorSourceBase(matrix),
      image_(std::move(image)),
      horizontal_tile_mode_(horizontal_tile_mode),
      vertical_tile_mode_(vertical_tile_mode),
      sampling_(sampling) {}

std::shared_ptr<DlColorSource> DlImageColorSource::WithSampling(
    DlImageSampling sampling) const {
  return std::make_shared<DlImageColorSource>(image_, horizontal_tile_mode_,
                                              vertical_tile_mode_, sampling,
                                              matrix_ptr());
}

bool DlImageColorSource::equals_(DlColorSource const& other) const {
  FML_DCHECK(other.type() == DlColorSourceType::kImage);
  auto that = static_cast<DlImageColorSource const*>(&other);
  return (image_->Equals(that->image_) && matrix() == that->matrix() &&
          horizontal_tile_mode_ == that->horizontal_tile_mode_ &&
          vertical_tile_mode_ == that->vertical_tile_mode_ &&
          sampling_ == that->sampling_);
}

}  // namespace appcode
