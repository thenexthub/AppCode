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

#include "impeller/toolkit/interop/paint.h"

namespace impeller::interop {

Paint::Paint() = default;

Paint::~Paint() = default;

const appcode::DlPaint& Paint::GetPaint() const {
  return paint_;
}

void Paint::SetColor(appcode::DlColor color) {
  paint_.setColor(color);
}

void Paint::SetBlendMode(BlendMode mode) {
  paint_.setBlendMode(ToDisplayListType(mode));
}

void Paint::SetDrawStyle(appcode::DlDrawStyle style) {
  paint_.setDrawStyle(style);
}

void Paint::SetStrokeCap(appcode::DlStrokeCap stroke_cap) {
  paint_.setStrokeCap(stroke_cap);
}

void Paint::SetStrokeJoin(appcode::DlStrokeJoin stroke_join) {
  paint_.setStrokeJoin(stroke_join);
}

void Paint::SetStrokeWidth(Scalar width) {
  paint_.setStrokeWidth(width);
}

void Paint::SetStrokeMiter(Scalar miter) {
  paint_.setStrokeMiter(miter);
}

void Paint::SetColorFilter(const ColorFilter& filter) {
  paint_.setColorFilter(filter.GetColorFilter());
}

void Paint::SetColorSource(const ColorSource& source) {
  paint_.setColorSource(source.GetColorSource());
}

void Paint::SetImageFilter(const ImageFilter& filter) {
  paint_.setImageFilter(filter.GetImageFilter());
}

void Paint::SetMaskFilter(const MaskFilter& filter) {
  paint_.setMaskFilter(filter.GetMaskFilter());
}

}  // namespace impeller::interop
