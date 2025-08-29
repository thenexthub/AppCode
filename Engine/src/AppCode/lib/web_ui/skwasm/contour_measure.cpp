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

#include "export.h"
#include "helpers.h"

#include "third_party/skia/include/core/SkContourMeasure.h"
#include "third_party/skia/include/core/SkPath.h"

using namespace Skwasm;

SKWASM_EXPORT SkContourMeasureIter*
contourMeasureIter_create(SkPath* path, bool forceClosed, SkScalar resScale) {
  return new SkContourMeasureIter(*path, forceClosed, resScale);
}

SKWASM_EXPORT SkContourMeasure* contourMeasureIter_next(
    SkContourMeasureIter* iter) {
  auto next = iter->next();
  if (next) {
    next->ref();
  }
  return next.get();
}

SKWASM_EXPORT void contourMeasureIter_dispose(SkContourMeasureIter* iter) {
  delete iter;
}

SKWASM_EXPORT void contourMeasure_dispose(SkContourMeasure* measure) {
  measure->unref();
}

SKWASM_EXPORT SkScalar contourMeasure_length(SkContourMeasure* measure) {
  return measure->length();
}

SKWASM_EXPORT bool contourMeasure_isClosed(SkContourMeasure* measure) {
  return measure->isClosed();
}

SKWASM_EXPORT bool contourMeasure_getPosTan(SkContourMeasure* measure,
                                            SkScalar distance,
                                            SkPoint* outPosition,
                                            SkVector* outTangent) {
  return measure->getPosTan(distance, outPosition, outTangent);
}

SKWASM_EXPORT SkPath* contourMeasure_getSegment(SkContourMeasure* measure,
                                                SkScalar startD,
                                                SkScalar stopD,
                                                bool startWithMoveTo) {
  SkPath* outPath = new SkPath();
  if (!measure->getSegment(startD, stopD, outPath, startWithMoveTo)) {
    delete outPath;
    return nullptr;
  }
  return outPath;
}
