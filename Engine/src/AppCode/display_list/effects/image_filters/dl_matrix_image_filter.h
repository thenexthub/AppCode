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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_MATRIX_IMAGE_FILTER_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_MATRIX_IMAGE_FILTER_H_

#include "display_list/effects/dl_image_filter.h"

#include "appcode/display_list/dl_sampling_options.h"

namespace appcode {

class DlMatrixImageFilter final : public DlImageFilter {
 public:
  DlMatrixImageFilter(const DlMatrix& matrix, DlImageSampling sampling)
      : matrix_(matrix), sampling_(sampling) {}
  explicit DlMatrixImageFilter(const DlMatrixImageFilter* filter)
      : DlMatrixImageFilter(filter->matrix_, filter->sampling_) {}
  DlMatrixImageFilter(const DlMatrixImageFilter& filter)
      : DlMatrixImageFilter(&filter) {}

  static std::shared_ptr<DlImageFilter> Make(const DlMatrix& matrix,
                                             DlImageSampling sampling);

  std::shared_ptr<DlImageFilter> shared() const override {
    return std::make_shared<DlMatrixImageFilter>(this);
  }

  DlImageFilterType type() const override { return DlImageFilterType::kMatrix; }
  size_t size() const override { return sizeof(*this); }

  const DlMatrix& matrix() const { return matrix_; }
  DlImageSampling sampling() const { return sampling_; }

  const DlMatrixImageFilter* asMatrix() const override { return this; }

  bool modifies_transparent_black() const override { return false; }

  DlRect* map_local_bounds(const DlRect& input_bounds,
                           DlRect& output_bounds) const override;

  DlIRect* map_device_bounds(const DlIRect& input_bounds,
                             const DlMatrix& ctm,
                             DlIRect& output_bounds) const override;

  DlIRect* get_input_device_bounds(const DlIRect& output_bounds,
                                   const DlMatrix& ctm,
                                   DlIRect& input_bounds) const override;

 protected:
  bool equals_(const DlImageFilter& other) const override;

 private:
  DlMatrix matrix_;
  DlImageSampling sampling_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_MATRIX_IMAGE_FILTER_H_
