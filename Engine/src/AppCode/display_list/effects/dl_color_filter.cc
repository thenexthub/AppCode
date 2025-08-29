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

#include "appcode/display_list/effects/dl_color_filter.h"

#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/effects/color_filters/dl_blend_color_filter.h"
#include "appcode/display_list/effects/color_filters/dl_linear_to_srgb_gamma_color_filter.h"
#include "appcode/display_list/effects/color_filters/dl_matrix_color_filter.h"
#include "appcode/display_list/effects/color_filters/dl_srgb_to_linear_gamma_color_filter.h"

namespace appcode {

std::shared_ptr<const DlColorFilter> DlColorFilter::MakeBlend(
    DlColor color,
    DlBlendMode mode) {
  // Delegate to a method private to DlBlendColorFilter due to private
  // constructor preventing |make_shared| from here.
  return DlBlendColorFilter::Make(color, mode);
}

std::shared_ptr<const DlColorFilter> DlColorFilter::MakeMatrix(
    const float matrix[20]) {
  // Delegate to a method private to DlBlendColorFilter due to private
  // constructor preventing |make_shared| from here.
  return DlMatrixColorFilter::Make(matrix);
}

std::shared_ptr<const DlColorFilter> DlColorFilter::MakeSrgbToLinearGamma() {
  return DlSrgbToLinearGammaColorFilter::kInstance;
}

std::shared_ptr<const DlColorFilter> DlColorFilter::MakeLinearToSrgbGamma() {
  return DlLinearToSrgbGammaColorFilter::kInstance;
}

}  // namespace appcode
