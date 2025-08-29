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

#ifndef APPCODE_IMPELLER_ENTITY_SAVE_LAYER_UTILS_H_
#define APPCODE_IMPELLER_ENTITY_SAVE_LAYER_UTILS_H_

#include <memory>
#include <optional>

#include "impeller/entity/contents/filters/filter_contents.h"
#include "impeller/geometry/rect.h"

namespace impeller {

/// @brief Compute the coverage of a subpass in the global coordinate space.
///
/// @param content_coverage the computed coverage of the contents of the save
///                         layer. This value may be empty if the save layer has
///                         no contents, or  Rect::Maximum if the contents are
///                         unbounded (like a destructive blend).
///
/// @param effect_transform The CTM of the subpass.
/// @param coverage_limit   The current clip coverage. This is used to bound the
///                         subpass size.
/// @param image_filter     A subpass image filter, or nullptr.
/// @param flood_output_coverage Whether the coverage should be flooded to clip
/// coverage regardless of input coverage. This should be set to true when the
/// restore Paint has a destructive blend mode.
/// @param flood_input_coverage  Whther the content coverage should be flooded.
/// This should be set to true if the paint has a backdrop filter or if there is
/// a transparent black effecting color filter.
///
/// The coverage computation expects `content_coverage` to be in the child
/// coordinate space. `effect_transform` is used to transform this back into the
/// global coordinate space. A return value of std::nullopt indicates that the
/// coverage is empty or otherwise does not intersect with the parent coverage
/// limit and should be discarded.
std::optional<Rect> ComputeSaveLayerCoverage(
    const Rect& content_coverage,
    const Matrix& effect_transform,
    const Rect& coverage_limit,
    const std::shared_ptr<FilterContents>& image_filter,
    bool flood_output_coverage = false,
    bool flood_input_coverage = false);

}  // namespace impeller

#endif  // APPCODE_IMPELLER_ENTITY_SAVE_LAYER_UTILS_H_
