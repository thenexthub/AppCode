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

#ifndef APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_LINEAR_GRADIENT_COLOR_SOURCE_H_
#define APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_LINEAR_GRADIENT_COLOR_SOURCE_H_

#include "appcode/display_list/effects/color_sources/dl_gradient_color_source_base.h"

namespace appcode {

class DlLinearGradientColorSource final : public DlGradientColorSourceBase {
 public:
  const DlLinearGradientColorSource* asLinearGradient() const override {
    return this;
  }

  bool isUIThreadSafe() const override { return true; }

  DlColorSourceType type() const override {
    return DlColorSourceType::kLinearGradient;
  }
  size_t size() const override { return sizeof(*this) + vector_sizes(); }

  std::shared_ptr<DlColorSource> shared() const override;

  const DlPoint& start_point() const { return start_point_; }
  const DlPoint& end_point() const { return end_point_; }

 protected:
  virtual const void* pod() const override { return this + 1; }

  bool equals_(DlColorSource const& other) const override;

 private:
  template <typename Colors>
  DlLinearGradientColorSource(const DlPoint start_point,
                              const DlPoint end_point,
                              uint32_t stop_count,
                              Colors colors,
                              const float* stops,
                              DlTileMode tile_mode,
                              const DlMatrix* matrix = nullptr)
      : DlGradientColorSourceBase(stop_count, tile_mode, matrix),
        start_point_(start_point),
        end_point_(end_point) {
    store_color_stops(this + 1, colors, stops);
  }

  explicit DlLinearGradientColorSource(
      const DlLinearGradientColorSource* source);

  DlPoint start_point_;
  DlPoint end_point_;

  friend class DlColorSource;
  friend class DisplayListBuilder;

  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(DlLinearGradientColorSource);
};

}  // namespace appcode

#endif  // APPCODE_DISPLAY_LIST_EFFECTS_COLOR_SOURCES_DL_LINEAR_GRADIENT_COLOR_SOURCE_H_
