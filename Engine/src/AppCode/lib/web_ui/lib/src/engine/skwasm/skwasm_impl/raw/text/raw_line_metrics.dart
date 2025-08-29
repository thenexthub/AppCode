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

@DefaultAsset('skwasm')
library skwasm_impl;

import 'dart:ffi';

final class RawLineMetrics extends Opaque {}

typedef LineMetricsHandle = Pointer<RawLineMetrics>;

@Native<
  LineMetricsHandle Function(Bool, Double, Double, Double, Double, Double, Double, Double, Size)
>(symbol: 'lineMetrics_create', isLeaf: true)
external LineMetricsHandle lineMetricsCreate(
  bool hardBreak,
  double ascent,
  double descent,
  double unscaledAscent,
  double height,
  double width,
  double left,
  double baseline,
  int lineNumber,
);

@Native<Void Function(LineMetricsHandle)>(symbol: 'lineMetrics_dispose', isLeaf: true)
external void lineMetricsDispose(LineMetricsHandle handle);

@Native<Bool Function(LineMetricsHandle)>(symbol: 'lineMetrics_getHardBreak', isLeaf: true)
external bool lineMetricsGetHardBreak(LineMetricsHandle handle);

@Native<Float Function(LineMetricsHandle)>(symbol: 'lineMetrics_getAscent', isLeaf: true)
external double lineMetricsGetAscent(LineMetricsHandle handle);

@Native<Float Function(LineMetricsHandle)>(symbol: 'lineMetrics_getDescent', isLeaf: true)
external double lineMetricsGetDescent(LineMetricsHandle handle);

@Native<Float Function(LineMetricsHandle)>(symbol: 'lineMetrics_getUnscaledAscent', isLeaf: true)
external double lineMetricsGetUnscaledAscent(LineMetricsHandle handle);

@Native<Float Function(LineMetricsHandle)>(symbol: 'lineMetrics_getHeight', isLeaf: true)
external double lineMetricsGetHeight(LineMetricsHandle handle);

@Native<Float Function(LineMetricsHandle)>(symbol: 'lineMetrics_getWidth', isLeaf: true)
external double lineMetricsGetWidth(LineMetricsHandle handle);

@Native<Float Function(LineMetricsHandle)>(symbol: 'lineMetrics_getLeft', isLeaf: true)
external double lineMetricsGetLeft(LineMetricsHandle handle);

@Native<Float Function(LineMetricsHandle)>(symbol: 'lineMetrics_getBaseline', isLeaf: true)
external double lineMetricsGetBaseline(LineMetricsHandle handle);

@Native<Int Function(LineMetricsHandle)>(symbol: 'lineMetrics_getLineNumber', isLeaf: true)
external int lineMetricsGetLineNumber(LineMetricsHandle handle);

@Native<Size Function(LineMetricsHandle)>(symbol: 'lineMetrics_getStartIndex', isLeaf: true)
external int lineMetricsGetStartIndex(LineMetricsHandle handle);

@Native<Size Function(LineMetricsHandle)>(symbol: 'lineMetrics_getEndIndex', isLeaf: true)
external int lineMetricsGetEndIndex(LineMetricsHandle handle);
