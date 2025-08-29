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

abstract class PathMetrics extends collection.IterableBase<PathMetric> {
  @override
  Iterator<PathMetric> get iterator;
}

abstract class PathMetricIterator implements Iterator<PathMetric> {
  @override
  PathMetric get current;

  @override
  bool moveNext();
}

abstract class PathMetric {
  double get length;
  int get contourIndex;
  Tangent? getTangentForOffset(double distance);
  Path extractPath(double start, double end, {bool startWithMoveTo = true});
  bool get isClosed;
}

class Tangent {
  const Tangent(this.position, this.vector);
  factory Tangent.fromAngle(Offset position, double angle) {
    return Tangent(position, Offset(math.cos(angle), math.sin(angle)));
  }
  final Offset position;
  final Offset vector;
  // flip the sign to be consistent with [Path.arcTo]'s `sweepAngle`
  double get angle => -math.atan2(vector.dy, vector.dx);
}
