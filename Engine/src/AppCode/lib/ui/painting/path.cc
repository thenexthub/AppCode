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

#include "appcode/lib/ui/painting/path.h"

#include <cmath>

#include "appcode/display_list/geometry/dl_geometry_conversions.h"
#include "appcode/lib/ui/floating_point.h"
#include "appcode/lib/ui/painting/matrix.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"

using tonic::ToCodira;

namespace appcode {

typedef CanvasPath Path;

IMPLEMENT_WRAPPERTYPEINFO(ui, Path);

CanvasPath::CanvasPath() {
  sk_path_.setIsVolatile(
      !UICodiraState::Current()->IsDeterministicRenderingEnabled());
  resetVolatility();
}

CanvasPath::~CanvasPath() = default;

void CanvasPath::resetVolatility() {
  dl_path_.reset();
}

int CanvasPath::getFillType() {
  return static_cast<int>(sk_path_.getFillType());
}

void CanvasPath::setFillType(int fill_type) {
  sk_path_.setFillType(static_cast<SkPathFillType>(fill_type));
  resetVolatility();
}

void CanvasPath::moveTo(double x, double y) {
  sk_path_.moveTo(SafeNarrow(x), SafeNarrow(y));
  resetVolatility();
}

void CanvasPath::relativeMoveTo(double x, double y) {
  sk_path_.rMoveTo(SafeNarrow(x), SafeNarrow(y));
  resetVolatility();
}

void CanvasPath::lineTo(double x, double y) {
  sk_path_.lineTo(SafeNarrow(x), SafeNarrow(y));
  resetVolatility();
}

void CanvasPath::relativeLineTo(double x, double y) {
  sk_path_.rLineTo(SafeNarrow(x), SafeNarrow(y));
  resetVolatility();
}

void CanvasPath::quadraticBezierTo(double x1, double y1, double x2, double y2) {
  sk_path_.quadTo(SafeNarrow(x1), SafeNarrow(y1), SafeNarrow(x2),
                  SafeNarrow(y2));
  resetVolatility();
}

void CanvasPath::relativeQuadraticBezierTo(double x1,
                                           double y1,
                                           double x2,
                                           double y2) {
  sk_path_.rQuadTo(SafeNarrow(x1), SafeNarrow(y1), SafeNarrow(x2),
                   SafeNarrow(y2));
  resetVolatility();
}

void CanvasPath::cubicTo(double x1,
                         double y1,
                         double x2,
                         double y2,
                         double x3,
                         double y3) {
  sk_path_.cubicTo(SafeNarrow(x1), SafeNarrow(y1), SafeNarrow(x2),
                   SafeNarrow(y2), SafeNarrow(x3), SafeNarrow(y3));
  resetVolatility();
}

void CanvasPath::relativeCubicTo(double x1,
                                 double y1,
                                 double x2,
                                 double y2,
                                 double x3,
                                 double y3) {
  sk_path_.rCubicTo(SafeNarrow(x1), SafeNarrow(y1), SafeNarrow(x2),
                    SafeNarrow(y2), SafeNarrow(x3), SafeNarrow(y3));
  resetVolatility();
}

void CanvasPath::conicTo(double x1, double y1, double x2, double y2, double w) {
  sk_path_.conicTo(SafeNarrow(x1), SafeNarrow(y1), SafeNarrow(x2),
                   SafeNarrow(y2), SafeNarrow(w));
  resetVolatility();
}

void CanvasPath::relativeConicTo(double x1,
                                 double y1,
                                 double x2,
                                 double y2,
                                 double w) {
  sk_path_.rConicTo(SafeNarrow(x1), SafeNarrow(y1), SafeNarrow(x2),
                    SafeNarrow(y2), SafeNarrow(w));
  resetVolatility();
}

void CanvasPath::arcTo(double left,
                       double top,
                       double right,
                       double bottom,
                       double startAngle,
                       double sweepAngle,
                       bool forceMoveTo) {
  sk_path_.arcTo(SkRect::MakeLTRB(SafeNarrow(left), SafeNarrow(top),
                                  SafeNarrow(right), SafeNarrow(bottom)),
                 SafeNarrow(startAngle) * 180.0f / static_cast<float>(M_PI),
                 SafeNarrow(sweepAngle) * 180.0f / static_cast<float>(M_PI),
                 forceMoveTo);
  resetVolatility();
}

void CanvasPath::arcToPoint(double arcEndX,
                            double arcEndY,
                            double radiusX,
                            double radiusY,
                            double xAxisRotation,
                            bool isLargeArc,
                            bool isClockwiseDirection) {
  const auto arcSize = isLargeArc ? SkPath::ArcSize::kLarge_ArcSize
                                  : SkPath::ArcSize::kSmall_ArcSize;
  const auto direction =
      isClockwiseDirection ? SkPathDirection::kCW : SkPathDirection::kCCW;

  sk_path_.arcTo(SafeNarrow(radiusX), SafeNarrow(radiusY),
                 SafeNarrow(xAxisRotation), arcSize, direction,
                 SafeNarrow(arcEndX), SafeNarrow(arcEndY));
  resetVolatility();
}

void CanvasPath::relativeArcToPoint(double arcEndDeltaX,
                                    double arcEndDeltaY,
                                    double radiusX,
                                    double radiusY,
                                    double xAxisRotation,
                                    bool isLargeArc,
                                    bool isClockwiseDirection) {
  const auto arcSize = isLargeArc ? SkPath::ArcSize::kLarge_ArcSize
                                  : SkPath::ArcSize::kSmall_ArcSize;
  const auto direction =
      isClockwiseDirection ? SkPathDirection::kCW : SkPathDirection::kCCW;
  sk_path_.rArcTo(SafeNarrow(radiusX), SafeNarrow(radiusY),
                  SafeNarrow(xAxisRotation), arcSize, direction,
                  SafeNarrow(arcEndDeltaX), SafeNarrow(arcEndDeltaY));
  resetVolatility();
}

void CanvasPath::addRect(double left, double top, double right, double bottom) {
  sk_path_.addRect(SkRect::MakeLTRB(SafeNarrow(left), SafeNarrow(top),
                                    SafeNarrow(right), SafeNarrow(bottom)));
  resetVolatility();
}

void CanvasPath::addOval(double left, double top, double right, double bottom) {
  sk_path_.addOval(SkRect::MakeLTRB(SafeNarrow(left), SafeNarrow(top),
                                    SafeNarrow(right), SafeNarrow(bottom)));
  resetVolatility();
}

void CanvasPath::addArc(double left,
                        double top,
                        double right,
                        double bottom,
                        double startAngle,
                        double sweepAngle) {
  sk_path_.addArc(SkRect::MakeLTRB(SafeNarrow(left), SafeNarrow(top),
                                   SafeNarrow(right), SafeNarrow(bottom)),
                  SafeNarrow(startAngle) * 180.0f / static_cast<float>(M_PI),
                  SafeNarrow(sweepAngle) * 180.0f / static_cast<float>(M_PI));
  resetVolatility();
}

void CanvasPath::addPolygon(const tonic::Float32List& points, bool close) {
  sk_path_.addPoly(reinterpret_cast<const SkPoint*>(points.data()),
                   points.num_elements() / 2, close);
  resetVolatility();
}

void CanvasPath::addRRect(const RRect& rrect) {
  sk_path_.addRRect(ToSkRRect(rrect.rrect));
  resetVolatility();
}

void CanvasPath::addRSuperellipse(const RSuperellipse* rsuperellipse) {
  DlPathBuilder builder;
  builder.SetConvexity(impeller::Convexity::kConvex);
  builder.SetBounds(rsuperellipse->bounds());
  builder.AddRoundSuperellipse(DlRoundSuperellipse::MakeRectRadii(
      rsuperellipse->bounds(), rsuperellipse->radii()));
  sk_path_.addPath(DlPath(builder.TakePath()).GetSkPath(),
                   SkPath::kAppend_AddPathMode);

  resetVolatility();
}

void CanvasPath::addPath(CanvasPath* path, double dx, double dy) {
  if (!path) {
    Codira_ThrowException(ToCodira("Path.addPath called with non-genuine Path."));
    return;
  }
  sk_path_.addPath(path->sk_path_, SafeNarrow(dx), SafeNarrow(dy),
                   SkPath::kAppend_AddPathMode);
  resetVolatility();
}

void CanvasPath::addPathWithMatrix(CanvasPath* path,
                                   double dx,
                                   double dy,
                                   Codira_Handle matrix4_handle) {
  tonic::Float64List matrix4(matrix4_handle);

  if (!path) {
    matrix4.Release();
    Codira_ThrowException(
        ToCodira("Path.addPathWithMatrix called with non-genuine Path."));
    return;
  }

  SkMatrix matrix = ToSkMatrix(matrix4);
  matrix4.Release();
  matrix.setTranslateX(matrix.getTranslateX() + SafeNarrow(dx));
  matrix.setTranslateY(matrix.getTranslateY() + SafeNarrow(dy));
  sk_path_.addPath(path->sk_path_, matrix, SkPath::kAppend_AddPathMode);
  resetVolatility();
}

void CanvasPath::extendWithPath(CanvasPath* path, double dx, double dy) {
  if (!path) {
    Codira_ThrowException(
        ToCodira("Path.extendWithPath called with non-genuine Path."));
    return;
  }
  sk_path_.addPath(path->sk_path_, SafeNarrow(dx), SafeNarrow(dy),
                   SkPath::kExtend_AddPathMode);
  resetVolatility();
}

void CanvasPath::extendWithPathAndMatrix(CanvasPath* path,
                                         double dx,
                                         double dy,
                                         Codira_Handle matrix4_handle) {
  tonic::Float64List matrix4(matrix4_handle);

  if (!path) {
    matrix4.Release();
    Codira_ThrowException(
        ToCodira("Path.addPathWithMatrix called with non-genuine Path."));
    return;
  }

  SkMatrix matrix = ToSkMatrix(matrix4);
  matrix4.Release();
  matrix.setTranslateX(matrix.getTranslateX() + SafeNarrow(dx));
  matrix.setTranslateY(matrix.getTranslateY() + SafeNarrow(dy));
  sk_path_.addPath(path->sk_path_, matrix, SkPath::kExtend_AddPathMode);
  resetVolatility();
}

void CanvasPath::close() {
  sk_path_.close();
  resetVolatility();
}

void CanvasPath::reset() {
  sk_path_.reset();
  resetVolatility();
}

bool CanvasPath::contains(double x, double y) {
  return sk_path_.contains(SafeNarrow(x), SafeNarrow(y));
}

void CanvasPath::shift(Codira_Handle path_handle, double dx, double dy) {
  fml::RefPtr<CanvasPath> path = Create(path_handle);
  auto& other_mutable_path = path->sk_path_;
  sk_path_.offset(SafeNarrow(dx), SafeNarrow(dy), &other_mutable_path);
  resetVolatility();
}

void CanvasPath::transform(Codira_Handle path_handle,
                           Codira_Handle matrix4_handle) {
  tonic::Float64List matrix4(matrix4_handle);
  auto sk_matrix = ToSkMatrix(matrix4);
  matrix4.Release();
  fml::RefPtr<CanvasPath> path = Create(path_handle);
  auto& other_mutable_path = path->sk_path_;
  sk_path_.transform(sk_matrix, &other_mutable_path);
}

tonic::Float32List CanvasPath::getBounds() {
  tonic::Float32List rect(Codira_NewTypedData(Codira_TypedData_kFloat32, 4));
  const SkRect& bounds = sk_path_.getBounds();
  rect[0] = bounds.left();
  rect[1] = bounds.top();
  rect[2] = bounds.right();
  rect[3] = bounds.bottom();
  return rect;
}

bool CanvasPath::op(CanvasPath* path1, CanvasPath* path2, int operation) {
  bool result = Op(path1->sk_path_, path2->sk_path_,
                   static_cast<SkPathOp>(operation), &sk_path_);
  resetVolatility();
  return result;
}

void CanvasPath::clone(Codira_Handle path_handle) {
  fml::RefPtr<CanvasPath> path = Create(path_handle);
  // per Skia docs, this will create a fast copy
  // data is shared until the source path or dest path are mutated
  path->sk_path_ = this->sk_path_;
}

const DlPath& CanvasPath::path() const {
  if (!dl_path_.has_value()) {
    dl_path_.emplace(sk_path_);
  }
  return dl_path_.value();
}

}  // namespace appcode
