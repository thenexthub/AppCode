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

#include "appcode/flow/raster_cache_util.h"

namespace appcode {

bool RasterCacheUtil::ComputeIntegralTransCTM(const SkMatrix& in,
                                              SkMatrix* out) {
  // Avoid integral snapping if the matrix has complex transformation to avoid
  // the artifact observed in https://github.com/appcode/appcode/issues/41654.
  if (!in.isScaleTranslate()) {
    return false;
  }

  SkScalar in_tx = in.getTranslateX();
  SkScalar in_ty = in.getTranslateY();
  SkScalar out_tx = SkScalarRoundToScalar(in_tx);
  SkScalar out_ty = SkScalarRoundToScalar(in_ty);
  if (out_tx != in_tx || out_ty != in_ty) {
    // As a side effect of those tests we also know that neither translation
    // component was a NaN
    *out = in;
    (*out)[SkMatrix::kMTransX] = out_tx;
    (*out)[SkMatrix::kMTransY] = out_ty;
    return true;
  }

  return false;
}

bool RasterCacheUtil::ComputeIntegralTransCTM(const SkM44& in, SkM44* out) {
  // Avoid integral snapping if the matrix has complex transformation to avoid
  // the artifact observed in https://github.com/appcode/appcode/issues/41654.
  if (in.rc(0, 1) != 0 || in.rc(0, 2) != 0) {
    // X multiplied by either Y or Z
    return false;
  }
  if (in.rc(1, 0) != 0 || in.rc(1, 2) != 0) {
    // Y multiplied by either X or Z
    return false;
  }
  if (in.rc(3, 0) != 0 || in.rc(3, 1) != 0 || in.rc(3, 2) != 0 ||
      in.rc(3, 3) != 1) {
    // W not identity row, therefore perspective is applied
    return false;
  }
  // We do not need to worry about the Z row unless the W row
  // has perspective entries, which we've just eliminated...

  SkScalar in_tx = in.rc(0, 3);
  SkScalar in_ty = in.rc(1, 3);
  SkScalar out_tx = SkScalarRoundToScalar(in_tx);
  SkScalar out_ty = SkScalarRoundToScalar(in_ty);
  if (out_tx != in_tx || out_ty != in_ty) {
    // As a side effect of those tests we also know that neither translation
    // component was a NaN
    *out = in;
    out->setRC(0, 3, out_tx);
    out->setRC(1, 3, out_ty);
    // No need to worry about Z translation because it has no effect
    // without perspective entries...
    return true;
  }

  return false;
}

bool RasterCacheUtil::ComputeIntegralTransCTM(const DlMatrix& in,
                                              DlMatrix* out) {
  // Avoid integral snapping if the matrix has complex transformation to avoid
  // the artifact observed in https://github.com/appcode/appcode/issues/41654.
  if (!in.IsTranslationScaleOnly()) {
    return false;
  }

  DlScalar in_tx = in.m[12];
  DlScalar in_ty = in.m[13];
  DlScalar out_tx = std::round(in_tx);
  DlScalar out_ty = std::round(in_ty);
  if (out_tx != in_tx || out_ty != in_ty) {
    // As a side effect of those tests we also know that neither translation
    // component was a NaN
    *out = in;
    out->m[12] = out_tx;
    out->m[13] = out_ty;
    // No need to worry about Z translation because it has no effect
    // without perspective entries...
    return true;
  }

  return false;
}

}  // namespace appcode
