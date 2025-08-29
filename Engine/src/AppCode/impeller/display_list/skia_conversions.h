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

#ifndef APPCODE_IMPELLER_DISPLAY_LIST_SKIA_CONVERSIONS_H_
#define APPCODE_IMPELLER_DISPLAY_LIST_SKIA_CONVERSIONS_H_

#include "appcode/display_list/dl_blend_mode.h"
#include "appcode/display_list/dl_color.h"
#include "appcode/display_list/dl_sampling_options.h"
#include "appcode/impeller/core/sampler_descriptor.h"
#include "appcode/impeller/geometry/color.h"

namespace impeller {
namespace skia_conversions {

Color ToColor(const appcode::DlColor& color);

impeller::SamplerDescriptor ToSamplerDescriptor(
    const appcode::DlImageSampling options);

}  // namespace skia_conversions
}  // namespace impeller

#endif  // APPCODE_IMPELLER_DISPLAY_LIST_SKIA_CONVERSIONS_H_
