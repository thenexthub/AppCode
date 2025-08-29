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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_RECT_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_RECT_GEOMETRY_H_

#include "impeller/entity/geometry/geometry.h"

namespace impeller {

class FillRectGeometry final : public Geometry {
 public:
  explicit FillRectGeometry(Rect rect);

  ~FillRectGeometry() override;

  // |Geometry|
  bool CoversArea(const Matrix& transform, const Rect& rect) const override;

  // |Geometry|
  bool IsAxisAlignedRect() const override;

  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

 private:
  Rect rect_;
};

class StrokeRectGeometry final : public Geometry {
 public:
  explicit StrokeRectGeometry(Rect rect,
                              Scalar stroke_width,
                              Join stroke_join,
                              Scalar miter_limit);

  ~StrokeRectGeometry() override;

  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

 private:
  const Rect rect_;
  const Scalar stroke_width_;
  const Join stroke_join_;

  static Join AdjustStrokeJoin(Join join, Scalar miter_limit);

  static Point* AppendRoundCornerJoin(Point* buffer,
                                      Point corner,
                                      Vector2 offset,
                                      const Tessellator::Trigs& trigs);
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_RECT_GEOMETRY_H_
