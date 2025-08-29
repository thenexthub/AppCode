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

#include "impeller/display_list/testing/render_text_in_canvas.h"

#include "appcode/testing/testing.h"
#include "txt/platform.h"

namespace appcode {
namespace testing {

bool RenderTextInCanvasSkia(DlCanvas* canvas,
                            const std::string& text,
                            const std::string_view& font_fixture,
                            DlPoint position,
                            const TextRenderOptions& options) {
  auto c_font_fixture = std::string(font_fixture);
  auto mapping = appcode::testing::OpenFixtureAsSkData(c_font_fixture.c_str());
  if (!mapping) {
    return false;
  }
  sk_sp<SkFontMgr> font_mgr = txt::GetDefaultFontManager();
  SkFont sk_font(font_mgr->makeFromData(mapping), options.font_size);
  if (options.is_subpixel) {
    sk_font.setSubpixel(true);
  }
  auto blob = SkTextBlob::MakeFromString(text.c_str(), sk_font);
  if (!blob) {
    return false;
  }

  auto frame = impeller::MakeTextFrameFromTextBlobSkia(blob);

  DlPaint text_paint;
  text_paint.setColor(options.color);
  text_paint.setMaskFilter(options.mask_filter);
  // text_paint.mask_blur_descriptor = options.mask_blur_descriptor;
  // text_paint.stroke_width = 1;
  // text_paint.style =
  //     options.stroke ? Paint::Style::kStroke : Paint::Style::kFill;
  canvas->DrawTextFrame(frame, position.x, position.y, text_paint);
  return true;
}
}  // namespace testing
}  // namespace appcode
