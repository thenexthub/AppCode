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

#include "appcode/impeller/entity/geometry/round_rect_geometry.h"

namespace impeller {

RoundRectGeometry::RoundRectGeometry(const Rect& bounds, const Size& radii)
    : bounds_(bounds), radii_(radii) {}

RoundRectGeometry::~RoundRectGeometry() = default;

GeometryResult RoundRectGeometry::GetPositionBuffer(
    const ContentContext& renderer,
    const Entity& entity,
    RenderPass& pass) const {
  return ComputePositionGeometry(renderer,
                                 renderer.GetTessellator().FilledRoundRect(
                                     entity.GetTransform(), bounds_, radii_),
                                 entity, pass);
}

std::optional<Rect> RoundRectGeometry::GetCoverage(
    const Matrix& transform) const {
  return bounds_.TransformBounds(transform);
}

bool RoundRectGeometry::CoversArea(const Matrix& transform,
                                   const Rect& rect) const {
  if (!transform.IsTranslationScaleOnly()) {
    return false;
  }
  bool flat_on_tb = bounds_.GetWidth() > radii_.width * 2;
  bool flat_on_lr = bounds_.GetHeight() > radii_.height * 2;
  if (!flat_on_tb && !flat_on_lr) {
    return false;
  }
  // We either transform the bounds and delta-transform the radii,
  // or we compute the vertical and horizontal bounds and then
  // transform each. Either way there are 2 transform operations.
  // We could also get a weaker answer by computing just the
  // "inner rect" and only doing a coverage analysis on that,
  // but this process will produce more culling results.
  if (flat_on_tb) {
    Rect vertical_bounds = bounds_.Expand(Size{-radii_.width, 0});
    Rect coverage = vertical_bounds.TransformBounds(transform);
    if (coverage.Contains(rect)) {
      return true;
    }
  }
  if (flat_on_lr) {
    Rect horizontal_bounds = bounds_.Expand(Size{0, -radii_.height});
    Rect coverage = horizontal_bounds.TransformBounds(transform);
    if (coverage.Contains(rect)) {
      return true;
    }
  }
  return false;
}

bool RoundRectGeometry::IsAxisAlignedRect() const {
  return false;
}

}  // namespace impeller
