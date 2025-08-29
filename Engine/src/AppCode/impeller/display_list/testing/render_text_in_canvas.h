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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_TESTING_RENDER_TEXT_IN_CANVAS_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_TESTING_RENDER_TEXT_IN_CANVAS_H_

#include <memory>

#include "appcode/display_list/dl_canvas.h"
#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/effects/dl_mask_filter.h"
#include "impeller/typographer/backends/skia/text_frame_skia.h"

namespace appcode {
namespace testing {

struct TextRenderOptions {
  bool stroke = false;
  SkScalar font_size = 50;
  DlColor color = DlColor::kYellow();
  std::shared_ptr<DlMaskFilter> mask_filter;
  bool is_subpixel = false;
};

bool RenderTextInCanvasSkia(DlCanvas* canvas,
                            const std::string& text,
                            const std::string_view& font_fixture,
                            DlPoint position,
                            const TextRenderOptions& options = {});

}  // namespace testing
}  // namespace appcode

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_TESTING_RENDER_TEXT_IN_CANVAS_H_
