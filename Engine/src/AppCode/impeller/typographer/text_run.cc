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

#include "impeller/typographer/text_run.h"

namespace impeller {

TextRun::TextRun(const Font& font) : font_(font) {
  if (!font_.IsValid()) {
    return;
  }
  is_valid_ = true;
}

TextRun::TextRun(const Font& font, std::vector<GlyphPosition>& glyphs)
    : font_(font), glyphs_(std::move(glyphs)) {
  if (!font_.IsValid()) {
    return;
  }
  is_valid_ = true;
}

TextRun::~TextRun() = default;

bool TextRun::AddGlyph(Glyph glyph, Point position) {
  glyphs_.emplace_back(GlyphPosition{glyph, position});
  return true;
}

bool TextRun::IsValid() const {
  return is_valid_;
}

const std::vector<TextRun::GlyphPosition>& TextRun::GetGlyphPositions() const {
  return glyphs_;
}

size_t TextRun::GetGlyphCount() const {
  return glyphs_.size();
}

const Font& TextRun::GetFont() const {
  return font_;
}

}  // namespace impeller
