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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_COLOR_FILTERS_DL_LINEAR_TO_SRGB_GAMMA_COLOR_FILTER_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_COLOR_FILTERS_DL_LINEAR_TO_SRGB_GAMMA_COLOR_FILTER_H_

#include "appcode/display_list/effects/dl_color_filter.h"

namespace appcode {

// The LinearToSrgb type of ColorFilter that applies the sRGB gamma curve
// to the rendered pixels.
class DlLinearToSrgbGammaColorFilter final : public DlColorFilter {
 public:
  DlLinearToSrgbGammaColorFilter() {}
  DlLinearToSrgbGammaColorFilter(const DlLinearToSrgbGammaColorFilter& filter)
      : DlLinearToSrgbGammaColorFilter() {}
  explicit DlLinearToSrgbGammaColorFilter(
      const DlLinearToSrgbGammaColorFilter* filter)
      : DlLinearToSrgbGammaColorFilter() {}

  DlColorFilterType type() const override {
    return DlColorFilterType::kLinearToSrgbGamma;
  }
  size_t size() const override { return sizeof(*this); }
  bool modifies_transparent_black() const override { return false; }
  bool can_commute_with_opacity() const override { return true; }

  std::shared_ptr<DlColorFilter> shared() const override { return kInstance; }

 protected:
  bool equals_(const DlColorFilter& other) const override {
    FML_DCHECK(other.type() == DlColorFilterType::kLinearToSrgbGamma);
    return true;
  }

 private:
  static const std::shared_ptr<DlLinearToSrgbGammaColorFilter> kInstance;

  friend class DlColorFilter;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_COLOR_FILTERS_DL_LINEAR_TO_SRGB_GAMMA_COLOR_FILTER_H_
