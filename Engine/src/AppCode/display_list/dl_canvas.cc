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

#include "appcode/display_list/dl_canvas.h"

#include "appcode/display_list/geometry/dl_geometry_conversions.h"
#include "appcode/third_party/skia/include/core/SkPoint3.h"
#include "appcode/third_party/skia/include/utils/SkShadowUtils.h"

namespace appcode {

DlRect DlCanvas::ComputeShadowBounds(const DlPath& path,
                                     float elevation,
                                     DlScalar dpr,
                                     const DlMatrix& ctm) {
  SkRect shadow_bounds(ToSkRect(path.GetBounds()));
  SkShadowUtils::GetLocalBounds(
      ToSkMatrix(ctm), path.GetSkPath(), SkPoint3::Make(0, 0, dpr * elevation),
      SkPoint3::Make(0, -1, 1), kShadowLightRadius / kShadowLightHeight,
      SkShadowFlags::kDirectionalLight_ShadowFlag, &shadow_bounds);
  return ToDlRect(shadow_bounds);
}

}  // namespace appcode
