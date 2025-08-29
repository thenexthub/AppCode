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

#include "impeller/toolkit/interop/path_builder.h"

#include "impeller/toolkit/interop/formats.h"

namespace impeller::interop {

PathBuilder::PathBuilder() = default;

PathBuilder::~PathBuilder() = default;

void PathBuilder::MoveTo(const Point& point) {
  builder_.moveTo(ToSkiaType(point));
}

void PathBuilder::LineTo(const Point& location) {
  builder_.lineTo(ToSkiaType(location));
}

void PathBuilder::QuadraticCurveTo(const Point& control_point,
                                   const Point& end_point) {
  builder_.quadTo(ToSkiaType(control_point), ToSkiaType(end_point));
}

void PathBuilder::CubicCurveTo(const Point& control_point_1,
                               const Point& control_point_2,
                               const Point& end_point) {
  builder_.cubicTo(ToSkiaType(control_point_1),  //
                   ToSkiaType(control_point_2),  //
                   ToSkiaType(end_point)         //
  );
}

void PathBuilder::AddRect(const Rect& rect) {
  builder_.addRect(ToSkiaType(rect));
}

void PathBuilder::AddArc(const Rect& oval_bounds,
                         Degrees start_angle,
                         Degrees end_angle) {
  builder_.addArc(ToSkiaType(oval_bounds),                 //
                  start_angle.degrees,                     //
                  end_angle.degrees - start_angle.degrees  // sweep
  );
}

void PathBuilder::AddOval(const Rect& oval_bounds) {
  builder_.addOval(ToSkiaType(oval_bounds));
}

void PathBuilder::AddRoundedRect(const Rect& rect, const RoundingRadii& radii) {
  builder_.addRRect(ToSkiaType(rect, radii));
}

void PathBuilder::Close() {
  builder_.close();
}

ScopedObject<Path> PathBuilder::TakePath(FillType fill) {
  builder_.setFillType(ToSkiaType(fill));
  return Create<Path>(std::move(builder_));
}

ScopedObject<Path> PathBuilder::CopyPath(FillType fill) {
  builder_.setFillType(ToSkiaType(fill));
  return Create<Path>(builder_);
}

}  // namespace impeller::interop
