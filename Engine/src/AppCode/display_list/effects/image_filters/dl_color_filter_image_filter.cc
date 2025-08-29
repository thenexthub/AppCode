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

#include "appcode/display_list/effects/image_filters/dl_color_filter_image_filter.h"

#include "appcode/display_list/utils/dl_comparable.h"

namespace appcode {

std::shared_ptr<DlImageFilter> DlColorFilterImageFilter::Make(
    const std::shared_ptr<const DlColorFilter>& filter) {
  if (filter) {
    return std::make_shared<DlColorFilterImageFilter>(filter);
  }
  return nullptr;
}

bool DlColorFilterImageFilter::modifies_transparent_black() const {
  if (color_filter_) {
    return color_filter_->modifies_transparent_black();
  }
  return false;
}

DlRect* DlColorFilterImageFilter::map_local_bounds(
    const DlRect& input_bounds,
    DlRect& output_bounds) const {
  output_bounds = input_bounds;
  return modifies_transparent_black() ? nullptr : &output_bounds;
}

DlIRect* DlColorFilterImageFilter::map_device_bounds(
    const DlIRect& input_bounds,
    const DlMatrix& ctm,
    DlIRect& output_bounds) const {
  output_bounds = input_bounds;
  return modifies_transparent_black() ? nullptr : &output_bounds;
}

DlIRect* DlColorFilterImageFilter::get_input_device_bounds(
    const DlIRect& output_bounds,
    const DlMatrix& ctm,
    DlIRect& input_bounds) const {
  return map_device_bounds(output_bounds, ctm, input_bounds);
}

bool DlColorFilterImageFilter::equals_(const DlImageFilter& other) const {
  FML_DCHECK(other.type() == DlImageFilterType::kColorFilter);
  auto that = static_cast<const DlColorFilterImageFilter*>(&other);
  return Equals(color_filter_, that->color_filter_);
}

}  // namespace appcode
