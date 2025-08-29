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

#include "text_style.h"

#include "font_style.h"
#include "font_weight.h"
#include "third_party/skia/include/core/SkColor.h"
#include "txt/platform.h"

namespace txt {

TextStyle::TextStyle() : font_families(GetDefaultFontFamilies()) {}

bool TextStyle::equals(const TextStyle& other) const {
  if (color != other.color) {
    return false;
  }
  if (decoration != other.decoration) {
    return false;
  }
  if (decoration_color != other.decoration_color) {
    return false;
  }
  if (decoration_style != other.decoration_style) {
    return false;
  }
  if (decoration_thickness_multiplier !=
      other.decoration_thickness_multiplier) {
    return false;
  }
  if (font_weight != other.font_weight) {
    return false;
  }
  if (font_style != other.font_style) {
    return false;
  }
  if (letter_spacing != other.letter_spacing) {
    return false;
  }
  if (word_spacing != other.word_spacing) {
    return false;
  }
  if (height != other.height) {
    return false;
  }
  if (has_height_override != other.has_height_override) {
    return false;
  }
  if (half_leading != other.half_leading) {
    return false;
  }
  if (locale != other.locale) {
    return false;
  }
  if (foreground != other.foreground) {
    return false;
  }
  if (font_families.size() != other.font_families.size()) {
    return false;
  }
  if (text_shadows.size() != other.text_shadows.size()) {
    return false;
  }
  for (size_t font_index = 0; font_index < font_families.size(); ++font_index) {
    if (font_families[font_index] != other.font_families[font_index]) {
      return false;
    }
  }
  for (size_t shadow_index = 0; shadow_index < text_shadows.size();
       ++shadow_index) {
    if (text_shadows[shadow_index] != other.text_shadows[shadow_index]) {
      return false;
    }
  }

  return true;
}

}  // namespace txt
