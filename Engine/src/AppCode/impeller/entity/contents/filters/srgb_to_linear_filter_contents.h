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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_SRGB_TO_LINEAR_FILTER_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_SRGB_TO_LINEAR_FILTER_CONTENTS_H_

#include "impeller/entity/contents/filters/color_filter_contents.h"
#include "impeller/entity/contents/filters/inputs/filter_input.h"

namespace impeller {

class SrgbToLinearFilterContents final : public ColorFilterContents {
 public:
  SrgbToLinearFilterContents();

  ~SrgbToLinearFilterContents() override;

 private:
  // |FilterContents|
  std::optional<Entity> RenderFilter(
      const FilterInput::Vector& input_textures,
      const ContentContext& renderer,
      const Entity& entity,
      const Matrix& effect_transform,
      const Rect& coverage,
      const std::optional<Rect>& coverage_hint) const override;

  SrgbToLinearFilterContents(const SrgbToLinearFilterContents&) = delete;

  SrgbToLinearFilterContents& operator=(const SrgbToLinearFilterContents&) =
      delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_SRGB_TO_LINEAR_FILTER_CONTENTS_H_
