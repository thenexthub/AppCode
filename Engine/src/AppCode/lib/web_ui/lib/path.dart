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

part of ui;

// For documentation see https://github.com/flutter/flutter/blob/main/engine/src/flutter/lib/ui/painting.dart

abstract class Path {
  factory Path() => engine.renderer.createPath();
  factory Path.from(Path source) => engine.renderer.copyPath(source);
  PathFillType get fillType;
  set fillType(PathFillType value);
  void moveTo(double x, double y);
  void relativeMoveTo(double dx, double dy);
  void lineTo(double x, double y);
  void relativeLineTo(double dx, double dy);
  void quadraticBezierTo(double x1, double y1, double x2, double y2);
  void relativeQuadraticBezierTo(double x1, double y1, double x2, double y2);
  void cubicTo(double x1, double y1, double x2, double y2, double x3, double y3);
  void relativeCubicTo(double x1, double y1, double x2, double y2, double x3, double y3);
  void conicTo(double x1, double y1, double x2, double y2, double w);
  void relativeConicTo(double x1, double y1, double x2, double y2, double w);
  void arcTo(Rect rect, double startAngle, double sweepAngle, bool forceMoveTo);
  void arcToPoint(
    Offset arcEnd, {
    Radius radius = Radius.zero,
    double rotation = 0.0,
    bool largeArc = false,
    bool clockwise = true,
  });
  void relativeArcToPoint(
    Offset arcEndDelta, {
    Radius radius = Radius.zero,
    double rotation = 0.0,
    bool largeArc = false,
    bool clockwise = true,
  });
  void addRect(Rect rect);
  void addOval(Rect oval);
  void addArc(Rect oval, double startAngle, double sweepAngle);
  void addPolygon(List<Offset> points, bool close);
  void addRRect(RRect rrect);
  void addRSuperellipse(RSuperellipse rsuperellipse);
  void addPath(Path path, Offset offset, {Float64List? matrix4});
  void extendWithPath(Path path, Offset offset, {Float64List? matrix4});
  void close();
  void reset();
  bool contains(Offset point);
  Path shift(Offset offset);
  Path transform(Float64List matrix4);
  // see https://skia.org/user/api/SkPath_Reference#SkPath_getBounds
  Rect getBounds();
  static Path combine(PathOperation operation, Path path1, Path path2) =>
      engine.renderer.combinePaths(operation, path1, path2);

  PathMetrics computeMetrics({bool forceClosed = false});
}
