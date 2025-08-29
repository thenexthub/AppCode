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

#ifndef APPCODE_IMPELLER_ENTITY_CONTENTS_LINE_CONTENTS_H_
#define APPCODE_IMPELLER_ENTITY_CONTENTS_LINE_CONTENTS_H_

#include <memory>

#include "appcode/impeller/entity/contents/contents.h"
#include "appcode/impeller/entity/geometry/line_geometry.h"
#include "impeller/entity/line.vert.h"

namespace impeller {
class LineContents : public Contents {
 public:
  static const Scalar kSampleRadius;
  static std::vector<uint8_t> CreateCurveData(Scalar width,
                                              Scalar radius,
                                              Scalar scale);

  struct EffectiveLineParameters {
    Scalar width;
    Scalar radius;
  };

  /// Calculates the values needed for the vertex shader, per vertex.
  /// Returns the effective line parameters that are used. These differ from the
  /// ones provided by `geometry` when the line gets clamped for being too thin.
  static fml::StatusOr<EffectiveLineParameters> CalculatePerVertex(
      LineVertexShader::PerVertexData* per_vertex,
      const LineGeometry* geometry,
      const Matrix& entity_transform);

  static std::unique_ptr<LineContents> Make(
      std::unique_ptr<LineGeometry> geometry,
      Color color);

  bool Render(const ContentContext& renderer,
              const Entity& entity,
              RenderPass& pass) const override;

  std::optional<Rect> GetCoverage(const Entity& entity) const override;

 private:
  explicit LineContents(std::unique_ptr<LineGeometry> geometry, Color color);

  std::unique_ptr<LineGeometry> geometry_;
  Color color_;
};
}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_CONTENTS_LINE_CONTENTS_H_
