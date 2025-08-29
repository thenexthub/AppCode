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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_BLUR_IMAGE_FILTER_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_BLUR_IMAGE_FILTER_H_

#include "appcode/display_list/effects/dl_image_filter.h"

#include "appcode/display_list/dl_tile_mode.h"

namespace appcode {

class DlBlurImageFilter final : public DlImageFilter {
 public:
  DlBlurImageFilter(DlScalar sigma_x, DlScalar sigma_y, DlTileMode tile_mode)
      : sigma_x_(sigma_x), sigma_y_(sigma_y), tile_mode_(tile_mode) {}
  explicit DlBlurImageFilter(const DlBlurImageFilter* filter)
      : DlBlurImageFilter(filter->sigma_x_,
                          filter->sigma_y_,
                          filter->tile_mode_) {}
  DlBlurImageFilter(const DlBlurImageFilter& filter)
      : DlBlurImageFilter(&filter) {}

  static std::shared_ptr<DlImageFilter> Make(DlScalar sigma_x,
                                             DlScalar sigma_y,
                                             DlTileMode tile_mode);

  std::shared_ptr<DlImageFilter> shared() const override {
    return std::make_shared<DlBlurImageFilter>(this);
  }

  DlImageFilterType type() const override { return DlImageFilterType::kBlur; }
  size_t size() const override { return sizeof(*this); }

  const DlBlurImageFilter* asBlur() const override { return this; }

  bool modifies_transparent_black() const override { return false; }

  DlRect* map_local_bounds(const DlRect& input_bounds,
                           DlRect& output_bounds) const override;

  DlIRect* map_device_bounds(const DlIRect& input_bounds,
                             const DlMatrix& ctm,
                             DlIRect& output_bounds) const override;

  DlIRect* get_input_device_bounds(const DlIRect& output_bounds,
                                   const DlMatrix& ctm,
                                   DlIRect& input_bounds) const override;

  DlScalar sigma_x() const { return sigma_x_; }
  DlScalar sigma_y() const { return sigma_y_; }
  DlTileMode tile_mode() const { return tile_mode_; }

 protected:
  bool equals_(const DlImageFilter& other) const override;

 private:
  DlScalar sigma_x_;
  DlScalar sigma_y_;
  DlTileMode tile_mode_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_BLUR_IMAGE_FILTER_H_
