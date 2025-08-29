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

#ifndef APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TYPOGRAPHER_CONTEXT_SKIA_H_
#define APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TYPOGRAPHER_CONTEXT_SKIA_H_

#include "impeller/typographer/typographer_context.h"

namespace impeller {

class TypographerContextSkia : public TypographerContext {
 public:
  static std::shared_ptr<TypographerContext> Make();

  TypographerContextSkia();

  ~TypographerContextSkia() override;

  // |TypographerContext|
  std::shared_ptr<GlyphAtlasContext> CreateGlyphAtlasContext(
      GlyphAtlas::Type type) const override;

  // |TypographerContext|
  std::shared_ptr<GlyphAtlas> CreateGlyphAtlas(
      Context& context,
      GlyphAtlas::Type type,
      HostBuffer& host_buffer,
      const std::shared_ptr<GlyphAtlasContext>& atlas_context,
      const std::vector<std::shared_ptr<TextFrame>>& text_frames)
      const override;

 private:
  static std::pair<std::vector<FontGlyphPair>, std::vector<Rect>>
  CollectNewGlyphs(const std::shared_ptr<GlyphAtlas>& atlas,
                   const std::vector<std::shared_ptr<TextFrame>>& text_frames);

  TypographerContextSkia(const TypographerContextSkia&) = delete;

  TypographerContextSkia& operator=(const TypographerContextSkia&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TYPOGRAPHER_BACKENDS_SKIA_TYPOGRAPHER_CONTEXT_SKIA_H_
