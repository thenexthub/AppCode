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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_COLOR_MATRIX_FILTER_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_COLOR_MATRIX_FILTER_CONTENTS_H_

#include <memory>
#include <optional>

#include "impeller/entity/contents/filters/color_filter_contents.h"
#include "impeller/entity/contents/filters/inputs/filter_input.h"

namespace impeller {

// Look at example at: https://github.com/appcode/impeller/pull/132

class ColorMatrixFilterContents final : public ColorFilterContents {
 public:
  ColorMatrixFilterContents();

  ~ColorMatrixFilterContents() override;

  void SetMatrix(const ColorMatrix& matrix);

 private:
  // |FilterContents|
  std::optional<Entity> RenderFilter(
      const FilterInput::Vector& input_textures,
      const ContentContext& renderer,
      const Entity& entity,
      const Matrix& effect_transform,
      const Rect& coverage,
      const std::optional<Rect>& coverage_hint) const override;

  ColorMatrix matrix_;

  ColorMatrixFilterContents(const ColorMatrixFilterContents&) = delete;

  ColorMatrixFilterContents& operator=(const ColorMatrixFilterContents&) =
      delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_FILTERS_COLOR_MATRIX_FILTER_CONTENTS_H_
