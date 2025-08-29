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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_BORDER_MASK_BLUR_FILTER_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_BORDER_MASK_BLUR_FILTER_CONTENTS_H_

#include <memory>
#include <optional>
#include "impeller/entity/contents/filters/filter_contents.h"
#include "impeller/entity/contents/filters/inputs/filter_input.h"

namespace impeller {

class BorderMaskBlurFilterContents final : public FilterContents {
 public:
  BorderMaskBlurFilterContents();

  ~BorderMaskBlurFilterContents() override;

  void SetSigma(Sigma sigma_x, Sigma sigma_y);

  void SetBlurStyle(BlurStyle blur_style);

  // |FilterContents|
  std::optional<Rect> GetFilterCoverage(
      const FilterInput::Vector& inputs,
      const Entity& entity,
      const Matrix& effect_transform) const override;

  // |FilterContents|
  std::optional<Rect> GetFilterSourceCoverage(
      const Matrix& effect_transform,
      const Rect& output_limit) const override;

 private:
  // |FilterContents|
  std::optional<Entity> RenderFilter(
      const FilterInput::Vector& input_textures,
      const ContentContext& renderer,
      const Entity& entity,
      const Matrix& effect_transform,
      const Rect& coverage,
      const std::optional<Rect>& coverage_hint) const override;

  Sigma sigma_x_;
  Sigma sigma_y_;
  BlurStyle blur_style_ = BlurStyle::kNormal;
  bool src_color_factor_ = false;
  bool inner_blur_factor_ = true;
  bool outer_blur_factor_ = true;

  BorderMaskBlurFilterContents(const BorderMaskBlurFilterContents&) = delete;

  BorderMaskBlurFilterContents& operator=(const BorderMaskBlurFilterContents&) =
      delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_BORDER_MASK_BLUR_FILTER_CONTENTS_H_
