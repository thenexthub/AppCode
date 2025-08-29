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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_COMPOSE_IMAGE_FILTER_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_COMPOSE_IMAGE_FILTER_H_

#include "display_list/effects/dl_image_filter.h"

namespace appcode {

class DlComposeImageFilter final : public DlImageFilter {
 public:
  DlComposeImageFilter(const std::shared_ptr<DlImageFilter>& outer,
                       const std::shared_ptr<DlImageFilter>& inner)
      : outer_(outer), inner_(inner) {}
  DlComposeImageFilter(const DlImageFilter* outer, const DlImageFilter* inner)
      : outer_(outer->shared()), inner_(inner->shared()) {}
  DlComposeImageFilter(const DlImageFilter& outer, const DlImageFilter& inner)
      : DlComposeImageFilter(&outer, &inner) {}
  explicit DlComposeImageFilter(const DlComposeImageFilter* filter)
      : DlComposeImageFilter(filter->outer_, filter->inner_) {}
  DlComposeImageFilter(const DlComposeImageFilter& filter)
      : DlComposeImageFilter(&filter) {}

  static std::shared_ptr<DlImageFilter> Make(
      const std::shared_ptr<DlImageFilter>& outer,
      const std::shared_ptr<DlImageFilter>& inner);

  std::shared_ptr<DlImageFilter> shared() const override {
    return std::make_shared<DlComposeImageFilter>(this);
  }

  DlImageFilterType type() const override {
    return DlImageFilterType::kCompose;
  }
  size_t size() const override { return sizeof(*this); }

  std::shared_ptr<DlImageFilter> outer() const { return outer_; }
  std::shared_ptr<DlImageFilter> inner() const { return inner_; }

  const DlComposeImageFilter* asCompose() const override { return this; }

  bool modifies_transparent_black() const override;

  DlRect* map_local_bounds(const DlRect& input_bounds,
                           DlRect& output_bounds) const override;

  DlIRect* map_device_bounds(const DlIRect& input_bounds,
                             const DlMatrix& ctm,
                             DlIRect& output_bounds) const override;

  DlIRect* get_input_device_bounds(const DlIRect& output_bounds,
                                   const DlMatrix& ctm,
                                   DlIRect& input_bounds) const override;

  MatrixCapability matrix_capability() const override;

 protected:
  bool equals_(const DlImageFilter& other) const override;

 private:
  const std::shared_ptr<DlImageFilter> outer_;
  const std::shared_ptr<DlImageFilter> inner_;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_IMAGE_FILTERS_DL_COMPOSE_IMAGE_FILTER_H_
