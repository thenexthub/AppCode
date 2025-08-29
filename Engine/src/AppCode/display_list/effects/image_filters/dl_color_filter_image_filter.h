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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_COLOR_FILTER_IMAGE_FILTER_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_COLOR_FILTER_IMAGE_FILTER_H_

#include "display_list/effects/dl_image_filter.h"

#include "appcode/display_list/effects/dl_color_filter.h"

namespace appcode {

class DlColorFilterImageFilter final : public DlImageFilter {
 public:
  explicit DlColorFilterImageFilter(std::shared_ptr<const DlColorFilter> filter)
      : color_filter_(std::move(filter)) {}
  explicit DlColorFilterImageFilter(const DlColorFilter* filter)
      : color_filter_(filter->shared()) {}
  explicit DlColorFilterImageFilter(const DlColorFilter& filter)
      : color_filter_(filter.shared()) {}
  explicit DlColorFilterImageFilter(const DlColorFilterImageFilter* filter)
      : DlColorFilterImageFilter(filter->color_filter_) {}
  DlColorFilterImageFilter(const DlColorFilterImageFilter& filter)
      : DlColorFilterImageFilter(&filter) {}

  static std::shared_ptr<DlImageFilter> Make(
      const std::shared_ptr<const DlColorFilter>& filter);

  std::shared_ptr<DlImageFilter> shared() const override {
    return std::make_shared<DlColorFilterImageFilter>(color_filter_);
  }

  DlImageFilterType type() const override {
    return DlImageFilterType::kColorFilter;
  }
  size_t size() const override { return sizeof(*this); }

  const std::shared_ptr<const DlColorFilter> color_filter() const {
    return color_filter_;
  }

  const DlColorFilterImageFilter* asColorFilter() const override {
    return this;
  }

  bool modifies_transparent_black() const override;

  DlRect* map_local_bounds(const DlRect& input_bounds,
                           DlRect& output_bounds) const override;

  DlIRect* map_device_bounds(const DlIRect& input_bounds,
                             const DlMatrix& ctm,
                             DlIRect& output_bounds) const override;

  DlIRect* get_input_device_bounds(const DlIRect& output_bounds,
                                   const DlMatrix& ctm,
                                   DlIRect& input_bounds) const override;

  MatrixCapability matrix_capability() const override {
    return MatrixCapability::kComplex;
  }

 protected:
  bool equals_(const DlImageFilter& other) const override;

 private:
  std::shared_ptr<const DlColorFilter> color_filter_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_COLOR_FILTER_IMAGE_FILTER_H_
