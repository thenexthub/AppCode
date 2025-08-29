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

#include "impeller/entity/contents/filters/inputs/filter_contents_filter_input.h"

#include <utility>

#include "impeller/base/strings.h"
#include "impeller/entity/contents/filters/filter_contents.h"

namespace impeller {

FilterContentsFilterInput::FilterContentsFilterInput(
    std::shared_ptr<FilterContents> filter)
    : filter_(std::move(filter)) {}

FilterContentsFilterInput::~FilterContentsFilterInput() = default;

std::optional<Snapshot> FilterContentsFilterInput::GetSnapshot(
    std::string_view label,
    const ContentContext& renderer,
    const Entity& entity,
    std::optional<Rect> coverage_limit,
    int32_t mip_count) const {
  if (!snapshot_.has_value()) {
    snapshot_ = filter_->RenderToSnapshot(renderer,        // renderer
                                          entity,          // entity
                                          coverage_limit,  // coverage_limit
                                          std::nullopt,    // sampler_descriptor
                                          true,            // msaa_enabled
                                          /*mip_count=*/mip_count,  //
                                          label);                   // label
  }
  return snapshot_;
}

std::optional<Rect> FilterContentsFilterInput::GetCoverage(
    const Entity& entity) const {
  return filter_->GetCoverage(entity);
}

std::optional<Rect> FilterContentsFilterInput::GetSourceCoverage(
    const Matrix& effect_transform,
    const Rect& output_limit) const {
  return filter_->GetSourceCoverage(effect_transform, output_limit);
}

Matrix FilterContentsFilterInput::GetLocalTransform(
    const Entity& entity) const {
  return filter_->GetLocalTransform(entity.GetTransform());
}

Matrix FilterContentsFilterInput::GetTransform(const Entity& entity) const {
  return filter_->GetTransform(entity.GetTransform());
}

void FilterContentsFilterInput::SetEffectTransform(const Matrix& matrix) {
  filter_->SetEffectTransform(matrix);
}

void FilterContentsFilterInput::SetRenderingMode(
    Entity::RenderingMode rendering_mode) {
  filter_->SetRenderingMode(rendering_mode);
}

}  // namespace impeller
