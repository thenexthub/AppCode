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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_COLOR_FILTERS_DL_BLEND_COLOR_FILTER_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_COLOR_FILTERS_DL_BLEND_COLOR_FILTER_H_

#include "appcode/display_list/effects/dl_color_filter.h"

namespace appcode {

// The Blend type of ColorFilter which specifies modifying the
// colors as if the color specified in the Blend filter is the
// source color and the color drawn by the rendering operation
// is the destination color. The mode parameter of the Blend
// filter is then used to combine those colors.
class DlBlendColorFilter final : public DlColorFilter {
 public:
  DlBlendColorFilter(DlColor color, DlBlendMode mode)
      : color_(color), mode_(mode) {}
  DlBlendColorFilter(const DlBlendColorFilter& filter)
      : DlBlendColorFilter(filter.color_, filter.mode_) {}
  explicit DlBlendColorFilter(const DlBlendColorFilter* filter)
      : DlBlendColorFilter(filter->color_, filter->mode_) {}

  DlColorFilterType type() const override { return DlColorFilterType::kBlend; }
  size_t size() const override { return sizeof(*this); }

  bool modifies_transparent_black() const override;
  bool can_commute_with_opacity() const override;

  std::shared_ptr<DlColorFilter> shared() const override {
    return std::make_shared<DlBlendColorFilter>(this);
  }

  const DlBlendColorFilter* asBlend() const override { return this; }

  DlColor color() const { return color_; }
  DlBlendMode mode() const { return mode_; }

 protected:
  bool equals_(DlColorFilter const& other) const override {
    FML_DCHECK(other.type() == DlColorFilterType::kBlend);
    auto that = static_cast<DlBlendColorFilter const*>(&other);
    return color_ == that->color_ && mode_ == that->mode_;
  }

 private:
  static std::shared_ptr<const DlColorFilter> Make(DlColor color,
                                                   DlBlendMode mode);

  DlColor color_;
  DlBlendMode mode_;

  friend class DlColorFilter;
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_COLOR_FILTERS_DL_BLEND_COLOR_FILTER_H_
