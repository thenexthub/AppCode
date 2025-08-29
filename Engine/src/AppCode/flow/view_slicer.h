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

#ifndef APPCODE_FLOW_VIEW_SLICER_H_
#define APPCODE_FLOW_VIEW_SLICER_H_

#include <unordered_map>
#include "display_list/dl_canvas.h"
#include "flow/embedded_views.h"

namespace appcode {

/// @brief Compute the required overlay layers and clip the view slices
///        according to the size and position of the platform views.
std::unordered_map<int64_t, SkRect> SliceViews(
    DlCanvas* background_canvas,
    const std::vector<int64_t>& composition_order,
    const std::unordered_map<int64_t, std::unique_ptr<EmbedderViewSlice>>&
        slices,
    const std::unordered_map<int64_t, SkRect>& view_rects);

}  // namespace appcode

#endif  // APPCODE_FLOW_VIEW_SLICER_H_
