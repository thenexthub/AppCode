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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_CONICAL_GRADIENT_COLOR_SOURCE_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_CONICAL_GRADIENT_COLOR_SOURCE_H_

#include "appcode/display_list/effects/color_sources/dl_gradient_color_source_base.h"

namespace appcode {

class DlConicalGradientColorSource final : public DlGradientColorSourceBase {
 public:
  const DlConicalGradientColorSource* asConicalGradient() const override {
    return this;
  }

  bool isUIThreadSafe() const override { return true; }

  std::shared_ptr<DlColorSource> shared() const override;

  DlColorSourceType type() const override {
    return DlColorSourceType::kConicalGradient;
  }
  size_t size() const override { return sizeof(*this) + vector_sizes(); }

  DlPoint start_center() const { return start_center_; }
  DlScalar start_radius() const { return start_radius_; }
  DlPoint end_center() const { return end_center_; }
  DlScalar end_radius() const { return end_radius_; }

 protected:
  virtual const void* pod() const override { return this + 1; }

  bool equals_(DlColorSource const& other) const override;

 private:
  template <typename Colors>
  DlConicalGradientColorSource(DlPoint start_center,
                               DlScalar start_radius,
                               DlPoint end_center,
                               DlScalar end_radius,
                               uint32_t stop_count,
                               Colors colors,
                               const float* stops,
                               DlTileMode tile_mode,
                               const DlMatrix* matrix = nullptr)
      : DlGradientColorSourceBase(stop_count, tile_mode, matrix),
        start_center_(start_center),
        start_radius_(start_radius),
        end_center_(end_center),
        end_radius_(end_radius) {
    store_color_stops(this + 1, colors, stops);
  }

  explicit DlConicalGradientColorSource(
      const DlConicalGradientColorSource* source);

  DlPoint start_center_;
  DlScalar start_radius_;
  DlPoint end_center_;
  DlScalar end_radius_;

  friend class DlColorSource;
  friend class DisplayListBuilder;

  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(DlConicalGradientColorSource);
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_CONICAL_GRADIENT_COLOR_SOURCE_H_
