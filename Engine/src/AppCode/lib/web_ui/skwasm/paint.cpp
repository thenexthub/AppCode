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
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkMaskFilter.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkShader.h"

using namespace Skwasm;

SKWASM_EXPORT SkPaint* paint_create(bool isAntiAlias,
                                    SkBlendMode blendMode,
                                    SkColor color,
                                    SkPaint::Style style,
                                    SkScalar strokeWidth,
                                    SkPaint::Cap strokeCap,
                                    SkPaint::Join strokeJoin,
                                    SkScalar strokeMiterLimit) {
  auto paint = new SkPaint();
  paint->setAntiAlias(isAntiAlias);
  paint->setBlendMode(blendMode);
  paint->setStyle(style);
  paint->setStrokeWidth(strokeWidth);
  paint->setStrokeCap(strokeCap);
  paint->setStrokeJoin(strokeJoin);
  paint->setColor(color);
  paint->setStrokeMiter(strokeMiterLimit);
  return paint;
}

SKWASM_EXPORT void paint_dispose(SkPaint* paint) {
  delete paint;
}

SKWASM_EXPORT void paint_setShader(SkPaint* paint, SkShader* shader) {
  paint->setShader(sk_ref_sp<SkShader>(shader));
}

SKWASM_EXPORT void paint_setImageFilter(SkPaint* paint, SkImageFilter* filter) {
  paint->setImageFilter(sk_ref_sp<SkImageFilter>(filter));
}

SKWASM_EXPORT void paint_setColorFilter(SkPaint* paint, SkColorFilter* filter) {
  paint->setColorFilter(sk_ref_sp<SkColorFilter>(filter));
}

SKWASM_EXPORT void paint_setMaskFilter(SkPaint* paint, SkMaskFilter* filter) {
  paint->setMaskFilter(sk_ref_sp<SkMaskFilter>(filter));
}
