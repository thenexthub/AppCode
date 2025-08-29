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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_IMAGE_COLOR_SOURCE_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_IMAGE_COLOR_SOURCE_H_

#include "appcode/display_list/effects/color_sources/dl_matrix_color_source_base.h"

namespace appcode {

class DlImageColorSource final : public DlMatrixColorSourceBase {
 public:
  DlImageColorSource(sk_sp<const DlImage> image,
                     DlTileMode horizontal_tile_mode,
                     DlTileMode vertical_tile_mode,
                     DlImageSampling sampling = DlImageSampling::kLinear,
                     const DlMatrix* matrix = nullptr);

  bool isUIThreadSafe() const override {
    return image_ ? image_->isUIThreadSafe() : true;
  }

  const DlImageColorSource* asImage() const override { return this; }

  std::shared_ptr<DlColorSource> shared() const override {
    return WithSampling(sampling_);
  }

  std::shared_ptr<DlColorSource> WithSampling(DlImageSampling sampling) const;

  DlColorSourceType type() const override { return DlColorSourceType::kImage; }
  size_t size() const override { return sizeof(*this); }

  bool is_opaque() const override { return image_->isOpaque(); }

  sk_sp<const DlImage> image() const { return image_; }
  DlTileMode horizontal_tile_mode() const { return horizontal_tile_mode_; }
  DlTileMode vertical_tile_mode() const { return vertical_tile_mode_; }
  DlImageSampling sampling() const { return sampling_; }

 protected:
  bool equals_(DlColorSource const& other) const override;

 private:
  sk_sp<const DlImage> image_;
  DlTileMode horizontal_tile_mode_;
  DlTileMode vertical_tile_mode_;
  DlImageSampling sampling_;

  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(DlImageColorSource);
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_IMAGE_COLOR_SOURCE_H_
