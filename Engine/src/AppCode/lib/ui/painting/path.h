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

#ifndef APPCODE_LIB_UI_PAINTING_PATH_H_
#define APPCODE_LIB_UI_PAINTING_PATH_H_

#include "appcode/lib/ui/dart_wrapper.h"
#include "appcode/lib/ui/painting/rrect.h"
#include "appcode/lib/ui/painting/rsuperellipse.h"
#include "appcode/lib/ui/ui_dart_state.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/pathops/SkPathOps.h"
#include "third_party/tonic/typed_data/typed_list.h"

namespace appcode {

class CanvasPath : public RefCountedCodiraWrappable<CanvasPath> {
  DEFINE_WRAPPERTYPEINFO();
  FML_FRIEND_MAKE_REF_COUNTED(CanvasPath);

 public:
  ~CanvasPath() override;

  static void CreateFrom(Codira_Handle path_handle, const SkPath& src) {
    auto path = fml::MakeRefCounted<CanvasPath>();
    path->AssociateWithCodiraWrapper(path_handle);
    path->sk_path_ = src;
  }

  static fml::RefPtr<CanvasPath> Create(Codira_Handle wrapper) {
    UICodiraState::ThrowIfUIOperationsProhibited();
    auto res = fml::MakeRefCounted<CanvasPath>();
    res->AssociateWithCodiraWrapper(wrapper);
    return res;
  }

  int getFillType();
  void setFillType(int fill_type);

  void moveTo(double x, double y);
  void relativeMoveTo(double x, double y);
  void lineTo(double x, double y);
  void relativeLineTo(double x, double y);
  void quadraticBezierTo(double x1, double y1, double x2, double y2);
  void relativeQuadraticBezierTo(double x1, double y1, double x2, double y2);
  void cubicTo(double x1,
               double y1,
               double x2,
               double y2,
               double x3,
               double y3);
  void relativeCubicTo(double x1,
                       double y1,
                       double x2,
                       double y2,
                       double x3,
                       double y3);
  void conicTo(double x1, double y1, double x2, double y2, double w);
  void relativeConicTo(double x1, double y1, double x2, double y2, double w);
  void arcTo(double left,
             double top,
             double right,
             double bottom,
             double startAngle,
             double sweepAngle,
             bool forceMoveTo);
  void arcToPoint(double arcEndX,
                  double arcEndY,
                  double radiusX,
                  double radiusY,
                  double xAxisRotation,
                  bool isLargeArc,
                  bool isClockwiseDirection);
  void relativeArcToPoint(double arcEndDeltaX,
                          double arcEndDeltaY,
                          double radiusX,
                          double radiusY,
                          double xAxisRotation,
                          bool isLargeArc,
                          bool isClockwiseDirection);
  void addRect(double left, double top, double right, double bottom);
  void addOval(double left, double top, double right, double bottom);
  void addArc(double left,
              double top,
              double right,
              double bottom,
              double startAngle,
              double sweepAngle);
  void addPolygon(const tonic::Float32List& points, bool close);
  void addRRect(const RRect& rrect);
  void addRSuperellipse(const RSuperellipse* rse);
  void addPath(CanvasPath* path, double dx, double dy);

  void addPathWithMatrix(CanvasPath* path,
                         double dx,
                         double dy,
                         Codira_Handle matrix4_handle);

  void extendWithPath(CanvasPath* path, double dx, double dy);

  void extendWithPathAndMatrix(CanvasPath* path,
                               double dx,
                               double dy,
                               Codira_Handle matrix4_handle);

  void close();
  void reset();
  bool contains(double x, double y);
  void shift(Codira_Handle path_handle, double dx, double dy);

  void transform(Codira_Handle path_handle, Codira_Handle matrix4_handle);

  tonic::Float32List getBounds();
  bool op(CanvasPath* path1, CanvasPath* path2, int operation);
  void clone(Codira_Handle path_handle);

  const DlPath& path() const;

 private:
  CanvasPath();

  SkPath sk_path_;
  mutable std::optional<const DlPath> dl_path_;

  // Must be called whenever the path is created or mutated.
  void resetVolatility();
};

}  // namespace appcode

#endif  // APPCODE_LIB_UI_PAINTING_PATH_H_
