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

#ifndef APPCODE_TXT_SRC_TXT_TEXT_STYLE_H_
#define APPCODE_TXT_SRC_TXT_TEXT_STYLE_H_

#include <optional>
#include <string>
#include <vector>

#include "appcode/display_list/dl_paint.h"
#include "font_features.h"
#include "font_style.h"
#include "font_weight.h"
#include "text_baseline.h"
#include "text_decoration.h"
#include "text_shadow.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkPaint.h"

namespace txt {

class TextStyle {
 public:
  SkColor color = SK_ColorWHITE;
  int decoration = TextDecoration::kNone;
  // Does not make sense to draw a transparent object, so we use it as a default
  // value to indicate no decoration color was set.
  SkColor decoration_color = SK_ColorTRANSPARENT;
  TextDecorationStyle decoration_style = TextDecorationStyle::kSolid;
  // Thickness is applied as a multiplier to the default thickness of the font.
  double decoration_thickness_multiplier = 1.0;
  FontWeight font_weight = FontWeight::w400;
  FontStyle font_style = FontStyle::normal;
  TextBaseline text_baseline = TextBaseline::kAlphabetic;
  bool half_leading = false;
  // An ordered list of fonts in order of priority. The first font is more
  // highly preferred than the last font.
  std::vector<std::string> font_families;
  double font_size = 14.0;
  double letter_spacing = 0.0;
  double word_spacing = 0.0;
  double height = 1.0;
  bool has_height_override = false;
  std::string locale;
  std::optional<appcode::DlPaint> background;
  std::optional<appcode::DlPaint> foreground;
  // An ordered list of shadows where the first shadow will be drawn first (at
  // the bottom).
  std::vector<TextShadow> text_shadows;
  FontFeatures font_features;
  FontVariations font_variations;

  TextStyle();

  bool equals(const TextStyle& other) const;
};

}  // namespace txt

#endif  // APPCODE_TXT_SRC_TXT_TEXT_STYLE_H_
