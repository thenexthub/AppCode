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

#include "appcode/display_list/effects/color_filters/dl_linear_to_srgb_gamma_color_filter.h"

namespace appcode {

const std::shared_ptr<DlLinearToSrgbGammaColorFilter>
    DlLinearToSrgbGammaColorFilter::kInstance =
        std::make_shared<DlLinearToSrgbGammaColorFilter>();

}  // namespace appcode
