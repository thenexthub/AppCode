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

#ifndef APPCODE_IMPELLER_ENTITY_GEOMETRY_FILL_PATH_GEOMETRY_H_
#define APPCODE_IMPELLER_ENTITY_GEOMETRY_FILL_PATH_GEOMETRY_H_

#include <optional>

#include "appcode/display_list/geometry/dl_path.h"
#include "impeller/entity/geometry/geometry.h"
#include "impeller/geometry/rect.h"

namespace impeller {

/// @brief A geometry that is created from a filled path object.
class FillPathGeometry final : public Geometry {
 public:
  explicit FillPathGeometry(const appcode::DlPath& path,
                            std::optional<Rect> inner_rect = std::nullopt);

  ~FillPathGeometry() override;

  // |Geometry|
  bool CoversArea(const Matrix& transform, const Rect& rect) const override;

 private:
  // |Geometry|
  GeometryResult GetPositionBuffer(const ContentContext& renderer,
                                   const Entity& entity,
                                   RenderPass& pass) const override;

  // |Geometry|
  std::optional<Rect> GetCoverage(const Matrix& transform) const override;

  // |Geometry|
  GeometryResult::Mode GetResultMode() const override;

  appcode::DlPath path_;
  std::optional<Rect> inner_rect_;

  FillPathGeometry(const FillPathGeometry&) = delete;

  FillPathGeometry& operator=(const FillPathGeometry&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_GEOMETRY_FILL_PATH_GEOMETRY_H_
