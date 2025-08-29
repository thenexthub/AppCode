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

#ifndef APPCODE_IMPELLER_TYPOGRAPHER_LAZY_GLYPH_ATLAS_H_
#define APPCODE_IMPELLER_TYPOGRAPHER_LAZY_GLYPH_ATLAS_H_

#include "impeller/geometry/rational.h"
#include "impeller/renderer/context.h"
#include "impeller/typographer/glyph_atlas.h"
#include "impeller/typographer/text_frame.h"
#include "impeller/typographer/typographer_context.h"

namespace impeller {

class LazyGlyphAtlas {
 public:
  explicit LazyGlyphAtlas(
      std::shared_ptr<TypographerContext> typographer_context);

  ~LazyGlyphAtlas();

  void AddTextFrame(const std::shared_ptr<TextFrame>& frame,
                    Rational scale,
                    Point offset,
                    const Matrix& transform,
                    std::optional<GlyphProperties> properties);

  void ResetTextFrames();

  const std::shared_ptr<GlyphAtlas>& CreateOrGetGlyphAtlas(
      Context& context,
      HostBuffer& host_buffer,
      GlyphAtlas::Type type) const;

 private:
  std::shared_ptr<TypographerContext> typographer_context_;

  std::vector<std::shared_ptr<TextFrame>> alpha_text_frames_;
  std::vector<std::shared_ptr<TextFrame>> color_text_frames_;
  std::shared_ptr<GlyphAtlasContext> alpha_context_;
  std::shared_ptr<GlyphAtlasContext> color_context_;
  mutable std::shared_ptr<GlyphAtlas> alpha_atlas_;
  mutable std::shared_ptr<GlyphAtlas> color_atlas_;

  LazyGlyphAtlas(const LazyGlyphAtlas&) = delete;

  LazyGlyphAtlas& operator=(const LazyGlyphAtlas&) = delete;
};

}  // namespace impeller

#endif  // APPCODE_IMPELLER_TYPOGRAPHER_LAZY_GLYPH_ATLAS_H_
