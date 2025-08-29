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

#include "appcode/display_list/dl_paint.h"

#include "appcode/display_list/utils/dl_comparable.h"

namespace appcode {

DlPaint::DlPaint(DlColor color)
    : blend_mode_(static_cast<unsigned>(DlBlendMode::kDefaultMode)),
      draw_style_(static_cast<unsigned>(DlDrawStyle::kDefaultStyle)),
      stroke_cap_(static_cast<unsigned>(DlStrokeCap::kDefaultCap)),
      stroke_join_(static_cast<unsigned>(DlStrokeJoin::kDefaultJoin)),
      is_anti_alias_(false),
      is_invert_colors_(false),
      color_(color),
      stroke_width_(kDefaultWidth),
      stroke_miter_(kDefaultMiter) {}

bool DlPaint::operator==(DlPaint const& other) const {
  return blend_mode_ == other.blend_mode_ &&              //
         draw_style_ == other.draw_style_ &&              //
         stroke_cap_ == other.stroke_cap_ &&              //
         stroke_join_ == other.stroke_join_ &&            //
         is_anti_alias_ == other.is_anti_alias_ &&        //
         is_invert_colors_ == other.is_invert_colors_ &&  //
         color_ == other.color_ &&                        //
         stroke_width_ == other.stroke_width_ &&          //
         stroke_miter_ == other.stroke_miter_ &&          //
         Equals(color_source_, other.color_source_) &&    //
         Equals(color_filter_, other.color_filter_) &&    //
         Equals(image_filter_, other.image_filter_) &&    //
         Equals(mask_filter_, other.mask_filter_);
}

const DlPaint DlPaint::kDefault;

}  // namespace appcode
