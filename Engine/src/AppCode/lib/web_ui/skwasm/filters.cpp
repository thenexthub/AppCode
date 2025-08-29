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

#include "third_party/skia/include/core/SkColorFilter.h"
#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/effects/SkImageFilters.h"

using namespace Skwasm;

SKWASM_EXPORT SkImageFilter* imageFilter_createBlur(SkScalar sigmaX,
                                                    SkScalar sigmaY,
                                                    SkTileMode tileMode) {
  return SkImageFilters::Blur(sigmaX, sigmaY, tileMode, nullptr).release();
}

SKWASM_EXPORT SkImageFilter* imageFilter_createDilate(SkScalar radiusX,
                                                      SkScalar radiusY) {
  return SkImageFilters::Dilate(radiusX, radiusY, nullptr).release();
}

SKWASM_EXPORT SkImageFilter* imageFilter_createErode(SkScalar radiusX,
                                                     SkScalar radiusY) {
  return SkImageFilters::Erode(radiusX, radiusY, nullptr).release();
}

SKWASM_EXPORT SkImageFilter* imageFilter_createMatrix(SkScalar* matrix33,
                                                      FilterQuality quality) {
  return SkImageFilters::MatrixTransform(createMatrix(matrix33),
                                         samplingOptionsForQuality(quality),
                                         nullptr)
      .release();
}

SKWASM_EXPORT SkImageFilter* imageFilter_createFromColorFilter(
    SkColorFilter* filter) {
  return SkImageFilters::ColorFilter(sk_ref_sp<SkColorFilter>(filter), nullptr)
      .release();
}

SKWASM_EXPORT SkImageFilter* imageFilter_compose(SkImageFilter* outer,
                                                 SkImageFilter* inner) {
  return SkImageFilters::Compose(sk_ref_sp<SkImageFilter>(outer),
                                 sk_ref_sp<SkImageFilter>(inner))
      .release();
}

SKWASM_EXPORT void imageFilter_dispose(SkImageFilter* filter) {
  filter->unref();
}

SKWASM_EXPORT void imageFilter_getFilterBounds(SkImageFilter* filter,
                                               SkIRect* inOutBounds) {
  SkIRect outputRect =
      filter->filterBounds(*inOutBounds, SkMatrix(),
                           SkImageFilter::MapDirection::kForward_MapDirection);
  *inOutBounds = outputRect;
}

SKWASM_EXPORT SkColorFilter* colorFilter_createMode(SkColor color,
                                                    SkBlendMode mode) {
  return SkColorFilters::Blend(color, mode).release();
}

SKWASM_EXPORT SkColorFilter* colorFilter_createMatrix(
    float* matrixData  // 20 values
) {
  return SkColorFilters::Matrix(matrixData).release();
}

SKWASM_EXPORT SkColorFilter* colorFilter_createSRGBToLinearGamma() {
  return SkColorFilters::SRGBToLinearGamma().release();
}

SKWASM_EXPORT SkColorFilter* colorFilter_createLinearToSRGBGamma() {
  return SkColorFilters::LinearToSRGBGamma().release();
}

SKWASM_EXPORT SkColorFilter* colorFilter_compose(SkColorFilter* outer,
                                                 SkColorFilter* inner) {
  return SkColorFilters::Compose(sk_ref_sp<SkColorFilter>(outer),
                                 sk_ref_sp<SkColorFilter>(inner))
      .release();
}

SKWASM_EXPORT void colorFilter_dispose(SkColorFilter* filter) {
  filter->unref();
}

SKWASM_EXPORT SkMaskFilter* maskFilter_createBlur(SkBlurStyle blurStyle,
                                                  SkScalar sigma) {
  return SkMaskFilter::MakeBlur(blurStyle, sigma).release();
}

SKWASM_EXPORT void maskFilter_dispose(SkMaskFilter* filter) {
  filter->unref();
}
