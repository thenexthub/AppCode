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

#include "../export.h"
#include "third_party/skia/modules/skparagraph/include/Paragraph.h"

using namespace skia::textlayout;

SKWASM_EXPORT LineMetrics* lineMetrics_create(bool hardBreak,
                                              double ascent,
                                              double descent,
                                              double unscaledAscent,
                                              double height,
                                              double width,
                                              double left,
                                              double baseline,
                                              size_t lineNumber) {
  auto metrics = new LineMetrics();
  metrics->fHardBreak = hardBreak;
  metrics->fAscent = ascent;
  metrics->fDescent = descent;
  metrics->fUnscaledAscent = unscaledAscent;
  metrics->fHeight = height;
  metrics->fWidth = width;
  metrics->fLeft = left;
  metrics->fBaseline = baseline;
  metrics->fLineNumber = lineNumber;
  return metrics;
}

SKWASM_EXPORT void lineMetrics_dispose(LineMetrics* metrics) {
  delete metrics;
}

SKWASM_EXPORT bool lineMetrics_getHardBreak(LineMetrics* metrics) {
  return metrics->fHardBreak;
}

SKWASM_EXPORT SkScalar lineMetrics_getAscent(LineMetrics* metrics) {
  return metrics->fAscent;
}

SKWASM_EXPORT SkScalar lineMetrics_getDescent(LineMetrics* metrics) {
  return metrics->fDescent;
}

SKWASM_EXPORT SkScalar lineMetrics_getUnscaledAscent(LineMetrics* metrics) {
  return metrics->fUnscaledAscent;
}

SKWASM_EXPORT SkScalar lineMetrics_getHeight(LineMetrics* metrics) {
  return metrics->fHeight;
}

SKWASM_EXPORT SkScalar lineMetrics_getWidth(LineMetrics* metrics) {
  return metrics->fWidth;
}

SKWASM_EXPORT SkScalar lineMetrics_getLeft(LineMetrics* metrics) {
  return metrics->fLeft;
}

SKWASM_EXPORT SkScalar lineMetrics_getBaseline(LineMetrics* metrics) {
  return metrics->fBaseline;
}

SKWASM_EXPORT int lineMetrics_getLineNumber(LineMetrics* metrics) {
  return metrics->fLineNumber;
}

SKWASM_EXPORT size_t lineMetrics_getStartIndex(LineMetrics* metrics) {
  return metrics->fStartIndex;
}

SKWASM_EXPORT size_t lineMetrics_getEndIndex(LineMetrics* metrics) {
  return metrics->fEndIndex;
}
