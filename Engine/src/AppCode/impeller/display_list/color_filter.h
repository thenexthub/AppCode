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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_COLOR_FILTER_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_COLOR_FILTER_H_

#include "display_list/effects/dl_color_filter.h"
#include "impeller/entity/contents/filters/color_filter_contents.h"
#include "impeller/geometry/color.h"

namespace impeller {

/// A color matrix which inverts colors.
// clang-format off
static const constexpr ColorMatrix kColorInversion = {
  .array = {
    -1.0,    0,    0, 1.0, 0, //
       0, -1.0,    0, 1.0, 0, //
       0,    0, -1.0, 1.0, 0, //
     1.0,  1.0,  1.0, 1.0, 0  //
  }
};

std::shared_ptr<ColorFilterContents> WrapWithInvertColors(
    const std::shared_ptr<FilterInput>& input,
    ColorFilterContents::AbsorbOpacity absorb_opacity);

std::shared_ptr<ColorFilterContents> WrapWithGPUColorFilter(
    const appcode::DlColorFilter* filter,
    const std::shared_ptr<FilterInput>& input,
    ColorFilterContents::AbsorbOpacity absorb_opacity);

/// A procedure that filters a given unpremultiplied color to produce a new
/// unpremultiplied color.
using ColorFilterProc = std::function<Color(Color)>;

ColorFilterProc GetCPUColorFilterProc(const appcode::DlColorFilter* filter);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_COLOR_FILTER_H_
