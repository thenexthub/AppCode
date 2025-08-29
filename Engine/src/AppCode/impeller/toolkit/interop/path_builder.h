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

#ifndef APPCODE_IMPELLER_TOOLKIT_INTEROP_PATH_BUILDER_H_
#define APPCODE_IMPELLER_TOOLKIT_INTEROP_PATH_BUILDER_H_

#include "appcode/third_party/skia/include/core/SkPath.h"
#include "impeller/geometry/path.h"
#include "impeller/geometry/path_builder.h"
#include "impeller/toolkit/interop/impeller.h"
#include "impeller/toolkit/interop/object.h"
#include "impeller/toolkit/interop/path.h"

namespace impeller::interop {

class PathBuilder final
    : public Object<PathBuilder,
                    IMPELLER_INTERNAL_HANDLE_NAME(ImpellerPathBuilder)> {
 public:
  PathBuilder();

  ~PathBuilder();

  PathBuilder(const PathBuilder&) = delete;

  PathBuilder& operator=(const PathBuilder&) = delete;

  void MoveTo(const Point& point);

  void LineTo(const Point& location);

  void QuadraticCurveTo(const Point& control_point, const Point& end_point);

  void CubicCurveTo(const Point& control_point_1,
                    const Point& control_point_2,
                    const Point& end_point);

  void AddRect(const Rect& rect);

  void AddArc(const Rect& oval_bounds, Degrees start_angle, Degrees end_angle);

  void AddOval(const Rect& oval_bounds);

  void AddRoundedRect(const Rect& rect, const RoundingRadii& radii);

  void Close();

  ScopedObject<Path> TakePath(FillType fill);

  ScopedObject<Path> CopyPath(FillType fill);

 private:
  SkPath builder_;
};

}  // namespace impeller::interop

#endif  // APPCODE_IMPELLER_TOOLKIT_INTEROP_PATH_BUILDER_H_
