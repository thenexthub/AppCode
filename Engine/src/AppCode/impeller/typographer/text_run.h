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

#ifndef APPCODE_IMPELLER_TYPOGRAPHER_TEXT_RUN_H_
#define APPCODE_IMPELLER_TYPOGRAPHER_TEXT_RUN_H_

#include <vector>

#include "impeller/geometry/point.h"
#include "impeller/typographer/font.h"
#include "impeller/typographer/glyph.h"

namespace impeller {

//------------------------------------------------------------------------------
/// @brief      Represents a collection of positioned glyphs from a specific
///             font.
///
class TextRun {
 public:
  struct GlyphPosition {
    Glyph glyph;
    Point position;

    GlyphPosition(Glyph p_glyph, Point p_position)
        : glyph(p_glyph), position(p_position) {}
  };

  //----------------------------------------------------------------------------
  /// @brief      Create an empty text run with the specified font.
  ///
  /// @param[in]  font  The font
  ///
  explicit TextRun(const Font& font);

  TextRun(const Font& font, std::vector<GlyphPosition>& glyphs);

  ~TextRun();

  bool IsValid() const;

  //----------------------------------------------------------------------------
  /// @brief      Add a glyph at the specified position to the run.
  ///
  /// @param[in]  glyph     The glyph
  /// @param[in]  position  The position
  ///
  /// @return     If the glyph could be added to the run.
  ///
  bool AddGlyph(Glyph glyph, Point position);

  //----------------------------------------------------------------------------
  /// @brief      Get the number of glyphs in the run.
  ///
  /// @return     The glyph count.
  ///
  size_t GetGlyphCount() const;

  //----------------------------------------------------------------------------
  /// @brief      Get a reference to all the glyph positions within the run.
  ///
  /// @return     The glyph positions.
  ///
  const std::vector<GlyphPosition>& GetGlyphPositions() const;

  //----------------------------------------------------------------------------
  /// @brief      Get the font for this run.
  ///
  /// @return     The font.
  ///
  const Font& GetFont() const;

 private:
  Font font_;
  std::vector<GlyphPosition> glyphs_;
  bool is_valid_ = false;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TYPOGRAPHER_TEXT_RUN_H_
