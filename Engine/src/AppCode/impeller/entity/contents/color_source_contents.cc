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

#include "impeller/entity/contents/color_source_contents.h"

#include "impeller/entity/entity.h"
#include "impeller/geometry/matrix.h"

namespace impeller {

ColorSourceContents::ColorSourceContents() = default;

ColorSourceContents::~ColorSourceContents() = default;

void ColorSourceContents::SetGeometry(const Geometry* geometry) {
  geometry_ = geometry;
}

const Geometry* ColorSourceContents::GetGeometry() const {
  return geometry_;
}

void ColorSourceContents::SetOpacityFactor(Scalar alpha) {
  opacity_ = alpha;
}

Scalar ColorSourceContents::GetOpacityFactor() const {
  return opacity_ * inherited_opacity_;
}

void ColorSourceContents::SetEffectTransform(Matrix matrix) {
  inverse_matrix_ = matrix.Invert();
}

const Matrix& ColorSourceContents::GetInverseEffectTransform() const {
  return inverse_matrix_;
}

bool ColorSourceContents::IsSolidColor() const {
  return false;
}

std::optional<Rect> ColorSourceContents::GetCoverage(
    const Entity& entity) const {
  return geometry_->GetCoverage(entity.GetTransform());
};

void ColorSourceContents::SetInheritedOpacity(Scalar opacity) {
  inherited_opacity_ = opacity;
}

bool ColorSourceContents::AppliesAlphaForStrokeCoverage(
    const Matrix& transform) const {
  return GetGeometry() && GetGeometry()->ComputeAlphaCoverage(transform) < 1.0;
}

}  // namespace impeller
