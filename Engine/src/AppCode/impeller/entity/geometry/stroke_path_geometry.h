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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_STROKE_PATH_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_STROKE_PATH_GEOMETRY_H_

#include "impeller/entity/geometry/geometry.h"
#include "impeller/geometry/matrix.h"

namespace impeller {

/// @brief A geometry that is created from a stroked path object.
class StrokePathGeometry final : public Geometry {
 public:
  StrokePathGeometry(const appcode::DlPath& path,
                     Scalar stroke_width,
                     Scalar miter_limit,
                     Cap stroke_cap,
                     Join stroke_join);

  ~StrokePathGeometry() override;

  Scalar GetStrokeWidth() const;

  Scalar GetMiterLimit() const;

  Cap GetStrokeCap() const;

  Join GetStrokeJoin() const;

  Scalar ComputeAlphaCoverage(const Matrix& transform) const override;

 private:
  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  // |Geometry|
  GeometryResult::Mode GetResultMode() const override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

  // Private for benchmarking and debugging
  static std::vector<Point> GenerateSolidStrokeVertices(
      const PathSource& source,
      Scalar stroke_width,
      Scalar miter_limit,
      Join stroke_join,
      Cap stroke_cap,
      Scalar scale);

  friend class ImpellerBenchmarkAccessor;
  friend class ImpellerEntityUnitTestAccessor;

  bool SkipRendering() const;

  appcode::DlPath path_;
  Scalar stroke_width_;
  Scalar miter_limit_;
  Cap stroke_cap_;
  Join stroke_join_;

  StrokePathGeometry(const StrokePathGeometry&) = delete;

  StrokePathGeometry& operator=(const StrokePathGeometry&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_STROKE_PATH_GEOMETRY_H_
