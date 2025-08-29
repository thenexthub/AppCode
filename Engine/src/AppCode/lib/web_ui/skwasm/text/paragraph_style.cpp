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
#include "../wrappers.h"
#include "third_party/skia/modules/skparagraph/include/Paragraph.h"

using namespace skia::textlayout;
using namespace Skwasm;

SKWASM_EXPORT ParagraphStyle* paragraphStyle_create() {
  auto style = new ParagraphStyle();

  // This is the default behavior in Flutter
  style->setReplaceTabCharacters(true);

  // Default text style has a black color
  TextStyle textStyle;
  textStyle.setColor(SK_ColorBLACK);
  style->setTextStyle(textStyle);

  return style;
}

SKWASM_EXPORT void paragraphStyle_dispose(ParagraphStyle* style) {
  delete style;
}

SKWASM_EXPORT void paragraphStyle_setTextAlign(ParagraphStyle* style,
                                               TextAlign align) {
  style->setTextAlign(align);
}

SKWASM_EXPORT void paragraphStyle_setTextDirection(ParagraphStyle* style,
                                                   TextDirection direction) {
  style->setTextDirection(direction);
}

SKWASM_EXPORT void paragraphStyle_setMaxLines(ParagraphStyle* style,
                                              size_t maxLines) {
  style->setMaxLines(maxLines);
}

SKWASM_EXPORT void paragraphStyle_setHeight(ParagraphStyle* style,
                                            SkScalar height) {
  style->setHeight(height);
}

SKWASM_EXPORT void paragraphStyle_setTextHeightBehavior(
    ParagraphStyle* style,
    bool applyHeightToFirstAscent,
    bool applyHeightToLastDescent) {
  TextHeightBehavior behavior;
  if (!applyHeightToFirstAscent && !applyHeightToLastDescent) {
    behavior = kDisableAll;
  } else if (!applyHeightToLastDescent) {
    behavior = kDisableLastDescent;
  } else if (!applyHeightToFirstAscent) {
    behavior = kDisableFirstAscent;
  } else {
    behavior = kAll;
  }
  style->setTextHeightBehavior(behavior);
}

SKWASM_EXPORT void paragraphStyle_setEllipsis(ParagraphStyle* style,
                                              SkString* ellipsis) {
  style->setEllipsis(*ellipsis);
}

SKWASM_EXPORT void paragraphStyle_setStrutStyle(ParagraphStyle* style,
                                                StrutStyle* strutStyle) {
  style->setStrutStyle(*strutStyle);
}

SKWASM_EXPORT void paragraphStyle_setTextStyle(ParagraphStyle* style,
                                               TextStyle* textStyle) {
  style->setTextStyle(*textStyle);
}

SKWASM_EXPORT void paragraphStyle_setApplyRoundingHack(ParagraphStyle* style,
                                                       bool applyRoundingHack) {
  style->setApplyRoundingHack(applyRoundingHack);
}
