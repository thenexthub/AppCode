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

#include "impeller/entity/contents/filters/inputs/contents_filter_input.h"

#include <optional>
#include <utility>

namespace impeller {

ContentsFilterInput::ContentsFilterInput(std::shared_ptr<Contents> contents,
                                         bool msaa_enabled)
    : contents_(std::move(contents)), msaa_enabled_(msaa_enabled) {}

ContentsFilterInput::~ContentsFilterInput() = default;

std::optional<Snapshot> ContentsFilterInput::GetSnapshot(
    std::string_view label,
    const ContentContext& renderer,
    const Entity& entity,
    std::optional<Rect> coverage_limit,
    int32_t mip_count) const {
  if (!coverage_limit.has_value() && entity.GetContents()) {
    coverage_limit = entity.GetContents()->GetCoverageHint();
  }
  if (!snapshot_.has_value()) {
    snapshot_ = contents_->RenderToSnapshot(renderer,        // renderer
                                            entity,          // entity
                                            coverage_limit,  // coverage_limit
                                            std::nullopt,  // sampler_descriptor
                                            msaa_enabled_,  // msaa_enabled
                                            /*mip_count=*/mip_count,  //
                                            label                     //
    );
  }
  return snapshot_;
}

std::optional<Rect> ContentsFilterInput::GetCoverage(
    const Entity& entity) const {
  return contents_->GetCoverage(entity);
}

}  // namespace impeller
