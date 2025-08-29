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

#include "impeller/typographer/backends/skia/text_frame_skia.h"

#include <vector>

#include "appcode/display_list/geometry/dl_path.h"
#include "appcode/fml/logging.h"
#include "fml/status.h"
#include "impeller/typographer/backends/skia/typeface_skia.h"
#include "impeller/typographer/font.h"
#include "impeller/typographer/glyph.h"
#include "third_party/skia/include/core/SkFont.h"
#include "third_party/skia/include/core/SkFontMetrics.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/modules/skparagraph/include/Paragraph.h"  // nogncheck
#include "third_party/skia/src/core/SkStrikeSpec.h"    // nogncheck
#include "third_party/skia/src/core/SkTextBlobPriv.h"  // nogncheck

namespace impeller {

static Font ToFont(const SkTextBlobRunIterator& run, AxisAlignment alignment) {
  auto& font = run.font();
  auto typeface = std::make_shared<TypefaceSkia>(font.refTypeface());

  SkFontMetrics sk_metrics;
  font.getMetrics(&sk_metrics);

  Font::Metrics metrics;
  metrics.point_size = font.getSize();
  metrics.embolden = font.isEmbolden();
  metrics.skewX = font.getSkewX();
  metrics.scaleX = font.getScaleX();

  return Font{std::move(typeface), metrics, alignment};
}

static Rect ToRect(const SkRect& rect) {
  return Rect::MakeLTRB(rect.fLeft, rect.fTop, rect.fRight, rect.fBottom);
}

std::shared_ptr<TextFrame> MakeTextFrameFromTextBlobSkia(
    const sk_sp<SkTextBlob>& blob) {
  bool has_color = false;
  std::vector<TextRun> runs;
  for (SkTextBlobRunIterator run(blob.get()); !run.done(); run.next()) {
    SkStrikeSpec strikeSpec = SkStrikeSpec::MakeWithNoDevice(run.font());
    SkBulkGlyphMetricsAndPaths paths{strikeSpec};
    SkSpan<const SkGlyph*> glyphs =
        paths.glyphs(SkSpan(run.glyphs(), run.glyphCount()));

    for (const auto& glyph : glyphs) {
      has_color |= glyph->isColor();
    }

    AxisAlignment alignment = AxisAlignment::kNone;
    if (run.font().isSubpixel() && run.font().isBaselineSnap() && !has_color) {
      alignment = AxisAlignment::kX;
    }

    switch (run.positioning()) {
      case SkTextBlobRunIterator::kFull_Positioning: {
        std::vector<TextRun::GlyphPosition> positions;
        positions.reserve(run.glyphCount());
        for (auto i = 0u; i < run.glyphCount(); i++) {
          // kFull_Positioning has two scalars per glyph.
          const SkPoint* glyph_points = run.points();
          const SkPoint* point = glyph_points + i;
          Glyph::Type type =
              glyphs[i]->isColor() ? Glyph::Type::kBitmap : Glyph::Type::kPath;
          positions.emplace_back(TextRun::GlyphPosition{
              Glyph{glyphs[i]->getGlyphID(), type}, Point{
                                                        point->x(),
                                                        point->y(),
                                                    }});
        }
        TextRun text_run(ToFont(run, alignment), positions);
        runs.emplace_back(text_run);
        break;
      }
      default:
        FML_DLOG(ERROR) << "Unimplemented.";
        continue;
    }
  }
  return std::make_shared<TextFrame>(
      runs, ToRect(blob->bounds()), has_color,
      [blob]() -> fml::StatusOr<appcode::DlPath> {
        SkPath path = skia::textlayout::Paragraph::GetPath(blob.get());
        if (path.isEmpty()) {
          return fml::Status(fml::StatusCode::kCancelled, "No path available");
        }
        SkPath transformed = path.makeTransform(
            SkMatrix::Translate(blob->bounds().left(), blob->bounds().top()));
        return appcode::DlPath(transformed);
      });
}

}  // namespace impeller
