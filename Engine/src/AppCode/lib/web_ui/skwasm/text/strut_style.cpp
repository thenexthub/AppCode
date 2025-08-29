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

SKWASM_EXPORT StrutStyle* strutStyle_create() {
  auto style = new StrutStyle();
  style->setStrutEnabled(true);
  return style;
}

SKWASM_EXPORT void strutStyle_dispose(StrutStyle* style) {
  delete style;
}

SKWASM_EXPORT void strutStyle_setFontFamilies(StrutStyle* style,
                                              SkString** fontFamilies,
                                              int count) {
  std::vector<SkString> families;
  families.reserve(count);
  for (int i = 0; i < count; i++) {
    families.push_back(*fontFamilies[i]);
  }
  style->setFontFamilies(std::move(families));
}

SKWASM_EXPORT void strutStyle_setFontSize(StrutStyle* style,
                                          SkScalar fontSize) {
  style->setFontSize(fontSize);
}

SKWASM_EXPORT void strutStyle_setHeight(StrutStyle* style, SkScalar height) {
  style->setHeight(height);
  style->setHeightOverride(true);
}

SKWASM_EXPORT void strutStyle_setHalfLeading(StrutStyle* style,
                                             bool halfLeading) {
  style->setHalfLeading(halfLeading);
}

SKWASM_EXPORT void strutStyle_setLeading(StrutStyle* style, SkScalar leading) {
  style->setLeading(leading);
}

SKWASM_EXPORT void strutStyle_setFontStyle(StrutStyle* style,
                                           int weight,
                                           SkFontStyle::Slant slant) {
  style->setFontStyle(SkFontStyle(weight, SkFontStyle::kNormal_Width, slant));
}

SKWASM_EXPORT void strutStyle_setForceStrutHeight(StrutStyle* style,
                                                  bool forceStrutHeight) {
  style->setForceStrutHeight(forceStrutHeight);
}
