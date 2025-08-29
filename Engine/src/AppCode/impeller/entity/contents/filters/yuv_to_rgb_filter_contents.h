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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_YUV_TO_RGB_FILTER_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_YUV_TO_RGB_FILTER_CONTENTS_H_

#include "impeller/entity/contents/filters/filter_contents.h"

namespace impeller {

class YUVToRGBFilterContents final : public FilterContents {
 public:
  YUVToRGBFilterContents();

  ~YUVToRGBFilterContents() override;

  void SetYUVColorSpace(YUVColorSpace yuv_color_space);

 private:
  // |FilterContents|
  std::optional<Entity> RenderFilter(
      const FilterInput::Vector& input_textures,
      const ContentContext& renderer,
      const Entity& entity,
      const Matrix& effect_transform,
      const Rect& coverage,
      const std::optional<Rect>& coverage_hint) const override;

  // |FilterContents|
  std::optional<Rect> GetFilterSourceCoverage(
      const Matrix& effect_transform,
      const Rect& output_limit) const override;

  YUVColorSpace yuv_color_space_ = YUVColorSpace::kBT601LimitedRange;

  YUVToRGBFilterContents(const YUVToRGBFilterContents&) = delete;

  YUVToRGBFilterContents& operator=(const YUVToRGBFilterContents&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_YUV_TO_RGB_FILTER_CONTENTS_H_
